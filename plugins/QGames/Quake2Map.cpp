///////////////////////////////////////////////////////////////////////////////
// Quake2Map.cpp
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
#include "Quake2Map.h"

///////////////////////////////////////////////////////////////////////////////
// CQuake2Map
///////////////////////////////////////////////////////////////////////////////

bool CQuake2Map::ExportTextures(const char *filename, CTreadDoc *doc)
{
	return true;
}

void CQuake2Map::WriteBrush(std::fstream &fs, CQBrush *brush, CTreadDoc *doc)
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
		shader.MakeLower();

		int contents = f->contents; // do it the way id tools do...
		if (contents == CONTENTS_SOLID)
		{
			contents = 0;
		}

		fs << shader;
		fs << " " << (int)f->shift[0] << " " << (int)f->shift[1] << " " << (int)f->rot << " " 
			<< f->scale[0] << " " << f->scale[1] << " " 
			<< contents << " " << f->surface << " " << f->value << "\n";
	}

	fs << "}\n";
}

CQBrush *CQuake2Map::ParseBrush(C_Tokenizer &script, CTreadDoc *doc)
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
		p.p_shader = doc->ShaderForDisplayName(CQuake2Game::TrimTextureName(t));
		script.GetFloat(&p.shift[0]);
		script.GetFloat(&p.shift[1]);
		script.GetFloat(&p.rot);
		script.GetFloat(&p.scale[0]);
		script.GetFloat(&p.scale[1]);

		script.GetToken(t);
		if (t != "}" && t != "(")
		{
			p.contents = atoi(t);
			script.GetToken(t);
			p.surface = atoi(t);
			script.GetToken(t);
			p.value = atoi(t);

			script.GetToken(t);
			if (t != "}" && t != "(")
			{
				script.Skip(2);
			}
			else
			{
				script.UngetToken();
			}
		}
		else
		{
			CQuake2Tex *tex = dynamic_cast<CQuake2Tex*>(p.p_shader);
			if (tex)
			{
				p.contents = tex->Contents();
				p.surface = tex->Surface();
				p.value = tex->Value();
			}
			script.UngetToken();
		}

		if (p.contents == 0)
		{
			p.contents = CONTENTS_SOLID;
		}

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
