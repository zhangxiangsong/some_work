///

#ifndef PUBLIC_H_
#define PUBLIC_H_


#include <string>
#include <vector>
#include <set>

#include "Markup.h"

#ifndef WIN32
#include <unistd.h>
#endif

//#define MARKUP_WCHAR
//
//#ifdef  MARKUP_WCHAR
//#define MCD_STR   std::wstring
//#define MCD_T(s) L ## s
//#else
//#define MCD_STR   std::string
//#define MCD_T(s)  s
//#endif
//
//#define  MCD_2PCSZ(s) s.c_str()

/// 睡眠多少秒
#ifdef WIN32
#define TSleep(second)        Sleep((second) * 1000)
#else
#define TSleep(second)        sleep(second)
#endif

/// 睡眠多少毫秒
#ifdef WIN32
#define TMSleep(millisecond)   Sleep(millisecond)
#else
#define TMSleep(millisecond)   usleep((millisecond) * 1000)
#endif

typedef enum
{
	MANAGER_STATI,
	MANAGER_EVENT
}HandleMode;

#define STATI_STRING     MCD_T("historyFile")
#define EVENT_STRING     MCD_T("eventFile")


int StringToInt( const MCD_STR str );

std::string InToString(int uint);

#ifdef MARKUP_WCHAR
void FormatString(MCD_STR& s, const wchar_t* format, ...);
void FormatString(std::string& s, const char* format, ...);

#else
void FormatString(MCD_STR& s, const char* format, ...);
#endif



MCD_STR S2ws(const std::string& s);

std::string Ws2s(const MCD_STR ws);


/// 递归创建目录结构

bool CreateFolder( const MCD_STR standPath );

bool FileExists(const MCD_STR localfile);


int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput,int outSize);

typedef std::set<MCD_STR>  StringList;

bool GetDirectoryList(const MCD_STR& pathName, StringList& fileList);

bool DeleteDir(const MCD_STR& pathName);


#endif
