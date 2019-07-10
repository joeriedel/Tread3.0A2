// CJTabCtrlBar.cpp : implementation file
//
// DevStudio Style Resizable Docking Tab Control Bar.
//
// Copyright © 1998 Written by Kirk Stowell   
//		mailto:kstowel@sprynet.com
//		http://www.geocities.com/SiliconValley/Haven/8230
//
// The code contained in this file is based on the original
// CSizingTabCtrlBar class written by Dirk Clemens,
//		mailto:dirk_clemens@hotmail.com
//		http://www.codeguru.com/docking/sizing_tabctrl_bar.shtml
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
#include "CJTabCtrlBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrlBar

CCJTabCtrlBar::CCJTabCtrlBar()
{
	m_nActiveTab = 0;
}

CCJTabCtrlBar::~CCJTabCtrlBar()
{
	while(!m_views.IsEmpty())
	{
		TCB_ITEM *pMember=m_views.RemoveHead();
		delete pMember;
	}
}

#define IDC_TABCTRLBAR 1000

BEGIN_MESSAGE_MAP(CCJTabCtrlBar, CCJControlBar)
	//{{AFX_MSG_MAP(CCJTabCtrlBar)
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABCTRLBAR, OnTabSelChange)
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrlBar message handlers

// *** K.Stowell
void CCJTabCtrlBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd *pWnd;
	
	if (IsFloating()) {
		m_tabctrl.MoveWindow( 5, 5, lpwndpos->cx-10, lpwndpos->cy-7 );
		for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos)) {
			pWnd=m_views.GetAt(pos)->pWnd;
			pWnd->MoveWindow(8, 8, lpwndpos->cx-16, lpwndpos->cy-34);
		}
	}
	
	else if (IsHorzDocked()) {
		m_tabctrl.MoveWindow( 17, 5, lpwndpos->cx-25, lpwndpos->cy-17 );
		for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos)) {
			pWnd=m_views.GetAt(pos)->pWnd;
			pWnd->MoveWindow(20, 8, lpwndpos->cx-31, lpwndpos->cy-44);
		}
	}
	
	else {
		m_tabctrl.MoveWindow( 5, 20, lpwndpos->cx-17, lpwndpos->cy-31 );
		for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos)) {
			pWnd=m_views.GetAt(pos)->pWnd;
			pWnd->MoveWindow(8, 23, lpwndpos->cx-23, lpwndpos->cy-58);
		}
	}

	CCJControlBar::OnWindowPosChanged(lpwndpos);
}

// *** K.Stowell
CImageList* CCJTabCtrlBar::SetTabImageList(CImageList *pImageList)
{
	return m_tabctrl.SetImageList (pImageList);
}

// *** K.Stowell
BOOL CCJTabCtrlBar::ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	return m_tabctrl.ModifyStyle(dwRemove, dwAdd);
}

// *** K.Stowell
int CCJTabCtrlBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//Create the Tab Control
	if (!m_tabctrl.Create(WS_VISIBLE|WS_CHILD|TCS_BOTTOM|TCS_TOOLTIPS, 
		CRect(0,0,0,0), this, IDC_TABCTRLBAR))
	{
		TRACE0("Unable to create tab control bar\n");
		return -1;
	}
	
	// set "normal" GUI-font
	CFont *font = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	m_tabctrl.SetFont(font);

// VC5 Support.
#if _MSC_VER >= 1200
	m_ToolTip = m_tabctrl.GetToolTips();
	m_tabctrl.SetToolTips(m_ToolTip);
#else
	m_ToolTip = m_tabctrl.GetTooltips();
	m_tabctrl.SetTooltips(m_ToolTip);
#endif
	return 0;
}

// *** KStowell
void CCJTabCtrlBar::OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetActiveView(m_tabctrl.GetCurSel());
	Invalidate();
}

//////////////////////////////////////////////////
// The remainder of this class was written by Dirk Clemens...

BOOL CCJTabCtrlBar::AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, CCreateContext *pContext)
{	

#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
#endif

	CCreateContext context;
	if (pContext == NULL)
	{
		// if no context specified, generate one from the currently selected
		//  client if possible
		CView* pOldView = NULL;
		if (pOldView != NULL && pOldView->IsKindOf(RUNTIME_CLASS(CView)))
		{
			// set info about last pane
			ASSERT(context.m_pCurrentFrame == NULL);
			context.m_pLastView = pOldView;
			context.m_pCurrentDoc = pOldView->GetDocument();
			if (context.m_pCurrentDoc != NULL)
				context.m_pNewDocTemplate =
				context.m_pCurrentDoc->GetDocTemplate();
		}
		pContext = &context;
	}
	
	CWnd* pWnd;
	TRY
	{
		pWnd = (CWnd*)pViewClass->CreateObject();
		if (pWnd == NULL)
			AfxThrowMemoryException();
	}
	CATCH_ALL(e)
	{
		TRACE0("Out of memory creating a view.\n");
		// Note: DELETE_EXCEPTION(e) not required
		return FALSE;
	}
	END_CATCH_ALL
		
    ASSERT_KINDOF(CWnd, pWnd);
	ASSERT(pWnd->m_hWnd == NULL);       // not yet created
	
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW;
	CRect rect;
	// Create with the right size and position
	if (!pWnd->Create(NULL, NULL, dwStyle, rect, this, 0, pContext))
	{
		TRACE0("Warning: couldn't create client pane for view.\n");
		// pWnd will be cleaned up by PostNcDestroy
		return FALSE;
	}
	m_pActiveView = (CView*) pWnd;

	TCB_ITEM *pMember=new TCB_ITEM;
	pMember->pWnd=pWnd;
	strcpy(pMember->szLabel, lpszLabel);
	m_views.AddTail(pMember);

	// ToolTip support for tabs.
	if((m_views.GetCount()-1)==0) {
		m_ToolTip->AddTool( &m_tabctrl, lpszLabel,
			NULL, m_views.GetCount()-1 );
	}
	else {
		m_ToolTip->AddTool( &m_tabctrl, lpszLabel,
			CRect(0,0,0,0), m_views.GetCount()-1 );
	}
	
	int nViews = m_views.GetCount();
	if (nViews!=1)
	{
		pWnd->EnableWindow(FALSE);
		pWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		((CFrameWnd *)GetParent())->SetActiveView((CView *)m_pActiveView);
	}

	TC_ITEM tci;
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = (LPTSTR)(LPCTSTR)lpszLabel;
	tci.iImage = nViews-1;
	m_tabctrl.InsertItem(nViews, &tci);

	return TRUE;
}

void CCJTabCtrlBar::RemoveView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);

	// remove the page from internal list
	m_views.RemoveAt(m_views.FindIndex(nView));
}

void CCJTabCtrlBar::SetActiveView(int nNewTab)
{
	ASSERT_VALID(this);
	ASSERT(nNewTab >= 0);

	if (nNewTab!=-1 && nNewTab!=m_nActiveTab)
	{
        TCB_ITEM *newMember=m_views.GetAt(m_views.FindIndex(nNewTab));
        TCB_ITEM *oldMember=NULL;
		
        if (m_nActiveTab!=-1)
        {
            oldMember=m_views.GetAt(m_views.FindIndex(m_nActiveTab));
            oldMember->pWnd->EnableWindow(FALSE);
            oldMember->pWnd->ShowWindow(SW_HIDE);
        }
        newMember->pWnd->EnableWindow(TRUE);
        newMember->pWnd->ShowWindow(SW_SHOW);
        newMember->pWnd->SetFocus();

        m_pActiveView = (CView *)newMember->pWnd;
		((CFrameWnd *)GetParent())->SetActiveView(m_pActiveView);

        m_nActiveTab = nNewTab;
		// select the tab (if tab programmatically changed)
		m_tabctrl.SetCurSel(m_nActiveTab);
    }
}

void CCJTabCtrlBar::SetActiveView(CRuntimeClass *pViewClass)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));

	int nNewTab = 0;
	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	{
		TCB_ITEM *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
			//first hide old first view
            m_pActiveView->EnableWindow(FALSE);
            m_pActiveView->ShowWindow(SW_HIDE);
			
			// set new active view
			m_pActiveView = (CView*)pMember->pWnd;
			// enable, show, set focus to new view
			m_pActiveView->EnableWindow(TRUE);
			m_pActiveView->ShowWindow(SW_SHOW);
			m_pActiveView->SetFocus();
			
			((CFrameWnd *)GetParent())->SetActiveView(m_pActiveView);

	        m_nActiveTab = nNewTab;
			// select the tab
			m_tabctrl.SetCurSel(m_nActiveTab);

			break;
		}
		nNewTab++;
    }
}

CView* CCJTabCtrlBar::GetActiveView()
{
	return m_pActiveView;
}

CView* CCJTabCtrlBar::GetView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);

	if (nView!=-1)
	{
        TCB_ITEM *pMember=m_views.GetAt(m_views.FindIndex(nView));
		return (CView*)pMember->pWnd;
	}
	else
		return NULL;
}

CView* CCJTabCtrlBar::GetView(CRuntimeClass *pViewClass)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));

	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	{
		TCB_ITEM *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
			return (CView*)pMember->pWnd;
		}
    }
	return NULL;
}
