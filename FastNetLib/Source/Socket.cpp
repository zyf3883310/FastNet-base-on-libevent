#include "Socket.h"
#include <event2/buffer.h>
#include <event2/event.h>
#include <assert.h>
#include "MsgPack.h"
#include "FastLock.h"
#include "FastNetDefine.h"
#include <iostream>

using fastnet::CSocket;
using fastnet::IMsgPack;
using fastnet::CMsgPack;
using fastnet::IFastNetEventListen;
using fastnet::CFastNetEvent;

using namespace fastbase;

CSocket::CSocket()
{
	m_pBuffEvent   = nullptr;
	m_pEventListen = nullptr;
	m_headSize     = 0;
}

CSocket::~CSocket()
{
	
}

int32 CSocket::SendMsg(uint16 msgtype, const void *buf, size_t size)
{
	//CSingleLock lock(&m_buffEventLock);

	if (nullptr == m_pBuffEvent)
		return -1;

	struct evbuffer *output = bufferevent_get_output(m_pBuffEvent);

	if (nullptr == output)
		return -1;

	ev_uint16_t headSize = (ev_uint16_t)size + sizeof(headSize) + sizeof(msgtype);


	evbuffer_add(output, &headSize, sizeof(headSize));
	evbuffer_add(output, &msgtype, sizeof(msgtype));
	
	evbuffer_add(output, buf, size);

	return 0;
}



bool CSocket::DisConnect()
{
	//CSingleLock lock(&m_buffEventLock);
	bufferevent_disable(m_pBuffEvent, EV_READ | EV_WRITE);
	//evutil_closesocket(GetSocketFd());

	//m_pBuffEvent = nullptr;
	m_headSize = 0;
	CFastNetEvent *pEvent = new CFastNetEvent();
	pEvent->eventType = EFNET_DISCONNECT;
	pEvent->pSocket = this;
	m_pEventListen->NotifyEvent(pEvent);
	return false;
}

void CSocket::AttachBufferEvent(struct bufferevent *pBufferEvent)
{
	CSingleLock lock(&m_buffEventLock);
	m_pBuffEvent = pBufferEvent;
}

void CSocket::OnRecvMsg()
{
	
	bool ret = true;
	//CSingleLock lock(&m_buffEventLock);
	
	struct evbuffer *input = bufferevent_get_input(m_pBuffEvent);
	/*
	if (input)
	{
		char recvbuf[256] = {0};
		size_t size_ = evbuffer_get_length(input);
		std::cout << "packet size_:" << size_  << std::endl;
		evbuffer_remove(input, recvbuf, size_);
		std::cout << "recv buf : " << recvbuf;
		struct evbuffer *output = bufferevent_get_output(m_pBuffEvent);
		evbuffer_add_printf(output, "%s", recvbuf);
	}*/
	
	if (input)
	{
		size_t sz = evbuffer_get_length(input);

		printf("size:%d\n",(int)sz);
		while (ret)
		{
			//如果是接受新包
			if (m_headSize == 0)
			{
				ret = ReadPackHead(input);
			}
			else
			{
				ret = ReadPackData(input);
			}
		}
		
	}


}


IMsgPack* CSocket::PickMsgPack()
{
	CSingleLock lock(&m_queueLock);
	if (m_msgPackList.size() <= 0)
		return nullptr;
	
	IMsgPack* pPack = m_msgPackList.front();
	//m_msgPackList.pop();
	m_msgPackList.pop_front();
	return pPack;
}


bool CSocket::ReadPackHead(struct evbuffer *pbuff)
{
	CSingleLock lock(&m_queueLock);
	size_t sz = evbuffer_get_length(pbuff);
	if (sz >= sizeof(m_headSize))
	{
		int retSize = evbuffer_remove(pbuff, &m_headSize, sizeof(m_headSize));
		if (retSize == -1 || m_headSize > MAX_MSG_PACK_BUFF_SIZE || retSize != sizeof(m_headSize))
		{
			DisConnect();
			perror("Error!");
			//assert(0);
			return false;
		}

		return true;
	}

	return false;
}

bool CSocket::ReadPackData(struct evbuffer *pbuff)
{
	size_t sz = evbuffer_get_length(pbuff);
	if (sz >= m_headSize - sizeof(m_headSize))
	{
		IMsgPack *pMsgPack = new CMsgPack();

		//取出类型
		uint16 msgtype = 0;
		int retMsgTypeSize = evbuffer_remove(pbuff, &msgtype, sizeof(msgtype));
		if (retMsgTypeSize != sizeof(msgtype))
		{
			delete pMsgPack;
			DisConnect();
			perror("Error!");
			assert(0);
			return false;
		}

		size_t bodySize = m_headSize - sizeof(m_headSize) - sizeof(msgtype);
		bool initRet = pMsgPack->Init((fastbase::uint16)bodySize, msgtype);
		if (!initRet)
		{
			delete pMsgPack;
			DisConnect();
			perror("Error!");
			assert(0);
			return false;
		}

		
		int retSize = evbuffer_remove(pbuff, pMsgPack->GetDataBuff(), bodySize);
		if (retSize == -1 || retSize != bodySize)
		{
			delete pMsgPack;
			DisConnect();
			perror("Error!");
			assert(0);
			return false;
		}
		
		m_headSize = 0;
		CFastNetEvent *pEvent = new CFastNetEvent();
		pEvent->eventType = EFNET_RECV_DATA;
		pEvent->pSocket = this;
		
		//std::cout << pMsgPack->GetDataBuff() << std::endl;
		
		PushPack(pMsgPack);
		NotifyEvent(pEvent);
		return true;
	}

	return false;
}


void CSocket::PushPack(IMsgPack *pPack)
{
	CSingleLock lock(&m_queueLock);
	m_msgPackList.push_back(pPack);
}


void CSocket::AttachFastNetEventListen(IFastNetEventListen *pEventListen)
{
	m_pEventListen = pEventListen;
}


void CSocket::NotifyEvent(CFastNetEvent *pEvent)
{
	m_pEventListen->NotifyEvent(pEvent);
}


void CSocket::OnDisConnect()
{
	FreeBuffevent();

	m_headSize = 0;
	m_msgPackList.clear();
}

void CSocket::OnConnect()
{
}

bool CSocket::IsConnect()
{
	return GetSocketFd() != -1;
}



evutil_socket_t CSocket::GetSocketFd()
{
	CSingleLock lock(&m_buffEventLock);
	if (nullptr == m_pBuffEvent)
		return -1;

	return bufferevent_getfd(m_pBuffEvent);
}

void CSocket::FreeBuffevent()
{
	CSingleLock lock(&m_buffEventLock);
	bufferevent_free(m_pBuffEvent);
	m_pBuffEvent = nullptr;
}
