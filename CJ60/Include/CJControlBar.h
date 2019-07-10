// CJControlBar.h : header file
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

#if !defined(CJCONTROLBAR_H_INCLUDED)
#define CJCONTROLBAR_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CJButton.h"

/////////////////////////////////////////////////////////////////////////////
// CCJControlBar class

class CLASS_EXPORT CCJControlBar : public CControlBar
{
	DECLARE_DYNAMIC(CCJControlBar)

// Construction
public:
	CCJControlBar();

// Attributes
protected:
	BOOL		 m_bInRecalcNC;
    BOOL         m_bTracking;
	BOOL		 m_bGripper;
	BOOL		 m_bButtons;
    UINT         m_cxEdge;
	UINT		 m_menuID;
    UINT         m_nDockBarID;
    CSize        m_sizeMin;
    CSize        m_sizeHorz;
    CSize        m_sizeVert;
    CSize        m_sizeFloat;
    CRect        m_rectBorder;
    CRect        m_rectTracker;
    CPoint       m_ptOld;
	CCJButton    m_btnClose;
	CCJButton    m_btnMinim;
	CImageList*  m_ImageList;
	CToolTipCtrl m_ToolTip;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJControlBar)
	public:
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL Create(CWnd* pParentWnd, UINT nID, LPCTSTR lpszWindowName = NULL, CSize sizeDefault = CSize(200,200), DWORD dwStyle = WS_CHILD|WS_VISIBLE|CBRS_TOP);
    virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );
    virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	virtual BOOL IsFloating();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsHorzDocked() const;
	BOOL IsVertDocked() const;
	
	void ShowFrameStuff(BOOL bGripper = TRUE, BOOL bButtons = TRUE) { 
		m_bGripper = bGripper;
		m_bButtons = bButtons;
	}

	void DrawGripper(CDC* pDC);
	CRect GetGripperRect();
	CRect GetButtonRect();
	UINT GetMenuID();
	void SetMenuID(UINT nID);
	CImageList* SetBtnImageList(CImageList *pImageList);
	void OnInvertTracker(const CRect& rect);
	void StopTracking(BOOL bAccept);
	void StartTracking();
	CPoint& ClientToWnd(CPoint& point);
	virtual ~CCJControlBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJControlBar)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonClose();
	afx_msg void OnUpdateButtonClose(CCmdUI* pCmdUI);
	afx_msg void OnButtonMinimize();
	afx_msg void OnUpdateButtonMinimize(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CJCONTROLBAR_H_INCLUDED)

