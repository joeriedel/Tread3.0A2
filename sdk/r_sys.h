///////////////////////////////////////////////////////////////////////////////
// r_sys.h
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

#ifndef R_SYS_H
#define R_SYS_H

#include "os.h"
#include "r_gl.h"
#include "shaders.h"
#include "math3d.h"

extern LongWord r_frame;

class CMapView;
class CPickObject;
class CRenderMesh;
class CTreadDoc;
class CEntity;
class CMapObject;

#define PAK_GL_RGBA( r, g, b, a ) ( ((LongWord)r)+((LongWord)g<<8)+((LongWord)b<<16)+((LongWord)a<<24) )

#define PICK_MANIPULATORS		0x01
#define PICK_OBJECTS			0x02
#define PICK_SELECTED_ONLY		0x04

#if defined(__BUILDING_TREAD__)
void R_RenderWindow( CMapView* pView );
#endif

OS_FNEXP CShader* R_PickShader( CMapView* pView, int x, int y );
OS_FNEXP void R_AdjustToViewTexture( CMapView* pView, CShader* shader );
OS_FNEXP void R_PickObjectList( CMapView* pView, int x, int y, int w, int h, CPickObject*** list, int* num, int flags = (PICK_MANIPULATORS|PICK_OBJECTS) );
OS_FNEXP void R_DeletePickObjectList(CPickObject **list);
OS_FNEXP CPickObject* R_PickObject( CMapView* pView, int x, int y, int w, int h, int flags = (PICK_MANIPULATORS|PICK_OBJECTS) );
OS_FNEXP void R_DrawStringXYZ( const vec3& xyz, int font, const char* string );
OS_FNEXP void R_DrawStringXYZ( float x, float y, float z, int font, const char* string );
OS_FNEXP void R_DrawStringXY( float x, float y, int font, const char* string );
OS_FNEXP void R_DrawBox( const vec3& mins, const vec3& maxs );
OS_FNEXP void R_ExtractBBoxPoints( const vec3& mins, const vec3& maxs, vec3* outpts );
OS_FNEXP void R_MakeBoxMeshCmds( CRenderMesh* mesh, const vec3& mins, const vec3& maxs, bool solid = true, bool double_sided = false );

//
// Computes the texture space transform given the vertices, normals, and texture coordinates.
//
// The transform output is computed for the *center* vertex: xyz[1], normal.
//
OS_FNEXP void R_GetTXSpaceVecs( vec3* xyz[3], vec3* normal, vec2* st[3], vec3* out /* should be [2] in size */ );

#endif