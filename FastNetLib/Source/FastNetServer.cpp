#include "FastNetServer.h"
#include "ClientListMgr.h"
#include "FastUtil.h"
#include <string.h>
#include <stdio.h>
using namespace fastnet;
using namespace fastbase;


//using namespace fastnet;

CFastNetServer::CFastNetServer()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	(void)WSAStartup(wVersionRequested, &wsaData);
#endif

	m_pServerCfg  = nullptr;
	m_base = nullptr;
	m_listener = nullptr;
}

CFastNetServer::~CFastNetServer()
{
	if (m_base)
		event_base_free(m_base);

	//释放内存
}

void CFastNetServer::InitNetServerCfg(IFastServerCfg *pCfg, IFastLog *pLog, IMessageFromClientHandler *pHandler)
{
	m_pServerCfg = pCfg;
	m_pFastLog = pLog;
	m_pMessageHandler = pHandler;
	
#ifdef WIN32
	evthread_use_windows_threads();
	evthread_enable_lock_debuging();
	event_enable_debug_mode();
#endif
}


void CFastNetServer::AcceptConnCB(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx)
{
	//printf("%s\n", "accept success");  //OK
	/* We got a new connection! Set up a bufferevent for it. */
	struct event_base *base = evconnlistener_get_base(listener);
	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	evbuffer_enable_locking(bufferevent_get_input(bev), NULL);
	evbuffer_enable_locking(bufferevent_get_output(bev), NULL);

	CClient *pClient = CClientListMgr::Instance()->CreateClient((IFastNetEventListen *)ctx);
	pClient->SetTcpNoDelay(true);
	pClient->AttachBufferEvent(bev);
	bufferevent_setcb(bev, ReadCB, NULL, EventCB, pClient);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	CFastNetEvent *pEvent = new CFastNetEvent();
	pEvent->eventType = EFNET_CONNECT;
	pEvent->pSocket = pClient;
	((IFastNetEventListen *)ctx)->NotifyEvent(pEvent);
	
}

void CFastNetServer::ReadCB(struct bufferevent *bev, void *ctx)
{
	CClient *pClient = reinterpret_cast<CClient*>(ctx);
	pClient->OnRecvMsg();
}

void CFastNetServer::EventCB(struct bufferevent *bev, short events, void *ctx)
{
	if (events & BEV_EVENT_ERROR)
	{
		//m_pFastLog->LogError("Error from bufferevent,error no:%d", EVUTIL_SOCKET_ERROR());
		perror("Error from bufferevent");
	}

	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
		CClient *pClient = reinterpret_cast<CClient*>(ctx);
		
		CClientListMgr::Instance()->ReleaseClient(pClient);
		bufferevent_free(bev);
	}
}

void CFastNetServer::Start()
{
#ifdef WIN32
	//1.创建event_base
	struct event_config* cfg = event_config_new();
	event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
	//根据CPU实际数量配置libEvent的CPU数
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	event_config_set_num_cpus_hint(cfg, si.dwNumberOfProcessors);
	//event_config_set_num_cpus_hint(cfg, 1);

	m_base = event_base_new_with_config(cfg);
	event_config_free(cfg);
#else
	m_base = event_base_new();
#endif 
	//2.监听
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	/* Listen on 0.0.0.0 */
	sin.sin_addr.s_addr = htonl(0);
	/* Listen on the given port. */
	sin.sin_port = htons((ev_uint16_t)m_pServerCfg->GetBindPort());

	m_listener = evconnlistener_new_bind(m_base, AcceptConnCB, this, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr*)&sin, sizeof(sin));
	if (!m_listener) {
		perror("Couldn't create listener");
		return ;
	}
#ifdef WIN32
	typedef unsigned long(__stdcall *THREAD_FUNC)(void *);
	CreateThread(NULL, 0, (THREAD_FUNC)ThreadServer, m_base, 0, NULL);
#else
	pthread_create(&m_tid, NULL, ThreadServer, m_base);
	//event_base_dispatch(m_base);
#endif 
}

void CFastNetServer::Stop()
{
#ifdef WIN32
#else
	pthread_join(m_tid, NULL);
#endif
}

#ifdef WIN32
ev_uint32_t WINAPI CFastNetServer::ThreadServer(LPVOID lPVOID)
#else
void* CFastNetServer::ThreadServer(void *lPVOID)
#endif 
{
	struct event_base* base = reinterpret_cast<struct event_base *>(lPVOID);
	if (base == NULL)
	{
#ifdef WIN32
		return -1;
#else
		return (void *)-1;
#endif 
	}

	event_base_dispatch(base);
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return (void *)pthread_self();
#endif 
}

bool CFastNetServer::MessageHandler()
{
	if (m_queueEvent.size() <= 0)
		return false;

	CFastNetEvent* pEvent = nullptr;
	{
		CSingleLock lock(&m_queueLock);
		pEvent = m_queueEvent.front();
		m_queueEvent.pop();
	}


	IMsgPack* pPack = pEvent->pSocket->PickMsgPack();
	if (pPack)
	{
		m_pMessageHandler->Handle(pPack, (CClient*)pEvent->pSocket);
	}
	else
	{
		if (pEvent->eventType == EFNET_CONNECT)
		{
			pEvent->pSocket->OnConnect();
			m_pMessageHandler->OnConnected((CClient*)pEvent->pSocket);
		}
		else if (pEvent->eventType == EFNET_DISCONNECT)
		{
			m_pMessageHandler->OnDisConnected((CClient*)pEvent->pSocket);
			pEvent->pSocket->OnDisConnect();
		}
	}

	SAFE_DELETE(pEvent);
	SAFE_DELETE(pPack);
	
	return true;
}

void CFastNetServer::NotifyEvent(CFastNetEvent *pEvent)
{
	CSingleLock lock(&m_queueLock);
	m_queueEvent.push(pEvent);
}

IFastServerCfg *CFastNetServer::GetServerCfg()
{
	return m_pServerCfg;
}
