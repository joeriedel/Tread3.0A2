///////////////////////////////////////////////////////////////////////////////
// SelScale_Manipulator.cpp
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
#include "selscale_manipulator.h"
#include "MapView.h"
#include "TreadDoc.h"
#include "r_sys.h"
#include "System.h"

CSelScale_Manipulator::CSelScale_Manipulator()
{
	m_bHover = false;
	m_bSizing = false;
	m_bRotate = false;
}

CSelScale_Manipulator::~CSelScale_Manipulator()
{
}

void CSelScale_Manipulator::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bMoved = false;

	/*if( nButtons&MS_CONTROL )
	{
		m_bRotate = true;
		pView->CenterCursor();
		Sys_ShowCursor(FALSE);
	}
	else*/
	{
		m_bSizing = true;
	}
	pView->SetCapture();
}

void CSelScale_Manipulator::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bSizing = false;
	m_bRotate = false;
	m_bHover = false;
	Sys_ShowCursor(TRUE);
	Sys_RedrawWindows();
	ReleaseCapture();
}

void CSelScale_Manipulator::OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( m_bRotate )
	{
		RECT r;

		pView->GetClientRect( &r );

		r.bottom >>= 1;
		if( nMY == r.bottom )
			return;

		float dy = r.bottom-nMY;

		if( !(nButtons&MS_SHIFT) )
		{
			//
			// lock to 15 degree increments.
			//
			float s = SIGN(dy);
			dy = fabs(dy);

			dy = floorf( dy / 15.0f ) * 15.0f * s;
			if( dy == 0.0f )
				return;
		}
		else
		{
			dy *= 0.25f;
		}

		float x, y, z;
		int z_axis = QUICK_AXIS( pView->View.or2d.frw );

		x = (z_axis == 0) ? 1 : 0;
		y = (z_axis == 1) ? 1 : 0;
		z = (z_axis == 2) ? 1 : 0;

		if( !m_bMoved )
		{
			m_bMoved = true;
			pView->GetDocument()->GenericUndoRedoFromSelection()->SetTitle( "Rotate" );
		}

		SysRotateInfo_t info;

		info.doc = pView->GetDocument();
		info.m = build_rotation_matrix( x, y, z, DEGREES_TO_RADIANS(dy) );
		info.org = pView->GetDocument()->m_selpos;

		pView->GetDocument()->GetSelectedObjectList()->WalkList( Sys_Rotate, &info );
		pView->GetDocument()->BuildSelectionBounds();
		Sys_RedrawWindows();
		pView->CenterCursor();
	}
	else
	if( m_bSizing )
	{
		int i;
		float ld;
		float ud;
		float dx, dy;
		float mapx, mapy;
		int minx, maxx;
		int miny, maxy;
		int x_axis, y_axis;
		vec3 pos;
		vec3 npos;
		vec3 scale;
		vec3 trans;
		SysScaleInfo_t sinf;
		CTreadDoc* pDoc = pView->GetDocument();

		pView->GetWinSelBox( &minx, &maxx, &miny, &maxy );

		ld = dot( vec, pView->View.or2d.lft );
		ud = dot( vec, pView->View.or2d.up );

		x_axis = y_axis = -1;
		if( ld != 0.0f )
			x_axis = QUICK_AXIS( pView->View.or2d.lft );
		if( ud != 0.0f )
			y_axis = QUICK_AXIS( pView->View.or2d.up );

		//
		// clamp the mouse to a valid rect.
		//
		if( ld > 0 )
		{
			nMX = MIN_VAL( nMX, maxx );
		}
		if( ld < 0 )
		{
			nMX = MAX_VAL( nMX, minx );
		}
		if( ud > 0 )
		{
			nMY = MIN_VAL( nMY, maxy );
		}
		if( ud < 0 )
		{
			nMY = MAX_VAL( nMY, miny );
		}

		pView->WinXYToMapXY( nMX, nMY, &mapx, &mapy );

		pos = vec3( (*x_in)[0], (*y_in)[1], (*z_in)[2] );			
		
		npos = vec3::zero;

		if( pView->GetGridSnap() )
		{
			mapx = Sys_Snapf( mapx, pView->GetGridSize() );
			mapy = Sys_Snapf( mapy, pView->GetGridSize() );
		}

		//
		// based on motion, did we increase or decrease size?
		//
		if( x_axis > -1 )
		{
			dx = (mapx-pos[x_axis]) * sign_vec(pView->View.or2d.lft);
			npos[x_axis] += SIGN( ld ) * dx;
		}
		if( y_axis > -1 )
		{
			dy = (mapy-pos[y_axis]) * sign_vec(pView->View.or2d.up);
			npos[y_axis] += SIGN( ud ) * dy;
		}
	
		if( equals( npos+pos, pos, 0.000001f ) == false )
		{
			vec3 csize, nsize;

			csize = pDoc->m_selmaxs-pDoc->m_selmins;
			nsize = csize+npos;

			for(i = 0; i < 3; i++)
			{
				if( csize[i] <= 0.0f )
					csize[i] = 1.0f;
				if( nsize[i] <= 0.0f )
					nsize[i] = 1.0f;

			}
			
			if( pView->GetGridSnap() )
			{
				if( x_axis > -1 )
				{
					if( nsize[x_axis] < pView->GetGridSize() )
					{
						nsize[x_axis] = pView->GetGridSize();
					}
				}

				if( y_axis > -1 )
				{
					if( nsize[y_axis] < pView->GetGridSize() )
					{
						nsize[y_axis] = pView->GetGridSize();
					}
				}
			}
		
			//
			// get the scale factor.
			//
			scale = nsize / csize;
			trans = (nsize-csize)/2.0f;

			//
			// alter the translation direction based on the side we're moving.
			//
			if( x_axis > -1 )
			{
				trans[x_axis] = fabsf(trans[x_axis]) * sign_vec( pView->View.or2d.lft ) * SIGN( dx );
			}
			if( y_axis > -1 )
			{
				trans[y_axis] = fabsf(trans[y_axis]) * sign_vec( pView->View.or2d.up ) * SIGN( dy );
			}

			sinf.doc = pDoc;
			sinf.scale = scale;
			sinf.mins_before = pDoc->m_selmins;
			sinf.maxs_before = pDoc->m_selmaxs;
			sinf.pos = pDoc->m_selpos;
			
			//
			// scale the bbox.
			//
			sinf.mins_after = pDoc->m_selpos + mul_vec((pDoc->m_selmins-pDoc->m_selpos), scale);
			sinf.maxs_after = pDoc->m_selpos + mul_vec((pDoc->m_selmaxs-pDoc->m_selpos), scale);

			if( !m_bMoved )
			{
				m_bMoved = true;
				pDoc->GenericUndoRedoFromSelection()->SetTitle( "Scale" );
			}

			pDoc->GetSelectedObjectList()->WalkList( Sys_Scale, &sinf );
			pDoc->GetSelectedObjectList()->WalkList( Sys_Translate, &trans, pDoc );
			pDoc->BuildSelectionBounds();

			Sys_RedrawWindows();
		}
	}
}

void CSelScale_Manipulator::OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = true;
	
	int c = TC_DEFAULT;

	float ld = dot( pView->View.or2d.lft, vec );
	float ud = dot( pView->View.or2d.up, vec );
		
	if( (ld > 0 && ud > 0) || (ld < 0 && ud < 0) )
		c = TC_IDC_SIZENWSE;
	else
	if( (ld > 0 && ud < 0) || (ld < 0 && ud > 0) )
		c = TC_IDC_SIZENESW;
	else
	if( ud > 0 || ud < 0 )
		c = TC_IDC_SIZENS;
	else
		c = TC_IDC_SIZEWE;
	
	pView->GetDocument()->AddManipulatorToMap(this);
	Sys_SetCursor( pView, c );
	pView->RedrawWindow();
}

void CSelScale_Manipulator::OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = false;
	pView->RedrawWindow();
}

void CSelScale_Manipulator::OnDraw( CMapView* pView )
{
	//
	// this sucker just draws as a square.
	//
	vec3 pos( (*x_in)[0], (*y_in)[1], (*z_in)[2] );
	vec3 mins, maxs;
	float temp_size;

	temp_size = size*0.5f;
	if( bConstantSize && pView->GetViewType() != VIEW_TYPE_3D )
	{
		temp_size *= pView->View.fInvScale;
	}

	mins = pos - temp_size;
	maxs = pos + temp_size;

	if( m_bHover )
		glColor4ubv( (GLubyte*)&hlcolor );
	else
		glColor4ubv( (GLubyte*)&color );

	R_DrawBox( mins, maxs );
}
