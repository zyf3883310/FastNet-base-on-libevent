#ifndef _FAST_UTIL_H__
#define _FAST_UTIL_H__
#include <event2/util.h>
#include <time.h>


namespace fastbase
{
	typedef ev_uint8_t uint8;
	typedef ev_int8_t  int8;

	typedef  ev_uint16_t uint16;
	typedef ev_int16_t int16;
	
	typedef ev_uint32_t uint32;
	typedef ev_int32_t int32;

	typedef ev_uint64_t uint64;
	typedef ev_int64_t int64;

	void sleep(uint32 milliseconds);
	//void Sleep(ev_uint32_t )

	int32 StrToInt32(const char *value);
	int64 StrToInt64(const char *value);


	uint64 GetLocalTimeStamp();

#define SAFE_DELETE(ptr) { if(ptr){ try{ delete ptr; }catch(...){ printf("CATCH: *** SAFE_DELETE() crash! *** at %s, %d", __FILE__, __LINE__);} }}

#define SAFE_ARRAY_DELETE(ptr) { if(ptr){ try{ delete []ptr; }catch(...){ printf("CATCH: *** SAFE_DELETE() crash! *** at %s, %d", __FILE__, __LINE__);} }}

#define  ASSERT(condition) assert(condition)


	//////////////////////////////////////////////////////////////////////
	// 超时类，以“秒”为单位
	//////////////////////////////////////////////////////////////////////
	class CTimer
	{
	public:
		CTimer(int nIntervalSecs = 0) { m_nInterval = nIntervalSecs; m_tUpdateTime = 0; }		// =0 : 用于数组初始化
		// virtual ~CTimer() {}
		CTimer& operator = (const CTimer& rhs)
		{
			m_nInterval = rhs.m_nInterval;
			m_tUpdateTime = rhs.m_tUpdateTime;
			return *this;
		}

	public:
		// 设置时间标签
		void	Update(void)					{ m_tUpdateTime = time(NULL); }

		// 用于定时触发的连续时钟。每次触发时为true。
		bool	ToNextTime(void)					{ if (IsExpire()) return Update(), true; else return false; }

		// 初始化时间片。(但不启动
		void	SetInterval(int nSecs)				{ m_nInterval = nSecs; }

		// 开始启动时钟(同SetInterval)
		void	Startup(int nSecs)				{ m_nInterval = nSecs; Update(); }

		// 时钟是否触发(只触发一次)，与Startup配合使用，可实现只触发一次的时钟。
		bool	TimeOver(void);//{ if(IsActive() && IsExpire()) return Clear(),true; return false; }
		void	Clear(void)					{ m_tUpdateTime = m_nInterval = 0; }
		bool	IsActive(void)					{ return m_tUpdateTime != 0; }
		void	IncInterval(int nSecs, int nLimit)	{ m_nInterval = __min(m_nInterval + nSecs, nLimit); }
		void	DecInterval(int nSecs)				{ m_nInterval = __max(m_nInterval - nSecs, 0); }

		// 检查是否超时
		bool	IsExpire(void)
		{
			time_t daa = time(NULL);
			return daa >= m_tUpdateTime + m_nInterval;
		}
		// 检查是否超时了指定时间片。用于同一时间标签检查多个不同超时事件。(可实现一个对象控制多个时钟)
		bool	IsExpire(int nSecs)				{ return time(NULL) >= m_tUpdateTime + nSecs; }
		//检查当前时间是否超过了以指定时间开始和指定时间间隔的
		bool    IsExpire(time_t Start, int Interval){ return time(NULL) >= Start + Interval; }

		// 用于变长的超时事件。
		bool	ToNextTime(int nSecs)				{ if (IsExpire(nSecs)) return Update(), true; else return false; }

		// 时钟会积累，但积累值不超过间隔值。其它同ToNextTime(...)
		bool	ToNextTick(int nSecs);
		//	void	AppendInterval	(int nSecs)			{ if(ToNextTime()) m_nInterval=nSecs; else m_nInterval+=nSecs; }	// ToNextTime(): 保证至少有nSecs秒

	public: // get
		time_t		GetRemain()							{ return m_tUpdateTime ? __min(__max(m_nInterval - (time(NULL) - (int)m_tUpdateTime), 0), m_nInterval) : 0; }
		time_t		GetPast()							{ return m_tUpdateTime ? __min(__max((time(NULL) - (int)m_tUpdateTime), 0), m_nInterval) : 0; }
		//int		GetRemainTime()							{ return m_tUpdateTime ? __min(__max(m_nInterval - (int32)(time(NULL) - (int)m_tUpdateTime), 0), m_nInterval) : 0; }
	protected:
		int		m_nInterval;
		time_t	m_tUpdateTime;
	};

	
	
}

#endif/*_FAST_UTIL_H__*/
