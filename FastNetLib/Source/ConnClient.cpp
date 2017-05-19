#include "ConnClient.h"


using fastnet::CConnClient;

CConnClient::CConnClient()
{

}

CConnClient::~CConnClient()
{

}


void CConnClient::Init(ev_uint64_t connId, std::string strIp, ev_uint16_t port)
{
	m_connId = connId;
	m_strIp = strIp;
	m_port = port;

}

ev_uint64_t CConnClient::GetID()
{
	return m_connId;
}


void CConnClient::OnDisConnect()
{
	CSocket::OnDisConnect();
	//delete this;
}

void CConnClient::OnConnect()
{
	CSocket::OnConnect();
}

std::string CConnClient::GetIp()
{
	return m_strIp;
}

ev_uint16_t CConnClient::GetPort()
{
	return m_port;
}

