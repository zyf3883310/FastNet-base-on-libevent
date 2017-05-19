#ifndef _FAST_NET_SERVER_H__
#define _FAST_NET_SERVER_H__
#include "IFastServerCfg.h"
#include <event2/util.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event-config.h>
#include "IFastNetServer.h"
#include "FastNetDefine.h"
#include "IFastLog.h"
#include "IHandleMsg.h"
#include "IFastServerCfg.h"

#ifdef WIN32
#include "event2/thread.h"
#else
#include <pthread.h>
#endif 

#include <queue>

namespace fastnet
{
	class CFastNetServer : public IFastNetEventListen, public IFastNetServer
	{
	public:
		CFastNetServer();
		virtual ~CFastNetServer();
		void InitNetServerCfg(IFastServerCfg *pCfg, IFastLog *pLog, IMessageFromClientHandler *pHandler);
		void Start();
		void Stop();
		virtual bool MessageHandler() override;
		
		#ifdef WIN32
		static ev_uint32_t WINAPI ThreadServer(LPVOID lPVOID);
		#else
		static void* ThreadServer(void *lPVOID);
		#endif 
		static void AcceptConnCB(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx);
		static void ReadCB(struct bufferevent *bev, void *ctx);
		static void EventCB(struct bufferevent *bev, short events, void *ctx);

		virtual void NotifyEvent(CFastNetEvent *pEvent) override;
		IFastServerCfg *GetServerCfg();
	private:
		IFastServerCfg *m_pServerCfg;
		IFastLog *m_pFastLog;
		IMessageFromClientHandler *m_pMessageHandler;
		struct event_base *m_base;
		struct evconnlistener *m_listener;
		std::queue<CFastNetEvent*> m_queueEvent;
		fastbase::CMutex m_queueLock;
		#ifndef WIN32
		pthread_t m_tid;
		#endif 
	};
}

#endif/*_FAST_NET_SERVER_H__*/
