///////////////////////////////////////////////////////////////////////////////
// Brush.cpp
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
#include "resource.h"
#include "Brush.h"
#include "shaders.h"
#include "MapView.h"
#include "TreadDoc.h"
#include "r_sys.h"
#include "mapfile.h"
#include "ents.h"
#include "InputLineDialog.h"
#include "Quake.h"
#include "Quake2.h"

#define NO_SHADOW_SURF	(SURF_NO_DRAW|SURF_SKY_BOX|SURF_SKY_PORTAL)
#define DEFAULT_TEXTURE_SCALE	1.0f

///////////////////////////////////////////////////////////////////////////////
// CUndoRedoHollow
///////////////////////////////////////////////////////////////////////////////

CUndoRedoHollow::CUndoRedoHollow()
{
}

CUndoRedoHollow::~CUndoRedoHollow()
{
}

void CUndoRedoHollow::Undo(CTreadDoc *doc)
{
	doc->DeleteSelection();
	CUndoRedoManager::DuplicateListIntoDocument(&m_SrcList, doc);
	doc->UpdateSelectionInterface();
	doc->Prop_UpdateSelection();
}

void CUndoRedoHollow::Redo(CTreadDoc *doc)
{
	doc->DeleteSelection();
	CUndoRedoManager::DuplicateListIntoDocument(&m_ResultList, doc);
	doc->UpdateSelectionInterface();
	doc->Prop_UpdateSelection();
}

void CUndoRedoHollow::ReconnectEntDefs(CTreadDoc *doc)
{
	m_SrcList.WalkList(CTreadDoc::ReconnectEntity, doc);
	m_ResultList.WalkList(CTreadDoc::ReconnectEntity, doc);
}

bool CUndoRedoHollow::WriteToFile(CFile* file, CTreadDoc* doc, int nVersion)
{
	CUndoRedoAction::WriteToFile(file, doc, nVersion);
	MAP_WriteObjectList(file, doc, nVersion, &m_SrcList);
	MAP_WriteObjectList(file, doc, nVersion, &m_ResultList);
	return true;
}

bool CUndoRedoHollow::ReadFromFile(CFile* file, CTreadDoc* doc, int nVersion)
{
	CUndoRedoAction::ReadFromFile(file, doc, nVersion);
	MAP_ReadObjectList(file, doc, nVersion, &m_SrcList);
	MAP_ReadObjectList(file, doc, nVersion, &m_ResultList);
	return true;
}

void CUndoRedoHollow::InitUndo(CLinkedList<CMapObject>* pSrc, CLinkedList<CMapObject>* pResult, CTreadDoc* pDoc)
{
	CUndoRedoManager::CopyObjectList(pSrc, &m_SrcList, pDoc);
	CUndoRedoManager::CopyObjectList(pResult, &m_ResultList, pDoc);
}

///////////////////////////////////////////////////////////////////////////////
// CUndoRedoCarve
///////////////////////////////////////////////////////////////////////////////

CUndoRedoCarve::CUndoRedoCarve() :
m_selectedUIDs(0),
m_numSelectedUIDs(0),
m_srcUIDs(0),
m_numSrcUIDs(0),
m_resultUIDs(0),
m_numResultUIDs(0)
{
}

CUndoRedoCarve::~CUndoRedoCarve()
{
	if (m_selectedUIDs)
	{
		CUndoRedoManager::DeleteUIDs(m_selectedUIDs);
	}
	if (m_srcUIDs)
	{
		CUndoRedoManager::DeleteUIDs(m_srcUIDs);
	}
	if (m_resultUIDs)
	{
		CUndoRedoManager::DeleteUIDs(m_resultUIDs);
	}
}

void CUndoRedoCarve::Undo(CTreadDoc *doc)
{
	doc->ClearSelection();
	CUndoRedoManager::SelectObjectsByUIDs(doc, m_resultUIDs, m_numResultUIDs);
	doc->DeleteSelection();
	CUndoRedoManager::DuplicateListIntoDocument(&m_SrcList, doc);
	doc->ClearSelection();
	CUndoRedoManager::SelectObjectsByUIDs(doc, m_selectedUIDs, m_numSelectedUIDs);
	doc->UpdateSelectionInterface();
	doc->Prop_UpdateSelection();
}

void CUndoRedoCarve::Redo(CTreadDoc *doc)
{
	doc->ClearSelection();
	CUndoRedoManager::SelectObjectsByUIDs(doc, m_srcUIDs, m_numSrcUIDs);
	doc->DeleteSelection();
	CUndoRedoManager::DuplicateListIntoDocument(&m_ResultList, doc);
	doc->ClearSelection();
	CUndoRedoManager::SelectObjectsByUIDs(doc, m_selectedUIDs, m_numSelectedUIDs);
	doc->UpdateSelectionInterface();
	doc->Prop_UpdateSelection();
}

void CUndoRedoCarve::ReconnectEntDefs(CTreadDoc *doc)
{
	m_SrcList.WalkList(CTreadDoc::ReconnectEntity, doc);
	m_ResultList.WalkList(CTreadDoc::ReconnectEntity, doc);
}

bool CUndoRedoCarve::WriteToFile(CFile* file, CTreadDoc* doc, int nVersion)
{
	CUndoRedoAction::WriteToFile(file, doc, nVersion);
	MAP_WriteObjectList(file, doc, nVersion, &m_SrcList);
	MAP_WriteObjectList(file, doc, nVersion, &m_ResultList);
	MAP_WriteInt(file, m_numSelectedUIDs);
	for (int i = 0; i < m_numSelectedUIDs; ++i)
	{
		MAP_WriteInt(file, m_selectedUIDs[i]);
	}
	MAP_WriteInt(file, m_numSrcUIDs);
	for (int i = 0; i < m_numSrcUIDs; ++i)
	{
		MAP_WriteInt(file, m_srcUIDs[i]);
	}
	MAP_WriteInt(file, m_numResultUIDs);
	for (int i = 0; i < m_numResultUIDs; ++i)
	{
		MAP_WriteInt(file, m_resultUIDs[i]);
	}
	return true;
}

bool CUndoRedoCarve::ReadFromFile(CFile* file, CTreadDoc* doc, int nVersion)
{
	CUndoRedoAction::ReadFromFile(file, doc, nVersion);
	MAP_ReadObjectList(file, doc, nVersion, &m_SrcList);
	MAP_ReadObjectList(file, doc, nVersion, &m_ResultList);
	m_numSelectedUIDs = MAP_ReadInt(file);
	if (m_numSelectedUIDs > 0)
	{
		m_selectedUIDs = CUndoRedoManager::AllocateUIDs(m_numSelectedUIDs);
		for (int i = 0; i < m_numSelectedUIDs; ++i)
		{
			m_selectedUIDs[i] = MAP_ReadInt(file);
		}
	}
	else
	{
		m_selectedUIDs = 0;
	}
	m_numSrcUIDs = MAP_ReadInt(file);
	if (m_numSrcUIDs > 0)
	{
		m_srcUIDs = CUndoRedoManager::AllocateUIDs(m_numSrcUIDs);
		for (int i = 0; i < m_numSrcUIDs; ++i)
		{
			m_srcUIDs[i] = MAP_ReadInt(file);
		}
	}
	else
	{
		m_srcUIDs = 0;
	}
	m_numResultUIDs = MAP_ReadInt(file);
	if (m_numResultUIDs > 0)
	{
		m_resultUIDs = CUndoRedoManager::AllocateUIDs(m_numResultUIDs);
		for (int i = 0; i < m_numResultUIDs; ++i)
		{
			m_resultUIDs[i] = MAP_ReadInt(file);
		}
	}
	else
	{
		m_resultUIDs = 0;
	}
	return true;
}

void CUndoRedoCarve::InitUndo(CLinkedList<CMapObject>* pSelected, CLinkedList<CMapObject>* pSrc, CLinkedList<CMapObject>* pResult, CTreadDoc* pDoc)
{
	CUndoRedoManager::CopyObjectList(pSrc, &m_SrcList, pDoc);
	CUndoRedoManager::CopyObjectList(pResult, &m_ResultList, pDoc);
	m_selectedUIDs = CUndoRedoManager::GetObjectUIDs(pSelected, &m_numSelectedUIDs);
	m_srcUIDs = CUndoRedoManager::GetObjectUIDs(pSrc, &m_numSrcUIDs);
	m_resultUIDs = CUndoRedoManager::GetObjectUIDs(pResult, &m_numResultUIDs);
}

///////////////////////////////////////////////////////////////////////////////
// CQBrushVertex_Manipulator
///////////////////////////////////////////////////////////////////////////////

CQBrushVertex_Manipulator::CQBrushVertex_Manipulator() : CManipulator()
{
	pos = 0;
	size = 0.0f;
	color = 0;
	hlcolor = 0;
	m_bHover = false;
	brush = 0;
	m_bVertDrag = false;
	m_onlist = 0;
	m_bOnList = false;
}

CQBrushVertex_Manipulator::~CQBrushVertex_Manipulator()
{
}

int CQBrushVertex_Manipulator::DragVert( CManipulator* m, void* p, void* p2 )
{
	CQBrushVertex_Manipulator* bm = dynamic_cast<CQBrushVertex_Manipulator*>(m);
	if( bm && !bm->m_bOnList )
	{
		(*bm->pos) += *((vec3*)p);
		bm->brush->OnVertexDrag();
	}

	return 0;
}

void CQBrushVertex_Manipulator::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( pView->GetViewType() == VIEW_TYPE_3D )
	{
		if( !m_bSelected )
		{
			if( !(nButtons&MS_CONTROL) )
			{
				pView->GetDocument()->ClearSelectedManipulators();
			}

			pView->GetDocument()->AddManipulatorToSelection( this );
			pView->GetDocument()->UpdateSelectionInterface();
			Sys_RedrawWindows();
		}
		else
		{
			if( (nButtons&MS_CONTROL) )
			{
				pView->GetDocument()->AddManipulatorToMap( this );
				pView->GetDocument()->UpdateSelectionInterface();
				Sys_RedrawWindows();
			}
		}
	}
	else
	{
		Sys_SetMouseCapture( pView );

		//
		// get all hit records from the click.
		//
		if( !m_bSelected )
		{
			if( !(nButtons&MS_CONTROL) )
			{
				pView->GetDocument()->ClearSelectedManipulators();
			}
		}

		CPickObject** list;
		int num;

		R_PickObjectList( pView, nMX, nMY, 1, 1, &list, &num, PICK_MANIPULATORS );
		if( num > 0 )
		{
			int i;
			
			m_numon = 0;

			for(i = 0; i < num; i++)
			{
				if( dynamic_cast<CQBrushVertex_Manipulator*>(list[i]) != 0 )
					m_numon++;
				else
					continue;

				pView->GetDocument()->AddManipulatorToSelection( (CManipulator*)list[i] );
				if( nButtons&MS_SHIFT ) // don't select multiples.
					break;
			}

			if( m_numon > 0 )
			{
				int c;

				c = 0;
				m_onlist = new CQBrushVertex_Manipulator*[m_numon];

				for(i = 0; i < num; i++)
				{
					CQBrushVertex_Manipulator* vm = dynamic_cast<CQBrushVertex_Manipulator*>(list[i]);
					if( vm )
					{
						m_onlist[c++] = vm;
						vm->m_bOnList = true;
						vm->brush->m_bTryWeld = false;
					}

					if (c == m_numon) break;
				}

				OS_ASSERT(c == m_numon);

				m_bVertDrag = true;
				m_bSnapSel = false;
				m_bMoved = false;

				pView->GetDocument()->UpdateSelectionInterface();
				Sys_RedrawWindows();
			}

			R_DeletePickObjectList(list);
		}
	}
}

void CQBrushVertex_Manipulator::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = false;
	m_bVertDrag = false;

	Sys_SetMouseCapture( 0 );
	pView->GetDocument()->ClearAllTrackPicks();

	if( m_onlist )
	{
		int i;
		for(i = 0; i < m_numon; i++)
		{
			m_onlist[i]->m_bOnList = false;
		}

		delete[] m_onlist;
	}

	m_numon = 0;
	m_onlist = 0;

	//
	// try weld all brushes.
	//
	CMapObject* obj;
	CQBrush* brush;

	for( obj = pView->GetDocument()->GetSelectedObjectList()->ResetPos(); obj; obj = pView->GetDocument()->GetSelectedObjectList()->GetNextItem() )
	{
		brush = dynamic_cast<CQBrush*>(obj);
		if( brush )
		{
			brush->TryWeldVerts( pView->GetDocument(), USER_WELD_DIST, true );
		}
	}

	pView->GetDocument()->BuildSelectionBounds();
	Sys_RedrawWindows();
}

void CQBrushVertex_Manipulator::OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( m_bVertDrag )
	{
		float mx, my;
		float dx, dy;
		int x_axis, y_axis;

		pView->WinXYToMapXY( nMX, nMY, &mx, &my );

		//
		// snap the sucker.
		//
		if( pView->GetGridSnap() )
		{
			mx = Sys_Snapf( mx, pView->GetGridSize() );
			my = Sys_Snapf( my, pView->GetGridSize() );

			if( m_bSnapSel )
			{
				if( mx == m_lastx && my == m_lasty )
					return;// no movement.
			}

			m_bSnapSel = true;
			m_lastx = mx;
			m_lasty = my;
		}

		x_axis = QUICK_AXIS( pView->View.or2d.lft );
		y_axis = QUICK_AXIS( pView->View.or2d.up );

		//
		// snap the on list.
		//
		int i;
		for(i = 0; i < m_numon; i++)
		{
			if( i == 0 )
			{
				dx = mx - (*(m_onlist[i]->pos))[x_axis];
				dy = my - (*(m_onlist[i]->pos))[y_axis];

				if( (dx != 0 || dy != 0) && !m_bMoved)
				{
					m_bMoved = true;
					pView->GetDocument()->GenericUndoRedoFromSelection()->SetTitle("Vertex Drag");
				}
			}

			(*(m_onlist[i]->pos))[x_axis] = mx;
			(*(m_onlist[i]->pos))[y_axis] = my;

			m_onlist[i]->brush->OnVertexDrag();
		}

		//
		// translate the remaining vertices.
		//
		vec3 drag = vec3::zero;

		drag[x_axis] = dx;
		drag[y_axis] = dy;

		pView->GetDocument()->GetSelectedManipulatorList()->WalkList( DragVert, &drag );
		pView->GetDocument()->BuildSelectionBounds();

		Sys_RedrawWindows();
	}
}

void CQBrushVertex_Manipulator::OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = true;

	if( m_bSelected )
		pView->GetDocument()->AddManipulatorToSelection( this ); // this little trick pushes it forward to be visible in all views.
	else
		pView->GetDocument()->AddManipulatorToMap( this ); // this little trick pushes it forward to be visible in all views.

	Sys_RedrawWindows();
}

void CQBrushVertex_Manipulator::OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = false;
	Sys_RedrawWindows();
}

void CQBrushVertex_Manipulator::OnDraw( CMapView* pView )
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
		glColor4ub( 0xFF, 0, 0, 0xFF );
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

	mins = (*pos) - (s*0.5f);
	maxs = (*pos) + (s*0.5f);

	R_DrawBox( mins, maxs );
}

///////////////////////////////////////////////////////////////////////////////
// CQBrushHandle_Manipulator
///////////////////////////////////////////////////////////////////////////////

CQBrushHandle_Manipulator::CQBrushHandle_Manipulator()
{
	m_bHover = false;
	brush = 0;
	SetViewFlags( VIEW_FLAG_2D );
}

CQBrushHandle_Manipulator::~CQBrushHandle_Manipulator()
{
}

bool CQBrushHandle_Manipulator::OnPopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( !brush->IsSelected() )
		return false;

	CQuakeUserData *ud = static_cast<CQuakeUserData*>(pView->GetDocument()->UserData());
	ud->MakeBrushMenu(pView->GetDocument());
	ud->m_BrushMenu.active_brush = brush;
	ud->m_BrushMenu.brush_face = &brush->m_faces[0];
	ud->m_BrushMenu.view = pView;

	Sys_DisplayObjectMenu( pView, nMX, nMY, &ud->m_BrushMenu );
	return true;
}

void CQBrushHandle_Manipulator::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bDrag = true;
	m_bMoved = false;

	if( brush->IsSelected() )
	{
		if( (nButtons&MS_CONTROL) )
		{
			pView->GetDocument()->MakeUndoDeselectAction();
			//pView->GetDocument()->AddObjectToMap( brush );
			brush->Deselect( pView->GetDocument() );
			pView->GetDocument()->UpdateSelectionInterface();
			pView->GetDocument()->Prop_UpdateSelection();
			Sys_RedrawWindows();
			m_bDrag = false;
			return;
		}
	}
	else
	{
		if( !(nButtons&MS_CONTROL) )
		{
			pView->GetDocument()->MakeUndoDeselectAction();
			pView->GetDocument()->ClearSelection();
		}
	
		//pView->GetDocument()->AddObjectToSelection( brush );
		brush->Select( pView->GetDocument() );
		pView->GetDocument()->Prop_UpdateSelection();
	}

	pView->GetDocument()->UpdateSelectionInterface();
	Sys_BeginDragSel( pView, nMX, nMY, nButtons );
	Sys_RedrawWindows();
}

void CQBrushHandle_Manipulator::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = false;

	if( m_bDrag )
	{
		Sys_EndDragSel( pView, nMX, nMY, nButtons );
		
		//
		// didn't move?
		//
		if( !m_bMoved && !(nButtons&MS_CONTROL) )
		{
			pView->GetDocument()->MakeUndoDeselectAction();
			pView->GetDocument()->ClearSelection();
			//pView->GetDocument()->AddObjectToSelection( brush );
			brush->Select( pView->GetDocument() );
			pView->GetDocument()->Prop_UpdateSelection();
		}

		pView->GetDocument()->UpdateSelectionInterface();
	}

	Sys_RedrawWindows();
}

void CQBrushHandle_Manipulator::OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( m_bDrag )
		m_bMoved = Sys_DragSel( pView, nMX, nMY, nButtons ) || m_bMoved;
}

void CQBrushHandle_Manipulator::OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = true;
	pView->RedrawWindow();
	Sys_SetCursor( pView, TC_DEFAULT );
}

void CQBrushHandle_Manipulator::OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = false;
	pView->RedrawWindow();
}

void CQBrushHandle_Manipulator::OnDraw( CMapView* pView )
{
	float size = 4.0f*pView->View.fInvScale;
	float ofs = 0.0f;

	//if( size < 16.0f )
	//	size = 16.0f;
	vec3 mpos;
	vec3 mins0, maxs0, mins1, maxs1;
	
	mpos = brush->m_pos;
	if( equals( mpos, pView->GetDocument()->m_selpos, 1.0f ) )
		ofs = 16.0f;

	if( m_bHover )
	{
		glColor4ub( 0x0, 0x00, 0xFF, 0xFF );
		//size += 1.2*pView->View.fInvScale;
		size *= 2;
	}
	else
	if( brush->IsSelected() )
		glColor4ub( 255, 0, 0, 255 );
	else
		glColor4ub( 0x20, 0xFF, 0x10, 0xFF );
	
	glBegin( GL_LINES );

	mins0 = mpos+(pView->View.or2d.lft*size)+(pView->View.or2d.up*size)-(pView->View.or2d.up*ofs);
	maxs0 = mpos-(pView->View.or2d.lft*size)-(pView->View.or2d.up*size)-(pView->View.or2d.up*ofs);
	glVertex3fv( mins0 );
	glVertex3fv( maxs0 );

	mins1 = mpos+(pView->View.or2d.lft*size)-(pView->View.or2d.up*size)-(pView->View.or2d.up*ofs);
	maxs1 = mpos-(pView->View.or2d.lft*size)+(pView->View.or2d.up*size)-(pView->View.or2d.up*ofs);
	glVertex3fv( mins1 );
	glVertex3fv( maxs1 );

	glEnd();

}

///////////////////////////////////////////////////////////////////////////////
// CQBrush
///////////////////////////////////////////////////////////////////////////////

// Defines base axis and there s,t axial directions.
static float vBaseAxis[18][3] =
{
{0,0,1}, {1,0,0}, {0,-1,0},			// floor
{0,0,-1}, {1,0,0}, {0,-1,0},		// ceiling
{1,0,0}, {0,1,0}, {0,0,-1},			// west wall
{-1,0,0}, {0,1,0}, {0,0,-1},		// east wall
{0,1,0}, {1,0,0}, {0,0,-1},			// south wall
{0,-1,0}, {1,0,0}, {0,0,-1}			// north wall
};

#define FRONT	0
#define BACK	1
#define ON		2
#define BASE_CUBE_SIZE		128
int CQBrush::ms_HollowDepth = 16;

CQBrush::BrushFaceChange_t::BrushFaceChange_t()
{
	s_shader = "";
	shift[0] = shift[1] = scale[0] = scale[1] = 0.0f;
	rot = 0.0f;
}

CQBrush::BrushFaceChange_t::~BrushFaceChange_t()
{
}

CQBrush::CQBrushParmsMenu::CQBrushParmsMenu() : CObjectMenu()
{
	active_brush = 0;
	brush_face = 0;
	view = 0;
}

CQBrush::CQBrushParmsMenu::~CQBrushParmsMenu()
{
}

void CQBrush::CQBrushParmsMenu::OnUpdateCmdUI( int id, CCmdUI* pUI )
{
	if( id >= BM_FIRST_LINK_TO_ENTITY_MENU )
	{
		//
		// are all selections brushes?
		//
		bool enable = view->GetDocument()->GetSelectedObjectCount() ==
					  view->GetDocument()->GetSelectedObjectCount( MAPOBJ_CLASS_BRUSH );

		pUI->Enable( enable );
		return;
	}

	switch( id )
	{
	case BM_EDIT_FACES:
		pUI->Enable();
		pUI->SetCheck( view->GetDocument()->IsEditingFaces() );
	break;
	case BM_EDIT_VERTS :
		pUI->Enable();
		pUI->SetCheck( view->GetDocument()->IsEditingVerts() );
	break;
	case BM_LOCK_TEXTURE:
		pUI->Enable();
		pUI->SetCheck( active_brush->m_texlock );
	break;
	case BM_GRAB_TEXTURE:
		pUI->Enable();
		{
			char buff[256];
			sprintf(buff, "Grab \"%s\"", (brush_face->p_shader) ? brush_face->p_shader->DisplayName() : brush_face->s_shader );
			pUI->SetText( buff );
		}
	break;
	case BM_APPLY_TEXTURE:
		pUI->Enable();
		{
			char buff[256];
			sprintf(buff, "Apply \"%s\"", view->GetDocument()->SelectedShaderDisplayName() );
			pUI->SetText( buff );
		}
	break;
	case BM_SELECT_ALL_WITH_TEXTURE:
		pUI->Enable();
		{
			char buff[256];
			if( view->GetDocument()->IsEditingFaces() )
			{
				sprintf(buff, "Select Faces With \"%s\"", (brush_face->p_shader) ? brush_face->p_shader->DisplayName() : brush_face->s_shader );
			}
			else
			{
				sprintf(buff, "Select Objects With \"%s\"", (brush_face->p_shader) ? brush_face->p_shader->DisplayName() : brush_face->s_shader );
			}
			pUI->SetText( buff );
		}
	break;
	/*case 6:
	case 7:
	case 8:
	case 9:

		pUI->Enable( view->GetDocument()->IsEditingFaces() && (view->GetViewType()==VIEW_TYPE_3D) );
		if( view->GetDocument()->IsEditingFaces()  && (view->GetViewType()==VIEW_TYPE_3D) )
		{
			int texplane = (id==9)?-1:((id-6)*2);
			bool check=true;

			view->GetDocument()->GetSelectedObjectList()->WalkList( AreBrushFacesTexturedFromPlane, (void*)texplane, &check );
			pUI->SetCheck( check );
		}

	break;*/

	case BM_SNAP_TO_GRID:
		pUI->Enable();
	break;

	case BM_UNLINK_FROM_ENTITY:
		{
			//
			// are all selections brushes?
			//
			bool enable = view->GetDocument()->GetSelectedObjectCount() ==
						  view->GetDocument()->GetSelectedObjectCount( MAPOBJ_CLASS_ALL, MAPOBJ_SUBCLASS_OWNED|MAPOBJ_SUBCLASS_OWNER );
			
			//
			// make sure they're all from the same entity.
			//
			if( enable )
			{
				CMapObject* obj = view->GetDocument()->GetSelectedObjectList()->GetItem(LL_HEAD);

				CQBrush* b = dynamic_cast<CQBrush*>(obj);
				if( b )
				{
					int uid = b->GetOwnerUID();
					view->GetDocument()->GetSelectedObjectList()->WalkList( AreBrushesLinkedToSameEntity, (void*)uid, &enable );
				}
				else
				{
					CEntity* e = dynamic_cast<CEntity*>(obj);
					if( e )
					{
						int uid = e->GetUID();
						view->GetDocument()->GetSelectedObjectList()->WalkList( AreBrushesLinkedToSameEntity, (void*)uid, &enable );
					}
					else
					{
						enable = false;
					}
				}
			}

			pUI->Enable( enable );
		}
	break;

	case BM_CSG_HOLLOW:
	case BM_CSG_CARVE:
		{
			bool enable = view->GetDocument()->GetSelectedObjectCount() ==
						  view->GetDocument()->GetSelectedObjectCount(MAPOBJ_CLASS_BRUSH, MAPOBJ_SUBCLASS_NONE);
			pUI->Enable(enable);
		} 
	break;

	case BM_SELECT_BRUSHES_WITH_SAME_CONTENTS:
	case BM_SELECT_BRUSHES_WITH_SAME_SURFACE:

		pUI->Enable();

	break;
	}
}

void CQBrush::CQBrushParmsMenu::OnMenuItem( int id )
{
	if( id >= BM_FIRST_LINK_TO_ENTITY_MENU )
	{
		int num = id-BM_FIRST_LINK_TO_ENTITY_MENU;
		int ofs = 0;
		CEntDef* def;
		CLinkedList<CEntDef>* list = Sys_GetActiveDocument()->GameDef()->EntDefList();

		for( def = list->ResetPos(); def; def = list->GetNextItem() )
		{
			if( def->IsOwner() )
			{
				if( ofs == num )
					break;
				ofs++;
			}
		}

		CLinkedList<CMapObject> *objects = view->GetDocument()->GetSelectedObjectList();

		{
			CString repaint = def->GetRepaint();

			if (!repaint.IsEmpty())
			{
				CShader *shader = view->GetDocument()->ShaderForName(repaint);
				for (CMapObject *obj = objects->ResetPos(); obj; obj = objects->GetNextItem())
				{
					CQBrush *brush = dynamic_cast<CQBrush*>(obj);
					if (brush)
					{
						brush->InternalSetShaderName(repaint, shader, view->GetDocument());
					}
				}
			}
		}

		CEntity* ent = (CEntity*)CEntity::MakeEntity( view->GetDocument(), def );
		ent->SetObjectWorldPos( vec3::zero, view->GetDocument() );
		
		view->GetDocument()->AssignUID( ent );
		view->GetDocument()->AddObjectToMap( ent );
		ent->OwnObjectList( view->GetDocument(), objects );
		view->GetDocument()->AddObjectToSelection( ent );
		view->GetDocument()->UpdateSelectionInterface();
		view->GetDocument()->Prop_UpdateSelection();
		Sys_RedrawWindows();

		view->GetDocument()->MakeUndoLinkAction( ent );

		return;
	}

	switch( id )
	{
	case BM_EDIT_FACES:
	case BM_EDIT_VERTS:
	{
		bool bEditFaces = (id == 1) ? !view->GetDocument()->IsEditingFaces() : false;
		bool bEditVerts = (id == 2) ? !view->GetDocument()->IsEditingVerts() : false;

		if( bEditFaces )
			view->GetDocument()->SetEditingFaces( true );
		if( bEditVerts )
			view->GetDocument()->SetEditingVerts( true );

		if( !bEditFaces && !bEditVerts )
		{
			view->GetDocument()->SetEditingFaces( false );
			view->GetDocument()->SetEditingVerts( false );
		}

		if( bEditFaces )
			view->GetDocument()->GetSelectedObjectList()->WalkList( EnterBrushFaceMode );
		else
			view->GetDocument()->GetSelectedObjectList()->WalkList( ExitBrushFaceMode );

		if( bEditVerts )
			view->GetDocument()->GetSelectedObjectList()->WalkList( EnterBrushVertexMode, view->GetDocument() );
		else
			view->GetDocument()->GetSelectedObjectList()->WalkList( ExitBrushVertexMode, view->GetDocument() );

		view->GetDocument()->UpdateSelectionInterface();
		view->GetDocument()->Prop_UpdateSelection();
		Sys_RedrawWindows();
	}
	break;

	case BM_LOCK_TEXTURE:

		view->GetDocument()->GetSelectedObjectList()->WalkList( LockTexture, (void*)(!active_brush->m_texlock) );

	break;

	case BM_GRAB_TEXTURE:

		// is it a face?
		{
			BrushFace_t* f;

			if( brush_face )
				f = brush_face;
			else
				f = &active_brush->m_faces[0];

			view->GetDocument()->SetSelectedShader( f->p_shader );
			Sys_AdjustToViewTexture( f->p_shader );
			Sys_RedrawWindows( VIEW_TYPE_TEXTURE );
		}

	break;

	case BM_APPLY_TEXTURE:

		view->GetDocument()->GenericUndoRedoFromSelection()->SetTitle( "Paint Selection" );
		view->GetDocument()->PaintSelection();
		Sys_RedrawWindows( VIEW_TYPE_3D );

	break;

	case BM_SELECT_ALL_WITH_TEXTURE:

		view->GetDocument()->SelectObjectsByShader( brush_face->s_shader );
		view->GetDocument()->UpdateSelectionInterface();
		view->GetDocument()->Prop_UpdateSelection();
		Sys_RedrawWindows();

	break;

	/*case 6:
	case 7:
	case 8:
	case 9:
		{
			int texplane = (id==9)?-1:((id-6)*2);

			view->GetDocument()->GenericUndoRedoFromSelection()->SetTitle( "Mapping Change" );
			view->GetDocument()->GetSelectedObjectList()->WalkList( SetBrushFaceTexturePlane, (void*)texplane );
			Sys_RedrawWindows( VIEW_TYPE_3D );

		}
	break;*/

	case BM_SNAP_TO_GRID:
		{
			bool x, y, z;

			if( view->GetViewType() == VIEW_TYPE_3D )
			{
				x = y = z = true;
			}
			else
			{
				int xaxis = QUICK_AXIS( view->View.or2d.lft );
				int yaxis = QUICK_AXIS( view->View.or2d.up );

				x = (xaxis == 0) || (yaxis == 0);
				y = (xaxis == 1) || (yaxis == 1);
				z = (xaxis == 2) || (yaxis == 2);
			}

			view->GetDocument()->GenericUndoRedoFromSelection()->SetTitle("Snap Selection To Grid");
			view->GetDocument()->SnapSelectionToGrid( view->GetGridSize(), x, y, z );
			view->GetDocument()->UpdateSelectionInterface();
			Sys_RedrawWindows();
		}
	break;

	case BM_UNLINK_FROM_ENTITY:
		{
			CEntity* ent;
			CMapObject* obj;
			CLinkedList<CMapObject>* list = view->GetDocument()->GetSelectedObjectList();

			ent = 0;

			for( obj = list->ResetPos(); obj; obj = list->GetNextItem() )
			{
				ent = dynamic_cast<CEntity*>(obj);
				if( ent )
				{
					break;
				}
			}

			if( ent )
			{
				ent->UnlinkOwnedObjects( view->GetDocument() );
				view->GetDocument()->DetachObject( ent );

				//
				// NOTE: undo system has object now.
				//
				view->GetDocument()->MakeUndoUnlinkAction( ent );

				view->GetDocument()->Prop_UpdateSelection();
				view->GetDocument()->UpdateSelectionInterface();
				Sys_RedrawWindows();
			}
		}
	break;

	case BM_CSG_HOLLOW:
		{
			
			CInputLineDialog &dlg = *CInputLineDialog::New();
			dlg.SetTitle("Specify Hollow Depth");
			dlg.SetFilter( FILTER_INT );
			dlg.AllowEmpty( FALSE );
			CString s;
			s.Format("%d", CQBrush::ms_HollowDepth);
			dlg.SetValue(s);
			if (dlg.DoModal() == IDOK)
			{
				CQBrush::ms_HollowDepth = atoi(dlg.GetValue());
				CQBrush::HollowSelected(CQBrush::ms_HollowDepth, view->GetDocument());
			}
			CInputLineDialog::Delete(&dlg);
		}
		break;

	case BM_CSG_CARVE:
		{
			CQBrush::CarveSelected(view->GetDocument());
		}
		break;

	case BM_SELECT_BRUSHES_WITH_SAME_CONTENTS:
	case BM_SELECT_BRUSHES_WITH_SAME_SURFACE:
		{
			int s, c;

			s = c = 0;

			if( id == BM_SELECT_BRUSHES_WITH_SAME_CONTENTS )
			{
				c = active_brush->GetContents();
				if( !c )
					c = CONTENTS_SOLID;
			}
			else
				s = active_brush->GetSurface();

			{
				BrushSelectAttributes bs;
				bs.doc = view->GetDocument();;
				bs.contents = c;
				bs.surface = s;

				view->GetDocument()->GetObjectList()->WalkList(CQBrush::SelectBrushByAttributes, &bs);
				view->GetDocument()->UpdateSelectionInterface();
				Sys_RedrawWindows();
			}
		}
	break;
	}
}

int CQBrush::SelectBrushByAttributes( CMapObject* obj, void* p1, void* p2 )
{
	BrushSelectAttributes* bs = (BrushSelectAttributes*)p1;

	CQBrush* b;

	if( obj->GetClass() != MAPOBJ_CLASS_BRUSH )
		return 0;
	if( !obj->IsVisible() )
		return 0;

	b = dynamic_cast<CQBrush*>(obj);
	if( !b )
		return 0;

	if( (b->GetContents()&bs->contents) || 
		(b->GetSurface()&bs->surface) ||
		((bs->contents&CONTENTS_SOLID)&&b->GetContents()==0) )
	{
		b->Select( bs->doc );
		return WALKLIST_RESTART; // this is important to return this.
	}
	
	return 0;
}

int CQBrush::AreBrushesLinkedToSameEntity( CMapObject* m, void* p, void* p2 )
{
	CQBrush* b = dynamic_cast<CQBrush*>(m);
	if( b )
	{
		int uid = (int)p;
		bool* result = (bool*)p2;

		if( b->GetOwnerUID() != uid )
		{
			*result = false;
			return WALKLIST_STOP;
		}
	}

	return 0;
}

void CQBrush::WriteToDXF( std::fstream& file, CTreadDoc* doc )
{
	int i, a, b;
	int ntris;
	BrushFace_t* f;
	BrushVert_t* v;

	ntris = 0;
	for(i = 0; i < m_numfaces; i++)
	{
		ntris += (m_faces[i].num_verts-2);
	}

	file << "0\nPOLYLINE\n8\n0\n66\n1\n70\n64\n71\n" << m_numxyz << "\n72\n" << ntris << "\n";
	file << "62\n128\n";

	for(i = 0; i < m_numxyz; i++)
	{
		file << "0\nVERTEX\n8\n0\n10\n" << m_xyz[i].x << "\n20\n" << m_xyz[i].y << "\n30\n" << m_xyz[i].z << "\n70\n192\n";
	}

	for(i = 0; i < m_numfaces; i++)
	{
		f = &m_faces[i];
		
		b = f->num_verts-1;
		
		//
		// write out triangles.
		//
		for( a = 0; a < f->num_verts-2; a++ )
		{	
			v = &f->verts[b];

			file << "0\nVERTEX\n8\n0\n10\n0\n20\n0\n30\n0\n70\n128\n71\n" << v->xyz+1;
			
			v = &f->verts[a+1];
			file << "\n72\n" << v->xyz+1;
			
			v = &f->verts[a];
			file << "\n73\n" << v->xyz+1 << "\n";
		}
	}

	file << "0\nSEQEND\n8\n0\n";
}

void CQBrush::SnapToGrid( float fGridSize, bool x, bool y, bool z )
{
	int i;

	for( i = 0; i < m_numxyz; i++ )
	{
		if( x )
			m_xyz[i][0] = Sys_Snapf( m_xyz[i][0], fGridSize );
		if( y )
			m_xyz[i][1] = Sys_Snapf( m_xyz[i][1], fGridSize );
		if( z )
			m_xyz[i][2] = Sys_Snapf( m_xyz[i][2], fGridSize );
	}

	UpdateRenderMeshXYZ();
	if( !m_texlock )
	{
		TextureBrush(0);
		UpdateRenderMeshTexture();
	}
	GenTXSpaceVecs();

	CMapObject::SnapToGrid( fGridSize, x, y, z );
}

int CQBrush::GetContents()
{
	int s = 0;
	int i;

	for(i = 0; i < m_numfaces; i++)
		s |= m_faces[i].contents;

	return s;
}

int CQBrush::AddContents(int bits)
{
	int s = 0;
	int i;

	for(i = 0; i < m_numfaces; i++)
	{
		m_faces[i].contents |= bits;
	}

	return GetContents();
}

int CQBrush::RemoveContents(int bits)
{
	int s = 0;
	int i;

	for(i = 0; i < m_numfaces; i++)
	{
		m_faces[i].contents &= ~bits;
	}

	return GetContents();
}

int CQBrush::AddSurface(int bits)
{
	int s = 0;
	int i;

	for(i = 0; i < m_numfaces; i++)
	{
		m_faces[i].surface |= bits;
	}

	return GetSurface();
}

int CQBrush::RemoveSurface(int bits)
{
	int s = 0;
	int i;

	for(i = 0; i < m_numfaces; i++)
	{
		m_faces[i].surface &= ~bits;
	}

	return GetSurface();
}

int CQBrush::GetSurface()
{
	int s = 0;
	int i;

	for(i = 0; i < m_numfaces; i++)
		s |= m_faces[i].surface;

	return s;
}

int CQBrush::ApplyShaderContents(bool selectedFacesOnly)
{
	for(int i = 0; i < m_numfaces; i++)
	{
		if ((!selectedFacesOnly || m_faces[i].selected) && m_faces[i].p_shader)
		{
			CQuake2Tex *tex = dynamic_cast<CQuake2Tex*>(m_faces[i].p_shader);
			if (tex)
			{
				m_faces[i].contents = tex->Contents();
				if (m_faces[i].contents == 0)
				{
					m_faces[i].contents = CONTENTS_SOLID;
				}
				m_faces[i].props[6].SetInt(m_faces[i].contents);
			}
		}
	}

	return GetContents();
}

int CQBrush::ApplyShaderSurface(bool selectedFacesOnly)
{
	for(int i = 0; i < m_numfaces; i++)
	{
		if ((!selectedFacesOnly || m_faces[i].selected) && m_faces[i].p_shader)
		{
			CQuake2Tex *tex = dynamic_cast<CQuake2Tex*>(m_faces[i].p_shader);
			if (tex)
			{
				m_faces[i].surface = tex->Surface();
				m_faces[i].props[5].SetInt(m_faces[i].surface);
			}
		}
	}

	return GetSurface();
}

void CQBrush::ApplyShaderValue(bool selectedFacesOnly)
{
	for(int i = 0; i < m_numfaces; i++)
	{
		if ((!selectedFacesOnly || m_faces[i].selected) && m_faces[i].p_shader)
		{
			CQuake2Tex *tex = dynamic_cast<CQuake2Tex*>(m_faces[i].p_shader);
			if (tex)
			{
				m_faces[i].value = tex->Value();
				m_faces[i].props[7].SetInt(m_faces[i].value);
			}
		}
	}
}

int CQBrush::GetFilterState( CTreadDoc* doc, CObjectFilter* filter )
{
	////
	//// check surfaces.
	////
	//int surf_flags = 0;

	//if( filter->BrushFilter.skybox )
	//	surf_flags |= SURF_SKY_BOX;
	//if( filter->BrushFilter.skyportal )
	//	surf_flags |= SURF_SKY_PORTAL;
	//if( filter->BrushFilter.nodraw )
	//	surf_flags |= SURF_NO_DRAW;

	//int i;
	//for(i = 0; i < m_numfaces; i++)
	//{
	//	if( m_faces[i].surface&surf_flags )
	//		return OBJFILTER_HIDE;
	//}

	////
	//// check contents.
	////
	//int contents_flags = 0;

	//if( filter->BrushFilter.always_shadow )
	//	contents_flags |= CONTENTS_ALWAYS_SHADOW;
	//if( filter->BrushFilter.areaportal )
	//	contents_flags |= CONTENTS_AREAPORTAL;
	//if( filter->BrushFilter.camera_clip )
	//	contents_flags |= CONTENTS_CAMERA_CLIP;
	//if( filter->BrushFilter.corona_block )
	//	contents_flags |= CONTENTS_CORONA_BLOCK;
	//if( filter->BrushFilter.detail )
	//	contents_flags |= CONTENTS_DETAIL;
	//if( filter->BrushFilter.monster_clip )
	//	contents_flags |= CONTENTS_MONSTER_CLIP;
	//if( filter->BrushFilter.noshadow )
	//	contents_flags |= CONTENTS_NO_SHADOW;
	//if( filter->BrushFilter.player_clip )
	//	contents_flags |= CONTENTS_PLAYER_CLIP;
	//if( filter->BrushFilter.solid )
	//	contents_flags |= CONTENTS_SOLID;
	//if( filter->BrushFilter.water )
	//	contents_flags |= CONTENTS_WATER;
	//if( filter->BrushFilter.window )
	//	contents_flags |= CONTENTS_WINDOW;

	//if( contents_flags&m_nContents )
	//	return OBJFILTER_HIDE;

	//if( contents_flags&CONTENTS_SOLID )
	//{
	//	if( m_nContents == 0 )
	//		return OBJFILTER_HIDE;
	//}

	return OBJFILTER_SHOW;
}

void CQBrush::SelectByShader( const char* szShader, CTreadDoc* pDoc )
{
	int i;
	BrushFace_t* f;

	if( !szShader || !szShader[0] )
		return;

	for( i = 0; i < m_numfaces; i++ )
	{
		f = &m_faces[i];
		if( f->s_shader.CompareNoCase( szShader ) == 0 )
		{
			if( !IsSelected() )
				Select( pDoc );

			if( !pDoc->IsEditingFaces() )
				break;

			//
			// select this faces.
			//
			SelectFace( i );
		}
		else
		{
			SelectFace( i, false );
		}
	}
}

int CQBrush::BrushFaceChange( CMapObject* p, void* parm, void* parm2 )
{
	CQBrush* b = dynamic_cast<CQBrush*>(p);
	BrushFaceChange_t* ch = (BrushFaceChange_t*)parm;

	if( b )
	{
		int i, k;

		for(i = 0; i < b->m_numfaces; i++)
		{
			if( !b->m_faces[i].selected )
				continue;

			b->m_faces[i].shift[0] += ch->shift[0];
			b->m_faces[i].props[0].SetFloat( b->m_faces[i].shift[0] );

			b->m_faces[i].shift[1] += ch->shift[1];
			b->m_faces[i].props[1].SetFloat( b->m_faces[i].shift[1] );

			b->m_faces[i].scale[0] += ch->scale[0];
			b->m_faces[i].props[2].SetFloat( b->m_faces[i].scale[0] );

			b->m_faces[i].scale[1] += ch->scale[1];
			b->m_faces[i].props[3].SetFloat( b->m_faces[i].scale[1] );

			b->m_faces[i].rot += ch->rot;
			b->m_faces[i].props[4].SetFloat( b->m_faces[i].rot );

			if( ch->s_shader != "" )
			{
				b->m_faces[i].s_shader = ch->s_shader;
				b->m_faces[i].p_shader = Sys_GetActiveDocument()->ShaderForName( ch->s_shader );
			}

			for(k = 0; k < b->m_faces[i].num_verts; k++)
			{
				b->m_faces[i].verts[k].st[0] += ch->shift[0];
				b->m_faces[i].verts[k].st[1] += ch->shift[1];
			}
		}

		if( ch->scale[0] != 0.0f || ch->scale[1] != 0.0f ||
			ch->rot != 0.0f )
		{
			b->TextureBrush(0);
		}

		if (b->GetType() == TYPE_Q2)
		{
			b->ApplyShaderContents(true);
			b->ApplyShaderSurface(true);
			b->ApplyShaderValue(true);
		}

		b->UpdateRenderMeshTexture();
		b->GenTXSpaceVecs();
	}

	return 0;
}

bool CQBrush::WriteToFile( CFile* pFile, CTreadDoc* pDoc, int nVersion )
{
	if( !m_numxyz || !m_numfaces )
		return true;

	CMapObject::WriteToFile( pFile, pDoc, nVersion );

	MAP_WriteInt( pFile, m_texlock );
	//
	// write the verts.
	//
	MAP_WriteInt( pFile, m_numxyz );

	int i;
	
	for(i = 0; i < m_numxyz; i++)
	{
		MAP_WriteVec3( pFile, &m_xyz[i] );
	}

	//
	// write the faces.
	//
	int k;
	BrushFace_t* f;

	MAP_WriteInt( pFile, m_numfaces );

	for( i = 0; i < m_numfaces; i++ )
	{
		f = &m_faces[i];
		
		MAP_WriteString( pFile, f->s_shader );
		MAP_WriteInt( pFile, f->surface );
		MAP_WriteInt(pFile, f->contents);
		MAP_WriteInt(pFile, f->value);
		MAP_WriteInt( pFile, f->selected );
		MAP_WriteFloat( pFile, f->shift[0] );
		MAP_WriteFloat( pFile, f->shift[1] );
		MAP_WriteFloat( pFile, f->scale[0] );
		MAP_WriteFloat( pFile, f->scale[1] );
		MAP_WriteFloat( pFile, f->rot );
		MAP_WritePlane( pFile, &f->plane );
		MAP_WriteInt( pFile, f->texplane );
		MAP_WriteInt( pFile, f->num_verts );

		for(k = 0; k < f->num_verts; k++)
		{
			MAP_WriteInt( pFile, f->verts[k].xyz );
			MAP_WriteVec2( pFile, &f->verts[k].st );
		}
	}

	return true;
}

bool CQBrush::ReadFromFile( CFile* pFile, CTreadDoc* pDoc, int nVersion )
{
	CMapObject::ReadFromFile( pFile, pDoc, nVersion );

	m_texlock = MAP_ReadInt( pFile );

	//
	// read verts.
	//
	int i;
	m_xyz = 0;
	m_numxyz = MAP_ReadInt( pFile );
	if( m_numxyz > 0 )
	{
		m_xyz = new vec3[m_numxyz];

		for(i = 0;i < m_numxyz; i++)
		{
			MAP_ReadVec3( pFile, &m_xyz[i] );
		}
	}

	//
	// read faces.
	//
	int k;
	BrushFace_t* f;

	m_faces = 0;
	m_numfaces = MAP_ReadInt( pFile );
	if( m_numfaces > 0 )
	{
		m_faces = new BrushFace_t[m_numfaces];
		for(i = 0; i < m_numfaces; i++)
		{
			f = &m_faces[i];

			f->s_shader = MAP_ReadString( pFile );
			f->surface = MAP_ReadInt( pFile );
			f->contents = MAP_ReadInt(pFile);
			f->value = MAP_ReadInt(pFile);

			PaintFaceWithContents( f );

			f->selected = MAP_ReadInt( pFile );
			f->shift[0] = MAP_ReadFloat( pFile );
			f->shift[1] = MAP_ReadFloat( pFile );
			f->scale[0] = MAP_ReadFloat( pFile );
			f->scale[1] = MAP_ReadFloat( pFile );
			f->rot = MAP_ReadFloat( pFile );
			MAP_ReadPlane( pFile, &f->plane );
			
			f->props[0].SetFloat( f->shift[0] );
			f->props[1].SetFloat( f->shift[1] );
			f->props[2].SetFloat( f->scale[0] );
			f->props[3].SetFloat( f->scale[1] );
			f->props[4].SetFloat( f->rot );
			f->props[5].SetInt(f->surface);
			f->props[6].SetInt(f->contents);
			f->props[7].SetInt(f->value);

			f->texplane = MAP_ReadInt( pFile );

			f->num_verts = MAP_ReadInt( pFile );
			f->verts = 0;
			if( f->num_verts > 0 )
			{
				f->verts = new BrushVert_t[f->num_verts];
				f->edges = new int[f->num_verts];

				for(k = 0; k < f->num_verts; k++)
				{
					f->verts[k].xyz = MAP_ReadInt( pFile );
					MAP_ReadVec2( pFile, &f->verts[k].st );
				}
			}
		}
	}

	AttachFaces();
	BuildInfo();

	MakeRenderMesh();
	UpdateRenderMeshTexture();
	GenTXSpaceVecs();

	return true;
}

void CQBrush::UpdateOwnerContents( CTreadDoc* pDoc )
{
	if (GetType() == TYPE_Q2)
	{
		CEntity* ent = GetOwner( pDoc );
		if( ent )
		{
			if( !stricmp( ent->GetClassname(), "func_areaportal" ) )
			{
				RemoveSurface(SURF_ALL);
				RemoveContents(CONTENTS_ALL);
				AddContents(CONTENTS_AREAPORTAL);
				PaintFacesWithContents();
				UpdateRenderMeshTexture();
				GenTXSpaceVecs();
			}
		}
		else
		{
			if( GetContents() & CONTENTS_AREAPORTAL )
			{
				RemoveSurface(SURF_ALL);
				RemoveContents(CONTENTS_ALL);
				for(int i = 0; i < m_numfaces; i++)
				{
					m_faces[i].s_shader = pDoc->SelectedShaderName();
					m_faces[i].p_shader = pDoc->SelectedShader();
				}
				ApplyShaderContents(false);
				ApplyShaderSurface(false);
				ApplyShaderValue(false);
				UpdateRenderMeshTexture();
				GenTXSpaceVecs();
			}
		}
	}
}

void CQBrush::MakePlanes()
{
	//
	// recompute all planes.
	//
	int i;
	BrushFace_t* f;

	for(i = 0; i < m_numfaces; i++)
	{
		f = &m_faces[i];
		f->plane = plane3( m_xyz[f->verts[2].xyz], m_xyz[f->verts[0].xyz], m_xyz[f->verts[1].xyz] );
	}
}

void CQBrush::OnVertexDrag()
{
	BuildInfo();

	MakePlanes();

	if( !m_texlock )
	{
		TextureBrush(0);
		UpdateRenderMeshTexture();
	}

	UpdateRenderMeshXYZ();
	GenTXSpaceVecs();
}

int CQBrush::LockTexture( CMapObject* p, void* parm, void* parm2 )
{
	CQBrush* b = dynamic_cast<CQBrush*>(p);
	if( b )
		b->m_texlock = (bool)parm;

	return 0;
}

int CQBrush::AreBrushFacesTexturedFromPlane( CMapObject* m, void* p, void* p2 )
{
	CQBrush* b = dynamic_cast<CQBrush*>(m);
	if( b )
	{
		int i;
		BrushFace_t* f;

		for( i = 0; i < b->m_numfaces; i++ )
		{
			f = &b->m_faces[i];
			if( f->selected )
			{
				if( f->texplane != (int)p )
				{
					*((bool*)p2) = false;
					return WALKLIST_STOP;
				}
			}
		}
	}

	return 0;
}

int CQBrush::SetBrushFaceTexturePlane( CMapObject* m, void* p, void* p2 )
{
	CQBrush* b = dynamic_cast<CQBrush*>(m);
	if( b )
	{
		int i;
		BrushFace_t* f;

		for( i = 0; i < b->m_numfaces; i++ )
		{
			f = &b->m_faces[i];
			if( f->selected )
			{
				f->texplane = (int)p;
			}
		}

		b->TextureBrush(0);
		b->UpdateRenderMeshTexture();
		b->GenTXSpaceVecs();
	}

	return 0;
}

int CQBrush::ExitBrushFaceMode( CMapObject* p, void* parm, void* parm2 )
{
	CQBrush* b = dynamic_cast<CQBrush*>(p);
	if( b )
		b->ExitFaceMode();

	return 0;
}

int CQBrush::EnterBrushFaceMode( CMapObject* p, void* parm, void* parm2 )
{
	CQBrush* b = dynamic_cast<CQBrush*>(p);
	if( b )
		b->EnterFaceMode();

	return 0;
}

void CQBrush::ExitFaceMode()
{
	DeselectFaces();
}

void CQBrush::EnterFaceMode()
{
	int i;

	for(i = 0; i < m_numfaces; i++)
	{
		SelectFace(i, true);
	}
}

int CQBrush::GetObjectTypeBits()
{
	if( GetOwnerUID() != -1 )
		return OBJECT_TYPE_BMODEL;

	return OBJECT_TYPE_WORLD;
}

void CQBrush::DeselectFaces()
{
	int i;
	
	for(i = 0; i < m_numfaces; i++)
	{
		SelectFace(i, false);
	}
}

void CQBrush::SelectFace( int i, bool select )
{
	CRenderMesh::poly_s* poly;

	if( i < 0 || i >= m_numfaces )
		return;

	poly = m_mesh.polys.GetItem(i);
	m_faces[i].selected = select;
	poly->selected = select;
	poly->selcolor = 0xFFFFFFFF;
}

#define HALF_CYLINDER_WIDTH		128
#define HALF_CYLINDER_HEIGHT	128

int CQBrush::ms_NumCylinderSides = 8;

CMapObject* CQBrush::MakeCylinder( CTreadDoc* doc, void* parm )
{
	CInputLineDialog &dlg = *CInputLineDialog::New();
	CString line;

	line.Format("%d", ms_NumCylinderSides );

	dlg.SetTitle("Number of Cylinder Sides");
	dlg.SetFilter( FILTER_INT );
	dlg.SetValue( line );
	dlg.AllowEmpty( FALSE );

	if( dlg.DoModal() != IDOK )
	{
		CInputLineDialog::Delete(&dlg);
		return 0;
	}
	
	CInputLineDialog::Delete(&dlg);

	//
	// make the cylinder.
	//
	int num = atoi( dlg.GetValue() );
	if( num < 3 )
	{
		MessageBox( 0, "A cylinder must have at least 3 sides.", "Make Cylinder", MB_TASKMODAL|MB_DEFBUTTON1|MB_OK );
		return 0;
	}
	if( num > 96 )
	{
		MessageBox( 0, "A cylinder cannot have more than 96 sides.", "Make Cylinder", MB_TASKMODAL|MB_DEFBUTTON1|MB_OK );
		return 0;
	}

	ms_NumCylinderSides = num;

	int i;
	float rads, radstep;
	PlaneInfo* planes = new PlaneInfo[num+2];

	rads = -QUARTER_CIRCLE_RADIANS;
	radstep = (2*PI) / (float)num;
	for(i = 0; i < num; i++)
	{
		planes[i].plane.a = cos( rads );
		planes[i].plane.b = sin( rads );
		planes[i].plane.c = 0.0f;
		planes[i].plane.d = (float)HALF_CYLINDER_WIDTH;

		rads += radstep;
	}

	planes[num].plane.a = 0.0f;
	planes[num].plane.b = 0.0f;
	planes[num].plane.c = 1.0f;
	planes[num].plane.d = HALF_CYLINDER_HEIGHT;

	planes[num+1].plane.a = 0.0f;
	planes[num+1].plane.b = 0.0f;
	planes[num+1].plane.c = -1.0f;
	planes[num+1].plane.d = HALF_CYLINDER_HEIGHT;

	CQBrush* b = BrushFromPlanes(static_cast<CQuakeGame*>(doc->UserData())->BrushType(), planes, num+2);
	if( !b ) return 0;

	for(i = 0; i < b->m_numfaces; i++)
	{
		b->m_faces[i].shift[0] = 0.0f;
		b->m_faces[i].shift[1] = 0.0f;
		b->m_faces[i].scale[0] = DEFAULT_TEXTURE_SCALE;
		b->m_faces[i].scale[1] = DEFAULT_TEXTURE_SCALE;
		b->m_faces[i].rot = 0;
	}

	b->MakePlanes();
	b->TextureBrush(doc->SelectedShaderName());
	b->MakeRenderMesh();
	b->UpdateRenderMeshTexture();
	b->GenTXSpaceVecs();
	b->BuildInfo();

	return b;
}

#define HALF_CYLINDER_HEIGHT	128
#define HALF_CYLINDER_WIDTH		128

int CQBrush::ms_NumConeSides = 8;
CMapObject* CQBrush::MakeCone( CTreadDoc* doc, void* parm )
{
	CInputLineDialog &dlg = *CInputLineDialog::New();
	CString line;

	line.Format("%d", ms_NumConeSides );

	dlg.SetTitle("Number of Cone Sides");
	dlg.SetFilter( FILTER_INT );
	dlg.SetValue( line );
	dlg.AllowEmpty( FALSE );

	if( dlg.DoModal() != IDOK )
	{
		CInputLineDialog::Delete(&dlg);
		return 0;
	}

	int num = atoi( dlg.GetValue() );

	CInputLineDialog::Delete(&dlg);

	//
	// make the cylinder.
	//
	
	if( num < 3 )
	{
		MessageBox( 0, "A cone must have at least 3 sides.", "Make Cone", MB_TASKMODAL|MB_DEFBUTTON1|MB_OK );
		return 0;
	}
	if( num > 96 )
	{
		MessageBox( 0, "A cone cannot have more than 96 sides.", "Make Cone", MB_TASKMODAL|MB_DEFBUTTON1|MB_OK );
		return 0;
	}

	ms_NumConeSides = num;

	vec3* xyz = new vec3[num+1]; // +1 for top.
	
	int i;
	float rads, radstep;
	
	rads = -QUARTER_CIRCLE_RADIANS;
	radstep = (2*PI) / (float)num;
	for(i = 0; i < num; i++)
	{
		xyz[i].x = cos( rads ) * (float)HALF_CYLINDER_WIDTH;
		xyz[i].y = sin( rads ) * (float)HALF_CYLINDER_WIDTH;
		xyz[i].z = -HALF_CYLINDER_HEIGHT;
		rads += radstep;
	}

	xyz[num].x = 0.0f;
	xyz[num].y = 0.0f;
	xyz[num].z = HALF_CYLINDER_HEIGHT;

	//
	// make the faces.
	//
	BrushFace_t* faces = new BrushFace_t[num+1];
	BrushFace_t* f;
	for(i = 0; i <  num; i++)
	{
		f = &faces[i];
		f->num_verts = 3;
		f->verts = new BrushVert_t[3];
		f->verts[0].xyz = num;
		f->verts[1].xyz = (i+1)%num;
		f->verts[2].xyz = i;
		f->edges = new int[3];
		f->shift[0] = 0;
		f->shift[1] = 0;
		f->scale[0] = DEFAULT_TEXTURE_SCALE;
		f->scale[1] = DEFAULT_TEXTURE_SCALE;
		f->rot = 0.0f;
	}

	f = &faces[num];
	f->num_verts = num;
	f->verts = new BrushVert_t[num];
	f->edges = new int[num];
	for(i = 0; i < num; i++)
	{
		f->verts[i].xyz = i;
	}

	CQBrush* b = new CQBrush(static_cast<CQuakeGame*>(doc->UserData())->BrushType());
	b->m_faces = faces;
	b->m_xyz = xyz;
	b->m_numxyz = num+1;
	b->m_numfaces = num+1;

	b->AttachFaces();
	b->MakePlanes();
	b->TextureBrush(doc->SelectedShaderName());
	b->MakeRenderMesh();
	b->UpdateRenderMeshTexture();
	b->GenTXSpaceVecs();
	b->BuildInfo();

	return b;
}

CMapObject* CQBrush::MakeCube(CTreadDoc* doc, void* parm)
{
	PlaneInfo planes[6];
	int i, k, ofs;

	ofs = 0;
	for(i = 0; i < 3; i++)
	{
		for(k = 0; k < 2; k++)
		{
			planes[ofs].plane = plane3( vec3::zero, 0.0f );
			if( k == 0 )
				planes[ofs].plane[i] = 1.0f;
			else
				planes[ofs].plane[i] = -1.0f;

			planes[ofs].scale[0] = DEFAULT_TEXTURE_SCALE;
			planes[ofs].scale[1] = DEFAULT_TEXTURE_SCALE;
			planes[ofs].shift[0] = 0.0f;
			planes[ofs].shift[1] = 0.0f;
			planes[ofs].rot = 0.0f;
			planes[ofs].plane.d = BASE_CUBE_SIZE;
			
			ofs++;
		}
	}

	CQBrush* b = BrushFromPlanes(static_cast<CQuakeGame*>(doc->GamePlugin())->BrushType(), planes, 6);
	if( !b )
		return 0;

	b->MakePlanes();
	b->TextureBrush(doc->SelectedShaderName());
	b->MakeRenderMesh();
	b->UpdateRenderMeshTexture();
	b->GenTXSpaceVecs();
	b->BuildInfo();

	return b;
}

const char* CQBrush::GetRootName()
{
	return "brush";
}

CQBrush::BrushFace_t::BrushFace_t() : CPickObject()
{
	props[0].SetName( "x shift" );
	props[0].SetDisplayName( "x shift" );
	props[0].SetInt( 0 );
	props[0].SetType( CObjProp::integer );

	props[1].SetName( "y shift" );
	props[1].SetDisplayName( "y shift" );
	props[1].SetInt( 0 );
	props[1].SetType( CObjProp::integer );

	props[2].SetName( "x scale" );
	props[2].SetDisplayName( "x scale" );
	props[2].SetFloat( DEFAULT_TEXTURE_SCALE );
	props[2].SetType( CObjProp::floatnum );

	props[3].SetName( "y scale" );
	props[3].SetDisplayName( "y scale" );
	props[3].SetFloat( DEFAULT_TEXTURE_SCALE );
	props[3].SetType( CObjProp::floatnum );

	props[4].SetName( "rotation" );
	props[4].SetDisplayName( "rotation" );
	props[4].SetFloat( 0 );
	props[4].SetType( CObjProp::floatnum );

	props[5].SetName( "surface flags" );
	props[5].SetDisplayName( "surface flags" );
	props[5].SetInt( 0 );
	props[5].SetType( CObjProp::integer );
	props[5].SetSubType( FALSE );

	{
		//
		// add surface properties here!
		//
		CObjProp* p = new CObjProp();
		p->SetName("Light");
		p->SetDisplayName( "Light");
		p->SetInt(SURF_LIGHT);
		p->SetType(CObjProp::integer);
		props[5].AddChoice(p);
		
		p = new CObjProp();
		p->SetName("Slick");
		p->SetDisplayName("Slick");
		p->SetInt(SURF_SLICK);
		p->SetType(CObjProp::integer);
		props[5].AddChoice(p);

		p = new CObjProp();
		p->SetName("Sky");
		p->SetDisplayName("Sky");
		p->SetInt(SURF_SKY);
		p->SetType(CObjProp::integer);
		props[5].AddChoice(p);

		p = new CObjProp();
		p->SetName("Warp");
		p->SetDisplayName("Warp");
		p->SetInt(SURF_WARP);
		p->SetType(CObjProp::integer);
		props[5].AddChoice(p);

		p = new CObjProp();
		p->SetName("Translucent 33%");
		p->SetDisplayName("Translucent 33%");
		p->SetInt(SURF_TRANS33);
		p->SetType(CObjProp::integer);
		props[5].AddChoice(p);

		p = new CObjProp();
		p->SetName("Translucent 66%");
		p->SetDisplayName("Translucent 66%");
		p->SetInt(SURF_TRANS66);
		p->SetType(CObjProp::integer);
		props[5].AddChoice(p);

		p = new CObjProp();
		p->SetName("Flowing");
		p->SetDisplayName("Flowing");
		p->SetInt(SURF_FLOWING);
		p->SetType(CObjProp::integer);
		props[5].AddChoice(p);

		p = new CObjProp();
		p->SetName("No Draw");
		p->SetDisplayName("No Draw");
		p->SetInt(SURF_NODRAW);
		p->SetType(CObjProp::integer);
		props[5].AddChoice(p);

		p = new CObjProp();
		p->SetName("Hint");
		p->SetDisplayName("Hint");
		p->SetInt(SURF_HINT);
		p->SetType(CObjProp::integer);
		props[5].AddChoice(p);
	}

	props[6].SetName("content flags");
	props[6].SetDisplayName("content flags");
	props[6].SetInt(0);
	props[6].SetType(CObjProp::integer);
	props[6].SetSubType(FALSE);

	{
		//
		// add content properties here!
		//
		CObjProp* p = new CObjProp();
		p->SetName("Solid");
		p->SetDisplayName( "Solid");
		p->SetInt(CONTENTS_SOLID);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Window");
		p->SetDisplayName("Window");
		p->SetInt(CONTENTS_WINDOW);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Aux");
		p->SetDisplayName("Aux");
		p->SetInt(CONTENTS_AUX);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Lava");
		p->SetDisplayName("Lava");
		p->SetInt(CONTENTS_LAVA);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Slime");
		p->SetDisplayName("Slime");
		p->SetInt(CONTENTS_SLIME);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Water");
		p->SetDisplayName("Water");
		p->SetInt(CONTENTS_WATER);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Mist");
		p->SetDisplayName( "Mist");
		p->SetInt(CONTENTS_MIST);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Player Clip");
		p->SetDisplayName("Player Clip");
		p->SetInt(CONTENTS_PLAYERCLIP);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);
		
		p = new CObjProp();
		p->SetName("Monster Clip");
		p->SetDisplayName("Monster Clip");
		p->SetInt(CONTENTS_MONSTERCLIP);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Current 0");
		p->SetDisplayName("Current 0");
		p->SetInt(CONTENTS_CURRENT_0);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Current 90");
		p->SetDisplayName("Current 90");
		p->SetInt(CONTENTS_CURRENT_90);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Current 180");
		p->SetDisplayName("Current 180");
		p->SetInt(CONTENTS_CURRENT_180);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Current 270");
		p->SetDisplayName("Current 270");
		p->SetInt(CONTENTS_CURRENT_270);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Current Up");
		p->SetDisplayName("Current Up");
		p->SetInt(CONTENTS_CURRENT_UP);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Current Down");
		p->SetDisplayName("Current Down");
		p->SetInt(CONTENTS_CURRENT_DOWN);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Detail");
		p->SetDisplayName("Detail");
		p->SetInt(CONTENTS_DETAIL);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Translucent");
		p->SetDisplayName("Translucent");
		p->SetInt(CONTENTS_TRANSLUCENT);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);

		p = new CObjProp();
		p->SetName("Ladder");
		p->SetDisplayName("Ladder");
		p->SetInt(CONTENTS_LADDER);
		p->SetType(CObjProp::integer);
		props[6].AddChoice(p);
	}

	props[7].SetName("value");
	props[7].SetDisplayName("value");
	props[7].SetInt(0);
	props[7].SetType(CObjProp::integer);
	props[7].SetSubType(FALSE);
	
	shift[0] = 0;
	shift[1] = 0;
	scale[0] = DEFAULT_TEXTURE_SCALE;
	scale[1] = DEFAULT_TEXTURE_SCALE;
	rot = 0;
	surface = 0;
	contents = 0;
	value = 0;

	p_shader = 0;
	verts = 0;
	texplane = -1;
	edges = 0;
}

CQBrush::BrushFace_t::~BrushFace_t()
{
}

int CQBrush::ExitBrushVertexMode( CMapObject* p, void* parm, void* parm2 )
{
	CQBrush* b = dynamic_cast<CQBrush*>(p);

	if( b )
	{
		b->FreeVertexManipulators( (CTreadDoc*)parm );
	}

	return 0;
}

int CQBrush::EnterBrushVertexMode( CMapObject* p, void* parm, void* parm2 )
{
	CQBrush* b = dynamic_cast<CQBrush*>(p);

	if( b )
	{
		b->MakeVertexManipulators( (CTreadDoc*)parm );
	}

	return 0;
}

void CQBrush::MakeVertexManipulators( CTreadDoc* pDoc )
{
	FreeVertexManipulators( pDoc );

	int i;
	
	//
	// make one for each vertex.
	//
	m_numvms = m_numxyz;
	m_vms = new CQBrushVertex_Manipulator*[m_numvms];
	
	unsigned int color = 0xFF00FF00;
	unsigned int hlcolor = 0xFF4DE6F2;
	float size = 5.0f;

	for(i = 0; i < m_numxyz; i++)
	{
		m_vms[i] = new CQBrushVertex_Manipulator();
		m_vms[i]->size = size;
		m_vms[i]->brush = this;
		m_vms[i]->color = color;
		m_vms[i]->hlcolor = hlcolor;
		m_vms[i]->pos = &m_xyz[i];
		m_vms[i]->SetViewFlags( VIEW_FLAG_MAP );
		pDoc->AddManipulatorToMap( m_vms[i] );
	}
}

void CQBrush::FreeVertexManipulators( CTreadDoc* pDoc )
{
	//pDoc->ClearAllTrackPicks();

	if( !m_numvms )
		return;

	int i;
	for(i = 0; i < m_numvms; i++)
	{
		pDoc->DetachManipulator( m_vms[i] );
		delete m_vms[i];
	}

	delete[] m_vms;
	m_vms = 0;
	m_numvms = 0;
}

void CQBrush::OnSetVisible( bool bVisible, CTreadDoc* pDoc )
{
	if( bVisible )
	{
		m_handle = new CQBrushHandle_Manipulator();
		m_handle->brush = this;
		pDoc->AddManipulatorToMap( m_handle );
	}
	else
	{
		FreeVertexManipulators( pDoc );

		if( m_handle )
		{
			pDoc->DetachManipulator( m_handle );
			delete m_handle;
			m_handle = 0;
		}
	}

	CMapObject::OnSetVisible( bVisible, pDoc );
}

void CQBrush::FlipObject( const vec3& origin, const vec3& axis )
{
	CMapObject::FlipObject( origin, axis );

	float d;
	float od;
	vec3 p;

	od = dot( origin, axis );

	int i;
	for(i = 0; i < m_numxyz; i++)
	{
		p = m_xyz[i];
		d = dot( p, axis ) - od;

		m_xyz[i] = (axis*(2*-d))+p;
	}

	//
	// flip faces...
	//
	BrushFace_t* f;
	int k;
	int* v;
	for(i = 0; i < m_numfaces; i++)
	{
		f = &m_faces[i];
		v = new int[f->num_verts];
		for(k = 0; k < f->num_verts; k++)
		{
			v[k] = f->verts[k].xyz;
		}
		for(k = 0; k < f->num_verts; k++)
		{
			f->verts[f->num_verts-k-1].xyz = v[k];
		}
		delete[] v;
	}

	MakePlanes();
	UpdateRenderMeshXYZ();

	//
	// texlock?
	//
	if( m_texlock == false )
	{
		TextureBrush(0);
		UpdateRenderMeshTexture();
	}

	GenTXSpaceVecs();
	BuildInfo();
}

void CQBrush::ScaleObject( const vec3& scale, const vec3& origin, const vec3& mins_before, const vec3& maxs_before, const vec3& mins_after, const vec3& maxs_after, CTreadDoc* pDoc )
{
	//
	// scale around our origin.
	//
	int i, k;
	vec3 p;

	for( i = 0; i < m_numxyz; i++ )
	{
		p = m_xyz[i];
		p -= origin;
		p = mul_vec(p, scale);
		
		p += origin;

		//
		// if we're close, then snap us completely.
		//
		for(k = 0; k < 3; k++)
		{

			if( fabsf(p[k]-mins_after[k]) < 1.5f )
				p[k] = mins_after[k];
			else
			if( fabsf(p[k]-maxs_after[k]) < 1.5f )
				p[k] = maxs_after[k];
		}

		/*p[0] = floorf(p[0]);
		p[1] = floorf(p[1]);
		p[2] = floorf(p[2]);*/

		m_xyz[i] = p;
	}

	OnVertexDrag();
}

bool CQBrush::OnPopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( !IsSelected() )
		return false;

	CQuakeUserData *ud = static_cast<CQuakeUserData*>(pView->GetDocument()->UserData());
	ud->MakeBrushMenu(pView->GetDocument());
	ud->m_BrushMenu.active_brush = this;
	CQBrush::BrushFace_t *f = dynamic_cast<CQBrush::BrushFace_t*>(pSrc);
	if (!f) f = &m_faces[0];
	ud->m_BrushMenu.brush_face = f;
	ud->m_BrushMenu.view = pView;
	
	Sys_DisplayObjectMenu( pView, nMX, nMY, &ud->m_BrushMenu );
	return true;
}

void CQBrush::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( m_bDrag )
	{
		Sys_EndDragSel( pView, nMX, nMY, nButtons );
		pView->GetDocument()->UpdateSelectionInterface();
	}
	else
	if( m_bTexDrag )
	{
		m_bTexDrag = false;
		if( m_bTexMoved )
		{
			ReleaseCapture();
			Sys_ShowCursor(TRUE);
		}
		
		if( !(nButtons&MS_CONTROL) && !m_bTexMoved )
		{
			CQBrush::BrushFace_t* f = dynamic_cast<CQBrush::BrushFace_t*>(pSrc);
			DeselectFaces();
			if( f )
			{
				SelectFace( f-m_faces, true );
			}

			pView->RedrawWindow();
		}
		else
		{
			CQBrush::BrushFace_t* f = dynamic_cast<CQBrush::BrushFace_t*>(pSrc);
			if( f && f->selected && !m_bTexMoved && (nButtons&MS_CONTROL) )
			{
				SelectFace( f-m_faces, false );
				pView->RedrawWindow();
			}
		}
		
		m_bTexMoved = false;
		pView->GetDocument()->Prop_UpdateSelection();
	}
}

void CQBrush::OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( m_bDrag )
		Sys_DragSel( pView, nMX, nMY, nButtons );
	else
	if( m_bTexDrag )
	{
		float dy;
		RECT r;
		BrushFaceChange_t bf;
		CString sVal;

		if( !m_bTexMoved )
		{
			if( m_nMY == nMY )
				return;

			m_bTexMoved = true;
			Sys_ShowCursor(FALSE);
			pView->SetCapture();
			pView->CenterCursor();
			return;
		}

		pView->GetClientRect( &r );

		dy = (r.bottom>>1)-nMY;
		
		if( dy == 0 )
			return;

		if( nButtons&MS_CONTROL )
		{
			bf.scale[1] = dy/100.0f;
			bf.scale[0] = dy/100.0f;
			sVal = "x scale";
		}
		else
		if( nButtons&MS_ALT )
		{
			//
			// rotate.
			//
			bf.rot = dy/4.0f;
			sVal = "rotation";
		}
		else
		{
			//
			// drag x, y shift.
			//
			if( nButtons&MS_SHIFT )
			{
				bf.shift[1] = dy;
				sVal = "y shift";
			}
			else
			{
				bf.shift[0] = dy;
				sVal = "x shift";
			}
		}

		if( !m_bMoved )
		{
			m_bMoved = true;
			pView->GetDocument()->GenericUndoRedoFromSelection()->SetTitle( "Manipulate Texture" );
		}

		pView->CenterCursor();
		pView->GetDocument()->GetSelectedObjectList()->WalkList( BrushFaceChange, &bf );
		pView->GetDocument()->Prop_PropChange( sVal );
		pView->GetDocument()->Prop_SelectProp( sVal );
		pView->RedrawWindow();
	}
}

void CQBrush::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	//
	// clear the selection?
	//
	bool redraw=false;
	m_bDrag = false;
	m_bTexDrag = false;

	if( pView->GetDocument()->IsEditingFaces() && pView->GetViewType() == VIEW_TYPE_3D )
	{
		m_nMY = nMY;

		if( !IsSelected() )
		{
			if( !(nButtons&MS_CONTROL) && !(nButtons&MS_SHIFT) )
			{
				pView->GetDocument()->MakeUndoDeselectAction();
				pView->GetDocument()->ClearSelection();
			}

			Select( pView->GetDocument(), pSrc );
			pView->GetDocument()->UpdateSelectionInterface();
			pView->GetDocument()->Prop_UpdateSelection();
		}
		
		//
		// now select the source face...
		//
		CQBrush::BrushFace_t* f = dynamic_cast<CQBrush::BrushFace_t*>(pSrc);
		if( f )
		{
			if( !(nButtons&MS_CONTROL) && !f->selected )
				DeselectFaces();
			else
			{
				if( f->selected )
				{
					m_bTexDrag = true;
					m_bTexMoved = false;
					m_bMoved = false;
				}
			}

			SelectFace( f-m_faces, true );
			
			pView->GetDocument()->Prop_UpdateSelection();
			Sys_RedrawWindows();
		}
	}
	else
	{
		if( !IsSelected() )
		{
			if( !(nButtons&MS_CONTROL) )
			{
				pView->GetDocument()->MakeUndoDeselectAction();
				pView->GetDocument()->ClearSelection();
			}

			Select( pView->GetDocument(), pSrc );
			pView->GetDocument()->Prop_UpdateSelection();
			redraw = true;

			pView->GetDocument()->UpdateSelectionInterface();
			
			if( pView->GetViewType() != VIEW_TYPE_3D )
			{
				m_bDrag = true;
				Sys_BeginDragSel( pView, nMX, nMY, nButtons );
			}
		}
		else
		{
			if( nButtons&MS_CONTROL )
			{
				pView->GetDocument()->MakeUndoDeselectAction();
				Deselect( pView->GetDocument() );
				pView->GetDocument()->Prop_UpdateSelection();
				pView->GetDocument()->UpdateSelectionInterface();
				redraw = true;
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
					redraw = true;
				}
			}
		}
	}

	if( redraw )
		Sys_RedrawWindows();
}

CLinkedList<CObjProp>* CQBrush::GetPropList( CTreadDoc* pDoc )
{
	if( pDoc->IsEditingFaces() == false )
	{
		CEntity* ent = GetOwner( pDoc );
		if( ent )
		{
			return ent->GetPropList( pDoc );
		}

		return 0;
	}

	m_PropList.ReleaseList();
	int i;

	for( i = 0; i < m_numfaces; i++)
	{
		if( !m_faces[i].selected )
			continue;
		
		m_PropList.AddItem(&m_faces[i].props[0]);
		m_PropList.AddItem(&m_faces[i].props[1]);
		m_PropList.AddItem(&m_faces[i].props[2]);
		m_PropList.AddItem(&m_faces[i].props[3]);
		m_PropList.AddItem(&m_faces[i].props[4]);
		if (m_type > TYPE_Q1)
		{
			m_PropList.AddItem(&m_faces[i].props[5]);
			m_PropList.AddItem(&m_faces[i].props[6]);
			m_PropList.AddItem(&m_faces[i].props[7]);
		}
	}

	return &m_PropList;
}

void CQBrush::PaintFacesWithContents()
{
	int i;
	
	for(i = 0; i < m_numfaces; i++)
	{
		PaintFaceWithContents( &m_faces[i] );
	}
}

void CQBrush::PaintFaceWithContents( CQBrush::BrushFace_t* f )
{
	CTreadDoc *doc = Sys_GetActiveDocument();

	if (GetType() == TYPE_Q2)
	{
		if( f->surface != 0 )
		{
			if( f->surface&SURF_HINT )
				f->p_shader = doc->ShaderForDisplayName( "hint" );
			else
			if( f->surface&SURF_SKIP )
				f->p_shader = doc->ShaderForDisplayName( "skip" );
			else
			if( f->surface&SURF_NODRAW )
				f->p_shader = doc->ShaderForDisplayName( "nodraw" );
			else
				goto paintwithbrush;
	
			return;
		}
	
	paintwithbrush:
	
		/*if( m_nContents&CONTENTS_AREAPORTAL )
			f->p_shader = doc->ShaderForName( "editor_areaportal" );
		else*/
		//
		// both clip?
		//
		if(f->contents&CONTENTS_PLAYERCLIP)
			f->p_shader = doc->ShaderForDisplayName( "clip" );
		else
		if(f->contents&CONTENTS_MONSTERCLIP)
			f->p_shader = doc->ShaderForDisplayName( "clip_mon" );
		else
			f->p_shader = doc->ShaderForName( f->s_shader );
	}
	else
	{
		f->p_shader = doc->ShaderForName( f->s_shader );
	}
}

void CQBrush::SetProp( CTreadDoc* pDoc, CObjProp* prop )
{
	const char* name = prop->GetName();

	if(pDoc->IsEditingFaces() == false)
	{
		return;
	}

	int idx = -1;
	

	if( !stricmp( name, "surface flags" ) )
	{
		//
		// face surface flags.
		//
		int i;
		BrushFace_t* f;

		for( i = 0; i < m_numfaces; i++ )
		{
			f = &m_faces[i];
			if( !f->selected )
				continue;
			
			f->surface = prop->GetInt();
			f->props[5].SetValue( prop );

			PaintFaceWithContents( f );
		}

		UpdateRenderMeshTexture();
		GenTXSpaceVecs();
		return;
	}

	if( !stricmp( name, "content flags" ) )
	{
		//
		// face content flags.
		//
		int i;
		BrushFace_t* f;

		for( i = 0; i < m_numfaces; i++ )
		{
			f = &m_faces[i];
			if( !f->selected )
				continue;
			
			f->contents = prop->GetInt();
			f->props[6].SetValue( prop );

			PaintFaceWithContents( f );
		}

		UpdateRenderMeshTexture();
		GenTXSpaceVecs();
		return;
	}
	
	if( !stricmp( name, "value" ) )
	{
		//
		// face value.
		//
		int i;
		BrushFace_t* f;

		for( i = 0; i < m_numfaces; i++ )
		{
			f = &m_faces[i];
			if( !f->selected )
				continue;
			
			f->value = prop->GetInt();
			f->props[7].SetValue( prop );
		}

		return;
	}

	if( !stricmp( name, "x shift" ) )
		idx = 0;
	else
	if( !stricmp( name, "y shift" ) )
		idx = 1;
	else
	if( !stricmp( name, "x scale" ) )
		idx = 2;
	else
	if( !stricmp( name, "y scale" ) )
		idx = 3;
	else
	if( !stricmp( name, "rotation" ) )
		idx = 4;

	if( idx == -1 )
		return;

	int i;
	float dx = 0, dy = 0;
	float v = prop->GetFloat();
	BrushFace_t* f;

	for( i = 0; i < m_numfaces; i++ )
	{
		f = &m_faces[i];
		if( !f->selected )
			continue;

		switch( idx )
		{
		case 0:

			dx = v - f->shift[0];
			f->shift[0] = v;

		break;
		case 1:

			dy = v - f->shift[1];
			f->shift[1] = v;

		break;
		case 2:

			f->scale[0] = v;
		
		break;
		case 3:

			f->scale[1] = v;

		break;

		case 4:

			f->rot = v;

		break;
		}

		f->props[idx].SetValue( prop );

		if( idx > 1 )
		{
			//
			// retexture.
			//
			TextureFaceFromPlane(f);
		}
		else
		{
			int k;

			for( k = 0; k < f->num_verts; k++)
			{
				f->verts[k].st[0] += dx;
				f->verts[k].st[1] += dy;
			}
		}
	}

	UpdateRenderMeshTexture();
	GenTXSpaceVecs();
}

void CQBrush::GenTXSpaceVecs()
{
	int i, pt, next, prev, ofs;
	BrushFace_t* f;
	vec3* xyz[3];
	vec2* st[3];
	vec3* normal;
	vec3  out[2];

	//
	// clear all the vecs.
	//
	for(i = 0; i < m_mesh.num_pts; i++)
	{
		m_mesh.normals[1][i] = vec3::zero;
		m_mesh.normals[2][i] = vec3::zero;
	}

	ofs = 0;
	for(i = 0; i < m_numfaces; i++)
	{
		f = &m_faces[i];

		for( pt = 0; pt < f->num_verts; pt++ )
		{
			next = pt+1;
			prev = pt-1;

			if( prev < 0 )
				prev = f->num_verts-1;
			if( next >= f->num_verts )
				next = 0;

			xyz[0] = &m_mesh.xyz[prev+ofs];
			st[0]  = &m_mesh.st[prev+ofs];
			xyz[1] = &m_mesh.xyz[pt+ofs];
			st[1]  = &m_mesh.st[pt+ofs];
			xyz[2] = &m_mesh.xyz[next+ofs];
			st[2]  = &m_mesh.st[next+ofs];

			normal = &m_mesh.normals[0][pt+ofs];

			R_GetTXSpaceVecs( xyz, normal, st, out );
			m_mesh.normals[1][pt+ofs] = out[0];
			m_mesh.normals[2][pt+ofs] = out[1];
		}

		ofs += f->num_verts;
	}
}

void CQBrush::MakeRenderMesh()
{
	int i, k;
	int numpts;
	CRenderMesh::poly_s* poly;
	BrushFace_t* f;

	numpts = 0;
	for(i = 0; i < m_numfaces; i++)
	{
		numpts += m_faces[i].num_verts;
	}

	m_mesh.AllocMesh( numpts, 0 );
	m_mesh.color2d = 0xFF400040;
	m_mesh.color3d = 0xFFFFFFFF;

	numpts = 0;
	for(i = 0; i < m_numfaces; i++)
	{
		f = &m_faces[i];

		poly = new CRenderMesh::poly_s();
		poly->num_pts = f->num_verts;
		poly->indices = new unsigned short[f->num_verts];
		poly->pick = f;
		poly->selcolor = 0xFFFFFFFF;
		poly->selected = f->selected;

		for(k = 0; k < f->num_verts; k++)
		{
			m_mesh.xyz[numpts+k] = m_xyz[f->verts[k].xyz];
			m_mesh.st[numpts+k] = f->verts[k].st;
			m_mesh.normals[0][numpts+k] = f->plane;
			m_mesh.rgba[numpts+k] = 0xFFFFFFFF;
			
			poly->shader = f->p_shader;
			poly->indices[k] = numpts+k;
		}

		numpts += f->num_verts;
		m_mesh.polys.AddItem( poly );
	}
}

void CQBrush::UpdateRenderMeshXYZ()
{
	int i, k, ofs;

	if( !m_mesh.xyz )
		return;

	ofs = 0;
	for(i = 0; i < m_numfaces; i++)
	{
		for(k = 0; k < m_faces[i].num_verts; k++)
		{
			m_mesh.xyz[ofs+k] = m_xyz[m_faces[i].verts[k].xyz];
			m_mesh.normals[0][ofs+k] = m_faces[i].plane;
		}

		ofs += m_faces[i].num_verts;
	}
}

void CQBrush::UpdateRenderMeshTexture()
{
	int i, k;
	int numpts;
	int w, h;
	BrushFace_t* f;

	numpts = 0;
	for( i = 0; i < m_numfaces; i++, numpts += f->num_verts)
	{
		f = &m_faces[i];

		if( f->p_shader == 0 )
		{
			m_mesh.polys.GetItem( i )->shader = 0;
			continue;
		}

		w = 128;
		h = 128;
		if (f->p_shader) f->p_shader->Dimensions(&w, &h);

		m_mesh.polys.GetItem( i )->shader = f->p_shader;

		for( k = 0; k < f->num_verts; k++ )
		{
			m_mesh.st[numpts+k][0] = f->verts[k].st[0] / ((float)w);
			m_mesh.st[numpts+k][1] = f->verts[k].st[1] / ((float)h);
		}
	}
}

int CQBrush::GetClass()
{
	return MAPOBJ_CLASS_BRUSH;
}

int CQBrush::GetSubClass()
{
	if( GetOwnerUID() != -1 )
		return MAPOBJ_SUBCLASS_OWNED;

	return MAPOBJ_SUBCLASS_NONE;
}

int CQBrush::GetNumRenderMeshes( CMapView* pView )
{
	return 1;
}

CRenderMesh* CQBrush::GetRenderMesh(int num, CMapView* pView)
{
	if( GetOwnerUID() != -1 )
	{
		m_mesh.color2d = 0xFF00FFFF;
		m_mesh.wireframe3d = 0xFF00FFFF;
	}
	else
	{
		m_mesh.color2d = 0xFF400040;
		m_mesh.wireframe3d = 0xFFDDDDDD;
	}

	return &m_mesh;
}

CMapObject* CQBrush::Clone()
{
	return new CQBrush(*this);
}

void CQBrush::CopyState( CMapObject* obj, CTreadDoc* pDoc )
{
	CMapObject::CopyState( obj, pDoc );

	CQBrush* b = dynamic_cast<CQBrush*>(obj);
	if( !b )
		return;

	//
	// copy the selected face state.
	//
	int i;
	for(i = 0; i < m_numfaces; i++)
	{
		SelectFace( i, b->m_faces[i].selected );
	}
}

void CQBrush::RepaintShader( const char* name, CTreadDoc* pDoc )
{
	for(int i = 0; i < m_numfaces; i++)
	{
		if( !name || m_faces[i].s_shader.CompareNoCase( name ) == 0 )
		{
			m_faces[i].p_shader = pDoc->ShaderForName( m_faces[i].s_shader );
			TextureFaceFromPlane(&m_faces[i] );
		}
	}

	UpdateRenderMeshTexture();
	GenTXSpaceVecs();
}

void CQBrush::SetShaderName( const char* name, CTreadDoc* pDoc )
{
	if( pDoc->IsEditingFaces() )
	{
		BrushFaceChange_t bf;

		bf.s_shader = name;
		pDoc->GetSelectedObjectList()->WalkList( BrushFaceChange, &bf );
	}
	else
	{
		CShader *shader = pDoc->ShaderForName( name );
		for(int i = 0; i < m_numfaces; i++)
		{
			m_faces[i].s_shader = name;
			m_faces[i].p_shader = shader;
		}

		if (GetType() == TYPE_Q2)
		{
			ApplyShaderContents(false);
			ApplyShaderSurface(false);
			ApplyShaderValue(false);
		}

		UpdateRenderMeshTexture();
		GenTXSpaceVecs();
	}
}

void CQBrush::InternalSetShaderName(const char *name, CShader *shader, CTreadDoc *pDoc)
{
	for(int i = 0; i < m_numfaces; i++)
	{
		m_faces[i].s_shader = name;
		m_faces[i].p_shader = shader;
	}

	UpdateRenderMeshTexture();
	GenTXSpaceVecs();
}

void CQBrush::SetObjectWorldPos( const vec3& pos, CTreadDoc* pDoc )
{
	//
	// translate the brush.
	//
	int i;
	vec3 t = pos-m_pos;

	for(i = 0; i < m_numxyz; i++)
	{
		m_xyz[i] += t;
	}
	
	OnVertexDrag();

	CMapObject::SetObjectWorldPos( pos, pDoc );
}

void CQBrush::SetObjectTransform( const mat3x3& m, CTreadDoc* pDoc )
{
	//
	// rotate...
	//
	int i;
	vec3 p;

	for(i = 0; i < m_numxyz; i++)
	{
		p = m_xyz[i] - m_pos;
		p *= m;
		m_xyz[i] = p + m_pos;
	}

	OnVertexDrag();

	CMapObject::SetObjectTransform( m, pDoc );
}

CQBrush::CQBrush(Type type) : CMapObject()
{
	m_type = type;
	m_pos = vec3::zero;
	m_mins = vec3::zero;
	m_maxs = vec3::zero;
	m_numfaces = 0;
	m_faces = 0;
	m_texlock = false;
	m_xyz = 0;
	m_numxyz = 0;

	m_handle = 0;
	m_numvms = 0;
	m_vms = 0;
	
	m_bDrag = false;
	m_bTexDrag = false;

	m_PropList.SetAutoDelete( false );
}

CQBrush::~CQBrush()
{
	if( m_xyz )
		delete[] m_xyz;
	if( m_faces )
	{
		int i;
		for(i = 0; i < m_numfaces; i++)
		{
			if(m_faces[i].verts)
				delete[] m_faces[i].verts;
			if(m_faces[i].edges )
				delete[] m_faces[i].edges;
		}

		delete[] m_faces;
	}

	m_mesh.FreeMesh();

	if( m_vms )
		delete[] m_vms;
}

CQBrush::CQBrush( const CQBrush& b ) : CMapObject( b )
{
	m_vms = 0;
	m_numvms = 0;
	m_handle = 0;
	m_type = b.m_type;

	m_bDrag = false;
	m_bTexDrag = false;

	m_pos = b.m_pos;
	m_mins = b.m_mins;
	m_maxs = b.m_maxs;
	m_numfaces = b.m_numfaces;
	if( m_numfaces > 0 )
	{
		m_faces = new BrushFace_t[m_numfaces];
		int i;
		for(i = 0; i < m_numfaces; i++)
		{
			m_faces[i] = b.m_faces[i];
		
			PaintFaceWithContents( &m_faces[i] );

			m_faces[i].verts = new BrushVert_t[b.m_faces[i].num_verts];
			m_faces[i].edges = new int[b.m_faces[i].num_verts];

			int k;
			for(k = 0; k < m_faces[i].num_verts; k++)
			{
				m_faces[i].verts[k] = b.m_faces[i].verts[k];
				m_faces[i].edges[k] = b.m_faces[i].edges[k];
			}
		}
	}
	else
	{
		m_faces = 0;
	}

	m_numxyz = b.m_numxyz;
	if( m_numxyz > 0 )
	{
		m_xyz = new vec3[m_numxyz];
		memcpy( m_xyz, b.m_xyz, sizeof(vec3)*m_numxyz );
	}

	m_texlock = b.m_texlock;

	m_PropList.SetAutoDelete( false );

	AttachFaces();
	MakeRenderMesh();
	UpdateRenderMeshTexture();
	GenTXSpaceVecs();
}

#define CONVEXITY_EPSILON				1.2f
#define DUPLICATE_NORMAL_EPSILON		0.05f
#define DUPLICATE_DIST_EPSILON			0.1f
#define BOGUS_RANGE						(1024*1024)
#define VERTEX_ON_PLANE_EPSILON			1.0f

bool CQBrush::CheckObject( CString& message, CTreadDoc* pDoc )
{
	//
	// check for convexity problems...
	//
	int i, k;
	float d;
	BrushFace_t* f, *f2;
	bool error;

	MakePlanes();

	error = false;

	for(i = 0; i < m_numfaces; i++)
	{
		f = &m_faces[i];
		
		for( k = 0; k < m_numxyz; k++ )
		{
			d = dot( f->plane, m_xyz[k] ) - f->plane.d;
			if( d > CONVEXITY_EPSILON )
			{
				error = true;
				message.Format("non convex brush! %f", d);
				goto endconvexitytest;
			}
		}

		//
		// check for duplicate planes.
		//
		for(k = 0; k < m_numfaces; k++)
		{
			if( k == i )
				continue;

			f2 = &m_faces[k];

			if (
			   fabs(f->plane.a - f2->plane.a) < DUPLICATE_NORMAL_EPSILON
			&& fabs(f->plane.b - f2->plane.b) < DUPLICATE_NORMAL_EPSILON
			&& fabs(f->plane.c - f2->plane.c) < DUPLICATE_NORMAL_EPSILON
			&& fabs(f->plane.d - f2->plane.d) < DUPLICATE_DIST_EPSILON )
			{
				error = true;
				message = "brush has duplicate planes!";
				goto endconvexitytest;
			}
		}
	}

endconvexitytest:

	if( error ) return error;

	//
	// check for duplicate points.
	//
	for(i = 0; i < m_numfaces; i++)
	{
		int j;

		f = &m_faces[i];

		for( k = 0; k < f->num_verts; k++ )
		{
			for( j = 0; j < f->num_verts; j++ )
			{
				if( j == k ) continue;

				if( equals( m_xyz[f->verts[j].xyz], m_xyz[f->verts[k].xyz], DUPLICATE_DIST_EPSILON ) )
				{
					message = "brush side has duplicate points!";
					error = true;
					goto endduplicatetest;
				}
			}
		}
	}

	
endduplicatetest:

	if( error ) return error;

	//
	// check for bounds problems
	//
	for(i = 0; i < m_numxyz; i++)
	{
		for(k = 0; k < 3; k++)
		{
			if( m_xyz[i][k] > BOGUS_RANGE || m_xyz[i][k] < -BOGUS_RANGE )
			{
				message = "brush is out of bounds!";
				error = true;
				goto endboundstest;
			}
		}
	}

endboundstest:

	//
	// make sure vertices lie on the face planes.
	//
	if( error ) return error;

	for(i = 0; i < m_numfaces; i++)
	{
		f = &m_faces[i];
		for( k = 0; k < f->num_verts; k++ )
		{
			d = dot( f->plane, m_xyz[f->verts[k].xyz] ) - f->plane.d;
			if( d > VERTEX_ON_PLANE_EPSILON || d < -VERTEX_ON_PLANE_EPSILON )
			{
				message = "brush has malformed face!";
				error = true;
				goto endvertextest;
			}
		}
	}

endvertextest:

	if( error ) return error;

	if (GetType() == TYPE_Q2)
	{
		for (i = 0; i < m_numfaces; ++i)
		{
			for (k = 0; k < m_numfaces; ++k)
			{
				if (i != k)
				{
					int a = m_faces[i].contents & (LAST_VISIBLE_CONTENTS-1);
					if (a == CONTENTS_SOLID) { a = 0; }
					int b = m_faces[k].contents & (LAST_VISIBLE_CONTENTS-1);
					if (b == CONTENTS_SOLID) { b = 0; }
					if (a != b)
					{
						message = "brush has mixed contents!";
						error = true;
						goto endcontentstest;
					}
				}
			}
		}
	}

endcontentstest:

	return error;
}

void CQBrush::CopyObject( CMapObject* obj, CTreadDoc* pDoc )
{
	CQBrush* b = dynamic_cast<CQBrush*>(obj);
	if( !b )
		return;

	FreeVertexManipulators( pDoc );

	if( m_xyz )
		delete[] m_xyz;
	if( m_faces )
	{
		int i;
		for(i = 0; i < m_numfaces; i++)
		{
			if(m_faces[i].verts)
				delete[] m_faces[i].verts;
			if(m_faces[i].edges )
				delete[] m_faces[i].edges;
		}

		delete[] m_faces;
	}

	m_mesh.FreeMesh();

	m_bDrag = false;
	m_bTexDrag = false;

	m_pos = b->m_pos;
	m_mins = b->m_mins;
	m_maxs = b->m_maxs;
	m_numfaces = b->m_numfaces;
	if( m_numfaces > 0 )
	{
		m_faces = new BrushFace_t[m_numfaces];
		int i;
		for(i = 0; i < m_numfaces; i++)
		{
			m_faces[i] = b->m_faces[i];
		
			PaintFaceWithContents( &m_faces[i] );

			m_faces[i].verts = new BrushVert_t[b->m_faces[i].num_verts];
			m_faces[i].edges = new int[b->m_faces[i].num_verts];

			int k;
			for(k = 0; k < m_faces[i].num_verts; k++)
			{
				m_faces[i].verts[k] = b->m_faces[i].verts[k];
				m_faces[i].edges[k] = b->m_faces[i].edges[k];
			}
		}
	}
	else
	{
		m_faces = 0;
	}

	m_numxyz = b->m_numxyz;
	if( m_numxyz > 0 )
	{
		m_xyz = new vec3[m_numxyz];
		memcpy( m_xyz, b->m_xyz, sizeof(vec3)*m_numxyz );
	}
	
	AttachFaces();
	MakeRenderMesh();
	UpdateRenderMeshTexture();
	GenTXSpaceVecs();
}

void CQBrush::AttachFaces()
{
	int i;

	for(i = 0; i < m_numfaces; i++)
		m_faces[i].SetParent( this );
}

void CQBrush::OnAddToSelection( CTreadDoc* pDoc )
{
	if( pDoc->IsEditingVerts() && !pDoc->IsInTrackAnimationMode() )
		MakeVertexManipulators( pDoc );
}

void CQBrush::OnAddToMap( CTreadDoc* pDoc )
{
	if( IsAttached() == false )
	{
		m_handle = 0;
		if( IsVisible() )
		{
			m_handle = new CQBrushHandle_Manipulator();
			m_handle->brush = this;
			pDoc->AddManipulatorToMap( m_handle );
		}
	}

	if( IsSelected() )
		FreeVertexManipulators( pDoc );

	DeselectFaces();

	CMapObject::OnAddToMap( pDoc );
}

void CQBrush::OnRemoveFromMap( CTreadDoc* pDoc )
{
	pDoc->DetachManipulator( m_handle );
	delete m_handle;
	m_handle = 0;

	FreeVertexManipulators( pDoc );

	CMapObject::OnRemoveFromMap( pDoc );
}

void CQBrush::GetWorldMinsMaxs( vec3* pMins, vec3* pMaxs )
{
	*pMins = m_mins;
	*pMaxs = m_maxs;
}

void CQBrush::GetObjectMinsMaxs( vec3* pMins, vec3* pMaxs )
{
	*pMins = m_mins-m_pos;
	*pMaxs = m_maxs-m_pos;
}

vec3 CQBrush::GetObjectWorldPos()
{
	return m_pos;
}

void CQBrush::BuildInfo()
{
	m_mins = vec3::bogus_max;
	m_maxs = vec3::bogus_min;

	int i;
	for(i = 0; i < m_numxyz; i++)
	{
		m_mins = vec_mins( m_mins, m_xyz[i] );
		m_maxs = vec_maxs( m_maxs, m_xyz[i] );
	}

	
	m_pos = (m_mins+m_maxs) / 2.0f;
}

//
// check to see if there is a collapsed face, and if we should weld the vertices and remove it...
//
void CQBrush::TryWeldVerts( CTreadDoc* pDoc, float dist, bool ask )
{
	static vec3 xyz[MAX_BRUSH_VERTS];
	int i, k, num_out;
	BrushFace_t* f;
	
	num_out = 0;
	for(i = 0; i < m_numfaces; i++)
	{
		f = &m_faces[i];
		for(k = 0; k < f->num_verts; k++)
		{
			HashBrushVert( xyz, &num_out, m_xyz[f->verts[k].xyz], dist );
		}
	}

	//
	// if the number out are different from the number in, then we have a weld..
	//
	if( num_out >= m_numxyz ) // how the fuck?
		return;

	if( (num_out < m_numxyz) && ask )
	{
		if( MessageBox( 0, "Would you like to weld the touching vertices and remove any collapsed faces?", "Weld Verts", MB_TASKMODAL|MB_DEFBUTTON1|MB_YESNO|MB_ICONQUESTION ) == IDNO )
			return;
	}

	//
	// hash the new vertices.
	//
	num_out = 0;
	for(i = 0; i < m_numfaces; i++)
	{
		f = &m_faces[i];
		for(k = 0; k < f->num_verts; k++)
		{
			f->verts[k].xyz = HashBrushVert( xyz, &num_out, m_xyz[f->verts[k].xyz], dist );
		}
	}

	delete[] m_xyz;
	
	m_xyz = new vec3[num_out];
	for(i = 0; i < num_out; i++)
		m_xyz[i] = xyz[i];

	m_numxyz = num_out;

	//
	// now, any faces that have collapsed vertex, create a new vertex order.
	//
	int numv, numf;
	int idx[256];
	vec2 st[256];

	numf = 0;
	for(i = 0; i < m_numfaces; i++)
	{
		numv = 0;
		f = &m_faces[i];
		for(k = 0; k < f->num_verts; k++)
		{
			if( f->verts[k].xyz != f->verts[(k+1)%f->num_verts].xyz )
			{
				//
				// output it.
				//
				st[numv] = f->verts[k].st;
				idx[numv] = f->verts[k].xyz;

				numv++;
			}
		}

		//
		// need to rework face?
		//
		if( numv > 2 ) // need to discard face.
		{
			numf++;
			if( numv < f->num_verts )
			{
				//
				// rebuild the verts...
				//
				delete[] f->verts;
				delete[] f->edges;

				f->verts = new BrushVert_t[numv];
				f->edges = new int[numv];

				for(k = 0; k < numv; k++)
				{
					f->verts[k].st  = st[k];
					f->verts[k].xyz = idx[k];
				}

				f->num_verts = numv;
			}
		}
		else
		{
			f->num_verts = 0; // note collapse.
		}
	}

	//
	// did the number of faces change?
	//
	BrushFace_t* faces = new BrushFace_t[numf];

	numf = 0;
	for(i = 0; i < m_numfaces; i++)
	{
		if( m_faces[i].num_verts > 0 )
			faces[numf++] = m_faces[i];
		else
		{
			delete[] m_faces[i].verts;
			delete[] m_faces[i].edges;
		}
	}

	delete[] m_faces;
	m_faces = faces;
	m_numfaces = numf;

	MakePlanes();
	TextureBrush(0);
	MakeRenderMesh();
	UpdateRenderMeshTexture();
	GenTXSpaceVecs();
	BuildInfo();
	FreeVertexManipulators( pDoc );
	if( pDoc->IsEditingVerts() )
	{
		MakeVertexManipulators( pDoc );
	}
}

void CQBrush::Carve(CQBrush *cutBrush, CLinkedList<CMapObject> *result, CTreadDoc *doc)
{
	CQBrush *front, *back;
	CQBrush *work = this;

	for (int i = 0; i < cutBrush->m_numfaces; ++i)
	{
		CQBrush::BrushFace_t *face = &cutBrush->m_faces[i];
		PlaneInfo plane;
		plane.plane = face->plane;
		plane.p_shader = face->p_shader;
		plane.s_shader = face->s_shader;
		plane.rot = face->rot;
		plane.shift[0] = face->shift[0];
		plane.shift[1] = face->shift[1];
		plane.scale[0] = face->scale[0];
		plane.scale[1] = face->scale[1];
		plane.surface =  face->surface;
		plane.contents = face->contents;
		plane.value = face->value;

		work->SplitBrushByPlane(plane, &front, &back);
		if (front)
		{
			result->AddItem(front);
		}
		if (back)
		{
			if (work != this)
			{
				delete work;
			}
			work = back;
		}
		else
		{
			break;
		}
	}

	if (work != this)
	{
		delete work;
	}
}

void CQBrush::GatherBrushes(CLinkedList<CMapObject> *cutBrushes, CLinkedList<CMapObject> *mapList, BrushList &list)
{
	for (CMapObject *obj = mapList->ResetPos(); obj; obj = mapList->GetNextItem())
	{
		CQBrush *b = dynamic_cast<CQBrush*>(obj);
		if (b)
		{
			// see if it touches any cut brushes.
			for (CMapObject *obj2 = cutBrushes->ResetPos(); obj2; obj2 = cutBrushes->GetNextItem())
			{
				CQBrush *cutBrush = dynamic_cast<CQBrush*>(obj2);
				OS_ASSERT(cutBrush);
				vec3 mins, maxs;
				cutBrush->GetWorldMinsMaxs(&mins, &maxs);
				if (b->TouchesBox(mins, maxs))
				{
					list.push_back(b);
				}
			}
		}
	}
}

void CQBrush::CarveBrushWithList(CLinkedList<CMapObject> *cutBrushes, CLinkedList<CMapObject> *result, CTreadDoc *doc)
{
	result->AddItem(new CQBrush(*this));

	for (CMapObject *obj = cutBrushes->ResetPos(); obj; obj = cutBrushes->GetNextItem())
	{
		CLinkedList<CMapObject> list;
		CQBrush *cutBrush = dynamic_cast<CQBrush*>(obj);
		OS_ASSERT(cutBrush);
		vec3 mins, maxs;
		cutBrush->GetWorldMinsMaxs(&mins, &maxs);
		
		while (CMapObject *src = result->RemoveItem(LL_HEAD))
		{
			CQBrush *srcBrush = dynamic_cast<CQBrush*>(src);
			OS_ASSERT(srcBrush);
			srcBrush->BuildInfo();
			if (srcBrush->TouchesBox(mins, maxs))
			{
				srcBrush->Carve(cutBrush, &list, doc);
				delete srcBrush;
			}
			else
			{
				list.AddItem(src);
			}
		}

		while (CMapObject *x = list.RemoveItem(LL_HEAD))
		{
			result->AddItem(x);
		}
	}

	for (CMapObject *x = result->ResetPos(); x; x = result->GetNextItem())
	{
		CQBrush *b = dynamic_cast<CQBrush*>(x);
		b->BuildInfo();
		b->MakePlanes();
		b->TextureBrush(0);
	}
}

void CQBrush::CarveSelected(CTreadDoc *doc)
{
	CLinkedList<CMapObject> *selected = doc->GetSelectedObjectList();
	BrushList touched;

	GatherBrushes(selected, doc->GetObjectList(), touched);
	if (touched.empty())
	{
		OS_OkAlertMessage("Error", "The selected brushe(s) do not touch anything, no carve will be performed.");
		return;
	}

	CLinkedList<CMapObject> cutList;
	for (int i = 0; i < touched.size(); ++i)
	{
		CQBrush *b = dynamic_cast<CQBrush*>(touched[i]);
		CLinkedList<CMapObject> result;
		b->CarveBrushWithList(selected, &result, doc);
		while (CMapObject *x = result.RemoveItem(LL_HEAD))
		{
			CQBrush *resultBrush = dynamic_cast<CQBrush*>(x);
			
			if (resultBrush->IsMicroBrush())
			{
				delete x;
			}
			else
			{	
				resultBrush->MakeRenderMesh();
				resultBrush->UpdateRenderMeshTexture();
				resultBrush->GenTXSpaceVecs();
				cutList.AddItem(x);
			}
		}
	}

	if (cutList.IsEmpty())
	{
		OS_OkAlertMessage("Error", "The selected brushe(s) do not touch anything, no carve will be performed.");
		return;
	}

	CLinkedList<CMapObject> srcList;
	for (int i = 0; i < touched.size(); ++i)
	{
		doc->DetachObject(touched[i]);
		doc->Prop_RemoveObject(touched[i]);
		srcList.AddItem(touched[i]);
	}

	for (CMapObject *x = cutList.ResetPos(); x; x = cutList.GetNextItem())
	{
		doc->AssignUID(x);
		x->SetName(doc->MakeUniqueObjectName(x));
	}

	{
		CObjectGroup *gr = new CObjectGroup();
		doc->AssignUID(gr);
		gr->MakeList(&cutList);
		gr->SetName(doc->MakeUniqueGroupName(gr));
		doc->Prop_AddGroup(gr);
		doc->GetObjectGroupList()->AddItem(gr);
		for (CMapObject *x = cutList.ResetPos(); x; x = cutList.GetNextItem())
		{
			x->SetGroupUID(gr->GetUID());
		}
	}

	CUndoRedoCarve *undo = new CUndoRedoCarve();
	undo->SetTitle("Carve");
	undo->InitUndo(selected, &srcList, &cutList, doc);

	while (CMapObject *x = cutList.RemoveItem(LL_HEAD))
	{
		doc->AddObjectToMap(x);
		doc->Prop_AddObject(x);
	}

	doc->RemoveNullGroups();
	doc->UpdateSelectionInterface();
	doc->Prop_UpdateSelection();
	doc->AddUndoItem(undo);
	Sys_RedrawWindows();
}

void CQBrush::HollowSelected(int depth, CTreadDoc *doc)
{
	CLinkedList<CMapObject> result;
	CLinkedList<CMapObject> final;
	CLinkedList<CMapObject> *selected = doc->GetSelectedObjectList();
	bool madeNewBrushes = false;
	for (CMapObject *obj = selected->ResetPos(); obj; obj = selected->GetNextItem())
	{
		CQBrush *b;
		CQBrush *orgBrush = dynamic_cast<CQBrush*>(obj);
		OS_ASSERT(orgBrush);
		bool shouldGroup = false;
		orgBrush->Hollow(depth, &result, doc);

		if (result.GetCount() > 1)
		{
			shouldGroup = true;
			madeNewBrushes = true;
		}

		for (b=dynamic_cast<CQBrush*>(result.ResetPos()); b; b = dynamic_cast<CQBrush*>(result.GetNextItem()))
		{
			if (shouldGroup)
			{
				doc->AssignUID(b);
				b->SetName(doc->MakeUniqueObjectName(b));
			}
			else
			{
				b->SetName(orgBrush->GetName());
				b->SetUID(orgBrush->GetUID());
			}
		}

		if (shouldGroup)
		{
			CObjectGroup *gr = new CObjectGroup();
			doc->AssignUID(gr);
			gr->MakeList(&result);
			gr->SetName(doc->MakeUniqueGroupName(gr));
			doc->GetObjectGroupList()->AddItem(gr);
			doc->Prop_AddGroup(gr);

			for (CMapObject *obj = result.ResetPos(); obj; obj = result.GetNextItem())
			{
				obj->SetGroupUID(gr->GetUID());
			}
		}

		// transfer to output.
		while (b=dynamic_cast<CQBrush*>(result.RemoveItem(LL_HEAD)))
		{
			final.AddItem(b);
		}
	}

	if (!madeNewBrushes) 
	{
		OS_OkAlertMessage("Warning", "The selected brushe(s) could not be hollowed with the specified depth.");
		return;
	}

	CUndoRedoHollow *undo = new CUndoRedoHollow();
	undo->SetTitle("Hollow");
	undo->InitUndo(selected, &final, doc);
	
	doc->DeleteSelection();
	while(CMapObject *b = final.RemoveItem(LL_HEAD))
	{
		doc->AddObjectToMap(b);
		doc->Prop_AddObject(b);
		doc->AddObjectToSelection(b);
	}

	doc->RemoveNullGroups();
	doc->UpdateSelectionInterface();
	doc->Prop_UpdateSelection();
	doc->AddUndoItem(undo);
	Sys_RedrawWindows();
}

void CQBrush::Hollow(int depth, CLinkedList<CMapObject> *result, CTreadDoc *doc)
{
	CQBrush *cutBrush = new CQBrush(*this);
	CQBrush *front, *back;

	std::vector<CQBrush*> out;

	for (int i = 0; i < m_numfaces; ++i)
	{
		CQBrush::BrushFace_t *face = &m_faces[i];
		PlaneInfo p;
		p.plane = face->plane;
		p.plane.d -= depth;
		p.s_shader = face->s_shader;
		p.p_shader = face->p_shader;
		p.rot = face->rot;
		p.scale[0] = face->scale[0];
		p.scale[1] = face->scale[1];
		p.shift[0] = face->shift[0];
		p.shift[1] = face->shift[1];
		p.surface = face->surface;
		p.contents = face->contents;
		p.value = face->value;
		cutBrush->SplitBrushByPlane(p, &front, &back);
		if (!back)
		{
			if (front) delete front;
			out.push_back(cutBrush);
			cutBrush = 0;
			break;
		}
		if (front)
		{
			out.push_back(front);
		}
		delete cutBrush;
		cutBrush = back;
	}

	if (cutBrush)
	{
		delete cutBrush;
	}

	for (int i = 0; i < out.size(); ++i)
	{
		CQBrush *b = out[i];
		b->TextureBrush(0);
		b->MakeRenderMesh();
		b->UpdateRenderMeshTexture();
		b->GenTXSpaceVecs();
		b->BuildInfo();
		b->MakePlanes();
		result->AddItem(b);
	}
}

bool CQBrush::SplitBrushByPlane(const PlaneInfo &plane, CQBrush **outFrontBrush, CQBrush **outBackBrush)
{
	*outFrontBrush = 0;
	*outBackBrush  = 0;

	struct winding
	{
		winding() : v(0), n(0), org(0) {}
		winding(const winding &w)
		{
			v = new vec3[w.n];
			n = w.n;
			memcpy(v, w.v, sizeof(vec3) * w.n);
			org = w.org;
		}
		~winding() { if (v) delete[] v; }
		vec3 *v;
		int   n;
		BrushFace_t *org;
	};

	typedef std::vector<winding> WindingList;

	WindingList front, back;

	{
		int maxVerts = 0;
		for (int i = 0; i < m_numfaces; ++i)
		{
			BrushFace_t *face = &m_faces[i];
			maxVerts = (face->num_verts > maxVerts) ? face->num_verts : maxVerts;
		}
		vec3 *verts = new vec3[maxVerts];
		for (int i = 0; i < m_numfaces; ++i)
		{
			BrushFace_t *face = &m_faces[i];

			// decompose face.
			for (int k = 0; k < face->num_verts; ++k)
			{
				verts[k] = m_xyz[face->verts[k].xyz];
			}

			winding f, b;
			SplitWinding(plane.plane, verts, face->num_verts, &f.v, &f.n, &b.v, &b.n, false);
			if (f.v && f.n)
			{
				f.org = face;
				front.push_back(f);
			}
			if (b.v && b.n)
			{
				b.org = face;
				back.push_back(b);
			}
		}
		delete [] verts;
	}

	if (!front.empty())
	{
		if (front.size()+1 >= 4)
		{
			PlaneInfo *planes = new PlaneInfo[front.size()+1];
			for (int i = 0; i < front.size(); ++i)
			{
				planes[i].plane = front[i].org->plane;
				planes[i].s_shader = front[i].org->s_shader;
				planes[i].p_shader = front[i].org->p_shader;
				planes[i].rot = front[i].org->rot;
				planes[i].shift[0] = front[i].org->shift[0];
				planes[i].shift[1] = front[i].org->shift[1];
				planes[i].scale[0] = front[i].org->scale[0];
				planes[i].scale[1] = front[i].org->scale[1];
				planes[i].contents = front[i].org->contents;
				planes[i].surface = front[i].org->surface;
				planes[i].value = front[i].org->value;
			}
			PlaneInfo &cap = planes[front.size()];
			cap = plane;
			cap.plane = -cap.plane;
			*outFrontBrush = BrushFromPlanes(m_type, planes, (int)front.size()+1);
			delete [] planes;
		}
		else
		{
			front.clear();
		}
	}

	if (!back.empty())
	{
		if (back.size()+1 >= 4)
		{
			PlaneInfo *planes = new PlaneInfo[back.size()+1];
			for (int i = 0; i < back.size(); ++i)
			{
				planes[i].plane = back[i].org->plane;
				planes[i].s_shader = back[i].org->s_shader;
				planes[i].p_shader = back[i].org->p_shader;
				planes[i].rot = back[i].org->rot;
				planes[i].shift[0] = back[i].org->shift[0];
				planes[i].shift[1] = back[i].org->shift[1];
				planes[i].scale[0] = back[i].org->scale[0];
				planes[i].scale[1] = back[i].org->scale[1];
				planes[i].contents = back[i].org->contents;
				planes[i].surface = back[i].org->surface;
				planes[i].value = back[i].org->value;
			}
			PlaneInfo &cap = planes[back.size()];
			cap = plane;
			*outBackBrush = BrushFromPlanes(m_type, planes, (int)back.size()+1);
			delete [] planes;
		}
		else
		{
			back.clear();
		}
	}

	return !front.empty() && !back.empty();
}

bool CQBrush::TouchesBox(const vec3 &mins, const vec3 &maxs)
{
	for (int i = 0; i < 3; ++i)
	{
		if (m_maxs[i] <= mins[i]) return false;
		if (m_mins[i] >= maxs[i]) return false;
	}
	return true;
}

bool CQBrush::IsMicroBrush() const
{
	for (int i = 0; i < 3; ++i)
	{
		if ((m_maxs[i]-m_mins[i]) <= PROGRAM_WELD_DIST) return true;
	}
	return false;
}

CQBrush* CQBrush::BrushFromPlanes(Type type, PlaneInfo *planes, int num_planes)
{
	int i, k, j, duplicates;
	int num_out;
	vec3 w[4];
	int num_outw;
	vec3* in;
	vec3* out;
	int num_cutplanes;
	
	static vec3* ow[MAX_BRUSH_PLANES];
	static int c[MAX_BRUSH_PLANES];
	static PlaneInfo cutplanes[MAX_BRUSH_PLANES];
	static PlaneInfo outplanes[MAX_BRUSH_PLANES];
	static bool discarded[MAX_BRUSH_PLANES];

	num_cutplanes = 0;

	memset(discarded, 0, sizeof(discarded));

	//
	// remove duplicate planes.
	//
	for(i = 0; i < num_planes; i++)
	{
		for(k = 0; k < num_planes; k++)
		{
			if( k == i )
				continue;

			if( !discarded[i] && !discarded[k] && equals( planes[i].plane, planes[k].plane, 0.000001f, 0.001f ) )
			{
				discarded[i] = true;
				break;
			}
		}

		if( k != num_planes )
			continue;

		cutplanes[num_cutplanes++] = planes[i];
	}

	OS_ASSERT(num_cutplanes >= 4);

	num_out = 0;
	num_outw = 0;

	//
	// create the brush.
	//
	for(i = 0; i < num_cutplanes; i++)
	{
		if( WindingFromPlane( cutplanes[i].plane, w ) )
		{
			//
			// cut it by all other planes.
			//
			in = w;
			num_out = 4;

			for(k = 0; k < num_cutplanes; k++)
			{
				if(k == i)
					continue;
		
				ChopWinding( cutplanes[k].plane, in, num_out, &out, &num_out, true );
				if( in != w )
					delete[] in;

				if( !out )
					break;

				in = out;
			}

			if( out )
			{
				c[num_outw] = num_out;
				outplanes[num_outw] = cutplanes[i];
				ow[num_outw++] = out;
			}
		}
	}

	if( num_outw < 1 )
		return 0;

	static vec3 xyz[MAX_BRUSH_VERTS];
	memset( xyz, 0, sizeof(xyz) );

	CQBrush* b = new CQBrush(type);
	BrushFace_t* bf;

	b->m_numfaces = num_outw;
	b->m_faces = new BrushFace_t[num_outw];
	b->m_numxyz = 0;

	for(i = 0; i < num_outw; i++)
	{
		bf = &b->m_faces[i];
		bf->num_verts = c[i];
		bf->verts = new BrushVert_t[c[i]];
		bf->edges = new int[c[i]];
		bf->plane = outplanes[i].plane;
		bf->s_shader = outplanes[i].s_shader;
		bf->p_shader = outplanes[i].p_shader;
		bf->rot = outplanes[i].rot;
		bf->surface = outplanes[i].surface;
		bf->contents = outplanes[i].contents;
		bf->value = outplanes[i].value;
		bf->shift[0] = outplanes[i].shift[0];
		bf->shift[1] = outplanes[i].shift[1];
		bf->scale[0] = outplanes[i].scale[0];
		bf->scale[1] = outplanes[i].scale[1];
		bf->props[0].SetFloat( bf->shift[0] );
		bf->props[1].SetFloat( bf->shift[1] );
		bf->props[2].SetFloat( bf->scale[0] );
		bf->props[3].SetFloat( bf->scale[1] );
		bf->props[4].SetFloat( bf->rot );
		bf->props[5].SetInt( bf->surface );
		bf->props[6].SetInt( bf->contents );
		bf->props[7].SetInt( bf->value );

		for(k = 0; k < c[i]; k++)
		{
			vec3 &v = ow[i][k];
			v.x = floor(v.x - 0.5f) + 1.0f;
			v.y = floor(v.y - 0.5f) + 1.0f;
			v.z = floor(v.z - 0.5f) + 1.0f;
			bf->verts[k].xyz = HashBrushVert( xyz, &b->m_numxyz, ow[i][k], PROGRAM_WELD_DIST );
		}

		//
		// do any equal?
		//
		duplicates = 0;
		for(k = 0; k < c[i]; k++)
		{
			for(j = 0; j < c[i]; j++)
			{
				if( j == k ) continue;

				if (bf->verts[k].xyz > -1 && bf->verts[j].xyz > -1)
				{
					if (bf->verts[k].xyz == bf->verts[j].xyz)
					{
						duplicates++;
						bf->verts[j].xyz = -1;
					}
				}
			}
		}

		if( duplicates > 0 )
		{
			if( (c[i]-duplicates) < 3 )
			{
				bf->num_verts = 0;
				delete [] bf->verts;
				bf->verts = 0;
				b->m_numfaces--;
				continue;
			}

			int ofs = 0;
			int newcount = c[i]-duplicates;
			BrushVert_t* verts = new BrushVert_t[newcount];
			int* edges = new int[newcount];
			
			for(k = 0; k < c[i]; k++)
			{
				if( bf->verts[k].xyz > -1 )
				{
					OS_ASSERT(ofs < newcount);
					verts[ofs].xyz = bf->verts[k].xyz;
					ofs++;
				}
			}

			delete[] bf->verts;
			delete[] bf->edges;

			bf->verts = verts;
			bf->edges = edges;
			bf->num_verts = newcount;
		}

		delete[] ow[i];
	}

	if( b->m_numfaces < 4 )
	{
		delete b;
		return 0;
	}

	if( b->m_numfaces != num_outw )
	{
		BrushFace_t* faces = new BrushFace_t[b->m_numfaces];

		k = 0;
		for(i = 0; i < num_outw; i++)
		{
			if( b->m_faces[i].num_verts > 0 )
			{
				faces[k++] = b->m_faces[i];
			}
		}

		delete[] b->m_faces;
		b->m_faces = faces;
	}

	b->m_xyz = new vec3[b->m_numxyz];
	memcpy( b->m_xyz, xyz, sizeof(vec3)*b->m_numxyz );

	b->AttachFaces();

	return b;
}

int CQBrush::HashBrushVert( vec3* xyz, int* num_verts, const vec3& hash_vert, float dist )
{
	int i;
	for( i = 0; i < *num_verts; i++)
	{
		if( equals( xyz[i], hash_vert, dist ) )
			return i;
	}

	if( *num_verts >= MAX_BRUSH_VERTS )
		return -1;

	xyz[*num_verts] = hash_vert;
	(*num_verts)++;
	return (*num_verts)-1;
}

void CQBrush::TextureBrush(const char* shader)
{
	int i;
	for(i = 0; i < m_numfaces; i++)
	{
		if( shader )
		{
			m_faces[i].s_shader = shader;
			m_faces[i].p_shader = Sys_GetActiveDocument()->ShaderForName( shader );
		}
		TextureFaceFromPlane(&m_faces[i]);
	}

	if (shader && GetType() == TYPE_Q2)
	{
		ApplyShaderContents(false);
		ApplyShaderSurface(false);
		ApplyShaderValue(false);
	}
}

bool CQBrush::WindingFromPlane( const plane3& pl, vec3* out )
{
	int i, x;
	float max, d;
	vec3 or, up, right;

	max = -999999.0f;
	x = -1;
	for(i = 0; i < 3; i++)
	{
		d = fabsf( pl[i] );
		if( d > max )
		{
			max = d;
			x = i;
		}
	}

	if( x == -1 )
		return false;

	up = vec3::zero;

	if( x == 0 || x == 1 )
		up[2] = 1.0f;
	else
		up[0] = 1.0f;

	d = dot( up, pl );
	up = scale_add( up, -d, pl );
	up.normalize();

	or = vec3(pl) * pl.d;
	right = cross( up, pl ) * 999999.0f;
	up *= 999999.0f;

	out[0] = or - right + up;
	out[1] = or + right + up;
	out[2] = or + right - up;
	out[3] = or - right - up;

	return true;
}

void CQBrush::SplitWinding( const plane3& plin, const vec3* in, int numin, vec3** outFront, int* numoutFront, vec3** outBack, int* numoutBack, bool fliptest )
{
	int i, j;
	int sides[MAX_WINDING_POINTS+4];
	int counts[3];
	int numFront, numBack;
	int maxpts;
	float d;
	float dists[MAX_WINDING_POINTS+4];
	const vec3* p1, *p2;
	vec3 *ptsFront, *ptsBack;
	plane3 pl;

	pl = plin;
	if( fliptest )
	{
		pl = -pl;
	}

	memset( counts, 0, sizeof(counts) );

	for(i = 0; i < numin; i++)
	{
		d = dot( in[i], pl ) - pl.d;

		if( d > 0.01f )
			sides[i] = FRONT;
		else
		if( d < -0.01f )
			sides[i] = BACK;
		else
			sides[i] = ON;

		dists[i] = d;
		counts[sides[i]]++;
	}

	if( !counts[FRONT] ) // no front points )
	{
		*outFront = 0;
		*numoutFront = 0;
		*outBack = new vec3[numin];
		memcpy( *outBack, in, sizeof(vec3)*numin );
		*numoutBack = numin;
		return;
	}
	if( !counts[BACK] )
	{
		*outBack = 0;
		*numoutBack = 0;
		*outFront = new vec3[numin];
		memcpy( *outFront, in, sizeof(vec3)*numin );
		*numoutFront = numin;
		return;
	}

	*numoutFront = 0;
	*numoutBack  = 0;
	maxpts = numin+4;
	ptsFront = new vec3[maxpts];
	numFront = 0;
	ptsBack = new vec3[maxpts];
	numBack = 0;

	for(i = 0; i < numin; i++)
	{
		if( sides[i] == BACK || sides[i] == FRONT || sides[i] == ON )
		{
			if (sides[i] == FRONT || sides[i] == ON)
			{
				*(ptsFront+numFront) = in[i];
				numFront++;
			}
			if (sides[i] == BACK || sides[i] == ON)
			{
				*(ptsBack+numBack) = in[i];
				numBack++;
			}
			if( sides[i] == ON )
				continue;
		}
		
		//
		// do we need to generate a split point?
		// 
		if( sides[(i+1)%numin] == ON || sides[(i+1)%numin] == sides[i] )
			continue;

		p1 = &in[i];
		p2 = &in[(i+1)%numin];

		d = dists[i] / ( dists[i] - dists[(i+1)%numin] );

		for(j = 0; j < 3; j++)
		{
			if( pl[j] == 1.0f )
			{
				(*(ptsFront+numFront))[j] = pl.d;
				(*(ptsBack+numBack))[j] = pl.d;
			}
			else
			if( pl[j] == -1.0f )
			{
				(*(ptsFront+numFront))[j] = -pl.d;
				(*(ptsBack+numBack))[j] = -pl.d;
			}
			else
			{
				(*(ptsFront+numFront))[j] = (*p1)[j] + d * ( (*p2)[j] - (*p1)[j] );
				(*(ptsBack+numBack))[j] = (*(ptsFront+numFront))[j];
			}
		}

		numFront++;
		numBack++;
	}

	//
	// copy to output.
	//
	*outFront = new vec3[numFront];
	memcpy( *outFront, ptsFront, sizeof(vec3)*numFront );
	delete[] ptsFront;
	*outBack = new vec3[numBack];
	memcpy( *outBack, ptsBack, sizeof(vec3)*numBack );
	delete[] ptsBack;

	*numoutFront = numFront;
	*numoutBack  = numBack;
}

void CQBrush::ChopWinding( const plane3& plin, const vec3* in, int numin, vec3** out, int* numout, bool fliptest )
{
	int i, j;
	int sides[MAX_WINDING_POINTS+4];
	int counts[3];
	int num;
	int maxpts;
	float d;
	float dists[MAX_WINDING_POINTS+4];
	const vec3* p1, *p2;
	vec3 *pts;
	plane3 pl;

	pl = plin;
	if( fliptest )
	{
		pl = -pl;
	}

	memset( counts, 0, sizeof(counts) );

	for(i = 0; i < numin; i++)
	{
		d = dot( in[i], pl ) - pl.d;

		if( d > 0.01f )
			sides[i] = FRONT;
		else
		if( d < -0.01f )
			sides[i] = BACK;
		else
			sides[i] = ON;

		dists[i] = d;
		counts[sides[i]]++;
	}

	if( !counts[FRONT] ) // no front points )
	{
		*out = 0;
		*numout = 0;
		return;
	}
	if( !counts[BACK] )
	{
		*out = new vec3[numin];
		memcpy( *out, in, sizeof(vec3)*numin );
		*numout = numin;
		return;
	}

	*numout = 0;
	maxpts = numin+4;
	pts = new vec3[maxpts];
	num = 0;

	for(i = 0; i < numin; i++)
	{
		if( sides[i] == FRONT || sides[i] == ON )
		{
			*(pts+num) = in[i];
			num++;
			if( sides[i] == ON )
				continue;
		}
		
		//
		// do we need to generate a split point?
		// 
		if( sides[(i+1)%numin] == ON || sides[(i+1)%numin] == sides[i] )
			continue;

		p1 = &in[i];
		p2 = &in[(i+1)%numin];

		d = dists[i] / ( dists[i] - dists[(i+1)%numin] );

		for(j = 0; j < 3; j++)
		{
			if( pl[j] == 1.0f )
				(*(pts+num))[j] = pl.d;
			else
			if( pl[j] == -1.0f )
				(*(pts+num))[j] = -pl.d;
			else
				(*(pts+num))[j] = (*p1)[j] + d * ( (*p2)[j] - (*p1)[j] );
		}

		num++;
	}

	//
	// copy to output.
	//
	*out = new vec3[num];
	memcpy( *out, pts, sizeof(vec3)*num );
	delete[] pts;

	*numout = num;
}

void CQBrush::TextureAxisFromPlane( const plane3& pl, vec3* x, vec3* y)
{
	int		nBestAxis;
	float	fDot,fBest;
	int		i;
	
	fBest = 0;
	nBestAxis = 0;
	
	for(i = 0; i < 6; i++)
	{
		fDot = dot( pl, vec3( vBaseAxis[i*3][0], vBaseAxis[i*3][1], vBaseAxis[i*3][2] ) );

		if (fDot > fBest)
		{
			fBest = fDot;
			nBestAxis = i;
		}
	}
	
	*x = vec3( vBaseAxis[nBestAxis*3+1][0], vBaseAxis[nBestAxis*3+1][1], vBaseAxis[nBestAxis*3+1][2] );
	*y = vec3( vBaseAxis[nBestAxis*3+2][0], vBaseAxis[nBestAxis*3+2][1], vBaseAxis[nBestAxis*3+2][2] );
}

void CQBrush::TextureFaceFromPlane(BrushFace_t* face)
{
	int i, j;
	int	sv, tv;
	float ns, nt;
	float ang, sinv, cosv;
	vec3 taxis[2];
	vec3 tvecs[2];

	if(face->texplane == -1)
		TextureAxisFromPlane(face->plane, &taxis[0], &taxis[1]);
	else
	{
		taxis[0] = vec3(vBaseAxis[face->texplane*3+1][0], vBaseAxis[face->texplane*3+1][1], vBaseAxis[face->texplane*3+1][2]);
		taxis[1] = vec3(vBaseAxis[face->texplane*3+2][0], vBaseAxis[face->texplane*3+2][1], vBaseAxis[face->texplane*3+2][2]);
	}

	if (face->scale[0] == 0.0F)
		face->scale[0] = 1.0F;
	if (face->scale[1] == 0.0F)
		face->scale[1] = 1.0F;

	// rotate axis
	if (face->rot == 0)
		{ sinv = 0 ; cosv = 1; }
	else if (face->rot == 90)
		{ sinv = 1 ; cosv = 0; }
	else if (face->rot == 180)
		{ sinv = 0 ; cosv = -1; }
	else if (face->rot == 270)
		{ sinv = -1 ; cosv = 0; }
	else
	{	
		ang = face->rot / 180 * PI;
		sinv = sinf(ang);
		cosv = cosf(ang);
	}

	if (taxis[0][0])
		sv = 0;
	else if (taxis[0][1])
		sv = 1;
	else
		sv = 2;
				
	if (taxis[1][0])
		tv = 0;
	else if (taxis[1][1])
		tv = 1;
	else
		tv = 2;
	
	//
	// rotate texture axis.
	//
	for (i=0 ; i<2 ; i++)
	{
		ns = cosv * taxis[i][sv] - sinv * taxis[i][tv];
		nt = sinv * taxis[i][sv] + cosv * taxis[i][tv];
		taxis[i][sv] = ns;
		taxis[i][tv] = nt;
	}
	
	for (i=0 ; i<2 ; i++)
		for (j=0 ; j<3 ; j++)
			tvecs[i][j] = taxis[i][j] / face->scale[i];

	//
	// output texture coordinates.
	//
	for(i = 0; i < face->num_verts; i++)
	{
		face->verts[i].st[0] = dot( m_xyz[face->verts[i].xyz], tvecs[0] ) + face->shift[0];
		face->verts[i].st[1] = dot( m_xyz[face->verts[i].xyz], tvecs[1] ) + face->shift[1];
	}
}
