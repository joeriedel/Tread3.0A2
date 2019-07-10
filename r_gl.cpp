///////////////////////////////////////////////////////////////////////////////
// r_gl.cpp
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
#include "util.h"
#include "files.h"
#include "System.h"
#include "texcache.h"

#ifdef __OPT_VC__
#include <stdlib.h>
#endif

#define TMU_FLAG(a) (1<<a)

#define MIN_MAJOR	1
#define MIN_MINOR	2

static glState_t def_state;
static glState_t driver_state;
static RTex_t* bound_list[2];
static RTex_t* unbound_list[2];
static int num_tmus;
static int num_imp_tmus;
static int alloced_size;
static int element_count=0;
static int accessed_texture_size=0;
static int accessed_texture_count=0;
static LongWord gl_version;
static bool imp_supports_ARB_multitexture=FALSE;
static bool imp_supports_ARB_imaging=FALSE;
static bool imp_supports_EXT_compiled_vertex_array=FALSE;
static bool locked_arrays=FALSE;
static bool do_element_count=FALSE;
static bool imp_supports_sgis_generate_mipmap=FALSE;
static bool imp_supports_s3tc_compression=FALSE;
static bool imp_supports_fxt1_compression=FALSE;
static bool imp_supports_ext_paletted_texture = FALSE;
static bool imp_supports_ARB_nonpow2 = FALSE;
static bool use_lock_arrays=TRUE;
static Byte* c_color_table=0;

static void SetTexture( RTex_t* tex );
static void BindTexture( RTex_t* tex );
static void PurgeTexture( RTex_t* tex );
static void PurgeTextureSpace( int size );
static void TextureAccessed( RTex_t* tex );
static void CommitTMU( TmuState_t* tmu, TmuState_t* drv );
static void LoadGLVersion(void);

/*
source contained in glu_mipmap.cpp
*/
GLint gluBuild3DMipmaps(GLenum target, GLint internalFormat, 
			GLsizei width, GLsizei height, GLsizei depth,
			GLenum format, GLenum type, const void *data);
			
#define ARRAY_STATES_EQUAL( a, b )	\
		(( (a)->array == (b)->array ) && ( (a)->size == (b)->size ) && ( (a)->type == (b)->type ) && ( (a)->stride == (b)->stride ))
#define INIT_ARRAY_STATE( a )	\
		( (a)->array = 0, (a)->size = 0, (a)->type = 0, (a)->stride = 0 )
#define LSP( a ) if( a == 0 ) a = &def_state

HGLRC R_glCreateContext( HDC hdc, bool need_window )
{
	PIXELFORMATDESCRIPTOR pfd;
    int  pixelFormat;
	HGLRC hglrc;

	memset(&pfd,0,sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL		// support OpenGL
		| PFD_DOUBLEBUFFER ,		// double buffered
	pfd.iPixelType = PFD_TYPE_RGBA;		// True color
	pfd.cColorBits = 32;				// True color
	pfd.cDepthBits = 32;				// Deep buffer
	pfd.cStencilBits = 8;
	pfd.cAlphaBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	if( need_window )
		pfd.dwFlags |= PFD_DRAW_TO_WINDOW;

	pixelFormat = ChoosePixelFormat(hdc, &pfd);
	if ( pixelFormat )
	{
        SetPixelFormat(hdc, pixelFormat, &pfd);
		{
            hglrc = wglCreateContext( hdc );	
            if (hglrc) 
			{
                if ( wglMakeCurrent( hdc, hglrc ) )
				{
					wglMakeCurrent( 0, 0 );
					return hglrc;
				}
				wglDeleteContext( hglrc );
            }
		}
	}

	return 0;
}

Byte* R_glGetColorTable()
{
	return c_color_table;
}

OS_FNEXP void R_glForceTextureUpload( RTex_t* tex )
{
	tex->reload = true;
}

static void FreeColorTable()
{
	if( c_color_table )
		OS_DeallocAPointer( c_color_table );
		
	c_color_table = 0;
}

void R_SetupBumpPal(void)
{
	if( R_glImpSupportsEXTPalettedTexture() && R_glEXT_ColorTable && c_color_table )
	{
		glEnable( GL_SHARED_TEXTURE_PALETTE_EXT );
		CHECK_GL_ERRORS();
		R_glEXT_ColorTable(GL_SHARED_TEXTURE_PALETTE_EXT, GL_RGB8, 256, GL_RGB, GL_UNSIGNED_BYTE, c_color_table);
		CHECK_GL_ERRORS();
	}
}

static bool LoadColorTable( const char* filename )
{
	Byte* data;
	LongWord size;
	
	FreeColorTable();
	
	data = (Byte*)FS_LoadFile( filename, &size );
	if( !data )
	{
		OS_OkAlertMessage( "Error", "Unable to load color table!" );
		return TRUE;
	}
	
	OS_ASSERT( size > 0 );
	
	if( size != 768 ) // 256 color palette?
	{
		OS_DeallocAPointer( data );
		data = 0;
		OS_OkAlertMessage( "Error", "Color table is corrupt!" );
		return TRUE;
	}
	
	c_color_table = data;
	return FALSE;
}

void R_glBeginFrame(void)
{
	accessed_texture_size = 0;
	accessed_texture_count = 0;
}

void R_glEndFrame(void)
{
}

int R_glGetAccessedTextureMemory(void)
{
	return accessed_texture_size;
}

int R_glGetAccessedTextureCount(void)
{
	return accessed_texture_count;
}

void R_glUseLockArrays( bool use )
{
	use_lock_arrays=use;
}

bool R_glImpSupportsFXT1Compression( void )
{
	return imp_supports_fxt1_compression;
}

bool R_glImpSupportsS3TCCompression( void )
{
	return imp_supports_s3tc_compression;
}

bool R_glImpSupportsSGISGenerateMipmap( void )
{
	return imp_supports_sgis_generate_mipmap;
}

bool R_glImpSupportsEXTPalettedTexture( void )
{
	return imp_supports_ext_paletted_texture && FALSE;
}

bool R_glImpSupportsARBNonPow2(void)
{
	return imp_supports_ARB_nonpow2;
}

bool R_glPrecacheTexture( RTex_t* rt )
{
	R_glDisableAllTMUs(0);
	
	R_glSet( 0, _dtf_always|_dwm_off|_cfm_none|
			_cwm_all|_va_off|_ca_off|_ia_off, _bm_off );
			
	R_glSetTMU( 0, TMU_0, rt, NO_CHANGE );
	R_glCommit(0);
		
	return FALSE;
}

bool R_glPrecacheTextures( void )
{
#if OPT_GL_PURGE == 0

	//
	// precache all the unbound textures.
	//
	RTex_t* rt;//, *next;
	int num_cached;
	LongWord start = ReadTickMilliseconds();
	
	glState_t* s = &def_state;

	Sys_printf("----\nPrecaching Textures...\n----\n");

	R_glDisableAllTMUs(0);
	
	R_glSet( 0, _dtf_always|_dwm_off|_cfm_none|
			_cwm_all|_va_off|_ca_off|_ia_off, _bm_off );
	
	R_glCommit( 0 );
	
	R_glSetActiveTMU( TMU_0 );

	num_cached = 0;
	/*for( rt = bound_list[0]; rt; rt = next)
	{
		next = rt->next;
		R_glSetTMU( 0, TMU_0, rt, NO_CHANGE );		
		CommitTMU( &s->tmus[TMU_0], &driver_state.tmus[TMU_0] );
		num_cached++;
	}*/
	
	while( unbound_list[0] )
	{	
		rt = unbound_list[0];
		R_glSetTMU( 0, TMU_0, rt, NO_CHANGE );	
		CommitTMU( &s->tmus[TMU_0], &driver_state.tmus[TMU_0] );
		num_cached++;
	}
	
	LongWord end = ReadTickMilliseconds();
	Sys_printf("precached: %d texture(s), %.2f seconds elapsed\n", num_cached, ((vec_t)end-start)/1000.0f);

#endif
	return FALSE;
}

void R_glLockArrays( int start, int end )
{ 
	if( imp_supports_EXT_compiled_vertex_array && use_lock_arrays )
	{
		if( locked_arrays && R_glEXT_UnlockArrays )
			R_glEXT_UnlockArrays();
		R_glEXT_LockArrays( start, end );
		locked_arrays = TRUE;
		CHECK_GL_ERRORS();
	}
}

void R_glUnlockArrays(void)
{
	if( imp_supports_EXT_compiled_vertex_array && use_lock_arrays )
	{
		if( locked_arrays && R_glEXT_UnlockArrays )
			R_glEXT_UnlockArrays();
		locked_arrays = FALSE;
		CHECK_GL_ERRORS();
	}
}

void R_glGetDriverState( glState_t* state )
{
	OS_ASSERT( state );
	*state = driver_state;
}

void R_glSetDriverState( glState_t* state )
{
	OS_ASSERT( state );
	driver_state = *state;
}

int R_glMaxTextureSize(void)
{
	int max_size;
	
	//glGetIntegerv( GL_MAX_TEXTURE_SIZE, (GLint *)&max_size );
	max_size = 256;//MIN_VAL( 256, max_size );

	return max_size;
}

void R_glResizeTextureForCard( int* width, int* height, int* type, int* components, void** data )
{
	int p2_w, p2_h;
	int out_type;
	
	p2_w = Util_PowerOf2( (*width) );
	p2_h = Util_PowerOf2( (*height) );
	
	//
	// too big for the card?
	//
	int max_size = R_glMaxTextureSize();
	
	p2_w = MIN_VAL( p2_w, max_size );
	p2_h = MIN_VAL( p2_h, max_size );
	
	out_type = *type;
	
	//
	// need to resize?
	//
	if( p2_w != *width || p2_h != *height )
	{
		void* in_data = *data; 
		void* out_data;
		
		int alloc_size;
		int num_comps;
		int bpp;
		
		OS_ASSERT( *components == GL_RGB || *components == GL_RGBA || *components == GL_LUMINANCE ||
				*components == GL_LUMINANCE_ALPHA || *components == GL_ALPHA );
		
		// select component size.
		switch( *components )
		{		
			case GL_RGB:
				num_comps = 3;
			break;
			
			case GL_RGBA:
				num_comps = 4;
			break;
			
			case GL_LUMINANCE:
			case GL_ALPHA:
				num_comps = 1;
			break;
			
			case GL_LUMINANCE_ALPHA:
				num_comps = 2;
			break;
			
			default:
				OS_BreakMsg("ERROR: R_glUploadTexture: invalid component type!\n");
		}
		
		// set bpp.
		switch( out_type )
		{
			case GL_FLOAT:
				bpp = num_comps*3;
			break;
			
			case GL_UNSIGNED_BYTE:
				bpp = num_comps;
			break;
			
			case GL_UNSIGNED_SHORT:
			case GL_UNSIGNED_INT:
				bpp = num_comps*2;
			break;
			
			case GL_UNSIGNED_BYTE_3_3_2:
			case GL_UNSIGNED_BYTE_2_3_3_REV:
				bpp = 1;
				OS_ASSERT( *components == GL_RGB );
			break;
				
			case GL_UNSIGNED_SHORT_5_6_5:
			case GL_UNSIGNED_SHORT_5_6_5_REV:
				bpp = 2;
				OS_ASSERT( *components == GL_RGB );
			break;
			
			case GL_UNSIGNED_SHORT_4_4_4_4:
			case GL_UNSIGNED_SHORT_4_4_4_4_REV:
			case GL_UNSIGNED_SHORT_5_5_5_1:
			case GL_UNSIGNED_SHORT_1_5_5_5_REV:
				bpp = 2;
				OS_ASSERT( *components == GL_RGBA );
			break;
			
			case GL_UNSIGNED_INT_8_8_8_8:
			case GL_UNSIGNED_INT_8_8_8_8_REV:
			case GL_UNSIGNED_INT_10_10_10_2:
			case GL_UNSIGNED_INT_2_10_10_10_REV:
				bpp = 4;
				OS_ASSERT( *components == GL_RGBA );
			break;
			
			default:
				OS_BreakMsg("ERROR: invalid type!\n" );
		}
	
		alloc_size = p2_w*p2_h*bpp;
		out_data = OS_AllocAPointer( alloc_size );
		if( !out_data )
		{
			OS_BreakMsg("R_glResizeTextureForCard: out of memory!");
			return;
		}	
		
		if( *components == GL_RGB && out_type == GL_UNSIGNED_BYTE )
		{
			Image_t in, out;

			memset( &in, 0, sizeof(in) );
			memset( &out, 0, sizeof(out) );

			in.Width = *width;
			in.Height = *height;
			in.DataType = IMAGE888;
			in.RowBytes = (*width)*bpp;
			in.ImagePtr = (Byte*)in_data;

			out.Width = p2_w;
			out.Height = p2_h;
			out.DataType = IMAGE888;
			out.RowBytes = p2_w*bpp;
			out.ImagePtr = (Byte*)out_data;

			ImageStore888( &out, &in );
		}
		else
		if( *components == GL_RGBA && out_type == GL_UNSIGNED_BYTE )
		{
			Image_t in, out;

			memset( &in, 0, sizeof(in) );
			memset( &out, 0, sizeof(out) );

			in.Width = *width;
			in.Height = *height;
			in.DataType = IMAGE8888;
			in.RowBytes = (*width)*bpp;
			in.ImagePtr = (Byte*)in_data;

			out.Width = p2_w;
			out.Height = p2_h;
			out.DataType = IMAGE8888;
			out.RowBytes = p2_w*bpp;
			out.ImagePtr = (Byte*)out_data;

			ImageStore8888( &out, &in );
		}
		else
		{
			gluScaleImage( *components, *width, *height, *type, in_data, p2_w, p2_h, out_type, out_data );
		}
		
		//
		// Free the in data.
		//
		OS_DeallocAPointer( in_data );
		
		*data = out_data;
		*width = p2_w;
		*height = p2_h;
		*type = out_type;
	}
}

OS_FNEXP RTex_t* R_glCreateTexture( const char* name, int width, int height, int depth, int bytes_per_pixel, RTexTouchProc load_proc, RTexTouchProc rebind_proc, RTexTouchProc purge_proc )
{
	RTex_t* tex;
	
	OS_ASSERT( load_proc );
	tex = (RTex_t*)OS_AllocAPointer( sizeof(RTex_t) );
	OS_ASSERT( tex );
	tex->load_proc = load_proc;
	tex->rebind_proc = rebind_proc;
	tex->purge_proc = purge_proc;
	tex->size = width*height*bytes_per_pixel;
	tex->frame = -1;
	tex->name = 0;
	tex->tmu_flags = 0;
	tex->user_data[0] = 0;
	tex->user_data[1] = 0;
	tex->target = ( depth > 1 ) ? GL_TEXTURE_3D : (height > 1 ) ? GL_TEXTURE_2D : GL_TEXTURE_1D;
	tex->width = width;
	tex->height = height;
	tex->depth = depth;
	tex->reload = false;
	tex->cache.handle = BAD_TC_HANDLE;
	tex->cache.key = BAD_TC_HANDLE;

	if( name && name[0] )
	{
		int len = strlen( name );
		
		len = CLAMP_VAL( len, 0, TEXTURE_NAME_LENGTH );
		strncpy( tex->str_name, name, len );
		tex->str_name[len] = 0;			
	}
	else
	{
		tex->str_name[0] = 0;
	}
	
	// add to end of unbound list.
	tex->prev = 0;
	tex->next = 0;
	
	tex->next = unbound_list[0];
	if(unbound_list[0])
		unbound_list[0]->prev = tex;
	
	unbound_list[0] = tex;
	
	if( unbound_list[1] == 0 )
		unbound_list[1] = tex;
	
	return tex;	
}

OS_FNEXP void R_glDeleteTexture( RTex_t* tex )
{
	// is it bound?
	if( tex->name )
		PurgeTexture( tex );
	
	//
	// remove from the unbound list.
	//
	if( tex->next )
		tex->next->prev = tex->prev;
	if( tex->prev )
		tex->prev->next = tex->next;
	
	if( tex == unbound_list[0] )
		unbound_list[0] = tex->next;
	if( tex == unbound_list[1] )
		unbound_list[1] = tex->prev;
		
	OS_DeallocAPointer(tex);	
}

#if defined(__OPT_WINOS__)
static void LoadGLVersion(void)
{ 
	int off;
	const char* s = (const char*)glGetString(GL_VERSION);
	char temp_string[1024];
	char* temp = temp_string;
	Word low, high;
	
	low = 0;
	high = 0;
	off = 0;
	
	// get first version #.
	while( s[off] == ' ' ) { off++; } // skip white space.
	
	// extract first.
	while( s[off] != '.' && s[off] != ' ' && s[off] != 0 )
	{
		*temp = s[off++];
		temp++;
	}
	*temp = 0;
	
	high = atoi( temp_string );
	if( s[off] != 0 )
	{
		OS_ASSERT( s[off] == '.' );
		off++;
		temp = temp_string;
		while( s[off] != '.' && s[off] != ' ' && s[off] != 0 )
		{
			*temp = s[off++];
			temp++;
		}
		*temp = 0;
		
		low = atoi( temp_string );
	}
	
	gl_version = MAKE_GL_VERSION( high, low);
}
#elif defined(__OPT_MACOS__)
static void LoadGLVersion(void)
{
	GLint minor=2, major=1;
	
	//aglGetVersion( &major, &minor );
	gl_version = MAKE_GL_VERSION( major, minor );
}
#endif

int R_glVersion()
{ return gl_version; }

int R_glMajorVersion()
{ return OS_HIGHWORD(gl_version); }

int R_glMinorVersion()
{ return OS_LOWWORD(gl_version); }

void R_glClearRangeElementCount()
{
	element_count = 0;
}

int  R_glRangeElementCount()
{ return element_count; }

//static int last_start=-1;
//static int last_end=-1;

void R_glDrawRangeElements( int mode, int start, int end, int count, int type, void* indices )
{
	/*if( last_start != start || last_end != end )
	{
		R_glUnlockArrays();
		last_start = start;
		last_end = end;
	}*/
	
	R_glLockArrays(start, end-start+1);
	
	if(R_glEXT_DrawRangeElements)
	{
		R_glEXT_DrawRangeElements( mode, start, end, count, type, indices );
	}
	else
	{
		glDrawElements( mode, count, type, indices );
	}
	
	//OS_ASSERT(type == GL_UNSIGNED_INT);
	
	/*glBegin(mode);
	for(int i = 0; i < count; i++)
		glArrayElement(((int*)indices)[i+start]);
	glEnd();*/
	
	if( do_element_count )
		element_count += count;
		
	CHECK_GL_ERRORS();
}

void R_glEnableStencil( glState_t* s, bool enabled )
{
	LSP( s );
	s->stencil_enabled = enabled;
}

void R_SetTMURGBCombineMode( glState_t* s, int tmu, int mode, int src0, int op0, int src1, int op1, int src2, int op2, float scale )
{
	LSP( s );
	OS_ASSERT( tmu < num_tmus );
	TmuState_t* t = &s->tmus[tmu];

	t->combine.rga_mode = mode;
	t->combine.src0_rgba = src0;
	t->combine.op0_rgba = op0;
	t->combine.src1_rgba = src1;
	t->combine.op1_rgba = op1;
	t->combine.src2_rgba = src2;
	t->combine.op2_rgba = op2;
	t->combine.rgba_scale = scale;
}

void R_SetTMUALPHACombineMode( glState_t* s, int tmu, int mode, int src0, int op0, int src1, int op1, int src2, int op2, float scale )
{
	LSP( s );
	OS_ASSERT( tmu < num_tmus );
	TmuState_t* t = &s->tmus[tmu];

	t->combine.alpha_mode = mode;
	t->combine.src0_alpha = src0;
	t->combine.op0_alpha = op0;
	t->combine.src1_alpha = src1;
	t->combine.op1_alpha = op1;
	t->combine.src2_alpha = src2;
	t->combine.op2_alpha = op2;
	t->combine.alpha_scale = scale;
}

void R_glStencilParms( glState_t* s, int func, int func_mask, int func_ref, int op_fail, int op_zfail, int op_zpass, int write_mask )
{
	LSP( s );
	
	if( func != NO_CHANGE )
		s->stencil_func = func;
	if( func_mask != NO_CHANGE )
		s->stencil_func_mask = func_mask;
	if( func_ref != NO_CHANGE )
		s->stencil_func_ref = func_ref;
	if( op_fail != NO_CHANGE )
		s->stencil_op_fail = op_fail;
	if( op_zfail != NO_CHANGE )
		s->stencil_op_zfail = op_zfail;
	if( op_zpass != NO_CHANGE )
		s->stencil_op_zpass = op_zpass;
	if( write_mask != NO_CHANGE )
		s->stencil_write_mask = write_mask;
}

bool R_glIsExtensionSupported( const char* ext )
{
	int i, k;
	int len_ext, len_glstring;
	const char* glstring;
	
	if(!ext || *ext == 0)
		return FALSE;
		
	len_ext = strlen( ext );
	
	glstring = (const char*)glGetString(GL_EXTENSIONS);
	if(!glstring || *glstring == 0)
		return FALSE;
		
	len_glstring = strlen( glstring );
	
	for(i = 0; i < len_glstring; i++)
	{
		if(glstring[i] == ' ')
			continue;
			
		/*
		found a char.
		compare with ext.
		mac doesn't have strcmp?
		*/
		for(k = 0; k < len_ext; k++)
		{
			if( tolower( ext[k] ) != tolower( glstring[i+k] ) )
				break;
		}
		
		if(k == len_ext)	/* found? */
		{
			/* 
			the next char in the glstring better
			be a space or a null otherwise we just
			were fooled by a substring.
			*/
			if( i+k == len_glstring || glstring[i+k] == ' ' )
				return TRUE;
		}
	}	
	
	return FALSE;
}

bool R_glImpSupportsARBMultiTexture( void )
{ return imp_supports_ARB_multitexture; }

bool R_glImpSupportsARBImaging( void )
{ return imp_supports_ARB_imaging; }

bool R_glImpSupportsEXTLockArrays( void )
{ return imp_supports_EXT_compiled_vertex_array; }

void R_glEnableRangeElementCount( bool enable )
{
	do_element_count = enable;
}

int R_glGetNumTMUs(void)
{ return num_tmus; }

int R_glGetNumImpTMUs(void)
{ return num_imp_tmus; }

bool R_glStartup( void )
{
	Sys_printf("R_glStartup...\n");
	Sys_printf("vendor: %s\n", glGetString(GL_VENDOR) );
	Sys_printf("renderer: %s\n", glGetString(GL_RENDERER) );
	Sys_printf("version: %s\n", glGetString(GL_VERSION) );
	//Sys_printf("extensions: %s\n", glGetString(GL_EXTENSIONS) );

	CHECK_GL_ERRORS();
	
	R_glEnableRangeElementCount(TRUE);
	
	// burger takes care of all the gl contexts for us.
	//
	// setup default gl states.
	//
	memset( &driver_state, 0, sizeof(driver_state) );
	memset( &def_state, 0, sizeof(def_state) );
	locked_arrays = FALSE;
	bound_list[0] = bound_list[1] = 0;
	unbound_list[0] = unbound_list[1] = 0;
	
	glDisable( GL_LIGHTING );
	glShadeModel( GL_SMOOTH );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	CHECK_GL_ERRORS();
	
	imp_supports_ARB_multitexture = R_glIsExtensionSupported( "GL_ARB_multitexture" );
	imp_supports_ARB_imaging = R_glIsExtensionSupported( "GL_ARB_imaging" );
	imp_supports_EXT_compiled_vertex_array = R_glIsExtensionSupported( "GL_EXT_compiled_vertex_array" );
	imp_supports_sgis_generate_mipmap = R_glIsExtensionSupported("GL_SGIS_generate_mipmap");
	imp_supports_s3tc_compression = R_glIsExtensionSupported("GL_EXT_texture_compression_s3tc") 
									&& R_glIsExtensionSupported("GL_ARB_texture_compression");
	imp_supports_fxt1_compression = R_glIsExtensionSupported("GL_3DFX_texture_compression_FXT1")
									&& R_glIsExtensionSupported("GL_ARB_texture_compression");
	imp_supports_ext_paletted_texture = R_glIsExtensionSupported( "GL_EXT_paletted_texture" ) 
										&& R_glIsExtensionSupported( "GL_EXT_shared_texture_palette" );
	imp_supports_ARB_nonpow2 = R_glIsExtensionSupported("GL_ARB_texture_non_power_of_two");

	CHECK_GL_ERRORS();
									
	if( R_glImpSupportsARBMultiTexture() )
	{
		glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, (GLint*)&num_tmus );
	}
	else
		num_tmus = 1;
	
	//
	// save before clamp.
	//
	num_imp_tmus = num_tmus;
	
	num_tmus = MIN_VAL(num_tmus, MAX_TMUS);
	
	LoadGLVersion();
	if( R_glVersion() < MAKE_GL_VERSION(MIN_MAJOR, MIN_MINOR) )
	{
		char temp_string[1024];
		sprintf(temp_string, "Game requires openGL version %d.%d", MIN_MAJOR, MIN_MINOR);
		OS_OkAlertMessage("Error", temp_string );
		return TRUE;
	}
	
	CHECK_GL_ERRORS();
	
	if( R_OS_glLoadExtensions() )
		return TRUE;
	
	CHECK_GL_ERRORS();
		
	// setup openGL states.
	R_glDisableAllTMUs( 0 );
	R_glSet(0, _dtf_always|_dwm_on|_cfm_none|_cwm_all|_no_arrays,
				_beq_add|_bm_off );
	
	R_glCommit(0);	// commit def_state.
	
	CHECK_GL_ERRORS();
	
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	
	if( R_glImpSupportsSGISGenerateMipmap() )
		glHint( GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST );
	
	CHECK_GL_ERRORS();
	
	use_lock_arrays=TRUE;
	
	/*Sys_printf("loading pals/bump.pal\n");

	if( LoadColorTable( "/pals/bump.pal" ) )
		return TRUE;
	else
	{
		Sys_printf("R_glStartup: loaded '%s' into global texture palette\n", "/pals/bump.pal" );
	}*/
	
	return FALSE;
}

bool R_glShutdown( void )
{
	RTex_t* bl[2], *ubl[2];
	
	bl[0] = bound_list[0];
	bl[1] = bound_list[1];
	ubl[0] = unbound_list[0];
	ubl[1] = unbound_list[1];
	
	/*OS_ASSERT( bound_list[0] == 0 );
	OS_ASSERT( bound_list[1] == 0 );
	OS_ASSERT( unbound_list[0] == 0 );
	OS_ASSERT( unbound_list[1] == 0 );*/
	
	FreeColorTable();
	
	return FALSE;
}

void R_glSetActiveTMU( int tmu )
{
	OS_ASSERT( tmu >= 0 && tmu < num_tmus );
	R_glEXT_ActiveTextureARB( GL_TEXTURE0_ARB+tmu );
	R_glEXT_ClientActiveTextureARB( GL_TEXTURE0_ARB+tmu );
}

void R_glSet( glState_t* s, int flags, int blends )
{
	LSP( s );
	if( flags != NO_CHANGE )
		s->state_flags = flags;
	if( blends != NO_CHANGE )
		s->blend_flags = blends;
}

void R_glAdd( glState_t* s, int flags, int blends )
{
	LSP( s );
	if( flags != NO_CHANGE )
		s->state_flags |= flags;
	if( flags != NO_CHANGE )
		s->blend_flags |= blends;
}

void R_glSub( glState_t* s, int flags, int blends )
{
	LSP( s );
	if( flags != NO_CHANGE )
		s->state_flags &= ~flags;
	if( flags != NO_CHANGE )
		s->blend_flags &= ~blends;
}

void R_glSetArray( glState_t* s, int array, int size, int type, int stride, void* data )
{
	LSP( s );
	OS_ASSERT( array < NUM_ARRAYS );
	glArrayState_t* a = &s->arrays[array];
	
	if( size != NO_CHANGE )
		a->size = size;
	if( type != NO_CHANGE )
		a->type = type;
	if( stride != NO_CHANGE )
		a->stride = stride;
	if( data != (void*)NO_CHANGE )
		a->array = data;
}

void R_glSetTMU( glState_t* s, int tmu, RTex_t* tex, int states )
{
	LSP( s );
	OS_ASSERT( tmu < num_tmus );
	TmuState_t* t = &s->tmus[tmu];
	
	if( tex != (RTex_t*)NO_CHANGE )
		t->tex = tex;
	if( states != NO_CHANGE )
		t->state_flags = states;
}

void R_glAddTMU( glState_t* s, int tmu, int states )
{
	LSP( s );
	OS_ASSERT( tmu < num_tmus );
	TmuState_t* t = &s->tmus[tmu];
	
	if( states != NO_CHANGE )
		t->state_flags |= states;
}

void R_glSubTMU( glState_t* s, int tmu, int states )
{
	LSP( s );
	OS_ASSERT( tmu < num_tmus );
	TmuState_t* t = &s->tmus[tmu];
	
	if( states != NO_CHANGE )
		t->state_flags &= ~states;
}

void R_glSetTMUArray( glState_t* s, int tmu, int array, int size, int type, int stride, void* data )
{
	LSP( s );
	OS_ASSERT( tmu < num_tmus );
	OS_ASSERT( array < NUM_TMU_ARRAYS );
	TmuState_t* t = &s->tmus[tmu];
	glArrayState_t* a = &t->arrays[array];
	
	if( size != NO_CHANGE )
		a->size = size;
	if( type != NO_CHANGE )
		a->type = type;
	if( stride != NO_CHANGE )
		a->stride = stride;
	if( data != (void*)NO_CHANGE )
		a->array = data;
}

void R_glDisableAllTMUs( glState_t* s )
{
	int i;
	for( i = 0; i < num_tmus; i++ )
	{
		R_glSetTMU( s, i, 0, _tca_off|_tem_replace );
	}
}

void R_glCommit( glState_t* s )
{
	LSP( s );
	glState_t* d = &driver_state;
	glArrayState_t* a;
	int sf, df;
	
	// configure main system....
	CHECK_GL_ERRORS();
	
	sf = s->state_flags;
	df = d->state_flags;
	
	// depth test.
	if( (sf&_dtf_flags) != 0 && (sf&_dtf_flags) != (df&_dtf_flags) )
	{
		// need to enable depth testing?
		if( df&_dtf_always )
			glEnable( GL_DEPTH_TEST );
		if( sf&_dtf_always )
			glDisable( GL_DEPTH_TEST );
		else
		{
			int m;
			switch( sf&_dtf_flags )
			{
				case _dtf_less: m = GL_LESS; break;
				case _dtf_greater : m = GL_GREATER; break;
				case _dtf_lequal : m = GL_LEQUAL; break;
				case _dtf_gequal : m = GL_GEQUAL; break;
				case _dtf_equal : m = GL_EQUAL; break;
				case _dtf_never : m = GL_NEVER; break;
			}
			glDepthFunc( m );
		}
		
		d->state_flags &= ~_dtf_flags;
		d->state_flags |= sf&_dtf_flags;
	}
	
	CHECK_GL_ERRORS();
	
	// depth write mask.
	if( (sf&_dwm_flags) != 0 && (sf&_dwm_flags) != (df&_dwm_flags) )
	{
		if( sf&_dwm_on )
			glDepthMask( GL_TRUE );
		else
			glDepthMask( GL_FALSE );
			
		d->state_flags &= ~_dwm_flags;
		d->state_flags |= sf&_dwm_flags;
	}
	
	CHECK_GL_ERRORS();
	
	// cull face.
	if( (sf&_cfm_flags) != 0 && (sf&_cfm_flags) != (df&_cfm_flags) )
	{
		if( df&_cfm_none )
			glEnable( GL_CULL_FACE );
		if( sf&_cfm_none )
			glDisable( GL_CULL_FACE );
		else
		{
			if( sf&_cfm_front )
				glCullFace( GL_FRONT );
			if( sf&_cfm_back )
				glCullFace( GL_BACK );
			
			if( sf&_cfm_front_cw )
				glFrontFace( GL_CW );
			if( sf&_cfm_front_ccw )
				glFrontFace( GL_CCW );
		}
		
		d->state_flags &= ~_cfm_flags;
		d->state_flags |= sf&_cfm_flags;
	}
	
	CHECK_GL_ERRORS();
	
	// color write mask.
	if( (sf&_cwm_flags) != 0 && (sf&_cwm_flags) != (df&_cwm_flags) )
	{
		bool r, g, b, a;
		
		r = (sf&_cwm_r) ? GL_TRUE : GL_FALSE;
		g = (sf&_cwm_g) ? GL_TRUE : GL_FALSE;
		b = (sf&_cwm_b) ? GL_TRUE : GL_FALSE;
		a = (sf&_cwm_a) ? GL_TRUE : GL_FALSE;
		
		glColorMask( r, g, b, a );
		
		d->state_flags &= ~_cwm_flags;
		d->state_flags |= sf&_cwm_flags;
	}
	
	CHECK_GL_ERRORS();
	
	// normal array.
	if( (sf&_na_flags) != 0 && (s->state_flags&_na_flags) != (df&_na_flags) )
	{
		if( df&_na_off )
			glEnableClientState( GL_NORMAL_ARRAY );
		if( sf&_na_off )
			glDisableClientState( GL_NORMAL_ARRAY );
			
		d->state_flags &= ~_na_flags;
		d->state_flags |= sf&_na_flags;
	}
	
	CHECK_GL_ERRORS();
	
	a = &s->arrays[_normal];
	if( (d->state_flags&_na_on) && !ARRAY_STATES_EQUAL( a, &d->arrays[_normal] ) )
	{
		//R_glUnlockArrays();
		glNormalPointer( a->type, a->stride, a->array );
		d->arrays[_normal] = *a;
	}

	CHECK_GL_ERRORS();
	
	// vertex array.
	if( (sf&_va_flags) != 0 && (s->state_flags&_va_flags) != (df&_va_flags) )
	{
		if( df&_va_off )
			glEnableClientState( GL_VERTEX_ARRAY );
		if( sf&_va_off )
			glDisableClientState( GL_VERTEX_ARRAY );
			
		d->state_flags &= ~_va_flags;
		d->state_flags |= sf&_va_flags;
	}
	
	CHECK_GL_ERRORS();
	
	a = &s->arrays[_vertex];
	if( (d->state_flags&_va_on) && !ARRAY_STATES_EQUAL( a, &d->arrays[_vertex] ) )
	{
		//R_glUnlockArrays();
		glVertexPointer( a->size, a->type, a->stride, a->array );
		d->arrays[_vertex] = *a;
	}
	
	CHECK_GL_ERRORS();
	
	// index array.
	if( (sf&_ia_flags) != 0 && (sf&_ia_flags) != (df&_ia_flags) )
	{
		if( df&_ia_off )
			glEnableClientState( GL_INDEX_ARRAY );
		if( sf&_ia_off )
			glDisableClientState( GL_INDEX_ARRAY );
		
		d->state_flags &= ~_ia_flags;
		d->state_flags |= sf&_ia_flags;
	}
	
	CHECK_GL_ERRORS();
	
	a = &s->arrays[_index];
	if( (d->state_flags&_ia_on) && !ARRAY_STATES_EQUAL( a, &d->arrays[_index] ) )
	{
		//R_glUnlockArrays();
		glIndexPointer( a->type, a->stride, a->array );
		d->arrays[_index] = *a;
	}
	
	CHECK_GL_ERRORS();
	
	// color array.
	if( (sf&_ca_flags) != 0 && (sf&_ca_flags) != (df&_ca_flags) )
	{
		if( df&_ca_off )
			glEnableClientState( GL_COLOR_ARRAY );
		if( sf&_ca_off )
			glDisableClientState( GL_COLOR_ARRAY );
			
		d->state_flags &= ~_ca_flags;
		d->state_flags |= sf&_ca_flags;
	}
	
	CHECK_GL_ERRORS();
	
	// normal.
	a = &s->arrays[_color];
	if( (d->state_flags&_ca_on) && !ARRAY_STATES_EQUAL( a, &d->arrays[_color] ) )
	{
		//R_glUnlockArrays();
		glColorPointer( a->size, a->type, a->stride, a->array );
		d->arrays[_color] = *a;
	}
	
	CHECK_GL_ERRORS();
	
	// interleaved.
	a = &s->arrays[_interleaved];
	if( !ARRAY_STATES_EQUAL( a, &d->arrays[_interleaved] ) )
	{
		//R_glUnlockArrays();
		glInterleavedArrays( a->type, a->stride, a->array );
		d->arrays[_interleaved] = *a;
	}
	
	CHECK_GL_ERRORS();
	
	// blend equation.
	sf = s->blend_flags;
	df = d->blend_flags;
	
	if( R_glImpSupportsARBImaging() )
	{
		if( (sf&_beq_flags) != 0 && (sf&_beq_flags) != (df&_beq_flags) )
		{
			int m;
			switch( sf&_beq_flags )
			{
				case _beq_add: m = GL_FUNC_ADD; break;
				case _beq_sub: m = GL_FUNC_SUBTRACT; break;
				case _beq_rev_sub: m = GL_FUNC_REVERSE_SUBTRACT; break;
				case _beq_min: m = GL_MIN; break;
				case _beq_max: m = GL_MAX; break;
			}
			
			R_glEXT_BlendEquation( m );
			
			d->blend_flags &= ~_beq_flags;
			d->blend_flags |= sf&_beq_flags;
		}
	}
	
	CHECK_GL_ERRORS();
	
	if( (sf&(_bms_flags|_bmd_flags)) != 0 && (sf&(_bms_flags|_bmd_flags)) != (df&(_bms_flags|_bmd_flags)) )
	{
		// off
		if( (df&_bm_off) == _bm_off )
			glEnable( GL_BLEND );
		if( (sf&_bm_off) == _bm_off )
		{
			d->blend_flags &= ~(_bms_flags|_bmd_flags);
			d->blend_flags |=_bm_off;
			glDisable( GL_BLEND );
		}
		else
		{
			int sb, db;
			int f;
			
			f = (sf&_bms_flags) ? sf : df;
			f &= _bms_flags;
			
			switch( f )
			{
				case _bms_one: sb = GL_ONE; break;
				case _bms_dst_color: sb = GL_DST_COLOR; break;
				case _bms_inv_dst_color: sb = GL_ONE_MINUS_DST_COLOR; break;
				case _bms_src_alpha: sb = GL_SRC_ALPHA; break;
				case _bms_inv_src_alpha: sb = GL_ONE_MINUS_SRC_ALPHA; break;
				case _bms_dst_alpha: sb = GL_DST_ALPHA; break;
				case _bms_inv_dst_alpha: sb = GL_ONE_MINUS_DST_ALPHA; break;
				case _bms_src_alpha_saturate: sb = GL_SRC_ALPHA_SATURATE; break;
				case _bms_zero: sb = GL_ZERO; break;
				case _bms_constant_color: sb = GL_CONSTANT_COLOR; break;
				case _bms_inv_constant_color: sb = GL_ONE_MINUS_CONSTANT_COLOR; break;
				case _bms_constant_alpha: sb = GL_CONSTANT_ALPHA; break;
				case _bms_inv_constant_alpha: sb = GL_ONE_MINUS_CONSTANT_ALPHA; break;
			}
			
			d->blend_flags &= ~_bms_flags;
			d->blend_flags |= f;
			
			f = (sf&_bmd_flags) ? sf : df;
			f &= _bmd_flags;
			
			switch( f )
			{
				case _bmd_one: db = GL_ONE; break;
				case _bmd_src_color: db = GL_SRC_COLOR; break;
				case _bmd_inv_src_color: db = GL_ONE_MINUS_SRC_COLOR; break;
				case _bmd_src_alpha: db = GL_SRC_ALPHA; break;
				case _bmd_inv_src_alpha: db = GL_ONE_MINUS_SRC_ALPHA; break;
				case _bmd_dst_alpha: db = GL_DST_ALPHA; break;
				case _bmd_inv_dst_alpha: db = GL_ONE_MINUS_DST_ALPHA; break;
				case _bmd_zero: db = GL_ZERO; break;
				case _bmd_constant_color: db = GL_CONSTANT_COLOR; break;
				case _bmd_inv_constant_color: db = GL_ONE_MINUS_CONSTANT_COLOR; break;
				case _bmd_constant_alpha: db = GL_CONSTANT_ALPHA; break;
				case _bmd_inv_constant_alpha: db = GL_ONE_MINUS_CONSTANT_ALPHA; break;
			}
		
			d->blend_flags &= ~_bmd_flags;
			d->blend_flags |= f;
				
			glBlendFunc( sb, db );
		}
	}
	
	CHECK_GL_ERRORS();
	
	// stencil test.
	if( s->stencil_enabled != d->stencil_enabled )
	{
		if( d->stencil_enabled == FALSE )
			glEnable(GL_STENCIL_TEST);
		if( s->stencil_enabled == FALSE )
			glDisable(GL_STENCIL_TEST);
			
		d->stencil_enabled = s->stencil_enabled;
	}
	
	CHECK_GL_ERRORS();
		
	if( s->stencil_func != d->stencil_func ||
		s->stencil_func_ref != d->stencil_func_ref ||
		s->stencil_func_mask != d->stencil_func_mask )
	{
		glStencilFunc( s->stencil_func, s->stencil_func_ref, s->stencil_func_mask );
		d->stencil_func = s->stencil_func;
		d->stencil_func_ref = s->stencil_func_ref;
		d->stencil_func_mask = s->stencil_func_mask;
	}
	
	CHECK_GL_ERRORS();
	
	if( s->stencil_write_mask != d->stencil_write_mask )
	{
		glStencilMask( s->stencil_write_mask );
		d->stencil_write_mask = s->stencil_write_mask;
	}
	
	CHECK_GL_ERRORS();
	
	if( s->stencil_op_fail != d->stencil_op_fail ||
		s->stencil_op_zfail != d->stencil_op_zfail ||
		s->stencil_op_zpass != d->stencil_op_zpass )
	{
		glStencilOp( s->stencil_op_fail, s->stencil_op_zfail, s->stencil_op_zpass );
		d->stencil_op_fail = s->stencil_op_fail;
		d->stencil_op_zfail = s->stencil_op_zfail;
		d->stencil_op_zpass = s->stencil_op_zpass;
	}
	
	CHECK_GL_ERRORS();
	
	// set each tmu.
	int i;
	for(i = 0; i < num_tmus; i++)
	{
		R_glSetActiveTMU( i );
		CommitTMU( &s->tmus[i], &driver_state.tmus[i] );
	}
	
	CHECK_GL_ERRORS();
}

// when we enter here the active TMU has already been set.
static void CommitTMU( TmuState_t* tmu, TmuState_t* drv )
{
	int df = drv->state_flags;
	int sf = tmu->state_flags;
	glArrayState_t* a;

	// did the texture change?
	if( tmu->tex != drv->tex )
	{
		//
		// if the targets don't match...
		//
		if( drv->tex && tmu->tex && ( drv->target != tmu->tex->target ) )
		{	
			glDisable( drv->target );
			
			CHECK_GL_ERRORS();
			
			drv->target = tmu->tex->target;
			glEnable( drv->target );
			
			CHECK_GL_ERRORS();
		}
		
		if( drv->tex == 0 )
		{
			drv->target = tmu->tex->target;
			glEnable( drv->target );
			CHECK_GL_ERRORS();
			
		}
		else
		{
			// unbind current texture.
			//OS_ASSERT( drv->tex->tmu_flags&TMU_FLAG(active_tmu) );
			//drv->tex->tmu_flags &= ~TMU_FLAG(active_tmu);
		}
		
		if( tmu->tex == 0 )
		{
			glDisable( drv->target );
			drv->target = 0;
			CHECK_GL_ERRORS();
		}
		
		drv->tex = tmu->tex;
	}
	
	// update it... and mark it as accessed again.
	if( drv->tex )
		SetTexture( drv->tex );
	
	CHECK_GL_ERRORS();
	
	// change texture env...
	if( ((df&_tem_flags) != (sf&_tem_flags)) && (sf&_tem_flags) != 0 )
	{
		int mode;
		
		switch( sf&_tem_flags )
		{
			case _tem_modulate:	mode = GL_MODULATE;	break;
			case _tem_decal: mode = GL_DECAL; break;
			case _tem_blend: mode = GL_BLEND; break;
			case _tem_replace: mode = GL_REPLACE; break;
			case _tem_combine: mode = GL_COMBINE_ARB; break;
			default: OS_BreakMsg("Unsupported texture environment!"); break;
		}
		
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode );
		drv->state_flags &= ~_tem_flags;
		drv->state_flags |= sf&_tem_flags;
	}

	if( drv->state_flags&_tem_combine )
	{
		//
		// setup combiner modes
		//
		if( (tmu->combine.rga_mode != 0 ) && (drv->combine.rga_mode != tmu->combine.rga_mode) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, tmu->combine.rga_mode );
			drv->combine.rga_mode = tmu->combine.rga_mode;
		}
		if( (tmu->combine.alpha_mode != 0 ) && (drv->combine.alpha_mode != tmu->combine.alpha_mode) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, tmu->combine.alpha_mode );
			drv->combine.alpha_mode = tmu->combine.alpha_mode;
		}

		//
		// RGB setup
		//
		if( (tmu->combine.src0_rgba != 0 ) && (drv->combine.src0_rgba != tmu->combine.src0_rgba) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, tmu->combine.src0_rgba );
			drv->combine.src0_rgba = tmu->combine.src0_rgba;
		}
		if( (tmu->combine.op0_rgba != 0 ) && (drv->combine.op0_rgba != tmu->combine.op0_rgba) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, tmu->combine.op0_rgba );
			drv->combine.op0_rgba = tmu->combine.op0_rgba;
		}
		if( (tmu->combine.src1_rgba != 0 ) && (drv->combine.src1_rgba != tmu->combine.src1_rgba) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, tmu->combine.src1_rgba );
			drv->combine.src1_rgba = tmu->combine.src1_rgba;
		}
		if( (tmu->combine.op1_rgba != 0 ) && (drv->combine.op1_rgba != tmu->combine.op1_rgba) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, tmu->combine.op1_rgba );
			drv->combine.op1_rgba = tmu->combine.op1_rgba;
		}
		if( (tmu->combine.src2_rgba != 0 ) && (drv->combine.src2_rgba != tmu->combine.src2_rgba) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, tmu->combine.src2_rgba );
			drv->combine.src2_rgba = tmu->combine.src2_rgba;
		}
		if( (tmu->combine.op2_rgba != 0 ) && (drv->combine.op2_rgba != tmu->combine.op2_rgba) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, tmu->combine.op2_rgba );
			drv->combine.op2_rgba = tmu->combine.op2_rgba;
		}

		//
		// ALPHA setup
		//
		if( (tmu->combine.src0_alpha != 0 ) && (drv->combine.src0_alpha != tmu->combine.src0_alpha) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, tmu->combine.src0_alpha );
			drv->combine.src0_alpha = tmu->combine.src0_alpha;
		}
		if( (tmu->combine.op0_alpha != 0 ) && (drv->combine.op0_alpha != tmu->combine.op0_alpha) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, tmu->combine.op0_alpha );
			drv->combine.op0_alpha = tmu->combine.op0_alpha;
		}
		if( (tmu->combine.src1_alpha != 0 ) && (drv->combine.src1_alpha != tmu->combine.src1_alpha) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, tmu->combine.src1_alpha );
			drv->combine.src1_alpha = tmu->combine.src1_alpha;
		}
		if( (tmu->combine.op1_alpha != 0 ) && (drv->combine.op1_alpha != tmu->combine.op1_alpha) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, tmu->combine.op1_alpha );
			drv->combine.op1_alpha = tmu->combine.op1_alpha;
		}
		if( (tmu->combine.src2_alpha != 0 ) && (drv->combine.src2_alpha != tmu->combine.src2_alpha) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB, tmu->combine.src2_alpha );
			drv->combine.src2_alpha = tmu->combine.src2_alpha;
		}
		if( (tmu->combine.op2_alpha != 0 ) && (drv->combine.op2_alpha != tmu->combine.op2_alpha) )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_ARB, tmu->combine.op2_alpha );
			drv->combine.op2_alpha = tmu->combine.op2_alpha;
		}

		//
		// scale.
		//
		if( drv->combine.rgba_scale != tmu->combine.rgba_scale )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, tmu->combine.rgba_scale );
			drv->combine.rgba_scale = tmu->combine.rgba_scale;
		}
		if( drv->combine.alpha_scale != tmu->combine.alpha_scale )
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_ALPHA_SCALE, tmu->combine.alpha_scale );
			drv->combine.alpha_scale = tmu->combine.alpha_scale;
		}
	}
	
	CHECK_GL_ERRORS();
	
	// texture coords.
	if( (df&_tca_flags) != (sf&_tca_flags) && (sf&_tca_flags) != 0 )
	{
		// switched tex coord on/off.
		if( sf&_tca_on )
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		else
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			
		drv->state_flags &= ~_tca_flags;
		drv->state_flags |= sf&_tca_flags;
	}
	
	CHECK_GL_ERRORS();
	
	// did the tex array change.
	a = &tmu->arrays[_texcoord];
	if( (drv->state_flags&_tca_on) && !ARRAY_STATES_EQUAL( a, &drv->arrays[_texcoord] ) )
	{
		//R_glUnlockArrays();
		glTexCoordPointer( a->size, a->type, a->stride, a->array );
		drv->arrays[_texcoord] = *a; // copy it.
	}
	
	CHECK_GL_ERRORS();
}

//
// uploads a texture into a cube-map.
//
OS_FNEXP void R_glUploadTexture_CubeMap( int target, RTex_t* tex, int width, int height, int depth, int type, int components, int flags, void* data, void** mips )
{
	//OS_ASSERT( width <= MAX_TEXTURE_WIDTH && height <= MAX_TEXTURE_WIDTH && depth <= MAX_TEXTURE_WIDTH );
	OS_ASSERT( (width&(width-1)) == 0 );
	OS_ASSERT( (height&(height-1)) == 0 );
	OS_ASSERT( (depth&(depth-1)) == 0 );
	OS_ASSERT( tex );
	OS_ASSERT( type );
	OS_ASSERT( components );
	OS_ASSERT( data );
	OS_ASSERT( tex->name );
		
	OS_ASSERT( depth == 1 );
	
	OS_ASSERT( components == GL_RGB || components == GL_RGBA );
	
	OS_ASSERT( tex->target == GL_TEXTURE_CUBE_MAP_ARB );
	OS_ASSERT( (target == GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB) ||
			   (target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB) ||
			   (target == GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB) ||
			   (target == GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB) ||
			   (target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB) ||
			   (target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB) );
	
	int int_format;
	int driver_size;
	int num_comps;
	int bpp;
	
	CHECK_GL_ERRORS();
	
	// select component size.
	switch( components )
	{		
		case GL_RGB:   
			num_comps = 3;
		break;
		
		case GL_RGBA:
			num_comps = 4;
		break;
		
		case GL_LUMINANCE:
		case GL_ALPHA:
			num_comps = 1;
		break;
		
		case GL_LUMINANCE_ALPHA:
			num_comps = 2;
		break;
		
		default:
			OS_BreakMsg("ERROR: R_glUploadTexture_CubeMap: invalid component type!\n");
	}
	
	// set bpp.
	switch( type )
	{
		case GL_FLOAT:
			bpp = num_comps*3;
		break;
		
		case GL_UNSIGNED_BYTE:
			bpp = num_comps;
		break;
		
		case GL_UNSIGNED_SHORT:
		case GL_UNSIGNED_INT:
			bpp = num_comps*2;
		break;
		
		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:
			bpp = 1;
			OS_ASSERT( components == GL_RGB );
		break;
		
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
			bpp = 2;
			OS_ASSERT( components == GL_RGB );
		break;
		
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			bpp = 2;
			OS_ASSERT( components == GL_RGBA );
		break;
		
		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			bpp = 4;
			OS_ASSERT( components == GL_RGBA );
		break;
		
		default:
			OS_BreakMsg("ERROR: invalid type!\n" );
	}
	
	// pick an internal format that best suits each type.
	switch( type )
	{
		case GL_FLOAT:
		{
			int_format = components;
		}
		break;
		
		case GL_UNSIGNED_BYTE:
		{
			switch( components )
			{
				case GL_RGBA:
					int_format = GL_RGBA8;
				break;
				case GL_RGB:
					int_format = GL_RGB8;
				break;
				case GL_LUMINANCE:
					int_format = GL_LUMINANCE8;
				break;
				case GL_LUMINANCE_ALPHA:
					int_format = GL_LUMINANCE8_ALPHA8;
				break;
				case GL_ALPHA:
					int_format = GL_ALPHA8;
				break;
			}
		}		
		break;
		
		// we max out our size at SHORT so INT is useless basically.
		case GL_UNSIGNED_SHORT:
		case GL_UNSIGNED_INT:
		{
			switch( components )
			{
				case GL_RGBA:
					int_format = GL_RGBA16;
				break;
				case GL_RGB:
					int_format = GL_RGB16;
				break;
				case GL_LUMINANCE:
					int_format = GL_LUMINANCE16;
				break;
				case GL_LUMINANCE_ALPHA:
					int_format = GL_LUMINANCE16_ALPHA16;
				break;
				case GL_ALPHA:
					int_format = GL_ALPHA16;
				break;
			}
		}	
		break;
		
		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:
			int_format = GL_R3_G3_B2;
		break;
			
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
			int_format = GL_RGB8;
		break;
		
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
			int_format = GL_RGBA4;
		break;
		
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			int_format = GL_RGB5_A1;
		break;
		
		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
			int_format = GL_RGBA8;
		break;
		
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			int_format = GL_RGB10_A2;
		break;
	}
	
	//
	// if we are allowing compression, then tweak the internal format.
	// some cards don't support 3d texture compression.
	//
	if( (flags&_upf_compress) && (depth==1) )
	{
		if( R_glImpSupportsS3TCCompression() )
		{
			switch( components )
			{
				case GL_RGB:
					int_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				break;
				case GL_RGBA:
					int_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
			}
		}
	}
	
	driver_size = width*height*depth*bpp; // six faces!
	if( flags&_upf_mipmap )
		driver_size += (driver_size+2)/3;
	
	if( int_format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
		int_format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
	{
		driver_size >>= 2;
	}
	
	// at this point we should already be attached to a tmu ( or tmus )
	// purge space for us.
	PurgeTextureSpace( driver_size );
	
	alloced_size += driver_size;
	tex->size += driver_size;
		
	// set texture params.
	if( flags&_upf_wrap )
	{
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_R, GL_REPEAT );
	}
	else
	{
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	}
	
	if( flags&_upf_mipmap )
	{
		glTexParameteri( tex->target, GL_TEXTURE_BASE_LEVEL, 0 );
		CHECK_GL_ERRORS();
		glTexParameteri( tex->target, GL_TEXTURE_MAX_LEVEL, MAX_MIPMAP_LEVELS-2 );
		CHECK_GL_ERRORS();
	}
	
	if( flags&_upf_filter )
	{
		if( flags&_upf_mipmap )
		{
			if( (flags&_upf_trilinear) )
				glTexParameteri( tex->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			else
				glTexParameteri( tex->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );

			glTexParameteri( tex->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}
		else
		{
			glTexParameteri( tex->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( tex->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}
	}
	else
	{
		if( flags&_upf_mipmap )
		{
			glTexParameteri( tex->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
			glTexParameteri( tex->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		}
		else
		{
			glTexParameteri( tex->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( tex->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		}
	}
	
	CHECK_GL_ERRORS();
	
	// upload the texture.
	if( flags&_upf_mipmap )
	{
		if( mips )
		{
			int i;
			int mipwidth, mipheight;
			
			mipwidth = width;
			mipheight = height;
			
			for(i = 0; i < MAX_MIPMAP_LEVELS; i++)
			{
				OS_ASSERT( mips[i] );
					
				glTexImage2D( target, i, int_format, mipwidth, mipheight, 0, components, type, mips[i] );
				
				mipwidth >>= 1;
				mipheight >>= 1;
				
				if( mipwidth < 1 || mipheight < 1 )
					break;
			}
		}
		else
		{
			gluBuild2DMipmaps( target, int_format, width, height, components, type, data );
		}
	}
	else
	{
		glTexImage2D( target, 0, int_format, width, height, 0, components, type, data );
	}
	
	CHECK_GL_ERRORS();
}

OS_FNEXP void R_glUploadSubTexture2D( RTex_t* tex, int xofs, int yofs, int level, int width, int height, int type, int components, void* data)
{
	OS_ASSERT( (width&(width-1)) == 0 );
	OS_ASSERT( (height&(height-1)) == 0 );
	OS_ASSERT( tex );
	OS_ASSERT( type );
	OS_ASSERT( components );
	OS_ASSERT( data );
	OS_ASSERT( tex->name );
	//OS_ASSERT( (tex->tmu_flags&TMU_FLAG(active_tmu)) );
		
	CHECK_GL_ERRORS();
	
	OS_ASSERT( components == GL_RGB || components == GL_RGBA || components == GL_LUMINANCE ||
			components == GL_LUMINANCE_ALPHA || components == GL_ALPHA || components == GL_COLOR_INDEX );
	
	OS_ASSERT( tex->target != GL_TEXTURE_CUBE_MAP_EXT );
	
	// upload the texture.
	if( tex->target == GL_TEXTURE_2D )
	{
	
		glTexSubImage2D( GL_TEXTURE_2D, 0, xofs, yofs, width, height, components, type, data );
				
	}
	
	CHECK_GL_ERRORS();
}

//
// this texture should already be bound
//
OS_FNEXP void R_glUploadTexture( RTex_t* tex, int width, int height, int depth, int type, int components, int flags, void* data, void** mips )
{
	//OS_ASSERT( width <= MAX_TEXTURE_WIDTH && height <= MAX_TEXTURE_WIDTH && depth <= MAX_TEXTURE_WIDTH );
//	OS_ASSERT( (width&(width-1)) == 0 );
//	OS_ASSERT( (height&(height-1)) == 0 );
//	OS_ASSERT( (depth&(depth-1)) == 0 );
	OS_ASSERT( tex );
	OS_ASSERT( type );
	OS_ASSERT( components );
	OS_ASSERT( data );
	OS_ASSERT( tex->name );
	//OS_ASSERT( (tex->tmu_flags&TMU_FLAG(active_tmu)) );
		
	int int_format;
	int driver_size;
	int num_comps;
	int bpp;
	//int hd = R_HD_GetHardwareCode();
	
	CHECK_GL_ERRORS();
	
	OS_ASSERT( components == GL_RGB || components == GL_RGBA || components == GL_LUMINANCE ||
			components == GL_LUMINANCE_ALPHA || components == GL_ALPHA || components == GL_COLOR_INDEX );
	
	OS_ASSERT( tex->target != GL_TEXTURE_CUBE_MAP_EXT );
	
	// select component size.
	switch( components )
	{		
		case GL_RGB:
			num_comps = 3;
		break;
		
		case GL_RGBA:
			num_comps = 4;
		break;
		
		case GL_LUMINANCE:
		case GL_ALPHA:
		case GL_COLOR_INDEX:
			num_comps = 1;
		break;
		
		case GL_LUMINANCE_ALPHA:
			num_comps = 2;
		break;
		
		default:
			OS_BreakMsg("ERROR: R_glUploadTexture: invalid component type!\n");
	}
	
	// set bpp.
	switch( type )
	{
		case GL_FLOAT:
			bpp = num_comps*3;
		break;
		
		case GL_UNSIGNED_BYTE:
			bpp = num_comps;
		break;
		
		case GL_UNSIGNED_SHORT:
		case GL_UNSIGNED_INT:
			bpp = num_comps*2;
		break;
		
		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:
			bpp = 1;
			OS_ASSERT( components == GL_RGB );
		break;
			
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
			bpp = 2;
			OS_ASSERT( components == GL_RGB );
		break;
		
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			bpp = 2;
			OS_ASSERT( components == GL_RGBA );
		break;
		
		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			bpp = 4;
			OS_ASSERT( components == GL_RGBA );
		break;
		
		default:
			OS_BreakMsg("ERROR: invalid type!\n" );
	}
	
	// pick an internal format that best suits each type.
	switch( type )
	{
		case GL_FLOAT:
		{
			int_format = components;
		}
		break;
		
		case GL_UNSIGNED_BYTE:
		{
			switch( components )
			{
				case GL_RGBA:
					int_format = GL_RGBA8;
				break;
				case GL_RGB:
					int_format = GL_RGB8;
				break;
				case GL_LUMINANCE:
					int_format = GL_LUMINANCE8;
				break;
				case GL_LUMINANCE_ALPHA:
					int_format = GL_LUMINANCE8_ALPHA8;
				break;
				case GL_ALPHA:
					int_format = GL_ALPHA8;
				break;
				case GL_COLOR_INDEX:
					int_format = GL_COLOR_INDEX8_EXT;
				break;
			}
		}		
		break;
		
		// we max out our size at SHORT so INT is useless basically.
		case GL_UNSIGNED_SHORT:
		case GL_UNSIGNED_INT:
		{
			switch( components )
			{
				case GL_RGBA:
					int_format = GL_RGBA16;
				break;
				case GL_RGB:
					int_format = GL_RGB16;
				break;
				case GL_LUMINANCE:
					int_format = GL_LUMINANCE16;
				break;
				case GL_LUMINANCE_ALPHA:
					int_format = GL_LUMINANCE16_ALPHA16;
				break;
				case GL_ALPHA:
					int_format = GL_ALPHA16;
				break;
			}
		}	
		break;
		
		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:
			int_format = GL_R3_G3_B2;
		break;
			
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
			int_format = GL_RGB8;
		break;
		
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
			int_format = GL_RGBA4;
		break;
		
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			int_format = GL_RGB5_A1;
		break;
		
		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
			int_format = GL_RGBA8;
		break;
		
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			int_format = GL_RGB10_A2;
		break;
	}
	
	if( components == GL_COLOR_INDEX )
		flags &= ~_upf_compress;
		
	//
	// if we are allowing compression, then tweak the internal format.
	// some cards don't support 3d texture compression.
	//
	if( (flags&_upf_compress) && (depth==1) )
	{
		if( R_glImpSupportsS3TCCompression() )
		{
			switch( components )
			{
				case GL_RGB:
					int_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				break;
				case GL_RGBA:
					int_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
			}
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//
	// rescale the image if necessary.
	// buggy drivers.
	bool rescaled = false;
	/*{
		const float MAX_ASPECT = 5.0f;
		float fw = width;
		float fh = height;
		float aspect = fh / fw;
		if (aspect > MAX_ASPECT)
		{
			fw *= aspect / MAX_ASPECT;
		}
		else
		{
			aspect = fw / fh;
			if (aspect > MAX_ASPECT)
			{
				fh *= aspect / MAX_ASPECT;
			}
		}

		int nw = fw;
		int nh = fh;

		if (nw != width || nh != height)
		{
			rescaled = true;
			mips = 0;
			unsigned char *dst = new unsigned char[nw * nh * bpp];
			gluScaleImage(components, width, height, type, data, nw, nh, type, dst);
			width = nw;
			height = nh;
			data = dst;
		}
	}*/
	
	driver_size = width*height*depth*bpp;
	if( flags&_upf_mipmap )
		driver_size += (driver_size+2)/3;
	
	if( int_format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
		int_format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
	{
		driver_size >>= 2;
	}
	
	// at this point we should already be attached to a tmu ( or tmus )
	// purge space for us.
	PurgeTextureSpace( driver_size );
	
	CHECK_GL_ERRORS();
	
	OS_ASSERT( (depth>1)?(tex->target==GL_TEXTURE_3D):((tex->target==GL_TEXTURE_2D)||(tex->target==GL_TEXTURE_1D)) );
	
	//
	// the target should be set by CreateTexture!
	//
		
	tex->size = driver_size;
		
	alloced_size += driver_size;
	
	// set texture params.
	if( flags&_upf_wrap )
	{
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_R, GL_REPEAT );
	}
	else
	{
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( tex->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	}
	
	CHECK_GL_ERRORS();
	
	//
	// make sure we can use them... indexed images must have the mips[] present.
	//
	if( components == GL_COLOR_INDEX && !mips )
		flags &= ~_upf_mipmap;

	if( flags&_upf_filter )
	{
		if( flags&_upf_mipmap )
		{
			if( (flags&_upf_trilinear) )
				glTexParameteri( tex->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			else
			glTexParameteri( tex->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
				
			glTexParameteri( tex->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}
		else
		{
			glTexParameteri( tex->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( tex->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}
	}
	else
	{
		if( flags&_upf_mipmap )
		{
		
			glTexParameteri( tex->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
			glTexParameteri( tex->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		}
		else
		{
			glTexParameteri( tex->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( tex->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		}
	}
	
	CHECK_GL_ERRORS();
	
	// upload the texture.
	if( tex->target == GL_TEXTURE_3D )
	{
		OS_ASSERT( R_glEXT_TexImage3D );
		
		OS_ASSERT( int_format != GL_COLOR_INDEX8_EXT );
		
		//
		// make sure we're running at least GL 1.2
		//
		if( R_glVersion() < MAKE_GL_VERSION(1,2) )
			OS_BreakMsg( "3D textures were attempted to be used on a non 1.2 gl compatible implementation!" );
		
		//
		// 3d texture.
		//
		if( flags&_upf_mipmap )
		{
			//gluBuild3DMipmaps( GL_TEXTURE_3D, int_format, width, height, depth, components, type, data );
		}
		else
		{
			R_glEXT_TexImage3D( GL_TEXTURE_3D, 0, int_format, width, height, depth, 0, components, type, data );
		}
	}
	else
	if( tex->target == GL_TEXTURE_2D )
	{
	
		if( flags&_upf_mipmap )
		{
			if( mips )
			{
				int i;
				int mipwidth, mipheight;
				
				mipwidth = width;
				mipheight = height;
				
				glTexParameteri( tex->target, GL_TEXTURE_BASE_LEVEL, 0 );
				CHECK_GL_ERRORS();
				
				for(i = 0; i < MAX_MIPMAP_LEVELS; i++)
				{
					OS_ASSERT( mips[i] );
						
					glTexImage2D( GL_TEXTURE_2D, i, int_format, mipwidth, mipheight, 0, components, type, mips[i] );
					
					mipwidth >>= 1;
					mipheight >>= 1;
					
					if( mipwidth < 1 || mipheight < 1 )
						break;
				}

				glTexParameteri( tex->target, GL_TEXTURE_MAX_LEVEL, i-2 );
				CHECK_GL_ERRORS();

			}
			else
			{
				gluBuild2DMipmaps( GL_TEXTURE_2D, int_format, width, height, components, type, data );
			}
		}
		else
		{
			glTexImage2D( GL_TEXTURE_2D, 0, int_format, width, height, 0, components, type, data );
		}
		
	}
	else 
	if( tex->target == GL_TEXTURE_1D )
	{
		if( flags&_upf_mipmap )
		{
			if( mips )
			{
				int i;
				int mipwidth;
				
				mipwidth = width;
				
				glTexParameteri( tex->target, GL_TEXTURE_BASE_LEVEL, 0 );
				CHECK_GL_ERRORS();

				for(i = 0; i < MAX_MIPMAP_LEVELS; i++)
				{
					OS_ASSERT( mips[i] );
						
					glTexImage1D( GL_TEXTURE_1D, i, int_format, mipwidth, 0, components, type, mips[i] );
					
					mipwidth >>= 1;
					
					if( mipwidth < 1 )
						break;
				}

				glTexParameteri( tex->target, GL_TEXTURE_MAX_LEVEL, i-2 );
				CHECK_GL_ERRORS();
			}
			else
			{
				gluBuild1DMipmaps( GL_TEXTURE_1D, int_format, width, components, type, data );
			}
		}
		else
		{
			glTexImage1D( GL_TEXTURE_1D, 0, int_format, width, 0, components, type, data );
		}
	}

	if (rescaled)
	{
		delete[] reinterpret_cast<unsigned char*>(data);
	}
	
	CHECK_GL_ERRORS();
}

int R_glGetTotalTextureMemory(void)
{
	return alloced_size;
}

static void PurgeTextureSpace( int size )
{
#if OPT_GL_PURGE == 1
	int free_space = MAX_TEXTURE_SPACE-alloced_size;
	
	if( free_space >= size )
		return;
	
	OS_ASSERT( size <= MAX_TEXTURE_SPACE );
	
	RTex_t* tex = bound_list[1];
	RTex_t* prev;
	
	while( free_space < size && tex )
	{
		prev = tex->prev;
		OS_ASSERT(tex->name);
		if( tex->size > 0 && tex->tmu_flags == 0 )
		{
			free_space += tex->size;
			PurgeTexture( tex );
		}
		tex = prev;
	}
	
	if( free_space < size )
	{
		OS_BreakMsg( "ERROR: not enough memory to load %d more bytes in texture partition. %d free, %d needed.\n", size, free_space, size-free_space );
	}
#endif
}

static void SetTexture( RTex_t* tex )
{
	bool first = tex->name == 0;
	int tex_frame = tex->frame;
	
	if( tex->reload )
	{
		tex->reload = false;
		first = true;
		glDeleteTextures( 1, (GLuint*)&tex->name );
		tex->name = 0;
	}

	OS_ASSERT( tex->load_proc );
	
	TextureAccessed( tex );
	BindTexture( tex );
		
	if( first )
		tex->load_proc( tex );
	else
	{
		// do we need to call the rebind proc again?
		if( tex->rebind_proc && (unsigned)tex_frame != r_frame )
			tex->rebind_proc( tex );
	}
}

static void PurgeTexture( RTex_t* tex )
{	
	//OS_ASSERT( tex->tmu_flags == 0 );	// can't be bound to any tmu's!
	OS_ASSERT( tex->name != 0 );
	
	if( tex->next )
		tex->next->prev = tex->prev;
	if( tex->prev )
		tex->prev->next = tex->next;
	 
	if( tex == bound_list[0] )
		bound_list[0] = tex->next;
	if( tex == bound_list[1] )
		bound_list[1] = tex->prev;
	
	glDeleteTextures(1, (GLuint*)&tex->name);
	tex->name = 0;
	
	alloced_size -= tex->size;
	OS_ASSERT( alloced_size >= 0 );
	
	if( tex->purge_proc )
		tex->purge_proc( tex );
		
	tex->next = tex->prev = 0;
	
	//
	// add to unbound list.
	//
	tex->next = unbound_list[0];
	
	if( unbound_list[0] )
		unbound_list[0]->prev = tex;
		
	unbound_list[0] = tex;
	
	if(unbound_list[1] == 0 )
		unbound_list[1] = tex;
				
}

static void TextureAccessed( RTex_t* tex )
{
	// first remove from any lists it's in.
	if( tex != bound_list[0] )
	{
		if( tex->next )
			tex->next->prev = tex->prev;
		if( tex->prev )
			tex->prev->next = tex->next;
		
		if( tex == unbound_list[0] )
			unbound_list[0] = tex->next;
		if( tex == unbound_list[1] )
			unbound_list[1] = tex->prev;
		
		//tex->next = tex->prev = 0;
		
		// add to head of bound list.
		if( tex == bound_list[1] )
			bound_list[1] = tex->prev;
			
		tex->prev = 0;
		
		if( bound_list[0] )
			bound_list[0]->prev = tex;
			
		tex->next = bound_list[0];
		bound_list[0] = tex;
		
		if( bound_list[1] == 0 )
			bound_list[1] = tex;
	}
	
	if( tex->frame != r_frame )
	{
		tex->frame = r_frame;
		accessed_texture_size += tex->size;
		accessed_texture_count++;
	}
}

static void BindTexture( RTex_t* tex )
{
	if( !tex->name )
	{
		glGenTextures(1, (GLuint*)&tex->name);
		OS_ASSERT( tex->name );
	}
	
	CHECK_GL_ERRORS();
	
	//if( (tex->tmu_flags&TMU_FLAG(active_tmu)) == 0 )
	{
		glBindTexture(tex->target, tex->name);
	//	tex->tmu_flags |= TMU_FLAG(active_tmu);
	}
	
	CHECK_GL_ERRORS();
}

void R_glCheckErrors(void)
{
	//
	// did things go as planned?
	//
	bool found=FALSE;
	GLenum err = glGetError();
	while( err != GL_NO_ERROR )
	{
		const char* string = (const char*)gluErrorString(err);
		OutputDebugString("GLERROR: ");
		OutputDebugString(string);
		OutputDebugString("\n");
		found = TRUE;
		err = glGetError();
	}
	
	if( found )
		OS_BreakMsg("R_glCheckErrors: found errors.");
}