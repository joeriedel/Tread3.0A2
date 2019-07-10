// CJFrameWnd.h : header file
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

#if !defined(CJFRAMEWNDEX_H_INCLUDED)
#define CJFRAMEWNDEX_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CoolMenu.h"

/////////////////////////////////////////////////////////////////////////////
// CCJFrameWnd class

class CLASS_EXPORT CCJFrameWnd : public CFrameWnd
{
	DECLARE_DYNAMIC(CCJFrameWnd)

// Construction
public:
	CCJFrameWnd();

// Attributes
protected:
	CCoolMenuManager m_pMenuMgr;

// Operations
public:
	void DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf);
	BOOL InstallCoolMenus(CWnd* pParentWnd, UINT nBar);
	BOOL InstallCoolMenus(CWnd* pParentWnd, UINT nNum, const UINT* nBars);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJFrameWnd)
	public:
	virtual void EnableDocking(DWORD dwDockStyle);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJFrameWnd();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJFrameWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CJFRAMEWNDEX_H_INCLUDED)

