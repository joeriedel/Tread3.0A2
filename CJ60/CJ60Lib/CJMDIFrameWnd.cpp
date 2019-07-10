// CJMDIFrameWnd.cpp : implementation file
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
#include "CJMDIFrameWnd.h"
#include "CJDockBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJMDIFrameWnd

CCJMDIFrameWnd::CCJMDIFrameWnd()
{
	// TODO: add construction code here.
}

CCJMDIFrameWnd::~CCJMDIFrameWnd()
{
	// TODO: add destruction code here.
}

IMPLEMENT_DYNAMIC(CCJMDIFrameWnd, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CCJMDIFrameWnd, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CCJMDIFrameWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJMDIFrameWnd message handlers

void CCJMDIFrameWnd::DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;
	
	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout(TRUE);
	
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP)				 ? AFX_IDW_DOCKBAR_TOP		: n;
	n = (dw&CBRS_ALIGN_BOTTOM	&& n==0) ? AFX_IDW_DOCKBAR_BOTTOM	: n;
	n = (dw&CBRS_ALIGN_LEFT		&& n==0) ? AFX_IDW_DOCKBAR_LEFT		: n;
	n = (dw&CBRS_ALIGN_RIGHT	&& n==0) ? AFX_IDW_DOCKBAR_RIGHT	: n;
	
	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line. By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
}

void CCJMDIFrameWnd::EnableDocking(DWORD dwDockStyle)
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);
	CMDIFrameWnd::EnableDocking(dwDockStyle);
	
	for (int i = 0; i < 4; i++) {
		if (nDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY) {
			CDockBar* pDock = (CDockBar*)GetControlBar(nDockBarMap[i][0]);
			
			// make sure the dock bar is of correct type
			if( pDock == 0 || ! pDock->IsKindOf(RUNTIME_CLASS(CCJDockBar)) ) {
				BOOL bNeedDelete = ! pDock->m_bAutoDelete;
				pDock->m_pDockSite->RemoveControlBar(pDock);
				pDock->m_pDockSite = 0;	// avoid problems in destroying the dockbar
				pDock->DestroyWindow();
				if( bNeedDelete )
					delete pDock;
				pDock = 0;
			}
			
			if( pDock == 0 ) {
				pDock = new CCJDockBar;
				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
					nDockBarMap[i][1], nDockBarMap[i][0])) {
					AfxThrowResourceException();
				}
			}
		}
	}
}


BOOL CCJMDIFrameWnd::InstallCoolMenus(CWnd* pParentWnd, UINT nNum, const UINT* nBars)
{
	ASSERT(pParentWnd);
	m_pMenuMgr.Install ((CFrameWnd*)pParentWnd);
	
	if (!m_pMenuMgr.LoadToolbars(nBars, nNum))
	{
		TRACE0("Unable to Install Cool Menus.\n");
		return -1;
	}

	return TRUE;
}

BOOL CCJMDIFrameWnd::InstallCoolMenus(CWnd * pParentWnd, UINT nBar)
{
	ASSERT(pParentWnd);
	m_pMenuMgr.Install ((CFrameWnd*)pParentWnd);
	
	if (!m_pMenuMgr.LoadToolbar(nBar))
	{
		TRACE0("Unable to Install Cool Menus.\n");
		return -1;
	}

	return TRUE;
}
