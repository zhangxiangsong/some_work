///

#ifndef CURL_PARSER_H_
#define CURL_PARSER_H_

#include <string>
#include <set>
#include "public.h"
/*
08-13-15  17:44               340884 e8000_V102.bin
08-17-15  18:01                  224 RS61850.log
08-14-15  14:22       <DIR>          ADTest

解析以上格式的字符串

*/

#define  FILENAME_POS     37 

typedef std::set<MCD_STR>      SetStringList;

class exCurlParser
{
public:
	exCurlParser(const MCD_STR str);

	bool SetString(const MCD_STR str);

	/// filelist  提取整个文件名

	bool ParseStringDIR(SetStringList&  filelist);

	bool ParseStringFile(SetStringList&  filelist);

private:
	MCD_STR m_string;
};


#endif

