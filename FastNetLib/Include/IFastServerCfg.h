#ifndef _I_FAST_SERVER_CFG_H__
#define _I_FAST_SERVER_CFG_H__
#include <string>
#include <event2/util.h>
#include "FastUtil.h"


namespace fastnet
{
	class IFastServerCfg
	{
	public:
		virtual  fastbase::int32 GetServerType() = 0;
		virtual std::string GetBindIp() = 0;
		virtual fastbase::uint16 GetBindPort() = 0;
		virtual fastbase::int32 GetServerId() = 0;
		virtual std::string GetAuthCode() = 0;

		virtual std::string GetUserServerIp(){ return ""; }
		virtual fastbase::uint16 GetUserServerPort(){ return 0; }

		virtual std::string GetDBMgrServerIp(){	return "";}
		virtual fastbase::uint16 GetDBMgrServerPort(){ return 0; }
	};
}

#endif/*_I_FAST_SERVER_CFG_H__*/
