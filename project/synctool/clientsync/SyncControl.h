///

#ifndef EX_SYNCCONTROL_H_
#define EX_SYNCCONTROL_H_

#include <string>
#include <curl/curl.h>

#include "Device_Control.h"
#include "Curl_parser.h"
#include "public.h"

typedef struct
{
	MCD_STR  ip;
	MCD_STR  port;
	MCD_STR  device_name; // 设备监测点编号
	MCD_STR  local_name;
}DeviceInfo;


struct FtpFile
{
	int   type;  // 0表示获取目录 1表示文件
#ifdef WIN32
	FILE* stream;
#else
	int   stream;
#endif
	int   pos;
	void* buffer;
	MCD_STR filename;
	FtpFile()
	{
		type = 0;
#ifdef WIN32
		stream = NULL;
#else
		stream = -1;
#endif
		pos = 0;
		buffer = 0;
	}
};

/*
同步目录
*/

class exSyncControl
{
public:
	exSyncControl(const DeviceInfo& device,HandleMode mode);

	~exSyncControl(void);

	bool SetHandleMode(HandleMode mode);

	/*
	 *	执行同步操作
	 */
	bool DeviceSync(CURL* mCurlcontrol,char* buffer,int buffer_size);


private:
	DeviceInfo m_device;
	HandleMode m_mode;

	
};

#endif
