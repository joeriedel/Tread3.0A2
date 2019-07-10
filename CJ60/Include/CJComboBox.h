// CJComboBox.h : header file
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

#if !defined(CJCOMBOBOX_H_INCLUDED)
#define CJCOMBOBOX_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FC_DRAWNORMAL	0x00000001
#define FC_DRAWRAISED	0x00000002
#define FC_DRAWPRESSD	0x00000004

/////////////////////////////////////////////////////////////////////////////
// CCJComboBox window

class CLASS_EXPORT CCJComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CCJComboBox)

// Construction
public:
	CCJComboBox();

// Attributes
public:
	bool m_bLBtnDown;

// Operations
public:
	void DrawCombo(DWORD dwStyle, COLORREF clrTopLeft, COLORREF clrBottomRight);
	int Offset();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJComboBox)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(CJCOMBOBOX_H_INCLUDED)
