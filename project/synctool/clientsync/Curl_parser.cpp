#include "Curl_parser.h"

#include <stdio.h>

exCurlParser::exCurlParser(const MCD_STR str)
:m_string(str)
{
}

bool exCurlParser::ParseStringDIR(SetStringList&  filelist)
{
	if (m_string.size() <= 0)
	{
		return false;
	}
	MCD_STR::size_type pos = 0;
	MCD_STR::size_type subpos = 0;
	MCD_STR::size_type linepos = 0;
	do 
	{
		pos = m_string.find(MCD_T("\n"),linepos);
		if (pos < m_string.size() && pos > linepos)
		{
			subpos = m_string.rfind(MCD_T(" "),pos);
			if (subpos < m_string.size() && subpos > linepos)
			{
#ifdef WIN32
				MCD_STR file = m_string.substr(subpos + 1,pos - subpos - 2);
#else
				MCD_STR file = m_string.substr(subpos + 1,pos - subpos - 1);
#endif
				filelist.insert(file);
				linepos = pos + 1;
			}
		}

	} while (pos != std::wstring::npos);

	return true;
}

bool exCurlParser::ParseStringFile(SetStringList&  filelist)
{
	if (m_string.size() <= 0)
	{
		return false;
	}
	MCD_STR::size_type pos = 0;
	MCD_STR::size_type subpos = 0;
	MCD_STR::size_type linepos = 0;
	do 
	{
		pos = m_string.find(MCD_T("\n"),linepos);
		if (pos < m_string.size() && pos > linepos)
		{
			subpos = m_string.rfind(MCD_T(" "),pos);
			if (subpos < m_string.size() && subpos > linepos)
			{
#ifdef WIN32
				MCD_STR file = m_string.substr(subpos + 1,pos - subpos - 2);
#else
				MCD_STR file = m_string.substr(subpos + 1,pos - subpos - 1);
#endif
				/// ��subposλ������ǰ20�ַ����ڲ���" 0 "�ַ�������ҵ�˵�����ļ�����Ϊ0����

				MCD_STR zerostr = m_string.substr(subpos - 20,21);
				if (zerostr.rfind(MCD_T(" 0 "),subpos) == MCD_STR::npos)
				{
					filelist.insert(file);
				}
				linepos = pos + 1;
			}
		}

	} while (pos != std::wstring::npos);

	return true;
}

bool exCurlParser::SetString( const MCD_STR str )
{
	m_string = str;
	return true;
}

