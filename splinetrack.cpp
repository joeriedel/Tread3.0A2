///////////////////////////////////////////////////////////////////////////////
// splinetrack.cpp
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
#include "system.h"
#include "splinetrack.h"
#include "mapview.h"
#include "TreadDoc.h"
#include "resource.h"
#include "mapfile.h"

#define SEGMENT_SUBDIVIDE_SIZE	50

//////////////////////////////////////////////////////////////////////
// CSplineControlPoint_Manipulator3D								//
//////////////////////////////////////////////////////////////////////
CSplineControlPoint_Manipulator3D::CSplineControlPoint_Manipulator3D() : CVec3D_Manipulator()
{
	m_bMoved = false;
	src = 0;
}

CSplineControlPoint_Manipulator3D::~CSplineControlPoint_Manipulator3D()
{
}

void CSplineControlPoint_Manipulator3D::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bMoved = false;
	CVec3D_Manipulator::OnMouseDown( pView, nMX, nMY, nButtons, pSrc );
}

bool CSplineControlPoint_Manipulator3D::OnDrag( CMapView* pView, int nButtons, const vec3& move )
{
	vec3 t;

	if( pView->GetGridSnap() )
	{
		t = Sys_SnapVec3( move, pView->GetGridSize() );
		if( equals( t, vec3::zero, 0.0001f ) )
			return false;
	}
	else
	{
		t = move;
	}

	if( !m_bMoved )
	{
		pView->GetDocument()->GenericUndoRedoFromSelection()->SetTitle( "SplineTrack Manipulation" );
		m_bMoved = true;
	}

	*(src->drag_pt) += t;

	if( src->m_bHandle )
	{
		//
		// this is a handle gripper.
		//
		if( src->reflect_pt )
		{
			vec3 d = *(src->drag_pt2) - *(src->drag_pt);
			*(src->reflect_pt) = *(src->drag_pt2) + d;
		}
	}
	else
	{
		if( src->drag_pt2 )
		{
			*(src->drag_pt2) = *(src->drag_pt);
		}
		if( src->t_pt )
			*(src->t_pt) += t;
		if( src->t_pt2 )
			*(src->t_pt2) += t;
	}

	if( src->segA )
		src->segA->rebuild = true;
	if( src->segB )
		src->segB->rebuild = true;

	pView->GetDocument()->BuildSelectionBounds();
	Sys_RedrawWindows();

	return true;
}

//////////////////////////////////////////////////////////////////////
// CSplineControlPoint_Manipulator									//
//////////////////////////////////////////////////////////////////////
CSplineControlPoint_Manipulator::CSplineControlPoint_Manipulator() : CManipulator()
{
	m_bHover = false;
	m_bMoved = false;
	m_bHandle = false;
	m_bDrag = false;

	size = 0;
	drag_pt = 0;
	drag_pt2 = 0;
	t_pt = 0;
	t_pt2 = 0;
	reflect_pt = 0;

	segA = segB = 0;

	color = hlcolor = 0;

	SetViewFlags(VIEW_FLAG_MAP);
}

CSplineControlPoint_Manipulator::~CSplineControlPoint_Manipulator()
{
}

void CSplineControlPoint_Manipulator::OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = true;
	pView->RedrawWindow();
	Sys_SetCursor( pView, TC_DEFAULT );
}

void CSplineControlPoint_Manipulator::OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = false;
	pView->RedrawWindow();
	Sys_SetCursor( pView, TC_DEFAULT );
}

void CSplineControlPoint_Manipulator::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	pView->GetDocument()->ClearSelectedManipulators();
	pView->GetDocument()->AddManipulatorToSelection( this );

	CSplineTrack::Clear3DManipulators( pView->GetDocument() );

	//
	// make 3d manipulators.
	//
	{
		float size = 64.0f;
		unsigned hlcolor = 0xFF4DE6F2;

		CSplineControlPoint_Manipulator3D** m = pView->GetDocument()->GetSpline3DManipulators();
		
		m[0] = new CSplineControlPoint_Manipulator3D();
		m[0]->color = 0xFF0000FF;
		m[0]->hlcolor = hlcolor;
		m[0]->size = size;
		m[0]->vec = sysAxisX;
		m[0]->x_in = drag_pt;
		m[0]->y_in = drag_pt;
		m[0]->z_in = drag_pt;
		m[0]->string = "drag x";
		m[0]->src = this;
		pView->GetDocument()->AddManipulatorToMap( m[0] );

		m[1] = new CSplineControlPoint_Manipulator3D();
		m[1]->color = 0xFF00FF00;
		m[1]->hlcolor = hlcolor;
		m[1]->size = size;
		m[1]->vec = sysAxisY;
		m[1]->x_in = drag_pt;
		m[1]->y_in = drag_pt;
		m[1]->z_in = drag_pt;
		m[1]->string = "drag y";
		m[1]->src = this;
		pView->GetDocument()->AddManipulatorToMap( m[1] );

		m[2] = new CSplineControlPoint_Manipulator3D();
		m[2]->color = 0xFFFF0000;
		m[2]->hlcolor = hlcolor;
		m[2]->size = size;
		m[2]->vec = sysAxisZ;
		m[2]->x_in = drag_pt;
		m[2]->y_in = drag_pt;
		m[2]->z_in = drag_pt;
		m[2]->string = "drag z";
		m[2]->src = this;
		pView->GetDocument()->AddManipulatorToMap( m[2] );
	}

	pView->SetCapture();

	Sys_RedrawWindows();

	if( pView->GetViewType() == VIEW_TYPE_3D )
		return;

	m_bDrag = true;
	m_bMoved = false;
}

void CSplineControlPoint_Manipulator::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = false;
	m_bDrag = false;
	ReleaseCapture();
	Sys_RedrawWindows();
}

void CSplineControlPoint_Manipulator::OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	float mapx, mapy;
	vec3 pos;
	int xaxis, yaxis;

	if( !m_bDrag )
		return;

	xaxis = QUICK_AXIS( pView->View.or2d.lft );
	yaxis = QUICK_AXIS( pView->View.or2d.up );

	pView->WinXYToMapXY( nMX, nMY, &mapx, &mapy );
	
	pos = *drag_pt;

	if( pView->GetGridSnap() )
	{
		pos[xaxis] = Sys_Snapf( mapx, pView->GetGridSize() );
		pos[yaxis] = Sys_Snapf( mapy, pView->GetGridSize() );
	}
	else
	{
		pos[xaxis] = mapx;
		pos[yaxis] = mapy;
	}

	if( equals( *drag_pt, pos, 0.00001f ) )
	{
		return;
	}

	if( !m_bMoved )
	{
		m_bMoved = true;
		pView->GetDocument()->GenericUndoRedoFromSelection()->SetTitle("SplineTrack Manipulation");
	}

	if( m_bHandle )
	{
		//
		// this is a handle gripper. it has a reflection point and whatnot.
		//
		*drag_pt = pos;

		if( reflect_pt )
		{
			vec3 d = *drag_pt2 - *drag_pt;
			*reflect_pt = *drag_pt2 + d;
		}
	}
	else
	{
		vec3 t = pos-*drag_pt;

		*drag_pt = pos;

		if( drag_pt2 )
			*drag_pt2 = pos;
		if( t_pt )
			*t_pt += t;
		if( t_pt2 )
			*t_pt2 += t;
	}

	if( segA )
		segA->rebuild = true;
	if( segB )
		segB->rebuild = true;

	pView->GetDocument()->BuildSelectionBounds();
	Sys_RedrawWindows();
}

void CSplineControlPoint_Manipulator::OnDraw( CMapView* pView )
{
	float s;
	vec3 mins, maxs;

	if( pView->GetViewType() != VIEW_TYPE_3D )
	{
		s = size*pView->View.fInvScale;
	}
	else
	{
		s = size;
	}

	if( m_bSelected )
	{
		glColor4f( 1, 0, 0, 1 );
	}
	else
	if( m_bHover )
	{
		glColor4ubv( (GLubyte*)&hlcolor );
	}
	else
	{
		glColor4ubv( (GLubyte*)&color );
	}

	mins = (*drag_pt) - (s*0.5f);
	maxs = (*drag_pt) + (s*0.5f);

	R_DrawBox( mins, maxs );

	if( m_bHandle && drag_pt2 )
	{
		//
		// draw a line.
		//
		glLoadName( 0 );

		glColor4f( 0, 0, 1, 1 );
		glBegin( GL_LINES );
		glVertex3fv( &drag_pt->x );
		glVertex3fv( &drag_pt2->x );
		glEnd();
		
		glLoadName( (GLint)this );
	}

	if( m_bHandle && reflect_pt )
	{
		//
		// draw a line.
		//
		glLoadName( 0 );

		glColor4f( 0, 0, 1, 1 );
		glBegin( GL_LINES );
		glVertex3fv( &drag_pt->x );
		glVertex3fv( &reflect_pt->x );
		glEnd();

		glLoadName( (GLint)this );
	}
}

//////////////////////////////////////////////////////////////////////
// CSplineParmsMenu													//
//////////////////////////////////////////////////////////////////////
CSplineTrack::CSplineParmsMenu::CSplineParmsMenu() : CObjectMenu()
{
	track = 0;
	seg = 0;
	view = 0;
}

CSplineTrack::CSplineParmsMenu::~CSplineParmsMenu()
{
}

void CSplineTrack::CSplineParmsMenu::OnMenuItem( int id )
{
	switch( id )
	{
	case 2:
	case 3:

		view->GetDocument()->ClearSelectedManipulators();
		CSplineTrack::Clear3DManipulators( view->GetDocument() );
		track->FreeManipulators( view->GetDocument() );
		
		if( id == 2 )
		{
			view->GetDocument()->GenericUndoRedoFromSelection()->SetTitle("Split SplineTrack Segment");
			track->SplitSegment( seg );
		}
		else
		{
			view->GetDocument()->GenericUndoRedoFromSelection()->SetTitle("Delete SplineTrack Segment");
			track->DeleteSegment( seg );
			view->GetDocument()->BuildSelectionBounds();
		}

		track->MakeManipulators( view->GetDocument() );
		track->CalcSplineTimes();

		view->GetDocument()->SetEditSplineTrack( track, seg );

		Sys_RedrawWindows();

	break;
	}
}

void CSplineTrack::CSplineParmsMenu::OnUpdateCmdUI( int id, CCmdUI* pUI )
{
	switch( id )
	{
	case 2:

		pUI->Enable( seg != 0 );

	break;

	case 3:

		pUI->Enable( track->m_segs.GetCount() > 1 );

	break;
	}
}

//////////////////////////////////////////////////////////////////////
// CSplineTrack														//
//////////////////////////////////////////////////////////////////////
bool CSplineTrack::m_bMenuCreated = false;
CSplineTrack::CSplineParmsMenu CSplineTrack::m_SplineMenu;

void CSplineTrack::MakeSplineMenu()
{
	if( m_bMenuCreated )
		return;

	m_SplineMenu.AddMenuItem( ID_TOOLS_ANIMATIONMODE, "Animation Mode", true );
	m_SplineMenu.AddMenuItem( 2, "Split Segment" );
	m_SplineMenu.AddMenuItem( 3, "Delete Segment" );
	m_SplineMenu.AddMenuItem( 0, "@SEP@" );
	m_SplineMenu.AddMenuItem( ID_TOOLS_HIDE, "Hide Selected", true );
	m_SplineMenu.AddMenuItem( ID_TOOLS_SHOWALL, "Show All", true );
	m_SplineMenu.AddMenuItem( ID_TOOLS_HIDEALL, "Hide All", true );
	m_SplineMenu.AddMenuItem( 0, "@SEP@" );
	m_SplineMenu.AddMenuItem( ID_TOOLS_MAKEGROUP, "Make Group", true );
	m_SplineMenu.AddMenuItem( ID_TOOLS_MAKEGROUPANDHIDE, "Make Group and Hide", true );
	m_SplineMenu.AddMenuItem( ID_TOOLS_HIDEALLINGROUP, "Hide All In Group(s)", true );
	m_SplineMenu.AddMenuItem( ID_TOOLS_SHOWALLINGROUPS, "Show All In Group(s)", true );
	m_SplineMenu.AddMenuItem( ID_TOOLS_SELECTALLINGROUPS, "Select All In Group(s)", true );
	m_SplineMenu.AddMenuItem( ID_TOOLS_REMOVEFROMGROUPS, "Remove From Group(s)", true );

	m_bMenuCreated = true;
}

bool CSplineTrack::OnPopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( !IsSelected() )
		return false;

	m_SplineMenu.track = this;
	m_SplineMenu.view = pView;
	m_SplineMenu.seg = dynamic_cast<CSplineSegment*>(pSrc);

	Sys_DisplayObjectMenu( pView, nMX, nMY, &CSplineTrack::m_SplineMenu );
	return true;
}

void CSplineTrack::DeleteSegment( CSplineSegment* seg )
{
	CSplineSegment* segA, *segB;

	segA = (CSplineSegment*)seg->prev;
	segB = (CSplineSegment*)seg->next;

	if( segA && segB )
	{
		segA->ctrls[CTRLB_POINT] = seg->ctrls[CTRLB_POINT];
		segA->ctrls[CTRLB_HANDLE] = seg->ctrls[CTRLB_HANDLE];

		segA->rebuild = true;
	}

	m_segs.DeleteItem( seg );
}

void CSplineTrack::SplitSegment( CSplineSegment* split_seg )
{
	int i;
	CSplineSegment* seg;
	vec3 pos, vec;
	CRenderMesh* m;

	BuildMeshes();

	for( i = 0, seg = m_segs.ResetPos(); seg; i++, seg = m_segs.GetNextItem() )
	{
		if( seg == split_seg )
			break;
	}

	if( !seg )
		return;

	m = &m_meshes[i];

	pos = m->xyz[SEGMENT_SUBDIVIDE_SIZE>>1];
	seg = new CSplineSegment();

	vec = (split_seg->ctrls[CTRLB_POINT]-pos)*0.25f;

	seg->start = split_seg->start;
	seg->ticks = split_seg->ticks>>1;
	seg->end = seg->start+seg->ticks;

	seg->ctrls[CTRLA_POINT] = pos;
	seg->ctrls[CTRLA_HANDLE] = pos + vec;
	seg->ctrls[CTRLB_POINT] = split_seg->ctrls[CTRLB_POINT];
	seg->ctrls[CTRLB_HANDLE] = split_seg->ctrls[CTRLB_HANDLE];
	seg->SetParent( this );
	seg->rebuild = true;

	split_seg->ctrls[CTRLB_POINT] = pos;
	split_seg->ctrls[CTRLB_HANDLE] = pos - vec;
	split_seg->rebuild = true;
	split_seg->start = seg->end;
	split_seg->ticks >>= 1;
	split_seg->end = split_seg->start+split_seg->ticks;

	m_segs.InsertItem( seg, split_seg, LL_INS_AFTER );
}

void CSplineTrack::Clear3DManipulators( CTreadDoc* doc )
{
	CSplineControlPoint_Manipulator3D** m = doc->GetSpline3DManipulators();

	if( m[0] )
	{
		doc->DetachManipulator( m[0] );
		delete m[0];
	}

	if( m[1] )
	{
		doc->DetachManipulator( m[1] );
		delete m[1];
	}

	if( m[2] )
	{
		doc->DetachManipulator( m[2] );
		delete m[2];
	}

	m[0] = m[1] = m[2] = 0;

}

CSplineTrack::CSplineTrack() : CMapObject()
{
	m_meshes = 0;
	m_mesh_count = 0;
	m_motion = true;
	m_pos = vec3::zero;
	m_manips = 0;
	m_num_manips = 0;
	m_bDrag = false;
	
	m_NameProp = new CObjProp();
	m_NameProp->SetType( CObjProp::string );
	m_NameProp->SetString( "" );
	m_NameProp->SetName("targetname");
	m_Props.AddItem( m_NameProp );

	MakeSplineMenu();
}

CSplineTrack::CSplineTrack( const CSplineTrack& t ) : CMapObject( t )
{
	m_meshes = 0;
	m_mesh_count = 0;
	m_motion = t.m_motion;
	m_pos = t.m_pos;
	m_manips = 0;
	m_num_manips = 0;

	CopySegs( (CLinkedList<CSplineSegment>*)&t.m_segs );
	CopyKeyFrames( (CLinkedList<CSplineKeyFrame>*)&t.m_rots, &m_rots );
	CopyKeyFrames( (CLinkedList<CSplineKeyFrame>*)&t.m_fovs, &m_fovs );
	CopyKeyFrames( (CLinkedList<CSplineKeyFrame>*)&t.m_events, &m_events );

	m_NameProp = new CObjProp();
	m_NameProp->SetType( CObjProp::string );
	m_NameProp->SetString( t.m_NameProp->GetString() );
	m_NameProp->SetName("targetname");
	m_NameProp->SetDisplayName("targetname");
	m_Props.AddItem( m_NameProp );

	MakeSplineMenu();
}

CSplineTrack::~CSplineTrack()
{
	FreeMeshes();

	if( m_manips )
		delete[] m_manips;
}

CLinkedList<CObjProp>* CSplineTrack::GetPropList( CTreadDoc* pDoc )
{
	return &m_Props;
}

void CSplineTrack::SetProp(  CTreadDoc* pDoc, CObjProp* prop )
{
	if( !stricmp(prop->GetName(), "targetname") )
	{
		m_NameProp->SetValue( prop );
	}
}


void CSplineTrack::SelectSegment( CSplineSegment* seg )
{
	m_selected_seg = seg;
}

void CSplineTrack::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bDrag = false;

	if( !IsSelected() )
	{
		if( !(nButtons&MS_CONTROL) )
		{
			pView->GetDocument()->MakeUndoDeselectAction();
			pView->GetDocument()->ClearSelection();
		}

		Select( pView->GetDocument(), pSrc );
		pView->GetDocument()->Prop_UpdateSelection();
		if( pView->GetViewType() != VIEW_TYPE_3D )
		{
			m_bDrag = true;
			Sys_BeginDragSel( pView, nMX, nMY, nButtons );
		}
		pView->GetDocument()->UpdateSelectionInterface();
	}
	else
	{
		if( nButtons&MS_CONTROL )
		{
			pView->GetDocument()->MakeUndoDeselectAction();
			Deselect( pView->GetDocument() );
			pView->GetDocument()->Prop_UpdateSelection();
			pView->GetDocument()->UpdateSelectionInterface();
		}
		else
		{
			if( pView->GetViewType() != VIEW_TYPE_3D )
			{
				m_bDrag = true;
				Sys_BeginDragSel( pView, nMX, nMY, nButtons );
			}
			else
			{
				pView->GetDocument()->MakeUndoDeselectAction();
				pView->GetDocument()->ClearSelection();

				Select( pView->GetDocument() );
				pView->GetDocument()->Prop_UpdateSelection();
				pView->GetDocument()->UpdateSelectionInterface();
			}
		}
	}

	CSplineSegment* seg = dynamic_cast<CSplineSegment*>(pSrc);

	if( seg )
	{
		SelectSegment( seg );

		if( IsSelected() )
		{
			pView->GetDocument()->SetEditSplineTrack( this, m_selected_seg );
		}
	}

	Sys_RedrawWindows();
}

void CSplineTrack::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( m_bDrag )
	{
		m_bDrag = false;
		Sys_EndDragSel( pView, nMX, nMY, nButtons );
	}
}

void CSplineTrack::OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( m_bDrag )
	{
		Sys_DragSel( pView, nMX, nMY, nButtons );
	}
}

void CSplineTrack::OnAddToMap( CTreadDoc* pDoc )
{
	if( IsSelected() )
	{
		FreeManipulators( pDoc );
		{
			CSplineTrack* t;
			
			pDoc->GetEditSplineTrack( &t, 0 );
			if( t == this )
				pDoc->SetEditSplineTrack( 0, 0 );
		}
	}

	SelectSegment( 0 );

	CMapObject::OnAddToMap( pDoc );
}

void CSplineTrack::OnRemoveFromMap( CTreadDoc* pDoc )
{
	{
		CSplineTrack* t;
		
		pDoc->GetEditSplineTrack( &t, 0 );
		if( t == this )
			pDoc->SetEditSplineTrack( 0, 0 );
	}

	FreeManipulators( pDoc );
	CMapObject::OnRemoveFromMap( pDoc );
}

void CSplineTrack::OnAddToSelection( CTreadDoc* pDoc )
{
	SelectSegment( m_segs.GetItem( LL_HEAD ) );

	pDoc->SetEditSplineTrack( this, m_selected_seg );

	MakeManipulators( pDoc );
	CMapObject::OnAddToSelection( pDoc );
}

void CSplineTrack::FreeManipulators( CTreadDoc* doc )
{
	//doc->ClearAllTrackPicks();

	if( m_manips )
	{
		int i;
		for(i = 0; i < m_num_manips; i++)
		{
			if( m_manips[i]->m_bSelected )
			{
				Clear3DManipulators( doc );
			}

			doc->DetachManipulator( m_manips[i] );
			delete m_manips[i];
		}

		delete[] m_manips;
	}

	m_manips = 0;
	m_num_manips = 0;
}

void CSplineTrack::MakeManipulators( CTreadDoc* doc )
{
	CSplineSegment* segA, *segB;
	CSplineControlPoint_Manipulator* m;
	float size = 7.0f;
	int ofs = 0;
	int count = 4 + (3*(m_segs.GetCount()-1));

	FreeManipulators( doc );

	m_manips = new CSplineControlPoint_Manipulator*[count];
	m_num_manips = count;

	segA = 0;

	for( segB = m_segs.ResetPos(); segB; segB = m_segs.GetNextItem() )
	{
		if( !segA )
		{
			m = new CSplineControlPoint_Manipulator();
			
			m->color = 0xFFFF00FF;
			m->hlcolor = 0xFF00FFFF;
			m->drag_pt = &segB->ctrls[CTRLA_POINT];
			m->t_pt = &segB->ctrls[CTRLA_HANDLE];
			m->m_bHandle = false;
			m->segA = segB;
			m->segB = 0;
			m->size = size;
			doc->AddManipulatorToMap( m );
			m_manips[ofs++] = m;

			m = new CSplineControlPoint_Manipulator();
			
			m->color = 0xFFFF00FF;
			m->hlcolor = 0xFF00FFFF;
			m->drag_pt = &segB->ctrls[CTRLA_HANDLE];
			m->drag_pt2 = &segB->ctrls[CTRLA_POINT];
			m->m_bHandle = true;
			m->segA = segB;
			m->segB = 0;
			m->size = size;
			doc->AddManipulatorToMap( m );
			m_manips[ofs++] = m;
		}
		else
		{
			m = new CSplineControlPoint_Manipulator();
			
			m->color = 0xFFFF00FF;
			m->hlcolor = 0xFF00FFFF;
			m->drag_pt = &segA->ctrls[CTRLB_POINT];
			m->drag_pt2 = &segB->ctrls[CTRLA_POINT];
			m->t_pt =  &segA->ctrls[CTRLB_HANDLE];
			m->t_pt2 = &segB->ctrls[CTRLA_HANDLE];
			m->m_bHandle = false;
			m->segA = segA;
			m->segB = segB;
			m->size = size;
			doc->AddManipulatorToMap( m );
			m_manips[ofs++] = m;

			m = new CSplineControlPoint_Manipulator();
			
			m->color = 0xFFFF00FF;
			m->hlcolor = 0xFF00FFFF;
			m->drag_pt = &segA->ctrls[CTRLB_HANDLE];
			m->drag_pt2 = &segA->ctrls[CTRLB_POINT];
			m->reflect_pt = &segB->ctrls[CTRLA_HANDLE];
			m->m_bHandle = true;
			m->segA = segA;
			m->segB = segB;
			m->size = size;
			doc->AddManipulatorToMap( m );
			m_manips[ofs++] = m;

			m = new CSplineControlPoint_Manipulator();
			
			m->color = 0xFFFF00FF;
			m->hlcolor = 0xFF00FFFF;
			m->drag_pt = &segB->ctrls[CTRLA_HANDLE];
			m->drag_pt2 = &segB->ctrls[CTRLA_POINT];
			m->reflect_pt = &segA->ctrls[CTRLB_HANDLE];
			m->m_bHandle = true;
			m->segA = segA;
			m->segB = segB;
			m->size = size;
			doc->AddManipulatorToMap( m );
			m_manips[ofs++] = m;
		}

		segA = segB;
	}

	if( segA )
	{
		m = new CSplineControlPoint_Manipulator();
			
		m->color = 0xFFFF00FF;
		m->hlcolor = 0xFF00FFFF;
		m->drag_pt = &segA->ctrls[CTRLB_POINT];
		m->t_pt =  &segA->ctrls[CTRLB_HANDLE];
		m->m_bHandle = false;
		m->segA = segA;
		m->segB = 0;
		m->size = size;
		doc->AddManipulatorToMap( m );
		m_manips[ofs++] = m;

		m = new CSplineControlPoint_Manipulator();
		
		m->color = 0xFFFF00FF;
		m->hlcolor = 0xFF00FFFF;
		m->drag_pt = &segA->ctrls[CTRLB_HANDLE];
		m->drag_pt2 = &segA->ctrls[CTRLB_POINT];
		m->m_bHandle = true;
		m->segA = segA;
		m->segB = 0;
		m->size = size;
		doc->AddManipulatorToMap( m );
		m_manips[ofs++] = m;
	}
}

CSplineKeyFrame* CSplineTrack::FindKeyFrame( CLinkedList<CSplineKeyFrame>* list, int ticks )
{
	CSplineKeyFrame* key;

	for( key = list->ResetPos(); key; key = list->GetNextItem() )
	{
		if( key->ticks == ticks )
			return key;
	}

	return 0;
}

void CSplineTrack::GetTickBounds( CLinkedList<CSplineKeyFrame>* list, int ticks, CSplineKeyFrame** front, CSplineKeyFrame** back )
{
	CSplineKeyFrame* key;

	if( front )
	{
		*front = 0;
		//
		// find front.
		//
		for( key = list->ResetPos(); key; key = list->GetNextItem() )
		{
			if( key->ticks <= ticks )
			{
				*front = key;
			}
		}
	}

	if( back )
	{
		*back = 0;
		//
		// find back.
		//
		for( key = list->ResetPos(); key; key = list->GetNextItem() )
		{
			if( key->ticks > ticks )
			{
				*back = key;
				break;
			}
		}
	}
}

void CSplineTrack::CopySegs( CLinkedList<CSplineSegment>* list )
{
	CSplineSegment* seg, *copy;

	m_segs.DestroyList();
	for( seg = list->ResetPos(); seg; seg = list->GetNextItem() )
	{
		copy = new CSplineSegment( *seg );
		m_segs.AddItem( copy );
		copy->SetParent( this );
	}
}

void CSplineTrack::CopyKeyFrames( CLinkedList<CSplineKeyFrame>* src, CLinkedList<CSplineKeyFrame>* dst )
{
	CSplineKeyFrame* key, *copy;

	dst->DestroyList();
	for( key = src->ResetPos(); key; key = src->GetNextItem() )
	{
		copy = new CSplineKeyFrame( *key );
		dst->AddItem( copy );
	}
}

void CSplineTrack::FreeMeshes()
{
	int i;
	for(i = 0; i < m_mesh_count; i++)
		m_meshes[i].FreeMesh();

	delete[] m_meshes;
	m_meshes = 0;
	m_mesh_count = 0;
}

void CSplineTrack::BuildMeshes()
{
	int i, num;
	CSplineSegment* seg;
	bool build_all = false;

	num = m_segs.GetCount();
	if( num != m_mesh_count )
	{
		FreeMeshes();
		m_meshes = new CRenderMesh[num];
		m_mesh_count = num;

		build_all = true;
	}

	for( i = 0, seg = m_segs.ResetPos(); seg; i++, seg = m_segs.GetNextItem() )
	{
		if( seg->rebuild || build_all )
			seg->MakeRenderMesh( &m_meshes[i] );

		m_meshes[i].allow_selected = true;
		m_meshes[i].color2d = m_meshes[i].color3d = 0xFF00FF00;
		m_meshes[i].fcolor3d[0] = 0.0f;
		m_meshes[i].fcolor3d[1] = 1.0f;
		m_meshes[i].fcolor3d[2] = 0.0f;
		m_meshes[i].fcolor3d[3] = 1.0f;
		m_meshes[i].allow_wireframe = false;
		m_meshes[i].line_size = 3;

		if( seg == m_selected_seg && IsSelected() )
		{
			m_meshes[i].allow_selected = false;
			m_meshes[i].color2d = m_meshes[i].color3d = 0xFF00FFFF;
			m_meshes[i].fcolor3d[0] = 1.0f;
			m_meshes[i].fcolor3d[1] = 1.0f;
			m_meshes[i].fcolor3d[2] = 0.0f;
			m_meshes[i].fcolor3d[3] = 1.0f;

		}
		
		seg->rebuild = false;
	}
}

CLinkedList<CSplineKeyFrame>* CSplineTrack::GetKeyFrames( int type )
{
	switch( type )
	{
	case KEYFRAME_ROTS:
		return &m_rots;
	case KEYFRAME_FOVS:
		return &m_fovs;
	case KEYFRAME_EVENTS:
		return &m_events;
	}

	return 0;
}

int CSplineTrack::GetTotalTicks()
{
	CSplineSegment* seg;
	int ticks = 0;

	for( seg = m_segs.ResetPos(); seg; seg = m_segs.GetNextItem() )
	{
		ticks += seg->ticks;
	}

	return ticks;
}

void CSplineTrack::CalcSplineTimes()
{
	CSplineSegment* seg;
	int ticks;

	ticks = 0;
	for( seg = m_segs.ResetPos(); seg; seg = m_segs.GetNextItem() )
	{
		seg->start = ticks;
		seg->end = ticks+seg->ticks;
		
		ticks += seg->ticks;
	}
}

void CSplineTrack::ScaleTimes( float frac )
{
	CSplineSegment* seg;

	for( seg = m_segs.ResetPos(); seg; seg = m_segs.GetNextItem() )
	{
		seg->start = ((float)seg->start)*frac;
		seg->ticks = ((float)seg->ticks)*frac;
		seg->end = seg->start+seg->ticks;
	}

	//
	// do keyframes.
	//
	CSplineKeyFrame* key;

	for( key = m_rots.ResetPos(); key; key = m_rots.GetNextItem() )
	{
		key->ticks = ((float)key->ticks)*frac;
	}

	for( key = m_fovs.ResetPos(); key; key = m_fovs.GetNextItem() )
	{
		key->ticks = ((float)key->ticks)*frac;
	}

	for( key = m_events.ResetPos(); key; key = m_events.GetNextItem() )
	{
		key->ticks = ((float)key->ticks)*frac;
	}
}

void CSplineTrack::AddKeyFrame( int ticks, vec3 v, const char* event, int type )
{
	CSplineKeyFrame* key, *front;
	CLinkedList<CSplineKeyFrame>* list;

	key = new CSplineKeyFrame();
	key->ticks = ticks;
	key->v = v;
	
	if( event )
		key->s = event;


	list = GetKeyFrames( type );
	GetTickBounds( list, ticks, &front, 0 );

	if( front )
	{
		list->InsertItem( key, front );
	}
	else
	{
		list->AddItem( key, LL_HEAD );
	}
}

bool CSplineTrack::KeyFrameExists( int ticks, int type )
{
	CSplineKeyFrame* key;
	CLinkedList<CSplineKeyFrame>* list;

	list = GetKeyFrames( type );
	if( !list )
		return false;

	for( key = list->ResetPos(); key; key = list->GetNextItem() )
	{
		if( key->ticks == ticks )
			return true;
	}

	return false;
}

void CSplineTrack::RemoveKeyFrame( int ticks, int type )
{
	CSplineKeyFrame* key;
	CLinkedList<CSplineKeyFrame>* list;

	list = GetKeyFrames( type );
	key = FindKeyFrame( list, ticks );
	if( key )
	{
		list->DeleteItem( key );
	}
}

void CSplineTrack::SetKeyFrameInfo( int ticks, int new_ticks, vec3 v, const char* event, int type, int flags )
{
	CSplineKeyFrame* key;
	CLinkedList<CSplineKeyFrame>* list;

	list = GetKeyFrames( type );
	key = FindKeyFrame( list, ticks );
	if( key )
	{
		if( flags&KEYFRAME_MASK_VEC )
			key->v = v;
		if( (flags&KEYFRAME_MASK_EVENT) && event )
		{
			key->s = event;
		}
		if( flags&KEYFRAME_MASK_TICKS )
		{
			list->RemoveItem( key );
			AddKeyFrame( new_ticks, key->v, key->s, type );
			delete key;
		}
	}
}

void CSplineTrack::EvaluateCurve1D( float* ctrls, float* outpt, float u )
{
	vec_t inv_u;
	vec_t sqr_u;
	vec_t cub_u;
	vec_t sqr_inv_u;
	vec_t cub_inv_u;
	vec_t _3u;
	vec_t _3u_sqr;

	sqr_u = u*u;
	cub_u = sqr_u*u;
	
	inv_u = 1-u;
	sqr_inv_u = inv_u*inv_u;
	cub_inv_u = sqr_inv_u*inv_u;
	
	_3u = 3*u*sqr_inv_u;
	_3u_sqr = 3*sqr_u*inv_u;

	outpt[0] = ctrls[0]*cub_inv_u +
			   ctrls[1]*_3u +
			   ctrls[2]*_3u_sqr +
			   ctrls[3]*cub_u;
}

void CSplineTrack::EvaluateCurve3D( vec3* ctrls, vec3* outpt, float u )
{
	vec_t inv_u;
	vec_t sqr_u;
	vec_t cub_u;
	vec_t sqr_inv_u;
	vec_t cub_inv_u;
	vec_t _3u;
	vec_t _3u_sqr;

	sqr_u = u*u;
	cub_u = sqr_u*u;
	
	inv_u = 1-u;
	sqr_inv_u = inv_u*inv_u;
	cub_inv_u = sqr_inv_u*inv_u;
	
	_3u = 3*u*sqr_inv_u;
	_3u_sqr = 3*sqr_u*inv_u;

	outpt[0][0] = ctrls[0][0]*cub_inv_u +
			   ctrls[1][0]*_3u +
			   ctrls[2][0]*_3u_sqr +
			   ctrls[3][0]*cub_u;
	
	outpt[0][1] = ctrls[0][1]*cub_inv_u +
			   ctrls[1][1]*_3u +
			   ctrls[2][1]*_3u_sqr +
			   ctrls[3][1]*cub_u;

	outpt[0][2] = ctrls[0][2]*cub_inv_u +
			   ctrls[1][2]*_3u +
			   ctrls[2][2]*_3u_sqr +
			   ctrls[3][2]*cub_u;
}

void CSplineTrack::GetFovBorders( CLinkedList<CSplineKeyFrame>* fovs, int ticks, float* start, float* end, float* t )
{
	CSplineKeyFrame* sk, *ek, *k;
	float st, et;
	float total;

	sk = ek = 0;
	for( k = fovs->ResetPos(); k; k = fovs->GetNextItem() )
	{
		if( k->ticks <= ticks )
			sk = k;
		else
		if( k->ticks > ticks )
			ek = k;

		if( ek )
			break;
	}

	if( !sk )
	{
		*start = 90.0f;
		st = 0.0f;
	}
	else
	{
		*start = sk->v.x;
		st = sk->ticks;
	}

	if( !ek )
	{
		*end = *start;
		et = st;
	}
	else
	{
		*end = ek->v.x;
		et = ek->ticks;
	}

	total = et-st;
	if( total < 1.0f )
	{
		*t = 0.0f;
		return;
	}

	*t = (ticks-st) / total;
}

void CSplineTrack::GetRotationBorders( CLinkedList<CSplineKeyFrame>* rots, int ticks, vec3* start, vec3* end, float* t )
{
	CSplineKeyFrame* sk, *ek, *k;
	float st, et;
	float total;

	sk = ek = 0;
	for( k = rots->ResetPos(); k; k = rots->GetNextItem() )
	{
		if( k->ticks <= ticks )
			sk = k;
		else
		if( k->ticks > ticks )
			ek = k;

		if( ek )
			break;
	}

	if( !sk )
	{
		st = 0.0f;
		*start = vec3::zero;
	}
	else
	{
		st = sk->ticks;
		*start = sk->v;
	}

	if( !ek )
	{
		*end = *start;
		et = st;
	}
	else
	{
		*end = ek->v;
		et = ek->ticks;
	}

	total = et-st;
	if( total < 1.0f )
	{
		*t = 0.0f;
		return;
	}

	*t = (ticks-st) / total;
}


CSplineSegment* CSplineTrack::SegmentForTicks( CLinkedList<CSplineSegment>* segs, int ticks )
{
	CSplineSegment* seg;

	for( seg = segs->ResetPos(); seg; seg = segs->GetNextItem() )
	{
		if( ticks >= seg->start && ticks < seg->end )
			return seg;
	}

	return 0;
}

float CSplineTrack::TemperalForTicks( CSplineSegment* seg, float ticks )
{
	return (ticks-((float)seg->start)) / ((float)seg->ticks);
}

void CSplineTrack::EvaluateSpline( int ticks, vec3* pos, vec3* rot, float* fov )
{
	CSplineSegment* seg;
	
	//
	// evaluate position.
	//
	if( m_motion )
	{
		seg = SegmentForTicks( &m_segs, ticks );
		if( seg )
		{
			EvaluateCurve3D( seg->ctrls, pos, TemperalForTicks( seg, ticks ) );
		}
		else
		{
			//
			// use last.
			//
			seg = m_segs.GetItem( LL_TAIL );
			if( seg )
			{
				*pos = seg->ctrls[CTRLB_POINT];
			}
		}
	}
	else
	{
		seg = m_segs.GetItem( LL_HEAD );
		if( seg )
		{
			*pos = seg->ctrls[CTRLA_POINT];
		}
	}

	//
	// evaluate rots.
	//
	{
		vec3 ctrls[4];
		float t;

		GetRotationBorders( &m_rots, ticks, &ctrls[0], &ctrls[3], &t );
		
		ctrls[1] = ctrls[0];
		ctrls[2] = ctrls[3];
		EvaluateCurve3D( ctrls, rot, t );
	}

	//
	// evaluate fov.
	//
	{
		float ctrls[4];
		float t;

		GetFovBorders( &m_fovs, ticks, &ctrls[0], &ctrls[3], &t );
		ctrls[1] = ctrls[0];
		ctrls[2] = ctrls[3];
		EvaluateCurve1D( ctrls, fov, t );
	}
}

bool CSplineTrack::HasMotion()
{
	return m_motion;
}

void CSplineTrack::SetMotion( bool motion )
{
	m_motion = motion;
}

const char* CSplineTrack::GetRootName()
{
	return "SplineTrack";
}

int CSplineTrack::GetClass()
{
	return 0;
}

void CSplineTrack::GetWorldMinsMaxs( vec3* pMins, vec3* pMaxs )
{
	int i;
	CSplineSegment* seg;
	vec3 mins, maxs;

	mins = vec3::bogus_max;
	maxs = vec3::bogus_min;

	for( seg = m_segs.ResetPos(); seg; seg = m_segs.GetNextItem() )
	{
		for(i = 0; i < 4; i++)
		{
			mins = vec_mins( mins, seg->ctrls[i] );
			maxs = vec_maxs( maxs, seg->ctrls[i] );
		}
	}

	*pMins = mins;
	*pMaxs = maxs;
}

void CSplineTrack::GetObjectMinsMaxs( vec3* pMins, vec3* pMaxs )
{
	vec3 mins, maxs;

	GetWorldMinsMaxs( &mins, &maxs );

	*pMins = mins-m_pos;
	*pMaxs = maxs-m_pos;
}

vec3 CSplineTrack::GetObjectWorldPos()
{
	return m_pos;
}

int CSplineTrack::GetNumRenderMeshes( CMapView* pView )
{
	BuildMeshes();
	return m_mesh_count;
}

CRenderMesh* CSplineTrack::GetRenderMesh( int num, CMapView* pView )
{
	return &m_meshes[num];
}

CMapObject* CSplineTrack::Clone()
{
	CSplineTrack* track = new CSplineTrack( *this );
	return track;
}

void CSplineTrack::CopyState( CMapObject* src, CTreadDoc* pDoc )
{
	CMapObject::CopyState( src, pDoc );

	CSplineTrack* s = dynamic_cast<CSplineTrack*>(src);
	if( !s )
		return;

	m_NameProp->SetValue( s->m_NameProp );
}

void CSplineTrack::SetObjectWorldPos( const vec3& pos, CTreadDoc* pDoc )
{
	vec3 t = pos - m_pos;
	int i;
	CSplineSegment* seg;

	for( seg = m_segs.ResetPos(); seg; seg = m_segs.GetNextItem() )
	{
		for(i = 0; i < 4; i++)
			seg->ctrls[i] += t;

		seg->rebuild = true;
	}

	m_pos = pos;
}

CMapObject* CSplineTrack::MakeSplineTrack( CTreadDoc* doc, void* parm )
{
	CSplineSegment* seg;
	CSplineTrack* track;

	track = new CSplineTrack();
	seg = new CSplineSegment();

	seg->start = 0;
	seg->end = 1000;
	seg->ticks = 1000; // one second.

	seg->ctrls[CTRLA_POINT] = vec3( -128.0f, 0, 0 );
	seg->ctrls[CTRLB_POINT] = vec3(  128.0f, 0, 0 );
	seg->ctrls[CTRLA_HANDLE] = vec3::zero;
	seg->ctrls[CTRLB_HANDLE] = vec3::zero;

	track->m_segs.AddItem( seg );
	seg->SetParent( track );

	return track;
}

bool CSplineTrack::WriteKeyframeList( CLinkedList<CSplineKeyFrame>* list, CFile* file, CTreadDoc* doc, int nVersion )
{
	int c;

	c = list->GetCount();
	MAP_WriteInt( file, c );

	if( c > 0 )
	{
		CSplineKeyFrame* k;

		for( k = list->ResetPos(); k; k = list->GetNextItem() )
		{
			MAP_WriteInt( file, k->ticks );
			MAP_WriteString( file, k->s );
			MAP_WriteVec3( file, &k->v );
		}
	}

	return true;
}

bool CSplineTrack::ReadKeyframeList( CLinkedList<CSplineKeyFrame>* list, CFile* file, CTreadDoc* doc, int nVersion )
{
	int i, c;
	CSplineKeyFrame* k;

	list->DestroyList();

	c = MAP_ReadInt( file );
	if( c > 0 )
	{
		for(i = 0; i < c; i++)
		{
			k = new CSplineKeyFrame();

			k->ticks = MAP_ReadInt( file );
			k->s = MAP_ReadString( file );
			MAP_ReadVec3( file, &k->v );

			list->AddItem( k );
		}
	}

	return true;
}

void CSplineTrack::WriteToMapFile( std::fstream& file, CTreadDoc* doc )
{
	int i;
	CSplineKeyFrame* k;
	CSplineSegment* s;
	CString str;

	file << "{\n\"classname\" \"info_spline_track\"\n";

	for( i = 0, s = m_segs.ResetPos(); s; i++, s = m_segs.GetNextItem() )
	{
		file << "\"segment " << i << "\" \"";
		file << "( " << s->ctrls[0].x << " " << s->ctrls[0].y << " " << s->ctrls[0].z << " ) ";
		file << "( " << s->ctrls[1].x << " " << s->ctrls[1].y << " " << s->ctrls[1].z << " ) ";
		file << "( " << s->ctrls[2].x << " " << s->ctrls[2].y << " " << s->ctrls[2].z << " ) ";
		file << "( " << s->ctrls[3].x << " " << s->ctrls[3].y << " " << s->ctrls[3].z << " ) ";
		file << s->ticks << "\"\n";
	}

	for( i = 0, k = m_rots.ResetPos(); k; i++, k = m_rots.GetNextItem() )
	{
		file << "\"rot_frame " << i << "\" \"";
		file << "( " << k->v.x << " " << k->v.y << " " << k->v.z << " ) " << k->ticks << "\"\n";
	}

	for( i = 0, k = m_fovs.ResetPos(); k; i++, k = m_fovs.GetNextItem() )
	{
		file << "\"fov_frame " << i << "\" \"";
		file << k->v.x << " " << k->ticks << "\"\n";
	}

	for( i = 0, k = m_events.ResetPos(); k; i++, k = m_events.GetNextItem() )
	{
		file << "\"event_keyframe_info " << i << "\" \"" << k->ticks << "\"\n";
		
		str = k->s;
		str.Replace( '"', '\'' );
		file << "\"event_keyframe_data " << i << "\" \"" << str << "\"\n";
	}

	if( strlen( m_NameProp->GetString() ) > 0 )
	{
		file << "\"targetname\" \"" << m_NameProp->GetString() << "\"\n";
	}

	if( m_motion )
	{
		file << "\"nomotion\" \"0\"";
	}
	else
	{
		file << "\"nomotion\" \"1\"";
	}

	file << "}\n";
}

bool CSplineTrack::WriteToFile( CFile* file, CTreadDoc* pDoc, int nVersion )
{
	CMapObject::WriteToFile( file, pDoc, nVersion );

	MAP_WriteString( file, m_NameProp->GetString() );
	MAP_WriteInt( file, m_motion );

	//
	// write segment data.
	//
	int c;

	c = m_segs.GetCount();
	MAP_WriteInt( file, c );
	if( c > 0 )
	{
		CSplineSegment* s;

		for( s = m_segs.ResetPos(); s; s = m_segs.GetNextItem() )
		{
			MAP_WriteInt( file, s->start );
			MAP_WriteInt( file, s->end );
			MAP_WriteInt( file, s->ticks );
			MAP_WriteInt( file, s->length );

			MAP_WriteVec3( file, &s->ctrls[0] );
			MAP_WriteVec3( file, &s->ctrls[1] );
			MAP_WriteVec3( file, &s->ctrls[2] );
			MAP_WriteVec3( file, &s->ctrls[3] );
		}
	}

	if( WriteKeyframeList( &m_rots, file, pDoc, nVersion ) == false )
		return false;
	if( WriteKeyframeList( &m_fovs, file, pDoc, nVersion ) == false )
		return false;
	if( WriteKeyframeList( &m_events, file, pDoc, nVersion ) == false )
		return false;

	return true;
}

bool CSplineTrack::ReadFromFile( CFile* file, CTreadDoc* pDoc, int nVersion )
{
	CMapObject::ReadFromFile( file, pDoc, nVersion );

	m_NameProp->SetString( MAP_ReadString( file ) );
	m_motion = MAP_ReadInt( file );

	//
	// read curve
	//
	{
		int i, c;
		CSplineSegment* s;

		m_segs.DestroyList();

		c = MAP_ReadInt( file );
		if( c > 0 )
		{
			for(i = 0; i < c; i++)
			{
				s = new CSplineSegment();

				s->start = MAP_ReadInt( file );
				s->end   = MAP_ReadInt( file );
				s->ticks = MAP_ReadInt( file );
				s->length = MAP_ReadInt( file );

				MAP_ReadVec3( file, &s->ctrls[0] );
				MAP_ReadVec3( file, &s->ctrls[1] );
				MAP_ReadVec3( file, &s->ctrls[2] );
				MAP_ReadVec3( file, &s->ctrls[3] );

				m_segs.AddItem( s );
				s->SetParent( this );
			}
		}
	}

	if( ReadKeyframeList( &m_rots, file, pDoc, nVersion ) == false )
		return false;
	if( ReadKeyframeList( &m_fovs, file, pDoc, nVersion ) == false )
		return false;
	if( ReadKeyframeList( &m_events, file, pDoc, nVersion ) == false )
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////
// CSplineKeyFrame													//
//////////////////////////////////////////////////////////////////////
CSplineKeyFrame::CSplineKeyFrame() : CLLObject()
{
	v = vec3::zero;
	s = "";
	ticks = 0.0f;
}

CSplineKeyFrame::CSplineKeyFrame( const CSplineKeyFrame& f ) : CLLObject()
{
	v = f.v;
	s = f.s;
	ticks = f.ticks;
}

CSplineKeyFrame::~CSplineKeyFrame()
{
}

//////////////////////////////////////////////////////////////////////
// CSplineSegment													//
//////////////////////////////////////////////////////////////////////

CSplineSegment::CSplineSegment() : CPickObject(), CLLObject()
{
	ctrls[0] = vec3::zero;
	ctrls[1] = vec3::zero;
	ctrls[2] = vec3::zero;
	ctrls[3] = vec3::zero;

	start = end = ticks = 0.0f;
	rebuild = false;
}

CSplineSegment::CSplineSegment( const CSplineSegment& s ) : CPickObject(), CLLObject()
{
	ctrls[0] = s.ctrls[0];
	ctrls[1] = s.ctrls[1];
	ctrls[2] = s.ctrls[2];
	ctrls[3] = s.ctrls[3];

	start = s.start;
	end = s.end;
	ticks = s.ticks;
	length = 0.0f;
	rebuild = true;
}

CSplineSegment::~CSplineSegment()
{
}

void CSplineSegment::MakeRenderMesh( CRenderMesh* m )
{
	int i;

	m->FreeMesh();
	m->cmds = GL_LINES;
	m->xyz = new vec3[SEGMENT_SUBDIVIDE_SIZE];
	m->num_pts = SEGMENT_SUBDIVIDE_SIZE;
	m->num_tris = SEGMENT_SUBDIVIDE_SIZE;
	m->tris = new unsigned short[SEGMENT_SUBDIVIDE_SIZE];
	m->pick = this;
	m->solid2d = false;

	for(i = 0; i < SEGMENT_SUBDIVIDE_SIZE; i++)
		m->tris[i] = i;

	//
	// the # of midway pts =='s numpts-1.
	//
	float nummids = 1.0f/(float)(SEGMENT_SUBDIVIDE_SIZE-1);

	float u;
	float inv_u;
	float sqr_u;
	float cub_u;
	float sqr_inv_u;
	float cub_inv_u;
	float _3u;
	float _3u_sqr;

	u = 0;
	for(i = 0; i < SEGMENT_SUBDIVIDE_SIZE; i++, u+=nummids)
	{
		sqr_u = u*u;
		cub_u = sqr_u*u;
		
		inv_u = 1-u;
		sqr_inv_u = inv_u*inv_u;
		cub_inv_u = sqr_inv_u*inv_u;
		
		_3u = 3*u*sqr_inv_u;
		_3u_sqr = 3*sqr_u*inv_u;

		m->xyz[i][0] = ctrls[0][0]*cub_inv_u +
				   ctrls[1][0]*_3u +
				   ctrls[2][0]*_3u_sqr +
				   ctrls[3][0]*cub_u;
		
		m->xyz[i][1] = ctrls[0][1]*cub_inv_u +
				   ctrls[1][1]*_3u +
				   ctrls[2][1]*_3u_sqr +
				   ctrls[3][1]*cub_u;

		m->xyz[i][2] = ctrls[0][2]*cub_inv_u +
				   ctrls[1][2]*_3u +
				   ctrls[2][2]*_3u_sqr +
				   ctrls[3][2]*cub_u;
	}
	
	//
	// find the length of the curve.
	//
	if( SEGMENT_SUBDIVIDE_SIZE > 0 )
	{
		float l;
		vec3 a, b;

		l = 0;
		a = ctrls[0];

		for(i = 1; i < SEGMENT_SUBDIVIDE_SIZE; i++)
		{
			b = m->xyz[i];
			l += vec_length( b - a );

			b = a;
		}

		length = l;
	}
}
