///////////////////////////////////////////////////////////////////////////////
// Quake2.cpp
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

static CResourceLoader s_q2colorMap(IDR_RT_Q2COLORMAP);

///////////////////////////////////////////////////////////////////////////////
// CQuake2Game
///////////////////////////////////////////////////////////////////////////////

const char *CQuake2Game::Name()
{
	return "Quake2";
}

void CQuake2Game::GetExportFile(const char *filename, char *buff, int buffSize)
{
	strcpy(buff, GameDir());
	strcat(buff, "\\baseq2\\maps\\");
	strcat(buff, filename);
	StrSetFileExtension(buff, "map");
}

CPluginFileExport *CQuake2Game::NativeMapExporter()
{
	return new CQuake2Map();
}

CPluginFileExport *CQuake2Game::FileExporter(int i)
{
	switch (i)
	{
	case 0: return new CQuake2Map();
	}
	return 0;
}

const char *CQuake2Game::PakType(int i)
{
	switch (i)
	{
	case 0: return "pak";
	}
	return 0;
}

CTextureFactory *CQuake2Game::CreatePakFile()
{
	return new CQuake2Pak();
}

CQuakeUserData *CQuake2Game::CreateUserData(CTreadDoc *doc)
{
	return new CQuake2UserData(doc, this);
}

void CQuake2Game::InitializeToolsList(QuakeToolsList &tools)
{
	tools.push_back(LoadToolOpts(CQuakeTool("QBsp"), "9:base:quake2.opts"));
	tools.push_back(LoadToolOpts(CQuakeTool("QVis"), "9:base:quake2.opts"));
	tools.push_back(LoadToolOpts(CQuakeTool("QRad"), "9:base:quake2.opts"));
}

CString CQuake2Game::TrimTextureName(const char *str)
{
	CString name = str;
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
	nameStart = name.GetLength() - nameStart;
	return name.Right(name.GetLength() - nameStart);
}

int CQuake2Game::ExportBrushTexture(CMapObject *obj, void *p1, void *p2)
{
	CQBrush *brush = dynamic_cast<CQBrush*>(obj);
	if (!brush) { return 0; }
	TextureExportParms *parms = reinterpret_cast<TextureExportParms*>(p1);
	
	CFile file;
	char buff[1024], buff2[1024];
	for (int i = 0; i < brush->m_numfaces; ++i)
	{
		CShader *shader = brush->m_faces[i].p_shader;
		CQuake2Tex *q2tex = dynamic_cast<CQuake2Tex*>(shader);

		if (q2tex)
		{
			if (parms->hash.find(shader->Name()) == parms->hash.end())
			{
				strcpy(buff, parms->q2texdir);
				strcat(buff, "\\");
				strcat(buff, shader->Name());
				strcat(buff, ".wal");
				Sys_GetDirectory(buff, buff2, 1024);
				CreateDirectoryPathNative(buff2);
				if (Sys_FileExists(buff))
				{
					parms->doc->WriteToCompileWindow("(exists, skipping) %s\n", shader->Name());
				}
				else if (file.Open(buff, CFile::modeWrite|CFile::modeCreate))
				{
					parms->doc->WriteToCompileWindow("%s\n", shader->Name());
					q2tex->WriteNativeToFile(&file);
					file.Close();
				}
				else
				{
					parms->doc->WriteToCompileWindow("(failed) %s\n", shader->Name());
				}
				parms->hash.insert(StringHash::value_type(shader->Name()));
			}
		}
	}

	return 0;
}

void CQuake2Game::ExportTextures(const char *q2texdir, CTreadDoc *doc)
{
	TextureExportParms parms;
	parms.q2texdir = q2texdir;
	parms.doc = doc;
	doc->WriteToCompileWindow("***************************\nExporting WAL files\n***************************\n");
	doc->WriteToCompileWindow("export dir: %s\n", q2texdir);
	doc->GetObjectList()->WalkList(ExportBrushTexture, &parms, 0);
	doc->GetSelectedObjectList()->WalkList(ExportBrushTexture, &parms, 0);
}

void CQuake2Game::RunMapCompile(const char *inpath, QuakeToolsList &tools, CTreadDoc *doc, bool runGame)
{
	CString str = inpath;
	char mapname[1024];
	char mappath[1024];
	strcpy(mappath, inpath);
	Sys_GetFilename(mappath, mapname, 1024);
	str.MakeLower();
	if (str.Find("quake2") == -1)
	{
		doc->WriteToCompileWindow("Non standard Q2 directory detected, building in temp directory for tools compatibility.\n");

		// the hard way...
		char temp[1024];
		
		// export colormap.pcx
		GetTempPath(1024, temp);
		strcat(temp, "quake2\\baseq2\\pics\\");
		CreateDirectoryPathNative(temp);
		strcat(temp, "colormap.pcx");
		Sys_WriteFile(temp, s_q2colorMap.Data(), s_q2colorMap.Length());
		
		// export .wal files
		GetTempPath(1024, temp);
		strcat(temp, "quake2\\baseq2\\textures");
		CreateDirectoryPathNative(temp);
		ExportTextures(temp, doc);
		
		GetTempPath(1024, temp);
		strcat(temp, "quake2\\baseq2\\maps\\");
		CreateDirectoryPathNative(temp);
		strcat(temp, mapname);
		CopyFileA(mappath, temp, FALSE);

		//
		// NOTE: this run function only kicks off a thread.
		//
		CQuakeGame::RunMapCompile(temp, tools, doc, runGame);
	}
	else
	{
		CQuakeGame *game = static_cast<CQuakeGame*>(doc->GamePlugin());

		char path[1024];
		strcpy(path, game->GameDir());
		strcat(path, "\\baseq2\\pics");
		CreateDirectoryPathNative(path);
		strcat(path, "\\colormap.pcx");
		Sys_WriteFile(path, s_q2colorMap.Data(), s_q2colorMap.Length());

		strcpy(path, game->GameDir());
		strcat(path, "\\baseq2\\textures");
		ExportTextures(path, doc);

		//
		// NOTE: this run function only kicks off a thread.
		//
		CQuakeGame::RunMapCompile(mappath, tools, doc, runGame);
	}
}

void CQuake2Game::GetLeakFileName(CTreadDoc *doc, char *buff, int buffSize)
{
	doc->GetMapExportPathName(buff, buffSize);
	StrSetFileExtension(buff, "lin");
}

bool CQuake2Game::LeakTraceExists(CTreadDoc *doc)
{
	char buff[1024];
	GetLeakFileName(doc, buff, 1024);
	return Sys_FileExists(buff);
}

CQuakeCompiler *CQuake2Game::CreateCompiler(const char *filename, QuakeToolsList &tools, CTreadDoc *doc, bool runMap)
{
	return new CQuake2Compiler(filename, tools, doc, runMap);
}

///////////////////////////////////////////////////////////////////////////////
// CQuake2Compiler
///////////////////////////////////////////////////////////////////////////////

CQuake2Compiler::CQuake2Compiler(const char *filename, QuakeToolsList &tools, CTreadDoc *doc, bool runMap) :
CQuakeCompiler(filename, tools, doc, false), m_myRunMap(runMap)
{
}

void CQuake2Compiler::ThreadProc()
{
	CQuakeCompiler::ThreadProc();

	CString str = m_realmappath;
	str.MakeLower();
	if (str.Find("quake2") == -1) // copy files.
	{
		char temp[1024];
		char mappath[1024];
		strcpy(temp, m_filename);
		strcpy(mappath, m_realmappath);

		StrSetFileExtension(temp, "lin");
		StrSetFileExtension(mappath, "lin");
		CopyFileA(temp, mappath, FALSE);

		StrSetFileExtension(temp, "bsp");
		StrSetFileExtension(mappath, "bsp");
		BOOL b = CopyFileA(temp, mappath, FALSE);
		OS_ASSERT(b);
	}

	if (m_myRunMap)
	{
		m_doc->WriteToCompileWindow("************\nRunning %s\n************\n", m_doc->GamePlugin()->Name());
		m_doc->GamePlugin()->RunGame(m_realmappath);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CQuake2UserData
///////////////////////////////////////////////////////////////////////////////

void CQuake2UserData::MakeBrushMenu(CTreadDoc *doc)
{
	if( m_BrushMenuCreated )
		return;

	m_BrushMenu.AddMenuItem( BM_EDIT_FACES, "Edit Faces" );
	m_BrushMenu.AddMenuItem( BM_EDIT_VERTS, "Edit Verts" );
	m_BrushMenu.AddMenuItem( BM_LOCK_TEXTURE, "Lock Texture" );
	m_BrushMenu.AddMenuItem( BM_GRAB_TEXTURE, "Grab Texture" );
	m_BrushMenu.AddMenuItem( BM_APPLY_TEXTURE, "Apply Texture" );
	m_BrushMenu.AddMenuItem( BM_SELECT_ALL_WITH_TEXTURE, "Select All w/ Texture" );
//	m_BrushMenu.AddMenuItem( 15, "Edit Shader" );
	//m_BrushMenu.AddMenuItem( 7, "Mapping\nPlanar X");
	//m_BrushMenu.AddMenuItem( 8, "Mapping\nPlanar Y");
	//m_BrushMenu.AddMenuItem( 6, "Mapping\nPlanar Z");
	//m_BrushMenu.AddMenuItem( 9, "Mapping\nPlanar Auto");
	m_BrushMenu.AddMenuItem( BM_SNAP_TO_GRID, "Snap To Grid" );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( BM_CSG_HOLLOW, "Hollow..." );
	m_BrushMenu.AddMenuItem( BM_CSG_CARVE, "Carve" );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
#if 0
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_NODRAW, "Brush Filter\nNo Draw", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_SOLID, "Brush Filter\nSolid", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_WINDOW, "Brush Filter\nWindow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_WATER, "Brush Filter\nWater", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_AREAPORTAL, "Brush Filter\nAreaportal", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_DETAIL, "Brush Filter\nDetail", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_NOSHADOW, "Brush Filter\nNo Shadow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_ALWAYSSHADOW, "Brush Filter\nAlways Shadow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_MONSTECLIP, "Brush Filter\nMonster Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_PLAYERCLIP, "Brush Filter\nPlayer Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_CAMERACLIP, "Brush Filter\nCamera Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_CORONABLOCK, "Brush Filter\nCorona Block", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_SKYBOX, "Brush Filter\nSky Box", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_SKYPORTAL, "Brush Filter\nSky Portal", true );
	m_BrushMenu.AddMenuItem( 0, "Brush Filter\n@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_FILTERALL, "Brush Filter\nFilter All", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_FILTERNONE, "Brush Filter\nFilter None", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_FILTERALLCLIP, "Brush Filter\nFilter All Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_FILTERALLSKY, "Brush Filter\nFilter All Sky", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_REAPPLYFILTER, "Brush Filter\nRe-Apply Filter", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_NODRAW, "Select Brushes By Attributes\nNo Draw", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SOLID, "Select Brushes By Attributes\nSolid", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_WINDOW, "Select Brushes By Attributes\nWindow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_WATER, "Select Brushes By Attributes\nWater", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_AREAPORTAL, "Select Brushes By Attributes\nAreaportal", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_DETAIL, "Select Brushes By Attributes\nDetail", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_NOSHADOW, "Select Brushes By Attributes\nNo Shadow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_ALWAYSSHADOW, "Select Brushes By Attributes\nAlways Shadow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_MONSTERCLIP, "Select Brushes By Attributes\nMonster Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_PLAYERCLIP, "Select Brushes By Attributes\nPlayer Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_CAMERACLIP, "Select Brushes By Attributes\nCamera Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_CORONABLOCK, "Select Brushes By Attributes\nCorona Block", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SKYBOX, "Select Brushes By Attributes\nSky Box", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SKYPORTAL, "Select Brushes By Attributes\nSky Portal", true );
#endif
	m_BrushMenu.AddMenuItem( BM_SELECT_BRUSHES_WITH_SAME_CONTENTS, "Select Brushes With Same Contents" );
	m_BrushMenu.AddMenuItem( BM_SELECT_BRUSHES_WITH_SAME_SURFACE, "Select Brushes With Same Surface Values" );

	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_CHECKSELECTEDOBJECTS, "Check Selected Objects For Errors...", true );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );

	//
	// add link to entity objects.
	//
	CLinkedList<CEntDef>* entdefs = doc->GameDef()->EntDefList();
	CEntDef* def;
	int id = BM_FIRST_LINK_TO_ENTITY_MENU;

	for( def = entdefs->ResetPos(); def; def = entdefs->GetNextItem() )
	{
		if( def->IsOwner() && !def->IsBaseClass() )
		{
			CString s;

			s.Format( "Link To Entity\n%s", def->GetTreeDisplayName());
			m_BrushMenu.AddMenuItem( id++, s );
		}
	}

	m_BrushMenu.AddMenuItem( BM_UNLINK_FROM_ENTITY, "Unlink From Entity" );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_HIDE, "Hide Selected", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SHOWALL, "Show All", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_HIDEALL, "Hide All" , true );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_MAKEGROUP, "Make Group", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_MAKEGROUPANDHIDE, "Make Group and Hide", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_HIDEALLINGROUP, "Hide All In Group(s)", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SHOWALLINGROUPS, "Show All In Group(s)", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTALLINGROUPS, "Select All In Group(s)", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_REMOVEFROMGROUPS, "Remove From Group(s)", true );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_FLIPVERTICAL, "Flip Vertical", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_FLIPHORIZONTAL, "Flip Horizontal", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_FLIPONXAXIS, "Flip On X Axis", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_FLIPONYAXIS, "Flip On Y Axis", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_FLIPONZAXIS, "Flip On Z Axis", true );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_ROTATE90CW, "Rotate 90º CW", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_ROTATE90CCW, "Rotate 90º CCW", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_ROTATE180, "Rotate 180º", true );

	m_BrushMenuCreated = true;
}