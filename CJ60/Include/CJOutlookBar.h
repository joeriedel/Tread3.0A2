// CJOutlookBar.h : header file
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

#if !defined(CJOUTLOOKBAR_H_INCLUDED)
#define CJOUTLOOKBAR_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//////////////////////////////////////////////////////////////////////
// CContentItems - Container class for menu items.

class CLASS_EXPORT CContentItems
{
protected:
	UINT m_nImageID;
	CString m_csText;

public:
	CContentItems( UINT nID, CString str ) :
	  m_nImageID( nID ), m_csText( str ) {
	}

	void operator = ( CContentItems& pItems ) {
		m_nImageID = pItems.m_nImageID;
		m_csText = pItems.m_csText;
	}

	UINT GetImageID() { return m_nImageID; }
	CString GetText() { return m_csText;  }
};

#define WM_OUTLOOKBAR_NOTIFY	(WM_USER + 1)
#define OBM_ITEMCLICK			1

/////////////////////////////////////////////////////////////////////////////
// CJOutlookBar class

class CLASS_EXPORT CCJOutlookBar : public CListBox
{
	DECLARE_DYNAMIC(CCJOutlookBar)

// Construction
public:
	CCJOutlookBar();

// Attributes
public:
	CWnd*			p_Owner;
    CPoint          m_point;
	CImageList		m_ImageListNormal;
	CImageList		m_ImageList;
	CContentItems*  m_pContents;
    int             m_nNumItems;
	int             m_nIndex;
    bool            m_bHilight;
    bool            m_bLBDown;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJOutlookBar)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetItems( CContentItems* pItems, int nNumItems );
	void SetOwner( CWnd* pWnd) { p_Owner = pWnd; }
	virtual ~CCJOutlookBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJOutlookBar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CJOUTLOOKBAR_H_INCLUDED)

