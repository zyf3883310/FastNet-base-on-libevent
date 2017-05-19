#include "ClientListMgr.h"

using fastnet::CClient;
using fastnet::CClientListMgr;
using fastnet::IFastNetEventListen;

CClientListMgr* CClientListMgr::m_staticClientListMgr = nullptr;

CClient *CClientListMgr::CreateClient(IFastNetEventListen *pEventListen)
{
	CClient *pClient = new CClient();

	pClient->Init((ev_uint64_t)pClient);
	pClient->AttachFastNetEventListen(pEventListen);
	m_mapClient[pClient->GetID()] = pClient;

	return pClient;
}

CClientListMgr *CClientListMgr::Instance()
{
	if (m_staticClientListMgr == nullptr)
	{
		m_staticClientListMgr = new CClientListMgr();
	}

	return m_staticClientListMgr;
}


bool CClientListMgr::ReleaseClient(CClient *pClient)
{
	ev_uint64_t id = ((ev_uint64_t)pClient);
	std::map<ev_uint64_t, CClient*>::iterator it = m_mapClient.find(id);
	if (it == m_mapClient.end())
		return false;

	delete it->second;
	m_mapClient.erase(it);

	return true;
}


CClient *CClientListMgr::GetClient(ev_uint64_t clientId)
{
	std::map<ev_uint64_t, CClient*>::iterator it = m_mapClient.find(clientId);
	if (it == m_mapClient.end())
		return NULL;

	return it->second;
}
