#include "FastNetClient.h"
#include <event2/event.h>
#include "FastLock.h"
#include "FastNetDefine.h"
#include <event2/thread.h>
#include "FastUtil.h"
//

#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <arpa/inet.h>
#include <string.h>
#endif

#include "util-internal.h"


using fastnet::CFastNetClient;
using fastbase::CSingleLock;
using fastnet::CFastNetEvent;
using fastnet::IMessageFromServerHandler;

CFastNetClient::CFastNetClient()
{/*
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	(void)WSAStartup(wVersionRequested, &wsaData);
#endif

	evthread_use_windows_threads();
	evthread_enable_lock_debuging();
	event_enable_debug_mode();*/
	m_base = nullptr;
	m_connFlag =false;
}

CFastNetClient::~CFastNetClient()
{
	if (m_base)
		event_base_free(m_base);
}

void CFastNetClient::InitNetClientCfg(IMessageFromServerHandler *pHandler)
{
	m_pMsgHandler = pHandler;
}

bool CFastNetClient::AddConnect(int32 serverType,ev_uint64_t connId , std::string strIp, ev_uint16_t port)
{
	CSingleLock lock(&m_ClientLock);

	if (m_mapConnClient.find(connId) != m_mapConnClient.end())
	{
		return false;
	}
	
	CConnClient *pClient = new CConnClient();
	pClient->Init(connId, strIp, port);
	pClient->AttachFastNetEventListen(this);
	CConnServerInfo &serverInfo = m_mapConnClient[connId];
	serverInfo.pConnClient= pClient;
	serverInfo.pingTime = time(NULL);
	serverInfo.serverType = serverType;
	return true;
}

void CFastNetClient::Start()
{
	#ifdef WIN32
	//1.创建event_base
	struct event_config* cfg = event_config_new();
	event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
	//根据CPU实际数量配置libEvent的CPU数
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	//event_config_set_num_cpus_hint(cfg, si.dwNumberOfProcessors);
	event_config_set_num_cpus_hint(cfg, 1);

	m_base = event_base_new_with_config(cfg);
	event_config_free(cfg);	
	typedef unsigned long(__stdcall *THREAD_FUNC)(void *);
	CreateThread(NULL, 0, (THREAD_FUNC)ThreadKeepConnect, this, 0, NULL);
	
	typedef unsigned long(__stdcall *THREAD_FUNC)(void *);
	CreateThread(NULL, 0, (THREAD_FUNC)ThreadHandler, this, 0, NULL);
	#else
	m_base = event_base_new();
	pthread_create(&m_keepConnTid, NULL, ThreadKeepConnect, this);
	pthread_create(&m_handlerTid, NULL, ThreadHandler, this);
	#endif 
}


void CFastNetClient::Stop()
{

}

#ifdef WIN32
ev_uint32_t WINAPI CFastNetClient::ThreadKeepConnect(LPVOID lPVOID)
#else
void* CFastNetClient::ThreadKeepConnect(void* lPVOID)
#endif 
{
	CFastNetClient *pFastNetClient = (CFastNetClient *)lPVOID;
	
	while (1)
	{
		fastbase::sleep(1000);
		pFastNetClient->KeepConnect();
		pFastNetClient->SetConnFlag();
	}
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return (void*)pthread_self();
#endif
}

#ifdef WIN32
ev_uint32_t WINAPI CFastNetClient::ThreadHandler(LPVOID lPVOID)
#else
void* CFastNetClient::ThreadHandler(void* lPVOID)
#endif
{
	CFastNetClient *pFastNetClient = (CFastNetClient *)lPVOID;
	pFastNetClient->EventDispatch();
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return (void*)pthread_self();
#endif
}



void CFastNetClient::ReadCB(struct bufferevent *bev, void *ctx)
{
	CConnClient *pClient = reinterpret_cast<CConnClient*>(ctx);
	pClient->OnRecvMsg();
}

void CFastNetClient::EventCB(struct bufferevent *bev, short events, void *ctx)
{
	if (events & BEV_EVENT_ERROR)
	{
		perror("Error from bufferevent");

	}
	else if (events & BEV_EVENT_CONNECTED)
	{

	}
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
		CConnClient *pClient = reinterpret_cast<CConnClient*>(ctx);
		CFastNetEvent *pEvent = new CFastNetEvent();
		pEvent->eventType = EFNET_DISCONNECT;
		pEvent->pSocket = pClient;
		//pClient->FreeBuffevent();
		pClient->NotifyEvent(pEvent);
	}
}

void CFastNetClient::KeepConnect()
{
	CSingleLock lock(&m_ClientLock);

	for (auto it = m_mapConnClient.begin(); it != m_mapConnClient.end(); ++it)
	{
		if(it->second.pConnClient->IsConnect())
			continue;

		printf("Connect Server Ip:%s Port:%u\n", it->second.pConnClient->GetIp().c_str(), (uint32)it->second.pConnClient->GetPort());
		//1.重置地址
		struct sockaddr_in sin;
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(it->second.pConnClient->GetPort());
		if (inet_pton(AF_INET, it->second.pConnClient->GetIp().c_str(), &sin.sin_addr) <= 0)
		{
			printf("inet_pton error,ip:%s,port:%d\n", it->second.pConnClient->GetIp().c_str(), it->second.pConnClient->GetPort());
			continue;
		}

		//2.连接
		evutil_socket_t fd = socket(PF_INET, SOCK_STREAM, 0);
		int rets = evutil_socket_connect(&fd, (struct sockaddr *)&sin, sizeof(sin));
		if (rets != 1) //失败
		{
			evutil_closesocket(fd);
			continue;
		}
		else //成功
		{
			evutil_make_socket_nonblocking(fd);
			struct bufferevent *bev;
			bev = bufferevent_socket_new(m_base, fd, BEV_OPT_CLOSE_ON_FREE);
			bufferevent_setcb(bev, ReadCB, NULL, EventCB, it->second.pConnClient);
			bufferevent_enable(bev, EV_READ | EV_WRITE);

			it->second.pConnClient->AttachBufferEvent(bev);


			CFastNetEvent *pEvent = new CFastNetEvent();
			pEvent->eventType = EFNET_CONNECT;
			pEvent->pSocket = it->second.pConnClient;
			it->second.pConnClient->NotifyEvent(pEvent);
		}
	}
}

void CFastNetClient::NotifyEvent(CFastNetEvent *pEvent)
{
	CSingleLock lock(&m_queueLock);
	m_queueEvent.push_back(pEvent);
}

bool CFastNetClient::MessageHandler()
{
	CFastNetEvent* pEvent = nullptr;
	{
		CSingleLock lock(&m_queueLock);
		if (m_queueEvent.size() <= 0)
			return false;
		pEvent = m_queueEvent.front();
		m_queueEvent.pop_front();
	}
	

	if (pEvent->eventType == EFNET_CONNECT)
	{
		pEvent->pSocket->OnConnect();
		m_pMsgHandler->OnConnected((CConnClient*)pEvent->pSocket);
	}
	else if (pEvent->eventType == EFNET_DISCONNECT)
	{
		m_pMsgHandler->OnDisConnected((CConnClient*)pEvent->pSocket);
		pEvent->pSocket->OnDisConnect();
	}
	else
	{
		IMsgPack* pPack = pEvent->pSocket->PickMsgPack();
		if (pPack)
		{
			m_pMsgHandler->Handle(pPack, (CConnClient*)pEvent->pSocket);
		}
		SAFE_DELETE(pPack);
	}

	SAFE_DELETE(pEvent);
	
	
	return true;
}


bool CFastNetClient::SendMsg(ev_uint64_t serverId, uint16 msgtype, const void *buf, size_t size)
{
	std::map<ev_uint64_t, CConnServerInfo>::iterator it = m_mapConnClient.find(serverId);
	if (it == m_mapConnClient.end())
		return false;

	it->second.pConnClient->SendMsg(msgtype,buf, size);
	return true;
}

void CFastNetClient::SetConnFlag()
{
	m_connFlag = true;
}

void CFastNetClient::EventDispatch()
{
	while (m_connFlag == false);
	event_base_dispatch(m_base);
}

int64 CFastNetClient::FechServerId(int32 serverType)
{
	int64 firstClientId = -1;

	for (auto it = m_mapConnClient.begin(); it != m_mapConnClient.end(); ++it)
	{
		if (it->second.serverType == serverType)
		{
			//保存第一个firstClientId
			if (firstClientId == -1)
			{
				firstClientId = it->first;
			}

			auto itFind = m_mapFetchServerId.find(serverType);
			if (itFind == m_mapFetchServerId.end() ||  it->first > itFind->second)
			{
				m_mapFetchServerId[serverType] = it->first;
			}

			return it->first;
		}
	}

	if (firstClientId != -1)
	{
		m_mapFetchServerId[serverType] = firstClientId;
	}

	return firstClientId;
}
