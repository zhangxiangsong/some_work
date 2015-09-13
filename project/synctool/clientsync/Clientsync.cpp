// clientsync.cpp : 定义控制台应用程序的入口点。
//

#ifdef WIN32
#include "stdafx.h"
#else
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#endif
#include "Clientsync.h"

#include <curl/curl.h>
#include <time.h>
#include "Device_Control.h"
#include "SyncControl.h"
#include "log.h"

#ifdef WIN32
#pragma warning(disable:4099)
#endif


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif

// 唯一的应用程序对象
#ifdef WIN32
CWinApp theApp;
#endif

using namespace std;

/// 线程是否继续运行

static int running = 0;

void sigint_handler(int signalId)
{
	Log_Print0(PRINT_STOR,"all work_thread will over.\n");
	curl_global_cleanup();
	exit(0);
	running = 0;
}


#ifdef WIN32
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
	}

	MCD_STR devicefile = MCD_T("E:/work_dir/PQ_E8000/doc/Subject/synctool/bin/devicelist.xml");

#else
int main(int argc, char *argv[])
{
	int nRetCode = 0;

	if(argc < 2)
	{
		return 0;
	}
	MCD_STR devicefile = MCD_STR(argv[1]);
	
	SetCurrentPath(devicefile);

	devicefile += MCD_STR("devicelist.xml");
#endif

	exDeviceControl* mDeviceControl = exDeviceControl::Instance();
	if(!mDeviceControl->Load_DeviceFile(devicefile))
	{
		Log_Print1(PRINT_STOR,"load devicelist.xml file failed (%s).\n",MCD_2PCSZ(devicefile));
		return nRetCode;
	}

	CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (code != CURLE_OK)
	{
		Log_Print0(PRINT_STOR,"curl_global_init failed.\n");
		return nRetCode;
	}
	running = 1;

	exSyncTools localSyncTools;

	localSyncTools.Server_Init(mDeviceControl);
	localSyncTools.Server_Start();

	return nRetCode;
}


exSyncTools::exSyncTools()
{
	memset(m_threadparam,0,sizeof(ThreadParam)*MAX_THREAD_COUNT);
#ifdef WIN32
	memset(m_threadgroup,0,sizeof(HANDLE)*MAX_THREAD_COUNT);
#else
	memset(m_threadgroup,0,sizeof(pthread_t)*MAX_THREAD_COUNT);
#endif
	m_fact_workthread_count = 0;
}

bool exSyncTools::Server_Init(exDeviceControl* devicectrl)
{
	if (!devicectrl)
	{
		return false;
	}
	size_t max_station_count = devicectrl->GetStation_Count();
	int per_thread_dostation_count = devicectrl->GetPerThreadDostationCount();

	if (max_station_count % per_thread_dostation_count == 0)
	{
		m_fact_workthread_count = max_station_count / per_thread_dostation_count;
	}
	else
	{
		m_fact_workthread_count = max_station_count / per_thread_dostation_count + 1;
	}

	if (m_fact_workthread_count > MAX_THREAD_COUNT)
	{
		m_fact_workthread_count = MAX_THREAD_COUNT;
	}

	RemoteStationList  stationlist;
	devicectrl->GetStationList(stationlist);
	if (max_station_count != stationlist.size())
	{
		return false;
	}
	for (size_t i = 0 ; i < stationlist.size() ; i++)
	{
		m_deviceinfo_list[i/per_thread_dostation_count].push_back(stationlist[i]);
	}

	/// 创建线程

#ifdef WIN32
	for (int i = 0 ; i < m_fact_workthread_count ; i++)
	{
		m_threadparam[i].pthis = this;
		m_threadparam[i].index = i;
		m_threadgroup[i] = DelCreateThread(NULL,0,WorkThread,&m_threadparam[i],0,0);
		if (m_threadgroup[i] == INVALID_HANDLE_VALUE)
		{
			Log_Print0(PRINT_STOR,"create work_thread failed.\n");
			return false;
		}
	}
#else
	for (int i = 0 ; i < m_fact_workthread_count ; i++)
	{
		m_threadparam[i].pthis = this;
		m_threadparam[i].index = i;
		if (pthread_create(&m_threadgroup[i],NULL,WorkThread,&m_threadparam[i]) != 0)
		{
			Log_Print0(PRINT_STOR,"create work_thread failed.\n");
			return false;
		}
	}

#endif



	return true;
}

bool exSyncTools::Server_Start()
{
	Log_Print0(PRINT_STOR,"synctools server begin.\n");
#ifdef WIN32
	Sleep(INFINITE);
#else
	signal(SIGINT,sigint_handler);
	for (int i = 0 ; i < m_fact_workthread_count ; i++)
	{
		int error = 0;
		if ((error = pthread_join(m_threadgroup[i],NULL)) != 0)
		{
			Log_Print2(PRINT_STOR,"wait work_thread over failed,error code : %d ,work_thread (index : %d).\n",error,i);
		}
	}
#endif
	return true;
}

#define  BUFFER_SIZE      1024*80


#ifdef WIN32
DWORD exSyncTools::WorkThread( PVOID pArg )
#else
void* exSyncTools::WorkThread( void* pArg )
#endif
{
	ThreadParam* threadParam = (ThreadParam*)pArg;
	exSyncTools* pthis = (exSyncTools*)threadParam->pthis;
	RemoteStationList localstationlist;
	localstationlist.assign(pthis->m_deviceinfo_list[threadParam->index].begin(),pthis->m_deviceinfo_list[threadParam->index].end());

	int count = 0;
    int default_circle = exDeviceControl::Instance()->GetCheckCircle();
	const int buffer_size = 1024*80;
	char* buffer = new char[buffer_size];
	memset(buffer,0,buffer_size);
	while(running)
	{
		Log_Print1(PRINT_ONLY,"[work_thread %d ] beg sync event and history data.\n",threadParam->index);
		time_t begtime;
		time(&begtime);
		CURL* mCurlcontrol = curl_easy_init();
		if (mCurlcontrol == NULL)
		{
			continue;
		}
		for (size_t i = 0 ; i < localstationlist.size() ; i++)
		{
			for (int m = 0 ; m < localstationlist[i].device_count ; m++)
			{
				RemoteDevice& remote_device = localstationlist[i].device_list[m];
				DeviceInfo deviceinfo;

				deviceinfo.ip = remote_device.device_ip;
				deviceinfo.port = remote_device.virtualport;
				deviceinfo.device_name = remote_device.device_name;
				deviceinfo.local_name = remote_device.local_name;
				deviceinfo.local_name += MCD_T("/");
				exSyncControl  syncobj(deviceinfo,MANAGER_EVENT);
			    syncobj.DeviceSync(mCurlcontrol,buffer,buffer_size);
				syncobj.SetHandleMode(MANAGER_STATI);
				syncobj.DeviceSync(mCurlcontrol,buffer,buffer_size);
				TMSleep(100);  ///< sleep 100ms
 			}
		}
		Log_Print2(PRINT_ONLY,"[work_thread %d ] end sync event and history data(%.6d).\n",threadParam->index,count++);
		curl_easy_cleanup(mCurlcontrol);
		mCurlcontrol = NULL;
		time_t endtime;
		time(&endtime);
		/// 同步一遍耗时多少s
		int alltime = int(endtime - begtime);
		if (alltime < default_circle)
		{
			TSleep(default_circle - alltime);
		}

	}
    delete [] buffer;
	Log_Print1(PRINT_ONLY,"[work_thread %d ] normal over.\n",threadParam->index);
	return 0;
}
