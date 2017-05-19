#include "FastUtil.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <time.h>
#include <assert.h>

using namespace fastbase;


void fastbase::sleep(uint32 milliseconds)
{
#ifdef WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds);
#endif
}


uint64 fastbase::GetLocalTimeStamp()
{
	return static_cast<uint64>(time(NULL));
}


int32 fastbase::StrToInt32(const char *value)
{
	return atoi(value);
}

int64 fastbase::StrToInt64(const char *value)
{
	assert(value);
	if (nullptr == value)
		return 0;
#ifdef WIN32
	return _atoi64(value);
#else
	return strtol(value, NULL, 10);
#endif 
	
	
}


