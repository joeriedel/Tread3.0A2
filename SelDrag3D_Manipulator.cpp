///////////////////////////////////////////////////////////////////////////////
// SelDrag3D_Manipulator.cpp
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
#include "seldrag3d_manipulator.h"
#include "TreadDoc.h"
#include "MapView.h"

CSelDrag3D_Manipulator::CSelDrag3D_Manipulator() : CVec3D_Manipulator()
{
	//SetViewFlags( VIEW_FLAG_MAP );
}

CSelDrag3D_Manipulator::~CSelDrag3D_Manipulator()
{
}

void CSelDrag3D_Manipulator::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	CVec3D_Manipulator::OnMouseUp( pView, nMX, nMY, nButtons, pSrc );

	pView->GetDocument()->UpdateSelectionInterface();

	Sys_RedrawWindows(VIEW_FLAG_MAP);
}

void CSelDrag3D_Manipulator::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	CVec3D_Manipulator::OnMouseDown( pView, nMX, nMY, nButtons, pSrc );

	m_bMoved = false;
	m_bRotate = (nButtons&MS_CONTROL)?true:false;
}

bool CSelDrag3D_Manipulator::OnDrag( CMapView* pView, int nButtons, const vec3& move )
{
	if( m_bRotate )
	{
		//
		// rotate the selection around this axis.
		//
		float len = vec_length(move);
		vec3 axis = normalized(move);
		
		if( !(nButtons&MS_SHIFT) )
		{
			len = floorf( len / 15.0f ) * 15.0f;			
			if( len == 0.0f )
				return false;
		}

		if( !m_bMoved )
		{
			m_bMoved = true;
			pView->GetDocument()->GenericUndoRedoFromSelection()->SetTitle( "Rotate" );
		}

		SysRotateInfo_t info;

		info.doc = pView->GetDocument();
		info.m = build_rotation_matrix( axis[0], axis[1], axis[2], DEGREES_TO_RADIANS(len) );
		info.org = pView->GetDocument()->m_selpos;

		pView->GetDocument()->GetSelectedObjectList()->WalkList( Sys_Rotate, &info );
		pView->GetDocument()->BuildSelectionBounds();
		Sys_RedrawWindows();
	}
	else
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
			if( nButtons&MS_SHIFT )
				pView->GetDocument()->CloneSelectionInPlace();
			else
				pView->GetDocument()->GenericUndoRedoFromSelection()->SetTitle( "Drag" );

			m_bMoved = true;
		}

		pView->GetDocument()->GetSelectedObjectList()->WalkList( Sys_Translate, &t, pView->GetDocument() );
		pView->GetDocument()->BuildSelectionBounds();
		Sys_RedrawWindows();
	}

	return true;
}

void CSelDrag3D_Manipulator::OnDraw( CMapView* pView )
{
	size = fabsf(dot( vec, (pView->GetDocument()->m_selpos-pView->GetDocument()->m_selmins) )) * 0.75f;
	if( size < 96 )
		size = 96.0f;

	CVec3D_Manipulator::OnDraw( pView );
}