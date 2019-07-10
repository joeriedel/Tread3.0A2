///////////////////////////////////////////////////////////////////////////////
// Quake.h
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

#if !defined(QUAKE_H)
     #define QUAKE_H

#include "resource.h"
#include "Brush.h"
#include "VertDrag3D_Manipulator.h"
#include "TextureFactory.h"
#include "QuakePak.h"
#include "QuakeMap.h"
#include <map>
#include <vector>

#define QUAKE_FILE_VERSION QUAKE_FILE_VERSION_2
#define QUAKE_FILE_VERSION_2 2
#define QUAKE_FILE_VERSION_1 1

#define ADD_QUOTES(s) (CString("\"") + (s) + CString("\""))

///////////////////////////////////////////////////////////////////////////////
// BrushMenu
///////////////////////////////////////////////////////////////////////////////

enum BrushMenu
{
	BM_EDIT_FACES = 1,
	BM_EDIT_VERTS,
	BM_LOCK_TEXTURE,
	BM_GRAB_TEXTURE,
	BM_APPLY_TEXTURE,
	BM_SELECT_ALL_WITH_TEXTURE,
	BM_SNAP_TO_GRID,
	BM_SELECT_BRUSHES_WITH_SAME_CONTENTS,
	BM_SELECT_BRUSHES_WITH_SAME_SURFACE,
	BM_UNLINK_FROM_ENTITY,
	BM_CSG_HOLLOW,
	BM_CSG_CARVE,
		
	BM_FIRST_LINK_TO_ENTITY_MENU = 64
};

typedef std::pair<CString, CString> StringPair;
typedef std::vector<StringPair> StringPairList;
typedef std::vector<CString> StringList;

///////////////////////////////////////////////////////////////////////////////
// CQuakeToolCmdLineOption
///////////////////////////////////////////////////////////////////////////////

struct CQuakeToolCmdLineOption
{
	CQuakeToolCmdLineOption() : val(0), always(false) {}
	CQuakeToolCmdLineOption(const char *str) : name(str), val(0), always(false) {}

	CString name;
	CString cmdline;
	int val;
	bool always;
};

typedef std::vector<CQuakeToolCmdLineOption> QuakeToolCmdLineOptionList;

///////////////////////////////////////////////////////////////////////////////
// CQuakeTool
///////////////////////////////////////////////////////////////////////////////

struct CQuakeTool
{
	CQuakeTool();
	CQuakeTool(const char *str);
	CString name;
	CString path;
	CString cmdLine;
	CString runCmdLine;
	QuakeToolCmdLineOptionList opts;
	int val;
	bool enabled;
	CString MakeCommandLine() const;
	void CleanValues();
};

typedef std::vector<CQuakeTool> QuakeToolsList;
typedef std::vector<CString> QuakePakList;

///////////////////////////////////////////////////////////////////////////////
// CQuakeUserData
///////////////////////////////////////////////////////////////////////////////

class CQuakeGame;
class CQuakeUserData
{
public:
	CQuakeUserData(CTreadDoc *doc, CQuakeGame *game);
	virtual ~CQuakeUserData();

	CQBrush::CQBrushParmsMenu m_BrushMenu;
	virtual void MakeBrushMenu(CTreadDoc *doc);
	virtual void InitWorldspawn(CEntDef *worldspawn);
	virtual void WriteToFile(CFile *file, CTreadDoc *doc, int version);
	virtual bool ReadFromFile(CFile *file, CTreadDoc *doc, int version);
	virtual CLinkedList<CObjProp> *GetWorldspawnProps();

	vec3 vertmanpos;
	// only a subset of the tool fields are valid in this copy of the
	// tools info.
	QuakeToolsList tools;

protected:

	bool m_BrushMenuCreated;

private:

	CLinkedList<CObjProp> m_worldspawn;
};

///////////////////////////////////////////////////////////////////////////////
// CQuakeCompiler
///////////////////////////////////////////////////////////////////////////////

class CQuakeCompiler : public CPluginMapCompiler
{
public:

	CQuakeCompiler(const char *filename, QuakeToolsList &tools, CTreadDoc *doc, bool runMap);
	virtual ~CQuakeCompiler();

	virtual void Run();

	virtual void Abort();

protected:

	void StartThread();

	virtual void ThreadProc();

	CTreadDoc *m_doc;
	CString m_filename;
	CString m_realmappath;
	bool m_runMap;
	volatile bool m_abort;

	CQuakeTool *ToolForName(const char *name);

private:

	static DWORD WINAPI StaticThreadProc(void *param);
	HANDLE m_thread;
	QuakeToolsList m_tools;
};

///////////////////////////////////////////////////////////////////////////////
// CQuakeGame
///////////////////////////////////////////////////////////////////////////////

class CQuakeGame : public CPluginGame
{
public:

	CQuakeGame();
	CQuakeGame(CQBrush::Type brushType);

	virtual void Initialize(PrefFileSection_t *prefs);
	virtual void SaveSettings(PrefFileSection_t *prefs);
	virtual void Release();
	
	virtual int FileVersion() { return QUAKE_FILE_VERSION; }

	virtual const char *Name();

	virtual bool AllowBrushes();
	
	virtual bool WantsConfiguration();

	virtual void Configure();
	
	virtual void GetExportFile(const char *filename, char *buff, int buffSize);
	virtual CPluginFileExport *NativeMapExporter();
	virtual CPluginFileExport *FileExporter(int i);
	
	virtual CLinkedList<CShader> *ShaderList();
	
	virtual void CompileMap(CTreadDoc *doc, bool run);
	virtual void GetLeakFileName(CTreadDoc *doc, char *buff, int buffSize);
	virtual bool LeakTraceExists(CTreadDoc *doc);
	virtual bool LoadLeakTrace(CTreadDoc *doc);
	virtual CLinkedList<CObjProp> *GetWorldspawnProps(CTreadDoc *doc);
	
	virtual CMapObject *ObjectForClass(int classbits, int subclassbits);
	virtual CUndoRedoAction *UndoRedoActionForClass(int classbits);

	virtual CObjectCreator *ObjectCreator(int i);

	virtual void RunGame(const char *mapFile);

	virtual void UpdateVertexMode(CTreadDoc *doc);
	virtual void UpdateSelectionBounds(CTreadDoc *doc);
	virtual void EnterVertexMode(CTreadDoc *doc, bool enter);
	virtual void EnterFaceMode(CTreadDoc *doc, bool enter);
	virtual void ProcessVertexBoxSelection(CTreadDoc *doc, int count, CPickObject **list, bool select);
	virtual void BindUserData(CTreadDoc *doc);
	virtual void ReleaseUserData(CTreadDoc *doc);
	virtual CQuakeUserData *CreateUserData(CTreadDoc *doc);

	virtual bool ReadMapHeader(CFile *file, CTreadDoc *doc, int version);
	virtual void WriteMapHeader(CFile *file, CTreadDoc *doc, int version);

	virtual const char *PakType(int i);

	virtual void InitializeToolsList(QuakeToolsList &tools);
	virtual CTextureFactory *CreatePakFile();
	virtual CQuakeCompiler *CreateCompiler(const char *filename, QuakeToolsList &tools, CTreadDoc *doc, bool runMap);

	CQBrush::Type BrushType() const { return m_type; }
	CQuakeTool ToolForName(const char *name) const;
	void GetToolsList(QuakeToolsList &out) const;
	const QuakePakList &PakList() const;
	const char *EXEPath() const;
	const char *EXECmdLine() const;
	const char *GameDir() const;
	const TextureFactoryList &GetTextureFactoryList() const;

protected:

	CQuakeTool &LoadToolOpts(CQuakeTool &tool, const char *filename);
	virtual void RunMapCompile(const char *mappath, QuakeToolsList &tools, CTreadDoc *doc, bool runGame);

private:

	typedef stdext::hash_map<std::string, bool> StringToBoolMap;
	static void WriteTextureWad(CMapObject *object, CTreadDoc *doc, void *parm);

	bool m_shadersLoaded;
	CLinkedList<CShader> m_shaders;
	TextureFactoryList m_tfl;
	CString m_exePath;
	CString m_exeCmdLine;
	CString m_gameDir;
	QuakeToolsList m_tools;
	QuakePakList  m_paks;
	CQBrush::Type m_type;
};

#endif