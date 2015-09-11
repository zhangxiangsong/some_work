#include "SyncControl.h"

#ifndef WIN32
#include <fcntl.h>
#include <unistd.h>
#endif

#include "log.h"
#include "UniConversion.h"

exSyncControl::exSyncControl(const DeviceInfo& device,HandleMode mode)
:m_device(device),m_mode(mode)
{
}

exSyncControl::~exSyncControl(void)
{

}


static size_t FetchFiles(void* buffer,size_t size,size_t nmemb,void* stream)
{
	FtpFile* out = (FtpFile*)stream;
	if (out->type == 0)
	{
		//if (size * nmemb > 8192)
		//{
		//	return -1;
		//}
		memcpy((char*)out->buffer + out->pos,buffer,nmemb);
		out->pos += nmemb;
		//fprintf(stdout,"%s",(char*)buffer);
		return nmemb;
	}
	else if (out->type == 1)
	{
#ifdef WIN32
		if (out && !out->stream)
		{
#ifdef MARKUP_WCHAR
			out->stream = _wfopen(MCD_2PCSZ(out->filename),MCD_T("wb"));
#else
			out->stream = fopen(MCD_2PCSZ(out->filename),MCD_T("wb"));
#endif
			if (!out->stream)
			{
				return -1;
			}
		}
		return fwrite(buffer,size,nmemb,out->stream);
#else
		if (out && out->stream < 0)
		{
			out->stream = open(MCD_2PCSZ(out->filename),O_RDWR | O_CREAT);
			if (out->stream < 0)
			{
				//fprintf(stderr,"open file(%s) failed.\n",out->filename);
				return -1;
			}
		}
		return write(out->stream,buffer,nmemb);
#endif
	}
	return -1;
}

bool exSyncControl::DeviceSync(CURL* mCurlcontrol,char* buffer,int buffer_size)
{
	exDeviceControl* mDeviceControl = exDeviceControl::Instance();
	FtpFile ftpFile;
	memset(buffer,0,buffer_size);
	ftpFile.buffer = buffer;
	ftpFile.pos = 0;
	ftpFile.type = 0;
	MCD_STR url;
	
	FormatString(url,MCD_T("ftp://%s:%s%s/"),MCD_2PCSZ(m_device.ip),MCD_2PCSZ(m_device.port),MCD_2PCSZ(mDeviceControl->GetRemoteRootPath()));
	MCD_STR datamodestring;
	if (m_mode == MANAGER_STATI)
	{
		FormatString(url,MCD_T("%s%s/"),MCD_2PCSZ(url),STATI_STRING);
		datamodestring = STATI_STRING;
	} 
	else if(m_mode == MANAGER_EVENT)
	{
		FormatString(url,MCD_T("%s%s/"),MCD_2PCSZ(url),EVENT_STRING);
		datamodestring = EVENT_STRING;
	}
	//curl_easy_setopt(mCurlcontrol,CURLOPT_URL,MCD_2PCSZ(Ws2s(url)));
	MCD_STR userinfo;
	FormatString(userinfo,MCD_T("%s:%s"),MCD_T("admin"),MCD_T("123456"));
	curl_easy_setopt(mCurlcontrol,CURLOPT_USERPWD,MCD_2PCSZ(Ws2s(userinfo)));
	curl_easy_setopt(mCurlcontrol,CURLOPT_WRITEFUNCTION,FetchFiles);
	curl_easy_setopt(mCurlcontrol,CURLOPT_WRITEDATA,&ftpFile);
	curl_easy_setopt(mCurlcontrol,CURLOPT_FTPPORT,"-");

	curl_easy_setopt(mCurlcontrol,CURLOPT_TIMEOUT,mDeviceControl->GetFtpServerTimeout());

	/// 禁用alarm超时机制，否则alarm与siglongjmp一起使用会造成线程崩溃

	curl_easy_setopt(mCurlcontrol,CURLOPT_NOSIGNAL,1);
	curl_easy_setopt(mCurlcontrol,CURLOPT_FORBID_REUSE,1);
/*
	CURLcode res = curl_easy_perform(mCurlcontrol);
	if (res != CURLE_OK)
	{
		if (res == CURLE_OPERATION_TIMEDOUT)
		{
			Log_Print2(PRINT_STOR,"device : %s:%s can not reached.\n",MCD_2PCSZ(m_device.ip),MCD_2PCSZ(m_device.port));
			return false;
		}
		Log_Print1(PRINT_STOR,"get station name failed(%s)\n",MCD_2PCSZ(S2ws(curl_easy_strerror(res))));
		return false;
	}

	std::string station = &buffer[0];
	MCD_STR wstation = S2ws(station);

#ifdef WIN32
	/// 转换为UNICODE
	wchar_t bufferU[1000];
	memset(bufferU,0,sizeof(wchar_t)*1000);
	UCS2FromUTF8(&buffer[0],strlen(&buffer[0]),bufferU,1000);
	wstation = &bufferU[0];
#endif

	memset(buffer,0,buffer_size);

	/// 这里将配置文件给出的设备监测点名称在列表里进行搜索
	/// 所过搜索不到表明 配置的设备监测点名称与实际设备不符
	MCD_STR::size_type pos = wstation.find(m_device.device_name);

	if (pos == MCD_STR::npos)
	{
		Log_Print2(PRINT_STOR,"station_name different remote device (%s:%s).\n",MCD_2PCSZ(m_device.ip),MCD_2PCSZ(m_device.port));
		Log_Print2(PRINT_STOR,"local station_name : %s .\nremote station_name : %s .\n",MCD_2PCSZ(m_device.device_name),MCD_2PCSZ(wstation));
		return false;
	}*/
	MCD_STR stationname = m_device.device_name;
	stationname += MCD_T("/");
	MCD_STR stationurl = url + stationname;
	curl_easy_setopt(mCurlcontrol,CURLOPT_URL,MCD_2PCSZ(Ws2s(stationurl)));
	ftpFile.type = 0;
	ftpFile.pos = 0;
	CURLcode res = curl_easy_perform(mCurlcontrol);
	if (res != CURLE_OK)
	{
		Log_Print1(PRINT_STOR,"get date dir(%s) failed.\n",MCD_2PCSZ(stationurl));
		Log_Print1(PRINT_STOR,"curl told us (%s).\n",MCD_2PCSZ(S2ws(curl_easy_strerror(res))));
		return false;
	}
	std::string station = &buffer[0];
	memset(buffer,0,buffer_size);

	SetStringList filelist;
	exCurlParser localParser(S2ws(station));
	localParser.ParseStringDIR(filelist);

	size_t maxday = exDeviceControl::Instance()->GetMaxSyncDay();

	/// maxday  为0 表示设备端文件全部遍历，主要考虑首次运行的情况
	SetStringList::iterator iter;
	if (maxday > 0)
	{
		iter = filelist.begin();
		if (filelist.size() > maxday)
		{
			for (size_t i = 0; i < filelist.size() - maxday;i++)
			{
				iter++;
			}
		}
		filelist.erase(filelist.begin(),iter);
	}

	/// 创建本设备在本地根目录的目录结构

	MCD_STR localroot = exDeviceControl::Instance()->GetLocalRootPath();
	localroot += datamodestring;
	localroot += MCD_T("/");
	if (!CreateFolder(localroot))
	{
		Log_Print1(PRINT_STOR,"create directory : %s failed.\n",MCD_2PCSZ(localroot));
		return false;
	}

	if (!CreateFolder(localroot + m_device.local_name))
	{
		Log_Print1(PRINT_STOR,"create directory : %s failed.\n",MCD_2PCSZ(MCD_STR(localroot + m_device.local_name)));
		return false;
	}

	///  这里实现本地数据目录循环删除功能，只保留最新N天的文件，保留多少天在配置文件中可配置

	StringList  dirlist;
	size_t max_localday =(size_t)exDeviceControl::Instance()->GetMaxLocalDay();
	GetDirectoryList(localroot + m_device.local_name,dirlist);
	if (max_localday < dirlist.size() && max_localday >= 1)
	{
		size_t del_day = dirlist.size() - max_localday;
		StringList::iterator local_iter = dirlist.begin();
		for (size_t i = 0 ; i < del_day ;i++,++local_iter)
		{
			DeleteDir(*local_iter);

			/// 删除本地文件夹后，就不要在同步这个文件夹了
			//SetStringList::iterator p_iter = filelist.find(*local_iter);
			//if (p_iter != filelist.end())
			//{
			//	filelist.erase(p_iter);
			//}
		}
	}

	iter = filelist.begin();
	for (;iter != filelist.end();++iter)
	{
		if (!CreateFolder(localroot + m_device.local_name + *iter + MCD_T("/")))
		{
			Log_Print1(PRINT_STOR,"create directory : %s failed.\n",MCD_2PCSZ(Ws2s(localroot + m_device.local_name + *iter + MCD_T("/"))));
			return false;
		}
	}

	/// 开始同步文件

	iter = filelist.begin();
	for (;iter != filelist.end();++iter)
	{
		MCD_STR dirurl = stationurl + *iter + MCD_T("/");
		MCD_STR localdir = localroot + m_device.local_name + *iter + MCD_T("/");
		curl_easy_setopt(mCurlcontrol,CURLOPT_URL,Ws2s(dirurl).c_str());
		ftpFile.type = 0;
		ftpFile.pos = 0;
		res = curl_easy_perform(mCurlcontrol);
		if (res != CURLE_OK)
		{
			Log_Print1(PRINT_STOR,"get remote file list (%s) failed.\n",MCD_2PCSZ(Ws2s(dirurl)));
			Log_Print1(PRINT_STOR,"curl told us (%s).\n",MCD_2PCSZ(S2ws(curl_easy_strerror(res))));
		}
		SetStringList datafilelist;
		std::string datafile = &buffer[0];
		memset(buffer,0,buffer_size);
		localParser.SetString(S2ws(datafile));
		localParser.ParseStringFile(datafilelist);
		SetStringList::iterator dataIter = datafilelist.begin();
		for (;dataIter != datafilelist.end();++dataIter)
		{
			MCD_STR localfile = localdir + *dataIter;
			if (FileExists(localfile))
			{
				continue;
			}
			MCD_STR fileurl = dirurl + *dataIter;
			curl_easy_setopt(mCurlcontrol,CURLOPT_URL,Ws2s(fileurl).c_str());
			ftpFile.type = 1;
			ftpFile.pos = 0;
			ftpFile.filename = localfile;
			res = curl_easy_perform(mCurlcontrol);
			if (res != CURLE_OK)
			{
				Log_Print1(PRINT_STOR,"get remote file failed,curl told us %s\n",curl_easy_strerror(res));
			}
			else
			{
				Log_Print1(PRINT_ONLY,"+new file (%s).\n",MCD_2PCSZ((datamodestring + MCD_T("/") + stationname + *iter + MCD_T("/") + *dataIter)));
			}
#ifdef WIN32
			if (ftpFile.stream)
			{
				fclose(ftpFile.stream);
			}
			ftpFile.stream = NULL;
#else
			if (ftpFile.stream >= 0)
			{
				close(ftpFile.stream);
			}
			ftpFile.stream = -1;
			usleep(200000);
#endif
		}
	}

	return true;
}

bool exSyncControl::SetHandleMode( HandleMode mode )
{
	m_mode = mode;
	return true;
}
