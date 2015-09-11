/**
 *\brief  
 */

#ifndef PQDIF_GEN_LOG_H_
#define PQDIF_GEN_LOG_H_

#include "public.h"
#ifdef WIN32
#include <Windows.h>
#endif
#include <stdarg.h>
#include <string>
#include <vector>

using std::string;
using std::wstring;
using std::vector;

/**
  \brief 日志类， 记录业务服务器的运行日志
 */
class DelLog
{
public:
    DelLog(bool useErrFile);

    bool m_useErrFile;

    void Write(std::string info, std::string src, int line,int isPrint);
    void Write(std::string info, int isPrint);

protected:
    std::string LogFilePath();
};

#define  PRINT_STOR    0
#define  PRINT_ONLY    1
#define  STORG_ONLY    2

#ifdef WIN32

#define Log_Print0(mode,format)   \
	do {\
	MCD_STR __info; \
	FormatString(__info,MCD_T(format)); \
	std::string __infos = Ws2s(__info);\
	ServerLogErr(__infos, __FILE__, __LINE__,mode);\
	} while (0)

#define Log_Print1(mode,format,arg1)   \
	do {\
	MCD_STR __info; \
	FormatString(__info,MCD_T(format),arg1); \
	std::string __infos = Ws2s(__info);\
	ServerLogErr(__infos, __FILE__, __LINE__,mode);\
	} while (0)

#define Log_Print2(mode,format,arg1,arg2)   \
	do {\
	MCD_STR __info; \
	FormatString(__info,MCD_T(format),arg1,arg2); \
	std::string __infos = Ws2s(__info);\
	ServerLogErr(__infos, __FILE__, __LINE__,mode);\
	} while (0)



#else

#define Log_Print0(mode,format)   \
	do {\
	MCD_STR __info; \
	FormatString(__info,MCD_T(format)); \
	ServerLogErr(__info, __FILE__, __LINE__,mode);\
	} while (0)

#define Log_Print1(mode,format,arg1)   \
	do {\
	MCD_STR __info; \
	FormatString(__info,MCD_T(format),arg1); \
	ServerLogErr(__info, __FILE__, __LINE__,mode);\
	} while (0)

#define Log_Print2(mode,format,arg1,arg2)   \
	do {\
	MCD_STR __info; \
	FormatString(__info,MCD_T(format),arg1,arg2); \
	ServerLogErr(__info, __FILE__, __LINE__,mode);\
	} while (0)


#endif


void ServerLogErr(std::string info, const string& src, int line,int isPrint);

#endif

