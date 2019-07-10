///////////////////////////////////////////////////////////////////////////////
// r_cubemap.cpp
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
#include "r_cubemap.h"
#include "math3d.h"

typedef struct
{
	int size;
	
	void* data[6];
} cubemap_data_t;

static bool UploadCubeMap( RTex_t* tex );

static bool UploadCubeMap( RTex_t* tex )
{
	cubemap_data_t* cm = (cubemap_data_t*)tex->user_data[0];
	
	tex->size = 0;
	
	R_glUploadTexture_CubeMap( GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, tex,
		cm->size, cm->size, 1, GL_UNSIGNED_BYTE, GL_RGB, _upf_filter|_upf_mipmap, cm->data[0], 0 );
	R_glUploadTexture_CubeMap( GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, tex,
		cm->size, cm->size, 1, GL_UNSIGNED_BYTE, GL_RGB, _upf_filter|_upf_mipmap, cm->data[1], 0 );
	R_glUploadTexture_CubeMap( GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, tex,
		cm->size, cm->size, 1, GL_UNSIGNED_BYTE, GL_RGB, _upf_filter|_upf_mipmap, cm->data[2], 0 );
	R_glUploadTexture_CubeMap( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, tex,
		cm->size, cm->size, 1, GL_UNSIGNED_BYTE, GL_RGB, _upf_filter|_upf_mipmap, cm->data[3], 0 );
	R_glUploadTexture_CubeMap( GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, tex,
		cm->size, cm->size, 1, GL_UNSIGNED_BYTE, GL_RGB, _upf_filter|_upf_mipmap, cm->data[4], 0 );
	R_glUploadTexture_CubeMap( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, tex,
		cm->size, cm->size, 1, GL_UNSIGNED_BYTE, GL_RGB, _upf_filter|_upf_mipmap, cm->data[5], 0 );
				
	return FALSE;
}

static RTex_t* MakeCubeMapTexture( int size )
{
	cubemap_data_t* cubemap;
	
	//
	// we use floats.
	//
	
	int face_size = 3*size*size;
	int data_size = (face_size*6)+sizeof(cubemap_data_t);
	Byte* data = (Byte*)OS_AllocAPointer( data_size );
	if( !data )
	{
		return 0;
	}
	
	cubemap = (cubemap_data_t*)data;
	data += sizeof(cubemap_data_t);
	
	cubemap->size = size;
	
	//
	// setup data pointers.
	//
	int i, k;
	float halfsize = ((float)size)/2;
	
	for(i = 0; i < 6; i++)
	{
		cubemap->data[i] = data;
		data += face_size;
	}
	
	//
	// make the sides, +x, -x, +y, -y, +z, -z.
	//
	vec3 v;
	Byte* cubedata = (Byte*)cubemap->data[0];
	
	// +x
	for(i = 0; i < size; i++)
	{
		for( k = 0; k < size; k++ )
		{
			v[2] = -(k+0.5f-halfsize);
			v[1] = -(i+0.5f-halfsize);
			v[0] = halfsize;
			
			v.normalize();
			
			//
			// collapse the vector.
			//
			v *= 0.5f;
			v[0] += 0.5f;
			v[1] += 0.5f;
			v[2] += 0.5f;
			
			v *= 255.0f;
			
			cubedata[0] = (Byte)v[0];
			cubedata[1] = (Byte)v[1];
			cubedata[2] = (Byte)v[2];
			
			cubedata += 3;			
		}
	}
	
	// -x
	cubedata = (Byte*)cubemap->data[1];
	for(i = 0; i < size; i++)
	{
		for( k = 0; k < size; k++ )
		{
			v[2] = (k+0.5f-halfsize);
			v[1] = -(i+0.5f-halfsize);
			v[0] = -halfsize;
			
			v.normalize();
			
			//
			// collapse the vector.
			//
			v *= 0.5f;
			v[0] += 0.5f;
			v[1] += 0.5f;
			v[2] += 0.5f;
			
			v *= 255.0f;
			
			cubedata[0] = (Byte)v[0];
			cubedata[1] = (Byte)v[1];
			cubedata[2] = (Byte)v[2];
			
			cubedata += 3;			
		}
	}
	
	// +y
	cubedata = (Byte*)cubemap->data[2];
	for(i = 0; i < size; i++)
	{
		for( k = 0; k < size; k++ )
		{
			v[0] = (k+0.5f-halfsize);
			v[2] = (i+0.5f-halfsize);
			v[1] = halfsize;
			
			v.normalize();
			
			//
			// collapse the vector.
			//
			v *= 0.5f;
			v[0] += 0.5f;
			v[1] += 0.5f;
			v[2] += 0.5f;
			
			v *= 255.0f;
			
			cubedata[0] = (Byte)v[0];
			cubedata[1] = (Byte)v[1];
			cubedata[2] = (Byte)v[2];
			
			cubedata += 3;			
		}
	}
	
	// -y
	cubedata = (Byte*)cubemap->data[3];
	for(i = 0; i < size; i++)
	{
		for( k = 0; k < size; k++ )
		{
			v[0] = (k+0.5f-halfsize);
			v[2] = -(i+0.5f-halfsize);
			v[1] = -halfsize;
			
			v.normalize();
			
			//
			// collapse the vector.
			//
			v *= 0.5f;
			v[0] += 0.5f;
			v[1] += 0.5f;
			v[2] += 0.5f;
			
			v *= 255.0f;
			
			cubedata[0] = (Byte)v[0];
			cubedata[1] = (Byte)v[1];
			cubedata[2] = (Byte)v[2];
			
			cubedata += 3;			
		}
	}
	
	// +z
	cubedata = (Byte*)cubemap->data[4];
	for(i = 0; i < size; i++)
	{
		for( k = 0; k < size; k++ )
		{
			v[0] = (k+0.5f-halfsize);
			v[1] = -(i+0.5f-halfsize);
			v[2] = halfsize;
			
			v.normalize();
			
			//
			// collapse the vector.
			//
			v *= 0.5f;
			v[0] += 0.5f;
			v[1] += 0.5f;
			v[2] += 0.5f;
			
			v *= 255.0f;
			
			cubedata[0] = (Byte)v[0];
			cubedata[1] = (Byte)v[1];
			cubedata[2] = (Byte)v[2];
			
			cubedata += 3;			
		}
	}
	
	// -z
	cubedata = (Byte*)cubemap->data[5];
	for(i = 0; i < size; i++)
	{
		for( k = 0; k < size; k++ )
		{
			v[0] = -(k+0.5f-halfsize);
			v[1] = -(i+0.5f-halfsize);
			v[2] = -halfsize;
			
			v.normalize();
			
			//
			// collapse the vector.
			//
			v *= 0.5f;
			v[0] += 0.5f;
			v[1] += 0.5f;
			v[2] += 0.5f;
			
			v *= 255.0f;
			
			cubedata[0] = (Byte)v[0];
			cubedata[1] = (Byte)v[1];
			cubedata[2] = (Byte)v[2];
			
			cubedata += 3;			
		}
	}
	
	RTex_t* cubetex = R_glCreateTexture( "cmtex", size, size, 1, 3, UploadCubeMap, 0, 0 );
	
	if( !cubetex )
	{
		OS_DeallocAPointer( cubemap );
		return 0;
	}
	
	cubetex->user_data[0] = cubemap;
	
	//
	// tweak the appropriate target type.
	//
	cubetex->target = GL_TEXTURE_CUBE_MAP_ARB;
	cubetex->size = size*size*3*6;
	
	return cubetex;	
}

RTex_t* R_CreateCubeMapTexture( int size )
{
	return MakeCubeMapTexture( size );
}

void R_DeleteCubeMapTexture( RTex_t* tex )
{
	if( tex->user_data[0] )
	{
		OS_DeallocAPointer( tex->user_data[0] );
	}
	
	R_glDeleteTexture( tex );
}