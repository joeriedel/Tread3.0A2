// CJPagerCtrl.cpp : implementation file
//
// Copyright © 1998 Written by Kirk Stowell   
//		mailto:kstowel@sprynet.com
//		http://www.geocities.com/SiliconValley/Haven/8230
//
// This code may be used in compiled form in any way you desire. This  
// file may be redistributed unmodified by any means PROVIDING it is   
// not sold for profit without the authors written consent, and   
// providing that this notice and the authors name and all copyright   
// notices remains intact. If the source code in this file is used in   
// any  commercial application then a statement along the lines of   
// "Portions Copyright © 1998 Kirk Stowell" must be included in   
// the startup banner, "About" box or printed documentation. An email   
// letting me know that you are using it would be nice as well. That's   
// not much to ask considering the amount of work that went into this.  
//  
// This file is provided "as is" with no expressed or implied warranty.  
// The author accepts no liability for any damage/loss of business that  
// this product may cause.  
//
// ==========================================================================
// HISTORY:	
// ==========================================================================
//			1.00	02 Oct 1998	- First release version.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJPagerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJPagerCtrl

CCJPagerCtrl::CCJPagerCtrl()
{
    if (bInitialized==false) {
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_PAGESCROLLER_CLASS|ICC_BAR_CLASSES;
        ::InitCommonControlsEx(&icex);
        bInitialized = true;
    }
}

CCJPagerCtrl::~CCJPagerCtrl()
{
}

BEGIN_MESSAGE_MAP(CCJPagerCtrl, CWnd)
	//{{AFX_MSG_MAP(CCJPagerCtrl)
	ON_NOTIFY_REFLECT_EX(PGN_SCROLL,	OnPagerScroll)
	ON_NOTIFY_REFLECT_EX(PGN_CALCSIZE,	OnPagerCalcSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CCJPagerCtrl::bInitialized = false;

BOOL CCJPagerCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
    return CWnd::Create(WC_PAGESCROLLER, _T(""),
		dwStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// CCJPagerCtrl message handlers

void CCJPagerCtrl::SetChild(HWND hWnd)
{
	Pager_SetChild(m_hWnd, hWnd);
}

void CCJPagerCtrl::RecalcSize()
{
	Pager_RecalcSize(m_hWnd);
}

void CCJPagerCtrl::ForwardMouse(bool bForward)
{
	Pager_ForwardMouse(m_hWnd,bForward);
}

COLORREF CCJPagerCtrl::SetBkColor(COLORREF clr)
{
	return Pager_SetBkColor(m_hWnd,clr);
}

COLORREF CCJPagerCtrl::GetBkColor()
{
	return Pager_GetBkColor(m_hWnd);
}

int CCJPagerCtrl::SetBorder(int iBorder)
{
	return Pager_SetBorder(m_hWnd,iBorder);
}

int CCJPagerCtrl::GetBorder()
{
	return Pager_GetBorder(m_hWnd);
}

int CCJPagerCtrl::SetPos(int iPos)
{
	return Pager_SetPos(m_hWnd,iPos);
}

int CCJPagerCtrl::GetPos()
{
	return Pager_GetPos(m_hWnd);
}

int CCJPagerCtrl::SetButtonSize(int iSize)
{
	return Pager_SetButtonSize(m_hWnd,iSize);
}

int CCJPagerCtrl::GetButtonSize()
{
	return Pager_GetButtonSize(m_hWnd);
}

DWORD CCJPagerCtrl::GetButtonState(int iButton)
{
	return Pager_GetButtonState(m_hWnd,iButton);
}

BOOL CCJPagerCtrl::OnPagerCalcSize(NMHDR * arg, LRESULT* pResult)
{
	NMPGCALCSIZE* pNMPGCalcSize = reinterpret_cast<NMPGCALCSIZE*>(arg);
    *pResult = 0;

	switch(pNMPGCalcSize->dwFlag)
    {
	case PGF_CALCWIDTH:
		pNMPGCalcSize->iWidth = m_nWidth;
		break;
		
	case PGF_CALCHEIGHT:
		pNMPGCalcSize->iHeight = m_nHeight;
        break;
	}
	
	return 0;
}

BOOL CCJPagerCtrl::OnPagerScroll(NMHDR * arg, LRESULT* pResult)
{
	NMPGSCROLL* pNMPGScroll = reinterpret_cast<NMPGSCROLL*>(arg);
    *pResult = 0;
	
  	switch(pNMPGScroll->iDir)
	{
	case PGF_SCROLLLEFT:
	case PGF_SCROLLRIGHT:
	case PGF_SCROLLUP:
	case PGF_SCROLLDOWN:
        break;
	}
	return 0;
}
