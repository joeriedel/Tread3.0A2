///////////////////////////////////////////////////////////////////////////////
// texcache.h
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

#ifndef TEXCACHE_H
#define TEXCACHE_H

#include "os.h"
#include "r_gl.h"

#define TCRTEX_UNIQUE	0
#define TCRTEX_SHADER	1
#define TCRTEX_SKY		2
#define TCRTEX_LIGHT	3
#define TCRTEX_ICON		4

//
// texture caching system.
//
typedef struct TC_DLock_s
{
	struct TC_DLock_s* next, *prev;
	int key;
} TC_DLock_t;

typedef struct TC_GLock_s
{
	struct TC_GLock_s* next, *prev;
	RTex_t* rtex;
	Byte type;
} TC_GLock_t;

#define MAX_TCTEXTURE_NAME_LEN		255

typedef struct TC_Texture_s
{
	TC_DLock_t* d_locks;
	TC_GLock_t* g_locks;
	
	char name[MAX_TCTEXTURE_NAME_LEN+1];
	
	int d_key;
	int g_key;
	
	int width, height, bpp;
	int original_width, original_height;
	int type, components;
	
	void*  data;
	void** mips;
	void*  mip_data[MAX_MIPMAP_LEVELS];
	
	bool used;
	bool loaded;
		
} TC_Texture_t;

#define MAX_TC_LOCKS			32
#define MAX_CACHED_TEXTURES		2048
#define BAD_TC_HANDLE			-1

bool TC_Init();
bool TC_Shutdown();

//
// loads textures off disk into a residence structure that GL textures can be derrived from.
// the entire engine uses the texture caching mechanism so duplicate instances of textures
// are eliminated.
//
// for a caller to use the texcache succesfully they must do the following:
// retrieve a texture handle by calling TC_CacheTexture, or from some other system in the engine.
// if TC_CacheTexture is not used, then TC_LockTexData must be called. It will return a key that
// may be used to create an RTex_t. A texture is guaranteed to have the raw image data available 
// while it is locked. If it is not locked by the caller, the state is not known. When a texture is
// unlocked and becomes unreferenced, it will be purged from system memory.
//
// the caller should *never* release the Rtex_t returned from TC_AllocRTex. you should instead
// call TC_DeallocRTex.
//
// it is an error to attempt to lock a texture that is not resident. I.E. it was unrefed and
// subsequently freed. therefore, it is always best to call TC_CacheTexture() to ensure that
// the texture is avaible, and to achieve a lock.
//
// example lifetime of a texture:
//
// int handle, key;
//
// TC_CacheTexture( &handle, &key, "texture.tga", GL_ALPHA, TRUE, FALSE, FALSE );
// if( TC_VALID( handle, key ) )
// { handle error }
//
// RTex_t* tex = TC_AllocRTex( handle, key, proc, proc, proc );
// if( !rtex )
// { handle error }
//
// R_glUploadTexture(...)
// TC_UnlockTexture( handle, key );
//
// ... when done with texture
// TC_DeallocRTex( handle, rtex );
// 
// NOTE: you can allocate ONE rtex per lock!
//

#define TC_VALID( handle, key ) ( ((handle)!=BAD_TC_HANDLE)&&((key)!=BAD_TC_HANDLE) )

//
// TC_CacheTexture will perform a lock.
//
void TC_CacheTexture( int* handle, int* key, const char* filename, int index_components, bool mipmap, bool flip_horz = FALSE, bool flip_vert = FALSE, bool invert = FALSE );

//
// get's the texture object associated with the handle.
//
TC_Texture_t* TC_GetTexture( int handle );

//
// get's the rtex associated with the handle.
//
RTex_t* TC_AllocRTex( int handle, int key, int type, char* name, RTexTouchProc load_proc, RTexTouchProc rebind_proc, RTexTouchProc purge_proc );
void TC_DeallocRTex( int handle, RTex_t* rtex );

bool TC_TexUpload( int handle, int flags, RTex_t* tex, bool unlock );
bool TC_TexRebind( int handle, int flags, RTex_t* tex );
bool TC_TexPurge(  int handle, RTex_t* tex );

int TC_LockTexture( int handle );
void TC_UnlockTexture( int handle, int key );

void TC_FreeUnlockedTextures();
int TC_GetNumLockedTextures( void );
int TC_GetNumLoadedTextures( void );

bool TC_ReloadTexture( int handle, int index_components, bool mipmap, bool flip_horz = FALSE, bool flip_vert = FALSE, bool invert = FALSE );

#endif