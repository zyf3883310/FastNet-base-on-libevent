#ifndef _ISOCKET_H__
#define _ISOCKET_H__
#include <event2/bufferevent.h>
#include "IMsgPack.h"
#include "FastNetDefine.h"
#include "FastUtil.h"


namespace fastnet
{
	
	

	class ISocket
	{
	public:
		virtual void AttachFastNetEventListen(IFastNetEventListen *pEventListen) = 0;
		virtual void AttachBufferEvent(struct bufferevent *pBufferEvent) = 0;

		virtual fastbase::int32  SendMsg(fastbase::uint16 msgtype, const void *buf, size_t size) = 0;
		virtual bool DisConnect() = 0;
		virtual void OnRecvMsg() = 0;
		virtual IMsgPack* PickMsgPack() = 0;		
		
		virtual void NotifyEvent(CFastNetEvent *pEvent) =0;
		virtual void OnDisConnect() = 0;
		virtual void OnConnect() = 0;
		virtual bool IsConnect() = 0;

	protected:
		virtual evutil_socket_t GetSocketFd() = 0;
	};
}


#endif/*_ISOCKET_H__*/
