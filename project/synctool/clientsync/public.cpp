#include "public.h"

#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#ifdef WIN32
#include <shlobj.h>
#include <shlwapi.h>
#include <tchar.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

std::string InToString(int uint)
{
#ifdef WIN32
	char buf[20];
	_itoa(uint,buf,10);
	std::string temp(buf);
	return temp;
#else
	std::string val = "";
	sscanf(val.c_str(),"%d",&uint);
	return val;
#endif
}
#ifdef MARKUP_WCHAR
void FormatString(MCD_STR& s, const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);

	std::vector<wchar_t> buf(8196,0);
	_vsnwprintf(&buf[0], 8196, format, args);
	s = &buf[0];    
	va_end(args);
}
void FormatString(std::string& s, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	std::vector<char> buf(8196,0);
	vsnprintf(&buf[0], 8196, format, args);
	s = &buf[0];    
	va_end(args);
}
#else
void FormatString(MCD_STR& s, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	std::vector<char> buf(8196,0);
	vsnprintf(&buf[0], 8196, format, args);
	s = &buf[0];    
	va_end(args);
}
#endif



#ifdef WIN32

MCD_STR S2ws(const std::string& s)
{
#ifdef MARKUP_WCHAR
	int lenU16 = MultiByteToWideChar(CP_OEMCP, 0, s.c_str(), s.size(), NULL, 0);
	if (0 == lenU16)
		return L"";
	std::vector<wchar_t> buf;
	buf.resize(lenU16);
	MultiByteToWideChar(CP_OEMCP, 0, s.c_str(), -1, &buf[0], lenU16);
	return std::wstring(buf.begin(), buf.end());
#else
	return s;
#endif
}

std::string Ws2s(const MCD_STR ws)
{
#ifdef MARKUP_WCHAR
	int len = WideCharToMultiByte(CP_ACP,0,ws.c_str(),ws.size(),NULL,0,NULL,NULL);
	if (0 == len)
		return "";
	std::vector<char> buf;
	buf.resize(len);
	WideCharToMultiByte(CP_ACP,0,ws.c_str(),ws.size(),&buf[0],len,NULL,NULL);
	return std::string(buf.begin(), buf.end());
#else
	return ws;
#endif
}


bool GetDirectoryList(const MCD_STR& pathName, StringList& fileList)
{
	WIN32_FIND_DATAW findDA;
	MCD_STR temp = pathName;
	temp += MCD_T("*");
	HANDLE hHandle = FindFirstFileW(temp.c_str(),&findDA);
	if (INVALID_HANDLE_VALUE != hHandle)
	{
		do 
		{
			if(findDA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (!_tcscmp(findDA.cFileName,MCD_T(".")) || !_tcscmp(findDA.cFileName,MCD_T("..")))
				{
					continue;
				}
				MCD_STR subtemp = pathName;
				subtemp += findDA.cFileName;
				//subtemp += MCD_T("/");
				fileList.insert(subtemp);
			}
		} while (FindNextFileW(hHandle,&findDA) != 0);
	}
	else{ return false;}
	FindClose(hHandle);
	return true;
}

bool DeleteDir(const MCD_STR& pathName)
{
	MCD_STR temp(pathName.begin(),pathName.end());
	MCD_STR::size_type pos = 0;
	while((pos = temp.find(MCD_T("/"))) != MCD_STR::npos)
	{
		temp.replace(pos,1,MCD_T("\\"));
	}
	system(("rd /s /q " + Ws2s(temp)).c_str());
	return true;
}


#else

size_t get_wchar_size(const char* str)
{
	size_t len = strlen(str);
	size_t size = 0;
	size_t i = 0;
	for (;i < len ; i++)
	{
/*		if (str[size] >= 0 && str[size] <= 127)  // 不是全角字符
		{
			size += sizeof(wchar_t);
		}
		else // 全角字符 中文*/
		{
			size += sizeof(wchar_t);
			i += 2;
		}
	}
	return size;
}

MCD_STR S2ws(const std::string& s)
{
#ifdef MARKUP_WCHAR
	setlocale(LC_ALL,"zh_CN.utf8");
	if (s.size() <= 0)
	{
		return L"";
	}
	//size_t size_of_ch = strlen(s.c_str()) * sizeof(char);
	size_t size_of_wc = get_wchar_size(s.c_str());
	std::vector<wchar_t> buffer(size_of_wc,0);
	mbstowcs(&buffer[0],s.c_str(),size_of_wc);
	return std::wstring(&buffer[0]);
#else
	return s;
#endif
}

std::string Ws2s(const MCD_STR ws)
{
#ifdef MARKUP_WCHAR
	setlocale(LC_ALL,"zh_CN.utf8");
	if (ws.size() <= 0)
	{
		return "";
	}
	size_t size = wcslen(ws.c_str()) * sizeof(wchar_t);
	std::vector<char> buffer(size,0);
	wcstombs(&buffer[0],ws.c_str(),size);
	return std::string(&buffer[0]);
#else
	return ws;
#endif
}


bool GetDirectoryList(const MCD_STR& pathName, StringList& fileList)
{
	DIR* dir;
	struct dirent* ptr;
	if ((dir = opendir(pathName.c_str())) == NULL)
	{
		return false;
	}
	while((ptr = readdir(dir)) != NULL)
	{
		if (strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)
		{
			continue;
		}
		else if (ptr->d_type == 4)
		{
			MCD_STR subtemp = pathName;
			subtemp += ptr->d_name;
			subtemp += MCD_T("/");
			fileList.insert(subtemp);
		}

	}
	return true;
}

bool DeleteDir(const MCD_STR& pathName)
{
	system((MCD_STR("rm -rf ")+ pathName).c_str());
	return true;
}

#endif


int StringToInt( const MCD_STR str )
{
	return atoi(MCD_2PCSZ(Ws2s(str)));
}

#ifdef WIN32
bool CreateFolder( const MCD_STR standPath )
{
	MCD_STR tempchar(standPath.begin(),standPath.end());
	MCD_STR temp = tempchar;
	MCD_STR::size_type pos = 0;
	while((pos = temp.find(MCD_T("/"))) != MCD_STR::npos)
	{
		temp.replace(pos,1,MCD_T("\\"));
	}

	pos = temp.rfind(MCD_T("\\"));
	if (MCD_STR::npos != pos)
	{
		MCD_STR stdFolder = temp.substr(0,pos+1);
		if(!PathFileExists(stdFolder.c_str()))
		{
			int result = SHCreateDirectoryEx(NULL,MCD_2PCSZ(stdFolder),NULL);
			if(result != ERROR_SUCCESS)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

bool FileExists(const MCD_STR localfile)
{
	MCD_STR file = localfile;
	if (PathFileExists(MCD_2PCSZ(file)))
	{
		return true;
	}
	return false;
}

#else


typedef std::vector<MCD_STR>         DelWStringList;

bool Split(const MCD_STR s_,const MCD_STR sep_,DelWStringList& delList,const size_t smin)
{
	typedef MCD_STR::const_iterator citer;
	citer sbeg = sep_.begin();
	citer send = sep_.end();
	for(citer beg = s_.begin(),end = s_.end();beg != end;)
	{
		citer tbeg = beg;
		for(;tbeg != end;++tbeg)
		{
			if(count(sbeg,send,*tbeg) == 0){
				break;
			}
		}
		citer tend = find_first_of(tbeg,end,sbeg,send);
		if(tbeg != tend)
		{
			MCD_STR  str(tbeg,tend);
			if(str.size() >= smin)  //过滤掉字符串长度为min的字符串
			{
				MCD_STR::size_type disb = str.find_first_not_of(MCD_T(" "));
				MCD_STR::size_type dise = str.find_last_not_of(MCD_T(" "));
				delList.push_back(str.substr(disb,dise + 1 - disb));				
			}
		}
		beg = tend;
	}
	return true;
}

bool CreateFolder( const MCD_STR standPath )
{
	if (standPath.size() <= 0)
	{
		return false;
	}
	DelWStringList filelist;
	Split(standPath,MCD_T("/"),filelist,0);
	MCD_STR tmp;
	for (size_t i = 0 ; i < filelist.size();i++)
	{
		if (i == 0)
		{
			filelist[i].insert(0,MCD_T("/"),1);
		}
		tmp += filelist[i];
		if (access(MCD_2PCSZ(Ws2s(tmp)),F_OK) < 0)
		{
			if(mkdir(MCD_2PCSZ(Ws2s(tmp)),0777) < 0)
			{
				//fprintf(stderr,"mkdir (%s) failed.\n",Ws2s(tmp).c_str());
			}
		}

		//if (chdir(tmp.c_str()) < 0)
		//{
		//	return false;
		//}
		tmp += "/";
	}
	return true;
}

bool FileExists(const MCD_STR localfile)
{
	if (access(MCD_2PCSZ(Ws2s(localfile)),F_OK) < 0)
	{
		return false;
	}
	return true;
}



#endif

// #c---
/*****************************************************************************
 * 将一个字符的Unicode(UCS-2和UCS-4)编码转换成UTF-8编码.
 *
 * 参数:
 *    unic     字符的Unicode编码值
 *    pOutput  指向输出的用于存储UTF8编码值的缓冲区的指针
 *    outsize  pOutput缓冲的大小
 *
 * 返回值:
 *    返回转换后的字符的UTF8编码所占的字节数, 如果出错则返回 0 .
 *
 * 注意:
 *     1. UTF8没有字节序问题, 但是Unicode有字节序要求;
 *        字节序分为大端(Big Endian)和小端(Little Endian)两种;
 *        在Intel处理器中采用小端法表示, 在此采用小端法表示. (低地址存低位)
 *     2. 请保证 pOutput 缓冲区有最少有 6 字节的空间大小!
 ****************************************************************************/

int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput,int outSize)
{
    assert(pOutput != NULL);
    assert(outSize >= 6);

    if ( unic <= 0x0000007F )
    {
        // * U-00000000 - U-0000007F:  0xxxxxxx
        *pOutput     = (unic & 0x7F);
        return 1;
    }
    else if ( unic >= 0x00000080 && unic <= 0x000007FF )
    {
        // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
        *(pOutput+1) = (unic & 0x3F) | 0x80;
        *pOutput     = ((unic >> 6) & 0x1F) | 0xC0;
        return 2;
    }
    else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )
    {
        // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
        *(pOutput+2) = (unic & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 12) & 0x0F) | 0xE0;
        return 3;
    }
    else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )
    {
        // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput+3) = (unic & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 18) & 0x07) | 0xF0;
        return 4;
    }
    else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )
    {
        // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput+4) = (unic & 0x3F) | 0x80;
        *(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 24) & 0x03) | 0xF8;
        return 5;
    }
    else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )
    {
        // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput+5) = (unic & 0x3F) | 0x80;
        *(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 30) & 0x01) | 0xFC;
        return 6;
    }

    return 0;
}
// #c---end
