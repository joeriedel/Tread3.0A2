// CJMDIFrameWnd.h : header file
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

#if !defined(CJMDIFRAMEWND_H_INCLUDED)
#define CJMDIFRAMEWND_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CoolMenu.h"

/////////////////////////////////////////////////////////////////////////////
// CCJMDIFrameWnd class

class CLASS_EXPORT CCJMDIFrameWnd : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CCJMDIFrameWnd)

// Construction
public:
	CCJMDIFrameWnd();

// Attributes
public:
	CCoolMenuManager m_pMenuMgr;

// Operations
public:
	void DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf);
	BOOL InstallCoolMenus(CWnd* pParentWnd, UINT nBar);
	BOOL InstallCoolMenus(CWnd* pParentWnd, UINT nNum, const UINT* nBars);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJMDIFrameWnd)
	public:
	virtual void EnableDocking(DWORD dwDockStyle);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJMDIFrameWnd();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJMDIFrameWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CJMDIFRAMEWND_H_INCLUDED)

