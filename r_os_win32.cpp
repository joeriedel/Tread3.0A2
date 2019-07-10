///////////////////////////////////////////////////////////////////////////////
// r_os_win32.cpp
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
#include "r_gl.h"

PFNGLACTIVETEXTUREARBPROC R_glEXT_ActiveTextureARB=0;
PFNGLCLIENTACTIVETEXTUREARBPROC R_glEXT_ClientActiveTextureARB=0;
PFNGLMULTITEXCOORD1FARBPROC R_glEXT_MultiTexCoord1fARB=0;
PFNGLMULTITEXCOORD1FVARBPROC R_glEXT_MultiTexCoord1fvARB=0;
PFNGLMULTITEXCOORD2FARBPROC R_glEXT_MultiTexCoord2fARB=0;
PFNGLMULTITEXCOORD2FVARBPROC R_glEXT_MultiTexCoord2fvARB=0;
PFNGLLOCKARRAYSEXTPROC R_glEXT_LockArrays=0;
PFNGLUNLOCKARRAYSEXTPROC R_glEXT_UnlockArrays=0;
PFNGLBLENDEQUATIONPROC R_glEXT_BlendEquation=0;
PFNGLDRAWRANGEELEMENTSEXTPROC R_glEXT_DrawRangeElements;

//
// 3d texture stuff.
//
PFNGLTEXIMAGE3DPROC R_glEXT_TexImage3D;

//
// NV Extensions.
//

//
// NV_vertex_program
//
PFNGLGENPROGRAMSNVPROC R_glEXT_GenProgramsNV;
PFNGLBINDPROGRAMNVPROC R_glEXT_BindProgramNV;
PFNGLDELETEPROGRAMSNVPROC R_glEXT_DeleteProgramsNV;
PFNGLEXECUTEPROGRAMNVPROC R_glEXT_ExecuteProgramNV;
PFNGLLOADPROGRAMNVPROC R_glEXT_LoadProgramNV;
PFNGLPROGRAMPARAMETER4DNVPROC R_glEXT_ProgramParameter4dNV;
PFNGLPROGRAMPARAMETER4DVNVPROC R_glEXT_ProgramParameter4dvNV;
PFNGLPROGRAMPARAMETER4FNVPROC R_glEXT_ProgramParameter4fNV;
PFNGLPROGRAMPARAMETER4FVNVPROC R_glEXT_ProgramParameter4fvNV;
PFNGLPROGRAMPARAMETERS4DVNVPROC R_glEXT_ProgramParameters4dvNV;
PFNGLPROGRAMPARAMETERS4FVNVPROC R_glEXT_ProgramParameters4fvNV;
PFNGLREQUESTRESIDENTPROGRAMSNVPROC R_glEXT_RequestResidentProgramsNV;
PFNGLTRACKMATRIXNVPROC R_glEXT_TrackMatrixNV;
PFNGLVERTEXATTRIBPOINTERNVPROC R_glEXT_VertexAttribPointerNV;
PFNGLVERTEXATTRIB4FNVPROC R_glEXT_VertexAttrib4fNV;
PFNGLVERTEXATTRIBS1DVNVPROC R_glEXT_VertexAttribs1dvNV;
PFNGLVERTEXATTRIBS1FVNVPROC R_glEXT_VertexAttribs1fvNV;
PFNGLVERTEXATTRIBS1SVNVPROC R_glEXT_VertexAttribs1svNV;
PFNGLVERTEXATTRIBS2DVNVPROC R_glEXT_VertexAttribs2dvNV;
PFNGLVERTEXATTRIBS2FVNVPROC R_glEXT_VertexAttribs2fvNV;
PFNGLVERTEXATTRIBS2SVNVPROC R_glEXT_VertexAttribs2svNV;
PFNGLVERTEXATTRIBS3DVNVPROC R_glEXT_VertexAttribs3dvNV;
PFNGLVERTEXATTRIBS3FVNVPROC R_glEXT_VertexAttribs3fvNV;
PFNGLVERTEXATTRIBS3SVNVPROC R_glEXT_VertexAttribs3svNV;
PFNGLVERTEXATTRIBS4DVNVPROC R_glEXT_VertexAttribs4dvNV;
PFNGLVERTEXATTRIBS4FVNVPROC R_glEXT_VertexAttribs4fvNV;
PFNGLVERTEXATTRIBS4SVNVPROC R_glEXT_VertexAttribs4svNV;
PFNGLVERTEXATTRIBS4UBVNVPROC R_glEXT_VertexAttribs4ubvNV;

//
// NV_register_combiners
//
PFNGLCOMBINERPARAMETERFVNVPROC R_glEXT_CombinerParameterfvNV;
PFNGLCOMBINERPARAMETERFNVPROC R_glEXT_CombinerParameterfNV;
PFNGLCOMBINERPARAMETERIVNVPROC R_glEXT_CombinerParameterivNV;
PFNGLCOMBINERPARAMETERINVPROC R_glEXT_CombinerParameteriNV;
PFNGLCOMBINERINPUTNVPROC R_glEXT_CombinerInputNV;
PFNGLCOMBINEROUTPUTNVPROC R_glEXT_CombinerOutputNV;
PFNGLFINALCOMBINERINPUTNVPROC R_glEXT_FinalCombinerInputNV;

//
// NV_vertex_array_range.
//
PFNWGLALLOCATEMEMORYNVPROC R_wglEXT_AllocateMemoryNV;
PFNWGLFREEMEMORYNVPROC R_wglEXT_FreeMemoryNV;
PFNGLVERTEXARRAYRANGENVPROC R_glEXT_VertexArrayRangeNV;
PFNGLFLUSHVERTEXARRAYRANGENVPROC R_glEXT_FlushVertexArrayRangeNV;

//
// NV_fence
//
PFNGLDELETEFENCESNVPROC R_glEXT_DeleteFencesNV;
PFNGLGENFENCESNVPROC R_glEXT_GenFencesNV;
PFNGLFINISHFENCENVPROC R_glEXT_FinishFenceNV;
PFNGLSETFENCENVPROC R_glEXT_SetFenceNV;
PFNGLTESTFENCENVPROC R_glEXT_TestFenceNV;

//
// glColorTableEXT
//
PFNGLCOLORTABLEEXTPROC R_glEXT_ColorTable;

//
// secondary color
//
extern PFNGLSECONDARYCOLOR3FEXTPROC R_glEXT_SecondaryColor3f = 0;
static bool gl_imp_supports_secondary_color_ext = FALSE;

bool R_glImpSupportsSecondaryColor()
{
	return gl_imp_supports_secondary_color_ext;
}

static bool gl_imp_supports_textureenvcombine = FALSE;
static bool gl_imp_supports_texturecubemap = FALSE;
static bool gl_imp_supports_textureenvdot3 = FALSE;
static bool gl_imp_supports_textureenvadd = FALSE;

bool R_glImpSupportsARBTextureEnvCombine()
{
	return gl_imp_supports_textureenvcombine;
}

bool R_glImpSupportsARBTextureCubeMap()
{
	return gl_imp_supports_texturecubemap;
}

bool R_glImpSupportsARBTextureEnvDot3()
{
	return gl_imp_supports_textureenvdot3;
}

bool R_glImpSupportsARBTextureEnvAdd()
{
	return gl_imp_supports_textureenvadd;
}

static bool gl_imp_supports_nvvertexprogram=FALSE;
static bool gl_imp_supports_nvregistercombiners=FALSE;
static bool gl_imp_supports_nvvertexarrayrange=FALSE;
static bool gl_imp_supports_nvfence=FALSE;

bool R_glImpSupportsNVVertexArrayRange()
{
	return gl_imp_supports_nvvertexarrayrange;
}

bool R_glImpSupportsNVFence()
{
	return gl_imp_supports_nvfence;
}

bool R_glImpSupportsNVVertexProgram()
{
	return gl_imp_supports_nvvertexprogram;
}

bool R_glImpSupportsNVRegisterCombiners()
{
	return gl_imp_supports_nvregistercombiners;
}

bool R_OS_SetupForHardware( void )
{
	return FALSE;
}

//
// All win32 extensions are loaded, regardless of hardware. That means
// that NVidia extensions are loaded if found, as well as ATI...
//
bool R_OS_glLoadExtensions( void )
{ 
	R_glEXT_ActiveTextureARB = 0;
	R_glEXT_ClientActiveTextureARB = 0;
	R_glEXT_MultiTexCoord1fARB = 0;
	R_glEXT_MultiTexCoord1fvARB = 0;
	R_glEXT_MultiTexCoord2fARB = 0;
	R_glEXT_MultiTexCoord2fvARB = 0;
	R_glEXT_LockArrays = 0;
	R_glEXT_UnlockArrays = 0;
	R_glEXT_DrawRangeElements = 0;
	
	gl_imp_supports_nvvertexprogram = FALSE;
	gl_imp_supports_nvregistercombiners = FALSE;
	gl_imp_supports_nvvertexarrayrange = FALSE;
	gl_imp_supports_nvfence = FALSE;
	
	if( !R_glImpSupportsARBMultiTexture() )
	{
		OS_OkAlertMessage("Error", "Game requires multitexturing extension!");
		return TRUE;
	}
	
	R_glEXT_ActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
	R_glEXT_ClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
	R_glEXT_MultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)wglGetProcAddress("glMultiTexCoord1fARB");
	R_glEXT_MultiTexCoord1fvARB = (PFNGLMULTITEXCOORD1FVARBPROC)wglGetProcAddress("glMultiTexCoord1fvARB");
	R_glEXT_MultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
	R_glEXT_MultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)wglGetProcAddress("glMultiTexCoord2fvARB");
	R_glEXT_LockArrays = (PFNGLLOCKARRAYSEXTPROC)wglGetProcAddress("glLockArraysEXT");
	R_glEXT_UnlockArrays = (PFNGLUNLOCKARRAYSEXTPROC)wglGetProcAddress("glUnlockArraysEXT");
	R_glEXT_BlendEquation = (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glBlendEquation");
	
	R_glEXT_DrawRangeElements = (PFNGLDRAWRANGEELEMENTSEXTPROC)wglGetProcAddress("glDrawRangeElements");
	
	gl_imp_supports_textureenvcombine = R_glIsExtensionSupported( "GL_ARB_texture_env_combine" );
	gl_imp_supports_texturecubemap = R_glIsExtensionSupported( "GL_ARB_texture_cube_map" );
	gl_imp_supports_textureenvdot3 = R_glIsExtensionSupported( "GL_ARB_texture_env_dot3" );
	gl_imp_supports_textureenvadd = R_glIsExtensionSupported( "GL_ARB_texture_env_add" );
	
	gl_imp_supports_secondary_color_ext = R_glIsExtensionSupported( "GL_EXT_secondary_color" );
	R_glEXT_SecondaryColor3f = 0;
	if( gl_imp_supports_secondary_color_ext )
	{
		R_glEXT_SecondaryColor3f = (PFNGLSECONDARYCOLOR3FEXTPROC)wglGetProcAddress("glSecondaryColor3fEXT");
		gl_imp_supports_secondary_color_ext = (R_glEXT_SecondaryColor3f != 0);
	}

	//
	// 3d texture stuff.
	//
	R_glEXT_TexImage3D = 0;	
	R_glEXT_TexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
	
	//
	// Load NVIDIA Extensions.
	//
	
	R_glEXT_GenProgramsNV = 0;
	R_glEXT_BindProgramNV = 0;
	R_glEXT_DeleteProgramsNV = 0;
	R_glEXT_ExecuteProgramNV = 0;
	R_glEXT_LoadProgramNV = 0;
	R_glEXT_ProgramParameter4dNV = 0;
	R_glEXT_ProgramParameter4dvNV = 0;
	R_glEXT_ProgramParameter4fNV = 0;
	R_glEXT_ProgramParameter4fvNV = 0;
	R_glEXT_ProgramParameters4dvNV = 0;
	R_glEXT_ProgramParameters4fvNV = 0;
	R_glEXT_RequestResidentProgramsNV = 0;
	R_glEXT_TrackMatrixNV = 0;
	R_glEXT_VertexAttribPointerNV = 0;
	R_glEXT_VertexAttribs1dvNV = 0;
	R_glEXT_VertexAttribs1fvNV = 0;
	R_glEXT_VertexAttribs1svNV = 0;
	R_glEXT_VertexAttribs2dvNV = 0;
	R_glEXT_VertexAttribs2fvNV = 0;
	R_glEXT_VertexAttribs2svNV = 0;
	R_glEXT_VertexAttribs3dvNV = 0;
	R_glEXT_VertexAttribs3fvNV = 0;
	R_glEXT_VertexAttribs3svNV = 0;
	R_glEXT_VertexAttribs4dvNV = 0;
	R_glEXT_VertexAttribs4fvNV = 0;
	R_glEXT_VertexAttribs4svNV = 0;
	R_glEXT_VertexAttribs4ubvNV = 0;
	
	//
	// NV_vertex_program
	//
	gl_imp_supports_nvvertexprogram = R_glIsExtensionSupported( "GL_NV_vertex_program" );
	
	if( gl_imp_supports_nvvertexprogram )
	{
		
		R_glEXT_GenProgramsNV 			  = (PFNGLGENPROGRAMSNVPROC)wglGetProcAddress("glGenProgramsNV");
		R_glEXT_BindProgramNV 			  = (PFNGLBINDPROGRAMNVPROC)wglGetProcAddress("glBindProgramNV");
		R_glEXT_DeleteProgramsNV 		  = (PFNGLDELETEPROGRAMSNVPROC)wglGetProcAddress("glDeleteProgramsNV");
		R_glEXT_ExecuteProgramNV 		  = (PFNGLEXECUTEPROGRAMNVPROC)wglGetProcAddress("glExecuteProgramNV");
		R_glEXT_LoadProgramNV 			  = (PFNGLLOADPROGRAMNVPROC)wglGetProcAddress("glLoadProgramNV");
		R_glEXT_ProgramParameter4dNV 	  = (PFNGLPROGRAMPARAMETER4DNVPROC)wglGetProcAddress("glProgramParameter4dNV");
		R_glEXT_ProgramParameter4dvNV 	  = (PFNGLPROGRAMPARAMETER4DVNVPROC)wglGetProcAddress("glProgramParameter4dvNV");
		R_glEXT_ProgramParameter4fNV 	  = (PFNGLPROGRAMPARAMETER4FNVPROC)wglGetProcAddress("glProgramParameter4fNV");
		R_glEXT_ProgramParameter4fvNV 	  = (PFNGLPROGRAMPARAMETER4FVNVPROC)wglGetProcAddress("glProgramParameter4fvNV");
		R_glEXT_ProgramParameters4dvNV	  = (PFNGLPROGRAMPARAMETERS4DVNVPROC)wglGetProcAddress("glProgramParameters4dvNV");
		R_glEXT_ProgramParameters4fvNV 	  = (PFNGLPROGRAMPARAMETERS4FVNVPROC)wglGetProcAddress("glProgramParameters4fvNV");
		R_glEXT_RequestResidentProgramsNV = (PFNGLREQUESTRESIDENTPROGRAMSNVPROC)wglGetProcAddress("glRequestResidentProgramsNV");
		R_glEXT_TrackMatrixNV 			  = (PFNGLTRACKMATRIXNVPROC)wglGetProcAddress("glTrackMatrixNV");
		R_glEXT_VertexAttribPointerNV 	  = (PFNGLVERTEXATTRIBPOINTERNVPROC)wglGetProcAddress("glVertexAttribPointerNV");
		R_glEXT_VertexAttrib4fNV          = (PFNGLVERTEXATTRIB4FNVPROC)wglGetProcAddress("glVertexAttrib4fNV");
		R_glEXT_VertexAttribs1dvNV 		  = (PFNGLVERTEXATTRIBS1DVNVPROC)wglGetProcAddress("glVertexAttribs1dvNV");
		R_glEXT_VertexAttribs1fvNV 		  = (PFNGLVERTEXATTRIBS1FVNVPROC)wglGetProcAddress("glVertexAttribs1fvNV");
		R_glEXT_VertexAttribs1svNV 		  = (PFNGLVERTEXATTRIBS1SVNVPROC)wglGetProcAddress("glVertexAttribs1svNV");
		R_glEXT_VertexAttribs2dvNV 		  = (PFNGLVERTEXATTRIBS2DVNVPROC)wglGetProcAddress("glVertexAttribs2dvNV");
		R_glEXT_VertexAttribs2fvNV 		  = (PFNGLVERTEXATTRIBS2FVNVPROC)wglGetProcAddress("glVertexAttribs2fvNV");
		R_glEXT_VertexAttribs2svNV 		  = (PFNGLVERTEXATTRIBS2SVNVPROC)wglGetProcAddress("glVertexAttribs2svNV");
		R_glEXT_VertexAttribs3dvNV		  = (PFNGLVERTEXATTRIBS3DVNVPROC)wglGetProcAddress("glVertexAttribs3dvNV");
		R_glEXT_VertexAttribs3fvNV 		  = (PFNGLVERTEXATTRIBS3FVNVPROC)wglGetProcAddress("glVertexAttribs3fvNV");
		R_glEXT_VertexAttribs3svNV 		  = (PFNGLVERTEXATTRIBS3SVNVPROC)wglGetProcAddress("glVertexAttribs3svNV");
		R_glEXT_VertexAttribs4dvNV 		  = (PFNGLVERTEXATTRIBS4DVNVPROC)wglGetProcAddress("glVertexAttribs4dvNV");
		R_glEXT_VertexAttribs4fvNV 		  = (PFNGLVERTEXATTRIBS4FVNVPROC)wglGetProcAddress("glVertexAttribs4fvNV");
		R_glEXT_VertexAttribs4svNV 		  = (PFNGLVERTEXATTRIBS4SVNVPROC)wglGetProcAddress("glVertexAttribs4svNV");
		R_glEXT_VertexAttribs4ubvNV 	  = (PFNGLVERTEXATTRIBS4UBVNVPROC)wglGetProcAddress("glVertexAttribs4ubvNV");
		
		//
		// check that we found all of them!
		//
		gl_imp_supports_nvvertexprogram = (R_glEXT_GenProgramsNV&&R_glEXT_BindProgramNV&&R_glEXT_DeleteProgramsNV&&
					R_glEXT_ExecuteProgramNV&&R_glEXT_LoadProgramNV&&R_glEXT_ProgramParameter4dNV&&
					R_glEXT_ProgramParameter4dvNV&&R_glEXT_ProgramParameter4fNV&&R_glEXT_ProgramParameter4fvNV&&
					R_glEXT_ProgramParameters4dvNV&&R_glEXT_ProgramParameters4fvNV&&R_glEXT_RequestResidentProgramsNV&&
					R_glEXT_TrackMatrixNV&&R_glEXT_VertexAttribPointerNV&&R_glEXT_VertexAttribs1dvNV&&
					R_glEXT_VertexAttribs1fvNV&&R_glEXT_VertexAttribs1svNV&&R_glEXT_VertexAttribs2dvNV&&
					R_glEXT_VertexAttribs2fvNV&&R_glEXT_VertexAttribs2svNV&&R_glEXT_VertexAttribs3dvNV&&
					R_glEXT_VertexAttribs3fvNV&&R_glEXT_VertexAttribs3svNV&&R_glEXT_VertexAttribs4dvNV&&
					R_glEXT_VertexAttribs4fvNV&&R_glEXT_VertexAttribs4svNV&&R_glEXT_VertexAttribs4ubvNV&&
					R_glEXT_VertexAttrib4fNV);
	}
	
	gl_imp_supports_nvregistercombiners = R_glIsExtensionSupported("GL_NV_register_combiners");
	
	
	R_glEXT_CombinerParameterfvNV = 0;
	R_glEXT_CombinerParameterfNV  = 0;
	R_glEXT_CombinerParameterivNV = 0;
	R_glEXT_CombinerParameteriNV  = 0;
	R_glEXT_CombinerInputNV       = 0;
	R_glEXT_CombinerOutputNV      = 0;
	R_glEXT_FinalCombinerInputNV  = 0;
	
	if( gl_imp_supports_nvregistercombiners )
	{
		R_glEXT_CombinerParameterfvNV = (PFNGLCOMBINERPARAMETERFVNVPROC)wglGetProcAddress("glCombinerParameterfvNV");
		R_glEXT_CombinerParameterfNV  = (PFNGLCOMBINERPARAMETERFNVPROC) wglGetProcAddress("glCombinerParameterfNV");
		R_glEXT_CombinerParameterivNV = (PFNGLCOMBINERPARAMETERIVNVPROC)wglGetProcAddress("glCombinerParameterivNV");
		R_glEXT_CombinerParameteriNV  = (PFNGLCOMBINERPARAMETERINVPROC) wglGetProcAddress("glCombinerParameteriNV");
		R_glEXT_CombinerInputNV       = (PFNGLCOMBINERINPUTNVPROC)wglGetProcAddress("glCombinerInputNV");
		R_glEXT_CombinerOutputNV      = (PFNGLCOMBINEROUTPUTNVPROC)wglGetProcAddress("glCombinerOutputNV");
		R_glEXT_FinalCombinerInputNV  = (PFNGLFINALCOMBINERINPUTNVPROC)wglGetProcAddress("glFinalCombinerInputNV");
		
		gl_imp_supports_nvregistercombiners = (R_glEXT_CombinerParameterfNV&&R_glEXT_CombinerParameterfNV&&
				R_glEXT_CombinerParameterivNV&&R_glEXT_CombinerParameteriNV&&R_glEXT_CombinerInputNV&&
				R_glEXT_CombinerOutputNV&&R_glEXT_FinalCombinerInputNV);
	}
	
	gl_imp_supports_nvvertexarrayrange = R_glIsExtensionSupported("GL_NV_vertex_array_range") && R_glIsExtensionSupported("GL_NV_vertex_array_range2");
	
	R_wglEXT_AllocateMemoryNV = 0;
	R_wglEXT_FreeMemoryNV = 0;
	R_glEXT_VertexArrayRangeNV = 0;
	R_glEXT_FlushVertexArrayRangeNV = 0;
	
	if( gl_imp_supports_nvvertexarrayrange )
	{
		R_wglEXT_AllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC)wglGetProcAddress("wglAllocateMemoryNV");
		R_wglEXT_FreeMemoryNV = (PFNWGLFREEMEMORYNVPROC)wglGetProcAddress("wglFreeMemoryNV");
		R_glEXT_VertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC)wglGetProcAddress("glVertexArrayRangeNV");
		R_glEXT_FlushVertexArrayRangeNV = (PFNGLFLUSHVERTEXARRAYRANGENVPROC)wglGetProcAddress("glFlushVertexArrayRangeNV");
		
		gl_imp_supports_nvvertexarrayrange = (R_wglEXT_AllocateMemoryNV&&R_wglEXT_FreeMemoryNV&&
			R_glEXT_VertexArrayRangeNV&&R_glEXT_FlushVertexArrayRangeNV);
	}
	
	gl_imp_supports_nvfence = R_glIsExtensionSupported("GL_NV_fence");
	
	R_glEXT_DeleteFencesNV = 0;
	R_glEXT_GenFencesNV = 0;
	R_glEXT_FinishFenceNV = 0;
	R_glEXT_SetFenceNV = 0;
	R_glEXT_TestFenceNV = 0;
	
	if( gl_imp_supports_nvfence )
	{
		R_glEXT_DeleteFencesNV = (PFNGLDELETEFENCESNVPROC)wglGetProcAddress("glDeleteFencesNV");
		R_glEXT_GenFencesNV = (PFNGLGENFENCESNVPROC)wglGetProcAddress("glGenFencesNV");
		R_glEXT_FinishFenceNV = (PFNGLFINISHFENCENVPROC)wglGetProcAddress("glFinishFenceNV");
		R_glEXT_SetFenceNV = (PFNGLSETFENCENVPROC)wglGetProcAddress("glSetFenceNV");
		R_glEXT_TestFenceNV = (PFNGLTESTFENCENVPROC)wglGetProcAddress("glTestFenceNV");
		
		gl_imp_supports_nvfence = (R_glEXT_DeleteFencesNV&&R_glEXT_GenFencesNV&&R_glEXT_FinishFenceNV&&
			R_glEXT_SetFenceNV&&R_glEXT_TestFenceNV);
	}
	
	R_glEXT_ColorTable = 0;
	if( R_glIsExtensionSupported( "GL_EXT_paletted_texture" ) && 
		R_glIsExtensionSupported( "GL_EXT_shared_texture_palette" ) )
	{
		R_glEXT_ColorTable = (PFNGLCOLORTABLEEXTPROC)wglGetProcAddress("glColorTableEXT");
	}
			
	return FALSE;
}
