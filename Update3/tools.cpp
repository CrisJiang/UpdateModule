#pragma once

#include "tools.h"
#include <time.h>

#define _MILLISECOND	((ULONGLONG)10000) 

std::string GBKToUTF8(const std::string& strGBK)
{
	int nLen = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	WCHAR * wszUTF8 = new WCHAR[nLen];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, wszUTF8, nLen);

	nLen = WideCharToMultiByte(CP_UTF8, 0, wszUTF8, -1, NULL, 0, NULL, NULL);
	char * szUTF8 = new char[nLen];
	WideCharToMultiByte(CP_UTF8, 0, wszUTF8, -1, szUTF8, nLen, NULL, NULL);

	std::string strTemp(szUTF8);
	delete[]wszUTF8;
	delete[]szUTF8;
	return strTemp;
}

std::string UTF8ToGBK(const std::string& strUTF8)
{
	int nLen = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	unsigned short * wszGBK = new unsigned short[nLen + 1];
	memset(wszGBK, 0, nLen * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, (LPWSTR)wszGBK, nLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[nLen + 1];
	memset(szGBK, 0, nLen + 1);
	WideCharToMultiByte(CP_ACP,0, (LPWSTR)wszGBK, -1, szGBK, nLen, NULL, NULL);

	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}
//[CrisJiang 2018/07/06]
/*
	t_src --- ms
	std::string "00:00:00"
*/
std::string getsTime(unsigned long t_src)
{
	int mseconde = (t_src % 1000) / 10;
	int second = t_src / 1000;
	int min = second / 60;

	std::stringstream stream;
	stream.fill('0');			//补0
	stream.width(2);			//设置最小宽度
	stream << min <<':';
	
	stream.width(2);
	stream << (second >= 60? second - min*60 : second)  <<':';

	stream.width(2);
	stream << mseconde;
	return stream.str();
}

std::string getsFtime(std::string &sFormat, time_t t_src)
{	
	char stime[100] = {0};
	struct tm* _tmtmp;
	//gmtime_s(&_tmtmp, &t_src);
	_tmtmp = localtime(&t_src);
	strftime(stime, sizeof(stime),
		sFormat.c_str(), _tmtmp);
	//delete _tmtmp;
	return std::string(stime);
}


SYSTEMTIME toSysTime(std::string strIn)
{
	SYSTEMTIME outTime;
	if (strIn.length() != 19
		&& strIn.length() != 23)
	{
		GetSystemTime(&outTime);
		return outTime;
	}

	if (strIn.length() == 19)
	{
		std::string stmp;
		stmp = strIn;
		std::stringstream ss;
		ss << stmp << ".000";
		strIn = ss.str();
	}

	int nYear, nMonth, nDate, nHour, nMin, nSec, nMiliSec;
	sscanf_s(strIn.c_str(), "%d-%d-%d %d:%d:%d.%d", &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec, &nMiliSec);
	
	outTime.wYear = nYear;
	outTime.wMonth = nMonth;
	outTime.wDay = nDate;
	outTime.wHour = nHour;
	outTime.wMinute = nMin;
	outTime.wSecond = nSec;
	outTime.wMilliseconds = nMiliSec;
	return outTime;
}

SYSTEMTIME toSysTime(time_t t)
{
#if 0
	tm temptm;
	localtime_s(&temptm, &t);

	SYSTEMTIME st = {1900 + temptm.tm_year,
		1 + temptm.tm_mon,
		temptm.tm_wday,
		temptm.tm_mday,
		temptm.tm_hour,
		temptm.tm_min,
		temptm.tm_sec,
		0};
	return st;
#else
	FILETIME ft;
	FILETIME ftLocal;
	SYSTEMTIME pst;
	LONGLONG nLL = Int32x32To64(t, 10000000) + 116444736000000000;
	ft.dwLowDateTime = (DWORD)nLL;
	ft.dwHighDateTime = (DWORD)(nLL >> 32);
	FileTimeToLocalFileTime(&ft, &ftLocal);
	FileTimeToSystemTime(&ftLocal, &pst);
	
	return pst;
#endif
} 

//好像不准 哦
//
//SYSTEMTIME Time_tToSystemTime(time_t t)
//{
//	tm temptm = *localtime(&t);
//	SYSTEMTIME st = {1900 + temptm.tm_year,
//		1 + temptm.tm_mon,
//		temptm.tm_wday,
//		temptm.tm_mday,
//		temptm.tm_hour,
//		temptm.tm_min,
//		temptm.tm_sec,
//		0};
//	return st;
//} 
//
//time_t SystemTimeToTime_t( const SYSTEMTIME& st )
//{
//	tm temptm = {st.wSecond,
//		st.wMinute,
//		st.wHour,
//		st.wDay,
//		st.wMonth - 1,
//		st.wYear - 1900,
//		st.wDayOfWeek,
//		0,
//		0};
//
//	return mktime(&temptm);
//}

//BOOL ChangeTimeByMiliSecond(SYSTEMTIME& sysTime, DWORD dwMiliSecond)
//{
//	if (dwMiliSecond == 0)
//	{
//		return TRUE;
//	}
//
//	FILETIME ft	= {0};
//
//	if(SystemTimeToFileTime(&sysTime, &ft) == FALSE)
//		return FALSE;
//
//	ULONGLONG qwResult;
//	// Copy the time into a quadword.
//	qwResult = (((ULONGLONG) ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
//	// minus 2 hours.
//
//	if (dwMiliSecond != 0)
//	{
//		qwResult = qwResult + dwMiliSecond * _MILLISECOND;
//	}
//
//	ft.dwLowDateTime = (DWORD)(qwResult & 0xFFFFFFFF);
//	ft.dwHighDateTime = (DWORD)(qwResult >> 32);
//
//	return FileTimeToSystemTime(&ft, &sysTime);
//}

std::string WString2String(const std::wstring& ws)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char *chDest = new char[nDestSize];
	memset(chDest,0,nDestSize);
	wcstombs(chDest,wchSrc,nDestSize);
	std::string strResult = chDest;
	delete []chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
}
// string => wstring
std::wstring String2WString(const std::string& s)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest,chSrc,nDestSize);
	std::wstring wstrResult = wchDest;
	delete []wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
}

std::string to_string(SYSTEMTIME *systime)
{			
	std::stringstream ss;
	ss << systime->wYear <<"-";
	
	ss.fill('0');			
	ss.width(2);	
	ss<< systime->wMonth << "-";
	
	ss.width(2);
	ss << systime->wDay <<" ";

	ss.width(2);
	ss << systime->wHour << ":";
	
	ss.width(2);
	ss << systime->wMinute << ":";
	
	ss.width(2);
	ss << systime->wSecond << ".";
	
	ss.width(3);
	ss << systime->wMilliseconds;
	return ss.str();
}
