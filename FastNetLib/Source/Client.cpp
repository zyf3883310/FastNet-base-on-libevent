#include "Client.h"
#include "ClientListMgr.h"


using namespace fastbase;
using fastnet::CClient;

CClient::CClient()
{
	
}

void CClient::Init(uint64 id)
{
	m_ClientID = id;
}

CClient::~CClient()
{

}

uint64 CClient::GetID()
{
	return m_ClientID;
}

void CClient::OnDisConnect()
{
	CSocket::OnDisConnect();
	CClientListMgr::Instance()->ReleaseClient(this);
}

void CClient::OnConnect()
{
	CSocket::OnConnect();
}

void CClient::SetTcpNoDelay(bool on)
{
	evutil_socket_t sockfd_ = GetSocketFd();
	int optval = on ? 1 : 0;
#ifdef WIN32
	::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               (const char *)&optval, (sizeof optval));
#else
  	::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               &optval, (sizeof optval));
#endif 
}