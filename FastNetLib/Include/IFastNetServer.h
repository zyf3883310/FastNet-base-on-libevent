#ifndef _I_FAST_SERVER_H__
#define _I_FAST_SERVER_H__


namespace fastnet
{
	class IFastNetServer
	{
	public:
		//virtual void AppendEvent(CFastNetEvent *pEvent) = 0;
		virtual bool MessageHandler() = 0;
	};

}

#endif/*_I_FAST_SERVER_H__*/
