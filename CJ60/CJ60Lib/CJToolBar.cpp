// CJToolBar.cpp : implementation file
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
//			1.02	02 Nov 1998 - Fixed bug with DrawNoGripper() method -
//								  (Christian Skovdal Andersen).
// ==========================================================================
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJToolBar.h"
#include "CJComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJToolBar

CCJToolBar::CCJToolBar()
{
	m_pControls = NULL;
}

CCJToolBar::~CCJToolBar()
{
	if( m_pControls ) {
		for( POSITION pos = m_pControls->GetHeadPosition() ; pos ; ) {
			delete m_pControls->GetNext(pos);
		}
		delete m_pControls;
	}
}

IMPLEMENT_DYNAMIC(CCJToolBar, CMyToolBar)

BEGIN_MESSAGE_MAP(CCJToolBar, CMyToolBar)
	//{{AFX_MSG_MAP(CCJToolBar)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_CREATE()
	ON_WM_NCCREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJToolBar message handlers

//////////////////////////////////////////////////////////////////////
// Inserts a control into the toolbar at the given button id.
// pClass	- runtime class of the control.
// strTitle - window title ( if any ) of the control.
// pRect	- size of the control to be inserted.
// nID		- resource id of the button where the control is to be placed.
// dwStyle  - style flags for the control

CWnd* CCJToolBar::InsertControl(CRuntimeClass * pClass, CString strTitle, CRect & pRect, UINT nID, DWORD dwStyle)
{
	dwStyle |= WS_CHILD | WS_VISIBLE;
	CWnd* pCtrl = 0;
	
	CRect rect = pRect;
	BOOL bCreate = FALSE;
	
	// make sure the id is valid
	ASSERT( CommandToIndex( nID ) >= 0 );
	SetButtonInfo( CommandToIndex( nID ), nID, TBBS_SEPARATOR, pRect.Width());
	
	CString strClass( pClass->m_lpszClassName );
	
	if( strClass == TEXT("CCJComboBox") ) {
		pCtrl = new CCJComboBox();
		bCreate = ((CCJComboBox*)pCtrl)->Create(dwStyle, rect, this, nID);
	}
	
	else if( strClass == TEXT("CComboBox") ) {
		pCtrl = new CComboBox();
		bCreate = ((CComboBox*)pCtrl)->Create(dwStyle, rect, this, nID);
	}
	
	else if( strClass == TEXT("CEdit") ) {
		pCtrl = new CEdit();
		bCreate = ((CEdit*)pCtrl)->Create(dwStyle, rect, this, nID);
	}
	
	else if( strClass == TEXT("CButton") ) {
		pCtrl = new CButton();
		bCreate = ((CButton*)pCtrl)->Create(strTitle,dwStyle, rect, this, nID);
	}
	
	else {
		pCtrl = (CWnd*)pClass->CreateObject();
		bCreate = pCtrl->Create(0, 0, dwStyle, rect, this, nID);
	}
	
	if( !pCtrl ) return 0;
	if( !bCreate ) { delete pCtrl; return 0; }
	
	GetItemRect( CommandToIndex(nID), &pRect );
	
	pCtrl->SetWindowPos(0, pRect.left, pRect.top, 0, 0,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOCOPYBITS );
	
	pCtrl->SetFont( &m_font );
	pCtrl->ShowWindow( SW_SHOW );
	
	// we have to remember this control, so we can delete it later
	if( !m_pControls ) {
		m_pControls = new CObList();
		m_pControls->AddTail( pCtrl );
	}

	// David Cofer
	else
		m_pControls->AddTail( pCtrl);
	
	return pCtrl;
}

//////////////////////////////////////////////////////////////////////
// dwStyle - TB_SETIMAGELIST grayscale (no color) image list
//         - TB_SETHOTIMAGELIST color image list

void CCJToolBar::SetImageList(CImageList * pList, DWORD dwStyle)
{
	SendMessage( dwStyle, 0, (LPARAM)pList->m_hImageList );
}

//////////////////////////////////////////////////////////////////////
// sets the specified button to have a drop down arrow
// nID - resource id of the toolbar button

void CCJToolBar::SetButtonDropDown(int nID)
{
	// change button style to dropdown
	SetButtonStyle( CommandToIndex(nID),
		GetButtonStyle(CommandToIndex(nID)) | TBSTYLE_DROPDOWN );
}

//////////////////////////////////////////////////////////////////////
// This draws the gripper bars seen to the left and
// top of toolbar

void CCJToolBar::DrawGripper(CDC & dc) const
{
    // no gripper if floating
	if (IsFloating()) {
		return;
	}

	if (m_dwStyle & CBRS_GRIPPER)
	{
		CRect gripper;
		GetWindowRect( gripper );
		ScreenToClient( gripper );
		gripper.OffsetRect( -gripper.left, -gripper.top );
		
		if( m_dwStyle & CBRS_ORIENT_HORZ ) {
			
			// gripper at left
			gripper.DeflateRect(4,3);
			gripper.right = gripper.left+3;
			gripper.bottom += 1;
			Draw3dRect(&dc, gripper);
		}
		
		else {
			
			// gripper at top
			gripper.DeflateRect(3,4);
			gripper.top -= 1;
			gripper.bottom = gripper.top+3;
			Draw3dRect(&dc, gripper);
		}
	}
}

void CCJToolBar::EraseNonClient(BOOL)
{
	// get window DC that is clipped to the non-client area
	CWindowDC dc(this);
	CRect rectClient;
	GetClientRect(rectClient);
	CRect rectWindow;
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.ExcludeClipRect(rectClient);     // draw borders in non-client area

	// draw borders in non-client area
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	DrawBorders(&dc, rectWindow);     // erase parts not drawn
	dc.IntersectClipRect(rectWindow);
	SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);
	DrawGripper(dc);
}

void CCJToolBar::DrawBorders(CDC * pDC, CRect & rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	DWORD dwStyle = m_dwStyle;
	if (!(dwStyle & CBRS_BORDER_ANY))
		return;

	// prepare for dark lines
	ASSERT(rect.top == 0 && rect.left == 0);

	COLORREF clr=GetSysColor(COLOR_3DSHADOW);
	if(dwStyle&CBRS_BORDER_RIGHT)
		pDC->FillSolidRect(rect.right-1,0,rect.right,rect.bottom,clr); //right
	if(dwStyle&CBRS_BORDER_BOTTOM)
		pDC->FillSolidRect(0,rect.bottom-1,rect.right,rect.bottom,clr); //bottom

	clr=GetSysColor(COLOR_3DHIGHLIGHT);
	if(dwStyle&CBRS_BORDER_TOP)
		pDC->FillSolidRect(0,0,rect.right,1,clr); //top
	if(dwStyle&CBRS_BORDER_LEFT)
		pDC->FillSolidRect(0,0,1,rect.bottom,clr); //left

	if(dwStyle&CBRS_BORDER_TOP)
		rect.top++;
	if(dwStyle&CBRS_BORDER_RIGHT)
		rect.right--;
	if(dwStyle&CBRS_BORDER_BOTTOM)
		rect.bottom--;
	if(dwStyle&CBRS_BORDER_LEFT)
		rect.left++;
}

//////////////////////////////////////////////////////////////////////
// *** Kirk Stowell
// This offsets the non-client area to allow enough
// room for the gripper to be drawn

void CCJToolBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	if (m_dwStyle & CBRS_GRIPPER)
	{	
		if( m_dwStyle & CBRS_ORIENT_HORZ ) {
#if _MSC_VER < 1200
			lpncsp->rgrc[0].top   += 2;
			lpncsp->rgrc[0].left  += 4;
			lpncsp->rgrc[0].right += 4;
#else
			lpncsp->rgrc[0].left  += 1;
			lpncsp->rgrc[0].right += 1;
#endif		
		}
		else {
#if _MSC_VER < 1200
			lpncsp->rgrc[0].top    += 6;
			lpncsp->rgrc[0].bottom += 6;
#else
			lpncsp->rgrc[0].top    += 1;
			lpncsp->rgrc[0].bottom += 1;
#endif
		}
	}
	CMyToolBar::OnNcCalcSize(bCalcValidRects, lpncsp);
}

//////////////////////////////////////////////////////////////////////
// *** Kirk Stowell
// This will draw the gripper on the toolbar then repaints
// client areas.

void CCJToolBar::OnNcPaint() 
{
	CControlBar::EraseNonClient();
	CWindowDC dc(this);
	
	CRect pRect;
	GetClientRect( &pRect );
	InvalidateRect( &pRect, TRUE );
	EraseNonClient(FALSE);
	
	// Do not call CMyToolBar::OnNcPaint() for painting messages
}

int CCJToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMyToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Set the extended style for drop arrows.
	SendMessage( TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS );
	
	ModifyStyle(0, TBSTYLE_FLAT); // flat with gripper.
	m_dwStyle |= CBRS_GRIPPER;
	
    // Define the font to use
	m_font.CreatePointFont( 80, _T("MS Sans Serif") );
    SetFont(&m_font);
	
	return 0;
}

void CCJToolBar::Draw3dRect(CDC *pDC, CRect rc) const
{
	static bool bDraw2nd = true;

	if (bDraw2nd == true) {
		rc.right  -= 1;
		rc.bottom -= 1;
	}

	// Get a pen for hilite and shadow.
	CPen penHilite(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHILIGHT));
	CPen penShadow(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));

	// Set up points for line draw.
	CPoint ptTopLeft	(rc.left,  rc.top);
	CPoint ptTopRight	(rc.right, rc.top);
	CPoint ptBottomLeft	(rc.left,  rc.bottom);
	CPoint ptBottomRight(rc.right, rc.bottom);

	// Select the shadow pen, and draw the bottom right.
	pDC->SelectObject(&penShadow);

	pDC->MoveTo(ptTopRight);
	pDC->LineTo(ptBottomRight);
	pDC->LineTo(ptBottomLeft);

	// Select the hilite pen, and draw the top left.
	pDC->SelectObject(&penHilite);

	pDC->MoveTo(ptBottomLeft);
	pDC->LineTo(ptTopLeft);
	pDC->LineTo(ptTopRight);

	// Draw the second gripper line.
	if (bDraw2nd == true)
	{
		bDraw2nd = false;
		if (m_dwStyle & CBRS_ORIENT_HORZ)
			rc.OffsetRect(3,0);
		else 
			rc.OffsetRect(0,3);
		Draw3dRect(pDC, rc);
		bDraw2nd = true;
	}
}
