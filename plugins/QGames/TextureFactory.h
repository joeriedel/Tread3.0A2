///////////////////////////////////////////////////////////////////////////////
// TextureFactory.h
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

#if !defined(TEXTUREFACTORY_H)
     #define TEXTUREFACTORY_H

class CTextureFactory;
typedef std::vector<CTextureFactory*> TextureFactoryList;

///////////////////////////////////////////////////////////////////////////////
// CTexture
///////////////////////////////////////////////////////////////////////////////

class CTexture
{
public:
	virtual ~CTexture() {}
};

///////////////////////////////////////////////////////////////////////////////
// CTexFile
///////////////////////////////////////////////////////////////////////////////

class CTexFile
{
public:

	virtual ~CTexFile() {}

	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };
	virtual int SeekToEnd() { return Seek(0, end); }
	virtual int SeekToBegin() { return Seek(0, begin); }
	virtual unsigned int Seek(int ofs, unsigned int from) = 0;
	virtual unsigned int Read(void *buff, unsigned int size) = 0;
	virtual unsigned int GetLength() const = 0;
	virtual unsigned int GetPosition() const = 0;
};

///////////////////////////////////////////////////////////////////////////////
// CTextureFactory
///////////////////////////////////////////////////////////////////////////////

class CTextureFactory
{
public:
	virtual ~CTextureFactory() {}

	virtual bool Open(const char *path) = 0;
	virtual bool Initialize(const TextureFactoryList &tfl) = 0;
	virtual int NumTextures() const = 0;
	virtual CShader *Texture(int i) = 0;
	virtual CTexFile *SeekToData(int i) = 0;
	virtual int EntrySize(int i) = 0;
	virtual void ReleaseFile(CTexFile *file) = 0;
	virtual bool ReadTextureDetails(CTexture *context, int i, int *width, int *height, int *bpp) = 0;
	virtual bool ReadTexture(CTexture *context, int i, void **dst, int *width, int *height, int *bpp) = 0;
	virtual void FreeTextureData(CTexture *context, void *src) = 0;

	static void *ExpandPalettized(const void *src, int width, int height, int dstBpp, const void *palette);
	static void FreeTexture(void *src);
};

#endif