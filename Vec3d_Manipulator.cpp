///////////////////////////////////////////////////////////////////////////////
// Vec3d_Manipulator.cpp
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
#include "vec3d_manipulator.h"
#include "MapView.h"
#include "TreadDoc.h"
#include "r_sys.h"
#include "System.h"

CVec3D_Manipulator::CVec3D_Manipulator()
{
	m_bHover = false;
	color = 0;
	hlcolor = 0;
	SetViewFlags( VIEW_TYPE_3D );
}

CVec3D_Manipulator::~CVec3D_Manipulator()
{
}

void CVec3D_Manipulator::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	Sys_ShowCursor(FALSE);
	pView->SetCapture();
	pView->CenterCursor();
}

void CVec3D_Manipulator::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = false;
	Sys_ShowCursor(TRUE);
	ReleaseCapture();
}

void CVec3D_Manipulator::OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	RECT r;
	pView->GetClientRect( &r );

	float dist = (r.bottom>>1)-nMY;
	if( dist == 0 )
		return;

	vec3 drag = vec * dist;
	if( OnDrag( pView, nButtons, drag ) )
	{
		pView->CenterCursor();
	}
}

void CVec3D_Manipulator::OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = true;
	pView->RedrawWindow();
}

void CVec3D_Manipulator::OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bHover = false;
	pView->RedrawWindow();
}

void CVec3D_Manipulator::OnDraw( CMapView* pView )
{
	if( m_bHover )
	{
		glColor4ubv( (GLubyte*)&hlcolor );
	}
	else
	{
		glColor4ubv( (GLubyte*)&color );
	}

	vec3 a, b, ra, rb;
	vec3 end;
	vec3 pos;
	vec3 ort;

	pos = vec3( (*x_in)[0], (*y_in)[1], (*z_in)[2] );
	end = pos + ( vec * size );

	ort = a = ortho_vec( vec );
	b = cross( a, vec );

	ra = -a;
	rb = -b;

	a  = (( a*(8.0f)) + end) - (vec * (24.0f));
	b  = (( b*(8.0f)) + end) - (vec * (24.0f));
	ra = ((ra*(8.0f)) + end) - (vec * (24.0f));
	rb = ((rb*(8.0f)) + end) - (vec * (24.0f));

	glLineWidth( 3.0f );
	glBegin(GL_LINES);
	
	glVertex3fv( pos );
	glVertex3fv( end );

	glEnd();
	glLineWidth( 1.0f );

	glBegin(GL_TRIANGLES);

	glVertex3fv( end );
	glVertex3fv( a );
	glVertex3fv( b );

	glVertex3fv( end );
	glVertex3fv( b );
	glVertex3fv( ra );

	glVertex3fv( end );
	glVertex3fv( ra );
	glVertex3fv( rb );

	glVertex3fv( end );
	glVertex3fv( rb );
	glVertex3fv( a );

	glEnd();

	if( m_bHover && string != "" )
	{
		//pos = pos + ( vec * ( size * 0.5f ) ) + (ort * 12.0f);
		glColor4ub( 0x00, 0xFF, 0x00, 0xFF );
		R_DrawStringXYZ( end + ( vec * 8.0f ), BOXSIZE_FONT, string );
	}
}