///////////////////////////////////////////////////////////////////////////////
// MapView.cpp
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
#include "MapView.h"
#include "MainFrm.h"
#include "TreadDoc.h"
#include "ChildFrm.h"
#include "ObjPropView.h"
#include "InputLineDialog.h"
#include "splinetrack.h"
#include <mmsystem.h>

#include "System.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_SCALE		5000.0f

/////////////////////////////////////////////////////////////////////////////
// CMapView

IMPLEMENT_DYNCREATE(CMapView, CView)

CMapView::CMapView()
{
	m_hglrc = 0;
	m_pActiveDC = 0;
	m_nViewType = VIEW_TYPE_NONE;
	memset( &m_glState, 0, sizeof(m_glState) );

	R_glSet( &m_glState, _dtf_always|_dwm_on|_cfm_none|_cwm_all|_no_arrays, _beq_add|_bm_off );

	TexView.fMax = 64.0f;
	TexView.fYofs = 0.0f;
	TexView.pHoverShader = 0;

	m_bScrollDrag = false;
	m_bPaintDC = false;
	View.fMapCenterX = 0.0f;
	View.fMapCenterY = 0.0f;

	View.bGridVisible = true;
	View.bSelBoxVisible = false;

	SetScale( 0.5f );

	m_ViewParmsMenu.pView = this;

	View.fov = 90.0f;
	View.or3d.set_angles( vec3( 0, 0, 90 ) );
	View.pos3d = vec3( 0, -1024, 0 );
	View.clipdist = 8000.0f;
	View.bShowWireframe = false;
	View.bSelectionWireframe = true;
	View.bShaded = true;
	View.bShowEntityNames = true;
	View.bShowIcons = true;

	m_lpick = m_rpick = m_trackpick = 0;
	m_hcursor = 0;

	m_bSelBoxDrag = false;

	m_fGridSize = 16.0f;
	m_bGridSnap = true;

	m_lastclicktime = 0;

}

CMapView::~CMapView()
{
	if( m_hglrc != 0 )
		wglDeleteContext( m_hglrc );
}

float CMapView::GetGridSize()
{
	return m_fGridSize;
}

void CMapView::SetGridSize( float fSize )
{
	m_fGridSize = fSize;
}

void CMapView::SetGridSnap( bool on )
{
	m_bGridSnap = on;
}

bool CMapView::GetGridSnap()
{
	return m_bGridSnap;
}

CMapView::CViewParmsMenu::CViewParmsMenu() : CObjectMenu()
{
	pView = 0;

	//
	// grid.
	//
	AddMenuItem( 0, "Grid\nVisible" );
	AddMenuItem( 11, "Grid\nSnap" );
	AddMenuItem( 0, "Grid\n@SEP@" );
	AddMenuItem( 12, "Grid\n1" );
	AddMenuItem( 1, "Grid\n2" );
	AddMenuItem( 2, "Grid\n4" );
	AddMenuItem( 3, "Grid\n8" );
	AddMenuItem( 4, "Grid\n16" );
	AddMenuItem( 5, "Grid\n32" );
	AddMenuItem( 6, "Grid\n64" );
	AddMenuItem( 7, "Grid\n128" );
	AddMenuItem( 8, "Grid\n256" );
	AddMenuItem( 9, "Grid\n512" );
	AddMenuItem( 10, "Grid\n1024" );
	
	//
	// now do view types.
	//
	AddMenuItem( 0, "@SEP@" );
	AddMenuItem( 31, "Textures" );
	AddMenuItem( 0, "@SEP@" );
	AddMenuItem( 32, "Top" );
	AddMenuItem( 33, "Bottom" );
	AddMenuItem( 34, "Left" );
	AddMenuItem( 35, "Right" );
	AddMenuItem( 36, "Front" );
	AddMenuItem( 37, "Back" );
	AddMenuItem( 38, "Perspective" );
	AddMenuItem( 0, "@SEP@" );
	AddMenuItem( 39, "Wireframe" );
	AddMenuItem( 44, "Selected Wireframe" );
	AddMenuItem( 42, "Shaded" );
	AddMenuItem( 45, "Icons" );
	AddMenuItem( 43, "Entity Names" );
	AddMenuItem( 0, "@SEP@" );
	AddMenuItem( 40, "Set FOV...");
	AddMenuItem( 41, "Set Clipping Distance..." );
}

CMapView::CViewParmsMenu::~CViewParmsMenu()
{
}

void CMapView::KillTrackPicks( CPickObject* obj )
{
	bool nocapture = false;

	if( m_lpick == obj )
	{
		m_lpick = 0;
		nocapture = true;
	}

	if( m_rpick == obj )
	{
		m_rpick = 0;
		nocapture = true;
	}

	if( m_trackpick == obj )
	{
		m_trackpick = 0;
		nocapture = true;
	}

	Sys_SetCursor( this, TC_DEFAULT );

	if( nocapture )
		Sys_SetMouseCapture( 0 );
}

void CMapView::CViewParmsMenu::OnMenuItem( int id )
{
	switch( id )
	{
	case 0:

		for (int i = 0; i < 4; ++i)
		{
			Sys_GetActiveFrame()->GetMapView(i)->View.bGridVisible = !Sys_GetActiveFrame()->GetMapView(i)->View.bGridVisible;
			Sys_GetActiveFrame()->GetMapView(i)->RedrawWindow();
		}

	break;

	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:

		for (int i = 0; i < 4; ++i)
		{
			Sys_GetActiveFrame()->GetMapView(i)->SetGridSize(1<<id);
			Sys_GetActiveFrame()->GetMapView(i)->RedrawWindow();
		}
	
	break;

	case 12:

		for (int i = 0; i < 4; ++i)
		{
			Sys_GetActiveFrame()->GetMapView(i)->SetGridSize(1);
			Sys_GetActiveFrame()->GetMapView(i)->RedrawWindow();
		}

	break;

	case 11:
		pView->SetGridSnap( !pView->GetGridSnap() );
		pView->RedrawWindow();
	break;

	case 31:
		pView->SetViewType(VIEW_TYPE_TEXTURE);
		pView->CalcSize();
		pView->RedrawWindow();
	break;

	case 32:
		pView->SetViewType(VIEW_TYPE_TOP);
		pView->CalcSize();
		pView->RedrawWindow();
	break;

	case 33:
		pView->SetViewType(VIEW_TYPE_BOTTOM);
		pView->CalcSize();
		pView->RedrawWindow();
	break;

	case 34:
		pView->SetViewType(VIEW_TYPE_LEFT);
		pView->CalcSize();
		pView->RedrawWindow();
	break;

	case 35:
		pView->SetViewType(VIEW_TYPE_RIGHT);
		pView->CalcSize();
		pView->RedrawWindow();
	break;

	case 36:
		pView->SetViewType(VIEW_TYPE_FRONT);
		pView->CalcSize();
		pView->RedrawWindow();
	break;

	case 37:
		pView->SetViewType(VIEW_TYPE_BACK);
		pView->CalcSize();
		pView->RedrawWindow();
	break;

	case 38:
		pView->SetViewType(VIEW_TYPE_3D);
		pView->CalcSize();
		pView->RedrawWindow();
	break;

	case 39:
	case 42:
	case 43:
	case 44:
	case 45:

		if( id == 39 )
			pView->View.bShowWireframe = !pView->View.bShowWireframe;
		if( id == 42 )
			pView->View.bShaded = !pView->View.bShaded;
		if( id == 43 )
			pView->View.bShowEntityNames = !pView->View.bShowEntityNames;
		if( id == 44 )
			pView->View.bSelectionWireframe = !pView->View.bSelectionWireframe;
		if( id == 45 )
			pView->View.bShowIcons = !pView->View.bShowIcons;

		pView->RedrawWindow();

	break;

	case 40:
	case 41:
		{
			CInputLineDialog dlg;

			dlg.AllowEmpty(false);

			if( id == 40 )
			{
				CString s;
				s.Format( "%.2f", pView->View.fov );

				dlg.SetFilter( FILTER_FLOAT );
				dlg.SetTitle( "Set FOV" );
				dlg.SetValue( s );
			}
			else
			{
				CString s;
				s.Format( "%d", (int)pView->View.clipdist );

				dlg.SetFilter( FILTER_INT );
				dlg.SetTitle( "Set Clipping Distance" );
				dlg.SetValue( s );
			}

			if( dlg.DoModal() == IDOK )
			{
				float v = atof( dlg.GetValue() );

				if( id == 40 )
					pView->View.fov = v;
				else
					pView->View.clipdist = v;

				pView->RedrawWindow();
			}
		}
	break;
	}
}

void CMapView::CViewParmsMenu::OnUpdateCmdUI( int id, CCmdUI* pUI )
{
	pUI->Enable( TRUE );

	switch( id )
	{
	case 0:
		pUI->SetCheck( pView->View.bGridVisible && (pView->GetViewType()&VIEW_FLAG_2D) );
		pUI->Enable( pView->GetViewType()&VIEW_FLAG_2D );
	break;

	case 1:
		pUI->SetCheck( pView->GetGridSize() == 2.0f );
	break;

	case 2:
		pUI->SetCheck( pView->GetGridSize() == 4.0f );
	break;

	case 3:
		pUI->SetCheck( pView->GetGridSize() == 8.0f );
	break;

	case 4:
		pUI->SetCheck( pView->GetGridSize() == 16.0f );
	break;

	case 5:
		pUI->SetCheck( pView->GetGridSize() == 32.0f );
	break;

	case 6:
		pUI->SetCheck( pView->GetGridSize() == 64.0f );
	break;

	case 7:
		pUI->SetCheck( pView->GetGridSize() == 128.0f );
	break;

	case 8:
		pUI->SetCheck( pView->GetGridSize() == 256.0f );
	break;

	case 9:
		pUI->SetCheck( pView->GetGridSize() == 512.0f );
	break;

	case 10:
		pUI->SetCheck( pView->GetGridSize() == 1024.0f );
	break;

	case 12:
		pUI->SetCheck( pView->GetGridSize() == 1.0f );
	break;

	case 11:
		pUI->SetCheck( pView->GetGridSnap() );
	break;

	case 31:
		pUI->SetCheck( pView->GetViewType() == VIEW_TYPE_TEXTURE );
	break;

	case 32:
		pUI->SetCheck( pView->GetViewType() == VIEW_TYPE_TOP );
	break;

	case 33:
		pUI->SetCheck( pView->GetViewType() == VIEW_TYPE_BOTTOM );
	break;

	case 34:
		pUI->SetCheck( pView->GetViewType() == VIEW_TYPE_LEFT );
	break;

	case 35:
		pUI->SetCheck( pView->GetViewType() == VIEW_TYPE_RIGHT );
	break;

	case 36:
		pUI->SetCheck( pView->GetViewType() == VIEW_TYPE_FRONT );
	break;

	case 37:
		pUI->SetCheck( pView->GetViewType() == VIEW_TYPE_BACK );
	break;

	case 38:
		pUI->SetCheck( pView->GetViewType() == VIEW_TYPE_3D );
	break;

	case 39:
	case 42:
	case 44:
	case 45:

		pUI->Enable( pView->GetViewType() == VIEW_TYPE_3D );
		
		if( id == 39 )
			pUI->SetCheck( pView->View.bShowWireframe );
		if( id == 42 )
			pUI->SetCheck( pView->View.bShaded );
		if( id == 44 )
			pUI->SetCheck( pView->View.bSelectionWireframe );
		if( id == 45 )
			pUI->SetCheck( pView->View.bShowIcons );

	break;

	case 43:

		pUI->Enable();
		pUI->SetCheck( pView->View.bShowEntityNames );

	break;

	case 40:
	case 41:

		pUI->Enable( pView->GetViewType() == VIEW_TYPE_3D );

	break;
	}
}

BEGIN_MESSAGE_MAP(CMapView, CView)
	//{{AFX_MSG_MAP(CMapView)
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapView drawing

void CMapView::OnDraw(CDC* pDC)
{
	// TODO: add draw code here

	//
	// free any active DC.
	//
	UnbindGL();

	m_pActiveDC = pDC;
	m_bPaintDC = true;

	R_RenderWindow( this );
	
	m_bPaintDC = false;
	m_pActiveDC = 0;
}

void CMapView::SetScale( float scale )
{
	if( scale <= 0.0f )
		return;

	if( scale > MAX_SCALE )
		scale = MAX_SCALE;
	if( scale < 1.0f/MAX_SCALE )
		scale = 1.0f/MAX_SCALE;

	View.fScale = scale;
	View.fInvScale = 1.0f/scale;
}

glState_t* CMapView::GetGLState()
{
	return &m_glState;
}

void CMapView::SetViewType( int nType )
{
	if( nType != m_nViewType )
	{
		m_nLastViewType = nType;
		m_nViewType = nType;

		switch( nType )
		{
		case VIEW_TYPE_TOP:

			View.or2d.lft = -sysAxisX;
			View.or2d.up  =  sysAxisY;
			View.or2d.frw = -sysAxisZ;
			
		break;

		case VIEW_TYPE_BOTTOM:

			View.or2d.lft =  sysAxisX;
			View.or2d.up  = -sysAxisY;
			View.or2d.frw =  sysAxisZ;

		break;

		case VIEW_TYPE_LEFT:

			View.or2d.lft =  sysAxisY;
			View.or2d.up  =  sysAxisZ;
			View.or2d.frw =  sysAxisX;

		break;

		case VIEW_TYPE_RIGHT:

			View.or2d.lft = -sysAxisY;
			View.or2d.up  =  sysAxisZ;
			View.or2d.frw = -sysAxisX;

		break;

		case VIEW_TYPE_FRONT:

			View.or2d.lft = -sysAxisX;
			View.or2d.up  =  sysAxisZ;
			View.or2d.frw =  sysAxisY;

		break;

		case VIEW_TYPE_BACK:

			View.or2d.lft =  sysAxisX;
			View.or2d.up  =  sysAxisZ;
			View.or2d.frw = -sysAxisY;

		break;

		case VIEW_TYPE_3D:
		break;
		}

		GetDocument()->ClearAllTrackPicks();
	}
}

int CMapView::GetViewType()
{
	return m_nViewType;
}

void CMapView::SetLastViewType( int nLastViewType )
{
	m_nLastViewType = nLastViewType;
}

int CMapView::GetLastViewType()
{
	return m_nLastViewType;
}

void CMapView::RedrawWindow()
{
	CView::RedrawWindow( 0, 0, RDW_INVALIDATE|RDW_UPDATENOW );
}

void CMapView::CalcSize()
{
	//
	// should we even bother?
	//
	RECT r;

	GetClientRect( &r );
	View.fWinWidth = (float)r.right;
	View.fWinHeight = (float)r.bottom;
	View.fWinCenterX = View.fWinWidth/2.0f;
	View.fWinCenterY = View.fWinHeight/2.0f;
	
	if( m_nViewType != VIEW_TYPE_3D && 
		m_nViewType != VIEW_TYPE_TEXTURE )
	{
		//
		// MapCenter's should be filled in externally.
		//
		View.fMapWidth  = View.fWinWidth*View.fInvScale;
		View.fMapHeight = View.fWinHeight*View.fInvScale;
		
		float w = View.fMapWidth/2.0f;
		float h = View.fMapHeight/2.0f;

		w *= sign_vec(View.or2d.lft);
		h *= sign_vec(View.or2d.up);

		View.fMapLeft = View.fMapCenterX + w;
		View.fMapRight = View.fMapCenterX - w;
		View.fMapTop = View.fMapCenterY + h;
		View.fMapBottom = View.fMapCenterY - h;
		
		View.fMapMinX = MIN_VAL( View.fMapLeft, View.fMapRight );
		View.fMapMaxX = MAX_VAL( View.fMapLeft, View.fMapRight );
		View.fMapMinY = MIN_VAL( View.fMapTop, View.fMapBottom );
		View.fMapMaxY = MAX_VAL( View.fMapTop, View.fMapBottom );

	}
}

const char* CMapView::GetViewTypeString()
{
	switch( m_nViewType )
	{
	case VIEW_TYPE_3D:
		return "perspective";
	break;
	case VIEW_TYPE_TOP:
		return "top";
	break;
	case VIEW_TYPE_BOTTOM:
		return "bottom";
	break;
	case VIEW_TYPE_LEFT:
		return "left";
	break;
	case VIEW_TYPE_RIGHT:
		return "right";
	break;
	case VIEW_TYPE_FRONT:
		return "front";
	break;
	case VIEW_TYPE_BACK:
		return "back";
	break;
	case VIEW_TYPE_TEXTURE:
		return "texture";
	break;
	}

	return "shit";
}

/////////////////////////////////////////////////////////////////////////////
// CMapView diagnostics

#ifdef _DEBUG
void CMapView::AssertValid() const
{
	CView::AssertValid();
}

void CMapView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMapView message handlers

bool CMapView::SwapBuffers()
{
	if( !m_pActiveDC )
		return FALSE;

	::SwapBuffers( m_pActiveDC->m_hDC );

	return TRUE;
}

bool CMapView::BindGL()
{
	if( !m_pActiveDC )
	{
		m_pActiveDC = GetDC();
	}

	return wglMakeCurrent( m_pActiveDC->m_hDC, m_hglrc ) ? true : false;
}

bool CMapView::UnbindGL()
{
	if( m_pActiveDC && !m_bPaintDC )
	{
		ReleaseDC( m_pActiveDC );
		m_pActiveDC = 0;
	}

	return wglMakeCurrent( 0, 0 ) ? true : false;
}

bool CMapView::SetupGL()
{
	CDC* pDC = GetDC();

	m_hglrc = R_glCreateContext( pDC->m_hDC );

	if( m_hglrc )
	{
		wglShareLists( CMainFrame::GetGlobalRC(), m_hglrc );
		R_SetupBumpPal();
	}

	ReleaseDC( pDC );

	return m_hglrc != 0;
}

void CMapView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	SetupGL();
}

BOOL CMapView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	LPCTSTR cname = AfxRegisterWndClass(CS_OWNDC | CS_VREDRAW | CS_HREDRAW, LoadCursor(NULL, IDC_ARROW), NULL, LoadIcon(NULL, IDI_APPLICATION));
	cs.lpszClass = cname;

	return CView::PreCreateWindow(cs);
}

BOOL CMapView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_trackpick )
	{
		m_trackpick->MouseLeave( this, pt.x, pt.y, 0 );
		m_trackpick = 0;
	}

	GetDocument()->SetModifiedFlag();

	if( m_nViewType == VIEW_TYPE_TEXTURE )
	{
		float old = TexView.fMax;
		if( zDelta > 0 )
		{
			if( TexView.fMax < 2048.0f )
			{
				TexView.fMax *= 2.0f;
				//TexView.fYofs *= 1.20f;
			}
		}
		else
		{
			if( TexView.fMax > 64 )
			{
				TexView.fMax *= 0.5f;
				//TexView.fYofs *= 0.80f;
			}
		}

		if( TexView.fMax < 64 )
			TexView.fMax = 64;
		if( TexView.fMax > 2048 )
			TexView.fMax = 2048;

		if (TexView.fMax != old)
		{
			R_AdjustToViewTexture( this, GetDocument()->SelectedShader() );
			RedrawWindow();
		}
	}
	else
	if( m_nViewType&VIEW_FLAG_2D )
	{
		//
		// zoom in a bit.
		//
		if( zDelta > 0 )
			SetScale( View.fScale*1.5f );
		else
			SetScale( View.fScale*0.75f );
	
		CalcSize();
		RedrawWindow();
	}
	else
	if( m_nViewType == VIEW_TYPE_3D )
	{
		float dist;
				
		if( GetDocument()->ObjectsAreSelected() )
		{
			vec3 v = normalized( GetDocument()->m_selpos - View.pos3d );

			if( dot( v, View.or3d.frw ) > 0.3f )
			{
				//
				// zoom speed is based on object.
				//
				dist = vec_length( GetDocument()->m_selpos - View.pos3d );
				if( dist < 256.0f )
				{
					dist = 16.0f;
				}
				else
				{
					dist *= 0.5f;
				}
			}
			else
			{
				dist = 128.0f;
			}
		}
		else
		{
			dist = 128.0f;
		}

		if( zDelta > 0 )
		{
			vec3 v = View.or3d.frw * dist;
			View.pos3d += v;
		}
		else
		{
			vec3 v = View.or3d.frw * -dist;
			View.pos3d += v;
		}

		RedrawWindow();
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

CTreadDoc* CMapView::GetDocument()
{
	return (CTreadDoc*)CView::GetDocument();
}

void CMapView::OnMouseMove(UINT nFlags, CPoint point) 
{

	// TODO: Add your message handler code here and/or call default
	if( GetDocument() != Sys_GetActiveDocument() )
	{
		return;
	}

	if( ALTKEYDOWN() )
		nFlags |= MS_ALT;

	SetFocus();
	GetDocument()->GetChildFrame()->SetActiveView( this );

	View.selbox.right = point.x;
	View.selbox.bottom = point.y;

	if( m_bSelBoxDrag )
	{
		RedrawWindow();
		return;
	}

	if( m_nViewType == VIEW_TYPE_TEXTURE )
	{
		if( m_bScrollDrag )
		{
			//
			// how far did we move?
			//
			RECT r;
			GetClientRect( &r );
			int cy = r.bottom>>1;

			float dx = cy-point.y;
			dx *= 1.5f;
			float sgn = (dx < 0.0f) ? -1.0f : 1.0f;

			dx = (dx*dx);
			if (dx > 256.0f) { dx = 256.0f; }
			dx *= sgn;

			TexView.fYofs += dx;

			if( TexView.fYofs < 0 )
				TexView.fYofs = 0;
			
			if( point.y != cy )
			{
				RedrawWindow();
				CenterCursor();
			}
		}
		else
		{
			CShader* s = R_PickShader( this, point.x, point.y );

			if( (nFlags&MS_LBUTTON) )
			{
				if( GetDocument()->IsInTrackAnimationMode() )
				{
					return;
				}

				if( s )
				{
					GetDocument()->SetSelectedShader(s);

					if( GetDocument()->ObjectsAreSelected() )
					{
						GetDocument()->PaintSelection();
						Sys_RedrawWindows( VIEW_TYPE_3D );
					}
					Sys_RedrawWindows( VIEW_TYPE_TEXTURE );
				}
			}
			else
			{
				if( TexView.pHoverShader != s )
				{
					TexView.pHoverShader = s;
					RedrawWindow();
				}
			}
		}
	}
	else 
	if( m_nViewType&VIEW_FLAG_2D )
	{
		if( m_bScrollDrag )
		{
			RECT r;

			//
			// no usefull movement.
			//
			GetClientRect( &r );
			if( point.x == (r.right>>1) &&
				point.y == (r.bottom>>1) )
			{
				return;
			}

			//
			// how far did we move?
			//
			float dx, dy;

			dx = point.x-(r.right>>1);
			dy = point.y-(r.bottom>>1);
			
			dx *= 2.0f;
			dy *= 2.0f;

			dx *= View.fInvScale;
			dy *= View.fInvScale;

			//
			// flip for the view types.
			//
			if( sign_vec( View.or2d.lft ) < 0.0f )
				dx = -dx;
			if( sign_vec( View.or2d.up ) < 0.0f )
				dy = -dy;

			View.fMapCenterX += dx;
			View.fMapCenterY += dy;
			CalcSize();

			RedrawWindow();
			CenterCursor();
		}
		else
		{
			if( m_lpick )
			{
				m_lpick->MouseMove( this, point.x, point.y, nFlags );
			}
			else
			{

				//
				// wandering mouse cursor gives us coordinates.
				//
				char msg[512];
				vec3 pos;

				WinXYToVec3( point.x, point.y, &pos );
				if( GetGridSnap() )
					pos = Sys_SnapVec3( pos, GetGridSize() );

				sprintf(msg, "%s, X: %.2f, Y: %.2f, Z: %.2f", GetViewTypeString(), pos[0], pos[1], pos[2] );
				Sys_GetMainFrame()->SetMapCoordText( msg );

				//
				// track hotspots.
				//
				int picksize = (m_nViewType==VIEW_TYPE_3D)?DEFAULT_3D_PICK_SIZE:DEFAULT_2D_PICK_SIZE;

				CPickObject* pick = R_PickObject( this, point.x, point.y, picksize, picksize, PICK_MANIPULATORS );

				if( m_trackpick && pick != m_trackpick )
				{
					m_trackpick->MouseLeave( this, point.x, point.y, nFlags );
				}

				if( pick )
				{
					pick->MouseEnter( this, point.x, point.y, nFlags );
					m_trackpick = pick;
				}
				else
				{
					Sys_SetCursor( this, TC_DEFAULT );
					m_trackpick = 0;
				}
			}
		}
	}
	else
	if( m_nViewType == VIEW_TYPE_3D )
	{
		if( m_bScrollDrag )
		{
			RECT r;

			//
			// no usefull movement.
			//
			GetClientRect( &r );
			if( point.x == (r.right>>1) &&
				point.y == (r.bottom>>1) )
			{
				return;
			}

			//
			// how far did we move?
			//
			float dx, dy;

			dx = point.x-(r.right>>1);
			dy = point.y-(r.bottom>>1);
		
			if( nFlags&MK_CONTROL )
			{
				//
				// move!!!.
				//
				vec3 v = View.or3d.frw * -dy * 2;
				if( nFlags&MK_SHIFT )
				{
					v *= 4.0f;
				}
				
				View.pos3d += v;
				RedrawWindow();
			}
			else
			{
				m_bAltPressed = ALTKEYDOWN();

				if( m_bAltPressed )
				{
					bool do_orbit = GetDocument()->ObjectsAreSelected() && !GetDocument()->IsInTrackAnimationMode();
					
					if( m_bShiftPressedLast != ((nFlags&MS_SHIFT)?TRUE:FALSE) )
						m_bPressedLast = false;

					//
					// was it pressed last time?
					//
					if( !m_bPressedLast )
					{
						m_bDoMove = false;
						m_bShiftPressedLast = (nFlags&MS_SHIFT)?TRUE:FALSE;

						if( do_orbit && !(nFlags&MS_SHIFT) )
						{
							m_orbit = GetDocument()->m_selpos;
							m_bDoMove = true;
						}
						else
						{
							if( do_orbit || !(nFlags&MS_SHIFT) || GetDocument()->IsInTrackAnimationMode() )
								m_orbit = View.pos3d + vec3( 1, 0, 0 );
							else
							{
								m_orbit = View.pos3d + (View.or3d.frw*192);
								m_bDoMove = true;
							}
						}

						m_bPressedLast = true;
					}

					//
					// orbit.
					//
					vec3 pos = View.pos3d - m_orbit;
					
					dx = -dx*0.35f;
					dy = dy*0.35f;

					quat q;

					q = quat( sysAxisZ, DEGREES_TO_RADIANS(dx) ) *
						quat( View.or3d.lft, DEGREES_TO_RADIANS( dy ) );
					
					mat3x3 m;
					quaternion_to_matrix( &m, &q );
					
					if( m_bDoMove )
						View.pos3d = (pos*m)+m_orbit;

					View.or3d.m *= m;
					View.or3d.make_vecs();

					{
						View.or3d.delta_angles += vec3( 0.0f, dy, dx );
					}

					RedrawWindow();
				}
				else
				{
					m_bPressedLast = false;

					//
					// move us on up/left.
					//
					vec3 up = View.or3d.up  * dy / 0.5f;
					vec3 lf = View.or3d.lft * dx / 0.5f;

					View.pos3d += up+lf;

					RedrawWindow();
				}
			}
			CenterCursor();
		}
		else
		{
			if( m_lpick )
			{
				m_lpick->MouseMove( this, point.x, point.y, nFlags );
			}
			else
			{
				//
				// track hotspots.
				//
				int picksize = (m_nViewType==VIEW_TYPE_3D)?DEFAULT_3D_PICK_SIZE:DEFAULT_2D_PICK_SIZE;

				CPickObject* pick = R_PickObject( this, point.x, point.y, picksize, picksize, PICK_MANIPULATORS );

				if( m_trackpick && pick != m_trackpick )
				{
					m_trackpick->MouseLeave( this, point.x, point.y, nFlags );
				}

				if( pick )
				{
					pick->MouseEnter( this, point.x, point.y, nFlags );
					m_trackpick = pick;
				}
				else
				{
					Sys_SetCursor( this, TC_DEFAULT );
					m_trackpick = 0;
				}
			}
		}
	}
}

void CMapView::CenterCursor()
{
	RECT r;
	POINT p;

	GetClientRect( &r );
	
	p.x = r.right>>1;
	p.y = r.bottom>>1;

	ClientToScreen( &p );

	SetCursorPos( p.x, p.y );
}

void CMapView::OnMButtonDown(UINT nFlags, CPoint point) 
{

	if( m_trackpick )
	{
		m_trackpick->MouseLeave( this, point.x, point.y, 0 );
		m_trackpick = 0;
	}

	GetDocument()->SetModifiedFlag();

	if( m_nViewType&(VIEW_FLAG_2D|VIEW_TYPE_TEXTURE|VIEW_TYPE_3D) )
	{
		SetCapture();
		Sys_ShowCursor(FALSE);
		CenterCursor();
		m_bScrollDrag = true;
		m_bPressedLast = false;
		m_bAltPressed = ALTKEYDOWN() ? TRUE : FALSE;
	}
}

void CMapView::OnMButtonUp(UINT nFlags, CPoint point) 
{
	if( m_nViewType&(VIEW_FLAG_2D|VIEW_TYPE_TEXTURE|VIEW_TYPE_3D))
	{
		m_bScrollDrag = false;
		ReleaseCapture();
		Sys_ShowCursor(TRUE);
	}
}

void CMapView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetDocument()->SetModifiedFlag();

	if( ALTKEYDOWN() )
		nFlags |= MS_ALT;

	m_trackpick = 0;

	DWORD clicktime = timeGetTime();
	if( (clicktime-m_lastclicktime) < 800 )
	{
		int picksize = (m_nViewType==VIEW_TYPE_3D)?DEFAULT_3D_PICK_SIZE:DEFAULT_2D_PICK_SIZE;

		CPickObject* p = R_PickObject( this, point.x, point.y, picksize, picksize );
		if( p )
		{

			if( p->GetParent() )
				p = p->GetParent();

			CMapObject* m = dynamic_cast<CMapObject*>(p);
			if( m )
			{
				if( m->GetGroupUID() != -1 )
				{
					Sys_GetPropView()->TreeSelectObject( m->GetGroupUID() );
				}
				else
				{
					Sys_GetPropView()->TreeSelectObject( m->GetUID() );
				}
			}
			m_lastclicktime = 0; // clear so we have to click again.
		}
	}
	else
	{
		m_lastclicktime = clicktime;
	}

	View.selbox.left = point.x;
	View.selbox.top  = point.y;

	// TODO: Add your message handler code here and/or call default
	if( m_nViewType == VIEW_TYPE_TEXTURE )
	{
		if( GetDocument()->IsInTrackAnimationMode() )
		{
			::MessageBox( 0, "You cannot texture in animation mode!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
			return;
		}

		CShader* s = R_PickShader( this, point.x, point.y );

		if( s )
		{
			GetDocument()->SetSelectedShader(s);

			if( GetDocument()->ObjectsAreSelected() )
			{
				GetDocument()->GenericUndoRedoFromSelection()->SetTitle("Paint Selection");
				GetDocument()->PaintSelection();
				Sys_RedrawWindows( VIEW_TYPE_3D );
			}
			Sys_RedrawWindows( VIEW_TYPE_TEXTURE );
		}
	}
	else
	if( m_nViewType != VIEW_TYPE_TEXTURE )
	{
		bool redraw=false;

		int picksize = (m_nViewType==VIEW_TYPE_3D)?DEFAULT_3D_PICK_SIZE:DEFAULT_2D_PICK_SIZE;

		m_lpick = R_PickObject( this, point.x, point.y, picksize, picksize, PICK_MANIPULATORS );
		if( !m_lpick )
			m_lpick = R_PickObject( this, point.x, point.y, picksize, picksize );
				
		if( GetDocument()->IsEditingVerts() && !GetDocument()->IsInTrackAnimationMode() )
		{
			bool box = m_lpick == 0;

			if( m_lpick )
			{
				//
				// hack to see if selected in vertex mode...
				//
				CPickObject* test = m_lpick->GetParent();
				CMapObject* m = 0;
				
				if( !test )
					test = m_lpick;

				if( test )
					m = dynamic_cast<CMapObject*>(test);

				if( m_nViewType == VIEW_TYPE_3D )
				{
					//
					// if we're not holding shift then we can click on map objects.
					//
					if( !m )
					{
						m_lpick->MouseDown( this, point.x, point.y, nFlags|MS_LBUTTON );
						redraw = true;
					}
					else if( !(nFlags&MS_SHIFT) && !(nFlags&MS_ALT) && !m->IsSelected() )
					{
						m_lpick->MouseDown( this, point.x, point.y, nFlags|MS_LBUTTON );
						redraw = true;
					}
					else
					{
						box = true;
					} 
				}
				else
				{
					m_lpick->MouseDown( this, point.x, point.y, nFlags|MS_LBUTTON );
					redraw = true;
				}
			}

			if( box )
			{
				View.bSelBoxVisible = true;
				m_bSelBoxAlt = (nFlags&MS_ALT)?true:false;
				m_bSelBoxCtrl = (nFlags&MS_CONTROL)?true:false;
				m_bSelBoxDrag = true;
				SetCapture();
				m_lpick = 0;
			}
		}
		else
		{
			bool box = false;

			if( m_lpick )
			{
				//
				// if this is a map object, and we held shift/alt, then don't select, just bbox drag.
				//
				{
					CPickObject* p = m_lpick->GetParent();
					CMapObject* m = 0;

					if( !p )
						p = m_lpick;

					m = dynamic_cast<CMapObject*>(p);
					if( (m && ((m_nViewType==VIEW_TYPE_3D)&&(nFlags&MS_SHIFT))) || (nFlags&MS_ALT) )
					{
						box = true;
						if( !(nFlags&MS_CONTROL) && !(nFlags&MS_ALT) )
						{
							GetDocument()->MakeUndoDeselectAction();
							GetDocument()->ClearSelection();
							GetDocument()->UpdateSelectionInterface();
							GetDocument()->Prop_UpdateSelection();
						}
					}
				}

				if( !box )
				{
					// in animation mode?
					if( GetDocument()->IsInTrackAnimationMode() )
					{
						CPickObject* p = m_lpick->GetParent();
						CMapObject* m = 0;

						if( !p )
							p = m_lpick;

						m = dynamic_cast<CMapObject*>(p);

						if( m )
						{
							/*if( m->GetClass() == MAPOBJ_CLASS_SPLINETRACK )
							{
								m_lpick->MouseDown( this, point.x, point.y, nFlags|MS_LBUTTON );
							}
							else*/
							{
								m_lpick = 0;

								//
								// toggle selection.
								//
								if( nFlags&MS_CONTROL )
								{
									if( m->IsSelected() )
										m->Deselect( GetDocument() );
									else
										m->Select( GetDocument() );
								}
								else if( nFlags&MS_ALT )
								{
									m->Deselect( GetDocument() );
								}
								else
								{
									if( !m->IsSelected() )
									{
										GetDocument()->DeselectTypes( MAPOBJ_CLASS_ALL/*&~MAPOBJ_CLASS_SPLINETRACK*/, MAPOBJ_CLASS_ALL );
										m->Select( GetDocument() );
									}
								}
							}

							GetDocument()->BuildSelectionBounds();
							GetDocument()->Prop_UpdateSelection();
						}
						else
						{
							//
							// is it a brush manipulator?
							//
							//m_lpick->MouseDown( this, point.x, point.y, nFlags|MS_LBUTTON );

							//if( dynamic_cast<CQBrushHandle_Manipulator*>(p) != 0 )
							//{
							//	//
							//	// release it (so we can't drag the brush).
							//	//
							//	m_lpick->MouseUp( this, point.x, point.y, nFlags|MS_LBUTTON );
							//	m_lpick = 0;
							//}
						}
					}
					else
					{
						//
						// tell pick we clicked on it.
						//
						m_lpick->MouseDown( this, point.x, point.y, nFlags|MS_LBUTTON );
					}
				}

				redraw = true;
			}
			else
			{
				if( !(nFlags&MK_CONTROL) && !(nFlags&MS_ALT) )
				{
					GetDocument()->MakeUndoDeselectAction();
					GetDocument()->ClearSelection();
					GetDocument()->UpdateSelectionInterface();
					GetDocument()->Prop_UpdateSelection();
					redraw = true;
				}
				
				box = true;
			}

			if( box )
			{
				View.bSelBoxVisible = true;
				m_bSelBoxAlt = (nFlags&MS_ALT)?true:false;
				m_bSelBoxCtrl = (nFlags&MS_CONTROL)?true:false;
				m_bSelBoxDrag = true;
				SetCapture();
				m_lpick = 0;
			}
		}

		if( redraw )
			Sys_RedrawWindows();
	}

	CView::OnLButtonDown(nFlags, point);
}

void CMapView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( ALTKEYDOWN() )
		nFlags |= MS_ALT;

	if( m_bSelBoxDrag )
	{
		m_bSelBoxDrag = false;
		View.bSelBoxVisible = false;

		ReleaseCapture();

		//
		// in vertex mode?
		//
		if( GetDocument()->IsEditingVerts() && !GetDocument()->IsInTrackAnimationMode() )
		{
			if( !m_bSelBoxCtrl && !m_bSelBoxAlt )
				GetDocument()->ClearSelectedManipulators();

			//
			// select vertices.
			//
			CPickObject** list;
			int num;

			Sys_Sort( View.selbox.left, View.selbox.right, (int*)&View.selbox.left, (int*)&View.selbox.right );
			Sys_Sort( View.selbox.top, View.selbox.bottom, (int*)&View.selbox.top, (int*)&View.selbox.bottom );
			
			int cx, cy;
			int w, h;

			w = View.selbox.right-View.selbox.left;
			h = View.selbox.bottom-View.selbox.top;
		
			//w >>= 1;
			//h >>= 1;

			if( w == 0 || h == 0 )
			{
				Sys_RedrawWindows();
				return;
			}

			cx = View.selbox.left+(w>>1);
			cy = View.selbox.top+(h>>1);
			
			R_PickObjectList( this, cx, cy, w, h, &list, &num, PICK_MANIPULATORS );

			if( num > 0 )
			{
				/*int i;
				for(i = 0; i < num; i++)
				{
					CQBrushVertex_Manipulator* m = dynamic_cast<CQBrushVertex_Manipulator*>(list[i]);
					if( m )
					{
						if( m_bSelBoxAlt )
							GetDocument()->AddManipulatorToMap( m );
						else
							GetDocument()->AddManipulatorToSelection( m );
					}

					GetDocument()->UpdateSelectionInterface();
				}*/

				GetDocument()->GamePlugin()->ProcessVertexBoxSelection(GetDocument(), num, list, !m_bSelBoxAlt);

				R_DeletePickObjectList(list);
			}

			Sys_RedrawWindows();
		}
		else
		{
			//
			// select objects.
			//
			CPickObject** list;
			int num;

			Sys_Sort( View.selbox.left, View.selbox.right, (int*)&View.selbox.left, (int*)&View.selbox.right );
			Sys_Sort( View.selbox.top, View.selbox.bottom, (int*)&View.selbox.top, (int*)&View.selbox.bottom );
			
			int cx, cy;
			int w, h;

			w = View.selbox.right-View.selbox.left;
			h = View.selbox.bottom-View.selbox.top;
		
			//w >>= 1;
			//h >>= 1;

			if( w == 0 || h == 0 )
			{
				Sys_RedrawWindows();
				return;
			}

			cx = View.selbox.left+(w>>1);
			cy = View.selbox.top+(h>>1);
			
			R_PickObjectList( this, cx, cy, w, h, &list, &num, PICK_OBJECTS );

			if( num > 0 )
			{
				int i, k;
				int hitsize;

				hitsize = 0;
				CPickObject** hitlist = new CPickObject*[num];

				memset( hitlist, 0, sizeof(CPickObject*)*num );

				for(i = 0; i < num; i++)
				{
					CPickObject* p = list[i]->GetParent();
					if( !p ) p = list[i];

					CMapObject* obj = dynamic_cast<CMapObject*>(p);

					if( obj )
					{
						//
						// should we ignore this object?
						//
						for(k = 0; k < hitsize; k++)
						{
							if( hitlist[k] == obj )
								break;
						}
						
						if( k == hitsize )
						{

							if( m_bSelBoxAlt )
								obj->Deselect( GetDocument() );
							else
								obj->Select( GetDocument() );

							hitlist[hitsize++] = obj;
						}
					}
				}

				R_DeletePickObjectList(list);
				delete[] hitlist;

				GetDocument()->UpdateSelectionInterface();
				GetDocument()->Prop_UpdateSelection();
			}

			Sys_RedrawWindows();
		}
	}

	if( m_lpick )
	{
		m_lpick->MouseUp( this, point.x, point.y, nFlags|MS_LBUTTON );
		m_lpick = 0;
		Sys_SetCursor( this, TC_DEFAULT );
	}

	CView::OnLButtonUp(nFlags, point);
}

void CMapView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( ALTKEYDOWN() )
		nFlags |= MS_ALT;

	if( m_trackpick )
	{
		m_trackpick->MouseLeave( this, point.x, point.y, 0 );
		m_trackpick = 0;
	}

	GetDocument()->SetModifiedFlag();

	// TODO: Add your message handler code here and/or call default
	if( point.x < 100 && point.y < 30 )
	{
		Sys_DisplayObjectMenu( this, point.x, point.y, &m_ViewParmsMenu );
		return;
	}

	if( m_nViewType == VIEW_TYPE_TEXTURE )
	{
		/*if( nFlags&MS_ALT )
		{
			CShader* s = NewShader();
			Sys_SetShader( s );
			Sys_AdjustToViewTexture( s );
			Sys_RedrawWindows( VIEW_TYPE_TEXTURE );
			Sys_EditShader( s );
		}
		else
		{
			CShader* s = R_PickShader( this, point.x, point.y );
			Sys_EditShader( s );
		}*/

		return;
	}

	if( ALTKEYDOWN() )
	{
		Sys_CreateSelectedObject(this, point.x, point.y);
		Sys_RedrawWindows();
		return;
	}

	if( ALTKEYDOWN() )
		nFlags |= MS_ALT;

	int picksize = (m_nViewType==VIEW_TYPE_3D)?DEFAULT_3D_PICK_SIZE:DEFAULT_2D_PICK_SIZE;
	CPickObject* pick = R_PickObject( this, point.x, point.y, picksize, picksize, PICK_MANIPULATORS|PICK_OBJECTS|PICK_SELECTED_ONLY );
	if( pick )
	{
		//
		// if we're in track animation, only popup on a splinetrack.
		//
		if( GetDocument()->IsInTrackAnimationMode() )
		{
			CPickObject* parent = pick->GetParent();
			if( !parent || (dynamic_cast<CSplineTrack*>(parent) == 0) )
				return;
		}
		
		if( pick->PopupMenu( this, point.x, point.y, nFlags ) )
			return;
	}
	
	CView::OnRButtonDown(nFlags, point);
}

void CMapView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnRButtonUp(nFlags, point);
}

void CMapView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CalcSize();

	if( m_nViewType == VIEW_TYPE_TEXTURE )
	{
		R_AdjustToViewTexture( this, GetDocument()->SelectedShader() );
	}
}

void CMapView::WinXYToVec3( float x, float y, vec3* out )
{
	int nType;
	vec3 v = vec3::zero;

	if( m_nViewType == VIEW_TYPE_TEXTURE )
		return;

	if( m_nViewType == VIEW_TYPE_3D )
	{
		//
		// these are the rough distances in 3d space.
		// forward is not used, it can be added by whatever the caller wants.
		//
		*out = (View.or3d.up*(View.fWinCenterY-y)) + (View.or3d.lft*(View.fWinCenterX-x)) + View.pos3d;
		return;
	}

	x = (View.fWinCenterX-x)*View.fInvScale;
	y = (View.fWinCenterY-y)*View.fInvScale;

	if( m_nViewType == VIEW_TYPE_TOP || m_nViewType == VIEW_TYPE_BOTTOM )
	{
		nType = VIEW_TYPE_FRONT|VIEW_TYPE_BACK|VIEW_TYPE_LEFT|VIEW_TYPE_RIGHT;

		v[0] = sign_vec(View.or2d.lft) * x + View.fMapCenterX;
		v[1] = sign_vec(View.or2d.up)  * y + View.fMapCenterY;
	}
	else
	if( m_nViewType == VIEW_TYPE_FRONT || m_nViewType == VIEW_TYPE_BACK )
	{
		v[0] = sign_vec(View.or2d.lft) * x + View.fMapCenterX;
		v[2] = sign_vec(View.or2d.up)  * y + View.fMapCenterY;

		nType = VIEW_TYPE_TOP|VIEW_TYPE_BOTTOM|VIEW_TYPE_LEFT|VIEW_TYPE_RIGHT;
	}
	else
	if( m_nViewType == VIEW_TYPE_LEFT || m_nViewType == VIEW_TYPE_RIGHT )
	{
		v[1] = sign_vec(View.or2d.lft) * x + View.fMapCenterX;
		v[2] = sign_vec(View.or2d.up)  * y + View.fMapCenterY;

		nType = VIEW_TYPE_TOP|VIEW_TYPE_BOTTOM|VIEW_TYPE_FRONT|VIEW_TYPE_BACK;
	}

	int i;
	CMapView* pView;
	for(i = 0; i < 4; i++)
	{
		pView = GetDocument()->GetChildFrame()->GetMapView( i );
		if( pView->GetViewType()&nType )
			break;
	}

	if(i == 4)
	{
		*out = v;
		return;
	}

	nType = pView->GetViewType();

	if( m_nViewType == VIEW_TYPE_TOP || m_nViewType == VIEW_TYPE_BOTTOM )
	{
		v[2] = pView->View.fMapCenterY;
	}
	else
	if( m_nViewType == VIEW_TYPE_LEFT || m_nViewType == VIEW_TYPE_RIGHT )
	{
		v[0] = pView->View.fMapCenterX;
	}
	else
	{
		if( nType == VIEW_TYPE_TOP || nType == VIEW_TYPE_BOTTOM ) 
			v[1] = pView->View.fMapCenterY;
		else
			v[1] = pView->View.fMapCenterX;
	}

	*out = v;
}

void CMapView::MapXYToWinXY( float mx, float my, float* x, float* y )
{
	if( x )
	{
		mx -= View.fMapCenterX;
		*x = View.fWinCenterX-(sign_vec(View.or2d.lft)*mx*View.fScale);
	}
	if( y )
	{
		my -= View.fMapCenterY;
		*y = View.fWinCenterY-(sign_vec(View.or2d.up)*my*View.fScale);
	}
}

void CMapView::WinXYToMapXY( float winx, float winy, float* x, float* y )
{
	if( x )
	{
		winx = View.fWinCenterX-winx;
		*x = View.fMapCenterX+(sign_vec(View.or2d.lft)*winx*View.fInvScale);
	}
	if( y )
	{
		winy = View.fWinCenterY-winy;
		*y = View.fMapCenterY+(sign_vec(View.or2d.up)*winy*View.fInvScale);
	}
}

void CMapView::GetWinSelBox( int *minx, int *maxx, int* miny, int* maxy )
{
	if( m_nViewType == VIEW_TYPE_3D )
		return;

	float x1, x2, y1, y2;
	vec3 mins, maxs;

	mins = GetDocument()->m_selmins;
	maxs = GetDocument()->m_selmaxs;

	x1 = mins[QUICK_AXIS(View.or2d.lft)];
	x2 = maxs[QUICK_AXIS(View.or2d.lft)];
	y1 = mins[QUICK_AXIS(View.or2d.up)];
	y2 = maxs[QUICK_AXIS(View.or2d.up)];
	
	MapXYToWinXY( x1, y1, &x1, &y1 );
	MapXYToWinXY( x2, y2, &x2, &y2 );

	Sys_Sort( x1, x2, minx, maxx );
	Sys_Sort( y1, y2, miny, maxy );
}

BOOL CMapView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if(nHitTest == HTCLIENT && m_hcursor != NULL)
		SetCursor(m_hcursor);
	else
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	return TRUE;
}

void CMapView::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	if( m_trackpick )
	{
		m_trackpick->OnMouseLeave( this, 0, 0, 0, 0 );
		m_trackpick = 0;	
	}
}

void CMapView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CView::OnChar(nChar, nRepCnt, nFlags);
	
	if( nRepCnt > 1 )
		return;

	// TODO: Add your message handler code here and/or call default
	if( nChar == 'M' || nChar == 'm' )
	{
		if( GetViewType() == VIEW_TYPE_TEXTURE && GetLastViewType() != VIEW_TYPE_TEXTURE )
		{
			//
			// switch back.
			//
			SetViewType( GetLastViewType() );
			RedrawWindow();
		}
		else
		{
			int last = GetViewType();
			SetViewType( VIEW_TYPE_TEXTURE );
			SetLastViewType( last );
			RedrawWindow();
		}
	}
}
