///
#ifndef CLIENTSYNC_H_
#define CLIENTSYNC_H_


#ifdef WIN32
#include <Windows.h>
#include <process.h>
#include "Resource.h"
#else
#include <pthread.h>
#endif
#include "Device_Control.h"

/// 最大工作线程个数
/// 每个线程管理10个站点

#define MAX_THREAD_COUNT      20

#ifdef WIN32

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define DelCreateThread(psa, cbStackSize, pfnStartAddr, \
	pvParam, dwCreateFlags, pdwThreadId)                 \
	((HANDLE)_beginthreadex(                              \
	(void *)        (psa),                              \
	(unsigned)      (cbStackSize),                       \
	(PTHREAD_START) (pfnStartAddr),                       \
	(void *)        (pvParam),                          \
	(unsigned)      (dwCreateFlags),                     \
	(unsigned *)    (pdwThreadId)))

#endif
typedef struct
{
	void*  pthis;
	size_t index;  /// 工作线程线程在组里的索引
}ThreadParam;


class exSyncTools
{
public:
	exSyncTools();

	bool Server_Init(exDeviceControl* devicectrl);

	bool Server_Start();

protected:

#ifdef WIN32
	static DWORD WorkThread(PVOID pArg);
#else
	static void*  WorkThread(void* pArg); 
#endif


private:

	/// 实际用于工作的工作线程个数

	int m_fact_workthread_count;

	RemoteStationList m_deviceinfo_list[MAX_THREAD_COUNT];
	ThreadParam m_threadparam[MAX_THREAD_COUNT];

#ifdef WIN32
	HANDLE      m_threadgroup[MAX_THREAD_COUNT];
#else
	pthread_t   m_threadgroup[MAX_THREAD_COUNT];
#endif
};

#endif
