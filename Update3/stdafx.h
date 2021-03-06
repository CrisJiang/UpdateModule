// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "../DuiLib/UIlib.h"
using namespace DuiLib;
#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\lib\\DuiLib_d.lib")
#   else
#       pragma comment(lib, "..\\lib80\\debug\\DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\..\\lib\\DuiLib.lib")
#   else
#       pragma comment(lib, "..\\lib80\\Release\\DuiLib.lib")
#   endif
#endif

#include "../Common/curl/curl.h"
#ifdef _DEBUG
#pragma comment(lib, "../lib80/commlib/libcurl.lib")
#else
#pragma comment(lib, "../lib80/commlib/libcurl.lib")
#endif

#include "json/json.h"
#ifdef _DEBUG
#pragma comment(lib, "..\\lib80\\debug\\lib_json.lib")
#else	
#pragma comment(lib, "..\\lib80\\Release\\lib_json.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "..\\lib80\\debug\\zlib.lib")
#else	
#pragma comment(lib, "..\\lib80\\Release\\zlib.lib")
#endif



// TODO: 在此处引用程序需要的其他头文件
