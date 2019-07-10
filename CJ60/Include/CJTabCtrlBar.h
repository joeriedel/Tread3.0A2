// CJTabCtrlBar.h : header file
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

#if !defined(CJTABCTRLBAR_H_INCLUDED)
#define CJTABCTRLBAR_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "CJControlBar.h"

typedef struct
{
	CWnd *pWnd;
	char szLabel[32];
}TCB_ITEM;

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrlBar tab control bar

class CLASS_EXPORT CCJTabCtrlBar : public CCJControlBar
{
// Construction
public:
	CCJTabCtrlBar();

// Attributes
public:
	CTabCtrl m_tabctrl;
	CToolTipCtrl* m_ToolTip;

protected:
	int m_nActiveTab;
	CView* m_pActiveView;
	CList <TCB_ITEM *,TCB_ITEM *> m_views;

// Operations
public:
	void SetActiveView(int nNewTab);
	void SetActiveView(CRuntimeClass *pViewClass);
	CImageList* SetTabImageList(CImageList *pImageList);
	BOOL ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags);

// Overrides
public:
    // ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJTabCtrlBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	CView* GetActiveView();
	CView* GetView(int nView);
	CView* GetView(CRuntimeClass *pViewClass);
	BOOL AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, CCreateContext *pContext = NULL);
	void RemoveView(int nView);
	virtual ~CCJTabCtrlBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJTabCtrlBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // CJTABCTRLBAR_H_INCLUDED
