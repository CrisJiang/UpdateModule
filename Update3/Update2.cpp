//Update2.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Update2.h"
#include "firstpage.h"
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

//#define IDR_ZIPRES1                     101

/*[CrisJiang 2018/05/18]
* 打包ZIP到exe程序中
case UILIB_ZIPRESOURCE:*/
void InitResource()
{
	// 资源类型
	CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
	// 资源路径
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	// 加载资源
	switch (CPaintManagerUI::GetResourceType())
	{
	case UILIB_FILE:
	{
		strResourcePath += _T("skin\\ADMonSetup\\");
		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
		// 加载资源管理器
		CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
		break;
	}
	case UILIB_RESOURCE:
	{
		strResourcePath += _T("skin\\ADMonSetup\\");
		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
		// 加载资源管理器
		CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
		break;
	}
	case UILIB_ZIP:
	{
		strResourcePath += _T("skin\\");
		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
		CPaintManagerUI::SetResourceZip(_T("ADMonSetup.zip"), true);
		// 加载资源管理器
		CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
		break;
	}

	case UILIB_ZIPRESOURCE:
	{
		strResourcePath += _T("\\res");
		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());

		HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), MAKEINTRESOURCE(IDR_ZIPRES1), _T("ZIPRES"));
		if (hResource == NULL)
		{
			//CString strtmp;
			//strtmp.Format("[ExtractFile] Locate Resource Error: %d\n", GetLastError());
			return;
		}

		if (hResource != NULL) {
			DWORD dwSize = 0;
			HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
			if (hGlobal != NULL) {
				dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
				if (dwSize > 0) {
					CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
					// 加载资源管理器
					//CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
				}
			}
			::FreeResource(hResource);
		}
	}
	break;
	}
}

BOOL IsProgrameRunning(const char* m_pszAppName, const char* m_pszExeName)
{
	HANDLE hSem = CreateSemaphore(NULL, 1, 1, m_pszAppName);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hSem);
		HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while (::IsWindow(hWndPrevious))
		{
			if (::GetProp(hWndPrevious, m_pszExeName))
			{
				if (::IsIconic(hWndPrevious))
					::ShowWindow(hWndPrevious, SW_RESTORE);

				::SetForegroundWindow(hWndPrevious);

				::SetForegroundWindow(
					::GetLastActivePopup(hWndPrevious));

				return TRUE;
			}
			hWndPrevious = ::GetWindow(hWndPrevious, GW_HWNDNEXT);
		}
		return TRUE;
	}
	return FALSE;
}

CFirstpage *p_mainWnd;
string g_pkgPath;

//这个用来保存对话框的窗口句柄，因为后面要向这个窗口发消息，必须知道其窗口句柄
HWND	g_hDlgWnd = NULL;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (IsProgrameRunning("KWTupdate", "KWTupdate"))
		return 0;
#ifndef _DEBUG
	InitResource();
#endif

	CPaintManagerUI::SetInstance(hInstance);

	CDuiString str = CPaintManagerUI::GetInstancePath();
	str += _T("\\lang");
	CPaintManagerUI::SetResourcePath(str);
	p_mainWnd = new CFirstpage(_T("CFirstPage"), _T("updatepage.xml"));
	p_mainWnd->Create(NULL, NULL, WS_POPUP, 0, 0, 0, 480, 800);
	p_mainWnd->CenterWindow();
	//ReadUpdateUrl();
	p_mainWnd->ShowModal();

	return 0;
}