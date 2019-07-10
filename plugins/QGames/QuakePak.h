///////////////////////////////////////////////////////////////////////////////
// QuakePak.h
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

#if !defined(QUAKEPAK_H)
     #define QUAKEPAK_H

#include <vector>
#include <hash_map>
#include <string>
#include "PakFile.h"

class CQuakeWad2Builder
{
public:
	
	bool Initialize(const char *filename);
	CFile *File();
	void AddEntryInfo(int ofs, int size, const char *name);
	bool Finalize();

private:

	struct FileEntry
	{
		int ofs;
		int size;
		char name[16];
	};

	typedef std::vector<FileEntry> FileEntryList;
	FileEntryList m_dir;
	CFile m_file;
};

typedef bool (*ReadTextureDetailsProc)(CTexture *context, CTexFile *file, int *width, int *height, int *bpp);
typedef bool (*ReadTextureProc)(CTexture *context, CTexFile *file, void **dst, int *width, int *height, int *bpp);
typedef void (*FreeTextureProc)(CTexture *context, void *src);

class CQuakePakBase : public CPakFile
{
public:

	CQuakePakBase(
		ReadTextureDetailsProc readTexDetails,
		ReadTextureProc readTex,
		FreeTextureProc freeTex
	);
	virtual ~CQuakePakBase() {}

	virtual bool ReadTextureDetails(CTexture *context, int i, int *width, int *height, int *bpp);
	virtual bool ReadTexture(CTexture *context, int i, void **dst, int *width, int *height, int *bpp);
	virtual void FreeTextureData(CTexture *context, void *src);
	virtual void ReleaseFile(CTexFile*);

private:

	ReadTextureDetailsProc m_readTexDetails;
	ReadTextureProc m_readTex;
	FreeTextureProc m_freeTex;
};

class CQuakePak : public CQuakePakBase
{
public:
	CQuakePak();
	CQuakePak(
		ReadTextureDetailsProc readTexDetails,
		ReadTextureProc readTex,
		FreeTextureProc freeTex
	);
	virtual ~CQuakePak() {}

	virtual bool Open(const char *path);
	virtual bool Initialize(const TextureFactoryList &texFactories);
	virtual int NumTextures() const;
	virtual CShader *Texture(int i);
	virtual CTexFile *SeekToData(int i);
	virtual int EntrySize(int i);

protected:

	struct FileEntry
	{
		CString name;
		CString displayName;
		int pos;
		int len;
	};

	typedef std::vector<FileEntry> FileEntryList;
	FileEntryList m_files;

	virtual CFile *CFileForIndex(int i);

private:

	enum Type
	{
		BSP,
		WAD
	};

	class CTexFileAdapter : public CTexFile
	{
	public:

		CTexFileAdapter();

		void SetFile(CFile *file);

		virtual unsigned int Seek(int ofs, unsigned int from);
		virtual unsigned int Read(void *buff, unsigned int size);
		virtual unsigned int GetLength() const;
		virtual unsigned int GetPosition() const;

	private:

		CFile *m_file;
	};

	Type m_type;
	CTexFileAdapter m_texFile;
	ReadTextureDetailsProc m_readTexDetails;
	ReadTextureProc m_readTex;
	FreeTextureProc m_freeTex;
};

class CQuakeTex : public CShader, public CTexture
{
public:

	CQuakeTex(int i, const char *name, const char *displayName, CTextureFactory *factory);
	virtual ~CQuakeTex();
	virtual RTex_t *Load();
	virtual void Purge();
	virtual bool Pickable();
	virtual const char *Name();
	virtual const char *DisplayName();
	virtual void Dimensions(int *w, int *h);
	virtual int MemorySize();
	
	virtual bool WriteNativeToFile(CFile *file);

	struct q1miptex_t
	{
		char name[16];
		int width;
		int height;
		int ofs1;
		int ofs2;
		int ofs3;
		int ofs4;
	};

protected:

	virtual int  GetBPP() { return 3; }
	static bool GL_TexUpload(RTex_t *tex);

	int m_index;
	int m_width, m_height;
	RTex_t *m_tex;
	CString m_name, m_displayName;
	CTextureFactory *m_factory;

private:


friend class CQuakePak;

	static bool ReadTextureDetails(CTexture *context, CTexFile *file, int *width, int *height, int *bpp);
	static bool ReadTexture(CTexture *context, CTexFile *file, void **dst, int *width, int *height, int *bpp);
	static void FreeTextureData(CTexture *context, void *src);
};

class CResourceLoader
{
public:
	CResourceLoader(int id) : m_id(id), m_data(0), m_length(0) {}
	void *Data();
	int Length();

private:

	int m_id;
	void *m_data;
	int m_length;
};

#endif
