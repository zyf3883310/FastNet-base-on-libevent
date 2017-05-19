#ifndef _I_FAST_NET_CLIENT_H__
#define _I_FAST_NET_CLIENT_H__

namespace fastnet
{
	class IFastNetClient
	{
	public:
		//virtual void AppendEvent(CFastNetEvent *pEvent) = 0;
		virtual bool MessageHandler() = 0;
	};

}

#endif/*_I_FAST_NET_CLIENT_H__*/
