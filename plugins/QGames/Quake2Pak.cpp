///////////////////////////////////////////////////////////////////////////////
// Quake2Pak.cpp
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
#include "Quake2.h"
#include "resource.h"

#define IDPAKHEADER		(('K'<<24)+('C'<<16)+('A'<<8)+'P')
#define SAFE_READ(f, x) ((f)->Read(&(x), sizeof(x)) == sizeof(x))
#define WRITE(f, x) ((f)->Write(&(x), sizeof(x)))

static CResourceLoader s_quake2Pal(IDR_RT_QUAKE2PAL);

///////////////////////////////////////////////////////////////////////////////
// CQuake2Pak
///////////////////////////////////////////////////////////////////////////////

CQuake2Pak::CQuake2Pak() : 
CQuakePak(CQuake2Tex::ReadTextureDetails,
		  CQuake2Tex::ReadTexture,
		  CQuake2Tex::FreeTextureData)
{
}

CQuake2Pak::CQuake2Pak(
	ReadTextureDetailsProc readTexDetails,
	ReadTextureProc readTex,
	FreeTextureProc freeTex
	) :
CQuakePak(readTexDetails,
		  readTex,
		  freeTex)
{
}

bool CQuake2Pak::Open(const char *path)
{
	if (!CPakFile::Open(path)) return false;

	CFile *file = File();

	int id;
	if (!SAFE_READ(file, id)) return false;
	if (id != IDPAKHEADER) return false;

	int dirOfs;
	int dirLen;

	if (!SAFE_READ(file, dirOfs)) return false;
	if (!SAFE_READ(file, dirLen)) return false;

	file->Seek(dirOfs, CFile::begin);

	dirLen /= 64;

	const char *texExt = GetTexExt();
	int texExtLen = (int)strlen(texExt);
	for (int i = 0; i < dirLen; ++i)
	{
		char buff[56];
		int pos, len;

		if (file->Read(buff, 56) != 56) return false;
		if (!SAFE_READ(file, pos)) return false;
		if (!SAFE_READ(file, len)) return false;

		if (strstr(buff, texExt))
		{
			CString name = buff;
			name.MakeLower();
			if (name.Find("textures/") == 0)
			{
				name = name.Right(name.GetLength() - 9);
			}
			CString rev = name;
			int nameStart = rev.MakeReverse().Find('/');
			if (nameStart == -1)
			{
				nameStart = rev.Find('\\');
			}
			if (nameStart == -1)
			{
				nameStart = name.GetLength();
			}
			
			if (nameStart >= 2) // can animate.
			{
				nameStart = name.GetLength() - nameStart;
				if (name[nameStart] == '+' && isdigit(name[nameStart+1]) && name[nameStart+1] != '0')
				{
					continue;
				}
			}
			name = name.Left(name.GetLength() - texExtLen);
			FileEntry e;
			e.name = name;
			e.displayName = e.name;
			e.pos = pos;
			e.len = len;
			m_files.push_back(e);
		}
	}
	return true;
}

CShader *CQuake2Pak::Texture(int i)
{
	// The shipping quake2 assets have tons of duplicate textures. We strip out the directory prefix, and let the game class filter
	// out the duplicates.
	return new CQuake2Tex(i, m_files[i].name, CQuake2Game::TrimTextureName(m_files[i].displayName), this);
}

///////////////////////////////////////////////////////////////////////////////
// CQuake2Tex
///////////////////////////////////////////////////////////////////////////////

CQuake2Tex::CQuake2Tex(int i, const char *name, const char *displayName, CTextureFactory *factory) :
CQuakeTex(i, name, displayName, factory), m_hasDetails(false)
{
}

CQuake2Tex::~CQuake2Tex()
{
}

int CQuake2Tex::Surface()
{
	LoadDetails();
	return m_surface;
}

int CQuake2Tex::Contents()
{
	LoadDetails();
	return m_contents;
}

int CQuake2Tex::Value()
{
	LoadDetails();
	return m_value;
}

void CQuake2Tex::LoadDetails()
{
	if (!m_hasDetails)
	{
		int w, h, b;
		m_factory->ReadTextureDetails(this, m_index, &w, &h, &b);
	}
}

#define SIZEOF_Q2MIPTEX 100
#define	MIPLEVELS	4
struct q2miptex_t
{
	char		name[32];
	unsigned int width, height;
	unsigned int offsets[MIPLEVELS];		// four mip maps stored
	char		animname[32];			// next frame in animation chain
	int			flags;
	int			contents;
	int			value;
};
OS_STATIC_ASSERT(sizeof(q2miptex_t) == SIZEOF_Q2MIPTEX);

bool CQuake2Tex::ReadTextureDetails(CTexture *context, CTexFile *file, int *width, int *height, int *bpp)
{
	CQuake2Tex *tex = static_cast<CQuake2Tex*>(context);
	if (!tex->m_hasDetails)
	{
		q2miptex_t m;
		if (file->Read(&m, sizeof(q2miptex_t)) != sizeof(q2miptex_t)) return false;
		tex->m_width = m.width;
		tex->m_height = m.height;
		tex->m_surface = m.flags;
		tex->m_contents = m.contents;
		tex->m_value = m.value;
		tex->m_hasDetails = true;
	}

	*width = tex->m_width;
	*height = tex->m_height;
	*bpp = 3;
	return true;
}

bool CQuake2Tex::ReadTexture(CTexture *context, CTexFile *file, void **dst, int *width, int *height, int *bpp)
{
	int vals[3];
	file->Seek(32, CFile::current);
	if (file->Read(vals, 12) != 12) return false;
	file->Seek(vals[2] - 44, CFile::current);
	int size = vals[0] * vals[1];
	unsigned char *_8bit = new unsigned char[size];
	if (file->Read(_8bit, size) != size) return false;
	*dst = CTextureFactory::ExpandPalettized(_8bit, vals[0], vals[1], 3, s_quake2Pal.Data());
	delete [] _8bit;
	*width = vals[0];
	*height = vals[1];
	*bpp = 3;
	return true;
}

void CQuake2Tex::FreeTextureData(CTexture *context, void *src)
{
	CTextureFactory::FreeTexture(src);
}