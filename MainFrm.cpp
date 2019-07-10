///////////////////////////////////////////////////////////////////////////////
// MainFrm.cpp
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

#include "stdafx.h"
#include "Tread.h"

#include "MainFrm.h"
#include "ConsoleDialog.h"
#include "ObjPropView.h"
#include "MapView.h"
#include "TreadDoc.h"
#include "TrackAnimDialog.h"
#include "CheckMapDialog.h"
#include "ObjectPropertiesDialog.h"

#include "files.h"
#include "shaders.h"
#include "texcache.h"
#include "r_gl.h"
#include "system.h"
#include "ents.h"

#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CCJMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CCJMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_SYSTEMCONSOLE, OnViewSystemconsole)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SYSTEMCONSOLE, OnUpdateViewSystemconsole)
	ON_COMMAND(ID_TOOLS_RELOADMODELS, OnToolsReloadmodels)
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	IDS_MAPCOORDS_STATUS
};

CTreadDoc* CMainFrame::m_ClipBoardDoc = 0;
CLinkedList<CMapObject> CMainFrame::m_ClipBoard;
vec3 CMainFrame::m_ClipBoardMins;
vec3 CMainFrame::m_ClipBoardMaxs;
vec3 CMainFrame::m_ClipBoardPos;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

BOOL CMainFrame::PumpMessages()
{
	MSG msg;

    //
    // Retrieve and dispatch any waiting messages.
    //
    while (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)) {
        if (!AfxGetApp ()->PumpMessage ()) {
            ::PostQuitMessage (0);
            return FALSE;
        }
    }

    //
    // Simulate the framework's idle processing mechanism.
    //
    LONG lIdle = 0;
    while (AfxGetApp ()->OnIdle (lIdle++));
    return TRUE;
}

CTrackAnimDialog* CMainFrame::GetTrackAnimDialog()
{
	return m_track_anim;
}

CCheckMapDialog* CMainFrame::GetCheckMapDialog()
{
	return m_checkmap_dlg;
}

HGLRC CMainFrame::GetGlobalRC()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	return pFrame->m_hglrc;
}

CMenu* CMainFrame::GetObjectMenuPopup()
{
	return &m_ObjectMenuPopup;
}

CMenu* CMainFrame::GetObjectCreateMenuPopup()
{
	return &m_ObjectCreateMenuPopup;
}

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_hglrc = 0;
	m_sys_console = 0;
	m_track_anim = 0;
	m_checkmap_dlg = 0;
	m_globalGLDC = 0;
	m_objectproperties_dlg = 0;
}

CMainFrame::~CMainFrame()
{
}

CObjectPropertiesDialog* CMainFrame::GetObjectPropertiesDialog()
{
	return m_objectproperties_dlg;
}

void CMainFrame::GlobalGLLock()
{
	m_globalGLDC = AfxGetMainWnd()->GetDC();
	wglMakeCurrent( m_globalGLDC->m_hDC, GetGlobalRC() );
}

void CMainFrame::GlobalGLUnlock()
{
	if( m_globalGLDC )
	{
		wglMakeCurrent( 0, 0 );
		AfxGetMainWnd()->ReleaseDC( m_globalGLDC );
		m_globalGLDC = 0;
	}
}

BOOL CMainFrame::InitStuff(void)
{
	Sys_printf("binding openGL...\n");

	//
	// setup basic GL.
	//
	CDC* pDC = AfxGetMainWnd()->GetDC();
	m_hglrc = R_glCreateContext( pDC->m_hDC );

	if( !m_hglrc )
	{
		OS_OkAlertMessage( "Error", "Unable to connect to OpenGL" );
		ReleaseDC( pDC );
		return FALSE;
	}

	wglMakeCurrent( pDC->m_hDC, m_hglrc );

	Sys_printf("initing burgerlib...\n");

#if 0
	MaxMemSize = 0xFFFFFFFF;
#else
	MaxMemSize = 8*1024*1024;
#endif
	DebugTraceFlag = DEBUGTRACE_THEWORKS;
	InitMemory();
	SetDefaultPrefixs();
	HardwareRandomize();

	{
		char cwd[1024];
		_getcwd(cwd, 1024);
		char *path = ConvertNativePathToPath(cwd);
		strcpy(cwd, path);
		DeallocAHandle(PrefixHandles[9]);
		PrefixHandles[9] = AllocAHandle(strlen(cwd)+1);
		path = (char*)LockAHandle(PrefixHandles[9]);
		strcpy(path, cwd);
		UnlockAHandle(PrefixHandles[9]);
	}
	DeleteAFile( "9:LogFile.txt" );

	Sys_printf("initing file system...\n");

	if(FS_Init())
		goto ohshit;
	
	FS_SetBase("9:");
	
	//
	// parse entities.
	//
	//if( ParseEntDefs() == false )
	//	goto ohshit;

	if( R_glStartup() )
	{
		OS_OkAlertMessage( "Error", "Unable to intialize openGL system" );
		goto ohshit;
	}

	Sys_printf("initing texture cache...\n");

	if( TC_Init() )
	{
		OS_OkAlertMessage( "Error", "Unable to initialize texture cache" );
		goto ohshit;
	}

	R_glPrecacheTextures();

	//
	// setup font bitmaps.
	//
	{
		CFont font;

		font.CreateFont(12, 0, 0, 0, FW_BOLD, false, false, false, 
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS Sans Serif");

		CFont* pOld = pDC->SelectObject( &font );
		wglUseFontBitmapsA( pDC->m_hDC, 0, 255, TEXTURE_NAME_FONT );
		wglUseFontBitmapsA( pDC->m_hDC, 0, 255, TEXTURE_NAME_FONT );
		pDC->SelectObject( pOld );
	}
	{
		CFont font;

		font.CreateFont(14, 0, 0, 0, FW_NORMAL, false, false, false, 
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Tahoma");

		CFont* pOld = pDC->SelectObject( &font );
		wglUseFontBitmapsA( pDC->m_hDC, 0, 255, VIEWTYPE_FONT );
		wglUseFontBitmapsA( pDC->m_hDC, 0, 255, VIEWTYPE_FONT );
		pDC->SelectObject( pOld );
	}
	{
		CFont font;

		font.CreateFont(14, 0, 0, 0, FW_NORMAL, false, false, false, 
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Courier New");

		CFont* pOld = pDC->SelectObject( &font );
		wglUseFontBitmapsA( pDC->m_hDC, 0, 255, BOXSIZE_FONT );
		wglUseFontBitmapsA( pDC->m_hDC, 0, 255, BOXSIZE_FONT );
		pDC->SelectObject( pOld );
	}
	
	Sys_Init();
	
	Sys_printf("done.\n");

	wglMakeCurrent( 0, 0 );
	ReleaseDC( pDC );

	return TRUE;

ohshit:

	wglMakeCurrent( 0, 0 );
	ReleaseDC( pDC );

	return FALSE;
}

CStatusBarWithProgress* CMainFrame::GetStatusBar()
{
	return &m_wndStatusBar;
}

void CMainFrame::DisplayWorldspawnProperties(CTreadDoc *doc)
{
	CObjectPropertiesDialog *temp = m_objectproperties_dlg;
	/*BOOL wasVisible = temp->IsWindowVisible();
	temp->ShowWindow(SW_HIDE);*/
	m_objectproperties_dlg = new CObjectPropertiesDialog(doc, this);
	m_objectproperties_dlg->DoModal();
	delete m_objectproperties_dlg;
	m_objectproperties_dlg = temp;
	doc->Prop_UpdateSelection();
	/*if (wasVisible)
	{
		temp->ShowWindow(SW_SHOW);
	}*/
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCJMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	Sys_RegisterMainFrame( this );

	m_sys_console = new CConsoleDialog();
	m_sys_console->Create( IDD_CONSOLE_DIALOG, this );
	m_sys_console->SetWindowText( "Tread Startup..." );
	m_sys_console->ShowWindow( SW_SHOW );
	m_sys_console->AllowHide( FALSE );
	m_sys_console->SetFocus();
	m_sys_console->RedrawWindow();
	
	Sys_SetConsole( m_sys_console );
	Sys_printf( "loading controls...\n" );

	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetPaneInfo( 1, IDS_MAPCOORDS_STATUS, SBPS_NORMAL, 300 );
	m_wndStatusBar.SetProgressBarWidth( 400 );
	m_wndStatusBar.SetStep( 1 );

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	m_track_anim = new CTrackAnimDialog();
	m_track_anim->Create( IDD_TRACK_DIALOG, this );
	m_track_anim->Load( 0, 0, 0 );

	m_checkmap_dlg = new CCheckMapDialog();
	m_checkmap_dlg->Create( IDD_CHECKMAP_DIALOG, this );

	m_objectproperties_dlg = new CObjectPropertiesDialog(0);
	m_objectproperties_dlg->Create( IDD_OBJECT_PROPERTIES_DIALOG, this );
	m_objectproperties_dlg->EnableControls(FALSE);

	m_ObjectMenuPopup.CreatePopupMenu();
	m_ObjectCreateMenuPopup.CreatePopupMenu();

	EnableDocking( CBRS_ALIGN_ANY );

	//SetInitialSize(200,200,180,180);

	if (!m_wndObjProp.Create(this, ID_VIEW_OBJECTPROPERTIES,
		_T("A Docking Bar"), CSize(283,600), CBRS_RIGHT))
	{
		TRACE0("Failed to create dialog bar m_wndObjProp\n");
		return false;		// fail to create
	}

	m_TabImages.Create(IDB_TAB1, 18, 0, RGB(255,0,255));
	m_wndObjProp.SetTabImageList(&m_TabImages);

//	m_wndObjProp.ShowFrameControls(false, false);
	m_wndObjProp.AddView(_T(""), RUNTIME_CLASS(CObjPropView));
//	m_wndObjProp.SetViewToolTip(0, "Textures");
//	m_wndObjProp.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndObjProp.EnableDocking( CBRS_ALIGN_ANY );

	DockControlBar( &m_wndObjProp, AFX_IDW_DOCKBAR_RIGHT );
	ShowControlBar( &m_wndObjProp, true, false );

//	EnableDockingSizeBar( CBRS_ALIGN_ANY );

//	DockSizeBar(&m_wndObjProp);
	
	m_wndObjProp.SetActiveView(0);

	if( InitStuff() == FALSE )
		return -1;

	m_sys_console->AllowHide( TRUE );
	m_sys_console->ShowWindow( SW_HIDE );

	return 0;
}

void CMainFrame::SetStatusText( const char* fmt, ... )
{
	va_list l;
	char buff[2084];

	va_start( l, fmt );
	vsprintf( buff, fmt, l );
	va_end( l );

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if( pFrame )
	{
		pFrame->m_wndStatusBar.SetPaneText( 0, buff );
	}
}

void CMainFrame::SetMapCoordText( const char* fmt, ... )
{
	va_list l;
	char buff[2084];

	va_start( l, fmt );
	vsprintf( buff, fmt, l );
	va_end( l );

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if( pFrame )
	{
		pFrame->m_wndStatusBar.SetPaneText( 1, buff );
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CCJMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CCJMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CCJMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	Sys_SetConsole( 0 );
	delete m_sys_console;
	delete m_track_anim;
	delete m_checkmap_dlg;
	delete m_objectproperties_dlg;

	// TODO: Add your message handler code here and/or call default
	CDC* pDC = GetDC();

	wglMakeCurrent( pDC->m_hDC, m_hglrc );

	//
	// shutdown the sytem.
	//
	TC_Shutdown();
	R_glShutdown();

	wglMakeCurrent( 0, 0 );
	ReleaseDC( pDC );

	if( m_hglrc )
	{
		wglDeleteContext( m_hglrc );
	}	

	Sys_Shutdown();

	return CCJMDIFrameWnd::DestroyWindow();
}

void CMainFrame::OnClose() 
{
	

	CCJMDIFrameWnd::OnClose();
}

//
// The fricken accelerator table chews up everything, no matter who has the focus, so we do most of our key
// acceleration by hand here.
//
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if( CWnd::GetFocus() && CWnd::GetFocus()->IsKindOf( RUNTIME_CLASS( CMapView) ) )
	{
		//
		// key message?
		//
		if( pMsg->message == WM_KEYDOWN )
		{
			//
			// is this a repeat?
			//
			if( pMsg->lParam&(1<<30) )
			{
				return true;
			}

			if( Sys_GetActiveDocument() != 0 )
			{
				switch( pMsg->wParam )
				{
				case 0x57: SendMessage( WM_COMMAND, ID_VIEW_EXPANDVIEW ); return true; // W
				case VK_DELETE: SendMessage( WM_COMMAND, ID_EDIT_DELETE ); return true;
				case 0xBF: SendMessage( WM_COMMAND, ID_TOOLS_CENTERONSELECTION ); return true; // '/'
				//case 0xBC: SendMessage( WM_COMMAND, ID_TOOLS_ANIMATIONMODE ); return true; // ,
				case 0x31: SendMessage( WM_COMMAND, ID_VIEW_VERTICES ); return true; // 1
				case 0x32: SendMessage( WM_COMMAND, ID_VIEW_FACES ); return true; // 2
				case 0x55: SendMessage( WM_COMMAND, ID_TOOLS_SELECTOBJECTBYUID ); return true; // U
				case 0xDB: SendMessage( WM_COMMAND, ID_VIEW_TOGGLEBRUSHES ); return true; // [
				case 0xDC: SendMessage( WM_COMMAND, ID_SELECTION_OPENGROUPMODE ); return true; // '\'
				//case 0xDD: SendMessage( WM_COMMAND, ID_VIEW_HIDESHOWJMODELS ); return true; // ]
				case 0xBE: SendMessage( WM_COMMAND, ID_VIEW_PROPERTIES ); return true; // '.'
					
				case VK_UP:
				case VK_DOWN:
				case VK_LEFT:
				case VK_RIGHT:
					{
						int key = pMsg->wParam;
						int dirbits = (key==VK_UP)?NUDGE_UP:
						(key==VK_DOWN)?NUDGE_DOWN:
						(key==VK_LEFT)?NUDGE_LEFT:NUDGE_RIGHT;

						Sys_GetActiveDocument()->NudgeSelection( dirbits );
					}
				break;
				}
			}
		}
	}

	return CCJMDIFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnViewSystemconsole() 
{
	// TODO: Add your command handler code here
	
	if( m_sys_console->IsWindowVisible() )
		m_sys_console->ShowWindow( SW_HIDE );
	else
		m_sys_console->ShowWindow( SW_SHOW );
}

void CMainFrame::OnUpdateViewSystemconsole(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_sys_console->IsWindowVisible() );
}

void CMainFrame::OnToolsReloadmodels() 
{
	// TODO: Add your command handler code here
	m_sys_console->ShowWindow( SW_SHOW );
	Sys_ReloadStuff();
	m_sys_console->ShowWindow( SW_HIDE );
}

void CMainFrame::OnFileImportmap(CPluginFileImport *plugin) 
{
	// TODO: Add your command handler code here
	CString sPath;
	sPath.Format("%s Files (*.%s)|*.%s|All Files (*.*)|*.*||", plugin->Type(), plugin->Extension(), plugin->Extension());
	CFileDialog dlgTemp(true, plugin->Extension(), NULL,
			OFN_NONETWORKBUTTON|OFN_HIDEREADONLY, sPath, this);
	
	CString title = CString("Import ") + plugin->Type();
	dlgTemp.m_ofn.lpstrTitle = title;

	if(dlgTemp.DoModal() != IDOK)
		return;

	sPath = dlgTemp.GetPathName();

	CTreadApp* pApp = (CTreadApp*)AfxGetApp();

	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();
	CString sTemp;

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = pApp->GetNextDocTemplate(curTemplatePos);

		curTemplate->GetDocString(sTemp, CDocTemplate::docName);
		if(sTemp == "Map")
		{
			Sys_SetImporter(plugin);
			curTemplate->OpenDocumentFile(sPath);
			return;
		}
	}
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if( pHandlerInfo == 0 )
	{
		if( (nID >= ID_PLUGIN_GLOBAL_MENU_MIN && nID <= ID_PLUGIN_GLOBAL_MENU_MAX) )
		{
			if( nCode == CN_COMMAND )
			{
				Sys_OnPluginCommand(nID);
				return true;
			}
			else
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				static_cast<CCmdUI*>(pExtra)->Enable();
				return true;
			}
		}
	}

	// TODO: Add your specialized code here and/or call the base class
	return CCJMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
