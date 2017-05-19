#ifndef _MSG_PACK_H__
#define _MSG_PACK_H__

#include <event2/util.h>
#include "IMsgPack.h"
#include "FastUtil.h"

namespace fastnet
{
	class CMsgPack : public IMsgPack
	{
	public:
		virtual bool Init(ev_uint16_t dataSize, fastbase::uint16 msgType);
		virtual char *GetDataBuff() override;
		virtual fastbase::int32 GetDataBuffSize() override;
		virtual fastbase::int32 GetBuffMaxSize() override;
		virtual fastbase::uint16 GetMsgPackType() override;
	private:
		char m_buffer[MAX_MSG_PACK_BUFF_SIZE];
		ev_uint16_t m_buffsize;
		fastbase::uint16 m_msgType;
	};

}

#endif/*_MSG_PACK_H__*/
