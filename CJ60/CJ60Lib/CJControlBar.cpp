// CJControlBar.cpp : implementation file
//  
// DevStudio Style Resizable Docking Control Bar.  
//  
// Copyright © 1998 Written by Kirk Stowell 
//		mailto:kstowell@codejockeys.com 
//		http://www.codejockeys.com/kstowell/index.html 
//  
// The code contained in this file is based on the original  
// CSizingControlBar class written by Cristi Posea,  
//		mailto:cristi@gds.ro  
//		http://www.codeguru.com/docking/docking_window2.shtml  
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
//			1.01	20 Oct 1998	- Fixed problem with gripper and buttons  
//								  disappearing when docking toggled. Overloaded  
//								  IsFloating() method from base class.  
//			1.02    22 Nov 1998 - Modified set cursor to display normal size
//								  cursor when static linked.
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJControlBar.h"
#include <afxpriv.h>    // for CDockContext

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJControlBar

#define IDC_BUTTON_HIDE 1000
#define IDC_BUTTON_MINI 1001

CCJControlBar::CCJControlBar()
{
    m_sizeMin		= CSize(32, 32);
    m_sizeHorz		= CSize(200, 200);
    m_sizeVert		= CSize(200, 200);
    m_sizeFloat		= CSize(200, 200);
    m_bTracking		= FALSE;
    m_bInRecalcNC	= FALSE;
    m_cxEdge		= 5;
	m_menuID		= 0;
	m_bGripper		= TRUE;
	m_bButtons		= FALSE;
}

CCJControlBar::~CCJControlBar()
{
}

IMPLEMENT_DYNAMIC(CCJControlBar, CControlBar)

BEGIN_MESSAGE_MAP(CCJControlBar, CControlBar)
	//{{AFX_MSG_MAP(CCJControlBar)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCHITTEST()
	ON_WM_NCCALCSIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_COMMAND(IDC_BUTTON_HIDE, OnButtonClose)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_HIDE, OnUpdateButtonClose)
	ON_COMMAND(IDC_BUTTON_MINI, OnButtonMinimize)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_MINI, OnUpdateButtonMinimize)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJControlBar message handlers

void CCJControlBar::OnUpdateCmdUI(CFrameWnd * pTarget, BOOL bDisableIfNoHndler)
{
    UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

BOOL CCJControlBar::Create(CWnd * pParentWnd, UINT nID, LPCTSTR lpszWindowName, CSize sizeDefault, DWORD dwStyle)
{
    ASSERT_VALID(pParentWnd);   // must have a parent
    ASSERT (((dwStyle & CBRS_SIZE_FIXED)   != CBRS_SIZE_FIXED) && 
	    	((dwStyle & CBRS_SIZE_DYNAMIC) != CBRS_SIZE_DYNAMIC));

    // save the style
    SetBarStyle(dwStyle & CBRS_ALL);

    CString wndclass = ::AfxRegisterWndClass(CS_DBLCLKS,
        ::LoadCursor(NULL, IDC_ARROW),
        ::GetSysColorBrush(COLOR_BTNFACE), 0);

    m_sizeHorz = m_sizeVert = m_sizeFloat = sizeDefault;

    dwStyle &= ~CBRS_ALL;
    dwStyle &= WS_VISIBLE | WS_CHILD;
    
	return CWnd::Create(wndclass, lpszWindowName, dwStyle,
		CRect(0,0,0,0), pParentWnd, nID);
}

CSize CCJControlBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
    CRect rc;

    m_pDockSite->GetControlBar(AFX_IDW_DOCKBAR_TOP)->GetWindowRect(rc);
    int nHorzDockBarWidth = bStretch ? 32767 : rc.Width() + 4;
    m_pDockSite->GetControlBar(AFX_IDW_DOCKBAR_LEFT)->GetWindowRect(rc);
    int nVertDockBarHeight = bStretch ? 32767 : rc.Height() + 4;

    if (bHorz)
        return CSize(nHorzDockBarWidth, m_sizeHorz.cy);
    else
        return CSize(m_sizeVert.cx, nVertDockBarHeight);
}

CSize CCJControlBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
    if (dwMode & (LM_HORZDOCK | LM_VERTDOCK))
    {
        if (nLength == -1)
            GetDockingFrame()->DelayRecalcLayout();
        return CControlBar::CalcDynamicLayout(nLength,dwMode);
    }

    if (dwMode & LM_MRUWIDTH)
        return m_sizeFloat;

    if (dwMode & LM_COMMIT)
    {
        m_sizeFloat.cx = nLength;
        return m_sizeFloat;
    }

    if (dwMode & LM_LENGTHY)
        return CSize(m_sizeFloat.cx,
            m_sizeFloat.cy = max(m_sizeMin.cy, nLength));
    else
        return CSize(max(m_sizeMin.cx, nLength), m_sizeFloat.cy);
}

void CCJControlBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CControlBar::OnWindowPosChanged(lpwndpos);
	
    // Find on which side are we docked
	m_nDockBarID = GetParent()->GetDlgCtrlID();

    if (m_bInRecalcNC == FALSE)
	{
        m_bInRecalcNC = TRUE;

        // Force recalc the non-client area
        SetWindowPos(NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE |
            SWP_NOACTIVATE | SWP_NOZORDER |
            SWP_FRAMECHANGED);

        m_bInRecalcNC = FALSE;
    }

	if (m_bButtons)
	{
		ASSERT(m_ImageList);

		if (IsFloating()) {
			m_btnClose.ShowWindow(SW_HIDE);
			m_btnMinim.ShowWindow(SW_HIDE);
			return;
		}
		else {
			m_btnClose.ShowWindow(SW_SHOW);
			m_btnMinim.ShowWindow(SW_SHOW);
		}

		CRect rcClose(GetButtonRect());
		CRect rcMinim(GetButtonRect());

		if (IsHorzDocked()) {
			rcMinim.OffsetRect(0,14);
			m_btnMinim.SetIcon(m_ImageList->ExtractIcon(2),CSize(13,13));
		}
		else {
			rcClose.OffsetRect(14,0);
			m_btnMinim.SetIcon(m_ImageList->ExtractIcon(1),CSize(13,13));
		}

		m_btnClose.MoveWindow(rcClose);
		m_btnMinim.MoveWindow(rcMinim);
	}

	Invalidate();
}

BOOL CCJControlBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    if ((nHitTest != HTSIZE) || m_bTracking)
        return CControlBar::OnSetCursor(pWnd, nHitTest, message);

#if defined(MFCXLIB_STATIC) 
	HINSTANCE hInst = AfxFindResourceHandle(
			MAKEINTRESOURCE(AFX_IDC_VSPLITBAR), RT_GROUP_CURSOR);

    if (IsHorzDocked())
        ::SetCursor(::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_VSPLITBAR)));
    else
        ::SetCursor(::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_HSPLITBAR)));
#else
    if (IsHorzDocked())
        ::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
    else
        ::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
#endif
    return TRUE;
}

BOOL CCJControlBar::IsHorzDocked() const
{
    return (m_nDockBarID == AFX_IDW_DOCKBAR_TOP ||
        m_nDockBarID == AFX_IDW_DOCKBAR_BOTTOM);
}

BOOL CCJControlBar::IsVertDocked() const
{
    return (m_nDockBarID == AFX_IDW_DOCKBAR_LEFT ||
        m_nDockBarID == AFX_IDW_DOCKBAR_RIGHT);
}

void CCJControlBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
    // Compute the rectangle of the mobile edge
    GetWindowRect(m_rectBorder);
    m_rectBorder.OffsetRect(-m_rectBorder.left, -m_rectBorder.top);
    m_rectBorder.DeflateRect(1,1);
    
    CRect rcWnd = lpncsp->rgrc[0];
    DWORD dwBorderStyle = m_dwStyle | CBRS_BORDER_ANY;

    switch (m_nDockBarID)
    {
    case AFX_IDW_DOCKBAR_TOP:
		{
			dwBorderStyle &= ~CBRS_BORDER_BOTTOM;
			rcWnd.DeflateRect(2, 2, 2, m_cxEdge + 2);
			m_rectBorder.top = m_rectBorder.bottom - m_cxEdge;
			break;
		}
    case AFX_IDW_DOCKBAR_BOTTOM:
		{
			dwBorderStyle &= ~CBRS_BORDER_TOP;
			rcWnd.DeflateRect(2, m_cxEdge + 2, 2, 2);
			m_rectBorder.bottom = m_rectBorder.top + m_cxEdge;
			lpncsp->rgrc[0].left += 10;
			break;
		}
    case AFX_IDW_DOCKBAR_LEFT:
		{
			dwBorderStyle &= ~CBRS_BORDER_RIGHT;
			rcWnd.DeflateRect(2, 2, m_cxEdge + 2, 6);
			m_rectBorder.left = m_rectBorder.right - m_cxEdge;
			break;
		}
    case AFX_IDW_DOCKBAR_RIGHT:
		{
			dwBorderStyle &= ~CBRS_BORDER_LEFT;
			rcWnd.DeflateRect(m_cxEdge + 2, 2, 2, 6);
			m_rectBorder.right = m_rectBorder.left + m_cxEdge;
			break;
		}
    default:
		{
			m_rectBorder.SetRectEmpty();
			break;
		}
    }

    lpncsp->rgrc[0] = rcWnd;

    SetBarStyle(dwBorderStyle);
}

void CCJControlBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DrawGripper(&dc);
}

void CCJControlBar::OnNcPaint() 
{
    // get window DC that is clipped to the non-client area
    CWindowDC dc(this);

	CRect rectWindow;
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);

	CRect rectClient;
	GetClientRect(rectClient);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.ExcludeClipRect(rectClient);
	
    // draw borders in non-client area
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	DrawBorders(&dc, rectWindow);
	
    // erase parts not drawn
	dc.IntersectClipRect(rectWindow);
	
    // erase NC background the hard way
	dc.FillRect(rectWindow, &CBrush(::GetSysColor(COLOR_BTNFACE)));
	
    // paint the mobile edge
    dc.Draw3dRect(m_rectBorder, ::GetSysColor(COLOR_BTNHIGHLIGHT),
        ::GetSysColor(COLOR_BTNSHADOW));
	
    ReleaseDC(&dc);
}

LRESULT CCJControlBar::OnNcHitTest(CPoint point) 
{
    if (IsFloating())
        return CControlBar::OnNcHitTest(point);

    CRect rc;
    GetWindowRect(rc);
    point.Offset(-rc.left, -rc.top);

    if (m_rectBorder.PtInRect(point))
        return HTSIZE;
    else
        return HTCLIENT;
}

BOOL CCJControlBar::IsFloating()
{
	return (!IsHorzDocked() && !IsVertDocked());
}

void CCJControlBar::StartTracking()
{
    SetCapture();

    // make sure no updates are pending
    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
    m_pDockSite->LockWindowUpdate();

    m_ptOld = m_rectBorder.CenterPoint();
    m_bTracking = TRUE;
    
    m_rectTracker = m_rectBorder;
    if (!IsHorzDocked()) m_rectTracker.bottom -= 4;

    OnInvertTracker(m_rectTracker);
}

void CCJControlBar::StopTracking(BOOL bAccept)
{
    OnInvertTracker(m_rectTracker);
    m_pDockSite->UnlockWindowUpdate();
    m_bTracking = FALSE;
    ReleaseCapture();
    
    if (!bAccept)
		return;

    int maxsize, minsize, newsize;
    CRect rcc;
    m_pDockSite->GetClientRect(rcc);

    newsize = IsHorzDocked() ? m_sizeHorz.cy : m_sizeVert.cx;
    maxsize = newsize + (IsHorzDocked() ? rcc.Height() : rcc.Width());
    minsize = IsHorzDocked() ? m_sizeMin.cy : m_sizeMin.cx;

    CPoint point = m_rectTracker.CenterPoint();
    switch (m_nDockBarID)
    {
    case AFX_IDW_DOCKBAR_TOP:
		{
			newsize += point.y - m_ptOld.y;
			break;
		}
    case AFX_IDW_DOCKBAR_BOTTOM:
		{
			newsize += -point.y + m_ptOld.y;
			break;
		}
    case AFX_IDW_DOCKBAR_LEFT:
		{
			newsize += point.x - m_ptOld.x;
			break;
		}
    case AFX_IDW_DOCKBAR_RIGHT:
		{
			newsize += -point.x + m_ptOld.x;
			break;
		}
    }

    newsize = max(minsize, min(maxsize, newsize));

    if (IsHorzDocked())
        m_sizeHorz.cy = newsize;
    else
        m_sizeVert.cx = newsize;

    m_pDockSite->DelayRecalcLayout();
}

void CCJControlBar::OnInvertTracker(const CRect &rect)
{
    ASSERT_VALID(this);
    ASSERT(!rect.IsRectEmpty());
    ASSERT(m_bTracking);

    CRect rct = rect, rcc, rcf;
    GetWindowRect(rcc);
    m_pDockSite->GetWindowRect(rcf);

    rct.OffsetRect(rcc.left - rcf.left, rcc.top - rcf.top);
    rct.DeflateRect(1, 1);

    CDC *pDC = m_pDockSite->GetDCEx(NULL,
        DCX_WINDOW|DCX_CACHE|DCX_LOCKWINDOWUPDATE);

    CBrush* pBrush = CDC::GetHalftoneBrush();
    HBRUSH hOldBrush = NULL;
    if (pBrush != NULL)
        hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);

    pDC->PatBlt(rct.left, rct.top, rct.Width(), rct.Height(), PATINVERT);

    if (hOldBrush != NULL)
        SelectObject(pDC->m_hDC, hOldBrush);

    m_pDockSite->ReleaseDC(pDC);
}

void CCJControlBar::OnCaptureChanged(CWnd *pWnd) 
{
    if (m_bTracking && pWnd != this)
        StopTracking(FALSE); // cancel tracking
	
	CControlBar::OnCaptureChanged(pWnd);
}

void CCJControlBar::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
    if (m_bTracking)
		return;
    
    if ((nHitTest == HTSIZE) && !IsFloating())
        StartTracking();
    else    
		CControlBar::OnNcLButtonDown(nHitTest, point);
}

void CCJControlBar::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (IsFloating() || !m_bTracking) {
        CControlBar::OnMouseMove(nFlags, point);
        return;
    }

    CPoint cpt = m_rectTracker.CenterPoint();
    ClientToWnd(point);

    if (IsHorzDocked()) {
        if (cpt.y != point.y) {
            OnInvertTracker(m_rectTracker);
            m_rectTracker.OffsetRect(0, point.y - cpt.y);
            OnInvertTracker(m_rectTracker);
        }
    }

    else {
        if (cpt.x != point.x) {
            OnInvertTracker(m_rectTracker);
            m_rectTracker.OffsetRect(point.x - cpt.x, 0);
            OnInvertTracker(m_rectTracker);
        }
    }
}

void CCJControlBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    if (m_pDockBar != NULL)
    {
        // start the drag
        ASSERT(m_pDockContext != NULL);
        ClientToScreen(&point);
        m_pDockContext->StartDrag(point);
    }
    else
		CControlBar::OnLButtonDown(nFlags, point);
}

void CCJControlBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
    if (!m_bTracking)
        CControlBar::OnLButtonUp(nFlags, point);
    else
    {
        ClientToWnd(point);
        StopTracking(TRUE);
    }
}

void CCJControlBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    if (m_pDockBar != NULL)
    {
        // toggle docking
        ASSERT(m_pDockContext != NULL);
        m_pDockContext->ToggleDocking();
    }
    else
		CControlBar::OnLButtonDblClk(nFlags, point);
}

CPoint& CCJControlBar::ClientToWnd(CPoint &point)
{
    point.Offset(2, 2);

    if (m_nDockBarID == AFX_IDW_DOCKBAR_BOTTOM)
        point.y += m_cxEdge;
    else if (m_nDockBarID == AFX_IDW_DOCKBAR_RIGHT)
        point.x += m_cxEdge;

    return point;
}

CRect CCJControlBar::GetButtonRect()
{
	CRect pRect;
	GetClientRect(pRect);
	pRect.OffsetRect(-pRect.left,-pRect.top);

	if(IsHorzDocked()) {
		pRect.top	 += 3;
		pRect.bottom = pRect.top+12;
		pRect.left  += 2;
		pRect.right  = pRect.left+12;
	}
	else
	{
		pRect.right -= 19;
		pRect.left   = pRect.right-12;
		pRect.top   += 3;
		pRect.bottom = pRect.top+12;
	}
	return pRect;
}

CRect CCJControlBar::GetGripperRect()
{
	CRect pRect;
	GetClientRect(pRect);
	pRect.OffsetRect(-pRect.left,-pRect.top);

	if(IsHorzDocked()) {
		pRect.DeflateRect(3,3);
		pRect.left		+= 1;
		pRect.right		 = pRect.left+3;
		pRect.bottom	-= 1;
		pRect.top		+= m_bButtons?30:1;
	}
	else {
		pRect.DeflateRect(4,4);
		pRect.top		+= 2;
		pRect.bottom	 = pRect.top+3;
		pRect.right		-= m_bButtons?30:2;
	}

	return pRect;
}

void CCJControlBar::DrawGripper(CDC* pDC)
{
	if (IsFloating())
		return;

	if (m_bGripper)
	{
		// draw the gripper.
		CRect pRect(GetGripperRect());
		pDC->Draw3dRect( pRect, ::GetSysColor(COLOR_BTNHIGHLIGHT),
			::GetSysColor(COLOR_BTNSHADOW) );
		
		if(IsHorzDocked())
			pRect.OffsetRect(4,0);
		else
			pRect.OffsetRect(0,4);
		
		pDC->Draw3dRect( pRect, ::GetSysColor(COLOR_BTNHIGHLIGHT),
			::GetSysColor(COLOR_BTNSHADOW) );
	}
    
	m_pDockSite->RecalcLayout();
}

int CCJControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_bButtons)
	{
		ASSERT(m_ImageList);

		if(!m_btnClose.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
			BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
			CRect(0,0,0,0), this, IDC_BUTTON_HIDE ))
		{
			TRACE0("Unable to create CCJControlBar close button\n");
			return -1;
		}

		if(!m_btnMinim.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
			BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
			CRect(0,0,0,0), this, IDC_BUTTON_MINI ))
		{
			TRACE0("Unable to create CCJControlBar close button\n");
			return -1;
		}

		m_btnClose.DisableFlatLook();
		m_btnMinim.DisableFlatLook();

		m_btnClose.SetIcon(m_ImageList->ExtractIcon(0),CSize(13,13));

		// Create the ToolTip control.
		m_ToolTip.Create(this);
		m_ToolTip.Activate(TRUE);
		
		m_ToolTip.AddTool (&m_btnClose, _T("Hide Docked Window"));
		m_ToolTip.AddTool (&m_btnMinim, _T("Minimize Docked Window"));
	}
	
	return 0;
}

CImageList* CCJControlBar::SetBtnImageList(CImageList *pImageList)
{
	CImageList* pPrevList = m_ImageList;
	m_ImageList = pImageList;
	return pPrevList;
}

void CCJControlBar::OnButtonClose()
{
	GetDockingFrame()->ShowControlBar(this, FALSE, FALSE);
}

void CCJControlBar::OnUpdateButtonClose(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void CCJControlBar::OnButtonMinimize()
{
	m_pDockContext->ToggleDocking();
}

void CCJControlBar::OnUpdateButtonMinimize(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

BOOL CCJControlBar::PreTranslateMessage(MSG* pMsg) 
{
	if (m_bButtons)
		m_ToolTip.RelayEvent(pMsg);
	return CControlBar::PreTranslateMessage(pMsg);
}

void CCJControlBar::SetMenuID(UINT nID)
{
	m_menuID = nID;
}

UINT CCJControlBar::GetMenuID()
{
	return m_menuID;
}

void CCJControlBar::OnContextMenu(CWnd* /*pWnd*/, CPoint point) 
{
	// if no menu, just return.
	if (m_menuID == 0 ) {
		TRACE0("No control bar menu defined.\n");
		return;
	}

	if (point.x == -1 && point.y == -1)
	{
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		
		point = rect.TopLeft();
		point.Offset(5, 5);
	}
	
	CMenu menu;
	VERIFY(menu.LoadMenu(m_menuID));
	
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;
	
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();
	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}
