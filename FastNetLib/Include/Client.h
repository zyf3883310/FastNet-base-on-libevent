#ifndef _CLIENT_H__
#define _CLIENT_H__
#include "Socket.h"
#include "FastUtil.h"

#ifdef WIN32
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#endif

namespace fastnet
{
	class CClient : public CSocket
	{
	public:
		CClient();
		void Init(fastbase::uint64 id);
		virtual ~CClient();
		ev_uint64_t GetID();
 		virtual void OnDisConnect() override;
 		virtual void OnConnect() override;
 		void SetTcpNoDelay(bool on);
	private:
		fastbase::uint64 m_ClientID;
	};
}



#endif/*_CLIENT_H__*/
