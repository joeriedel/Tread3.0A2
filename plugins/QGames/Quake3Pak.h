///////////////////////////////////////////////////////////////////////////////
// Quake3Pak.h
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

#if !defined(QUAKE3PAK_H)
     #define QUAKE3PAK_H

#include "Quake.h"
#include "unzip.h"
#include "C_Tokenizer.h"
#include <hash_map>
#include <vector>
#include <string>

class CQuake3Tex;

class CBuffReader : public CTexFile
{
public:

	CBuffReader(const unsigned char *data, int len);
	virtual ~CBuffReader();

	virtual unsigned int Seek(int ofs, unsigned int from);
	virtual unsigned int Read(void *buff, unsigned int size);
	virtual unsigned int GetLength() const;
	virtual unsigned int GetPosition() const;

private:

	const unsigned char *m_data;
	int m_len;
	int m_ofs;
};

struct CQuake3Shader
{
	CString name;
	CString displayName;
	CString file;
	CString path;
	CString map;
	bool fake;
	StringList maps;
};

typedef std::vector<CQuake3Shader> Quake3ShaderList;

class CQuake3ShaderFile
{
public:

	static bool Parse(const void *data, int len, Quake3ShaderList &list);

private:

	static bool ParseShader(C_Tokenizer &script, CQuake3Shader &shader);
};

class CQuake3PakBase : public CQuakePakBase
{
public:

	CQuake3PakBase();
	CQuake3PakBase(
		ReadTextureDetailsProc readTexDetails,
		ReadTextureProc readTex,
		FreeTextureProc freeTex
	);

	virtual bool Open(const char *path);
	virtual CQuake3Tex *TextureForName(const char *name) = 0;
	virtual bool HasShader(const char *name) = 0;
	virtual bool ExportFile(CTreadDoc *doc, const char *dir, const char *file) const = 0;

protected:

	enum FileType
	{
		TEXTURE,
		SHADER,
		SKIP
	};

	static CString TrimTextureName(const char *path, bool extOnly = false);
	static FileType ClassifyFile(const CString &name);
};

class CQuake3Tex : public CQuakeTex
{
	friend class CQuake3Pak;
	friend class CQuake3PakBase;

public:

	CQuake3Tex(int i, const char *name, const char *displayName, CTextureFactory *factory);
	virtual ~CQuake3Tex();

	virtual int GetBPP();
	virtual int DiskSize() const = 0;

protected:

	int m_bpp;
	bool m_hasDetails;

private:

	static bool ReadTextureDetails(CTexture *context, CTexFile *file, int *width, int *height, int *bpp);
	static bool ReadTexture(CTexture *context, CTexFile *file, void **dst, int *width, int *height, int *bpp);
	static void FreeTextureData(CTexture *context, void *src);
};

class CQuake3Pak : public CQuake3PakBase
{
public:
	CQuake3Pak();
	CQuake3Pak(
		ReadTextureDetailsProc readTexDetails,
		ReadTextureProc readTex,
		FreeTextureProc freeTex
	);
	virtual ~CQuake3Pak();

	virtual bool Open(const char *path);
	virtual bool Initialize(const TextureFactoryList &tfl);
	virtual int NumTextures() const;
	virtual CShader *Texture(int i);
	virtual CTexFile *SeekToData(int i);
	virtual void ReleaseFile(CTexFile *file);
	virtual int EntrySize(int i);
	virtual bool ReadTextureDetails(CTexture *context, int i, int *width, int *height, int *bpp);
	virtual bool ExportFile(CTreadDoc *doc, const char *dir, const char *file) const;

protected:

	struct ZipEntry
	{
		CString name;
		int size;
		unz_s info;
		unzFile file;
	};

	struct Shader
	{
		CQuake3Shader info;
		CQuake3Tex *tex;
	};

	class CQuake3PakTex : public CQuake3Tex
	{
	public:

		CQuake3PakTex(ZipEntry *entry, int i, const char *name, const char *displayName, CTextureFactory *factory);
		virtual ~CQuake3PakTex();

		ZipEntry *Entry() const;
		virtual int DiskSize() const;

	private:

		ZipEntry *m_entry;
	};

	static unsigned char *LoadFile(ZipEntry *entry, int *len);
	virtual CQuake3Tex *TextureForName(const char *name);
	virtual bool HasShader(const char *name);
	static CQuake3Tex *FindTextureInPaks(const char *name, const TextureFactoryList &tfl);
	static bool FindShaderInPaks(const char *name, const TextureFactoryList &tfl);

	typedef stdext::hash_map<std::string, ZipEntry> FileMap;
	typedef stdext::hash_map<std::string, ZipEntry*> RefFileMap;
	typedef stdext::hash_map<std::string, CQuake3PakTex*> TextureMap;
	typedef std::vector<CQuake3PakTex*> TextureList;
	typedef stdext::hash_map<std::string, Shader> ShaderMap;
	typedef std::vector<Shader*> ShaderList;
	
	FileMap m_files;
	TextureMap m_textureMap;
	TextureList m_textureList;
	ShaderMap m_shaderMap;
	ShaderList m_shaderList;
	unzFile m_zipFile;
};

class CQuake3ShaderTex : public CShader
{
public:

	CQuake3ShaderTex(CQuake3Shader *shader, CString name, CString displayName, CQuake3Tex *tex) : 
	  m_shader(shader), m_name(name), m_displayName(displayName), m_tex(tex) {}
	virtual ~CQuake3ShaderTex() {}

	virtual struct RTex_s *Load() { return m_tex->Load(); }
	virtual void Purge() { m_tex->Purge(); }
	virtual bool Pickable() { return m_tex->Pickable(); }
	virtual const char *Name() { return m_name; }
	virtual const char *DisplayName() { return m_displayName; }
	virtual CQuake3Shader *Shader() { return m_shader; }
	virtual void Dimensions(int *w, int *h) { m_tex->Dimensions(w, h); }
	virtual int MemorySize() { return m_tex->MemorySize(); }
	virtual void *Root() { return m_tex; }
	
protected:

	CQuake3Shader *m_shader;
	CString m_name;
	CString m_displayName;
	CQuake3Tex *m_tex;
	
};

#endif