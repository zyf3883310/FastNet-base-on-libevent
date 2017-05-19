#ifndef _SOCKET_H__
#define _SOCKET_H__
#include "ISocket.h"
#include "FastLock.h"
#include "FastUtil.h"
#include <deque>

using namespace fastbase;
namespace fastnet
{
	class CSocket : public ISocket
	{
	public:
		CSocket();
		virtual ~CSocket();
		virtual int32 SendMsg(uint16 msgtype,const void *buf, size_t size) override;
		
		virtual bool DisConnect() override; //主动关闭连接
		

		virtual void OnRecvMsg() override;
		virtual void AttachBufferEvent(struct bufferevent *pBufferEvent) override;

		virtual IMsgPack* PickMsgPack() override;
		virtual void AttachFastNetEventListen(IFastNetEventListen *pEventListen) override;

		virtual void OnDisConnect() override;
		virtual void OnConnect() override;
		virtual bool IsConnect() override;

		void NotifyEvent(CFastNetEvent *pEvent);
	protected:
		
		virtual evutil_socket_t GetSocketFd() override;
		
	private:
		bool ReadPackHead(struct evbuffer *pbuff);
		bool ReadPackData(struct evbuffer *pbuff);
		void PushPack(IMsgPack *pPack);
		void FreeBuffevent();
	protected:
		bufferevent *m_pBuffEvent;
		fastbase::CMutex m_buffEventLock;
		
		uint16 m_headSize;//头大小
		std::deque<IMsgPack*> m_msgPackList;
		fastbase::CMutex m_queueLock;
		IFastNetEventListen *m_pEventListen;
	};
}

#endif/*_SOCKET_H__*/
