
#include "log.h"
#include <stdio.h>
#include <time.h>

#ifndef WIN32
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif

#include <iostream>
#include "lock.h"

//static Log g_logInfo(false);
static DelLog g_logErr(true);

static std::string TimeToString()
{
	time_t localtime;
	tm* tmtime = NULL;
	time(&localtime);
	tmtime = gmtime(&localtime);

    char timep[35]={0};
    sprintf(timep, "%02d:%02d:%02d", tmtime->tm_hour, tmtime->tm_min, tmtime->tm_sec);
	return timep;
}

//获取当前日期
static std::string GetCurrentDate()
{
	//获得当前日期
	time_t localtime;
	tm* tmtime = NULL;
	time(&localtime);
	tmtime = gmtime(&localtime);
	char szCurDate[10] = {0};
	sprintf(szCurDate, "%d%02d%02d", tmtime->tm_year + 1900, tmtime->tm_mon + 1,tmtime->tm_mday);
	return szCurDate;
}
#ifdef WIN32
static bool StringReplace( std::string& str,std::string lhs,std::string rhs )
{
	std::string::size_type beginpos = str.find(lhs);
	while (beginpos != std::string::npos)
	{
		str.replace(str.begin()+beginpos,str.begin()+beginpos+lhs.size(),rhs);
		beginpos += lhs.size();
		beginpos = str.find(lhs,beginpos);
	}
	return true;
}

#else

static std::string linux_str;
void SetCurrentPath(std::string path)
{
	linux_str = path;
}
#endif


static std::string ModuleDir()
{
	std::string ret;
#ifdef WIN32
	char path[_MAX_PATH];
	if ( GetModuleFileNameA(NULL, path, _MAX_PATH) )
	{
		std::string fullpath = path;
		ret = fullpath;
		if (std::string::npos != fullpath.find_last_of("\\"))
		{
			ret = fullpath.substr(0,fullpath.find_last_of("\\//"));
		}
	}
	StringReplace(ret,"\\","/");
	ret += "/log/";
	CreateFolder(S2ws(ret));
#else
	ret = linux_str;
	ret += "log/";
	CreateFolder(ret);
#endif
	return ret;
}

static Lock localLock;




static  bool SaveFile(std::string path, size_t start, size_t len, const char* content)
{
	bool save_success = true;
	if (path.empty()) 
		return false;

	AutoLock autoLock(localLock);
#ifdef WIN32
	

	FILE* fp = fopen(MCD_2PCSZ(path), "rb+");
	if (!fp)
		fp = fopen(MCD_2PCSZ(path), "wb");


	if (!fp)
		return false;

	if (0==len)
		return true;

	fseek(fp, 0, SEEK_END);
	size_t fileLen = ftell(fp);
	size_t offset = __min(fileLen, start);

	fseek(fp, offset, SEEK_SET);
	save_success = (len == fwrite(content, 1, len, fp));
	fflush(fp);
	fclose(fp);

#else
	int fd = open(MCD_2PCSZ(path),O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR);
	if (fd < 0)
	{
		/// 表示文件不存在
		fd = open(MCD_2PCSZ(path),O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
		if (fd < 0)
		{
			printf("open (%s) failed (error info :%s).\n",MCD_2PCSZ(path),strerror(errno));
			return false;
		}
	}
	lseek(fd,0,SEEK_END);
	save_success = (write(fd,content,len) == (ssize_t)len);
	close(fd);
#endif

	return save_success;
}

DelLog::DelLog(bool useErrFile)
{
    m_useErrFile = useErrFile;
}

void DelLog::Write(std::string info, int isPrint)
{
	if (isPrint == PRINT_STOR)
	{
		printf("%s",info.c_str());
		SaveFile(LogFilePath(), 0xFFFFFFFF, info.length(), MCD_2PCSZ(info));
	}
	else if (isPrint == PRINT_ONLY)
	{
		printf("%s",info.c_str());
	}
	else if (isPrint == STORG_ONLY)
	{
		SaveFile(LogFilePath(), 0xFFFFFFFF, info.length(), MCD_2PCSZ(info));
	}
}


void DelLog::Write(std::string info, std::string /*src*/, int /*line*/,int isPrint)
{
	Write(info,isPrint);
}


std::string DelLog::LogFilePath()
{
    std::string s;
    if (m_useErrFile)
        s = ModuleDir() + "Erro" + GetCurrentDate() + ".txt";
    else
        s = ModuleDir() + "Info" + GetCurrentDate() + ".txt";
	//printf("%s\n",s.c_str());
    return s;
}

#ifdef WIN32
static void SetConsoleColor(
	unsigned short textColor = FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE, 
	unsigned short BackColor = 0)
{
	static HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE); 
	SetConsoleTextAttribute(hCon,textColor|BackColor);
}
#endif

void ServerLogErr(std::string info, const string& src, int line,int isPrint)
{
#ifdef WIN32
    //SetConsoleColor(FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
    g_logErr.Write("[" + TimeToString() + "] " + info,src, line,isPrint);
#ifdef WIN32
	//SetConsoleColor();
#endif
}

