#ifndef _I_FAST_LOG_H__
#define _I_FAST_LOG_H__

namespace fastnet
{
	class IFastLog
	{
	public:
		virtual void LogTrace(const char* pFormat, ...) = 0;
		virtual void LogDebug(const char* pFormat, ...) = 0;
		virtual void LogInfo(const char* pFormat, ...) = 0;
		virtual void LogWarn(const char* pFormat, ...) = 0;
		virtual void LogError(const char* pFormat, ...) = 0;
		virtual void LogFatal(const char* pFormat, ...) = 0;
	};
}


#endif/*_I_FAST_LOG_H__*/

