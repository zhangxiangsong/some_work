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

/// ������̸߳���
/// ÿ���̹߳���10��վ��

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
	size_t index;  /// �����߳��߳������������
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

	/// ʵ�����ڹ����Ĺ����̸߳���

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
