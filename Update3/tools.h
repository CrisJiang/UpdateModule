#pragma once

#include <iostream>
#include <sstream>
#include <Windows.h>


template<typename T>
std::string to_string(T _in)
{
	std::string str;
	std::stringstream stream;
	stream << _in;
	str = stream.str();
	stream.str("");
	return str;
}


template<typename T>
void stringToType(std::string _in, T &_out)
{
	std::stringstream stream;
	stream << _in;
	stream >> _out;
	stream.str("");
}

std::string to_string(SYSTEMTIME *systime);
std::string GBKToUTF8(const std::string& strGBK);
std::string UTF8ToGBK(const std::string& strUTF8);
std::string getsTime(unsigned long t_src);
std::string getsFtime(std::string &sFormat, time_t t_src);
SYSTEMTIME toSysTime(std::string strIn);
SYSTEMTIME toSysTime(time_t t);
//SYSTEMTIME Time_tToSystemTime(time_t t);
//time_t SystemTimeToTime_t( const SYSTEMTIME& st );
//static BOOL ChangeTimeByMiliSecond(SYSTEMTIME& sysTime, DWORD dwMiliSecond);
std::string WString2String(const std::wstring& ws);
std::wstring String2WString(const std::string& s);
