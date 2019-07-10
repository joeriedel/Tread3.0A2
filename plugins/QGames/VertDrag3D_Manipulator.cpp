///////////////////////////////////////////////////////////////////////////////
// VertDrag3D_Manipulator.h
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
#include "TreadDoc.h"
#include "MapView.h"
#include "VertDrag3D_Manipulator.h"
#include "Brush.h"

///////////////////////////////////////////////////////////////////////////////
// CVertDrag3D_Manipulator
///////////////////////////////////////////////////////////////////////////////

CVertDrag3D_Manipulator::CVertDrag3D_Manipulator() : CVec3D_Manipulator()
{
}

CVertDrag3D_Manipulator::~CVertDrag3D_Manipulator()
{
}

void CVertDrag3D_Manipulator::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bMoved = false;
	CVec3D_Manipulator::OnMouseDown( pView, nMX, nMY, nButtons, pSrc );
}

void CVertDrag3D_Manipulator::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	CVec3D_Manipulator::OnMouseUp( pView, nMX, nMY, nButtons, pSrc );

	pView->GetDocument()->ClearAllTrackPicks();

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

bool CVertDrag3D_Manipulator::OnDrag( CMapView* pView, int nButtons, const vec3& move )
{
	vec3 t = move;

	if( pView->GetGridSnap() )
	{
		t = Sys_SnapVec3( move, pView->GetGridSize() );
		if( equals( t, vec3::zero, 0.00001f ) )
			return false;
	}

	if( !m_bMoved )
	{
		m_bMoved = true;
		pView->GetDocument()->GenericUndoRedoFromSelection()->SetTitle("Vertex Drag");
	}

	CManipulator* m;
	CQBrushVertex_Manipulator* bm;

	for( m = pView->GetDocument()->GetSelectedManipulatorList()->ResetPos(); m; m = pView->GetDocument()->GetSelectedManipulatorList()->GetNextItem() )
	{
		bm = dynamic_cast<CQBrushVertex_Manipulator*>(m);
		if( bm )
		{
			*(bm->pos) += t;
			bm->brush->OnVertexDrag();
		}
	}

	pView->GetDocument()->BuildSelectionBounds();
	Sys_RedrawWindows();
	return true;
}

void CVertDrag3D_Manipulator::OnDraw( CMapView* pView )
{
	if( pView->GetDocument()->GetSelectedManipulatorList()->IsEmpty() )
		return;

	CVec3D_Manipulator::OnDraw( pView );
}