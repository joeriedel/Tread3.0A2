// CJPagerCtrl.h : header file
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
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(CJPAGERCTRL_H_INCLUDED)
#define CJPAGERCTRL_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if _MSC_VER < 1200 // VC5 support
#include <comm_control.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJPagerCtrl window

class CLASS_EXPORT CCJPagerCtrl : public CWnd
{
// Construction
public:
	CCJPagerCtrl();
	virtual ~CCJPagerCtrl();

// Attributes
protected:
	static bool bInitialized;
	CWnd* m_pChild;
	int m_nWidth;
	int m_nHeight;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJPagerCtrl)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetScrollArea (int nWidth, int nHeight) { m_nWidth = nWidth; m_nHeight = nHeight; }
	void SetChild(HWND hWnd);
	void RecalcSize();
	void ForwardMouse(bool bForward);
	COLORREF SetBkColor(COLORREF clr);
	COLORREF GetBkColor();
	int SetBorder(int iBorder);
	int GetBorder();
	int SetPos(int iPos);
	int GetPos();
	int SetButtonSize(int iSize);
	int GetButtonSize();
	DWORD GetButtonState(int iButton);

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJPagerCtrl)
	virtual BOOL OnPagerScroll(NMHDR * pNMPGScroll, LRESULT * pResult);
	virtual BOOL OnPagerCalcSize(NMHDR * pNMPGCalcSize, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CJPAGERCTRL_H_INCLUDED)
