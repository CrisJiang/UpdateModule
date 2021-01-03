#pragma  once

#include "stdafx.h"
#include <iostream>

class PageObj : public WindowImplBase{
	protected:
		virtual CDuiString GetSkinFile() { return m_sXml.c_str();}
		virtual CDuiString GetSkinFolder() { return "";}
		virtual LPCTSTR GetWindowClassName() const {return m_sClass.c_str();}
		virtual void InitWindow(){}
		virtual void Notify(TNotifyUI& msg) { return WindowImplBase::Notify(msg);}
		void SetParent(HWND _parent) { m_Parent = _parent; }
		
		virtual void SetCount(int num);
		virtual void KillTimer();
		virtual void CountPerSeg();
		virtual void CountEndEvt();
		
		std::string m_sClass;
		std::string m_sXml;
		HWND		m_Parent;
		int			nPageObjCountDown;

	public:
		PageObj(std::string sClass, std::string sXml, HWND hParent=NULL);
		virtual ~PageObj();
		virtual HWND GetSafeWnd() const{return this->m_hWnd;}
		HWND GetParent() { return m_Parent; }
		virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

};



