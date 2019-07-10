///////////////////////////////////////////////////////////////////////////////
// Quake3Pak.cpp
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
#include "Quake3Pak.h"
#include "Tga.h"
#include "Jpg.h"
#include <algorithm>

#undef min
#undef max

///////////////////////////////////////////////////////////////////////////////
// CBuffReader
///////////////////////////////////////////////////////////////////////////////

CBuffReader::CBuffReader(const unsigned char *data, int len) : m_data(data), m_len(len), m_ofs(0)
{
}

CBuffReader::~CBuffReader()
{
	if (m_data)
	{
		delete[] m_data;
	}
}

unsigned int CBuffReader::Seek(int ofs, unsigned int from)
{
	switch (from)
	{
	case begin:
		m_ofs = ofs;
		break;
	case end:
		m_ofs = m_len + ofs;
		break;
	case current:
		m_ofs += ofs;
		break;
	}

	m_ofs = std::max(0, std::min(m_len, m_ofs));
	return m_ofs;
}

unsigned int CBuffReader::Read(void *buff, unsigned int size)
{
	size = std::min<unsigned int>(size, m_len-m_ofs);
	memcpy(buff, m_data+m_ofs, size);
	m_ofs += size;
	return size;
}

unsigned int CBuffReader::GetLength() const
{
	return m_len;
}

unsigned int CBuffReader::GetPosition() const
{
	return m_ofs;
}

///////////////////////////////////////////////////////////////////////////////
// CQuake3ShaderFile
///////////////////////////////////////////////////////////////////////////////

bool CQuake3ShaderFile::Parse(const void *data, int len, Quake3ShaderList &list)
{
	C_Tokenizer script;
	if (!script.InitParsing((const char*)data, len)) return false;

	CQuake3Shader shader;
	shader.fake = false;
	while (ParseShader(script, shader))
	{
		list.push_back(shader);
		shader.maps.clear();
	}

	return true;
}

bool CQuake3ShaderFile::ParseShader(C_Tokenizer &script, CQuake3Shader &shader)
{
	if (!script.GetToken(shader.name)) return false;
	int brace = 0;
	bool foundMap = false;
	
	for (;;)
	{
		CString t;
		if (!script.GetToken(t)) return false;
		if (t == "{")
		{
			++brace;
		}
		else if (t == "}")
		{
			--brace;
		}

		if (0 == brace) break;

		if (t == "map")
		{
			script.GetToken(t);

			if (t != "$lightmap")
			{
				if (!foundMap)
				{
					foundMap = true;
					shader.map = t;
				}
				shader.maps.push_back(t);
			}
		}
		else if (t == "qer_editorimage")
		{
			if (!script.GetToken(shader.map)) return false;
			foundMap = true;
		}
	}

	return foundMap;
}

///////////////////////////////////////////////////////////////////////////////
// CQuake3PakBase
///////////////////////////////////////////////////////////////////////////////

CQuake3PakBase::CQuake3PakBase() : CQuakePakBase(&CQuake3Tex::ReadTextureDetails, &CQuake3Tex::ReadTexture, &CQuake3Tex::FreeTextureData)
{
}

CQuake3PakBase::CQuake3PakBase(
	ReadTextureDetailsProc readTexDetails,
	ReadTextureProc readTex,
	FreeTextureProc freeTex
) : CQuakePakBase(readTexDetails, readTex, freeTex)
{
}

bool CQuake3PakBase::Open(const char *path)
{
	return true; // quake3 paks use unzip to open.
}

CString CQuake3PakBase::TrimTextureName(const char *path, bool extOnly)
{
	CString s = path;
	if (s[s.GetLength()-4] == '.')
	{
		s.Delete(s.GetLength()-4, 4);
	}
	if (!extOnly) { s.Delete(0, 9); }
	return s;
}

CQuake3PakBase::FileType CQuake3PakBase::ClassifyFile(const CString &name)
{
	if (name.Find(".shader") != -1)
	{
		return SHADER;
	}

	if ((name.Find("textures/") != -1) && (name.Find(".tga") != -1 || name.Find(".jpg") != -1))
	{
		return TEXTURE;
	}

	return SKIP;
}

///////////////////////////////////////////////////////////////////////////////
// CQuake3Tex
///////////////////////////////////////////////////////////////////////////////

CQuake3Tex::CQuake3Tex(int i, const char *name, const char *displayName, CTextureFactory *factory) 
: CQuakeTex(i, name, displayName, factory), m_bpp(0), m_hasDetails(false)
{
}

CQuake3Tex::~CQuake3Tex()
{
}

int CQuake3Tex::GetBPP()
{
	return m_bpp;
}

bool CQuake3Tex::ReadTextureDetails(CTexture *context, CTexFile *file, int *width, int *height, int *bpp)
{
	CQuake3Tex *tex = static_cast<CQuake3Tex*>(context);
	if (!tex->m_hasDetails)
	{
		tex->m_hasDetails = true;
		const int BUFF_SIZE = 1024*16;
		unsigned char buff[BUFF_SIZE];
		int len = file->Read(buff, BUFF_SIZE);

		if (CTga::IsTga(buff, len))
		{
			return CTga::ReadInfo(buff, BUFF_SIZE, width, height, bpp);
		}
		else
		{
			return CJpg::ReadInfo(buff, BUFF_SIZE, width, height, bpp);
		}
	}
	return true;
}

bool CQuake3Tex::ReadTexture(CTexture *context, CTexFile *file, void **dst, int *width, int *height, int *bpp)
{
	int len = file->GetLength();
	unsigned char *data = new unsigned char[len];
	/*CString s;
	s.Format("name %s, len %d, data %x\n", static_cast<CQuake3Tex*>(context)->Name(), len, data);
	OutputDebugString(s);*/
	if (file->Read(data, len) != len)
	{
		delete[] data;
		return false;
	}

	*width = 0;
	*height = 0;

	if (CTga::IsTga(data, len))
	{
		CTga::Read(data, len, dst, width, height, bpp);
	}
	else
	{
		CJpg::Read(data, len, dst, width, height, bpp);
	}

	delete [] data;
	return true;
}

void CQuake3Tex::FreeTextureData(CTexture *context, void *src)
{
	delete [] reinterpret_cast<unsigned char*>(src);
}

///////////////////////////////////////////////////////////////////////////////
// CQuake3PakTex
///////////////////////////////////////////////////////////////////////////////

CQuake3Pak::CQuake3PakTex::CQuake3PakTex(ZipEntry *entry, int i, const char *name, const char *displayName, CTextureFactory *factory) :
	CQuake3Tex(i, name, displayName, factory), m_entry(entry)
{
}

CQuake3Pak::CQuake3PakTex::~CQuake3PakTex()
{
}

CQuake3Pak::ZipEntry *CQuake3Pak::CQuake3PakTex::Entry() const
{
	return m_entry;
}

int CQuake3Pak::CQuake3PakTex::DiskSize() const
{
	return m_entry->info.cur_file_info.uncompressed_size;
}

///////////////////////////////////////////////////////////////////////////////
// CQuake3Pak
///////////////////////////////////////////////////////////////////////////////

CQuake3Pak::CQuake3Pak() : m_zipFile(0)
{
}

CQuake3Pak::CQuake3Pak(
	ReadTextureDetailsProc readTexDetails,
	ReadTextureProc readTex,
	FreeTextureProc freeTex
) : CQuake3PakBase(readTexDetails, readTex, freeTex), m_zipFile(0)
{
}

CQuake3Pak::~CQuake3Pak()
{
	for (TextureList::iterator it = m_textureList.begin(); it != m_textureList.end(); ++it)
	{
		delete *it;
	}

	if (m_zipFile)
	{
		unzClose(m_zipFile);
	}
}

bool CQuake3Pak::Open(const char *path)
{
	if (!CQuake3PakBase::Open(path)) return false;

	m_zipFile = unzOpen(path);
	if (!m_zipFile) return false;

	if (unzGoToFirstFile(m_zipFile) != UNZ_OK) return false;

	do
	{
		unz_file_info info;
		char name[1024];
		unzGetCurrentFileInfo(m_zipFile, &info, name, 1024, 0, 0, 0, 0);
		CString s = name;
		s.MakeLower();
		s.Replace('\\', '/');

		ZipEntry entry;
		entry.name = s;
		entry.size = info.uncompressed_size;
		entry.info = *reinterpret_cast<unz_s*>(m_zipFile);
		entry.file = m_zipFile;
		std::pair<FileMap::iterator, bool> it = m_files.insert(FileMap::value_type((const char*)s, entry));
		
		FileType type = ClassifyFile(s);

		switch (type)
		{
		case TEXTURE:

			{
				CQuake3PakTex *tex = new CQuake3PakTex(
					&(it.first->second),
					(int)m_textureList.size(), 
					s,
					s,
					this
				);

				m_textureList.push_back(tex);
				m_textureMap.insert(TextureMap::value_type((const char*)s, tex));
			}

			break;

		case SHADER:

			{
				int len;
				void *source = LoadFile(&entry, &len);
				if (source)
				{
					Quake3ShaderList shaders;
					if (CQuake3ShaderFile::Parse(source, len, shaders))
					{
						for (Quake3ShaderList::iterator it = shaders.begin(); it != shaders.end(); ++it)
						{
							Shader s;
							s.info = *it;
							char file[1024];
							Sys_GetFilename(name, file, 1024);
							StrSetFileExtension(file, "");
							s.info.file = file;
							s.info.path = name;
							s.tex = 0;
							std::pair<ShaderMap::iterator, bool> it2 = m_shaderMap.insert(ShaderMap::value_type((const char*)s.info.name, s));
							m_shaderList.push_back(&(it2.first->second));
						}
					}
					delete[] (unsigned char*)source;
				}
			}

			break;

		case SKIP: break;
		}

	} while(unzGoToNextFile(m_zipFile) == UNZ_OK);

	return !m_files.empty();
}

bool CQuake3Pak::Initialize(const TextureFactoryList &tfl)
{
	for (ShaderList::iterator it = m_shaderList.begin(); it != m_shaderList.end();)
	{
		Shader &s = **it;
		char path[1024];
		strcpy(path, s.info.map);
		s.info.displayName = "* " + TrimTextureName(s.info.name);
		StrSetFileExtension(path, ".jpg");
		s.tex = FindTextureInPaks(path, tfl);
		if (!s.tex)
		{
			StrSetFileExtension(path, ".tga");
			s.tex = FindTextureInPaks(path, tfl);
			if (!s.tex)
			{
				OutputDebugString("q3 missing: ");
				OutputDebugString(s.info.map);
				OutputDebugString(" (tga/jpg)\n");
				it = m_shaderList.erase(it);
			}
			else
			{
				s.info.map = path;
				for (StringList::iterator it2 = s.info.maps.begin(); it2 != s.info.maps.end(); ++it2)
				{
					strcpy(path, *it2);
					StrSetFileExtension(path, ".jpg");
					if (!FindTextureInPaks(path, tfl))
					{
						StrSetFileExtension(path, ".tga");
						if (!FindTextureInPaks(path, tfl))
						{
							strcpy(path, *it2);
						}
					}
					*it2 = path;
				}

				++it;
			}
		}
		else
		{
			++it;
		}
	}
#if 1
	for (TextureList::iterator it = m_textureList.begin(); it != m_textureList.end(); ++it)
	{
		CQuake3PakTex &tex = **it;
		CString file = TrimTextureName(tex.Name(), true);
		if (!FindShaderInPaks(file, tfl))
		{
			Shader s;
			s.info.map = tex.Name();
			s.info.maps.push_back(tex.Name());
			s.info.name  = tex.Name();
			s.info.file = file;
			s.info.fake = true;
			s.info.displayName = TrimTextureName(s.info.name);
			s.tex = &tex;
			std::pair<ShaderMap::iterator, bool> it2 = m_shaderMap.insert(ShaderMap::value_type((const char*)s.info.name, s));
			m_shaderList.push_back(&(it2.first->second));
		}
	}
#endif
	return true;
}

bool CQuake3Pak::ExportFile(CTreadDoc *doc, const char *dir, const char *file) const
{
	CString sdir = dir;
	CString sfile = file;
	sfile.Replace('\\', '/');
	sdir.Replace('\\', '/');
	sdir = sdir + "/" + sfile;
	if (Sys_FileExists(sdir))
	{
		doc->WriteToCompileWindow("(exists, skipping) %s\n", file);
		return true; // skip.
	}
	
	FileMap::const_iterator it = m_files.find((const char*)sfile);
	if (it != m_files.end())
	{
		char path[1024];
		sdir.Replace('/', '\\');
		Sys_GetDirectory(sdir, path, 1024);
		CreateDirectoryPathNative(path);
		FILE *fp = fopen(sdir, "wb");
		if (fp)
		{
			doc->WriteToCompileWindow("%s\n", file);
			int len;
			unsigned char *data = LoadFile(const_cast<ZipEntry*>(&(it->second)), &len);
			if (data)
			{
				fwrite(data, 1, len, fp);
				delete[] data;
			}
			fclose(fp);
		}
	}

	return it != m_files.end();
}

int CQuake3Pak::NumTextures() const
{
	return (int)m_shaderList.size();
}

CShader *CQuake3Pak::Texture(int i)
{
	Shader *shader = m_shaderList[i];
	CString name = TrimTextureName(shader->info.name);
	return new CQuake3ShaderTex(&shader->info, name, shader->info.displayName, shader->tex);
}

CTexFile *CQuake3Pak::SeekToData(int i)
{
	unsigned char *data;
	int len;

	CQuake3PakTex *tex = m_textureList[i];
	data = LoadFile(tex->Entry(), &len);

	if (0 == data || 0 == len) return 0;

	return new CBuffReader(data, len);
}

void CQuake3Pak::ReleaseFile(CTexFile *file)
{
	delete file;
}

bool CQuake3Pak::ReadTextureDetails(CTexture *context, int i, int *width, int *height, int *bpp)
{
	CQuake3Tex *tex = static_cast<CQuake3Tex*>(context);
	if (!tex->m_hasDetails)
	{
		return CQuake3PakBase::ReadTextureDetails(context, i, width, height, bpp);
	}
	return true;
}

int CQuake3Pak::EntrySize(int i)
{
	Shader *shader = m_shaderList[i];
	return shader->tex->DiskSize();
}

unsigned char *CQuake3Pak::LoadFile(ZipEntry *entry, int *len)
{
	*len = 0;
	*reinterpret_cast<unz_s*>(entry->file) = entry->info;
	if (unzOpenCurrentFile(entry->file) != UNZ_OK) return 0;
	*len = entry->info.cur_file_info.uncompressed_size;
	unsigned char *data = new unsigned char[(*len + 4095)/4096*4096];
	if (unzReadCurrentFile(entry->file, data, *len) != *len)
	{
		delete [] data;
		*len = 0;
		return 0;
	}
	unzCloseCurrentFile(entry->file);
	return data;
}

CQuake3Tex *CQuake3Pak::TextureForName(const char *name)
{
	TextureMap::iterator it = m_textureMap.find(name);
	if (it == m_textureMap.end()) return 0;
	return it->second;
}

bool CQuake3Pak::HasShader(const char *name)
{
	return m_shaderMap.find(name) != m_shaderMap.end();
}

CQuake3Tex *CQuake3Pak::FindTextureInPaks(const char *name, const TextureFactoryList &tfl)
{
	for (TextureFactoryList::const_iterator it = tfl.begin(); it != tfl.end(); ++it)
	{
		CQuake3PakBase *q3pak = dynamic_cast<CQuake3PakBase*>(*it);
		if (q3pak)
		{
			CQuake3Tex *tex = q3pak->TextureForName(name);
			if (tex) return tex;
		}
	}

	return 0;
}

bool CQuake3Pak::FindShaderInPaks(const char *name, const TextureFactoryList &tfl)
{
	for (TextureFactoryList::const_iterator it = tfl.begin(); it != tfl.end(); ++it)
	{
		CQuake3PakBase *q3pak = dynamic_cast<CQuake3PakBase*>(*it);
		if (q3pak)
		{
			if (q3pak->HasShader(name)) return true;
		}
	}

	return false;
}