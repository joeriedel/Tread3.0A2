///////////////////////////////////////////////////////////////////////////////
// MainFrm.h
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, Joe Riedel
// All rights reserved.
//
// Redistribution and use in source and binary forms, 
// with or without modification, are permitted provided 
// that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
//
// Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation and/or 
// other materials provided with the distribution. 
//
// Neither the name of the <ORGANIZATION> nor the names of its contributors may be 
// used to endorse or promote products derived from this software without specific 
// prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
// OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__59296110_F016_43CF_81AA_4955148149BB__INCLUDED_)
#define AFX_MAINFRM_H__59296110_F016_43CF_81AA_4955148149BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <cj60lib.h>
#include "StatusBarWithProgress.h"
#include "LinkedList.h"
#include "Math3d.h"

class CPluginFileImport;
class CMapObject;
class CConsoleDialog;
class CTreadDoc;
class CTrackAnimDialog;
class CCheckMapDialog;
class CShaderEditDialog;
class CObjectPropertiesDialog;

class CMainFrame : public CCJMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

private:

	HGLRC m_hglrc;
	CConsoleDialog* m_sys_console;
	CTrackAnimDialog* m_track_anim;
	CCheckMapDialog* m_checkmap_dlg;
	CObjectPropertiesDialog* m_objectproperties_dlg;

	BOOL InitStuff(void);

	CDC* m_globalGLDC;

public:

	static HGLRC GetGlobalRC();
	static void SetMapCoordText( const char* fmt, ... );
	static void SetStatusText( const char* fmt, ... );
	
	CStatusBarWithProgress* GetStatusBar();

	CMenu* GetObjectMenuPopup();
	CMenu* GetObjectCreateMenuPopup();

	static CTreadDoc* m_ClipBoardDoc;
	static CLinkedList<CMapObject> m_ClipBoard;
	static vec3 m_ClipBoardMins;
	static vec3 m_ClipBoardMaxs;
	static vec3 m_ClipBoardPos;

	CTrackAnimDialog* GetTrackAnimDialog();
	CCheckMapDialog* GetCheckMapDialog();
	CObjectPropertiesDialog* GetObjectPropertiesDialog();
	void DisplayWorldspawnProperties(CTreadDoc *doc);

	void GlobalGLLock();
	void GlobalGLUnlock();

	//
	// returns false if WM_QUIT was found.
	//
	BOOL PumpMessages();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBarWithProgress  m_wndStatusBar;
	CCJToolBar    m_wndToolBar;
	CCJTabCtrlBar m_wndObjProp;
	CMenu		m_ObjectCreateMenuPopup;
	CMenu		m_ObjectMenuPopup;
	CImageList m_TabImages;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnViewSystemconsole();
	afx_msg void OnUpdateViewSystemconsole(CCmdUI* pCmdUI);
	afx_msg void OnToolsReloadmodels();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void OnFileImportmap(CPluginFileImport *plugin);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__59296110_F016_43CF_81AA_4955148149BB__INCLUDED_)
