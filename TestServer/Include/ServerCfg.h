#ifndef _SERVER_CFG_H_
#define _SERVER_CFG_H_
#include "IFastServerCfg.h"
#include "FastUtil.h"
#include <string>


class ServerCfg : public fastnet::IFastServerCfg
{
public:
	fastbase::uint16 GetBindPort();	
	fastbase::int32 GetServerType()
	{
		return 0;
	}
	std::string GetBindIp()
	{
		return "192.168.136.133";
	}
	fastbase::int32 GetServerId()
	{
		return 0;
	}
	std::string GetAuthCode()
	{
		return "0";
	}
};



#endif //_SERVER_CFG_H_

