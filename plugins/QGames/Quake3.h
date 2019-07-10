///////////////////////////////////////////////////////////////////////////////
// Quake3.h
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

#if !defined(QUAKE3_H)
     #define QUAKE3_H

#include "Quake.h"
#include "Quake3Pak.h"
#include "Quake3Map.h"
#include <hash_set>

///////////////////////////////////////////////////////////////////////////////
// CQuake3Compiler
///////////////////////////////////////////////////////////////////////////////

class CQuake3Compiler : public CQuakeCompiler
{
public:

	CQuake3Compiler(const char *filename, QuakeToolsList &tools, CTreadDoc *doc, bool runMap);
	virtual ~CQuake3Compiler() {}

protected:

	virtual void ThreadProc();

private:

	bool m_myRunMap;
};

///////////////////////////////////////////////////////////////////////////////
// CQuake3Game
///////////////////////////////////////////////////////////////////////////////

class CQuake3Game : public CQuakeGame
{
public:

	CQuake3Game() : CQuakeGame(CQBrush::TYPE_Q3) {}
	
	virtual const char *Name();
	
	virtual void GetExportFile(const char *filename, char *buff, int buffSize);
	virtual CPluginFileExport *NativeMapExporter();
	virtual CPluginFileExport *FileExporter(int i);

	virtual const char *PakType(int i);
	virtual CTextureFactory *CreatePakFile();
	virtual CQuakeCompiler *CreateCompiler(const char *filename, QuakeToolsList &tools, CTreadDoc *doc, bool runMap);

	virtual CQuakeUserData *CreateUserData(CTreadDoc *doc);
	virtual void InitializeToolsList(QuakeToolsList &tools);
	virtual void GetLeakFileName(CTreadDoc *doc, char *buff, int buffSize);
	virtual bool LeakTraceExists(CTreadDoc *doc);
	virtual void ExportFile(CTreadDoc *doc, const char *dir, const char *file);

protected:

	typedef stdext::hash_set<std::string> StringHash;

	struct ShaderListExportParams
	{
		FILE *fp;
		const char *baseq3;
		StringHash shaders;
		StringHash files;
		CTreadDoc *doc;
	};

	virtual void ExportShaderList(CTreadDoc *doc);
	static int ExportBrushShaders(CMapObject *obj, void *p1, void *p2);

	virtual void RunMapCompile(const char *mappath, QuakeToolsList &tools, CTreadDoc *doc, bool runGame);
};

///////////////////////////////////////////////////////////////////////////////
// CQuake3UserData
///////////////////////////////////////////////////////////////////////////////

class CQuake3UserData : public CQuakeUserData
{
public:
	CQuake3UserData(CTreadDoc *doc, CQuake3Game *game) : CQuakeUserData(doc, game) {}
	void MakeBrushMenu(CTreadDoc *doc);
};

#endif