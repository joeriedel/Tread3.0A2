///////////////////////////////////////////////////////////////////////////////
// QuakeMap.cpp
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
#include "QuakeMap.h"
#include "Quake.h"
#include <vector>

static bool ParseEntity( C_Tokenizer* script, CTreadDoc* pDoc );
static CQBrush* ParseBrush( C_Tokenizer* script, CTreadDoc* pDoc, bool* was_halflife );

///////////////////////////////////////////////////////////////////////////////
// CQuakeMap
///////////////////////////////////////////////////////////////////////////////

int CQuakeMap::WriteEntities( CMapObject* obj, void* p, void* p2 )
{
	CQuakeMap* qm = (CQuakeMap*)p;
	CTreadDoc* doc = (CTreadDoc*)p2;
	
	CEntity *e = dynamic_cast<CEntity*>(obj);

	if(e)
	{
		qm->WriteEntity(*qm->m_file, e, doc);
		Sys_StepStatusBar();
	}

	return 0;
}

int CQuakeMap::WriteNonOwnedBrushes( CMapObject* obj, void* p, void* p2 )
{
	CQuakeMap* qm = (CQuakeMap*)p;
	CTreadDoc* doc = (CTreadDoc*)p2;

	CQBrush* b = dynamic_cast<CQBrush*>(obj);
	if( b )
	{
		if( b->GetOwnerUID() == -1 )
		{
			qm->WriteBrush(*qm->m_file, b, doc);
			Sys_StepStatusBar();
		}
	}
	
	return 0;
}

void CQuakeMap::WriteBrush(std::fstream &fs, CQBrush *brush, CTreadDoc *doc)
{
	fs << "{\n";
	
	//
	// write the faces.
	//
	int i, k;
	CQBrush::BrushFace_t* f;
	CQBrush::BrushVert_t* v;
	for(i = 0; i < brush->m_numfaces; i++)
	{
		f = &brush->m_faces[i];
		if (f->num_verts < 3) break;
		
		for( k = 0; k < 3; k++ )
		{
			v = &f->verts[k];

			fs << "( " << (int)(floor(brush->m_xyz[v->xyz].x - 0.5f) + 1.0f) << " " 
			           << (int)(floor(brush->m_xyz[v->xyz].y - 0.5f) + 1.0f) << " " 
					   << (int)(floor(brush->m_xyz[v->xyz].z - 0.5f) + 1.0f) << " ) ";
		}

		CString shader = f->s_shader;
		shader.MakeUpper();

		fs << shader;
		fs << " " << (int)f->shift[0] << " " << (int)f->shift[1] << " " << (int)f->rot << " " 
			<< f->scale[0] << " " << f->scale[1] << "\n";
	}

	fs << "}\n";
}

CQBrush *CQuakeMap::ParseBrush(C_Tokenizer &script, CTreadDoc *doc)
{
	int i, num;
	vec3 pts[3];
	CString t;
	std::vector<CQBrush::PlaneInfo> planes;
	std::vector<CString> shaders;
	std::vector<vec2> shifts;
	std::vector<float> rots;
	std::vector<vec2> scales;

	num = 0;
	for(;;)
	{
		if( !script.GetToken( t ) )
			return 0;

		if( t == "}" ) // end brush
			break;

		script.UngetToken();

		for(i = 0; i < 3; i++)
		{
			script.Skip();
			script.GetFloat(&pts[i].x);
			script.GetFloat(&pts[i].y);
			script.GetFloat(&pts[i].z);
			script.Skip();
		}

		script.GetToken(t);
		t.MakeLower();
		CQBrush::PlaneInfo p;
		p.s_shader = t;
		p.p_shader = doc->ShaderForName(t);
		script.GetFloat(&p.shift[0]);
		script.GetFloat(&p.shift[1]);
		script.GetFloat(&p.rot);
		script.GetFloat(&p.scale[0]);
		script.GetFloat(&p.scale[1]);
		p.plane = plane3(pts[2], pts[0], pts[1]);
		planes.push_back(p);
	}

	if(planes.size() < 4) return 0;

	CQBrush* brush = CQBrush::BrushFromPlanes(
		static_cast<CQuakeGame*>(doc->GamePlugin())->BrushType(), 
		&planes[0], 
		(int)planes.size()
	);

	if( !brush ) return 0;

	brush->TryWeldVerts( doc, PROGRAM_WELD_DIST, false );
	brush->MakeRenderMesh();
	brush->MakePlanes();
	brush->TextureBrush(0);
	brush->UpdateRenderMeshTexture();
	brush->GenTXSpaceVecs();
	brush->BuildInfo();

	Sys_StepStatusBar();
	return brush;
}

void CQuakeMap::WriteEntity(std::fstream &file, CEntity *entity, CTreadDoc *doc)
{
	CObjProp* p;

	file << "{\n\"classname\" \"" << entity->GetClassname() << "\"\n";
	
	for( p = entity->GetPropList(doc)->ResetPos(); p; p = entity->GetPropList(doc)->GetNextItem() )
	{
		if( p->GetString() != 0 &&
			strlen( p->GetString() ) > 0 )
		{	
			CString s = p->GetString();
			s.Replace( '"', '\'' );

			file << "\"" << p->GetName() << "\" \"" << s << "\"\n";
		}
	}

	//
	// write any owned brushes.
	//
	int numobjs;
	CMapObject* obj;
	int *objs = entity->GetOwnedObjectUIDs(&numobjs);

	if (numobjs == 0)
	{
		vec3 pos = entity->GetObjectWorldPos();
		file << "\"origin\" \"" << pos.x << " " << pos.y << " " << pos.z << "\"\n";
	}
	else
	{
		for(int i = 0; i < numobjs; ++i )
		{
			obj = doc->ObjectForUID( objs[i] );
			if( obj )
			{
				CQBrush *b = dynamic_cast<CQBrush*>(obj);
				if (b) { WriteBrush(file, b, doc); }
				Sys_StepStatusBar();
			}
		}
	}

	file << "}\n";
}

bool CQuakeMap::ParseEntity(C_Tokenizer &script, CTreadDoc *doc)
{
	CLinkedList<CEntKey> key_list;
	CString t, n;
	CQBrush* b;
	CLinkedList<CMapObject> brush_list;

	for( ;; )
	{
		if( !script.GetToken( t ) )
			return false;

		if( t == "}" )
			break;

		if( t == "{" )
		{
			b = ParseBrush( script, doc );

			if( !b )
				continue;

			brush_list.AddItem( b );
	
			continue;
		}

		if( !script.GetToken( n ) )
			return false;

		CEntKey* key = new CEntKey();

		key->SetName(t);
		key->SetDisplayName(t);
		key->SetKeyValue(n);
		key_list.AddItem(key);
	}

	//
	// generate brush data.
	//
	doc->ClearSelection();

	for( b = (CQBrush*)brush_list.ResetPos(); b; b = (CQBrush*)brush_list.GetNextItem() )
	{
		doc->AssignUID( b );
		b->SetName( doc->MakeUniqueObjectName( b ) );
	}

	CEntKey* classkey = CEntity::FindKey( &key_list, "classname" );
	if( classkey )
	{
		//
		// don't do worldspawn...
		//
		if(!stricmp( classkey->StringForKey(), "worldspawn" ))
		{
			CLinkedList<CObjProp> *props = doc->GamePlugin()->GetWorldspawnProps(doc);
			if (props)
			{
				for (CObjProp *p = props->ResetPos(); p; p = props->GetNextItem())
				{
					CEntKey *key = CEntity::FindKey(&key_list, p->GetName());
					if (key)
					{
						p->SetString(key->StringForKey());
					}
				}
			}
			goto noentity;
		}

		if (!stricmp(classkey->StringForKey(), "func_group"))
		{
			CObjectGroup *gr = new CObjectGroup();
			doc->AssignUID(gr);
			gr->SetName(doc->MakeUniqueGroupName(gr));
			gr->MakeList(&brush_list);
			doc->GetObjectGroupList()->AddItem(gr);

			for( b = (CQBrush*)brush_list.ResetPos(); b; b = (CQBrush*)brush_list.GetNextItem() )
			{
				b->SetGroupUID(gr->GetUID());
			}

			goto noentity;
		}

		CMapObject* obj = CEntity::MakeFromKeyList( doc, &key_list );
		if( obj )
		{
			doc->AssignUID( obj );
			obj->SetName( doc->MakeUniqueObjectName( obj ) );
			doc->AddObjectToMap( obj );

			CEntity* ent = (CEntity*)obj;
			ent->OwnObjectList( doc, &brush_list );
		}
	}

noentity:

	//
	// transfer to document.
	//
	for( ;; )
	{
		b = (CQBrush*)brush_list.RemoveItem( LL_HEAD );
		if( !b )
			break;

		doc->AddObjectToMap( b );
		b->UpdateOwnerContents( doc );
	}

	return true;
}

int CQuakeMap::CountEntsInFile(C_Tokenizer &script)
{
	int num = 0;
	int indent;
	CString t;

	indent = 0;
	//
	// count num ents.
	//
	for( ;; )
	{
		if( !script.GetToken(t) ) break;

		if( t == "}" ) { num++; }
		//if( t == "{" ) { indent++; }

		//if( indent == 0 ) num++;
	}

	if( indent != 0 )
	{
		OS_OkAlertMessage("Error Importing", "Error importing map!");
		return -1;
	}

	return num;
}

bool CQuakeMap::Import(const char *filename, CTreadDoc *doc)
{
	CFile file;
	if (!file.Open(filename, CFile::modeRead | CFile::shareDenyWrite))
	{
		OS_OkAlertMessage("Error Importing", "Error importing map!");
		return false;
	}

	DWORD size = file.GetLength();
	if( size < 1 )
		return false;

	char* data = new char[size];
	file.Read( data, size );

	C_Tokenizer script;

	script.InitParsing( data, (int)size );
	delete[] data;

	int num = CountEntsInFile(script);
	if (num < 1) return false;

	Sys_SetStatusBarRange( 0, num );
	Sys_SetStatusBarPos( 0 );

	CString t;
	num = 0;
	script.RestartParsing();
	for( ;; )
	{
		if( !script.GetToken( t ) )
			break;

		if( !ParseEntity(script, doc) )
		{
			OS_OkAlertMessage( "Error Importing", "Error parsing entity!" );
			return false;
		}

		Sys_StepStatusBar();
	}

	return true;

}

void CQuakeMap::WriteWorldspawn(const char *wad, std::fstream &fs, CLinkedList<CMapObject> *objects, CLinkedList<CMapObject> *selected, CTreadDoc *doc)
{
	fs << "{\n\"classname\" \"worldspawn\"\n";

	if (static_cast<CQuakeGame*>(doc->GamePlugin())->BrushType() == CQBrush::TYPE_Q1)
	{
		fs << "\"wad\" \"" << wad << "\"\n";
	}

	CLinkedList<CObjProp> *worldspawn = doc->GamePlugin()->GetWorldspawnProps(doc);
	if (worldspawn)
	{
		for (CObjProp *prop = worldspawn->ResetPos(); prop; prop = worldspawn->GetNextItem())
		{
			const char *value = prop->GetString();
			if (value && value[0])
			{
				fs << "\"" << prop->GetName() << "\" \"" << value << "\"\n";
			}
		}
	}

	//
	// write all brushes that are not owned.
	//
	objects->WalkList( WriteNonOwnedBrushes, this, doc );
	selected->WalkList( WriteNonOwnedBrushes, this, doc );

	fs << "}\n";
}

bool CQuakeMap::ParseWorldspawn(C_Tokenizer &script, CTreadDoc *doc)
{
	return ParseEntity(script, doc);
}

bool CQuakeMap::WriteTextureWad(CMapObject *object, CTreadDoc *doc, void *parm)
{
	CQuakeMap *qmap = (CQuakeMap*)parm;
	CQBrush *brush = dynamic_cast<CQBrush*>(object);
	if (brush)
	{
		CFile *file = qmap->m_wadBuilder.File();

		for (int i = 0; i < brush->m_numfaces; ++i)
		{
			CQBrush::BrushFace_t *face = &brush->m_faces[i];
			if (qmap->m_wadTexs.find((const char*)face->s_shader) == qmap->m_wadTexs.end())
			{
				qmap->m_wadTexs[(const char*)face->s_shader] = true;
				CQuakeTex *qtex = dynamic_cast<CQuakeTex*>(face->p_shader);
				if (qtex)
				{
					int pos = (int)file->GetPosition();
					if (qtex->WriteNativeToFile(file))
					{
						int size = (int)file->GetPosition() - pos;
						qmap->m_wadBuilder.AddEntryInfo(pos, size, face->s_shader);
					}
				}
			}
		}
	}

	return false;
}

bool CQuakeMap::ExportTextures(const char *filename, CTreadDoc *doc)
{
	char buff[1024];
	strcpy(buff, filename);
	StrSetFileExtension(buff, "wad");
	{
		CString s;
		s.Format("exporting '%s'...\n", buff);
		doc->WriteToCompileWindow(s);
	}
	if (!m_wadBuilder.Initialize(buff)) 
	{
		doc->WriteToCompileWindow("error opening wad file!");
		return false;
	}
	doc->WalkObjects(WriteTextureWad, this, true, true);
	m_wadBuilder.Finalize();
	m_wadTexs.clear();
	return true;
}

bool CQuakeMap::Export(const char *filename, bool forCompile, CTreadDoc *doc)
{
	{
		char buff[1024];
		Sys_GetDirectory(filename, buff, 1024);
		CreateDirectoryPathNative(buff);

		if (forCompile)
		{
			if (!ExportTextures(filename, doc)) return false;
		}
	}

	std::fstream fs;
	fs.open( filename, std::ios::out );

	{
		CString s;
		s.Format("exporting '%s'...\n", filename);
		doc->WriteToCompileWindow( s );
	}

	CLinkedList<CMapObject>* list[2];

	list[0] = doc->GetObjectList();
	list[1] = doc->GetSelectedObjectList();

	Sys_SetStatusBarRange( 0, list[0]->GetCount() + list[1]->GetCount() );
	Sys_SetStatusBarPos( 0 );

	m_file = &fs;
	{
		char wad[256], wad2[256];
		Sys_GetFilename(filename, wad, 256);
		StrSetFileExtension(wad, "wad");
		strcpy(wad2, "id1/maps/");
		strcat(wad2, wad);
		WriteWorldspawn(wad2, fs, list[0], list[1], doc);
	}

	//
	// write entities now.
	//
	list[0]->WalkList( WriteEntities, this, doc );
	list[1]->WalkList( WriteEntities, this, doc );

	fs.close();
	return true;
}