///////////////////////////////////////////////////////////////////////////////
// r_gl.h
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

#ifndef R_GL_H
#define R_GL_H

#include "os.h"
#include "r_os.h"

typedef bool (*RTexTouchProc) ( struct RTex_s* texture );

#define TEXTURE_NAME_LENGTH 31
typedef struct RTex_s
{
	struct RTex_s* next, *prev;
	
	char str_name[TEXTURE_NAME_LENGTH+1];
	unsigned int name;
	int width, height, depth;
	int bytes_pp;
	int size;
	int frame;
	int tmu_flags;
	void* user_data[2];
	int target;	// GL_TEXTURE_2D, or GL_TEXTURE_3D,
	bool reload;

	RTexTouchProc load_proc;
	RTexTouchProc rebind_proc;
	RTexTouchProc purge_proc;

	struct
	{
		int handle;
		int key;
	} cache;

} RTex_t;

enum
{
	_upf_wrap = 0x1,
	_upf_filter = 0x2,
	_upf_mipmap = 0x4,
	_upf_compress = 0x8,
	_upf_trilinear = 0x10
};

OS_FNEXP RTex_t* R_glCreateTexture( const char* name, int width, int height, int depth, int bytes_per_pixel, RTexTouchProc load_proc, RTexTouchProc rebind_proc, RTexTouchProc purge_proc );
OS_FNEXP void R_glDeleteTexture( RTex_t* tex );

OS_FNEXP void R_glForceTextureUpload( RTex_t* tex );

// note: format *must* be extended pixel data types.
OS_FNEXP void R_glUploadTexture( RTex_t* tex, int width, int height, int depth, int type, int components, int flags, void* data, void** mips);
OS_FNEXP void R_glUploadSubTexture2D( RTex_t* tex, int xofs, int yofs, int level, int width, int height, int type, int components, void* data);
OS_FNEXP void R_glUploadTexture_CubeMap( int target, RTex_t* tex, int width, int height, int depth, int type, int components, int flags, void* data, void** mips );

#define MAX_TMUS				8
#define MAX_TEXTURE_SPACE		(1024*1024*256)
#define MAX_MIPMAP_LEVELS		8

#define TMU_0					0
#define TMU_1					1
#define TMU_2					2
#define TMU_3					3
#define TMU_4					4
#define TMU_5					5
#define TMU_6					6
#define TMU_7					7

#if 0
#define CHECK_GL_ERRORS()	R_glCheckErrors()
#else
#define CHECK_GL_ERRORS()  (void(0))
#endif

//
// The GL texture system is programmed to only maintain MAX_TEXTURE_SPACE
// bytes of texture data in the GL driver. As this space is used, textures
// may be purged out of the texture system to make room. To disable this feature
// change this OPT_ to 0. This also means that after the UploadProc of a texture
// has been called, the app is free to dispose of it's texture.
//
#define OPT_GL_PURGE			1

//
// this state system uses active state committing.
// if you want to change or set a state you *must* specify the state
// otherwise it is not changed.
//
// for example, to shut off depth writing you would 
// have to specify _dwm_off. if you don't specify a _dwm_* flag
// the the depth write mask is left unchanged from it's current driver state.
//
enum
{
	// depth test function
	_dtf_always = 0x1, // always = NO_DEPTH_TESTING
	_dtf_less = 0x2,
	_dtf_greater = 0x4,
	_dtf_lequal = 0x8,
	_dtf_gequal = 0x10,
	_dtf_equal = 0x20,
	_dtf_never = 0x40,
	_dtf_flags = (_dtf_always|_dtf_less|_dtf_greater|_dtf_lequal|_dtf_gequal|_dtf_equal|_dtf_never),
	// depth write mask
	_dwm_on = 0x80,
	_dwm_off = 0x100,
	_dwm_flags = ( _dwm_on|_dwm_off ),
	// cull face mode
	_cfm_front = 0x200,
	_cfm_back  = 0x400,
	_cfm_none  = 0x800,
	_cfm_front_cw = 0x1000,
	_cfm_front_ccw = 0x2000,
	_cfm_flags = (_cfm_front|_cfm_back|_cfm_none|_cfm_front_cw|_cfm_front_ccw),
	// color write mask
	_cwm_r = 0x4000,
	_cwm_g = 0x8000,
	_cwm_b = 0x10000,
	_cwm_a = 0x20000,
	_cwm_off = 0x40000,
	_cwm_all = (_cwm_r|_cwm_g|_cwm_b|_cwm_a),
	_cwm_rgb = (_cwm_r|_cwm_g|_cwm_b),
	_cwm_flags = (_cwm_all|_cwm_off),
	// vertex array
	_va_on = 0x80000,
	_va_off = 0x100000,
	_va_flags = (_va_on|_va_off),
	// index array
	_ia_on = 0x200000,
	_ia_off = 0x400000,
	_ia_flags = (_ia_on|_ia_off),
	// color array
	_ca_on = 0x8000000,
	_ca_off = 0x10000000,
	_ca_flags = (_ca_on|_ca_off),
	// normal array.
	_na_on  = 0x20000000,
	_na_off = 0x40000000,
	_na_flags = (_na_on|_na_off),
	// some nice array combos,
	_no_arrays = (_va_off|_ia_off|_ca_off|_na_off),
	_all_arrays = (_va_on|_ia_on|_ca_on),
	_va_ia_on = (_va_on|_ia_on|_ca_off),
	_va_ca_on = (_va_on|_ca_on|_ia_off),
	_ia_ca_on = (_ia_on|_ca_on|_va_off),
	// blend stuff is passed in on the blend flags since there's so damn many
	// blend equations
	_beq_add = 0x1,
	_beq_sub = 0x2,
	_beq_rev_sub = 0x4,
	_beq_min = 0x8,
	_beq_max = 0x10,
	_beq_flags = (_beq_add|_beq_sub|_beq_rev_sub|_beq_min|_beq_max),
	// blend mode source functions
	_bms_one = 0x20,
	_bms_dst_color = 0x40,
	_bms_inv_dst_color = 0x80,
	_bms_src_alpha = 0x100,
	_bms_inv_src_alpha = 0x200,
	_bms_dst_alpha = 0x400,
	_bms_inv_dst_alpha = 0x800,
	_bms_src_alpha_saturate = 0x1000,
	_bms_zero = 0x2000,
	_bms_constant_color = 0x4000,
	_bms_inv_constant_color = 0x8000,
	_bms_constant_alpha = 0x10000,
	_bms_inv_constant_alpha = 0x20000,
	_bms_flags = (_bms_one|_bms_dst_color|_bms_inv_dst_color|_bms_src_alpha|
					_bms_inv_src_alpha|_bms_dst_alpha|_bms_inv_dst_alpha|
					_bms_src_alpha_saturate|_bms_zero|_bms_constant_color|
					_bms_inv_constant_color|_bms_constant_alpha|_bms_inv_constant_alpha),
	// blend mode destination funcions
	_bmd_zero = 0x40000,
	_bmd_one  = 0x80000,
	_bmd_src_color = 0x100000,
	_bmd_inv_src_color = 0x200000,
	_bmd_src_alpha = 0x400000,
	_bmd_inv_src_alpha = 0x800000,
	_bmd_dst_alpha = 0x1000000,
	_bmd_inv_dst_alpha = 0x2000000,
	_bmd_constant_color = 0x4000000,
	_bmd_inv_constant_color = 0x8000000,
	_bmd_constant_alpha = 0x10000000,
	_bmd_inv_constant_alpha = 0x20000000,
	_bmd_flags = (_bmd_one|_bmd_src_color|_bmd_inv_src_color|_bmd_src_alpha|
					_bmd_inv_src_alpha|_bmd_dst_alpha|_bmd_inv_dst_alpha|
					_bmd_zero|_bmd_constant_color|
					_bmd_inv_constant_color|_bmd_constant_alpha|_bmd_inv_constant_alpha),
	_bm_off = (_bms_one|_bmd_zero),
	// texture unit states
	// texture env mode
	_tem_modulate = 0x1,
	_tem_decal = 0x2,
	_tem_blend = 0x4,
	_tem_replace = 0x8,
	_tem_combine = 0x10,
	_tem_flags = (_tem_modulate|_tem_decal|_tem_blend|_tem_replace|_tem_combine),
	// tex coordinate array
	_tca_on = 0x20,
	_tca_off = 0x40,
	_tca_flags = (_tca_on|_tca_off)
};

// array types.
enum
{
	_vertex=0,
	_color,
	_index,
	_normal,

	// NOTE: the interleaved arrays that hold texture coordinates are
	// *not* supported by this GL system.
	
	_interleaved,
	
	NUM_ARRAYS,
	
	_texcoord=0,
	
	NUM_TMU_ARRAYS
};

typedef struct glArrayState_s
{
	int size;
	int type;
	int stride;
	void* array;
	
} glArrayState_t;

typedef struct TmuState_s
{
	glArrayState_t arrays[NUM_TMU_ARRAYS];
	int state_flags;
	int target;	
	RTex_t* tex;
	
	struct arb_combine
	{
		int rga_mode;
		int alpha_mode;

		int src0_rgba;
		int src1_rgba;
		int src2_rgba;
		int op0_rgba;
		int op1_rgba;
		int op2_rgba;
		float rgba_scale;

		int src0_alpha;
		int src1_alpha;
		int src2_alpha;
		int op0_alpha;
		int op1_alpha;
		int op2_alpha;
		float alpha_scale;

	};

	arb_combine combine;

} TmuState_t;

typedef struct glState_s
{
	TmuState_t tmus[MAX_TMUS];
	glArrayState_t arrays[NUM_ARRAYS];
	
	int state_flags;
	int blend_flags;
	
	bool stencil_enabled;
	int stencil_func;
	int stencil_func_mask;
	int stencil_func_ref;
	int stencil_op_fail;
	int stencil_op_zfail;
	int stencil_op_zpass;
	int stencil_write_mask;
} glState_t;

#define NO_CHANGE		-1

#define MAKE_GL_VERSION( high, low ) ( (high<<16)+low )

#if defined(__BUILDING_TREAD__)

// passing NULL (0) for 'state' means use the default state.
void R_glBeginFrame(void);
void R_glEndFrame(void);
int R_glGetAccessedTextureMemory(void);
int R_glGetAccessedTextureCount(void);
int R_glGetTotalTextureMemory(void);
void R_glSet( glState_t* state, int flags, int blends );
void R_glAdd( glState_t* state, int flags, int blends );
void R_glSub( glState_t* state, int flags, int blends );
void R_glSetArray( glState_t* state, int array, int size, int type, int stride, void* data );
void R_glSetTMU( glState_t* state, int tmu, RTex_t* tex, int states );
void R_glAddTMU( glState_t* state, int tmu, int states );
void R_glSubTMU( glState_t* state, int tmu, int states );
void R_glSetTMUArray( glState_t* state, int tmu, int array, int size, int type, int stride, void* data );
void R_glDisableAllTMUs( glState_t* state );
void R_glCommit( glState_t* state );
void R_glGetDriverState( glState_t* state );
void R_glSetDriverState( glState_t* state );
void R_glUnlockArrays(void);
void R_glLockArrays( int start, int end );

void R_SetupBumpPal(void);

//
// ARB ENV_combine mode setup. You must tell the TMU you want _tem_combine mode.
//
void R_SetTMURGBCombineMode( glState_t* state, int tmu, int mode, int src0, int op0, int src1=0, int op1=0, int src2=0, int op2=0, float scale = 1.0f );
void R_SetTMUALPHACombineMode( glState_t* state, int tmu, int mode, int src0, int op0, int src1=0, int op1=0, int src2=0, int op2=0, float scale = 1.0f );

// passing NO_CHANGE for any of these leaves values unnafected.
void R_glEnableStencil( glState_t* state, bool enabled=TRUE );
void R_glStencilParms( glState_t* state, int func, int func_mask, int func_ref, int op_fail, int op_zfail, int op_zpass, int write_mask );

bool R_glIsExtensionSupported( const char* ext );
bool R_glImpSupportsARBMultiTexture( void );
bool R_glImpSupportsARBImaging( void );
bool R_glImpSupportsEXTLockArrays( void );
bool R_glImpSupportsSGISGenerateMipmap( void );
bool R_glImpSupportsS3TCCompression( void );
bool R_glImpSupportsFXT1Compression( void );
bool R_glImpSupportsEXTPalettedTexture( void );
bool R_glImpSupportsARBNonPow2(void);

bool R_glStartup( void );
bool R_glShutdown( void );
bool R_glPrecacheTextures( void ); // loads all textures into driver.
bool R_glPrecacheTexture( RTex_t* rtex );
void R_glSetActiveTMU( int tmu );
void R_glDrawRangeElements( int mode, int start, int end, int count, int type, void* indices );
void R_glClearRangeElementCount();
int  R_glRangeElementCount();
void R_glEnableRangeElementCount( bool enable=TRUE );
int R_glVersion();
int R_glMajorVersion();
int R_glMinorVersion();

int R_glMaxTextureSize(void);

void R_glUseLockArrays( bool use=TRUE );

void R_glResizeTextureForCard( int* width, int* height, int* type, int* components, void** data );
void R_glCheckErrors(void);

//
// note: this will never return > MAX_TMUS
//
int R_glGetNumTMUs();

//
// note: this can return > MAX_TMUS
//
int R_glGetNumImpTMUs();

Byte* R_glGetColorTable();

HGLRC R_glCreateContext( HDC dc, bool need_window=TRUE );

#endif

#endif
