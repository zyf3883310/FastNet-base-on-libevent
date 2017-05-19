#ifndef _FAST_LOCK_H__
#define _FAST_LOCK_H__

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
#include "FastUtil.h"

namespace fastbase
{
	class ILockObj
	{
	public:
		virtual void Lock(void) = 0;
		virtual void Unlock(void) = 0;
	};


	class CMutex : public ILockObj
	{
	public:
		CMutex(uint32 = 2000)
		{
			//InitializeCriticalSectionAndSpinCount( &m_CritSec , spincount);
			#ifdef WIN32
			InitializeCriticalSection(&m_CritSec);
			#else
			pthread_mutex_init(&m_CritSec, NULL);
			#endif
		};

		virtual ~CMutex()
		{
			#ifdef WIN32
			DeleteCriticalSection(&m_CritSec);
			#else
			pthread_mutex_destroy(&m_CritSec);
			#endif
		};
		void Lock(void)
		{
			#ifdef WIN32
			EnterCriticalSection(&m_CritSec);
			#else
			pthread_mutex_lock(&m_CritSec);
			#endif
		};

		void Unlock(void)
		{
			#ifdef WIN32
			LeaveCriticalSection(&m_CritSec);
			#else
			pthread_mutex_unlock(&m_CritSec);
			#endif
		};

	private:
		#ifdef WIN32
		CRITICAL_SECTION    m_CritSec;
		#else
		pthread_mutex_t m_CritSec;
		#endif 
	};

	class CSingleLock
	{
	public:
		CSingleLock(ILockObj* pLock)
			: m_pLock(pLock)
		{
			if (m_pLock)
				m_pLock->Lock();
		}

		~CSingleLock(void)
		{

			if (m_pLock)
			{
				//printf("Unlock Start...\n");
				m_pLock->Unlock();
				//printf("Unlock Over...\n");

			}

		}

	private:
		ILockObj* m_pLock;
	};

}
#endif/*_FAST_LOCK_H__*/
