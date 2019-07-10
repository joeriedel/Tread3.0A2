///////////////////////////////////////////////////////////////////////////////
// Quake.cpp
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
#include "Quake.h"
#include "QuakeConfig.h"
#include "QuakeBuildOptionsDialog.h"
#include <hash_map>
#include <string>
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PREFS_STRING_SIZE 256

///////////////////////////////////////////////////////////////////////////////
// CQuakeTool
///////////////////////////////////////////////////////////////////////////////

CQuakeTool::CQuakeTool() : val(0), enabled(true)
{
}

CQuakeTool::CQuakeTool(const char *str) : val(0), enabled(true), name(str)
{
}

CString CQuakeTool::MakeCommandLine() const
{
	CString s = cmdLine;
	if (!runCmdLine.IsEmpty())
	{
		s += CString(" ") + runCmdLine;
	}
	for (QuakeToolCmdLineOptionList::const_iterator it = opts.begin(); it != opts.end(); ++it)
	{
		const CQuakeToolCmdLineOption &opt = *it;
		if (opt.always || val & opt.val)
		{
			s += CString(" ") + opt.cmdline;
		}
	}
	return s;
}

void CQuakeTool::CleanValues()
{
	int newVal = 0;
	for (QuakeToolCmdLineOptionList::const_iterator it = opts.begin(); it != opts.end(); ++it)
	{
		const CQuakeToolCmdLineOption &opt = *it;
		if (val & opt.val)
		{
			newVal |= opt.val;
		}
	}
	val = newVal;
}

///////////////////////////////////////////////////////////////////////////////
// CQuakeCompiler
///////////////////////////////////////////////////////////////////////////////

CQuakeCompiler::CQuakeCompiler(const char *filename, QuakeToolsList &tools, CTreadDoc *doc, bool runMap) : 
m_abort(false), 
m_doc(doc),
m_filename(filename),
m_runMap(runMap),
m_tools(tools)
{
	char temp[1024];
	temp[0] = 0;
	doc->GetMapExportPathName(temp, 1024);
	m_realmappath = temp;
}

CQuakeCompiler::~CQuakeCompiler()
{
}

void CQuakeCompiler::Run()
{
	StartThread();
}

void CQuakeCompiler::Abort()
{
	m_abort = true;
}

void CQuakeCompiler::StartThread()
{
	m_thread = CreateThread(0, 0, StaticThreadProc, this, 0, 0);
}

CQuakeTool *CQuakeCompiler::ToolForName(const char *name)
{
	for (int i = 0; i < m_tools.size(); ++i)
	{
		if (m_tools[i].name == name) return &m_tools[i];
	}
	return 0;
}

void CQuakeCompiler::ThreadProc()
{
	CQuakeGame *game = static_cast<CQuakeGame*>(m_doc->GamePlugin());
	char gameDir[1024];
	strcpy(gameDir, game->GameDir());
	char bspFile[1024];
	strcpy(bspFile, m_filename);
	StrSetFileExtension(bspFile, "");
	CString cmdLine;

	int start = ReadTickMilliseconds();

	for (QuakeToolsList::iterator it = m_tools.begin(); it != m_tools.end(); ++it)
	{
		if (m_abort) break;
		CQuakeTool &tool = *it;

		if (tool.enabled)
		{
			cmdLine = ADD_QUOTES(tool.path) + CString(" ") + tool.MakeCommandLine() + CString(" ") + ADD_QUOTES(bspFile);
			m_doc->WriteToCompileWindow("************\nExecing %s: %s\n************\n", tool.name, cmdLine);
			
			RunConsoleProcess(
				0, 
				cmdLine,
				gameDir,
				m_doc,
				&m_abort
			);
		}
		else
		{
			m_doc->WriteToCompileWindow("SKIPPING %s\n", tool.name);
		}
	}

	int end = ReadTickMilliseconds();

	m_doc->WriteToCompileWindow("************\n\nelapsed time: %d second(s)\n", (end-start)/1000);
	m_doc->WriteToCompileWindow("done.\n");

	PlaySound( "SystemAsterisk" , 0, SND_ALIAS|SND_ASYNC);

	if (m_runMap)
	{
		m_doc->WriteToCompileWindow("************\nRunning %s\n************\n", game->Name());
		game->RunGame(m_filename);
	}
}	

DWORD WINAPI CQuakeCompiler::StaticThreadProc(void *parm)
{
	CQuakeCompiler *c = (CQuakeCompiler*)parm;
	c->m_doc->SetMapCompiler(c);
	c->ThreadProc();
	c->m_doc->SetMapCompiler(0);
	CloseHandle(c->m_thread);
	delete c;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// CQuakeGame
///////////////////////////////////////////////////////////////////////////////

CQuakeGame::CQuakeGame() : 
m_type(CQBrush::TYPE_Q1), 
m_shadersLoaded(false)
{
}

CQuakeGame::CQuakeGame(CQBrush::Type brushType) : 
m_type(brushType),
m_shadersLoaded(false)
{
}

void CQuakeGame::Initialize(PrefFileSection_t *prefs)
{
	m_tools.clear();
	InitializeToolsList(m_tools);
	char buff[PREFS_STRING_SIZE];

	PrefFileSectionGetString(prefs, "EXEPath", "", buff, PREFS_STRING_SIZE);
	m_exePath = buff;
	PrefFileSectionGetString(prefs, "EXECmdLine", "", buff, PREFS_STRING_SIZE);
	m_exeCmdLine = buff;
	PrefFileSectionGetString(prefs, "GameDir", "", buff, PREFS_STRING_SIZE);
	m_gameDir = buff;

	for (QuakeToolsList::iterator it = m_tools.begin(); it != m_tools.end(); ++it)
	{
		CQuakeTool &tool = *it;
		PrefFileSectionGetString(prefs, CString(tool.name) + "_path", "", buff, PREFS_STRING_SIZE);
		tool.path = buff;
		PrefFileSectionGetString(prefs, CString(tool.name) + "_cmdline", "", buff, PREFS_STRING_SIZE);
		tool.cmdLine = buff;
	}

	for (int i = 0; ; ++i)
	{
		CString s;
		s.Format("pak_%d", i);
		PrefFileSectionGetString(prefs, s, "", buff, PREFS_STRING_SIZE);
		if (buff[0] == 0) break;
		m_paks.push_back(buff);
	}
}

void CQuakeGame::SaveSettings(PrefFileSection_t *prefs)
{
	// NOTE: Tread always clears the section contents, so save everything here!
	PrefFileSectionPutString(prefs, "EXEPath", m_exePath);
	PrefFileSectionPutString(prefs, "EXECmdLine", m_exeCmdLine);
	PrefFileSectionPutString(prefs, "GameDir", m_gameDir);

	for (QuakeToolsList::iterator it = m_tools.begin(); it != m_tools.end(); ++it)
	{
		CQuakeTool &tool = *it;
		PrefFileSectionPutString(prefs, CString(tool.name) + "_path", tool.path);
		PrefFileSectionPutString(prefs, CString(tool.name) + "_cmdline", tool.cmdLine);
	}

	for (int i = 0; i < m_paks.size(); ++i)
	{
		CString s;
		s.Format("pak_%d", i);
		PrefFileSectionPutString(prefs, s, m_paks[i]);
	}
}

void CQuakeGame::Release()
{
	for (TextureFactoryList::iterator it = m_tfl.begin(); it != m_tfl.end(); ++it)
	{
		delete *it;
	}
	m_tfl.clear();
	delete this;
}

CQuakeTool &CQuakeGame::LoadToolOpts(CQuakeTool &tool, const char *filename)
{
	unsigned long len;
	void *data = LoadAFile(filename, &len);
	if (!data) return tool;
	C_Tokenizer script;
	if (!script.InitParsing((const char*)data, len))
	{
		OS_DeallocAPointer(data);
		return tool;
	}
	OS_DeallocAPointer(data);
	int num = 0;
	if (script.FindTokeni(tool.name))
	{
		if (!script.IsNextToken("{")) return tool;
		CString t;
		for (;;)
		{
			if (!script.GetToken(t)) break;
			if (t == "}") break;
			CQuakeToolCmdLineOption opt;
			opt.name = t;
			if (!script.IsNextToken("="))
			{
				OS_OkAlertMessage("Tool opts parsing error!", "Expected =");
				return tool;
			}
			if (!script.GetToken(t))
			{
				OS_OkAlertMessage("Tool opts parsig error!", "Expected token!");
				return tool;
			}
			opt.cmdline = t;
			opt.val = 1 << (num++);
			
			if (!script.IsNextToken(":"))
			{
				tool.opts.push_back(opt);
				script.UngetToken();
				continue;
			}
			
			if (!script.GetToken(t))
			{
				OS_OkAlertMessage("Tool opts parsig error!", "Expected token!");
				return tool;
			}
			
			if (!stricmp(t, "on"))
			{
				tool.val |= opt.val;
			}
			else if (!stricmp(t, "always"))
			{
				opt.always = true;
				opt.val = 0;
				--num;
			}
			tool.opts.push_back(opt);
		}
	}
	return tool;
}

void CQuakeGame::InitializeToolsList(QuakeToolsList &tools)
{
	tools.push_back(LoadToolOpts(CQuakeTool("QBsp"), "9:base:quake.opts"));
	tools.push_back(LoadToolOpts(CQuakeTool("QVis"), "9:base:quake.opts"));
	tools.push_back(LoadToolOpts(CQuakeTool("QLight"), "9:base:quake.opts"));
}

CQuakeTool CQuakeGame::ToolForName(const char *name) const
{
	for (int i = 0; i < m_tools.size(); ++i)
	{
		if (m_tools[i].name == name) return m_tools[i];
	}

	return CQuakeTool();
}

void CQuakeGame::GetToolsList(QuakeToolsList &out) const
{
	out = m_tools;
}

const QuakePakList &CQuakeGame::PakList() const
{
	return m_paks;
}

const char *CQuakeGame::EXEPath() const
{
	return m_exePath;
}

const char *CQuakeGame::EXECmdLine() const
{
	return m_exeCmdLine;
}

const char *CQuakeGame::GameDir() const
{
	return m_gameDir;
}

const TextureFactoryList &CQuakeGame::GetTextureFactoryList() const
{
	return m_tfl;
}

const char *CQuakeGame::Name()
{
	return "Quake";
}

bool CQuakeGame::AllowBrushes()
{
	return true;
}

bool CQuakeGame::WantsConfiguration()
{
	return true;
}

void CQuakeGame::Configure()
{
	CString tempExePath = m_exePath;
	CString tempGameDir = m_gameDir;
	CString tempExeCmdLine = m_exeCmdLine;
	QuakeToolsList tempTools = m_tools;
	QuakePakList  tempPaks = m_paks;

	CString s;
	for (int i = 0; ; ++i)
	{
		const char *c = PakType(i);
		if (!c) break;
		if (i > 0) s += ";";
		s += CString("*.") + c;
	}

	CQuakeConfig dlg(Name(), s, tempExePath, tempGameDir, tempExeCmdLine, tempTools, tempPaks);
	if (dlg.DoModal() == IDOK)
	{
		m_exePath = tempExePath;
		m_gameDir = tempGameDir;
		m_exeCmdLine = tempExeCmdLine;
		m_tools = tempTools;
		m_paks = tempPaks;
		m_shadersLoaded = false;
		m_shaders.DestroyList();
		m_tfl.clear();
		Sys_RepaintAllShaders();
		Sys_RedrawWindows(VIEW_FLAG_ALL);
	}
}

CPluginFileExport *CQuakeGame::NativeMapExporter()
{
	return new CQuakeMap();
}

void CQuakeGame::GetExportFile(const char *filename, char *buff, int buffSize)
{
	strcpy(buff, m_gameDir);
	strcat(buff, "\\id1\\maps\\");
	strcat(buff, filename);
	StrSetFileExtension(buff, "map");
}

CPluginFileExport *CQuakeGame::FileExporter(int i)
{
	switch (i)
	{
	case 0:
		return new CQuakeMap();
	}
	return 0;
}	

const char *CQuakeGame::PakType(int i)
{
	switch (i)
	{
	case 0: return "pak";
	case 1: return "wad";
	}

	return 0;
}

CLinkedList<CShader> *CQuakeGame::ShaderList()
{
	if (!m_shadersLoaded)
	{
		typedef stdext::hash_map<std::string, CShader*> ShaderHash;
		ShaderHash hash;
		for (QuakePakList::iterator it = m_paks.begin(); it != m_paks.end(); ++it)
		{
			CTextureFactory *tf = CreatePakFile();
			if (!tf->Open(*it))
			{
				delete tf;
				continue;
			}
			m_tfl.push_back(tf);
		}
		for (int i = 0; i < m_tfl.size();)
		{
			CTextureFactory *tf = m_tfl[i];
			if (tf->Initialize(m_tfl))
			{
				for (int k = 0; k < tf->NumTextures(); ++k)
				{
					CShader *shader = tf->Texture(k);
					if (hash.find(shader->DisplayName()) != hash.end())
					{
						delete shader;
						continue;
					}
					m_shaders.AddItem(shader);
					hash[shader->DisplayName()] = shader;
				}

				++i;
			}
			else
			{
				m_tfl.erase(m_tfl.begin()+i);
			}
		}
		m_shadersLoaded = true;
	}
	return &m_shaders;
}

CTextureFactory *CQuakeGame::CreatePakFile()
{
	return new CQuakePak();
}

void CQuakeGame::RunMapCompile(const char *mappath, QuakeToolsList &tools, CTreadDoc *doc, bool runGame)
{
	CQuakeCompiler *qc = CreateCompiler(mappath, tools, doc, runGame);
	if (qc) { qc->Run(); }
}

CQuakeCompiler *CQuakeGame::CreateCompiler(const char *filename, QuakeToolsList &tools, CTreadDoc *doc, bool runMap)
{
	return new CQuakeCompiler(filename, tools, doc, runMap);
}

void CQuakeGame::CompileMap(CTreadDoc *doc, bool runGame)
{
	QuakeToolsList tools;
	CQuakeUserData *ud = static_cast<CQuakeUserData*>(doc->UserData());
	CQuakeGame *game = static_cast<CQuakeGame*>(doc->GamePlugin());
	game->GetToolsList(tools);
	// pull over any pertinent information from the user data in the document.
	for (int i = 0; i < tools.size(); ++i)
	{
		tools[i].enabled = ud->tools[i].enabled;
		tools[i].val = ud->tools[i].val;
		tools[i].runCmdLine = ud->tools[i].runCmdLine;
		tools[i].CleanValues();
	}
	{
		CQuakeBuildOptionsDialog dlg(tools);
		if (dlg.DoModal() != IDOK) return;
		ud->tools = tools; // copy any changed settings.
	}

	char mappath[1024];
	if (!doc->ExportMapForCompile(mappath, 1024)) return;
	{
		if (LeakTraceExists(doc))
		{
			char leakfile[1024];
			leakfile[0] = 0;
			GetLeakFileName(doc, leakfile, 1024);
			DeleteFileA(leakfile);
		}
	}
	RunMapCompile(mappath, tools, doc, runGame);
}

void CQuakeGame::GetLeakFileName(CTreadDoc *doc, char *buff, int buffSize)
{
}

bool CQuakeGame::LeakTraceExists(CTreadDoc *doc)
{
	return false;
}

bool CQuakeGame::LoadLeakTrace(CTreadDoc *doc)
{
	CFile f;
	char* data;
	int len;

	vec3 *leakpts = 0;
	int   numpts = 0;

	char filename[1024];
	GetLeakFileName(doc, filename, 1024);

	if( f.Open( filename, CFile::modeRead|CFile::typeBinary ) == false )
		return false;

	len = f.GetLength();
	if( len < 1 )
		return false;

	data = new char[len];

	f.Read( data, len );
	f.Close();

	C_Tokenizer sc;
	CString t;

	if( !sc.InitParsing( data, len ) )
	{
		delete[] data; 
		return false;
	}

	delete[] data; 

	for(;;)
	{
		if( !sc.GetToken(t) || !sc.GetToken(t) || !sc.GetToken(t) )
			break;

		numpts++;
	}

	if( numpts < 2 )
	{
		numpts = 0;
		return false;
	}

	sc.RestartParsing();
	leakpts = new vec3[numpts];

	int i;
	for(i = 0; i < numpts; i++)
	{
		sc.GetFloat( &leakpts[i].x );
		sc.GetFloat( &leakpts[i].y );
		sc.GetFloat( &leakpts[i].z );
	}

	sc.FreeScript();

	doc->SetLeakTrace(leakpts, numpts);
	delete[] leakpts;
	return true;
}

CLinkedList<CObjProp> *CQuakeGame::GetWorldspawnProps(CTreadDoc *doc)
{
	CQuakeUserData *ud = reinterpret_cast<CQuakeUserData*>(doc->UserData());
	if (ud)
	{
		return ud->GetWorldspawnProps();
	}
	return 0;
}

CMapObject *CQuakeGame::ObjectForClass(int classbits, int subclassbits)
{
	switch (classbits)
	{
	case MAPOBJ_CLASS_BRUSH: return new CQBrush(BrushType());
	}
	return 0;
}

CUndoRedoAction *CQuakeGame::UndoRedoActionForClass(int classbits)
{
	switch (classbits)
	{
	case UNDOREDO_CLASS_HOLLOW: return new CUndoRedoHollow();
	case UNDOREDO_CLASS_CARVE: return new CUndoRedoCarve();
	}
	return 0;
}

CObjectCreator *CQuakeGame::ObjectCreator(int i)
{
	switch (i)
	{
	case 0:
		return new CMakeBrushCube();
	case 1:
		return new CMakeBrushCylinder();
	case 2:
		return new CMakeBrushCone();
	}

	return 0;
}

void CQuakeGame::RunGame(const char *mapFile)
{
	char buff[1024];
	CString wrkDir = m_gameDir;
	CString cmdLine = ADD_QUOTES(m_exePath) + CString(" ") + m_exeCmdLine;
	if (mapFile)
	{
		Sys_GetFilename(mapFile, buff, 1024);
		StrSetFileExtension(buff, "");
		cmdLine += CString(" +map ") + buff;
	}
	RunProcess(0, cmdLine, wrkDir);
}

void CQuakeGame::UpdateVertexMode(CTreadDoc *doc)
{
	CVertDrag3D_Manipulator* m;
	float size = 64.0f;
	unsigned hlcolor = 0xFF4DE6F2;

	CQuakeUserData *ud = static_cast<CQuakeUserData*>(doc->UserData());
	
	doc->m_manips.push_back(m = new CVertDrag3D_Manipulator());
	m->color = 0xFF0000FF;
	m->hlcolor = hlcolor;
	m->size = size;
	m->vec = sysAxisX;
	m->x_in = &ud->vertmanpos;
	m->y_in = &ud->vertmanpos;
	m->z_in = &ud->vertmanpos;
	m->string = "vert drag x";
	doc->AddManipulatorToMap( m );

	doc->m_manips.push_back(m = new CVertDrag3D_Manipulator());
	m->color = 0xFF00FF00;
	m->hlcolor = hlcolor;
	m->size = size;
	m->vec = sysAxisY;
	m->x_in = &ud->vertmanpos;
	m->y_in = &ud->vertmanpos;
	m->z_in = &ud->vertmanpos;
	m->string = "vert drag y";
	doc->AddManipulatorToMap( m );

	doc->m_manips.push_back(m = new CVertDrag3D_Manipulator());
	m->color = 0xFFFF0000;
	m->hlcolor = hlcolor;
	m->size = size;
	m->vec = sysAxisZ;
	m->x_in = &ud->vertmanpos;
	m->y_in = &ud->vertmanpos;
	m->z_in = &ud->vertmanpos;
	m->string = "vert drag z";
	doc->AddManipulatorToMap( m );
}

void CQuakeGame::UpdateSelectionBounds(CTreadDoc *doc)
{
	CQuakeUserData *ud = static_cast<CQuakeUserData*>(doc->UserData());
	CManipulator* pM;

	float num = 0;
	ud->vertmanpos = vec3::zero;

	for( pM = doc->GetSelectedManipulatorList()->ResetPos(); pM; pM = doc->GetSelectedManipulatorList()->GetNextItem() )
	{
		CQBrushVertex_Manipulator* bm = dynamic_cast<CQBrushVertex_Manipulator*>(pM);
		if( bm )
		{
			ud->vertmanpos += *(bm->pos);
			num += 1.0f;
		}
	}

	if( num > 0.0f )
		ud->vertmanpos /= num;
}

void CQuakeGame::EnterVertexMode(CTreadDoc *doc, bool enter)
{
	if (enter)
	{
		doc->GetSelectedObjectList()->WalkList( CQBrush::EnterBrushVertexMode, doc );
	}
	else
	{
		doc->GetSelectedObjectList()->WalkList( CQBrush::ExitBrushVertexMode, doc );
	}
}

void CQuakeGame::EnterFaceMode(CTreadDoc *doc, bool enter)
{
	if (enter)
	{
		doc->GetSelectedObjectList()->WalkList( CQBrush::EnterBrushFaceMode );
	}
	else
	{
		doc->GetSelectedObjectList()->WalkList( CQBrush::ExitBrushFaceMode );
	}
}

void CQuakeGame::ProcessVertexBoxSelection(CTreadDoc *doc, int num, CPickObject **list, bool select)
{
	for(int i = 0; i < num; i++)
	{
		CQBrushVertex_Manipulator* m = dynamic_cast<CQBrushVertex_Manipulator*>(list[i]);
		if( m )
		{
			if( select )
				doc->AddManipulatorToSelection( m );
			else
				doc->AddManipulatorToMap( m );
		}
	}
	doc->UpdateSelectionInterface();
}

CQuakeUserData *CQuakeGame::CreateUserData(CTreadDoc *doc)
{
	return new CQuakeUserData(doc, this);
}

void CQuakeGame::BindUserData(CTreadDoc *doc)
{
	CQuakeUserData *data = CreateUserData(doc);
	doc->SetUserData(data);
	data->InitWorldspawn(doc->GameDef()->Worldspawn());
}

void CQuakeGame::ReleaseUserData(CTreadDoc *doc)
{
	delete static_cast<CQuakeUserData*>(doc->UserData());
	doc->SetUserData(0);
}

bool CQuakeGame::ReadMapHeader(CFile *file, CTreadDoc *doc, int version)
{
	CQuakeUserData *ud = static_cast<CQuakeUserData*>(doc->UserData());
	if (ud)
	{
		if (!ud->ReadFromFile(file, doc, version)) return false;
	}
	return true;
}

void CQuakeGame::WriteMapHeader(CFile *file, CTreadDoc *doc, int version)
{
	CQuakeUserData *ud = static_cast<CQuakeUserData*>(doc->UserData());
	if (ud)
	{
		ud->WriteToFile(file, doc, version);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CQuakeUserData
///////////////////////////////////////////////////////////////////////////////

CQuakeUserData::CQuakeUserData(CTreadDoc *doc, CQuakeGame *game) : 
m_BrushMenuCreated(false)
{
	game->GetToolsList(tools);
}

CQuakeUserData::~CQuakeUserData()
{
}

void CQuakeUserData::WriteToFile(CFile *file, CTreadDoc *doc, int version)
{
	MAP_WriteInt(file, (int)tools.size());
	for (int i = 0; i < tools.size(); ++i)
	{
		CQuakeTool &t = tools[i];
		MAP_WriteString(file, t.name);
		MAP_WriteString(file, t.runCmdLine);
		MAP_WriteInt(file, t.val);
		MAP_WriteInt(file, t.enabled ? 1 : 0);
	}
	MAP_WriteInt(file, m_worldspawn.GetCount());
	for (CObjProp *prop = m_worldspawn.ResetPos(); prop; prop = m_worldspawn.GetNextItem())
	{
		MAP_WriteString(file, prop->GetName());
		MAP_WriteString(file, prop->GetString());
	}
}

bool CQuakeUserData::ReadFromFile(CFile *file, CTreadDoc *doc, int version)
{
	CQuakeTool temp;
	int num = MAP_ReadInt(file);
	for (int i = 0; i < num; ++i)
	{
		CQuakeTool *dst = 0;
		CString name = MAP_ReadString(file);
		dst = &temp;
		for (int k = 0; k < tools.size(); ++k)
		{
			if (tools[i].name == name)
			{
				dst = &tools[i];
				break;
			}
		}
		dst->runCmdLine = MAP_ReadString(file);
		dst->val = MAP_ReadInt(file);
		dst->enabled = (MAP_ReadInt(file) ? true : false);
	}
	if (PLUGIN_FILE_VERSION(version) >= QUAKE_FILE_VERSION_2)
	{
		num = MAP_ReadInt(file);
		for (int i = 0; i < num; ++i)
		{
			CString s = MAP_ReadString(file);
			CObjProp *p = CObjProp::FindProp(&m_worldspawn, s);
			s = MAP_ReadString(file);
			if (p) { p->SetString(s); }
		}
	}
	return true;
}

void CQuakeUserData::InitWorldspawn(CEntDef *worldspawn)
{
	if (worldspawn)
	{
		m_worldspawn.Copy(*worldspawn->GetKeys());
	}
	else
	{
		m_worldspawn.DestroyList();
	}
}

CLinkedList<CObjProp> *CQuakeUserData::GetWorldspawnProps()
{
	if (m_worldspawn.IsEmpty()) { return 0; }
	return &m_worldspawn;
}

void CQuakeUserData::MakeBrushMenu(CTreadDoc *doc)
{
	if( m_BrushMenuCreated )
		return;

	CQuakeGame *game = static_cast<CQuakeGame*>(doc->GamePlugin());

	m_BrushMenu.AddMenuItem( BM_EDIT_FACES, "Edit Faces" );
	m_BrushMenu.AddMenuItem( BM_EDIT_VERTS, "Edit Verts" );
	m_BrushMenu.AddMenuItem( BM_LOCK_TEXTURE, "Lock Texture" );
	m_BrushMenu.AddMenuItem( BM_GRAB_TEXTURE, "Grab Texture" );
	m_BrushMenu.AddMenuItem( BM_APPLY_TEXTURE, "Apply Texture" );
	m_BrushMenu.AddMenuItem( BM_SELECT_ALL_WITH_TEXTURE, "Select All w/ Texture" );
	m_BrushMenu.AddMenuItem( BM_SNAP_TO_GRID, "Snap To Grid" );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( BM_CSG_HOLLOW, "Hollow..." );
	m_BrushMenu.AddMenuItem( BM_CSG_CARVE, "Carve" );
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