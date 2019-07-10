////////////////////////////////////////////////////////////////
// 1997 Microsoft Sytems Journal
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
//

#if !defined(SUBCLASS_H_INCLUDED)
#define SUBCLASS_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//////////////////
// Generic class to hook messages on behalf of a CWnd.
// Once hooked, all messages go to CSubclassWnd::WindowProc before going
// to the window. Specific subclasses can trap messages and do something.
//
// To use:
//
// * Derive a class from CSubclassWnd.
//
// * Override CSubclassWnd::WindowProc to handle messages. Make sure you call
//   CSubclassWnd::WindowProc if you don't handle the message, or your
//   window will never get messages. If you write seperate message handlers,
//   you can call Default() to pass the message to the window.
//
// * Instantiate your derived class somewhere and call HookWindow(pWnd)
//   to hook your window, AFTER it has been created.
//	  To unhook, call HookWindow(NULL).
//
// This is a very important class, crucial to many of the widgets Window
// widgets implemented in PixieLib. To see how it works, look at the HOOK
// sample program.
//

class CLASS_EXPORT CSubclassWnd : public CObject {
public:
	DECLARE_DYNAMIC(CSubclassWnd);
	CSubclassWnd();
	~CSubclassWnd();

	// Subclass a window. Hook(NULL) to unhook (automatic on WM_NCDESTROY)
	BOOL	HookWindow(CWnd* pRealWnd);
	BOOL	IsHooked()			{ return m_pWndHooked!=NULL; }

	friend LRESULT CALLBACK HookWndProc(HWND, UINT, WPARAM, LPARAM);
	friend class CSubclassWndMap;

protected:
	CWnd*				m_pWndHooked;		// the window hooked
	WNDPROC			m_pOldWndProc;		// ..and original window proc
	CSubclassWnd*	m_pNext;				// next in chain of hooks for this window

	// Override this to handle messages in specific handlers
	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);
	LRESULT Default();				// call this at the end of handler fns
};

#endif // SUBCLASS_H_INCLUDED

