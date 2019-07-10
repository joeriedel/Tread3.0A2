///////////////////////////////////////////////////////////////////////////////
// QuakeMap.h
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

#if !defined(QUAKEMAP_H)
     #define QUAKEMAP_H

#include "Brush.h"
#include "QuakePak.h"
#include <hash_map>

///////////////////////////////////////////////////////////////////////////////
// CQuakeMap
///////////////////////////////////////////////////////////////////////////////

class CQuakeMap : public CPluginFileImport, public CPluginFileExport
{
public:

	virtual void Release() { delete this; }
	virtual bool Import(const char *filename, CTreadDoc *doc);
	virtual bool Export(const char *filename, bool forCompile, CTreadDoc *doc);
	virtual const char *Type() { return "Quake Map"; }
	virtual const char *Extension() { return "map"; }
	virtual const char *PluginGame() { return "Quake"; }

protected:

	virtual bool ExportTextures(const char *filename, CTreadDoc *doc);
	virtual void WriteWorldspawn(const char *wad, std::fstream &file, CLinkedList<CMapObject> *objects, CLinkedList<CMapObject> *selected, CTreadDoc *doc);
	virtual bool ParseWorldspawn(C_Tokenizer &script, CTreadDoc *doc);
	virtual void WriteEntity(std::fstream &file, CEntity *entity, CTreadDoc *doc);
	virtual void WriteBrush(std::fstream &file, CQBrush *brush, CTreadDoc *doc);
	virtual CQBrush *ParseBrush(C_Tokenizer &script, CTreadDoc *doc);
	virtual bool ParseEntity(C_Tokenizer &script, CTreadDoc *doc);
	virtual int  CountEntsInFile(C_Tokenizer &script);
	virtual CQBrush::Type BrushType() const { return CQBrush::TYPE_Q1; }

	static int WriteEntities( CMapObject* obj, void* p, void* p2 );
	static int WriteNonOwnedBrushes( CMapObject* obj, void* p, void* p2 );

	typedef stdext::hash_map<std::string, bool> StringToBoolMap;
	static bool WriteTextureWad(CMapObject *object, CTreadDoc *doc, void *parm);

	std::fstream *m_file;
	StringToBoolMap m_wadTexs;
	CQuakeWad2Builder m_wadBuilder;
};

#endif
