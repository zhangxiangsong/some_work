/// 所有设备信息的管理类

#ifndef DEVICE_CONTROL_H_
#define DEVICE_CONTROL_H_

#include <string>
#include <vector>

#include "public.h"

/*
 *	远程站点的一台设备
 */
typedef struct
{
	MCD_STR device_name;  // 监测点名称
	MCD_STR device_ip;    // 监测点ip
	MCD_STR virtualport;  // 虚拟端口号
	MCD_STR local_name;   // 设备监测点名的本地映射
}RemoteDevice;

typedef std::vector<RemoteDevice>      RemoteDeviceList;


typedef struct
{
	int device_count;        // 站点设备个数
	MCD_STR station_name;    // 站点名称
	RemoteDeviceList device_list;
}RemoteStation;

typedef std::vector<RemoteStation>     RemoteStationList;

class exDeviceControl
{
public:

	static exDeviceControl* Instance();

	~exDeviceControl();

	/*
	 *	devicelist   devicelist.ini配置文件名称及其路径
	 */

	bool Load_DeviceFile(MCD_STR devicelist);

	/*
	 *	获取站点个数
	 */
	int GetStation_Count();

	/*
	 *	获取数据存储根目录
	 */
	MCD_STR GetLocalRootPath();

	MCD_STR GetRemoteRootPath();

	/*
	 *	获取次访问同步设置目录最近的多少天数
	 */
	int GetMaxSyncDay();

	int GetMaxLocalDay();

	int GetFtpServerTimeout();

	int GetCheckCircle();

	int GetPerThreadDostationCount();


	bool GetStationList(RemoteStationList& stationlist);


private:
	MCD_STR     m_filename;
	CMarkup     xml;

	/// 设备信息

	int  m_station_count;
	RemoteStationList m_station_list;

	MCD_STR  m_rootdir;
	MCD_STR  m_remoterootdir;
	int m_maxsyncday;
	int m_maxlocalday;

	int m_timeout;
	int m_check_circle;
	int m_per_thread_dostation_count;

};


#endif
