// CJDockBar.h : header file
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

#if !defined(CJDOCKBAR_H_INCLUDED)
#define CJDOCKBAR_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxpriv.h>

// nDockBarMap
const DWORD nDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};

/////////////////////////////////////////////////////////////////////////////
// CCJDockBar class

class CLASS_EXPORT CCJDockBar : public CDockBar
{
	DECLARE_DYNAMIC(CCJDockBar)

// Construction
public:
	CCJDockBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJDockBar)
	public:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJDockBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJDockBar)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CJDOCKBAR_H_INCLUDED)

