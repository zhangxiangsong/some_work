
#include "lock.h"

//////////////////////////////////////////////////////////////////////////

LockImpl::LockImpl() 
{
#ifdef WIN32
	InitializeCriticalSection(&os_lock_);
#else
	pthread_mutex_init(&os_lock_,NULL);
#endif
}

LockImpl::~LockImpl() 
{
#ifdef WIN32
	DeleteCriticalSection(&os_lock_);
#else
	pthread_mutex_destroy(&os_lock_);
#endif
}

bool LockImpl::Try()
{
#ifdef WIN32
	if (TryEnterCriticalSection(&os_lock_) != FALSE) 
	{
		return true;
	}
#else
	if (pthread_mutex_trylock(&os_lock_) == 0) 
	{
		return true;
	}
#endif

    return false;
}

void LockImpl::Lock() {
#ifdef WIN32
	EnterCriticalSection(&os_lock_);
#else
	pthread_mutex_lock(&os_lock_);
#endif
    
}

void LockImpl::Unlock() {
#ifdef WIN32
	LeaveCriticalSection(&os_lock_);
#else
	pthread_mutex_unlock(&os_lock_);
#endif
    
}

//////////////////////////////////////////////////////////////////////////
