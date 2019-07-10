///////////////////////////////////////////////////////////////////////////////
// SelScale_Manipulator.h
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

#ifndef SELSCALE_MANIPULATOR
#define SELSCALE_MANIPULATOR

#include "system.h"

class CSelScale_Manipulator : public CManipulator
{
private:

	bool m_bHover;
	bool m_bSizing;
	bool m_bRotate;
	bool m_bMoved;

public:

	CSelScale_Manipulator();
	virtual ~CSelScale_Manipulator();

	bool bConstantSize;

	float size;
	vec3 vec;
	vec3* x_in;
	vec3* y_in;
	vec3* z_in;
	unsigned int color;
	unsigned int hlcolor;

	void OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnDraw( CMapView* pView );

};

#endif