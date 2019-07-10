///////////////////////////////////////////////////////////////////////////////
// r_os_win32.h
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

#ifndef R_OS_WIN32_H
#define R_OS_WIN32_H

#include "gl/gl.h"
#include "gl/glu.h"
#include "gl/glext.h"
#include "gl/wglext.h"

//
// gl 1.2 stuff that i can't get the headers for, damn it!
//

extern PFNGLTEXIMAGE3DPROC R_glEXT_TexImage3D;

//
// define windows hooks
//

extern PFNGLACTIVETEXTUREARBPROC R_glEXT_ActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC R_glEXT_ClientActiveTextureARB;
extern PFNGLMULTITEXCOORD1FARBPROC R_glEXT_MultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD1FVARBPROC R_glEXT_MultiTexCoord1fvARB;
extern PFNGLMULTITEXCOORD2FARBPROC R_glEXT_MultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2FVARBPROC R_glEXT_MultiTexCoord2fvARB;
extern PFNGLLOCKARRAYSEXTPROC R_glEXT_LockArrays;
extern PFNGLUNLOCKARRAYSEXTPROC R_glEXT_UnlockArrays;
extern PFNGLBLENDEQUATIONPROC R_glEXT_BlendEquation;
extern PFNGLDRAWRANGEELEMENTSEXTPROC R_glEXT_DrawRangeElements;

bool R_glImpSupportsARBTextureEnvCombine();
bool R_glImpSupportsARBTextureCubeMap();
bool R_glImpSupportsARBTextureEnvDot3();
bool R_glImpSupportsARBTextureEnvAdd();

//
// NV Extensions.
//

//
// NV_vertex_program
//
bool R_glImpSupportsNVVertexProgram();

extern PFNGLGENPROGRAMSNVPROC R_glEXT_GenProgramsNV;
extern PFNGLBINDPROGRAMNVPROC R_glEXT_BindProgramNV;
extern PFNGLDELETEPROGRAMSNVPROC R_glEXT_DeleteProgramsNV;
extern PFNGLEXECUTEPROGRAMNVPROC R_glEXT_ExecuteProgramNV;
extern PFNGLLOADPROGRAMNVPROC R_glEXT_LoadProgramNV;
extern PFNGLPROGRAMPARAMETER4DNVPROC R_glEXT_ProgramParameter4dNV;
extern PFNGLPROGRAMPARAMETER4DVNVPROC R_glEXT_ProgramParameter4dvNV;
extern PFNGLPROGRAMPARAMETER4FNVPROC R_glEXT_ProgramParameter4fNV;
extern PFNGLPROGRAMPARAMETER4FVNVPROC R_glEXT_ProgramParameter4fvNV;
extern PFNGLPROGRAMPARAMETERS4DVNVPROC R_glEXT_ProgramParameters4dvNV;
extern PFNGLPROGRAMPARAMETERS4FVNVPROC R_glEXT_ProgramParameters4fvNV;
extern PFNGLREQUESTRESIDENTPROGRAMSNVPROC R_glEXT_RequestResidentProgramsNV;
extern PFNGLTRACKMATRIXNVPROC R_glEXT_TrackMatrixNV;
extern PFNGLVERTEXATTRIBPOINTERNVPROC R_glEXT_VertexAttribPointerNV;
extern PFNGLVERTEXATTRIB4FNVPROC R_glEXT_VertexAttrib4fNV;
extern PFNGLVERTEXATTRIBS1DVNVPROC R_glEXT_VertexAttribs1dvNV;
extern PFNGLVERTEXATTRIBS1FVNVPROC R_glEXT_VertexAttribs1fvNV;
extern PFNGLVERTEXATTRIBS1SVNVPROC R_glEXT_VertexAttribs1svNV;
extern PFNGLVERTEXATTRIBS2DVNVPROC R_glEXT_VertexAttribs2dvNV;
extern PFNGLVERTEXATTRIBS2FVNVPROC R_glEXT_VertexAttribs2fvNV;
extern PFNGLVERTEXATTRIBS2SVNVPROC R_glEXT_VertexAttribs2svNV;
extern PFNGLVERTEXATTRIBS3DVNVPROC R_glEXT_VertexAttribs3dvNV;
extern PFNGLVERTEXATTRIBS3FVNVPROC R_glEXT_VertexAttribs3fvNV;
extern PFNGLVERTEXATTRIBS3SVNVPROC R_glEXT_VertexAttribs3svNV;
extern PFNGLVERTEXATTRIBS4DVNVPROC R_glEXT_VertexAttribs4dvNV;
extern PFNGLVERTEXATTRIBS4FVNVPROC R_glEXT_VertexAttribs4fvNV;
extern PFNGLVERTEXATTRIBS4SVNVPROC R_glEXT_VertexAttribs4svNV;
extern PFNGLVERTEXATTRIBS4UBVNVPROC R_glEXT_VertexAttribs4ubvNV;

//
// NV_register_combiners
//
bool R_glImpSupportsNVRegisterCombiners();

extern PFNGLCOMBINERPARAMETERFVNVPROC R_glEXT_CombinerParameterfvNV;
extern PFNGLCOMBINERPARAMETERFNVPROC R_glEXT_CombinerParameterfNV;
extern PFNGLCOMBINERPARAMETERIVNVPROC R_glEXT_CombinerParameterivNV;
extern PFNGLCOMBINERPARAMETERINVPROC R_glEXT_CombinerParameteriNV;
extern PFNGLCOMBINERINPUTNVPROC R_glEXT_CombinerInputNV;
extern PFNGLCOMBINEROUTPUTNVPROC R_glEXT_CombinerOutputNV;
extern PFNGLFINALCOMBINERINPUTNVPROC R_glEXT_FinalCombinerInputNV;

//
// NV_vertex_array_range.
//
bool R_glImpSupportsNVVertexArrayRange();

extern PFNWGLALLOCATEMEMORYNVPROC R_wglEXT_AllocateMemoryNV;
extern PFNWGLFREEMEMORYNVPROC R_wglEXT_FreeMemoryNV;
extern PFNGLVERTEXARRAYRANGENVPROC R_glEXT_VertexArrayRangeNV;
extern PFNGLFLUSHVERTEXARRAYRANGENVPROC R_glEXT_FlushVertexArrayRangeNV;

//
// NV_fence
//
bool R_glImpSupportsNVFence();

extern PFNGLDELETEFENCESNVPROC R_glEXT_DeleteFencesNV;
extern PFNGLGENFENCESNVPROC R_glEXT_GenFencesNV;
extern PFNGLFINISHFENCENVPROC R_glEXT_FinishFenceNV;
extern PFNGLSETFENCENVPROC R_glEXT_SetFenceNV;
extern PFNGLTESTFENCENVPROC R_glEXT_TestFenceNV;

//
// glColorTableEXT
//
extern PFNGLCOLORTABLEEXTPROC R_glEXT_ColorTable;

//
// secondary color
//
extern PFNGLSECONDARYCOLOR3FEXTPROC R_glEXT_SecondaryColor3f;
bool R_glImpSupportsSecondaryColor();

#endif