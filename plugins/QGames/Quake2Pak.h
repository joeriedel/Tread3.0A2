///////////////////////////////////////////////////////////////////////////////
// Quake2Pak.h
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

#if !defined(QUAKE2PAK_H)
     #define QUAKE2PAK_H

#include "QuakePak.h"

///////////////////////////////////////////////////////////////////////////////
// CQuake2Pak
///////////////////////////////////////////////////////////////////////////////

class CQuake2Pak : public CQuakePak
{
public:
	CQuake2Pak();
	CQuake2Pak(
		ReadTextureDetailsProc readTexDetails,
		ReadTextureProc readTex,
		FreeTextureProc freeTex
	);
	virtual ~CQuake2Pak() {}

	virtual bool Open(const char *path);
	virtual CShader *Texture(int i);

protected:

	const char *GetTexExt() const { return ".wal"; }
};

///////////////////////////////////////////////////////////////////////////////
// CQuake2Tex
///////////////////////////////////////////////////////////////////////////////

class CQuake2Tex : public CQuakeTex
{
public:

	CQuake2Tex(int i, const char *name, const char *displayName, CTextureFactory *factory);
	virtual ~CQuake2Tex();
	
	virtual int Surface();
	virtual int Contents();
	virtual int Value();

protected:

	int m_surface;
	int m_contents;
	int m_value;

	virtual void LoadDetails();
	bool m_hasDetails;

private:

friend class CQuake2Pak;

	static bool ReadTextureDetails(CTexture *context, CTexFile *file, int *width, int *height, int *bpp);
	static bool ReadTexture(CTexture *context, CTexFile *file, void **dst, int *width, int *height, int *bpp);
	static void FreeTextureData(CTexture *context, void *src);
};

#endif