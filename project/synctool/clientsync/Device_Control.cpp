#include "Device_Control.h"

#include <stdio.h>
#include "public.h"


exDeviceControl* exDeviceControl::Instance()
{
	static exDeviceControl localDeviceControl;
	return &localDeviceControl;
}


exDeviceControl::~exDeviceControl()
{

}


#define CHECK_EFF(ret)   do{if(!ret) return false;} while(0)

bool exDeviceControl::Load_DeviceFile(MCD_STR devicelist)
{
	m_filename = devicelist;
	bool el = xml.Load(m_filename.c_str());
	if (false == el)
	{
		//fprintf(stderr,"Load config file failed :%s.\n",m_filename.c_str());
		return el;
	}

	CHECK_EFF(xml.FindElem(MCD_T("root")));
	CHECK_EFF(xml.FindChildElem(MCD_T("public")));
	xml.IntoElem();
	CHECK_EFF(xml.FindChildElem(MCD_T("node_num")));
	m_station_count = StringToInt(xml.GetChildData());

#ifdef WIN32
	CHECK_EFF(xml.FindChildElem(MCD_T("rootdir_win")));
#else
	CHECK_EFF(xml.FindChildElem(MCD_T("rootdir_linux")));
#endif
	m_rootdir = xml.GetChildData();

	CHECK_EFF(xml.FindChildElem(MCD_T("remoterootdir")));
	m_remoterootdir = xml.GetChildData();

	CHECK_EFF(xml.FindChildElem(MCD_T("timeout")));
	m_timeout = StringToInt(xml.GetChildData());

	CHECK_EFF(xml.FindChildElem(MCD_T("max_sync_day")));
	m_maxsyncday = StringToInt(xml.GetChildData());

	CHECK_EFF(xml.FindChildElem(MCD_T("max_local_day")));
	m_maxlocalday = StringToInt(xml.GetChildData());

	CHECK_EFF(xml.FindChildElem(MCD_T("check_circle")));
	m_check_circle = StringToInt(xml.GetChildData());

	CHECK_EFF(xml.FindChildElem(MCD_T("per_thread_dostation_count")));
	m_per_thread_dostation_count = StringToInt(xml.GetChildData());
	xml.OutOfElem();
	for (int i = 0 ; i < m_station_count;i++)
	{
		MCD_STR station_name;
		FormatString(station_name,MCD_T("station%d"),i+1);
		CHECK_EFF(xml.FindChildElem(station_name));
		xml.IntoElem();
		RemoteStation localStation;
		CHECK_EFF(xml.FindChildElem(MCD_T("node_devce_num")));
		localStation.device_count = StringToInt(xml.GetChildData());
		CHECK_EFF(xml.FindChildElem(MCD_T("station_name")));
		localStation.station_name = xml.GetChildAttrib(MCD_T("name"));
		for (int m = 0 ; m < localStation.device_count;m++)
		{
			MCD_STR devicesign;
			FormatString(devicesign,MCD_T("device%d"),m+1);
			CHECK_EFF(xml.FindChildElem(devicesign));
			RemoteDevice localDevice;
			localDevice.device_ip   = xml.GetChildAttrib(MCD_T("dev_extip"));
			localDevice.virtualport = xml.GetChildAttrib(MCD_T("dev_port"));
			localDevice.device_name = xml.GetChildAttrib(MCD_T("devicename"));
			localDevice.local_name  = xml.GetChildAttrib(MCD_T("localname"));
			localStation.device_list.push_back(localDevice);
		}
		m_station_list.push_back(localStation);
		xml.OutOfElem();
	}

	return true;
}

int exDeviceControl::GetStation_Count()
{
	return m_station_count;
}

MCD_STR exDeviceControl::GetLocalRootPath()
{
	MCD_STR temp = m_rootdir;
	temp += MCD_T("/");
	return temp;
}

MCD_STR exDeviceControl::GetRemoteRootPath()
{
	return m_remoterootdir;
}

int exDeviceControl::GetMaxSyncDay()
{
	return m_maxsyncday;
}

bool exDeviceControl::GetStationList( RemoteStationList& stationlist )
{
	stationlist.clear();
	stationlist.assign(m_station_list.begin(),m_station_list.end());
	return true;
}

int exDeviceControl::GetFtpServerTimeout()
{
	return m_timeout;
}

int exDeviceControl::GetCheckCircle()
{
	return m_check_circle;
}

int exDeviceControl::GetPerThreadDostationCount()
{
	return m_per_thread_dostation_count;
}

int exDeviceControl::GetMaxLocalDay()
{
	return m_maxlocalday;
}
