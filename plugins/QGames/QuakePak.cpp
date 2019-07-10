///////////////////////////////////////////////////////////////////////////////
// QuakePak.cpp
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
#include "QuakePak.h"
#include "resource.h"

#define QUAKE_WAD_ID 0x32444157
#define QUAKE_PAK_ID 0x4B434150
#define QUAKE_TEX_ID 0x44

#define SAFE_READ(f, x) ((f)->Read(&(x), sizeof(x)) == sizeof(x))
#define WRITE(f, x) ((f)->Write(&(x), sizeof(x)))

static CResourceLoader s_quakePal(IDR_RT_QUAKEPAL);

bool CQuakeWad2Builder::Initialize(const char *filename)
{
	if (!m_file.Open(filename, CFile::modeWrite|CFile::modeCreate)) return false;
	int i = QUAKE_WAD_ID;
	WRITE(&m_file, i);
	i = 0;
	WRITE(&m_file, i);
	WRITE(&m_file, i);
	return true;
}

CFile *CQuakeWad2Builder::File()
{
	return &m_file;
}

void CQuakeWad2Builder::AddEntryInfo(int ofs, int size, const char *name)
{
	FileEntry e;
	e.ofs = ofs;
	e.size = size;
	strncpy(e.name, name, 16);
	m_dir.push_back(e);
}

bool CQuakeWad2Builder::Finalize()
{
	if (m_dir.size() > 0)
	{
		int dirOfs = (int)m_file.GetPosition();
		for (FileEntryList::iterator it = m_dir.begin(); it != m_dir.end(); ++it)
		{
			FileEntry &e = *it;
			struct WadEntry
			{
				int ofs;
				int dsize;
				int size;
				char type;
				char cmps;
				short dummy;
				char name[16];
			};
			OS_STATIC_ASSERT(sizeof(WadEntry) == 32);
			WadEntry wad;
			wad.ofs = e.ofs;
			wad.dsize = e.size;
			wad.size = e.size;
			wad.type = QUAKE_TEX_ID;
			wad.cmps = 0;
			wad.dummy = 0;
			strncpy(wad.name, e.name, 16);
			m_file.Write(&wad, sizeof(wad));
		}
		m_file.Seek(4, CFile::begin);
		int num = (int)m_dir.size();
		WRITE(&m_file, num);
		WRITE(&m_file, dirOfs);
	}

	m_file.Close();
	return true;
}

CQuakePakBase::CQuakePakBase(
	ReadTextureDetailsProc readTexDetails,
	ReadTextureProc readTex,
	FreeTextureProc freeTex
) : m_readTexDetails(readTexDetails), m_readTex(readTex), m_freeTex(freeTex)
{
}

bool CQuakePakBase::ReadTextureDetails(CTexture *context, int i, int *width, int *height, int *bpp)
{
	CTexFile *file = SeekToData(i);
	bool b = m_readTexDetails(context, file, width, height, bpp);
	ReleaseFile(file);
	return b;
}

bool CQuakePakBase::ReadTexture(CTexture *context, int i, void **dst, int *width, int *height, int *bpp)
{
	CTexFile *file = SeekToData(i);
	bool b = m_readTex(context, file, dst, width, height, bpp);
	ReleaseFile(file);
	return b;
}

void CQuakePakBase::FreeTextureData(CTexture *context, void *src)
{
	m_freeTex(context, src);
}

void CQuakePakBase::ReleaseFile(CTexFile *)
{
}

CQuakePak::CQuakePak() : CQuakePakBase(CQuakeTex::ReadTextureDetails, CQuakeTex::ReadTexture, CQuakeTex::FreeTextureData)
{
}

CQuakePak::CQuakePak(
	ReadTextureDetailsProc readTexDetails,
	ReadTextureProc readTex,
	FreeTextureProc freeTex
) : CQuakePakBase(readTexDetails, readTex, freeTex)
{
}

bool CQuakePak::Open(const char *path)
{
	if (!CQuakePakBase::Open(path)) return false;

	CFile *file = File();
	int id;
	if (!SAFE_READ(file, id)) return false;
	if (id == QUAKE_PAK_ID)
	{
		m_type = BSP;
	}
	else if (id == QUAKE_WAD_ID)
	{
		m_type = WAD;
	}
	else
	{
		return false;
	}

	int dirOfs;
	int dirCount;

	if (m_type == BSP)
	{
		if (!SAFE_READ(file, dirOfs)) return false;
		if (!SAFE_READ(file, dirCount)) return false;

		dirCount /= 64;

		file->Seek(dirOfs, CFile::begin);
		
		std::vector<int> ofsVec;
		for (int i = 0; i < dirCount; ++i)
		{
			char buff[56];
			int pos, len;

			if (file->Read(buff, 56) != 56) 
			{
				return false;
			}
			if (!SAFE_READ(file, pos)) 
			{
				return false;
			}
			if (!SAFE_READ(file, len)) 
			{
				return false;
			}
			
			if (strstr(buff, ".bsp"))
			{
				ofsVec.push_back(pos);
			}
		}

		for (int i = 0; i < ofsVec.size(); ++i)
		{
			file->Seek(ofsVec[i] + sizeof(int) * 5, CFile::begin);
			int pos;
			if (!SAFE_READ(file, pos)) return false;
			int mipheaderOfs = ofsVec[i] + pos;
			file->Seek(mipheaderOfs, CFile::begin);
			int numtex;
			if (!SAFE_READ(file, numtex)) return false;
			int *ofs = new int[numtex];
			if (file->Read(ofs, sizeof(int) * numtex) != sizeof(int) * numtex)
			{
				delete [] ofs;
				return false;
			}
			int minOfs = sizeof(int) * (numtex+1);
			for (int k = 0; k < numtex; ++k)
			{
				if (ofs[k] < minOfs) continue;
				FileEntry e;
				e.pos = mipheaderOfs + ofs[k];
				e.len = -1;
				file->Seek(e.pos, CFile::begin);
				CQuakeTex::q1miptex_t tex;
				if (file->Read(&tex, sizeof(tex)) != sizeof(tex))
				{
					delete [] ofs;
					return false;
				}
				e.name = tex.name;
				e.len = tex.ofs4 + (tex.width / 8 * tex.height / 8);
				if (e.name[0] != '+' || e.name[1] == '0')
				{
					e.name.MakeLower();
					e.displayName = e.name;
					m_files.push_back(e);
				}
			}
			delete [] ofs;
		}
	}
	else
	{
		int dirOfs;
		int dirCount;

		if (!SAFE_READ(file, dirCount)) return false;
		if (!SAFE_READ(file, dirOfs)) return false;

		file->Seek(dirOfs, CFile::begin);

		for (int i = 0; i < dirCount; ++i)
		{
			FileEntry e;
			int size, dsize;
			char type, cmp;
			
			if (!SAFE_READ(file, e.pos)) return false;
			if (!SAFE_READ(file, dsize)) return false;
			if (!SAFE_READ(file, size)) return false;
			if (!SAFE_READ(file, type)) return false;
			if (!SAFE_READ(file, cmp)) return false;
			file->Seek(2, CFile::current);
			char name[16];
			if (file->Read(name, 16) != 16) return false;
			
			if (type == QUAKE_TEX_ID)
			{
				e.name = name;
				e.len  = dsize;
				if (e.name[0] != '+' || e.name[1] == '0')
				{
					e.name.MakeLower();
					e.displayName = e.name;
					m_files.push_back(e);
				}
			}
		}
	}

	return true;
}

bool CQuakePak::Initialize(const TextureFactoryList &texFactories)
{
	return true;
}

int CQuakePak::NumTextures() const
{
	return (int)m_files.size();
}

CShader *CQuakePak::Texture(int i)
{
	return new CQuakeTex(i, m_files[i].name, m_files[i].displayName, this);
}

CTexFile *CQuakePak::SeekToData(int i)
{
	m_texFile.SetFile(CFileForIndex(i));
	return &m_texFile;
}

CFile *CQuakePak::CFileForIndex(int i)
{
	FileEntry &e = m_files[i];
	CFile *file = File();
	file->Seek(e.pos, CFile::begin);
	return file;
}

CQuakePak::CTexFileAdapter::CTexFileAdapter() : m_file(0)
{
}

void CQuakePak::CTexFileAdapter::SetFile(CFile *file)
{
	m_file = file;
}

unsigned int CQuakePak::CTexFileAdapter::Seek(int ofs, unsigned int from)
{
	return m_file->Seek(ofs, from);
}

unsigned int CQuakePak::CTexFileAdapter::Read(void *buff, unsigned int size)
{
	return m_file->Read(buff, size);
}

unsigned int CQuakePak::CTexFileAdapter::GetLength() const
{
	return m_file->GetLength();
}

unsigned int CQuakePak::CTexFileAdapter::GetPosition() const
{
	return m_file->GetPosition();
}

int CQuakePak::EntrySize(int i)
{
	return m_files[i].len;
}

CQuakeTex::CQuakeTex(int i, const char *name, const char *displayName, CTextureFactory *factory) :
m_index(i),
m_factory(factory),
m_width(0),
m_height(0),
m_name(name),
m_displayName(displayName),
m_tex(0)
{
}

CQuakeTex::~CQuakeTex()
{
	Purge();
}

RTex_t *CQuakeTex::Load()
{
	if (m_tex) return m_tex;
	int bpp;
	if (!m_factory->ReadTextureDetails(this, m_index, &m_width, &m_height, &bpp)) return 0;
	m_tex = R_glCreateTexture(
		m_name,
		m_width,
		m_height,
		1,
		bpp,
		GL_TexUpload,
		0,
		0
	);
	m_tex->user_data[0] = this;
	return m_tex;
}

void CQuakeTex::Purge()
{
	if (m_tex)
	{
		R_glDeleteTexture(m_tex);
		m_tex = 0;
	}
}

bool CQuakeTex::Pickable()
{
	return true;
}

const char *CQuakeTex::Name()
{
	return m_name;
}

const char *CQuakeTex::DisplayName()
{
	return m_displayName;
}

void CQuakeTex::Dimensions(int *w, int *h)
{
	if (m_width == 0 || m_height == 0)
	{
		int bpp;
		m_factory->ReadTextureDetails(this, m_index, &m_width, &m_height, &bpp);
	}
	*w = m_width;
	*h = m_height;
}

int CQuakeTex::MemorySize()
{
	return m_width * m_height * GetBPP();
}

bool CQuakeTex::ReadTextureDetails(CTexture *context, CTexFile *file, int *width, int *height, int *bpp)
{
	file->Seek(16, CFile::current);
	if (!SAFE_READ(file, *width)) return false;
	if (!SAFE_READ(file, *height)) return false;
	*bpp = 3;
	return true;
}

bool CQuakeTex::ReadTexture(CTexture *context, CTexFile *file, void **dst, int *width, int *height, int *bpp)
{
	if (!ReadTextureDetails(context, file, width, height, bpp)) return false;
	int mipOfs[4];
	if (file->Read(mipOfs, 16) != 16) return false;
	file->Seek(mipOfs[0] - 40, CFile::current);

	int size = *width * *height;
	unsigned char *_8bit = new unsigned char[size];
	if (file->Read(_8bit, size) != size)
	{
		delete[] _8bit;
		return false;
	}

	*dst = CTextureFactory::ExpandPalettized(_8bit, *width, *height, 3, s_quakePal.Data());
	delete [] _8bit;
	return *dst != 0;
}

bool CQuakeTex::WriteNativeToFile(CFile *dstFile)
{
	CTexFile *srcFile = m_factory->SeekToData(m_index);
	int len = m_factory->EntrySize(m_index);

	unsigned char *data = new unsigned char[len];
	if (srcFile->Read(data, len) != len)
	{
		delete [] data;
		return false;
	}

	dstFile->Write(data, len);
	delete [] data;

	m_factory->ReleaseFile(srcFile);
	return true;
}

void CQuakeTex::FreeTextureData(CTexture *context, void *src)
{
	CTextureFactory::FreeTexture(src);
}

bool CQuakeTex::GL_TexUpload(RTex_t *tex)
{
	CQuakeTex *qtex = static_cast<CQuakeTex*>(tex->user_data[0]);
	int w, h, bpp;
	void *src;
	if (!qtex->m_factory->ReadTexture(qtex, qtex->m_index, &src, &w, &h, &bpp)) return false;
	R_glUploadTexture(
		tex,
		w,
		h,
		1,
		GL_UNSIGNED_BYTE,
		(bpp == 3) ? GL_RGB : GL_RGBA,
		_upf_wrap|_upf_filter,
		src,
		0);
	qtex->m_factory->FreeTextureData(qtex, src);
	return true;
}

void *CResourceLoader::Data()
{
	if (m_data) return m_data;

	HRSRC hrsrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(m_id), "RT_RCDATA");
	HGLOBAL global = LoadResource(AfxGetResourceHandle(), hrsrc);
	m_length = SizeofResource(AfxGetResourceHandle(), hrsrc);

	m_data = LockResource(global);
	return m_data;
}

int CResourceLoader::Length()
{
	Data();
	return m_length;
}