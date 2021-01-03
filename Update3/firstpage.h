#pragma once
#include "PageObj.h"

class  CFirstpage : public PageObj {

	protected:
		virtual void InitWindow();
		virtual void Notify(TNotifyUI& msg);
		bool OnItemClick(TNotifyUI& msg);
		bool OnBtnJlhzClick();
		LRESULT OnNetBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		
		int GetSettingIni();
		void	AnimaStart();
		bool				bfirstRun;

	public:
		CFirstpage(std::string sClass, std::string sXml);
		~CFirstpage();

		string m_strProxyIp;
		int m_nProxyPort;
		bool	m_bIsProxy;

		deque<string> deq_Msg;

		string m_strVersionLocal;
		string m_strCity;
		string m_strPlatformUrl;
		string m_strVersionDown;
		string m_strZipName;
		string m_strZipDown;
		CLabelUI			*label_show;
		CProgressUI			*progress_install;		//·Ã¿Í×¢²á
		int StartMainThread();
		int StartMain();
		int StartNext();
		int StartEnd();
		int StartExe();

		string sOpenProgramPath;
		string sUnzipPathG;
		string sNewVersion;
};
DWORD WINAPI mainThread(LPVOID lpParam);