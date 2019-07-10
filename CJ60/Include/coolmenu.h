////////////////////////////////////////////////////////////////
// 1997 Microsoft Systems Journal. 
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
//

#if !defined(COOLMENU_H_INCLUDED)
#define COOLMENU_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SubClass.h"

namespace PxLib
{
	extern void FillRect(CDC& dc, const CRect& rc, COLORREF color);
	extern void DrawEmbossed(CDC& dc, CImageList& il, int i,
		CPoint p, BOOL bColor=FALSE);
	extern HBITMAP LoadSysColorBitmap(LPCTSTR lpResName, BOOL bMono=FALSE);
	inline HBITMAP LoadSysColorBitmap(UINT nResID, BOOL bMono=FALSE) {
		return LoadSysColorBitmap(MAKEINTRESOURCE(nResID), bMono);
	}
} // end namespace

//////////////////
// CCoolMenuManager implements "cool" menus with buttons in them. To use:
//
//  *	Instantiate in your CMainFrame.
//	 * Call Install to install it
//  * Call LoadToolbars or LoadToolbar to load toolbars
//
//  Don't forget to link with CoolMenu.cpp, Subclass.cpp and DrawTool.cpp!
//

class CLASS_EXPORT CCoolMenuManager : private CSubclassWnd
{
	DECLARE_DYNAMIC(CCoolMenuManager)
public:
	CCoolMenuManager();
	~CCoolMenuManager();

	// You can set these any time
	BOOL m_bShowButtons;			// use to control whether buttons are shown
	BOOL m_bAutoAccel;			// generate auto accelerators
	BOOL m_bUseDrawState;		// use ::DrawState for disabled buttons
	BOOL m_bDrawDisabledButtonsInColor; // draw disabled buttons in color
										// (only if m_bUseDrawState = FALSE)
	// public functions to use
	void Install(CFrameWnd* pFrame);					// connect to main frame
	BOOL LoadToolbars(const UINT* arIDs, int n);	// load multiple toolbars
	BOOL LoadToolbar(UINT nID);						// load one toolbar
	BOOL AddSingleBitmap(UINT nBitmapID, UINT n, UINT *nID);

	// should never need to call:
	virtual void Destroy(); // destroys everything--to re-load new toolbars?
	virtual void Refresh(); // called when system colors, etc change
	static  HBITMAP GetMFCDotBitmap();	// get..
	static  void    FixMFCDotBitmap();	// and fix MFC's dot bitmap
	static BOOL bTRACE;	// Set TRUE to see extra diagnostics in DEBUG code

protected:
	CFrameWnd*		m_pFrame;		// frame window I belong to
	CUIntArray		m_arToolbarID;	// array of toolbar IDs loaded
	CImageList		m_ilButtons;	// image list for all buttons
	CMapWordToPtr	m_mapIDtoImage;// maps command ID -> image list index
	CMapWordToPtr	m_mapIDtoAccel;// maps command ID -> ACCEL*
	HACCEL			m_hAccel;		// current accelerators, if any
	ACCEL*			m_pAccel;		// ..and table in memory
	CPtrList			m_menuList;		// list of HMENU's initialized
	CSize				m_szBitmap;		// size of button bitmap
	CSize				m_szButton;		// size of button (including shadow)
	CFont				m_fontMenu;		// menu font

	// helpers
	void DestroyAccel();
	void DrawMenuText(CDC& dc, CRect rc, CString text, COLORREF color);
	BOOL Draw3DCheckmark(CDC& dc, const CRect& rc, BOOL bSelected,
				HBITMAP hbmCheck=NULL);
	void ConvertMenu(CMenu* pMenu,UINT nIndex,BOOL bSysMenu,BOOL bShowButtons);
	void LoadAccel(HACCEL hAccel);
	BOOL AppendAccelName(CString& sItemName, UINT nID);
	CFont* GetMenuFont();

	// Get button index for given command ID, or -1 if not found
	int  GetButtonIndex(WORD nID) {
		void* val;
		return m_mapIDtoImage.Lookup(nID, val) ? (int)val : -1;
	}

	// Get ACCEL structure associated with a given command ID
	ACCEL* GetAccel(WORD nID) {
		void* val;
		return m_mapIDtoAccel.Lookup(nID, val) ? (ACCEL*)val : NULL;
	}

	// window proc to hook frame using CSubclassWnd implementation
	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);

	// CSubclassWnd message handlers 
	virtual void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
	virtual BOOL OnMeasureItem(LPMEASUREITEMSTRUCT lpms);
	virtual BOOL OnDrawItem(LPDRAWITEMSTRUCT lpds);
	virtual LONG OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	virtual void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
};

//////////////////
// Friendly version of MENUITEMINFO initializes itself
//
struct CMenuItemInfo : public MENUITEMINFO {
	CMenuItemInfo()
	{ memset(this, 0, sizeof(MENUITEMINFO));
	  cbSize = sizeof(MENUITEMINFO);
	}
};

#endif