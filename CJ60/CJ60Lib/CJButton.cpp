// CJButton.cpp : implementation file
//
// Owner drawn button control.
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
//			1.00	17 Oct 1998	- Initial re-write and release.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJButton

CCJButton::CCJButton()
{
	m_bLBtnDown  = false;
	m_bFlatLook  = true;
}

CCJButton::~CCJButton()
{
}

IMPLEMENT_DYNAMIC(CCJButton, CButton)

BEGIN_MESSAGE_MAP(CCJButton, CButton)
	//{{AFX_MSG_MAP(CCJButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJButton message handlers

void CCJButton::OnMouseMove(UINT nFlags, CPoint point)
{
	SetTimer (1,10,NULL);
	CButton::OnMouseMove(nFlags, point);
}

void CCJButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = true;
	CButton::OnLButtonDown(nFlags, point);
}

void CCJButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = false;
	CButton::OnLButtonUp(nFlags, point);
}

void CCJButton::OnTimer(UINT nIDEvent) 
{
	CRect rcItem;
	GetWindowRect(rcItem);

	CPoint ptCursor;
	GetCursorPos(&ptCursor);

	static bool pPainted = false;

	if (m_bLBtnDown==true) {
		KillTimer (1);

		if (pPainted == true) {
			InvalidateRect (NULL);
		}

		pPainted = false;
		return;
	}

	if (!rcItem.PtInRect(ptCursor)) {
		KillTimer (1);

		if (pPainted) {
			InvalidateRect (NULL);
		}

		pPainted = false;
		return;
	}

	// On mouse over, show raised button.
	else if(m_bFlatLook)
	{
		// Get the device context for the client area.
		CDC *pDC = GetDC();

		if (pPainted == false)
		{
			// repaint client area.
			GetClientRect(rcItem);
			pDC->FillSolidRect(rcItem, ::GetSysColor(COLOR_BTNFACE));

			// if an icon is associated with button, draw it.
			if( GetIcon() ) {
				SetIconRect();
				DrawIconEx( pDC->GetSafeHdc(), m_rcIcon.left, m_rcIcon.top,
					GetIcon(), m_rcIcon.Width(), m_rcIcon.Height(), NULL, (HBRUSH)NULL, DI_NORMAL ); 
			}
			
			// draw the button rect.
			pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),
				::GetSysColor(COLOR_BTNSHADOW));
			
			pPainted = true;
		}

		ReleaseDC (pDC);
	}

	CButton::OnTimer(nIDEvent);
}

void CCJButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct != NULL);	
	CDC* pDC = GetDC();
	
	m_nState = lpDrawItemStruct->itemState;

	// copy the rect, and fill the background.
	m_rcItem.CopyRect(&lpDrawItemStruct->rcItem);	
	pDC->FillSolidRect(m_rcItem, ::GetSysColor(COLOR_BTNFACE));

	// define rect to be used for left button down.
	SetIconRect();

	if(m_bFlatLook)
	{
		// Draw button pressed.
		if ((m_nState & ODS_SELECTED)) {
			pDC->Draw3dRect (m_rcItem, ::GetSysColor(COLOR_BTNSHADOW),
				::GetSysColor(COLOR_BTNHIGHLIGHT));
		}
		// Draw button flat.
		else {
			pDC->Draw3dRect (m_rcItem, ::GetSysColor(COLOR_BTNFACE),
				::GetSysColor(COLOR_BTNFACE));
		}
	}

	else
	{
		CRect rc(m_rcItem);
		rc.DeflateRect(1,1);

		// Draw button pressed.
		if ((m_nState & ODS_SELECTED)) {
			pDC->Draw3dRect (m_rcItem, ::GetSysColor(COLOR_3DDKSHADOW),
				::GetSysColor(COLOR_BTNHIGHLIGHT));
			pDC->Draw3dRect (rc, ::GetSysColor(COLOR_BTNSHADOW),
				::GetSysColor(COLOR_BTNFACE));
		}
		// Draw button raised.
		else {
			pDC->Draw3dRect (m_rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),
				::GetSysColor(COLOR_3DDKSHADOW));
			pDC->Draw3dRect (rc, ::GetSysColor(COLOR_BTNFACE),
				::GetSysColor(COLOR_BTNSHADOW));
		}
	}

	// Save the item state, set background to transparent.
	pDC->SetBkMode( TRANSPARENT );

	// if an icon is associated with button, draw it.
	if(GetIcon())
	{
		DrawIconEx (pDC->GetSafeHdc(), m_rcIcon.left, m_rcIcon.top,
			GetIcon(), m_rcIcon.Width(), m_rcIcon.Height(), NULL, (HBRUSH)NULL, DI_NORMAL);
	}

	ReleaseDC (pDC);
}

void CCJButton::SetIcon(HICON hIcon, CSize cSize)
{
	CButton::SetIcon(hIcon);
	m_cSize.cx = cSize.cx;
	m_cSize.cy = cSize.cy;
}

void CCJButton::SetIconRect()
{
	CRect rcClient;
	GetClientRect (&rcClient);

	CRect rcWindow;
	GetWindowRect (&rcWindow);

	CPoint ptCursor;
	GetCursorPos (&ptCursor);

	CPoint ptCenter = rcClient.CenterPoint();

	int x1 = (rcClient.Width()-ptCenter.x)/2-1;
	int x2 = x1+rcClient.Width()-1;
	int y1 = (rcClient.Height()-ptCenter.y)/2-1;
	int y2 = y1+rcClient.Height()-1;

	m_rcIcon.CopyRect (rcClient);

	if( m_bLBtnDown ) {
		if(rcWindow.PtInRect(ptCursor)) {
			m_rcIcon.OffsetRect(1,1);
		}
	}
}
