#include "MsgPack.h"
#include <assert.h>

using fastnet::CMsgPack;

bool CMsgPack::Init(ev_uint16_t dataSize, fastbase::uint16 msgType)
{
	if (dataSize > MAX_MSG_PACK_BUFF_SIZE)
	{
		return false;
	}

	m_buffsize = dataSize;
	m_msgType = msgType;

	return true;
}

char *CMsgPack::GetDataBuff()
{
	return m_buffer;
}

fastbase::int32 CMsgPack::GetDataBuffSize()
{
	assert(m_buffsize <= MAX_MSG_PACK_BUFF_SIZE);

	return m_buffsize;
}

fastbase::int32 CMsgPack::GetBuffMaxSize()
{
	return MAX_MSG_PACK_BUFF_SIZE;
}

fastbase::uint16 CMsgPack::GetMsgPackType()
{
	return m_msgType;
}