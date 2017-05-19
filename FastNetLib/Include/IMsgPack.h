#ifndef _I_MSG_PACK__
#define _I_MSG_PACK__
#include <event2/util.h>
#include "FastUtil.h"

#define MAX_MSG_PACK_BUFF_SIZE 4096
namespace fastnet
{
	class IMsgPack
	{
	public:
		virtual bool Init(ev_uint16_t dataSize, fastbase::uint16 msgType) = 0;
		virtual char *GetDataBuff() = 0;
		virtual fastbase::int32 GetDataBuffSize() = 0;
		virtual fastbase::int32 GetBuffMaxSize() = 0;
		virtual fastbase::uint16 GetMsgPackType() = 0;
	};
}

#endif/*_I_MSG_PACK__*/
