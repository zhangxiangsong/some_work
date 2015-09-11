/// �����豸��Ϣ�Ĺ�����

#ifndef DEVICE_CONTROL_H_
#define DEVICE_CONTROL_H_

#include <string>
#include <vector>

#include "public.h"

/*
 *	Զ��վ���һ̨�豸
 */
typedef struct
{
	MCD_STR device_name;  // ��������
	MCD_STR device_ip;    // ����ip
	MCD_STR virtualport;  // ����˿ں�
	MCD_STR local_name;   // �豸�������ı���ӳ��
}RemoteDevice;

typedef std::vector<RemoteDevice>      RemoteDeviceList;


typedef struct
{
	int device_count;        // վ���豸����
	MCD_STR station_name;    // վ������
	RemoteDeviceList device_list;
}RemoteStation;

typedef std::vector<RemoteStation>     RemoteStationList;

class exDeviceControl
{
public:

	static exDeviceControl* Instance();

	~exDeviceControl();

	/*
	 *	devicelist   devicelist.ini�����ļ����Ƽ���·��
	 */

	bool Load_DeviceFile(MCD_STR devicelist);

	/*
	 *	��ȡվ�����
	 */
	int GetStation_Count();

	/*
	 *	��ȡ���ݴ洢��Ŀ¼
	 */
	MCD_STR GetLocalRootPath();

	MCD_STR GetRemoteRootPath();

	/*
	 *	��ȡ�η���ͬ������Ŀ¼����Ķ�������
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

	/// �豸��Ϣ

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
