// CJComboBox.cpp : implementation file
// 
// Copyright © 1998 Kirk Stowell 
// All Rights Reserved. 
// 
// Email: kstowel@sprynet.com 
// URL:   www.geocities.com/SiliconValley/Haven/8230 
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
// Expect bugs!
// 
// Please use and enjoy, and let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into 
// this file. 
//
// ==========================================================================
// HISTORY:	
// ==========================================================================
//			1.00	02 Oct 1998	- First release version.
//			1.01	05 Oct 1998	- Fixed offset bug to use GetSystemMetrics(),
//								  instead of hard coded values -
//								  (Todd Brannam).
//			1.02	02 Nov 1998 - Added OnSetFocus(), OnKillFocus() and
//								  modified OnPaint() so the control remains
//								  beveled until it actually looses focus -
//								  (Vachik Hovhannissian).
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJComboBox

CCJComboBox::CCJComboBox()
{
	m_bLBtnDown  = false;
}

CCJComboBox::~CCJComboBox()
{
}

IMPLEMENT_DYNAMIC(CCJComboBox, CComboBox)

BEGIN_MESSAGE_MAP(CCJComboBox, CComboBox)
	//{{AFX_MSG_MAP(CCJComboBox)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJComboBox message handlers

void CCJComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	SetTimer(1,10,NULL);
	CComboBox::OnMouseMove(nFlags, point);
}

void CCJComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = true;
	CComboBox::OnLButtonDown(nFlags, point);
}

void CCJComboBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = false;
	Invalidate();
	CComboBox::OnLButtonUp(nFlags, point);
}

void CCJComboBox::OnTimer(UINT nIDEvent) 
{
	POINT pt;
	GetCursorPos(&pt);
	CRect rcItem;
	GetWindowRect(&rcItem);

	static bool bPainted = false;

	// OnLButtonDown, show pressed.
	if (m_bLBtnDown==true) {
		KillTimer (1);
		if (bPainted == true) {
			DrawCombo(FC_DRAWPRESSD, ::GetSysColor(COLOR_BTNSHADOW),
				::GetSysColor(COLOR_BTNHIGHLIGHT));
			bPainted = false;
		}
		return;
	}

	// If mouse leaves, show flat.
	if (!rcItem.PtInRect(pt)) {
		KillTimer (1);
		if (bPainted == true) {
			DrawCombo(FC_DRAWNORMAL, ::GetSysColor(COLOR_BTNFACE),
				::GetSysColor(COLOR_BTNFACE));
			bPainted = false;
		}
		return;
	}

	// On mouse over, show raised.
	else {
		if (bPainted == true)
			return;
		else {
			bPainted = true;
			DrawCombo(FC_DRAWRAISED, ::GetSysColor(COLOR_BTNSHADOW),
				::GetSysColor(COLOR_BTNHIGHLIGHT));
		}
	}
	
	CComboBox::OnTimer(nIDEvent);
}

void CCJComboBox::OnPaint() 
{
	ModifyStyleEx (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE,
		0, SWP_FRAMECHANGED);
	
	Default();

	CPoint pt;
	GetCursorPos(&pt);

	CRect rcItem;
	GetWindowRect(&rcItem);

	if ((rcItem.PtInRect(pt)) || (::GetFocus() == m_hWnd))
		DrawCombo(FC_DRAWRAISED, ::GetSysColor(COLOR_BTNSHADOW),
			::GetSysColor(COLOR_BTNHIGHLIGHT));
	else
		DrawCombo(FC_DRAWNORMAL, ::GetSysColor(COLOR_BTNFACE),
			::GetSysColor(COLOR_BTNFACE));
}

void CCJComboBox::DrawCombo(DWORD dwStyle, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	CRect rcItem;
	GetClientRect(&rcItem);
	CDC* pDC = GetDC();
	
	// Cover up dark 3D shadow.
	pDC->Draw3dRect(rcItem, clrTopLeft, clrBottomRight);
	rcItem.DeflateRect(1,1);
	
	if (!IsWindowEnabled()) {
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),
			::GetSysColor(COLOR_BTNHIGHLIGHT));
	}
	
	else {
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE),
			::GetSysColor(COLOR_BTNFACE));
	}

	// Cover up dark 3D shadow on drop arrow.
	rcItem.DeflateRect(1,1);
	rcItem.left = rcItem.right-Offset();
	pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE),
		::GetSysColor(COLOR_BTNFACE));
	
	// Cover up normal 3D shadow on drop arrow.
	rcItem.DeflateRect(1,1);
	pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE),
		::GetSysColor(COLOR_BTNFACE));
	
	if (!IsWindowEnabled()) {
		return;
	}

	switch (dwStyle)
	{
	case FC_DRAWNORMAL:
		rcItem.top -= 1;
		rcItem.bottom += 1;
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),
			::GetSysColor(COLOR_BTNHIGHLIGHT));
		rcItem.left -= 1;
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),
			::GetSysColor(COLOR_BTNHIGHLIGHT));
		break;

	case FC_DRAWRAISED:
		rcItem.top -= 1;
		rcItem.bottom += 1;
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),
			::GetSysColor(COLOR_BTNSHADOW));
		break;

	case FC_DRAWPRESSD:
		rcItem.top -= 1;
		rcItem.bottom += 1;
		rcItem.OffsetRect(1,1);
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNSHADOW),
			::GetSysColor(COLOR_BTNHIGHLIGHT));
		break;
	}

	ReleaseDC(pDC);
}

int CCJComboBox::Offset()
{
	return ::GetSystemMetrics(SM_CXHTHUMB);
}

void CCJComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CComboBox::OnSetFocus(pOldWnd);
	DrawCombo(FC_DRAWRAISED, ::GetSysColor(COLOR_BTNSHADOW),
		::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void CCJComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	DrawCombo(FC_DRAWNORMAL, ::GetSysColor(COLOR_BTNFACE),
		::GetSysColor(COLOR_BTNFACE));	
}
