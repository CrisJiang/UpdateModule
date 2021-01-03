#include "PageObj.h"

#define PAGEOBJ_TIMER_COUNTDOWN 23457
#define WM_PAGE_SHOW		23498
#define WM_PAGE_HIDE		23499

PageObj::PageObj(std::string sClass, std::string sXml, HWND hParent)
{
	m_sClass = sClass;
	m_sXml = sXml;
	m_Parent = hParent;
}

PageObj::~PageObj()
{

}

LRESULT PageObj::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;
	bHandled = FALSE;
	switch (uMsg)
	{
		case WM_TIMER:
			{
				if (wParam == PAGEOBJ_TIMER_COUNTDOWN)
				{
					if (nPageObjCountDown != 0) {
						nPageObjCountDown--;
						CountPerSeg();
					}
					else {
						CountEndEvt();
					}
					
				}
				break;
			}
		case WM_PAGE_SHOW:
			{
				this->ShowWindow(true);
				UpdateWindow(GetSafeWnd());
			}
			break;

		case WM_PAGE_HIDE:
			{
				::KillTimer(GetSafeWnd(), PAGEOBJ_TIMER_COUNTDOWN);
				this->ShowWindow(false);
			}
			break;
	}
	return 0;
}

void PageObj::SetCount(int num)
{
	nPageObjCountDown = num;
	::KillTimer(GetSafeWnd(), PAGEOBJ_TIMER_COUNTDOWN);
	::SetTimer(GetSafeWnd(), PAGEOBJ_TIMER_COUNTDOWN, 1000 , NULL);	
}

void PageObj::KillTimer()
{
	::KillTimer(GetSafeWnd(), PAGEOBJ_TIMER_COUNTDOWN);
}

void PageObj::CountPerSeg()
{

}

void PageObj::CountEndEvt()
{
	::KillTimer(GetSafeWnd(), PAGEOBJ_TIMER_COUNTDOWN);
}
