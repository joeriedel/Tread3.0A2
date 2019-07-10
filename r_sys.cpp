///////////////////////////////////////////////////////////////////////////////
// r_sys.cpp
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
#include "r_sys.h"
#include "r_gl.h"
#include "system.h"
#include "TreadDoc.h"
#include "MapView.h"
#include "ChildFrm.h"
#include "ents.h"
#include "texcache.h"
#include "Mainfrm.h"
#include "files.h"
#include "lss.h"
#include "r_cubemap.h"

#define PICK_BUFF_SIZE		1024*1024
LongWord c_pickbuff[PICK_BUFF_SIZE];

LongWord r_frame = (LongWord)-1;

typedef void (*DRAWPROC) (CRenderMesh*, int, int, unsigned int);

static void R_RenderTextureList( CMapView* pView, int mousex, int mousey, CShader** pickshader );

static void* R_GetPickBuffObject( int num );
static void R_SortPickBuff( int num_hits );

static void R_DrawSelBox( CMapView* pView, RECT* r, unsigned int color );
static void R_DrawGrid( CMapView* pView );
static void R_Draw2D( CMapView* pView );
static void R_DrawTypeString( CMapView* pView );
static void R_Draw3D( CMapView* pView );
static void MYgluPerspective( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar );
static void R_SetupProjection( CMapView* pView );
static void R_DrawForLightZ( CRenderMesh* mesh, int flags, int blend, unsigned int color );
static void R_DrawTextured( CRenderMesh* mesh, int flags, int blend, unsigned int color );
static void R_DrawWireframe( CRenderMesh* mesh, int flags, int blend, unsigned int color );
static void R_DrawMesh2D( CRenderMesh* mesh, int flags, int blend, unsigned int color );
static void R_DrawSolid( CRenderMesh* mesh, int flags, int blend, unsigned int color );
static void R_DrawPick3D( CRenderMesh* mesh, int flags, int blend, unsigned int color );
static void R_DrawPick2D( CRenderMesh* mesh, int flags, int blend, unsigned int color );
static void R_RotateForViewer( const vec3& pos, const COrientation& or );
static void R_RotateForObject( const vec3& pos, const COrientation& or );
static inline void R_OutputMeshTris( CRenderMesh* mesh );
static inline void R_OutputMeshPoly( CRenderMesh::poly_s* poly, int mode = GL_POLYGON );
static void R_RenderObjectList( CMapView* pView, CLinkedList<CMapObject>* list, DRAWPROC proc, int flags, int blends, unsigned int color );
static void R_RenderManipulatorList( CMapView* pView, CLinkedList<CManipulator>* list, bool pick );
static void R_SetupPickProjection( CMapView* pView, float x, float y, float w, float h );
static int R_Pick3D( CMapView* pView, int x, int y, int w, int h, int flags );
static int R_Pick2D( CMapView* pView, int x, int y, int w, int h, int flags );
static void R_RotateForOrthoView( CMapView* pView );
static void R_DrawSelBox( const vec3& mins, const vec3& maxs, vec3 size, unsigned int color );
static void R_DrawEntityName( CMapView* pView, CEntity* entity );
static void R_DrawEntityNames( CMapView* pView, CLinkedList<CMapObject>* list );
static void R_DrawLeakTrace( CMapView* pView, vec3* xyz, int num_xyz, unsigned int color );

void InverseMatrixFromAngles( vec3_t angles, Matrix3D_t* m )
{
	//
	// need more precision
	//
	vec_t yaw, pitch, roll;
	vec_t sx, sy, sz;
	vec_t cx, cy, cz;
	vec_t temp1, temp2;
	
	yaw = pitch = roll = 0;
	
	yaw = DEGREES_TO_RADIANS( -angles[1] );
	pitch = DEGREES_TO_RADIANS( angles[0] );
	roll = DEGREES_TO_RADIANS( angles[2] );
		
	sx = sinf( pitch );	
	cx = sinf( pitch+PI/2 );
	sy = sinf( yaw );
	cy = sinf( yaw+PI/2 );
	sz = sinf( roll );
	cz = sinf( roll+PI/2 );
	
	temp1 = sx*sz;
	temp2 = sx*cz;
	
	m->x.x = ( cy*cz ) + ( temp1*sy );
	m->x.y = ( cy*sz ) - ( temp2*sy );
	m->x.z = ( cx*sy );
	
	m->y.x = -cx*sz;
	m->y.y = cx * cz;
	m->y.z = sx;
	
	m->z.x = ( -sy * cz ) + ( temp1 * cy );
	m->z.y = ( -sz * sy ) - ( temp2 * cy );
	m->z.z = cx * cy;
}

OS_FNEXP void R_GetTXSpaceVecs( vec3* xyz[3], vec3* normal, vec2* st[3], vec3* out )
{
	int indx[4];
	float m[4][4];
	float ldout[2][4];

	((LongWord*)&m[0][0])[0] = ((LongWord*)&(xyz[0]->x))[0];
	((LongWord*)&m[0][1])[0] = ((LongWord*)&(xyz[0]->y))[0];
	((LongWord*)&m[0][2])[0] = ((LongWord*)&(xyz[0]->z))[0];
	m[0][3] = -1.0f;
	((LongWord*)&m[1][0])[0] = ((LongWord*)&(xyz[1]->x))[0];
	((LongWord*)&m[1][1])[0] = ((LongWord*)&(xyz[1]->y))[0];
	((LongWord*)&m[1][2])[0] = ((LongWord*)&(xyz[1]->z))[0];
	m[1][3] = -1.0f;
	((LongWord*)&m[2][0])[0] = ((LongWord*)&(xyz[2]->x))[0];
	((LongWord*)&m[2][1])[0] = ((LongWord*)&(xyz[2]->y))[0];
	((LongWord*)&m[2][2])[0] = ((LongWord*)&(xyz[2]->z))[0];
	m[2][3] = -1.0f;
	((LongWord*)&m[3][0])[0] = ((LongWord*)&(normal->x))[0];
	((LongWord*)&m[3][1])[0] = ((LongWord*)&(normal->y))[0];
	((LongWord*)&m[3][2])[0] = ((LongWord*)&(normal->z))[0];
	m[3][3] = 0.0f;

	if( ludcmp4( &m[0][0], indx ) )
	{
		ldout[0][0] = st[0]->x;
		ldout[0][1] = st[1]->x;
		ldout[0][2] = st[2]->x;
		ldout[0][3] = 0.0f;
		
		lubksub4( &m[0][0], &indx[0], &ldout[0][0] );

		ldout[1][0] = st[0]->y;
		ldout[1][1] = st[1]->y;
		ldout[1][2] = st[2]->y;
		ldout[1][3] = 0.0f;
		
		lubksub4( &m[0][0], &indx[0], &ldout[1][0] );

		out[0].x = -ldout[0][0];
		out[0].y = -ldout[0][1];
		out[0].z = -ldout[0][2];
		out[1].x = -ldout[1][0];
		out[1].y = -ldout[1][1];
		out[1].z = -ldout[1][2];

		out[0].normalize();
		out[1].normalize();
	}
	else
	{
		//
		// bad tangent planes
		//
		out[0] = vec3::zero;
		out[1] = vec3::zero;

		out[0].x = out[1].y = 1.0f;
	}
}

////////////////////////////////////////////////////////////////////////////
// NORMAL Drawing
////////////////////////////////////////////////////////////////////////////

static void R_DrawEntityName( CMapView* pView, CEntity* entity )
{
	glColor4f( 1.0f, 1.0f, 0.2f, 1.0f );
	const float ofs = 16.0f;

	if( pView->GetViewType() == VIEW_TYPE_3D )
	{
		vec3 mn, mx, pos, c;
		const char* s = entity->GetDisplayName();
		int len = strlen( s );
		
		entity->GetWorldMinsMaxs( &mn, &mx );
		c =   (mx - mn) / 2.0f;
		pos = (mx + mn) / 2.0f;

		pos -= pView->View.or3d.lft * (c + vec3(ofs, ofs, ofs));
		pos += pView->View.or3d.up  * (c + vec3(ofs, ofs, ofs));

		R_DrawStringXYZ( pos, BOXSIZE_FONT, s );
	}
	else
	{
		vec3 mn, mx, pos, c;
		const char* s = entity->GetDisplayName();
		int len = strlen( s );
		
		entity->GetWorldMinsMaxs( &mn, &mx );
		c =   (mx - mn) / 2.0f;
		pos = (mx + mn) / 2.0f;

		pos -= pView->View.or2d.lft * (c + vec3(ofs, ofs, ofs) * pView->View.fInvScale);
		pos += pView->View.or2d.up  * (c + vec3(ofs, ofs, ofs) * pView->View.fInvScale);

		R_DrawStringXYZ( pos, BOXSIZE_FONT, s );
	}
}

static void R_DrawEntityNames( CMapView* pView, CLinkedList<CMapObject>* list )
{
	CMapObject* o;

	for( o = list->ResetPos(); o; o = list->GetNextItem() )
	{
		if( !o->IsVisible() )
			continue;
		if( o->GetClass() != MAPOBJ_CLASS_ENTITY )
			continue;

		CEntity* ent = dynamic_cast<CEntity*>(o);
		if( ent )
			R_DrawEntityName( pView, ent );
	}
}

static void R_DrawBoxSize( CMapView* pView, const vec3& mins, const vec3& maxs, const vec3& center )
{
	char wmsg[256];
	char hmsg[256];
	char dmsg[256];
	vec3 pos;
	vec3 size = maxs-mins;
	vec3 up = pView->View.or2d.up;
	vec3 lf = pView->View.or2d.lft;
	float width  = fabsf(dot( lf, size ));
	float height = fabsf(dot( up, size ));
	float wlen, hlen;
	float fh;

	if( pView->GetViewType() == VIEW_TYPE_3D )
	{
		return ;

		sprintf( wmsg, "%.2f - x", size[0] );
		sprintf( hmsg, "%.2f - y", size[1] );
		sprintf( dmsg, "%.2f - z", size[2] );

		R_DrawStringXYZ( center-(sysAxisX*(size[0]/2.0f))+(sysAxisX*16.0f), BOXSIZE_FONT, wmsg );
		R_DrawStringXYZ( center-(sysAxisY*(size[1]/2.0f))+(sysAxisY*16.0f), BOXSIZE_FONT, hmsg );
		R_DrawStringXYZ( center-(sysAxisZ*(size[2]/2.0f))+(sysAxisZ*16.0f), BOXSIZE_FONT, dmsg );
	}
	else
	{
		sprintf( wmsg, "%.2f", width );
		sprintf( hmsg, "%.2f", height );

		wlen = strlen( wmsg )*8*pView->View.fInvScale;
		hlen = strlen( hmsg )*8*pView->View.fInvScale;
		fh = 6*pView->View.fInvScale;
		
		R_DrawStringXYZ( center+(up*(height/2.0f+(14.0f*pView->View.fInvScale)))+(lf*(wlen/2.0f)), BOXSIZE_FONT, wmsg );
		R_DrawStringXYZ( center-(up*(height/2.0f+(18.0f*pView->View.fInvScale)))+(lf*(wlen/2.0f)), BOXSIZE_FONT, wmsg );
		R_DrawStringXYZ( center+(lf*(width/2.0f+(2.0f*pView->View.fInvScale)))+(lf*(hlen))-(up*(fh/2.0f)), BOXSIZE_FONT, hmsg );
		R_DrawStringXYZ( center-(lf*(width/2.0f+(12.0f*pView->View.fInvScale)))-(up*(fh/2.0f)), BOXSIZE_FONT, hmsg );
	}
}

static void R_DrawLeakTrace( CMapView* pView, vec3* xyz, int num_xyz, unsigned int color )
{
	glColor4ubv( (GLubyte*)&color );

	int i;
	glBegin(GL_LINES);
	for(i = 0; i < num_xyz-1; i++)
	{
		glVertex3fv( &xyz[i].x );
		glVertex3fv( &xyz[i+1].x );
	}
	glEnd();
}

static void R_DrawSelBox( const vec3& mins, const vec3& maxs, vec3 size, unsigned int color )
{
	vec3 x, y, z, pt;

	x = vec3( size[0], 0, 0 );
	y = vec3( 0, size[1], 0 );
	z = vec3( 0, 0, size[2] );

	glColor4ubv( (GLubyte*)&color );

	glBegin(GL_LINES);
	
		glVertex3fv( mins );
		glVertex3fv( mins+x );
		glVertex3fv( mins );
		glVertex3fv( mins+y );
		glVertex3fv( mins );
		glVertex3fv( mins+z );

		glVertex3fv( maxs );
		glVertex3fv( maxs-x );
		glVertex3fv( maxs );
		glVertex3fv( maxs-y );
		glVertex3fv( maxs );
		glVertex3fv( maxs-z );

		pt = vec3( mins[0], maxs[1], mins[2] );

		glVertex3fv( pt );
		glVertex3fv( pt + x );
		glVertex3fv( pt );
		glVertex3fv( pt - y );
		glVertex3fv( pt );
		glVertex3fv( pt + z );

		pt = vec3( maxs[0], maxs[1], mins[2] );

		glVertex3fv( pt );
		glVertex3fv( pt - x );
		glVertex3fv( pt );
		glVertex3fv( pt - y );
		glVertex3fv( pt );
		glVertex3fv( pt + z );

		pt = vec3( maxs[0], mins[1], mins[2] );

		glVertex3fv( pt );
		glVertex3fv( pt - x );
		glVertex3fv( pt );
		glVertex3fv( pt + y );
		glVertex3fv( pt );
		glVertex3fv( pt + z );

		pt = vec3( mins[0], mins[1], maxs[2] );

		glVertex3fv( pt );
		glVertex3fv( pt + x );
		glVertex3fv( pt );
		glVertex3fv( pt + y );
		glVertex3fv( pt );
		glVertex3fv( pt - z );

		pt = vec3( mins[0], maxs[1], maxs[2] );

		glVertex3fv( pt );
		glVertex3fv( pt + x );
		glVertex3fv( pt );
		glVertex3fv( pt - y );
		glVertex3fv( pt );
		glVertex3fv( pt - z );

		pt = vec3( maxs[0], mins[1], maxs[2] );

		glVertex3fv( pt );
		glVertex3fv( pt - x );
		glVertex3fv( pt );
		glVertex3fv( pt + y );
		glVertex3fv( pt );
		glVertex3fv( pt - z );

	glEnd();
}

static void R_RotateForOrthoView( CMapView* pView )
{
	switch( pView->GetViewType() )
	{
	case VIEW_TYPE_FRONT:
	case VIEW_TYPE_BACK:

		glRotatef( -90, 1, 0, 0 );

	break;

	case VIEW_TYPE_LEFT:
	case VIEW_TYPE_RIGHT:

		glRotatef( -90, 0, 0, 1 );
		glRotatef( -90, 0, 1, 0 );

	break;
	}
}

void R_ExtractBBoxPoints( const vec3& mins, const vec3& maxs, vec3* outpts )
{
	outpts[0][0] = mins[0];
	outpts[0][1] = mins[1];
	outpts[0][2] = maxs[2];
	
	outpts[1][0] = mins[0];
	outpts[1][1] = maxs[1];
	outpts[1][2] = maxs[2];
	
	outpts[2][0] = maxs[0];
	outpts[2][1] = maxs[1];
	outpts[2][2] = maxs[2];
	
	outpts[3][0] = maxs[0];
	outpts[3][1] = mins[1];
	outpts[3][2] = maxs[2];
	
	outpts[4][0] = mins[0];
	outpts[4][1] = mins[1];
	outpts[4][2] = mins[2];

	outpts[5][0] = mins[0];
	outpts[5][1] = maxs[1];
	outpts[5][2] = mins[2];
		
	outpts[6][0] = maxs[0];
	outpts[6][1] = maxs[1];
	outpts[6][2] = mins[2];
	
	outpts[7][0] = maxs[0];
	outpts[7][1] = mins[1];
	outpts[7][2] = mins[2];
}

void R_MakeBoxMeshCmds( CRenderMesh* mesh, const vec3& mins, const vec3& maxs, bool solid, bool double_sided )
{
	mesh->FreeMesh();

	mesh->shader = 0;
	mesh->texture = 0;

	mesh->xyz = new vec3[8];
	mesh->normals[0] = new vec3[8];
	mesh->num_pts = 8;

	R_ExtractBBoxPoints( mins, maxs, mesh->xyz );

	int i;
	for(i = 0; i < 8; i++)
		mesh->normals[0][i] = normalized(sysAxisX+sysAxisY);

	if( solid )
	{
		int mult = (double_sided) ? 2 : 1;
		mesh->num_tris = 12*mult;
		mesh->cmds = GL_TRIANGLES;
		mesh->tris = new unsigned short[6*6*mult];

		unsigned short* tris = mesh->tris;

		//
		// top
		//
		*(tris++) = 0;
		*(tris++) = 1;
		*(tris++) = 2;
		*(tris++) = 2;
		*(tris++) = 3;
		*(tris++) = 0;

		//
		// bottom.
		//
		*(tris++) = 6;
		*(tris++) = 5;
		*(tris++) = 4;
		*(tris++) = 4;
		*(tris++) = 7;
		*(tris++) = 6;

		//
		// left
		//
		*(tris++) = 5;
		*(tris++) = 1;
		*(tris++) = 0;
		*(tris++) = 0;
		*(tris++) = 4;
		*(tris++) = 5;

		//
		// right.
		//
		*(tris++) = 3;
		*(tris++) = 2;
		*(tris++) = 6;
		*(tris++) = 6;
		*(tris++) = 7;
		*(tris++) = 3;

		//
		// front.
		//
		*(tris++) = 0;
		*(tris++) = 3;
		*(tris++) = 7;
		*(tris++) = 7;
		*(tris++) = 4;
		*(tris++) = 0;

		//
		// back.
		//
		*(tris++) = 6;
		*(tris++) = 2;
		*(tris++) = 1;
		*(tris++) = 1;
		*(tris++) = 5;
		*(tris++) = 6;

		if( double_sided )
		{
			//
			// top
			//
			*(tris++) = 2;
			*(tris++) = 1;
			*(tris++) = 0;
			*(tris++) = 0;
			*(tris++) = 3;
			*(tris++) = 2;

			//
			// bottom.
			//
			*(tris++) = 4;
			*(tris++) = 5;
			*(tris++) = 6;
			*(tris++) = 6;
			*(tris++) = 7;
			*(tris++) = 4;

			//
			// left
			//
			*(tris++) = 0;
			*(tris++) = 1;
			*(tris++) = 5;
			*(tris++) = 5;
			*(tris++) = 4;
			*(tris++) = 0;

			//
			// right.
			//
			*(tris++) = 6;
			*(tris++) = 2;
			*(tris++) = 3;
			*(tris++) = 3;
			*(tris++) = 7;
			*(tris++) = 6;

			//
			// front.
			//
			*(tris++) = 7;
			*(tris++) = 3;
			*(tris++) = 0;
			*(tris++) = 0;
			*(tris++) = 4;
			*(tris++) = 7;

			//
			// back.
			//
			*(tris++) = 1;
			*(tris++) = 2;
			*(tris++) = 6;
			*(tris++) = 6;
			*(tris++) = 5;
			*(tris++) = 1;
		}
	}
	else
	{
		mesh->num_tris = 5*2*6;
		mesh->cmds = GL_LINES;
		mesh->tris = new unsigned short[mesh->num_tris];

		unsigned short* tris = mesh->tris;

		//
		// top
		//
		*(tris++) = 0;
		*(tris++) = 1;
		*(tris++) = 2;
		*(tris++) = 0;
		*(tris++) = -1; // triangle break;
		*(tris++) = 2;
		*(tris++) = 3;
		*(tris++) = 0;
		*(tris++) = 2;
		*(tris++) = -1; // triangle break;

		//
		// bottom.
		///
		*(tris++) = 6;
		*(tris++) = 5;
		*(tris++) = 4;
		*(tris++) = 6;
		*(tris++) = -1; // triangle break;
		*(tris++) = 4;
		*(tris++) = 7;
		*(tris++) = 6;
		*(tris++) = 4;
		*(tris++) = -1; // triangle break;

		//
		// left
		//
		*(tris++) = 5;
		*(tris++) = 1;
		*(tris++) = 0;
		*(tris++) = 5;
		*(tris++) = -1; // triangle break;
		*(tris++) = 0;
		*(tris++) = 4;
		*(tris++) = 5;
		*(tris++) = 0;
		*(tris++) = -1; // triangle break;

		//
		// right.
		//
		*(tris++) = 3;
		*(tris++) = 2;
		*(tris++) = 6;
		*(tris++) = 3;
		*(tris++) = -1; // triangle break;
		*(tris++) = 6;
		*(tris++) = 7;
		*(tris++) = 3;
		*(tris++) = 6;
		*(tris++) = -1; // triangle break;

		//
		// front.
		//
		*(tris++) = 0;
		*(tris++) = 3;
		*(tris++) = 7;
		*(tris++) = 0;
		*(tris++) = -1; // triangle break;
		*(tris++) = 7;
		*(tris++) = 4;
		*(tris++) = 0;
		*(tris++) = 7;
		*(tris++) = -1; // triangle break;

		//
		// back.
		//
		*(tris++) = 6;
		*(tris++) = 2;
		*(tris++) = 1;
		*(tris++) = 6;
		*(tris++) = -1; // triangle break;
		*(tris++) = 1;
		*(tris++) = 5;
		*(tris++) = 6;
		*(tris++) = 1;
		*(tris++) = -1; // triangle break;
	}
}

void R_DrawBox( const vec3& mins, const vec3& maxs )
{
	glBegin(GL_QUADS);

	//
	// top.
	//
	glVertex3f( mins[0], mins[1], maxs[2] );
	glVertex3f( mins[0], maxs[1], maxs[2] );
	glVertex3f( maxs[0], maxs[1], maxs[2] );
	glVertex3f( maxs[0], mins[1], maxs[2] );
	
	//
	// bottom.
	//
	glVertex3f( mins[0], mins[1], mins[2] );
	glVertex3f( mins[0], maxs[1], mins[2] );
	glVertex3f( maxs[0], maxs[1], mins[2] );
	glVertex3f( maxs[0], mins[1], mins[2] );	
	
	//
	// left.
	//
	glVertex3f( mins[0], maxs[1], mins[2] );
	glVertex3f( mins[0], maxs[1], maxs[2] );
	glVertex3f( mins[0], mins[1], maxs[2] );
	glVertex3f( mins[0], mins[1], mins[2] );	
	
	//
	// right.
	//
	glVertex3f( maxs[0], maxs[1], mins[2] );
	glVertex3f( maxs[0], maxs[1], maxs[2] );
	glVertex3f( maxs[0], mins[1], maxs[2] );
	glVertex3f( maxs[0], mins[1], mins[2] );
	
	//
	// front.
	//
	glVertex3f( mins[0], mins[1], maxs[2] );
	glVertex3f( maxs[0], mins[1], maxs[2] );
	glVertex3f( maxs[0], mins[1], mins[2] );
	glVertex3f( mins[0], mins[1], mins[2] );
	
	//
	// back.
	//
	glVertex3f( mins[0], maxs[1], maxs[2] );
	glVertex3f( maxs[0], maxs[1], maxs[2] );
	glVertex3f( maxs[0], maxs[1], mins[2] );
	glVertex3f( mins[0], maxs[1], mins[2] );
	
	glEnd();
}

static int R_Pick2D( CMapView* pView, int x, int y, int w, int h, int pick_flags )
{
	CTreadDoc* pDoc = pView->GetDocument();

	int blends = _beq_add|_bm_off;
	int flags = _dtf_always|_dwm_off|_cfm_none|_cwm_all;

	R_glDisableAllTMUs( 0 );
	R_glSet( 0, flags|_no_arrays, blends );
	R_glCommit( 0 );

	glSelectBuffer( PICK_BUFF_SIZE, (GLuint*)c_pickbuff );
	glRenderMode( GL_SELECT );
	glInitNames();
	glPushName( 0 );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	GLint viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );
	gluPickMatrix( x, viewport[3] - y, w, h, viewport );
	glOrtho( pView->View.fMapLeft, pView->View.fMapRight,
			 pView->View.fMapBottom, pView->View.fMapTop, -65535.0f, 65535.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//
	// generate a transform for this side...
	//
	R_RotateForOrthoView( pView );

	//
	// draw all objects.
	//
	if( pick_flags&PICK_OBJECTS )
	{
		if( (pick_flags&PICK_SELECTED_ONLY) == 0 )
			R_RenderObjectList( pView, pDoc->GetObjectList(), R_DrawPick2D, flags, blends, 0xFF400040 );
		else
			R_RenderObjectList( pView, pDoc->GetSelectedObjectList(), R_DrawPick2D, flags, blends, 0xFF0000FF );
	}

	if( pick_flags&PICK_MANIPULATORS )
	{
		R_RenderManipulatorList( pView, pDoc->GetManipulatorList(), true );
		R_RenderManipulatorList( pView, pDoc->GetSelectedManipulatorList(), true );
	}

	return glRenderMode(GL_RENDER);
}

static void R_RenderManipulatorList( CMapView* pView, CLinkedList<CManipulator>* list, bool pick )
{
	CManipulator* m;
	CPickObject* p;

	for( m = list->ResetPos(); m; m = list->GetNextItem() )
	{
		if( (m->GetViewFlags()&pView->GetViewType()) == 0 )
			continue;

		if( pick )
		{
			p = (CPickObject*)m;
			glLoadName( (GLuint)p );
		}
		m->OnDraw( pView );
	}
}

static int R_Pick3D( CMapView* pView, int x, int y, int w, int h, int pick_flags )
{
	CTreadDoc* pDoc = pView->GetDocument();

	//
	// setup the frustum.
	//
	glSelectBuffer( PICK_BUFF_SIZE, (GLuint*)c_pickbuff );
	glRenderMode( GL_SELECT );
	glInitNames();
	glPushName( 0 );

	R_SetupPickProjection( pView, x, y, w, h );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	R_RotateForViewer( pView->View.pos3d, pView->View.or3d );

	//
	// draw all objects.
	//
	int blends = _beq_add|_bm_off;
	int flags = _dtf_lequal|_dwm_on|_cwm_all|_cfm_back|_cfm_front_cw;

	R_glDisableAllTMUs( 0 );
	R_glSet( 0, flags|_no_arrays, blends );
	R_glCommit( 0 );

	glClearColor( 0, 0, 0, 0 );
	glClear( GL_DEPTH_BUFFER_BIT );

	if( pick_flags&PICK_OBJECTS )
	{
		if( (pick_flags&PICK_SELECTED_ONLY) == 0 )
			R_RenderObjectList( pView, pDoc->GetObjectList(), R_DrawPick3D, flags, blends, 0xFFFFFFFF );

		R_RenderObjectList( pView, pDoc->GetSelectedObjectList(), R_DrawPick3D, flags, blends, 0xFFFFFFFF );
	}
	if( pick_flags&PICK_MANIPULATORS )
	{
		if( pDoc->IsInTrackAnimationMode() == false )
		{
			R_RenderManipulatorList( pView, pDoc->GetManipulatorList(), true );
			R_RenderManipulatorList( pView, pDoc->GetSelectedManipulatorList(), true );
		}
	}

	return glRenderMode(GL_RENDER);
}

OS_FNEXP void R_PickObjectList( CMapView* pView, int x, int y, int w, int h, CPickObject*** list, int* num, int flags )
{
	int num_hits = 0;

	pView->BindGL();
	R_glSetDriverState( pView->GetGLState() );

	if( pView->GetViewType() == VIEW_TYPE_3D )
	{
		num_hits = R_Pick3D( pView, x, y, w, h, flags );
	}
	else
	{
		num_hits = R_Pick2D( pView, x, y, w, h, flags|PICK_SELECTED_ONLY );
		if( (num_hits < 1) && (flags&PICK_OBJECTS) && (flags&PICK_SELECTED_ONLY) == 0 )
		{
			//
			// try to pick normal objects.
			//
			num_hits = R_Pick2D( pView, x, y, w, h, flags );
		}
	}

	R_glGetDriverState( pView->GetGLState() );
	pView->UnbindGL();

	*list = 0;
	*num = 0;

	if( num_hits > 0 )
	{
		int i;

		R_SortPickBuff( num_hits );

		*list = new CPickObject*[num_hits];
		*num = num_hits;

		for(i = 0; i < num_hits; i++)
		{
			(*list)[i] = (CPickObject*)R_GetPickBuffObject(i);
		}
	}
}

OS_FNEXP void R_DeletePickObjectList(CPickObject **list)
{
	if (list)
	{
		delete [] list;
	}
}

OS_FNEXP CPickObject* R_PickObject( CMapView* pView, int x, int y, int w, int h, int flags )
{
	CPickObject** list;
	int num;

	R_PickObjectList( pView, x, y, w, h, &list, &num, flags );

	if( num > 0 )
	{
		CPickObject* p = list[0];
		R_DeletePickObjectList(list);
		return p;
	}

	return 0;
}

static void R_DrawPick3D( CRenderMesh* mesh, int flags, int blend, unsigned int color )
{
	R_glSet( 0, flags|_va_on|_ca_off|_ia_off|_na_off, blend );
	R_glSetArray( 0, _vertex, 3, GL_FLOAT, sizeof(vec3), mesh->xyz );
	R_glCommit( 0 );

	if( mesh->pick )
	{
		glLoadName((GLuint)mesh->pick);
		R_OutputMeshTris( mesh );	
	}

	if( mesh->polys.IsEmpty() == false )
	{
		R_glLockArrays(0, mesh->num_pts-1);

		CRenderMesh::poly_s* poly;
		for( poly = mesh->polys.ResetPos(); poly; poly = mesh->polys.GetNextItem() )
		{
			if( (poly->pick || mesh->pick ) )
			{
				glLoadName((GLuint)(poly->pick?poly->pick:mesh->pick));
				R_OutputMeshPoly( poly );
			}
		}

		R_glUnlockArrays();
	}
}

static void R_RenderObjectList( CMapView* pView, CLinkedList<CMapObject>* list, DRAWPROC proc, int flags, int blends, unsigned int color  )
{
	int i, c;
	CMapObject* o;
	CLinkedList<CMapObject> temp_list;

	//
	// render everything but entities.
	//
	for(o = list->ResetPos(); o; )
	{
		if( o->IsVisible() == false )
		{
			o = list->GetNextItem();
			continue;
		}
		
		if( o->GetClass() == MAPOBJ_CLASS_ENTITY )
		{
			list->RemoveItem( LL_CURRENT );
			temp_list.AddItem( o );
			o = list->GetItem( LL_CURRENT );
			continue;
		}

		c = o->GetNumRenderMeshes( pView );
		for(i = 0; i < c; i++)
			proc( o->GetRenderMesh(i, pView), flags, blends, color );

		o = list->GetNextItem();
	}

	//
	// render the entities.
	//
	for( ;; )
	{
		o = temp_list.RemoveItem( LL_HEAD );
		if( !o )
			break;

		list->AddItem( o );

		c = o->GetNumRenderMeshes( pView );
		for(i = 0; i < c; i++)
			proc( o->GetRenderMesh(i, pView), flags, blends, color );
	}
}

static inline void R_OutputMeshPoly( CRenderMesh::poly_s* poly, int mode )
{
	int i;
	glBegin(mode);
	for(i = 0; i < poly->num_pts; i++)
	{
		glArrayElement( poly->indices[i] );
	}
	glEnd();
}

static inline void R_OutputMeshTris( CRenderMesh* mesh )
{
	if( mesh->num_tris > 0 )
	{
		if( mesh->cmds == GL_TRIANGLES )
		{
			R_glDrawRangeElements( GL_TRIANGLES, 0, mesh->num_pts-1, mesh->num_tris*3, GL_UNSIGNED_SHORT, mesh->tris );
		}
		else
		if( mesh->cmds == GL_LINES )
		{
			glLineWidth( mesh->line_size );

			R_glLockArrays( 0, mesh->num_pts-1 );
			glBegin(GL_LINES);
			int i;
			for(i = 0; i < mesh->num_tris-1; i++) // num_tris is the point count.
			{
				if( mesh->tris[i] == (unsigned short)-1 )
				{
					continue;
				}
				if(mesh->tris[i+1] == (unsigned short)-1 )
				{
					i++;
					continue;
				}

				glArrayElement( mesh->tris[i] );
				glArrayElement( mesh->tris[i+1] );
			}
			glEnd();
			R_glUnlockArrays();

			glLineWidth( 1 );
		}
	}
}

//
// this has to be done because triangle and polygons don't obey glPolygonMode() in hit mode.
//
static void R_DrawPick2D( CRenderMesh* mesh, int flags, int blend, unsigned int color )
{
	//
	// assume that wireframe states are already set.
	//
	R_glSet( 0, flags|_va_on|_ca_off|_ia_off|_na_off, blend );
	R_glSetArray( 0, _vertex, 3, GL_FLOAT, sizeof(vec3), mesh->xyz );

	R_glCommit( 0 );

	if( mesh->pick && mesh->num_tris > 0 )
	{
		glLoadName((GLuint)mesh->pick);

		if( mesh->solid2d ) 
		{
			R_OutputMeshTris( mesh );
		}
		else
		{
			R_glLockArrays(0, mesh->num_pts-1);

			if( mesh->cmds == GL_TRIANGLES )
			{
				int i, ofs;
				for(i = 0, ofs = 0; i < mesh->num_tris; i++, ofs+=3)
				{
					glBegin(GL_LINE_LOOP);

						glArrayElement( mesh->tris[ofs] );
						glArrayElement( mesh->tris[ofs+1] );
						glArrayElement( mesh->tris[ofs+2] );

					glEnd();
				}
			}
			else
			{
				glBegin(GL_LINES);

				int i;
				for(i = 0; i < mesh->num_tris-1; i++)
				{
					if( mesh->tris[i] == (unsigned short)-1 )
					{
						continue;
					}
					if(mesh->tris[i+1] == (unsigned short)-1 )
					{
						i++;
						continue;
					}

					glArrayElement( mesh->tris[i] );
					glArrayElement( mesh->tris[i+1] );
				}

				glEnd();
			}
		}
	}
	
	if( mesh->polys.IsEmpty() == false )
	{
		R_glLockArrays(0, mesh->num_pts-1);

		CRenderMesh::poly_s* poly;
		for( poly = mesh->polys.ResetPos(); poly; poly = mesh->polys.GetNextItem() )
		{
			if( (poly->pick || mesh->pick ) )
			{
				glLoadName((GLuint)(poly->pick?poly->pick:mesh->pick));
				R_OutputMeshPoly( poly, (mesh->solid2d)?GL_POLYGON:GL_LINE_LOOP );
			}
		}

		R_glUnlockArrays();
	}
}

static void R_DrawMesh2D( CRenderMesh* mesh, int flags, int blend, unsigned int color )
{
	//
	// assume that wireframe states are already set.
	//
	if( mesh->solid2d == true )
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	//R_glDisableAllTMUs(0);

	R_glSet( 0, flags|_va_on|_ca_off|_ia_off|_na_off, blend );
	R_glSetArray( 0, _vertex, 3, GL_FLOAT, sizeof(vec3), mesh->xyz );

	if( color == 0xFFFFFFFF || mesh->allow_selected == false )
		glColor4ubv( (GLubyte*)&mesh->color2d );
	else
		glColor4ubv( (GLubyte*)&color );

	if( mesh->texture ) // oooooh.
	{
		R_glSetTMU( 0, TMU_0, mesh->texture, _tca_on|_tem_modulate );
		R_glSetTMUArray( 0, TMU_0, _texcoord, 2, GL_FLOAT, sizeof(vec2), mesh->st );
	}
	else
	{
		R_glSetTMU( 0, TMU_0, 0, _tca_off|_tem_modulate );
	}

	R_glCommit( 0 );

	R_OutputMeshTris( mesh );	
	
	if( mesh->polys.IsEmpty() == false )
	{
		R_glLockArrays(0, mesh->num_pts-1);

		CRenderMesh::poly_s* poly;
		for( poly = mesh->polys.ResetPos(); poly; poly = mesh->polys.GetNextItem() )
		{
			R_OutputMeshPoly( poly );
		}

		R_glUnlockArrays();
	}

	if( mesh->solid2d == true )
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

static void R_DrawWireframe( CRenderMesh* mesh, int flags, int blend, unsigned int color )
{
	if( !mesh->allow_wireframe )
		return;

	//
	// assume that wireframe states are already set.
	//
	R_glSet( 0, flags|_va_on|_ca_off|_ia_off|_na_off, blend );
	R_glSetArray( 0, _vertex, 3, GL_FLOAT, sizeof(vec3), mesh->xyz );

	if( color != 0xFF0000FF )
		glColor4ubv( (GLubyte*)&mesh->wireframe3d );
	else
		glColor4ubv( (GLubyte*)&color );

	R_glCommit( 0 );

	R_OutputMeshTris( mesh );		
	
	if( mesh->polys.IsEmpty() == false )
	{
		R_glLockArrays(0, mesh->num_pts-1);

		CRenderMesh::poly_s* poly;
		for( poly = mesh->polys.ResetPos(); poly; poly = mesh->polys.GetNextItem() )
		{
			R_OutputMeshPoly( poly );
		}

		R_glUnlockArrays();
	}
}

static void R_DrawTextured( CRenderMesh* mesh, int flags, int blend, unsigned int color )
{
	R_glDisableAllTMUs( 0 );
	
	flags |= _va_on|_ca_off|_ia_off;

	R_glSetArray( 0, _vertex, 3, GL_FLOAT, sizeof(vec3), mesh->xyz );

	if( mesh->normals[0] )
	{
		flags |= _na_on;
		R_glSetArray( 0, _normal, 0, GL_FLOAT, sizeof(vec3), mesh->normals[0] );
	}
	else
	{
		flags |= _na_off;
	}
	
	if( color == 0xFFFFFFFF || mesh->allow_selected == false )
	{
		glColor4ubv( (GLubyte*)&mesh->color3d );

	}
	else
	{
		glColor4ubv( (GLubyte*)&color );
	}

	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mesh->fcolor3d );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mesh->fcolor3d );

	R_glSet( 0, flags, blend );

	if( mesh->num_tris > 0 )
	{
		if (mesh->shader && mesh->shader->Load())
		{
			R_glSetTMU( 0, TMU_0, mesh->shader->Load(), _tca_on|_tem_modulate );
			R_glSetTMUArray( 0, TMU_0, _texcoord, 2, GL_FLOAT, sizeof(vec2), mesh->st );
		}
		else
		if( mesh->texture )
		{
			R_glSetTMU( 0, TMU_0, mesh->texture, _tca_on|_tem_modulate );
			R_glSetTMUArray( 0, TMU_0, _texcoord, 2, GL_FLOAT, sizeof(vec2), mesh->st );
		}
		else
		{
			R_glSetTMU( 0, TMU_0, 0, _tca_off|_tem_modulate );
		}
		
		R_glCommit( 0 );

		//
		// check for this case...
		//
		if( mesh->shader == 0 && mesh->texture )
		{
			if( mesh->texture->name == 0 )
			{
				R_glSetTMU( 0, TMU_0, 0, _tca_off|_tem_modulate );
				R_glCommit( 0 );
				return;
			}
		}

		R_OutputMeshTris( mesh );	
	}

	if( mesh->polys.IsEmpty() == false )
	{
		if( mesh->st )
			R_glSetTMUArray( 0, TMU_0, _texcoord, 2, GL_FLOAT, sizeof(vec2), mesh->st );

		R_glCommit( 0 );
		R_glLockArrays(0, mesh->num_pts-1);

		CRenderMesh::poly_s* poly;
		for( poly = mesh->polys.ResetPos(); poly; poly = mesh->polys.GetNextItem() )
		{
			if( poly->selected )
				glColor4ubv( (GLubyte*)&poly->selcolor );

			if( poly->shader && poly->shader->Load() )
				R_glSetTMU( 0, TMU_0, poly->shader->Load() , _tca_on|_tem_modulate );
			else
			if( poly->texture )
				R_glSetTMU( 0, TMU_0, poly->texture , _tca_off|_tem_modulate );
			else
				R_glSetTMU( 0, TMU_0, 0, _tca_off|_tem_replace );

			R_glCommit( 0 );
			R_OutputMeshPoly( poly );

			if( poly->selected )
				glColor4ubv( (GLubyte*)&color );
		}

		R_glUnlockArrays();
	}
}

void R_RotateForViewer( const vec3& pos, const COrientation& or )
{	
	glRotatef(-90,  1, 0, 0);	    // put Z going up
    glRotatef( 90,  0, 0, 1);	    // put Z going up

	/*glRotatef(-(or.angles[0]), 1.0f, 0.0f, 0.0f);
	glRotatef(-(or.angles[1]), 0.0f, 1.0f, 0.0f);
	glRotatef(-(or.angles[2]), 0.0f, 0.0f, 1.0f);*/
	mat4x4 m( or.m );
	glMultMatrixf( &m.m[0][0] );
	
	glTranslatef(-pos[0], -pos[1], -pos[2]);
}

void R_RotateForObject( const vec3& pos, const COrientation& or )
{
	glTranslatef( pos[0], pos[1], pos[2] );
	glRotatef( or.angles[2], 0, 0, 1 );
	glRotatef( or.angles[0], 1, 0, 0 );
	glRotatef( or.angles[1], 0, 1, 0 );
}

static void MYgluPerspective( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

static void R_SetupPickProjection( CMapView* pView, float x, float y, float w, float h )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	GLint viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );

	gluPickMatrix( x, viewport[3] - y, w, h, viewport );
		
	MYgluPerspective( pView->View.fov, pView->View.fWinWidth/pView->View.fWinHeight, 1.0f, pView->View.clipdist );
}

static void R_SetupProjection( CMapView* pView )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if( pView->GetDocument()->GetViewLightingFlag() )
	{
		MYgluPerspective( pView->View.fov, pView->View.fWinWidth/pView->View.fWinHeight, 4.0f, 30000.0f );
	}
	else
	{
		MYgluPerspective( pView->View.fov, pView->View.fWinWidth/pView->View.fWinHeight, 4.0f, pView->View.clipdist );
	}
}

static void R_DisableLights(void)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
}

static void R_EnableLights(void)
{
	glEnable(GL_LIGHTING);
	glShadeModel( GL_SMOOTH );

	float dir0[4]  = { 0.4f, 0.7f, 1.0f, 0.0f };
	float amb0[4]  = { 0.2f, 0.2f, 0.2f, 1.0f };
	float diff0[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	glLightfv( GL_LIGHT0, GL_POSITION, dir0 );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diff0 );
	glEnable(GL_LIGHT0);

	float dir1[4]  = { -0.4f, -0.7f, -1.0f, 0.0f };
	float diff1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	glLightfv( GL_LIGHT1, GL_POSITION, dir1 );
	glLightfv( GL_LIGHT1, GL_DIFFUSE, diff1 );
	glEnable(GL_LIGHT1);

	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
}

static void R_Draw3D( CMapView* pView )
{
	CTreadDoc* pDoc = pView->GetDocument();

	if( pDoc->GetViewLightingFlag() )
	{
		//R_DrawLightView( pView, pDoc );
		return;
	}

	//
	// setup the frustum.
	//
	R_SetupProjection( pView );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	R_RotateForViewer( pView->View.pos3d, pView->View.or3d );
	
	//
	// draw all objects.
	//
	int blends = _bm_off;//_beq_add|_bms_src_alpha|_bmd_inv_src_alpha;
	int flags = _dtf_lequal|_dwm_on|_cwm_all|_cfm_back|_cfm_front_cw;

	R_glDisableAllTMUs( 0 );
	R_glSet( 0, flags|_no_arrays, blends );
	R_glCommit( 0 );

	glClearColor( 0, 0, 0, 0 );
	glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

	if( pView->View.bShaded )
		R_EnableLights();

	glDisable( GL_ALPHA_TEST );
	//glAlphaFunc( GL_GREATER, 0.05f );

	R_RenderObjectList( pView, pDoc->GetObjectList(), R_DrawTextured, flags, blends, 0xFFFFFFFF );
	
	if( pView->View.bShaded )
		R_DisableLights();

	R_RenderObjectList( pView, pDoc->GetSelectedObjectList(), R_DrawTextured, flags, blends, 0xFF8080FF );	

	//glDisable( GL_ALPHA_TEST );
	blends = _beq_add|_bm_off;

	flags = _dtf_always|_dwm_off|_cwm_all|_cfm_none;

	R_glDisableAllTMUs( 0 );
	R_glSet( 0, flags|_no_arrays, blends );
	R_glCommit( 0 );

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	if( pView->View.bSelectionWireframe )
	{
		R_RenderObjectList( pView, pDoc->GetSelectedObjectList(), R_DrawWireframe, flags, blends, 0xFF0000FF );
	}

	if( pView->View.bShowWireframe )
	{
		R_RenderObjectList( pView, pDoc->GetObjectList(), R_DrawWireframe, flags, blends, 0xFFDDDDDD );
	}
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	R_glSet( 0, _dtf_lequal|_dwm_on|_cwm_all|_cfm_none|_no_arrays, blends );
	R_glCommit( 0 );

	glClear( GL_DEPTH_BUFFER_BIT );

	if( pDoc->IsInTrackAnimationMode() == false ) // not in playback?
	{
		R_RenderManipulatorList( pView, pDoc->GetManipulatorList(), false );
		R_RenderManipulatorList( pView, pDoc->GetSelectedManipulatorList(), false );
	}

	if( pDoc->ObjectsAreSelected() )
	{
		if( pDoc->DrawSelBox() && pDoc->IsInTrackAnimationMode() == false )
		{

			R_glSet( 0, _dtf_always|_dwm_off|_cwm_all|_cfm_none|_no_arrays, blends );
			R_glCommit( 0 );
			
			glLineWidth( 2 );
			R_DrawSelBox( pDoc->m_selmins - 8.0f, pDoc->m_selmaxs + 8.0f, (pDoc->m_selmaxs-pDoc->m_selmins)/6.0f, 0xFF00FFFF );
			glLineWidth( 1 );
		}

		if( pView->View.bShowEntityNames )
			R_DrawEntityNames( pView, pDoc->GetSelectedObjectList() );
	}

	//
	// leak trace.
	//
	{
		vec3* xyz;
		int num_xyz;

		pView->GetDocument()->GetLeakTrace( &xyz, &num_xyz );
		if( xyz && num_xyz > 0 )
		{
			R_DrawLeakTrace( pView, xyz, num_xyz, 0xFFFFFF00 );
		}
	}

	if( pView->View.bSelBoxVisible )
	{
		R_DrawSelBox( pView, &pView->View.selbox, 0xFF0000FF );
	}

	R_DrawTypeString( pView );
	pView->SwapBuffers();
}

static void R_DrawTypeString( CMapView* pView )
{
	glViewport( 0, 0, pView->View.fWinWidth, pView->View.fWinHeight );

	R_glDisableAllTMUs( 0 );
	R_glSet( 0, _dtf_always|_dwm_off|_cfm_none|_cwm_all|_no_arrays, _beq_add|_bm_off );
	R_glCommit( 0 );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, pView->View.fWinWidth, pView->View.fWinHeight, 0, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	char string[256];
	if( pView->GetViewType() != VIEW_TYPE_3D )
	{

		glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );

		if( pView->GetGridSnap() )
		{
			sprintf(string, "%s | grid = %d | mag = %.3fx", pView->GetViewTypeString(), (int)pView->GetGridSize(), pView->View.fScale);
		}
		else
		{
			sprintf(string, "%s | grid ain't on | mag = %.3fx", pView->GetViewTypeString(), pView->View.fScale);
		}
	}
	else
	{
		glColor4f( 0.0f, 1.0f, 1.0f, 1.0f );

		if( pView->GetGridSnap() )
		{
			sprintf( string, "%s | grid = %d | fov = %.2f | clip = %.2f", pView->GetViewTypeString(), (int)pView->GetGridSize(), pView->View.fov, pView->View.clipdist );
		}
		else
		{
			sprintf( string, "%s | grid ain't on | fov = %.2f | clip = %.2f", pView->GetViewTypeString(), pView->View.fov, pView->View.clipdist );
		}
	}

	R_DrawStringXY( 5, 14, VIEWTYPE_FONT, string );
}

static void R_DrawSelBox( CMapView* pView, RECT* r, unsigned int color )
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, pView->View.fWinWidth,
			 pView->View.fWinHeight, 0, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glColor4ubv( (GLubyte*)&color );
	glBegin(GL_LINES);

	glVertex2i( r->left, r->top );
	glVertex2i( r->right, r->top );

	glVertex2i( r->right, r->top );
	glVertex2i( r->right, r->bottom );

	glVertex2i( r->right, r->bottom );
	glVertex2i( r->left, r->bottom );

	glVertex2i( r->left, r->bottom );
	glVertex2i( r->left, r->top );

	glEnd();
}

static void R_Draw2D( CMapView* pView )
{
	CTreadDoc* pDoc = pView->GetDocument();

	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	int blends = _beq_add|_bm_off;
	int flags = _dtf_always|_dwm_off|_cfm_none|_cwm_all;

	R_glDisableAllTMUs( 0 );
	R_glSet( 0, flags|_no_arrays, blends );
	R_glCommit( 0 );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( pView->View.fMapLeft, pView->View.fMapRight,
			 pView->View.fMapBottom, pView->View.fMapTop, -65535.0f, 65535.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	R_DrawGrid( pView );

	//
	// generate a transform for this view.
	//
	R_RotateForOrthoView( pView );

	blends = _beq_add|_bms_src_alpha|_bmd_inv_src_alpha;

	//
	// draw all objects.
	//
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	R_RenderObjectList( pView, pDoc->GetObjectList(), R_DrawMesh2D, flags, blends, 0xFFFFFFFF );
	R_RenderObjectList( pView, pDoc->GetSelectedObjectList(), R_DrawMesh2D, flags, blends, 0xFF0000FF );

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	blends = _beq_add|_bm_off;
	R_glSet( 0, flags|_no_arrays, blends );
	R_glDisableAllTMUs(0);
	R_glCommit( 0 );

	if( pDoc->ObjectsAreSelected() )
	{
		if( pDoc->DrawSelBox() && pDoc->IsInTrackAnimationMode() == false )
		{
			R_DrawSelBox( pDoc->m_selmins - 8.0f, pDoc->m_selmaxs + 8.0f, (pDoc->m_selmaxs-pDoc->m_selmins)/6.0f, 0xFFFFFFFF );
			R_DrawBoxSize( pView, pDoc->m_selmins, pDoc->m_selmaxs, pDoc->m_selpos );
		}

		if( pView->View.bShowEntityNames )
			R_DrawEntityNames( pView, pDoc->GetSelectedObjectList() );
	}

	blends = _beq_add|_bms_src_alpha|_bmd_inv_src_alpha;
	R_glSet( 0, flags|_no_arrays, blends );
	R_glCommit( 0 );

	R_RenderManipulatorList( pView, pDoc->GetManipulatorList(), false );
	R_RenderManipulatorList( pView, pDoc->GetSelectedManipulatorList(), false );

	blends = _beq_add|_bm_off;
	R_glSet( 0, flags|_no_arrays, blends );
	R_glCommit( 0 );

	//
	// leak trace.
	//
	{
		vec3* xyz;
		int num_xyz;

		pView->GetDocument()->GetLeakTrace( &xyz, &num_xyz );
		if( xyz && num_xyz > 0 )
		{
			R_DrawLeakTrace( pView, xyz, num_xyz, 0xFFFFFF00 );
		}
	}

	if( pView->View.bSelBoxVisible )
	{
		R_DrawSelBox( pView, &pView->View.selbox, 0xFF0000FF );
	}

	R_DrawTypeString( pView );

	pView->SwapBuffers();
}

#define DIST_OFS	1
#define NAME_OFS	3

static int _cdecl compare_hit( const void* elm1, const void* elm2 )
{
	LongWord* hit1, *hit2;

	hit1 = (LongWord*)elm1;
	hit2 = (LongWord*)elm2;

	return (hit1[DIST_OFS]<hit2[DIST_OFS]) ? -1 : (hit1[DIST_OFS]>hit2[DIST_OFS]) ? 1 : 0;
}

static void R_SortPickBuff( int num_hits )
{
	//
	// a hit record is 16 bytes long.
	//
	qsort( c_pickbuff, num_hits, 16, compare_hit );
}

static void* R_GetPickBuffObject( int num )
{
	num <<= 2;

	if( num < 0 || num >= PICK_BUFF_SIZE )
		return 0;

	return (void*)(c_pickbuff[num+NAME_OFS]);
}

void R_DrawStringXYZ( const vec3& xyz, int font, const char* string )
{
	R_DrawStringXYZ( xyz[0], xyz[1], xyz[2], font, string );
}

void R_DrawStringXYZ( float x, float y, float z, int font, const char* string )
{
	if( !string || !string[0] )
		return;

	glRasterPos3f( x, y, z );
	glListBase( font );
	glCallLists( strlen(string), GL_UNSIGNED_BYTE, string );
}

void R_DrawStringXY( float x, float y, int font, const char* string )
{
	R_DrawStringXYZ( x, y, 0, font, string );
}

//
// Ortho projection should already be setup.
//
static void R_DrawGrid( CMapView* pView )
{
	CTreadDoc* pDoc = pView->GetDocument();

	if( pView->View.bGridVisible == FALSE )
		return;
	
	float fGridSize = pView->GetGridSize();
	if( fGridSize < 1.0f )
		return;

	if( pView->View.fScale*fGridSize  < 2.0f )
		return;

	//
	// do vertical lines.
	//
	float pos;
	float vmin, vmax;

	pos = pView->View.fMapMinX;
	pos = floorf( pos/fGridSize ) * fGridSize;

	vmin = pView->View.fMapMinY;
	vmax = pView->View.fMapMaxY;

	glBegin( GL_LINES );
	while( pos < pView->View.fMapMaxX )
	{
		if( pos > -1.0f && pos <= 1.0f )
		{
			//
			// origin color.
			//
			glColor4f( 0.44f, 0.44f, 0.44f, 1.0f );
		}
		else
		{
			glColor4f( 0.58f, 0.58f, 0.58f, 1.0f );
		}

		glVertex2f( pos, vmin );
		glVertex2f( pos, vmax );

		pos += fGridSize;
	}
	
	pos = pView->View.fMapMinY;
	pos = floorf( pos/fGridSize ) * fGridSize;

	vmin = pView->View.fMapMinX;
	vmax = pView->View.fMapMaxX;

	while( pos < pView->View.fMapMaxY )
	{
		if( pos > -1.0f && pos <= 1.0f )
		{
			//
			// origin color.
			//
			glColor4f( 0.44f, 0.44f, 0.44f, 1.0f );
		}
		else
		{
			glColor4f( 0.58f, 0.58f, 0.58f, 1.0f );
		}

		glVertex2f( vmin, pos );
		glVertex2f( vmax, pos );

		pos += fGridSize;
	}
	glEnd();
}

void R_RenderWindow( CMapView* pView )
{
	//
	// setup the local driver state.
	//
	//AfxGetApp()->BeginWaitCursor();

	pView->BindGL();
	R_glSetDriverState( pView->GetGLState() );

	glViewport( 0, 0, pView->View.fWinWidth, pView->View.fWinHeight );

	int nType = pView->GetViewType();
	if( nType == VIEW_TYPE_TEXTURE )
	{
		R_RenderTextureList( pView, 0, 0, 0 );
	}
	else
	if( nType&VIEW_FLAG_2D )
	{
		R_Draw2D( pView );
	}
	else
	if( nType == VIEW_TYPE_3D )
	{
		R_Draw3D( pView );
	}
	else
	{
		glClearColor( 0, 0, 0, 0 );
		glClear( GL_COLOR_BUFFER_BIT );
		R_DrawTypeString( pView );
		pView->SwapBuffers();
	}

	//
	// we have to disable ALL tmu's after this so the system can change
	// the rtex's around.
	//
	R_glDisableAllTMUs( 0 );
	R_glCommit( 0 );

	//
	// store the local driver state.
	//
	R_glGetDriverState( pView->GetGLState() );
	pView->UnbindGL();
	//AfxGetApp()->EndWaitCursor();

}

CShader* R_PickShader( CMapView* pView, int x, int y )
{
	CShader* s;

	R_RenderTextureList( pView, x, y, &s );

	return s;
}

#define FONT_WIDTH 7.5f

void R_AdjustToViewTexture( CMapView* pView, CShader* shader )
{
	CLinkedList<CShader>* pShaders = pView->GetDocument()->ShaderList();
	if (!pShaders) { return; }
	CShader* s;
	float w, h, x, y, my;
	CString filter;

	filter = pView->GetDocument()->GetTextureFilter();
	filter.MakeLower();

#define FONT_HEIGHT			16
#define TEXTURE_SPACING		8

	x = TEXTURE_SPACING;
	y = TEXTURE_SPACING;
	my = 0;

	for( s = pShaders->ResetPos(); s; s = pShaders->GetNextItem() )
	{
		if (!s->Pickable())
			continue;

		if( filter != "" )
		{
			//
			// can we list?
			//
			char a[256];
			strcpy(a, s->DisplayName());
			{
				char *p = a;
				while (p[0] != 0) { p[0] = (char)tolower(p[0]); ++p; }
			}
			if( strstr( a, filter ) == 0 )
				continue;
		}

		RTex_t *rtex = s->Load();

		if (!rtex)
			continue;

		//
		// draw the first texture.
		//
		
		{
			int a, b;
			s->Dimensions(&a, &b);
			w = a;
			h = b;
		}
		
		if( w > h )
		{
			if( w > pView->TexView.fMax )
			{
				float f = pView->TexView.fMax/w;

				w = pView->TexView.fMax;
				h *= f;
			}
		}
		else
		{
			if( h > pView->TexView.fMax )
			{
				float f = pView->TexView.fMax/h;

				h = pView->TexView.fMax;
				w *= f;
			}
		}

		char str[256];
		int totalw = w;

		{
			int dx, dy;
			s->Dimensions(&dx, &dy);
			sprintf(str, "(%dx%d)", dx, dy);
			float len = strlen(str);
			float len2 = strlen(s->DisplayName());
			len = max(len, len2);
			len *= FONT_WIDTH; // pixels per character.
			len += 2;
			totalw = max(len, w);
		}

		if( x > 0 )
		{
			if( x+totalw >= pView->View.fWinWidth-TEXTURE_SPACING )
			{
				x = TEXTURE_SPACING;
				y = y+my+FONT_HEIGHT+FONT_HEIGHT+TEXTURE_SPACING;
				my = 0;
			}
		}

		//
		// is this the texture?
		//
		if( s == shader )
		{
			//
			// is it off the edge?
			//
			if( (y < pView->TexView.fYofs) || 
				(y+h+FONT_HEIGHT+FONT_HEIGHT >= (pView->TexView.fYofs+pView->View.fWinHeight)) )
			{
				pView->TexView.fYofs = y - TEXTURE_SPACING;
			}
			
			return;
		}

		x += totalw+TEXTURE_SPACING;
		
		if( h > my )
			my = h;
	}
}

static void R_RenderTextureList( CMapView* pView, int mousex, int mousey, CShader** pickshader )
{
	CLinkedList<CShader>* pShaders = pView->GetDocument()->ShaderList();
	CShader* s;
	float w, h, x, y, my;
	CString filter;

	filter = pView->GetDocument()->GetTextureFilter();
	filter.MakeLower();

	if( pickshader )
	{
		*pickshader = 0;
		mousey += pView->TexView.fYofs;
	}
	else
	{	
		glRenderMode(GL_RENDER);
		glClearColor( 0.60f, 0.60f, 0.60f, 0.0f );
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
			
		glOrtho( 0, pView->View.fWinWidth, pView->View.fWinHeight+pView->TexView.fYofs, pView->TexView.fYofs, -1, 1 );

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		R_glDisableAllTMUs( 0 );
		R_glSet( 0, _dtf_always|_dwm_off|_cfm_none|_cwm_all|_no_arrays, _beq_add|_bm_off );
	}

	if (pShaders)
	{
	#define FONT_HEIGHT			16
	#define TEXTURE_SPACING		8

		x = TEXTURE_SPACING;
		y = TEXTURE_SPACING;
		my = 0;

		for( s = pShaders->ResetPos(); s; s = pShaders->GetNextItem() )
		{
			if (!s->Pickable())
				continue;

			if( filter != "" )
			{
				char a[256];
				strcpy(a, s->DisplayName());
				{
					char *p = a;
					while (p[0] != 0) { p[0] = (char)tolower(p[0]); ++p; }
				}
				if( strstr( a, filter ) == 0 )
					continue;
			}

			RTex_t *rtex = s->Load();
			
			if( !rtex )
			{
				w = 128;
				h = 128;
			}
			else
			{
				int a, b;
				s->Dimensions(&a, &b);
				w = a;
				h = b;
			}
			
			if( w > h )
			{
				if( w > pView->TexView.fMax )
				{
					float f = pView->TexView.fMax/w;

					w = pView->TexView.fMax;
					h *= f;
				}
			}
			else
			{
				if( h > pView->TexView.fMax )
				{
					float f = pView->TexView.fMax/h;

					h = pView->TexView.fMax;
					w *= f;
				}
			}

			char str[256];
			int totalw = w;

			{
				int dx, dy;
				s->Dimensions(&dx, &dy);
				sprintf(str, "(%dx%d)", dx, dy);
				float len = strlen(str);
				float len2 = strlen(s->DisplayName());
				len = max(len, len2);
				len *= FONT_WIDTH; // pixels per character.
				len += 2;
				totalw = max(len, w);
			}

			if( x > 0 )
			{
				if( x+totalw >= pView->View.fWinWidth-TEXTURE_SPACING )
				{
					x = TEXTURE_SPACING;
					y = y+my+FONT_HEIGHT+FONT_HEIGHT+TEXTURE_SPACING;
					my = 0;

					if( y >= pView->View.fWinHeight+pView->TexView.fYofs )
						break;
				}
			}

			if( y+h >= pView->TexView.fYofs )
			{
				if( pickshader )
				{
					if( mousex >= x && mousex <= x+totalw )
					{
						if( mousey >= y && mousey <= y+h+FONT_HEIGHT+FONT_HEIGHT )
						{
							*pickshader = s;
							break;
						}
					}
				}
				else
				{
					if (s->selected)
					{
						R_glSetTMU( 0, TMU_0, 0, 0 );
						R_glCommit( 0 );

						glColor4f( 1, 0, 0, 1 );
						glBegin(GL_QUADS);
						
							glVertex2f( x-TEXTURE_SPACING+4, y-TEXTURE_SPACING+4 );
							glVertex2f( x+totalw+TEXTURE_SPACING-4, y-TEXTURE_SPACING+4 );
							glVertex2f( x+totalw+TEXTURE_SPACING-4, y+h+TEXTURE_SPACING+FONT_HEIGHT+FONT_HEIGHT-4 );
							glVertex2f( x-TEXTURE_SPACING+4, y+h+TEXTURE_SPACING+FONT_HEIGHT+FONT_HEIGHT-4 );

						glEnd();
					}

					if(rtex)
						R_glSetTMU( 0, TMU_0, rtex, _tem_replace|_tca_off );
					else
						R_glSetTMU( 0, TMU_0, 0, _tem_replace|_tca_off );

					R_glCommit( 0 );
					
					int cx = x + (totalw>>1) - ((int)w>>1);
					glColor4f( 1, 1, 1, 1 );
					glBegin(GL_QUADS);

						glTexCoord2i( 0, 0 );
						glVertex2f( cx, y );
						glTexCoord2i( 1, 0 );
						glVertex2f( cx+w, y );
						glTexCoord2i( 1, 1 );
						glVertex2f( cx+w, y+h );
						glTexCoord2i( 0, 1 );
						glVertex2f( cx, y+h );

					glEnd();
					
					R_glSetTMU( 0, TMU_0, 0, 0 );
					R_glCommit( 0 );
					
					if( s->selected == false )
						glColor4f( 70.0f/255.0f, 185.0f/255.0f, 186.0f/255.0f, 1.0f );
					else
						glColor4f( 1, 0, 0, 1 );

					glBegin(GL_QUADS);

						glVertex2f( x, y+h );
						glVertex2f( x+totalw, y+h );
						glVertex2f( x+totalw, y+h+FONT_HEIGHT+FONT_HEIGHT );
						glVertex2f( x, y+h+FONT_HEIGHT+FONT_HEIGHT );

					glEnd();
					
					glColor4f( 1, 1, 1, 1 );

					{
						R_DrawStringXY( x+2, y+h+FONT_HEIGHT-4, TEXTURE_NAME_FONT, s->DisplayName() );
						R_DrawStringXY( x+2, y+h+FONT_HEIGHT+FONT_HEIGHT-4, TEXTURE_NAME_FONT, str);
					}

					if( !s->selected && s == pView->TexView.pHoverShader )
					{
						//
						// draw a selection box.
						//
						glColor4f( 1, 1, 0, 1 );
						
						glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

						glBegin(GL_QUADS);

							glVertex2f( x, y );
							glVertex2f( x+totalw, y );
							glVertex2f( x+totalw, y+h+FONT_HEIGHT+FONT_HEIGHT );
							glVertex2f( x, y+h+FONT_HEIGHT+FONT_HEIGHT );
							glVertex2f( x-1, y-1 );
							glVertex2f( x+totalw+1, y-1 );
							glVertex2f( x+totalw+1, y+h+FONT_HEIGHT+FONT_HEIGHT+1 );
							glVertex2f( x-1, y+h+FONT_HEIGHT+FONT_HEIGHT+1 );

						glEnd();

						glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
					}
				}
			}

			x += totalw+TEXTURE_SPACING;
			
			if( h > my )
				my = h;
		}
	}

	if( pickshader == 0 )
		pView->SwapBuffers();
}