#include "firstpage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <tchar.h>
#include "Unzipper.h"
#include "tools.h"

const char cLabel_state[] = _T("textProgress");
const char cProcessBar_install[] = _T("install");

#define WM_INITWINDOW			WM_USER+3125
#define TIMER_LOGINING		10010
#define BTN_JLHZ_CLICK		1025
#define WM_PROGRESS_UPDATE	WM_USER+3177
#define WM_DOWNLOAD_ZIP     WM_USER+3178
#define WM_UNZIP			WM_USER+3179
#define  WM_START_EXE			WM_USER+3180

#define DST_VERSION_JSON "/static/version/"
#define DST_VERSION_PATH "/static/version/"

CFirstpage* g_pFirstpage = NULL;

string sVersionFileName = ""; //version+���� ���ļ���
string sZipFileName = "";	  //���صĸ��°��ļ���

//�ṩ��CURL���ؽ��Ȼص��ĺ��������ڱ������ص����ݵ��ļ�
static size_t	DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
//�ṩ��CURL���ؽ��Ȼص��ĺ��������ڼ������ؽ���֪ͨ����
static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

//�ṩ��CURL���ؽ��Ȼص��ĺ��������ڱ������ص����ݵ��ļ�
static size_t	DownloadCallbackZip(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
//�ṩ��CURL���ؽ��Ȼص��ĺ��������ڼ������ؽ���֪ͨ����
static int ProgressCallbackZip(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
//���ǶԻ������Ϣѭ�����ڿ���̨�������洴��GUI��������Ϊ�˸��õ�չ�����ػص��������
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//��������ص��̺߳�����Ϊ�˲��ѶԻ��������濨�����϶���Ҫ�Լ����߳������ص���
DWORD WINAPI DownloadThread(LPVOID lpParam);
DWORD WINAPI DownloadThreadZip(LPVOID lpParam);

std::vector<std::string> vStringSplit(const  std::string& s, const std::string& delim = ",")
{
	std::vector<std::string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}

string GetAppPath(HMODULE hdl)
{
	string path;
	path.resize(MAX_PATH);
	GetModuleFileName(hdl, &path[0], MAX_PATH);
	return path.substr(0, path.find_last_of('\\') + 1);
}

int ReadJsonFile(string &sRead, string sPath)
{
	std::ifstream t(sPath); //���ļ�ifstream,д�ļ�ofstream���ɶ���дfstream
	std::stringstream buffer;
	buffer << t.rdbuf();
	sRead = buffer.str();
	if (sRead.length() > 0)
		return 0;
	else
		return -1;
}

int GetLocalInfo(string &sVersion, string &sCity, string &sUrl)
{
	string stmp;
	ReadJsonFile(stmp, string(GetAppPath(NULL) + "./localinfo.json"));
	Json::Reader sIn;
	Json::Value value;
	sIn.parse(stmp, value);
	sVersion = value["version"].asString();
	sUrl = value["platform_url"].asString();
	sCity = value["city"].asString();
	return 0;
}

int SetLocalInfo(string &sVersion, string &sUrl)
{
	string stmp;
	string stmpCity;
	ReadJsonFile(stmp, string(GetAppPath(NULL) + "./localinfo.json"));
	Json::Reader sIn;
	Json::Value value;
	sIn.parse(stmp, value);
	sUrl = value["platform_url"].asString();
	stmpCity = value["city"].asString();

	Json::Value send;
	send["version"] = sVersion;
	send["platform_url"] = sUrl;
	send["city"] = stmpCity;

	Json::FastWriter writer;
	string sJson = writer.write(send);
	
	ofstream outFile(string(GetAppPath(NULL) + "./localinfo.json"));
	outFile.write(sJson.c_str(), sJson.length());
	return 0;
}

int CheckVersion(string &sVersionOld, string &sZipname)
{
	string stmp;
	ReadJsonFile(stmp, g_pFirstpage->m_strVersionDown);
	//ReadJsonFile(stmp, string(GetAppPath(NULL) + "./localinfo.json"));
	Json::Reader sIn;
	Json::Value value;
	bool isParse = sIn.parse(stmp, value);
	if (!isParse)
		return -1;
	string sVersionNew = value["version"].asString();
	g_pFirstpage->sNewVersion = sVersionNew;
	sZipname = value["zipname"].asString();

	string sub1 = sZipname.substr(0, sZipname.find('.'));
	string sub2 = sZipname.substr(sZipname.find('.'));
	
	sZipFileName = sub1 + "_" + g_pFirstpage->m_strCity + sub2;

	auto arrOld = vStringSplit(sVersionOld, ".");
	auto arrNew = vStringSplit(sVersionNew, ".");
	bool bIsNeedUpdate = false;
	for (int i = 0; i < arrOld.size(); i++) {
		int nOld = 0;
		int nNew = 0;
		stringToType(arrOld[i], nOld);
		stringToType(arrNew[i], nNew);
		if (nNew > nOld) {
			bIsNeedUpdate = true;
		}
	}

	if (bIsNeedUpdate) {
		return 1; //��Ҫ����
	}
	else
		return -1;// ����Ҫ����
}

BOOL IsPathExist(const string  csPath);
void ReadUpdateUrl()
{
	char sPlatDst[MAX_PATH] = { 0 };
	GetPrivateProfileString("OPTIONS", "PFC_URL", "", sPlatDst, MAX_PATH, "d:\\ELS_data\\StartPad.ini");
	string g_pkgPath = string(sPlatDst) + "update/update.exe";
	string downladPath = "d:\\update.exe";
	if (IsPathExist(downladPath))
	{
		remove(downladPath.c_str());
	}
	HANDLE hThread = CreateThread(NULL, 0, DownloadThread, 0, 0, NULL);
	CloseHandle(hThread);
}

// �ж��ļ����ļ����Ƿ����
BOOL IsPathExist(const string  csPath)
{
	//int nRet = _taccess(csPath.c_str(), 0);
	//return 0 == nRet || EACCES == nRet;
	return TRUE;
}


CFirstpage::CFirstpage(std::string sClass, std::string sXml)
				:PageObj(sClass, sXml),bfirstRun(true)
{

}

CFirstpage::~CFirstpage()
{

}

void CFirstpage::InitWindow()
{
	label_show = static_cast<CProgressUI*>(m_pm.FindControl(cLabel_state));
	progress_install = static_cast<CProgressUI*>(m_pm.FindControl(cProcessBar_install));

	progress_install->SetValue(0);
	g_pFirstpage = this;
}

void CFirstpage::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("windowinit"))
	{
		SendMessage(WM_INITWINDOW, NULL, NULL);
		return;
	}
	return PageObj::Notify(msg);
}

LRESULT CFirstpage::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;
	bHandled = FALSE;
	switch (uMsg)
	{
	case WM_INITWINDOW:
	{
		StartMainThread();
		SetCount(600);
		deq_Msg.push_back(_T("StartMain"));
		break;
	}
	case WM_PROGRESS_UPDATE:
	{
		string tmp = "�������ظ����ļ�..." + to_string((int)wParam) + "%";
		label_show->SetText(tmp.c_str());
		progress_install->SetValue((int)wParam);
		break;
	}
	case WM_DOWNLOAD_ZIP:
	{
		deq_Msg.push_back(_T("StartNext"));
		//StartNext();
		break;
	}
	case WM_UNZIP:
	{
		deq_Msg.push_back(_T("StartEnd"));
		//StartEnd();
		break;
	}
	case WM_START_EXE:
	{
		label_show->SetText(_T("���ڸ����ļ�..."));
		deq_Msg.push_back(_T("StartExe"));
		//StartExe();
		break;
	}

		
	}
	return 0;
}

int CFirstpage::GetSettingIni()
{
	string sIniPath = string(GetAppPath(NULL) + "setting.ini");
	char cOpenProgram[256] = {0};
	char cUnzipDist[256] = { 0 };
	char cProxyIp[256] = { 0 };
	char cProxyPort[256] = { 0 };
	//BOOL bIsProxy = FALSE;
	GetPrivateProfileString("Setting", "OpenProgram", "", cOpenProgram, 256, sIniPath.c_str());
	GetPrivateProfileString("Setting", "UnzipDst", "", cUnzipDist, 256, sIniPath.c_str());
	GetPrivateProfileString("Proxy", "ProxyIp", "", cProxyIp, 256, sIniPath.c_str());
	m_nProxyPort = GetPrivateProfileInt("Proxy", "ProxyPort", 0, sIniPath.c_str());
	m_bIsProxy = GetPrivateProfileInt("Proxy", "bIsProxy", 0, sIniPath.c_str());
	sOpenProgramPath = cOpenProgram;
	sUnzipPathG = cUnzipDist;
	m_strProxyIp = cProxyIp;
	return 0;
}

int CFirstpage::StartMainThread()
{
	HANDLE hThread = CreateThread(NULL, 0, mainThread, this, 0, NULL);
	return 0;
}

int CFirstpage::StartMain()
{
	GetSettingIni();
	::DeleteFile(string(GetAppPath(NULL) + "/unzip/version.json").c_str());
	::DeleteFile(string(GetAppPath(NULL) + "/unzip/zip.zip").c_str());
	label_show->SetText(_T("���ڼ���ļ�Ŀ¼..."));
	// 0 ��� Ŀ¼
	CUnzipper unzipper;
	unzipper.CreateFolder(string(GetAppPath(NULL) + "/unzip/").c_str());
	// 0-1 ���Ŀ¼
	label_show->SetText(_T("���ڼ�鱾�ذ汾..."));
	//1. ��ȡ������Ϣ ��������ַ ���ذ汾��
	GetLocalInfo(m_strVersionLocal, m_strCity, m_strPlatformUrl);

	sVersionFileName = string(DST_VERSION_JSON) + "version_" + m_strCity + ".json";
	
	m_strVersionDown = GetAppPath(NULL) + "\\unzip\\version.json";
	label_show->SetText(_T("���������ļ������б�..."));
	//2. ����Զ�˰汾�ļ�
	HANDLE hThread = CreateThread(NULL, 0, DownloadThread, this, 0, NULL);
	CloseHandle(hThread);
	//StartEnd();
	//progress_install->SetValue(50);
	return 0;
}

int CFirstpage::StartNext()
{
	label_show->SetText(_T("���ڼ�����..."));
	//3. ����Ƿ�Ҫ����
	if (CheckVersion(m_strVersionLocal, m_strZipName) == 1)
	{
		label_show->SetText(_T("��⵽����..."));
	}
	else
	{
		label_show->SetText(_T("û����Ҫ����..."));
		StartExe();
		return 0;
	}
		
	m_strZipDown = GetAppPath(NULL) + "/unzip/" + m_strZipName;
	label_show->SetText(_T("�������ظ����ļ�..."));
	//4. ���Ҫ����, ����Զ��ѹ�����ļ�
	HANDLE hThread = CreateThread(NULL, 0, DownloadThreadZip, this, 0, NULL);
	CloseHandle(hThread);
	//6. ִ��electron����
	return 0;
}

int CFirstpage::StartEnd()
{
	label_show->SetText(_T("������ȡ���°�..."));
#if _DEBUG
	//m_strZipDown = GetAppPath(NULL) + "unzip/zip.zip";
#endif
	string sUnzipPath = GetAppPath(NULL) + "resources\\app\\dist\\electron";
	if (sUnzipPathG.length() != 0)
		sUnzipPath = sUnzipPathG;
	
	//5. ��ѹ��ѹ�����ļ�
	CUnzipper unzipper;
	if (!unzipper.OpenZip(m_strZipDown.c_str()))
		return -1;
	if (!unzipper.UnzipTo(sUnzipPath.c_str()))
		return -2;
	label_show->SetText(_T("���ڸ����ļ�..."));

	// 
	string sUrl;
	sUrl = "";
	SetLocalInfo(sNewVersion, sUrl);
	::UpdateWindow(g_pFirstpage->GetSafeWnd());
	::PostMessage(this->GetSafeWnd(), WM_START_EXE, NULL, NULL);
	return 0;
}

int CFirstpage::StartExe()
{
	label_show->SetText(_T("Ӧ��������,���Ժ�..."));
	progress_install->SetValue(100);
	ShellExecute(NULL, "open", sOpenProgramPath.c_str(), NULL, NULL, SW_NORMAL);
	return 0;
}

static size_t DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	//�����ص���������׷�ӵķ�ʽд���ļ�(һ��Ҫ��a������ǰ��д������ݾͻᱻ������)
	FILE* fp = NULL;
	fopen_s(&fp, g_pFirstpage->m_strVersionDown.c_str(), "ab+");
	size_t nWrite = fwrite(pBuffer, nSize, nMemByte, fp);
	fclose(fp);
	return nWrite;
}

bool gDownload_json = false;
bool gDownload_zip = false;

static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (dltotal > -0.1 && dltotal < 0.1)
		return 0;
	int nPos = (int)((dlnow / dltotal) * 100);
	//֪ͨ�������������ؽ���
	//::PostMessage(g_pFirstpage->GetHWND(), WM_PROGRESS_UPDATE, nPos, 0);
	//::Sleep(10);
	if (nPos >= 100) 
	{
		if (!gDownload_json)
		{
			gDownload_json = true;
			::Sleep(500);
			::PostMessage(g_pFirstpage->GetHWND(), WM_DOWNLOAD_ZIP, nPos, 0);
		}
	}
	/*if (nPos >= 100)
	{
		if (!gInstall)
		{
			gInstall = true;
			string sCmd = string("/c ") + "d:\\update.exe /silent";
			ShellExecute(NULL, "open", _T("cmd"), sCmd.c_str(), NULL, SW_HIDE);
			g_pFirstpage->Close();
		}

	}*/
	return 0;
}

static size_t DownloadCallbackZip(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	//�����ص���������׷�ӵķ�ʽд���ļ�(һ��Ҫ��a������ǰ��д������ݾͻᱻ������)
	FILE* fp = NULL;
	fopen_s(&fp, g_pFirstpage->m_strZipDown.c_str(), "ab+");
	size_t nWrite = fwrite(pBuffer, nSize, nMemByte, fp);
	fclose(fp);
	return nWrite;
}

static int ProgressCallbackZip(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (dltotal > -0.1 && dltotal < 0.1)
		return 0;
	int nPos = (int)((dlnow / dltotal) * 100);
	//֪ͨ�������������ؽ���
	::SendMessage(g_pFirstpage->GetHWND(), WM_PROGRESS_UPDATE, nPos, 0);
	::Sleep(10);
	if (nPos >= 100)
	{
		if (!gDownload_zip)
		{
			gDownload_zip = true;
			::Sleep(500);
			::SendMessage(g_pFirstpage->GetHWND(), WM_UNZIP, nPos, 0);
		}
	}
	/*if (nPos >= 100)
	{
		if (!gInstall)
		{
			gInstall = true;
			string sCmd = string("/c ") + "d:\\update.exe /silent";
			ShellExecute(NULL, "open", _T("cmd"), sCmd.c_str(), NULL, SW_HIDE);
			g_pFirstpage->Close();
		}

	}*/
	return 0;
}

DWORD WINAPI mainThread(LPVOID lpParam)
{
	CFirstpage* p = (CFirstpage*)lpParam;

	HANDLE hProcess = CreateEvent(NULL, FALSE, TRUE, NULL);

	while (true)
	{
		DWORD dw = WaitForSingleObject(hProcess, 200);
		if (dw == WAIT_OBJECT_0 || dw == WAIT_TIMEOUT)
		{
			if (p->deq_Msg.size() != 0)
			{
				string s = p->deq_Msg.front();

				if (s.compare(_T("StartMain")) == 0)
				{
					p->StartMain();
				}
				else if (s.compare(_T("StartNext")) == 0)
				{
					p->StartNext();
				}
				else if (s.compare(_T("StartEnd")) == 0)
				{
					p->StartEnd();
				}
				else if (s.compare(_T("StartExe")) == 0)
				{
					p->StartExe();
				}
				else if (s.compare(_T("Close")) == 0)
				{
					break;
				}
				p->deq_Msg.pop_front();
			}
		}
	}
	return 0;
}



DWORD WINAPI DownloadThread(LPVOID lpParam)
{
	CFirstpage* pFirstpage = (CFirstpage*)lpParam;
	//Sleep(10000);
	//��ʼ��curl������Ǳ����
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, string(pFirstpage->m_strPlatformUrl+ sVersionFileName).c_str());
	//���ý������ݵĻص�
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadCallback);
	// �����ض����������
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// ����301��302��ת����location
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 7777);
	//���ý��Ȼص�����
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	if (pFirstpage->m_bIsProxy)
	{
		curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC); //������֤ģʽ
		curl_easy_setopt(curl, CURLOPT_PROXY, pFirstpage->m_strProxyIp.c_str()); //�����������ַ
		curl_easy_setopt(curl, CURLOPT_PROXYPORT, pFirstpage->m_nProxyPort); //����������˿�
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP); //ʹ��http����ģʽ
	}
	//curl_easy_getinfo(curl,  CURLINFO_CONTENT_LENGTH_DOWNLOAD, &lFileSize);
	//curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, g_hDlgWnd);
	//��ʼִ������
	CURLcode retcCode = curl_easy_perform(curl);
	//�鿴�Ƿ��г�����Ϣ
	const char* pError = curl_easy_strerror(retcCode);
	//����curl����ǰ��ĳ�ʼ��ƥ��
	curl_easy_cleanup(curl);
	if (retcCode != CURLE_OK)
	{
		pFirstpage->label_show->SetText(pError);
		Sleep(1000);
		pFirstpage->deq_Msg.push_back(_T("StartExe"));
	}
	//p_mainWnd->Close();
	return 0;
}



DWORD WINAPI DownloadThreadZip(LPVOID lpParam)
{
	CFirstpage* pFirstpage = (CFirstpage*)lpParam;
	//Sleep(10000);
	//��ʼ��curl������Ǳ����
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, string(pFirstpage->m_strPlatformUrl + DST_VERSION_PATH + sZipFileName).c_str());
	//���ý������ݵĻص�
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadCallbackZip);
	// �����ض����������
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// ����301��302��ת����location
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	//���ý��Ȼص�����
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallbackZip);
	if (pFirstpage->m_bIsProxy)
	{
		curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC); //������֤ģʽ
		curl_easy_setopt(curl, CURLOPT_PROXY, pFirstpage->m_strProxyIp.c_str()); //�����������ַ
		curl_easy_setopt(curl, CURLOPT_PROXYPORT, pFirstpage->m_nProxyPort); //����������˿�
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP); //ʹ��http����ģʽ
	}
	//curl_easy_getinfo(curl,  CURLINFO_CONTENT_LENGTH_DOWNLOAD, &lFileSize);
	//curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, g_hDlgWnd);
	//��ʼִ������
	CURLcode retcCode = curl_easy_perform(curl);
	//�鿴�Ƿ��г�����Ϣ
	const char* pError = curl_easy_strerror(retcCode);
	//����curl����ǰ��ĳ�ʼ��ƥ��
	curl_easy_cleanup(curl);
	if (retcCode != CURLE_OK)
	{
		pFirstpage->label_show->SetText(pError);
		Sleep(1000);
		pFirstpage->deq_Msg.push_back(_T("StartExe"));
	}
	//p_mainWnd->Close();
	return 0;
}




