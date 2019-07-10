///////////////////////////////////////////////////////////////////////////////
// SelScale3D_Manipulator.cpp
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
#include "selscale3d_manipulator.h"
#include "TreadDoc.h"
#include "MapView.h"

CSelScale3D_Manipulator::CSelScale3D_Manipulator() : CVec3D_Manipulator()
{
}

CSelScale3D_Manipulator::~CSelScale3D_Manipulator()
{
}

void CSelScale3D_Manipulator::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	CVec3D_Manipulator::OnMouseUp( pView, nMX, nMY, nButtons, pSrc );

	Sys_RedrawWindows(VIEW_TYPE_3D);
}

void CSelScale3D_Manipulator::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bMoved = false;
	CVec3D_Manipulator::OnMouseDown( pView, nMX, nMY, nButtons, pSrc );
}

bool CSelScale3D_Manipulator::OnDrag( CMapView* pView, int nButtons, const vec3& movein )
{
	CTreadDoc* pDoc = pView->GetDocument();
	vec3 move;

	if( pView->GetGridSnap() )
	{
		move = Sys_SnapVec3( movein, pView->GetGridSize() );
	}
	else
	{
		move = movein;
	}

	if( equals( move, vec3::zero, 0.00001f ) )
		return false;

	vec3 size = pDoc->m_selmaxs-pDoc->m_selmins;
	float scale = SIGN( dot(move, vec) ) * (vec_length(move) / vec_length( mul_vec(size, vec) ));
	vec3 s;
	SysScaleInfo_t inf;

	s[0] = 1.0f + fabsf(vec[0])*scale;
	s[1] = 1.0f + fabsf(vec[1])*scale;
	s[2] = 1.0f + fabsf(vec[2])*scale;

	inf.doc = pDoc;
	inf.scale = s;
	inf.pos = pDoc->m_selpos;
	inf.mins_before = pDoc->m_selmins;
	inf.maxs_before = pDoc->m_selmaxs;
	inf.mins_after = (pDoc->m_selmins-pDoc->m_selpos)*s + pDoc->m_selpos;
	inf.maxs_after = (pDoc->m_selmaxs-pDoc->m_selpos)*s + pDoc->m_selpos;

	//
	// if the resultant size on any axis is < then the grid size, then make it the grid size.
	//
	vec3 t = move / 2.0f;

	if( pView->GetGridSnap() )
	{
		int i;
		
		for(i = 0; i < 3; i++)
		{
			if( (inf.maxs_after[i]-inf.mins_after[i]) < pView->GetGridSize() && s[i] < 1.0f )
			{
				t[i] = 0.0f;
				inf.scale[i] = 1.0f;
				inf.maxs_after[i] = inf.maxs_before[i];
				inf.mins_after[i] = inf.mins_before[i];
			}
		}
	}

	if( !m_bMoved )
	{
		m_bMoved = true;
		pDoc->GenericUndoRedoFromSelection()->SetTitle( "Scale" );
	}

	pDoc->GetSelectedObjectList()->WalkList( Sys_Scale, &inf );
	pDoc->GetSelectedObjectList()->WalkList( Sys_Translate, &t, pDoc );

	pDoc->BuildSelectionBounds();
	Sys_RedrawWindows();

	return true;
}