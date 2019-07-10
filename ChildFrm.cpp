///////////////////////////////////////////////////////////////////////////////
// ChildFrm.cpp
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
#include "ObjPropView.h"

#include "ChildFrm.h"
#include "MapView.h"
#include "TreadDoc.h"
#include "system.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_EXPANDVIEW, OnViewExpandview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPANDVIEW, OnUpdateViewExpandview)
	//}}AFX_MSG_MAP
	ON_WM_INITMENU()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	m_bSplitterCreated = false;
	m_pActiveView = 0;
	m_bExpanded = false;
	m_bMapLoad = false;
	m_nSizeMsgCount = 0;
}

CChildFrame::~CChildFrame()
{
}

void CChildFrame::GetRowColSizes( int* sizes )
{
	//
	// save out the sizes.
	//
	int crap;

	m_MainSplitter.GetColumnInfo( 0, sizes[0], crap );
	m_MainSplitter.GetColumnInfo( 1, sizes[1], crap );
	m_SubSplit[0].GetRowInfo( 0, sizes[2], crap );
	m_SubSplit[0].GetRowInfo( 1, sizes[3], crap );
	m_SubSplit[1].GetRowInfo( 0, sizes[4], crap );
	m_SubSplit[1].GetRowInfo( 1, sizes[5], crap );
}

void CChildFrame::SetRowColSizes( int* sizes, bool map_load )
{
	memcpy( m_nViewSizes, sizes, sizeof(int)*6 );

	if( !map_load )
	{
		m_MainSplitter.SetColumnInfo( 0, sizes[0], 10 );
		m_MainSplitter.SetColumnInfo( 1, sizes[1], 10 );
		m_SubSplit[0].SetRowInfo( 0, sizes[2], 10 );
		m_SubSplit[0].SetRowInfo( 1, sizes[3], 10 );
		m_SubSplit[1].SetRowInfo( 0, sizes[4], 10 );
		m_SubSplit[1].SetRowInfo( 1, sizes[5], 10 );

		m_MainSplitter.RecalcLayout();
	}
	
	m_bMapLoad = map_load;
}

void CChildFrame::ExpandCollapseView()
{
	if( m_pActiveView )
	{
		RECT r;
		GetClientRect( &r );

		if( m_bExpanded )
		{
			SetRowColSizes( m_nViewSizes );
		}
		else
		{
			GetRowColSizes( m_nViewSizes );

			int row;
			int col;
			
			row = ((m_pMapViews[0]==m_pActiveView)||(m_pMapViews[1]==m_pActiveView))?0:1;
			col = ((m_pMapViews[0]==m_pActiveView)||(m_pMapViews[2]==m_pActiveView))?0:1;

			m_MainSplitter.SetColumnInfo( col^1, 0, 10 );
			m_MainSplitter.SetColumnInfo( col, r.right, 10 );
			m_SubSplit[col].SetRowInfo( row^1, 0, 10 );
			m_SubSplit[col].SetRowInfo( row, r.bottom, 10 );

			m_MainSplitter.RecalcLayout();
		}

		m_bExpanded = !m_bExpanded;
	}
}

CMapView* CChildFrame::GetActiveView()
{
	return m_pActiveView;
}

void CChildFrame::SetActiveView( CMapView* pView )
{
	m_pActiveView = pView;
}

CMapView* CChildFrame::GetMapView( int num )
{
	if( num < 0 || num > 3 )
		return 0;

	return m_pMapViews[num];
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	return TRUE;
}

void CChildFrame::ActivateFrame(int nCmdShow)
{
	// TODO: Modify this function to change how the frame is activated.

	nCmdShow = SW_SHOWMAXIMIZED;
	CMDIChildWnd::ActivateFrame(nCmdShow);
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	int nWidth, nHeight;
	
	//
	// the fractions were developed by hand testing.
	//
	nWidth  = (lpcs->cx + (lpcs->cx>>2))>>1;
	nHeight = (lpcs->cy + (lpcs->cy>>1))>>1;

	//
	// setup our splitter windows.
	//
	if( m_MainSplitter.CreateStatic( this, 1, 2 ) == false )
	{
		return false;
	}
	
	m_SubSplit[0].CreateStatic(&m_MainSplitter, 2, 1, WS_CHILD | WS_VISIBLE,
										m_MainSplitter.IdFromRowCol(0, 0));
	m_SubSplit[1].CreateStatic(&m_MainSplitter, 2, 1, WS_CHILD | WS_VISIBLE,
										m_MainSplitter.IdFromRowCol(0, 1));

	/*m_MainSplitter.CreateView( 0, 0, RUNTIME_CLASS(CMapView), CSize(0, 0), pContext );
	m_MainSplitter.CreateView( 0, 1, RUNTIME_CLASS(CMapView), CSize(0, 0), pContext );
	m_MainSplitter.CreateView( 1, 0, RUNTIME_CLASS(CMapView), CSize(0, 0), pContext );
	m_MainSplitter.CreateView( 1, 1, RUNTIME_CLASS(CMapView), CSize(0, 0), pContext );*/
	
	m_SubSplit[0].CreateView( 0, 0, RUNTIME_CLASS(CMapView), CSize(0, 0), pContext );
	m_SubSplit[0].CreateView( 1, 0, RUNTIME_CLASS(CMapView), CSize(0, 0), pContext );
	m_SubSplit[1].CreateView( 0, 0, RUNTIME_CLASS(CMapView), CSize(0, 0), pContext );
	m_SubSplit[1].CreateView( 1, 0, RUNTIME_CLASS(CMapView), CSize(0, 0), pContext );

	/*m_MainSplitter.SetRowInfo( 0, nHeight, 10 );
	m_MainSplitter.SetRowInfo( 1, nHeight, 10 );
	m_MainSplitter.SetColumnInfo( 0, nWidth, 10 );
	m_MainSplitter.SetColumnInfo( 1, nWidth, 10 );
	m_MainSplitter.RecalcLayout();*/
	int sizes[6];

	sizes[0] = nWidth;
	sizes[1] = nWidth;
	sizes[2] = nHeight;
	sizes[3] = nHeight;
	sizes[4] = nHeight;
	sizes[5] = nHeight;

	SetRowColSizes( sizes );

	/*m_pMapViews[0] = (CMapView*)m_MainSplitter.GetPane( 0, 0 );
	m_pMapViews[1] = (CMapView*)m_MainSplitter.GetPane( 0, 1 );
	m_pMapViews[2] = (CMapView*)m_MainSplitter.GetPane( 1, 0 );
	m_pMapViews[3] = (CMapView*)m_MainSplitter.GetPane( 1, 1 );*/

	m_pMapViews[0] = (CMapView*)m_SubSplit[0].GetPane( 0, 0 );
	m_pMapViews[1] = (CMapView*)m_SubSplit[1].GetPane( 0, 0 );
	m_pMapViews[2] = (CMapView*)m_SubSplit[0].GetPane( 1, 0 );
	m_pMapViews[3] = (CMapView*)m_SubSplit[1].GetPane( 1, 0 );

	((CTreadDoc*)m_pMapViews[0]->GetDocument())->SetChildFrame( this );

	m_pMapViews[0]->SetViewType( VIEW_TYPE_FRONT );
	m_pMapViews[1]->SetViewType( VIEW_TYPE_TOP );
	m_pMapViews[2]->SetViewType( VIEW_TYPE_TEXTURE );
	m_pMapViews[3]->SetViewType( VIEW_TYPE_3D );

	m_pMapViews[0]->CalcSize();
	m_pMapViews[0]->RedrawWindow();
	m_pMapViews[1]->CalcSize();
	m_pMapViews[1]->RedrawWindow();
	m_pMapViews[2]->CalcSize();
	m_pMapViews[2]->RedrawWindow();
	m_pMapViews[3]->CalcSize();
	m_pMapViews[3]->RedrawWindow();

	m_bSplitterCreated = true;

	return true;
}

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( m_bSplitterCreated == false )
		return;

	m_nSizeMsgCount++;

	if( m_nSizeMsgCount == 3 && m_bMapLoad )
	{
		SetRowColSizes( m_nViewSizes, false );
		return;
	}

	int nWidth, nHeight;

	nWidth = cx>>1;
	nHeight = cy>>1;

	int sizes[6];
	sizes[0] = nWidth;
	sizes[1] = nWidth;
	sizes[2] = nHeight;
	sizes[3] = nHeight;
	sizes[4] = nHeight;
	sizes[5] = nHeight;
	
	m_MainSplitter.SetColumnInfo( 0, sizes[0], 10 );
	m_MainSplitter.SetColumnInfo( 1, sizes[1], 10 );
	m_SubSplit[0].SetRowInfo( 0, sizes[2], 10 );
	m_SubSplit[0].SetRowInfo( 1, sizes[3], 10 );
	m_SubSplit[1].SetRowInfo( 0, sizes[4], 10 );
	m_SubSplit[1].SetRowInfo( 1, sizes[5], 10 );

	m_MainSplitter.RecalcLayout();

	m_bExpanded = false;
}

void CChildFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	if( m_bSplitterCreated )
	{
		if (m_pMapViews[0]->GetDocument()->OnGainFocus())
		{
			//SetTimer(1, 500, 0);
			Sys_BuildPluginMenu(Sys_GetMainFrame()->GetMenu(), m_pMapViews[0]->GetDocument());
		}
	}
}

void CChildFrame::OnViewExpandview() 
{
	// TODO: Add your command handler code here
	ExpandCollapseView();
}

void CChildFrame::OnUpdateViewExpandview(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( TRUE );
	pCmdUI->SetText( m_bExpanded ? "Collapse View" : "Expand View" );
}

void CChildFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CMDIChildWnd::OnTimer(nIDEvent);

	if (nIDEvent == 1)
	{
		KillTimer(1);
		Sys_BuildPluginMenu(Sys_GetMainFrame()->GetMenu(), m_pMapViews[0]->GetDocument());
	}
}
