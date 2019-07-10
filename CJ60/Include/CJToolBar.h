// CJToolBar.h : header file
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

#if !defined(CJTOOLBAR_H_INCLUDED)
#define CJTOOLBAR_H_INCLUDED

#if _MSC_VER < 1200 // Compatibility with VC5
#include "FlatBar.h"
#define CMyToolBar CFlatToolBar
#else
#define CMyToolBar CToolBar
#endif // _MSC_VER < 1200

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCJToolBar class

class CLASS_EXPORT CCJToolBar : public CMyToolBar
{
	DECLARE_DYNAMIC(CCJToolBar)

// Construction
public:
	CCJToolBar();

// Attributes
protected:
	CObList* m_pControls;
	CFont m_font;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Draw3dRect(CDC *pDC, CRect rc) const;
	void SetButtonDropDown(int nID);
	void SetImageList(CImageList * pList, DWORD dwStyle);
	CWnd* InsertControl(CRuntimeClass* pClass, CString strTitle, CRect& pRect, UINT nID, DWORD dwStyle );
	void DrawNoGripper() { m_dwStyle &= ~CBRS_GRIPPER; }
	virtual ~CCJToolBar();

protected:
	void DrawBorders(CDC* pDC, CRect& rect);
	void EraseNonClient(BOOL);
	void DrawGripper(CDC & dc) const;

// Generated message map functions
protected:
	//{{AFX_MSG(CCJToolBar)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CJTOOLBAR_H_INCLUDED)

