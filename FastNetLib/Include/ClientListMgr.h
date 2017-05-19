#ifndef _CLIENT_LIST_MGR_H__
#define _CLIENT_LIST_MGR_H__
#include <map>
#include <event2/util.h>
#include "FastNetDefine.h"
#include "Client.h"

namespace fastnet
{
	class CClientListMgr
	{
	public:
		static CClientListMgr *Instance();
		CClient *CreateClient(IFastNetEventListen *pEventListen);
		bool ReleaseClient(CClient *pClient);

		CClient *GetClient(ev_uint64_t clientId);
	private:
		std::map<ev_uint64_t, CClient*> m_mapClient;
		static CClientListMgr* m_staticClientListMgr;
	};
}

#endif/*_CLIENT_LIST_MGR_H__*/
