#ifndef _CONN_CLIENT_H__
#define _CONN_CLIENT_H__
#include <string>
#include "Socket.h"

namespace fastnet
{
	class CConnClient : public CSocket
	{
	public:
		CConnClient();
		~CConnClient();
		void Init(ev_uint64_t connId, std::string strIp, ev_uint16_t port);
		ev_uint64_t GetID();
		ev_uint32_t GetServerId();
		virtual void OnDisConnect() override;
		virtual void OnConnect() override;

		std::string GetIp();
		ev_uint16_t GetPort();

		
	private:
		ev_uint64_t m_connId;
		std::string m_strIp;
		ev_uint16_t m_port;
	};

}

#endif/*_CONN_CLIENT_H__*/
