#ifndef _I_HANDLE_MSG_H__
#define _I_HANDLE_MSG_H__
#include "IMsgPack.h"
#include "Client.h"
#include "ConnClient.h"

namespace fastnet
{
	class IMessageFromClientHandler
	{
	public:
		virtual bool OnConnected(fastnet::CClient *pClient) = 0;
		virtual bool OnDisConnected(fastnet::CClient *pClient) = 0;
		virtual bool Handle(fastnet::IMsgPack *pPack, fastnet::CClient *pClient) = 0;
	};

	class IMessageFromServerHandler
	{
	public:
		virtual bool OnConnected(fastnet::CConnClient *pClient) = 0;
		virtual bool OnDisConnected(fastnet::CConnClient *pClient) = 0;
		virtual bool Handle(fastnet::IMsgPack *pPack, fastnet::CConnClient *pClient) = 0;
	};

}

#endif/*_I_HANDLE_MSG_H__*/
