#ifndef _FAST_NET_CLIENT_H__
#define _FAST_NET_CLIENT_H__
#include <string>
#include <map>

#include <event2/util.h>
#include "ConnClient.h"
#include "IFastNetClient.h"
#include "FastNetDefine.h"
#include "IHandleMsg.h"

namespace fastnet
{
	struct CConnServerInfo
	{
		CConnServerInfo()
		{
			serverType = 0;
			pConnClient = nullptr;
			pingTime = time(NULL);
		}
		int32 serverType;
		CConnClient *pConnClient;
		time_t pingTime;
	};

	class CFastNetClient : public IFastNetClient,public IFastNetEventListen
	{
	public:
		CFastNetClient();
		virtual ~CFastNetClient();
		void InitNetClientCfg(IMessageFromServerHandler *pHandler);
		bool AddConnect(int32 serverType, ev_uint64_t connId, std::string strIp, ev_uint16_t port);
		void Start();
		void Stop();
		void KeepConnect();
		#ifdef WIN32
		static ev_uint32_t WINAPI ThreadKeepConnect(LPVOID lPVOID);
		static ev_uint32_t WINAPI ThreadHandler(LPVOID lPVOID);
		#else
		static void* ThreadKeepConnect(void* lPVOID);
		static void* ThreadHandler(void* lPVOID);
		#endif
		static void ReadCB(struct bufferevent *bev, void *ctx);
		static void EventCB(struct bufferevent *bev, short events, void *ctx);
		virtual void NotifyEvent(CFastNetEvent *pEvent) override;

		virtual bool MessageHandler() override;
		virtual bool SendMsg(ev_uint64_t serverId, uint16 msgtype, const void *buf, size_t size);
		void SetConnFlag();
		int64 FechServerId(int32 serverType);
		void EventDispatch();
		
	protected:
		struct event_base *m_base;
		std::map<ev_uint64_t, CConnServerInfo> m_mapConnClient;

		std::deque<CFastNetEvent*> m_queueEvent;
		fastbase::CMutex m_queueLock;
		fastbase::CMutex m_ClientLock;
		bool m_connFlag;

		IMessageFromServerHandler *m_pMsgHandler;
		std::map<int32,int32> m_mapFetchServerId;
		#ifdef WIN32
		#else
		pthread_t m_keepConnTid;
		pthread_t m_handlerTid;
		#endif  
	};
}



#endif/*_FAST_NET_CLIENT_H__*/
