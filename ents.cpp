///////////////////////////////////////////////////////////////////////////////
// Ents.cpp
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
#include "ents.h"
#include "c_tokenizer.h"
#include "files.h"
#include "system.h"
#include "texcache.h"
#include "r_gl.h"
#include "MapView.h"
#include "TreadDoc.h"
#include "mapfile.h"
#include "resource.h"
#include "MainFrm.h"

#define PROJECT_VERTEX( v, l, d, pv ) ((pv) = (v) + normalized( (v) - (l) ) * (d))
#define MAX_SKEL_MATERIALS	32

static bool IconTexUpload( RTex_t* tex )
{
	int handle = (int)tex->user_data[0];

	TC_TexUpload( handle, _upf_wrap|_upf_mipmap|_upf_compress|_upf_filter, tex, FALSE );

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CEntKey															//
//////////////////////////////////////////////////////////////////////

IMPLEMENT_TREAD_NEW(CEntKey);

CEntKey::CEntKey()
{
}

CEntKey::~CEntKey()
{
}

const char* CEntKey::GetName()
{
	return m_sName;
}

void CEntKey::SetName( const char* name )
{
	if( !name )
		return;

	m_sName = name;
}

const char* CEntKey::GetDisplayName()
{
	return m_sDisplayName;
}

void CEntKey::SetDisplayName( const char* name )
{
	if( !name )
		return;

	m_sDisplayName = name;
}

const char* CEntKey::StringForKey()
{
	return m_sValue;
}

void CEntKey::SetKeyValue( const char* value )
{
	if( !value || !value[0] )
	{
		m_sValue = "";
	}
	else
	{
		m_sValue = value;
	}
}

int CEntKey::IntForKey()
{
	int i;

	sscanf( m_sValue, "%d", &i );
	return i;
}

float CEntKey::FloatForKey()
{
	float f;

	sscanf( m_sValue, "%f", &f );
	return f;
}

vec3 CEntKey::VecForKey()
{
	vec3 v;

	sscanf( m_sValue, "%f %f %f", &v.x, &v.y, &v.z );
	return v;
}

CEntKey *CEntKey::Clone()
{
	CEntKey *key = new CEntKey();
	key->m_sName = m_sName;
	key->m_sDisplayName = m_sName;
	key->m_sValue = m_sValue;
	return key;
}

//////////////////////////////////////////////////////////////////////
// CEntity															//
//////////////////////////////////////////////////////////////////////

IMPLEMENT_TREAD_NEW(CEntity);

bool CEntity::m_bMenuCreated = false;
CEntity::CEntityParmsMenu CEntity::m_EntityMenu;

void CEntity::MakeEntityMenu()
{
	if( m_bMenuCreated )
		return;

	m_EntityMenu.AddMenuItem( 1, "Grab Texture" );
	m_EntityMenu.AddMenuItem( 2, "Apply Texture" );
	m_EntityMenu.AddMenuItem( 3, "Select All w/ Texture" );
	//m_EntityMenu.AddMenuItem( 4, "Edit Shader" );
	m_EntityMenu.AddMenuItem( 0, "@SEP@" );
	m_EntityMenu.AddMenuItem( ID_TOOLS_CHECKSELECTEDOBJECTS, "Check Selected Objects For Errors...", true );
	m_EntityMenu.AddMenuItem( 0, "@SEP@" );
	m_EntityMenu.AddMenuItem( ID_TOOLS_HIDE, "Hide Selected", true );
	m_EntityMenu.AddMenuItem( ID_TOOLS_SHOWALL, "Show All", true );
	m_EntityMenu.AddMenuItem( ID_TOOLS_HIDEALL, "Hide All", true );
	m_EntityMenu.AddMenuItem( 0, "@SEP@" );
	m_EntityMenu.AddMenuItem( ID_TOOLS_MAKEGROUP, "Make Group", true );
	m_EntityMenu.AddMenuItem( ID_TOOLS_MAKEGROUPANDHIDE, "Make Group and Hide", true );
	m_EntityMenu.AddMenuItem( ID_TOOLS_HIDEALLINGROUP, "Hide All In Group(s)", true );
	m_EntityMenu.AddMenuItem( ID_TOOLS_SHOWALLINGROUPS, "Show All In Group(s)", true );
	m_EntityMenu.AddMenuItem( ID_TOOLS_SELECTALLINGROUPS, "Select All In Group(s)", true );
	m_EntityMenu.AddMenuItem( ID_TOOLS_REMOVEFROMGROUPS, "Remove From Group(s)", true );

	m_bMenuCreated = true;
}

CEntity::CEntityParmsMenu::CEntityParmsMenu() : CObjectMenu()
{
	entity = 0;
}

CEntity::CEntityParmsMenu::~CEntityParmsMenu()
{
}

void CEntity::CEntityParmsMenu::OnMenuItem( int id )
{
	switch( id )
	{
	case 1: // grab.
		{
			CShader* s = view->GetDocument()->ShaderForName( entity->m_sShader );
			if( s )
			{
				view->GetDocument()->SetSelectedShader(s);
				Sys_AdjustToViewTexture( s );
				Sys_RedrawWindows( VIEW_TYPE_TEXTURE );
			}
		}
	break;
	case 2:
		view->GetDocument()->GenericUndoRedoFromSelection()->SetTitle( "Paint Selection" );
		view->GetDocument()->PaintSelection();
		Sys_RedrawWindows( VIEW_TYPE_3D );
	break;
	case 3:
		view->GetDocument()->SelectObjectsByShader( entity->m_sShader );
		view->GetDocument()->UpdateSelectionInterface();
		view->GetDocument()->Prop_UpdateSelection();
		Sys_RedrawWindows();
	break;
	/*case 4:
		Sys_EditShader( ShaderForName( entity->m_sShader ) );
	break;*/
	}
}

void CEntity::CEntityParmsMenu::OnUpdateCmdUI( int id, CCmdUI* pUI )
{
	switch( id )
	{
	case 1:
	case 2:
	case 3:
	case 4:

		if( entity->m_pDef && entity->m_pDef->GetDrawStyle() == EDDS_JMODEL )
		{
			pUI->Enable();

			CString shader, string;

			if( id == 1 )
				string = "Grab";
			else
			if( id == 2 )
				string = "Apply";
			else
			if( id == 3 )
				string = "Select Objects With";
			/*else
			if( id == 4 )
				string = "Edit Shader";*/

			if( id == 1 || id == 3 || id == 4 )
				shader = entity->m_sShader;
			else
				shader = view->GetDocument()->SelectedShaderName();

			{
				char buff[256];
				sprintf(buff, "%s \"%s\"", string, shader );
				pUI->SetText( buff );
			}
		}
		else
		{
			pUI->Enable( FALSE );
		}
	break;
	}
}

CEntity::CEntity() : CMapObject()
{
	m_pDef = 0;
	m_icon = 0;
	m_iconhandle = BAD_TC_HANDLE;
	//m_skm_meshes = 0;
	//m_skm_mesh_count = 0;
	m_pos = vec3::zero;
	m_bDrag = false;
	//m_skm = 0;
	//m_jmodel = 0;
	m_model_angles = vec3::zero;
	m_numobjs = 0;
	m_objs = 0;
	m_bLightBoxMesh = false;
	m_shadow_xyz = 0;
	m_shadow_planes = 0;
	m_plane_facings = 0;
	m_skm_tris = 0;

	MakeEntityMenu();
}

CEntity::CEntity( const CEntity& e ) : CMapObject( e )
{
	m_pDef = 0;
	m_icon = 0;
	//m_skm = 0;
	//m_skm_meshes = 0;
	//m_skm_mesh_count = 0;
	m_iconhandle = BAD_TC_HANDLE;
	m_bDrag = false;
	m_pos = e.m_pos;
	//m_jmodel = 0;
	m_model_angles = e.m_model_angles;
	m_sClassname = e.m_sClassname;
	m_sShader = e.m_sShader;
	m_numobjs = e.m_numobjs;
	m_objs = 0;
	m_bLightBoxMesh = false;
	m_shadow_xyz = 0;
	m_shadow_planes = 0;
	m_plane_facings = 0;
	m_skm_tris = 0;

	if( m_numobjs > 0 )
	{
		m_objs = new int[m_numobjs];
		memcpy( m_objs, e.m_objs, sizeof(int)*m_numobjs );
	}

	CEntity::CopyPropList( (CLinkedList<CObjProp>*)&e.m_props, &m_props );

	OnConnectToEntDefs(Sys_GetActiveDocument());

	MakeEntityMenu();
}

CEntity::~CEntity() 
{
	m_icomesh.FreeMesh();
	m_boxmesh.FreeMesh();
	m_lightbox_mesh.FreeMesh();
	m_lightbox_outline_mesh.FreeMesh();

	if( m_objs )
		delete[] m_objs;
	
	if( m_shadow_xyz )
		delete[] m_shadow_xyz;
	if( m_shadow_planes )
		delete[] m_shadow_planes;
	if( m_plane_facings )
		delete[] m_plane_facings;
	if( m_skm_tris )
		delete[] m_skm_tris;

	//FreeSkmMeshes();
	//FreeJmdlMesh();
}

void CEntity::OnSetVisible( bool bVisible, CTreadDoc* pDoc )
{
	CMapObject::OnSetVisible( bVisible, pDoc );
}

int* CEntity::GetOwnedObjectUIDs( int* num )
{
	*num = m_numobjs;
	return m_objs;
}

vec3 CEntity::GetModelAngles()
{
	if( !m_pDef )
		return vec3::zero;

	CObjProp* p = CObjProp::FindProp( &m_props, m_pDef->GetRotationKey() );
	if( !p )
		return vec3::zero;

	if( p->GetType() == CObjProp::vector )
		return p->GetVector();

	return vec3( 0, 0, p->GetFloat() );
}

void CEntity::OwnObjectList(  CTreadDoc* pDoc, CLinkedList<CMapObject>* list )
{
	CMapObject* obj;

	if( m_objs )
		delete[] m_objs;
	m_objs = 0;

	m_numobjs = list->GetCount();
	if( m_numobjs < 1 )
		return;

	m_objs = new int[m_numobjs];

	vec3 mins, maxs, mn, mx;

	mins = vec3::bogus_max;
	maxs = vec3::bogus_min;

	int i;
	for( obj = list->ResetPos(), i = 0; obj; obj = list->GetNextItem(), i++ )
	{
		obj->SetOwnerUID( GetUID() );

		obj->GetWorldMinsMaxs( &mn, &mx );
		mins = vec_mins( mn, mins );
		maxs = vec_maxs( mx, maxs );

		m_objs[i] = obj->GetUID();

		obj->UpdateOwnerContents( pDoc );
	}

	m_pos = (maxs+mins) * 0.5f;
}

void CEntity::UnlinkOwnedObjects( CTreadDoc* pDoc )
{
	int i;
	CMapObject* obj;
	
	if( m_numobjs < 1 )
		return;

	CMapObject** objs = pDoc->GetObjectsForOwner( GetUID(), m_numobjs, true, true );
	if( !objs )
		return;

	for(i = 0; i < m_numobjs; i++)
	{
		obj = objs[i];
		if( obj )
		{
			obj->SetOwnerUID( -1 );
			obj->UpdateOwnerContents( pDoc );
		}
	}

	delete[] objs;
}

void CEntity::RelinkOwnedObjects( CTreadDoc* pDoc )
{
	int i;
	CMapObject* obj;
	
	if( m_numobjs < 1 )
		return;

	CMapObject** objs = pDoc->GetObjectsFromUIDs(m_objs, m_numobjs, true, true);//pDoc->GetObjectsForOwner( GetUID(), m_numobjs, true, true );
	if( !objs )
		return;

	for(i = 0; i < m_numobjs; i++)
	{
		obj = objs[i];
		if( obj )
		{
			obj->SetOwnerUID( GetUID() );
			obj->UpdateOwnerContents( pDoc );
		}
	}

	delete[] objs;
}

void CEntity::SelectOwnedObjects( CTreadDoc* pDoc )
{
	int i;
	CMapObject* obj;
	CMapObject** objs = pDoc->GetObjectsForOwner( GetUID(), m_numobjs, true, false );
	if( !objs )
		return;

	for(i = 0; i < m_numobjs; i++)
	{
		obj = objs[i];
		if( obj )
		{
			if( !obj->IsVisible() )
			{
				obj->SetVisible( pDoc );
				pDoc->Prop_UpdateObjectState( obj );
			}
			
			obj->SetEntOwnerCall( true );
			obj->Select( pDoc );
		}
	}

	for(i = 0; i < m_numobjs; i++)
	{
		obj = objs[i];
		if( obj )
		{
			obj->SetEntOwnerCall( false );
		}
	}

	Select( pDoc );

	delete[] objs;
}

void CEntity::DeselectOwnedObjects( CTreadDoc* pDoc )
{
	int i;
	CMapObject* obj;
	CMapObject** objs = pDoc->GetObjectsForOwner( GetUID(), m_numobjs, false, true );
	if( !objs )
		return;

	for(i = 0; i < m_numobjs; i++)
	{
		obj = objs[i];
		if( obj )
		{
			obj->SetEntOwnerCall( true );
			obj->Deselect( pDoc );
		}
	}

	for(i = 0; i < m_numobjs; i++)
	{
		obj = objs[i];
		if( obj )
		{
			obj->SetEntOwnerCall( false );
		}
	}


	Deselect( pDoc );

	delete[] objs;
}

bool CEntity::ObjectIsFirstOwned( CMapObject* obj )
{
	if( m_numobjs < 1 )
		return false;

	if( m_objs[0] == obj->GetUID() )
		return true;

	return false;
}

void CEntity::SelectByShader( const char* szShader, CTreadDoc* pDoc )
{
	if( m_pDef == 0 || m_pDef->GetDrawStyle() != EDDS_JMODEL )
		return;

	if( !szShader || !szShader[0] )
		return;

	if( m_sShader == szShader )
	{
		if( !IsSelected() )
			Select( pDoc );
	}
}

bool CEntity::CheckObject( CString& message, CTreadDoc* pDoc )
{
	if( !m_pDef )
	{
		message = "class definition not found for entity!";
		return true;
	}

	if( m_pDef->GetDrawStyle() == EDDS_JMODEL )
	{
		/*if( m_jmodel == 0 )
		{
			message = "missing jmodel!";
			return true;
		}*/
	}

	if( m_pDef->IsOwner() )
	{
		//
		// if we have objects, make sure they are linked to US!
		//
		if( m_numobjs < 1 )
		{
			message.Format("detached %s! (0)", m_pDef->GetDisplayName());
			return true;
		}

		CMapObject** objs = pDoc->GetObjectsForOwner( GetUID(), m_numobjs, true, true );
		if( !objs )
		{
			message.Format("detached %s! (1)", m_pDef->GetDisplayName());
			return true;
		}

		int i;
		for(i = 0; i < m_numobjs; i++)
		{
			if( objs[i] )
			{
				int id = objs[i]->GetOwnerUID();
				if( id != GetUID() )
				{
					message.Format("detached %s! (2)", m_pDef->GetDisplayName());
					return true;
				}
			}
			else
			{
				message.Format("detached %s! (3)", m_pDef->GetDisplayName());
				return true;
			}
		}

		delete[] objs;
	}

	return false;
}

CMapObject* CEntity::MakeFromKeyList( CTreadDoc* pDoc, CLinkedList<CEntKey>* list )
{
	CEntKey* classkey;

	classkey = FindKey( list, "classname" );
	if( !list )
		return 0;

	CEntity* ent = new CEntity();

	bool jmodel = false;

	if( !stricmp( classkey->StringForKey(), "jmodel" ) )
	{
		jmodel = true;

		CEntKey* key = FindKey( list, "model" );
		
		char buff[256];
		strcpy( buff, key->StringForKey() );

		//
		// skip /vmodels/
		//
		CString s = &buff[9];
		s.Replace( '/', ':' );

		ent->m_sClassname.Format("jmodel@@%s", s );
		
		key = FindKey( list, "shader" );
		if( key )
			ent->m_sShader = key->StringForKey();
	}
	else
	{
		ent->m_sClassname = classkey->StringForKey();
	}

	CEntKey* key;

	for(key = list->ResetPos(); key; key = list->GetNextItem() )
	{
		if( key == classkey )
			continue;

		//
		// skip jmodel keys?
		//
		if( jmodel )
		{
			if( !stricmp( key->GetName(), "model" ) || 
				!stricmp( key->GetName(), "shader" ) )
			{
				continue;
			}
		}

		//
		// special keys.
		//
		if( !stricmp( key->GetName(), "origin" ) ) 
		{
			ent->m_pos = key->VecForKey();
			continue;
		}

		//
		// make a prop for it.
		//
		CObjProp* p = new CObjProp();
		p->SetName( key->GetName() );
		p->SetDisplayName( key->GetDisplayName() );
		p->SetString( key->StringForKey() );

		ent->m_props.AddItem( p );
	}

	ent->OnConnectToEntDefs(pDoc);

	return ent;
}

CEntKey* CEntity::FindKey( CLinkedList<CEntKey>* list, const char* name )
{
	CEntKey* key;

	if( !name || !name[0] )
		return 0;

	for( key = list->ResetPos(); key; key = list->GetNextItem() )
	{
		if( !stricmp( key->GetName(), name ) )
			return key;
	}

	return 0;
}

//void CEntity::FreeJmdlMesh()
//{
//	if( m_jmdlmesh.xyz )
//		delete[] m_jmdlmesh.xyz;
//	if( m_jmdlmesh.normals[0] )
//		delete[] m_jmdlmesh.normals[0];
//	if( m_jmdlmesh.normals[1] )
//		delete[] m_jmdlmesh.normals[1];
//	if( m_jmdlmesh.normals[2] )
//		delete[] m_jmdlmesh.normals[2];
//
//	m_jmdlmesh.xyz = 0;
//	m_jmdlmesh.normals[0] = 
//	m_jmdlmesh.normals[1] = 
//	m_jmdlmesh.normals[2] = 0;
//	
//	m_jmodel = 0;
//}

void CEntity::CopyPropList( CLinkedList<CObjProp>* src, CLinkedList<CObjProp>* dst )
{
	CObjProp* p, *t;

	for( p = src->ResetPos(); p ; p = src->GetNextItem() )
	{
		t = new CObjProp(*p);
		dst->AddItem( t );
	}
}

CLinkedList<CObjProp>* CEntity::GetPropList( CTreadDoc* pDoc )
{
	if( m_pDef && m_pDef->IsOwner() )
	{
		if( pDoc->IsEditingFaces() ) // let us get to face properties on linked objects.
			return 0;
	}

	return &m_props;
}

void CEntity::SetProp( CTreadDoc* pDoc, CObjProp* prop )
{
	CObjProp* t = CObjProp::FindProp( &m_props, prop->GetName() );
	if( !t )
		return;

	t->SetValue( prop );

	//
	// are we a skel?
	//
	if( m_pDef )
	{
		if( m_pDef->GetDrawStyle() == EDDS_SKEL )
		{
			if( m_pDef->ModelFromKey() )
			{
				if( !stricmp( t->GetName(), "skel" ) )
				{
					//FreeSkmMeshes();
					//SetupSkmMesh();
					//SetLightingRebuildFlag();
				}
				else
				if( !stricmp( t->GetName(), "edit_model" ) )
				{
					/*if( m_skm )
					{
						m_skm->SetAnimation( t->GetString() );
						UpdateSkelMesh();
						SetLightingRebuildFlag();
					}*/
				}		
			}
		}
		if( m_pDef->GetDrawStyle() == EDDS_JMODEL )
		{
			if( !stricmp( prop->GetName(), "modelpath" ) )
			{
				//
				// try to change it...
				//
				m_sClassname.Format("jmodel@@%s", prop->GetString());
				m_sClassname.Replace('/', ':');
				m_sClassname.Replace('\\', ':');
				OnConnectToEntDefs(pDoc);
				pDoc->BuildSelectionBounds();
				pDoc->UpdateSelectionInterface();
				return;
			}
		}

		if( m_pDef->GetDrawStyle() == EDDS_SKEL ||
			m_pDef->GetDrawStyle() == EDDS_JMODEL )
		{
			if( !stricmp( prop->GetName(), m_pDef->GetRotationKey() ) )
			{
				if( prop->GetType() == CObjProp::facing ) 
				{
					m_model_angles[2] = prop->GetFloat();
					m_model_angles[0] = m_model_angles[1] = 0.0f;
				}
				else
				{
					m_model_angles = prop->GetVector();
				}

				int i;
				for(i = 0; i < 3; i++)
				{
					m_model_angles[i] = CLAMP_VAL( m_model_angles[i], 0, 360.0f );
				}

				/*UpdateSkelMesh();
				UpdateJmdlMesh();
				SetLightingRebuildFlag();*/

				if( prop->GetType() == CObjProp::facing ) 
				{
					prop->SetFloat( m_model_angles[2] );
				}
				else
				{
					prop->SetVector( m_model_angles );
				}
				
				t->SetValue( prop );
				
				pDoc->Prop_PropChange( prop->GetName() );
			}
		}

		if( !stricmp( t->GetName(), "light_size" ) ||
			!stricmp( t->GetName(), "function_range" ) ||
			!stricmp( t->GetName(), "light_flags" ) )
		{
			SetupLightBoxMesh();
		}
	}

}

void CEntity::SetupProps( CLinkedList<CObjProp>* master_list )
{
	if( m_props.IsEmpty() )
	{
		CEntity::CopyPropList( master_list, &m_props );
		return;
	}

	//
	// we have a prop list, update/add keys.
	//
	CObjProp* p, *t;

	for( p = master_list->ResetPos(); p; p = master_list->GetNextItem() )
	{
		t = CObjProp::FindProp( &m_props, p->GetName() );
		if( t )
		{
			m_props.RemoveItem( t );

			CObjProp* newp = new CObjProp( *p );
			newp->SetValue( t );

			m_props.AddItem( newp );
			delete t;
		}
		else
		{
			t = new CObjProp( *p );
			m_props.AddItem( t );
		}
	}
}

void CEntity::SetShaderName( const char* name, CTreadDoc* pDoc )
{
	m_sShader = name;
	//if( m_pDef && m_jmodel )
	//{
	//	m_jmdlmesh.shader = ShaderForName( name );
	//}
}

void CEntity::OnRemoveFromMap( CTreadDoc* pDoc )
{
	CMapObject::OnRemoveFromMap( pDoc );
}

void CEntity::OnAddToMap( CTreadDoc* pDoc )
{
	CMapObject::OnAddToMap( pDoc );
}

bool CEntity::WriteToFile( CFile* pFile, CTreadDoc* pDoc, int nVersion )
{
	CMapObject::WriteToFile( pFile, pDoc, nVersion );

	MAP_WriteString( pFile, m_sClassname );
	MAP_WriteVec3( pFile, &m_pos );
	
	MAP_WriteVec3( pFile, &m_model_angles );
	MAP_WriteString( pFile, m_sShader );

	//
	// write the properties.
	//
	int c = m_props.GetCount();
	MAP_WriteInt( pFile, c );

	CObjProp* p;
	for( p = m_props.ResetPos(); p; p = m_props.GetNextItem() )
	{
		MAP_WriteString( pFile, p->GetName() );
		MAP_WriteString( pFile, p->GetString() );
	}

	//
	// write owned objects.
	//
	MAP_WriteInt( pFile, m_numobjs );
	for( c = 0; c < m_numobjs; c++ )
		MAP_WriteInt( pFile, m_objs[c] );

	return true;
}

bool CEntity::ReadFromFile( CFile* pFile, CTreadDoc* pDoc, int nVersion )
{
	CMapObject::ReadFromFile( pFile, pDoc, nVersion );

	m_sClassname = MAP_ReadString( pFile );
	MAP_ReadVec3( pFile, &m_pos );

	MAP_ReadVec3( pFile, &m_model_angles );
	m_sShader = MAP_ReadString( pFile );
	
	{
		int c = MAP_ReadInt( pFile );
		if( c > 0 )
		{
			CObjProp* p;
			int i;

			for(i = 0; i < c; i++)
			{
				p = new CObjProp();
				p->SetName( MAP_ReadString( pFile ) );
				p->SetDisplayName(p->GetName());
				p->SetString( MAP_ReadString( pFile ) );
				m_props.AddItem( p );
			}
		}
	}

	{
		m_numobjs = MAP_ReadInt( pFile );
		m_objs = 0;

		if( m_numobjs > 0 )
		{
			m_objs = new int[m_numobjs];

			int i;
			for( i = 0; i < m_numobjs; i++)
				m_objs[i] = MAP_ReadInt( pFile );
		}
	}

	OnConnectToEntDefs(pDoc);

	return true;
}

//void CEntity::FreeSkmMeshes()
//{
//	if( m_skm )
//		delete m_skm;
//
//	m_skm = 0;
//
//	if( m_skm_meshes )
//	{
//		int i;
//		for(i = 0; i < m_skm_mesh_count; i++)
//		{
//			m_skm_meshes[i].st = 0;
//			m_skm_meshes[i].FreeMesh();
//		}
//
//		delete[] m_skm_meshes;
//	}
//
//	m_skm_mesh_count = 0;
//	m_skm_meshes = 0;
//}

void CEntity::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	m_bDrag = false;

	if( !IsSelected() )
	{
		if( !(nButtons&MS_CONTROL) )
		{
			pView->GetDocument()->MakeUndoDeselectAction();
			pView->GetDocument()->ClearSelection();
		}

		Select( pView->GetDocument(), pSrc );
		pView->GetDocument()->Prop_UpdateSelection();
		if( pView->GetViewType() != VIEW_TYPE_3D )
		{
			m_bDrag = true;
			Sys_BeginDragSel( pView, nMX, nMY, nButtons );
		}
		pView->GetDocument()->UpdateSelectionInterface();
	}
	else
	{
		if( nButtons&MS_CONTROL )
		{
			pView->GetDocument()->MakeUndoDeselectAction();
			Deselect( pView->GetDocument() );
			pView->GetDocument()->Prop_UpdateSelection();
			pView->GetDocument()->UpdateSelectionInterface();
		}
		else
		{
			if( pView->GetViewType() != VIEW_TYPE_3D )
			{
				m_bDrag = true;
				Sys_BeginDragSel( pView, nMX, nMY, nButtons );
			}
			else
			{
				pView->GetDocument()->MakeUndoDeselectAction();
				pView->GetDocument()->ClearSelection();

				Select( pView->GetDocument() );
				pView->GetDocument()->Prop_UpdateSelection();
				pView->GetDocument()->UpdateSelectionInterface();
			}
		}
	}

	Sys_RedrawWindows();
}

void CEntity::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( m_bDrag )
	{
		m_bDrag = false;
		Sys_EndDragSel( pView, nMX, nMY, nButtons );
	}
}

void CEntity::OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( m_bDrag )
	{
		Sys_DragSel( pView, nMX, nMY, nButtons );
	}
}

bool CEntity::OnPopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	if( !IsSelected() )
		return false;

	CEntity::m_EntityMenu.view = pView;
	CEntity::m_EntityMenu.entity = this;
	
	Sys_DisplayObjectMenu( pView, nMX, nMY, &CEntity::m_EntityMenu );
	return true;
}

const char* CEntity::GetClassname()
{
	return m_sClassname;
}

const char* CEntity::GetDisplayName()
{
	return m_pDef ? m_pDef->GetDisplayName() : GetClassname();
}

bool CEntity::CanAddToTree()
{
	if( m_pDef && m_pDef->IsOwner() )
		return false;

	return true;
}

void CEntity::SetupLightBoxMesh()
{
	if( !m_pDef || stricmp( m_pDef->GetClassName(), "light" ) )
		return;

	m_bLightBoxMesh = false;
	m_lightbox_mesh.FreeMesh();
	m_lightbox_outline_mesh.FreeMesh();

	//
	// get the light size.
	//
	CObjProp* p = CObjProp::FindProp( &m_props, "light_size" );
	CObjProp* p2 = CObjProp::FindProp( &m_props, "function_range" );
	CObjProp* p3 = CObjProp::FindProp( &m_props, "light_flags" );

	if( p && p2 && p3 )
	{
		vec3 v, mn, mx;
		float maxi, temp;

		sscanf( p2->GetString(), "%f %f", &temp, &maxi );

		v = p->GetVector();
		v *= maxi*0.5f;
		mn = m_pos - v;
		mx = m_pos + v;

		R_MakeBoxMeshCmds( &m_lightbox_mesh, mn, mx, true, false ); // non-solid.
		
		//
		// no shadows?
		//
		if( p3->GetInt() & 1 )
		{
			m_lightbox_mesh.color2d = 0x2000228F;
			m_lightbox_mesh.color3d = 0x2000228F;
		}
		else
		{
			m_lightbox_mesh.color2d = 0x20FFFF00;
			m_lightbox_mesh.color3d = 0x20FFFF00;
		}
		
		m_lightbox_mesh.solid2d = true;
		m_lightbox_mesh.allow_selected = false;
		m_lightbox_mesh.allow_wireframe = false;

		m_lightbox_mesh.fcolor3d[0] = 1.0f;
		m_lightbox_mesh.fcolor3d[1] = 1.0f;
		m_lightbox_mesh.fcolor3d[2] = 1.0f;
		m_lightbox_mesh.fcolor3d[3] = 1.0f;
	
		//
		// outline
		//
		R_MakeBoxMeshCmds( &m_lightbox_outline_mesh, mn, mx, false, false ); // non-solid.
		
		m_lightbox_outline_mesh.color2d = 0xFFFFFFFF;
		m_lightbox_outline_mesh.color3d = 0xFFFFFFFF;
		m_lightbox_outline_mesh.solid2d = false;
		m_lightbox_outline_mesh.allow_selected = false;
		m_lightbox_outline_mesh.allow_wireframe = false;

		m_lightbox_outline_mesh.fcolor3d[0] = 1.0f;
		m_lightbox_outline_mesh.fcolor3d[1] = 1.0f;
		m_lightbox_outline_mesh.fcolor3d[2] = 1.0f;
		m_lightbox_outline_mesh.fcolor3d[3] = 1.0f;

		m_bLightBoxMesh = true;
	}
}

void CEntity::SetupIconMesh()
{
	char buff[1024];
	FS_MakeBurgerPath( m_pDef->GetIcon(), buff );
	TC_CacheTexture( &m_iconhandle, &m_iconkey, buff, GL_LUMINANCE, TRUE );

	if( TC_VALID( m_iconhandle, m_iconkey ) )
	{
		m_icomesh.xyz = new vec3[4];
		m_icomesh.normals[0] = new vec3[4];
		m_icomesh.tris = new unsigned short[6];
		m_icomesh.st = new vec2[4];

		m_icomesh.num_pts = 4;
		m_icomesh.num_tris = 2;

		m_icomesh.color2d = 0xFFFFFFFF;
		m_icomesh.color3d = 0xFFFFFFFF;
		m_icomesh.wireframe3d = 0xFFFFFFFF;
		m_icomesh.fcolor3d[0] = 
		m_icomesh.fcolor3d[1] = 
		m_icomesh.fcolor3d[2] = 
		m_icomesh.fcolor3d[3] = 1.0f;

		m_icomesh.solid2d = true;
		m_icomesh.pick = this;
		m_icomesh.allow_wireframe = false;

		int i;
		for(i = 0; i < 4; i++)
			m_icomesh.normals[0][i] = normalized(sysAxisX+sysAxisY);

		m_icomesh.st[0].x = 0.0f;
		m_icomesh.st[0].y = 0.0f;

		m_icomesh.st[1].x = 1.0f;
		m_icomesh.st[1].y = 0.0f;
		
		m_icomesh.st[2].x = 1.0f;
		m_icomesh.st[2].y = 1.0f;

		m_icomesh.st[3].x = 0.0f;
		m_icomesh.st[3].y = 1.0f;

		m_icomesh.tris[0] = 0;
		m_icomesh.tris[1] = 1;
		m_icomesh.tris[2] = 2;
		m_icomesh.tris[3] = 2;
		m_icomesh.tris[4] = 3;
		m_icomesh.tris[5] = 0;
	}

	SetupLightBoxMesh();
}

void CEntity::RepaintShader( const char* szShader, CTreadDoc* pDoc )
{
}

void CEntity::GenTriMeshTXSpaceVecs( CRenderMesh* mesh )
{
	int i, k, ofs, prev, next;
	int a, b, c;
	vec3* xyz[3];
	vec2* st[3];
	vec3  out[2];

	for(i = 0; i < mesh->num_pts; i++)
	{
		mesh->normals[1][i] = vec3::zero;
		mesh->normals[2][i] = vec3::zero;
	}

	for(i = 0; i < mesh->num_tris; i++)
	{
		ofs = i+i+i;
		for(k = 0; k < 3; k++)
		{
			prev = k-1;
			next = k+1;

			if( prev < 0 )
				prev = 2;
			if( next > 2 )
				next = 0;

			a = mesh->tris[prev+ofs];
			b = mesh->tris[k+ofs];
			c = mesh->tris[next+ofs];

			xyz[0] = &mesh->xyz[a];
			st[0]  = &mesh->st[a];
			xyz[1] = &mesh->xyz[b];
			st[1]  = &mesh->st[b];
			xyz[2] = &mesh->xyz[c];
			st[2]  = &mesh->st[c];

			R_GetTXSpaceVecs( xyz, &mesh->normals[0][b], st, out );

			mesh->normals[1][b] += out[0];
			mesh->normals[2][b] += out[1];
		}
	}

	for(i = 0; i < mesh->num_pts; i++)
	{
		mesh->normals[1][i].normalize();
		mesh->normals[2][i].normalize();
	}
}

int CEntity::GetObjectTypeBits()
{
	/*if( m_skm )
		return OBJECT_TYPE_SKEL;
	if( m_jmodel )
		return OBJECT_TYPE_JMODEL;*/

	return 0;
}

void CEntity::OnConnectToEntDefs(CTreadDoc* doc)
{
	m_icon = 0;
	m_iconhandle = BAD_TC_HANDLE;
	m_iconkey = BAD_TC_HANDLE;
	m_icomesh.FreeMesh();
	m_icomesh.shader = 0;
	m_icomesh.texture = 0;
	m_boxmesh.FreeMesh();
	m_boxmesh.shader = 0;
	m_boxmesh.texture = 0;
	//FreeSkmMeshes();
	//FreeJmdlMesh();

	if( m_sClassname != "" )
		m_pDef = doc->GameDef()->EntDefForName( m_sClassname );
	else
	{
		m_pDef = 0;
		return;
	}

	if( !m_pDef )
	{
		//
		// setup the box mesh.
		//
		m_boxpos = vec3::zero;
		
		R_MakeBoxMeshCmds( &m_boxmesh, vec3( -8, -8, -8 ), vec3( 8, 8, 8 ) );

		{
			m_boxmesh.color2d = 0xFFFF0000;
			m_boxmesh.color3d = m_boxmesh.color2d;

			m_boxmesh.fcolor3d[0] = 0.0f;
			m_boxmesh.fcolor3d[1] = 0.0f;
			m_boxmesh.fcolor3d[2] = 1.0f;
			m_boxmesh.fcolor3d[3] = 1.0f;
		}

		m_boxmesh.pick = this;
		m_boxmesh.solid2d = true;

		UpdateBoxMesh();
		return;
	}

	SetupProps( m_pDef->GetKeys() );

	//
	// sync up the angles...
	//
	{
		CObjProp* p = CObjProp::FindProp( &m_props, m_pDef->GetRotationKey() );
		if( p )
		{
			if( p->GetType() == CObjProp::facing )
			{
				p->SetFloat( m_model_angles[2] );
			}
			else
			{
				p->SetVector( m_model_angles );
			}
		}
	}

	//
	// load up the icon.
	//
	if( m_pDef->GetIcon() != 0 )
	{
		SetupIconMesh();
	}
	if( m_pDef->GetModel() != 0 )
	{
		//if( m_pDef->GetDrawStyle() == EDDS_SKEL )
		//	SetupSkmMesh();
		//else
		//if( m_pDef->GetDrawStyle() == EDDS_JMODEL )
		//	SetupJmdlMesh();
	}

	//
	// setup the box mesh.
	//
	m_boxpos = vec3::zero;
	vec3 mins, maxs, color;

	m_pDef->GetBoxInfo( &mins, &maxs, &color );
	R_MakeBoxMeshCmds( &m_boxmesh, mins, maxs );

	{
		LongWord r, g, b, a;

		a = 255;
		r = (255.0f*color.x);
		g = (255.0f*color.y);
		b = (255.0f*color.z);

		m_boxmesh.color2d = PAK_GL_RGBA( r, g, b, a );
		m_boxmesh.color3d = m_boxmesh.color2d;

		m_boxmesh.fcolor3d[0] = color.x;
		m_boxmesh.fcolor3d[1] = color.y;
		m_boxmesh.fcolor3d[2] = color.z;
		m_boxmesh.fcolor3d[3] = 1.0f;
	}

	m_boxmesh.pick = this;
	m_boxmesh.solid2d = true;

	UpdateBoxMesh();
	SetupLightBoxMesh();
}

void CEntity::UpdateBoxMesh()
{
	vec3 t = m_pos - m_boxpos;

	int i;
	for(i = 0; i < m_boxmesh.num_pts; i++)
	{
		m_boxmesh.xyz[i] += t;
	}

	m_boxpos = m_pos;
}

void CEntity::GetObjectMinsMaxs( vec3* pMins, vec3* pMaxs )
{
	if( m_pDef )
	{
		switch( m_pDef->GetDrawStyle() )
		{
		case EDDS_JMODEL:

			//if( m_jmodel )
			//{
			//	//*pMins = m_jmodel->mins;
			//	//*pMaxs = m_jmodel->maxs;
			//	int i;
			//	vec3 mn, mx;

			//	mn = vec3::bogus_max;
			//	mx = vec3::bogus_min;

			//	for(i = 0; i < m_jmdlmesh.num_pts; i++)
			//	{
			//		mn = vec_mins( m_jmdlmesh.xyz[i], mn );
			//		mx = vec_maxs( m_jmdlmesh.xyz[i], mx );
			//	}

			//	*pMins = mn - m_pos;
			//	*pMaxs = mx - m_pos;

			//	return;
			//}

		break;
		}

		vec3 cl;
		m_pDef->GetBoxInfo( pMins, pMaxs, &cl );
	}
	else
	{
		//
		// default.
		//
		*pMins = vec3( -8, -8, -8 );
		*pMaxs = vec3( 8, 8, 8 );
	}
}

void CEntity::GetWorldMinsMaxs( vec3* mins, vec3* maxs )
{
	vec3 mn, mx;
	GetObjectMinsMaxs( &mn, &mx );

	*mins = m_pos + mn;
	*maxs = m_pos + mx;
}

vec3 CEntity::GetObjectWorldPos()
{
	return m_pos;
}

void CEntity::SetObjectWorldPos( const vec3& pos, CTreadDoc* pDoc )
{
	m_pos = pos;
	UpdateBoxMesh();
	/*UpdateSkelMesh();
	UpdateJmdlMesh();*/
	SetupLightBoxMesh();
	CMapObject::SetObjectWorldPos( pos, pDoc );
}

void CEntity::SetObjectTransform( const mat3x3& m, CTreadDoc* pDoc )
{
	if( m_pDef )
	{
		if( m_pDef->GetDrawStyle() == EDDS_SKEL ||
			m_pDef->GetDrawStyle() == EDDS_JMODEL )
		{
			vec3 angles = euler_from_matrix( m );

			m_model_angles[0] += RADIANS_TO_DEGREES(angles[2]);
			m_model_angles[1] += RADIANS_TO_DEGREES(angles[0]);
			m_model_angles[2] += RADIANS_TO_DEGREES(angles[1]);
			
			int i;
			for(i = 0; i < 3; i++)
			{
				if( m_model_angles[i] > 360.0f )
					m_model_angles[i] = m_model_angles[i] - 360.0f;
				if( m_model_angles[i] < 0.0f )
					m_model_angles[i] = 360.0f + m_model_angles[i];
			}

			CObjProp* p = CObjProp::FindProp( &m_props, m_pDef->GetRotationKey() );
			if( p )
			{
				if( p->GetType() == CObjProp::facing )
				{
					p->SetFloat( m_model_angles[2] );
					m_model_angles[0] = m_model_angles[1] = 0.0f;
				}
				else
				{
					p->SetVector( m_model_angles );
				}

				pDoc->Prop_PropChange( p->GetName() );
				pDoc->Prop_SelectProp( p->GetName() );
			}

			/*UpdateSkelMesh();
			UpdateJmdlMesh();*/
		}
		else
		{
			CObjProp* p = CObjProp::FindProp( &m_props, "angles" );
			if( p )
			{
				vec3 temp = euler_from_matrix( m );
				vec3 delta;

				delta[0] = RADIANS_TO_DEGREES(temp[2]);
				delta[1] = RADIANS_TO_DEGREES(temp[0]);
				delta[2] = RADIANS_TO_DEGREES(temp[1]);

				vec3 angles = p->GetVector();

				p->SetVector( angles + delta );
				pDoc->Prop_PropChange( p->GetName() );
				pDoc->Prop_SelectProp( p->GetName() );
			}
		}
	}

	CMapObject::SetObjectTransform( m, pDoc );
}

void CEntity::OnAnimationMode( CTreadDoc* pDoc, bool animating )
{
	CMapObject::OnAnimationMode( pDoc, animating );
}

void CEntity::CopyObject( CMapObject* obj, CTreadDoc* pDoc )
{
	CEntity* ent = dynamic_cast<CEntity*>(obj);
	if( !ent )
		return;

	m_pos = ent->m_pos;
	m_model_angles = ent->m_model_angles;

	{
		CObjProp* p1 = CObjProp::FindProp( &ent->m_props, "angles" );
		if( p1 )
		{
			CObjProp* p2 = CObjProp::FindProp( &m_props, "angles" );
			if( p2 )
				p2->SetValue( p1 );
		}
	}

	UpdateBoxMesh();
	/*UpdateSkelMesh();
	UpdateJmdlMesh();*/
	SetupLightBoxMesh();
}

int CEntity::GetClass()
{
	return MAPOBJ_CLASS_ENTITY;
}

int CEntity::GetSubClass()
{
	if( m_pDef && m_pDef->IsOwner() )
		return MAPOBJ_SUBCLASS_OWNER;

	return MAPOBJ_SUBCLASS_NONE;
}

CMapObject* CEntity::Clone()
{
	CEntity* e = new CEntity(*this);
	return e;
}

int CEntity::GetNumRenderMeshes( CMapView* view )
{
	if( m_pDef && m_pDef->IsOwner() )
		return 0;

	if( m_pDef && m_pDef->GetDrawStyle() == EDDS_ICON )
	{
		if( view->View.bShowIcons == FALSE ) return 0;
	}

	/*if( m_pDef && m_skm && m_skm_mesh_count && m_pDef->GetDrawStyle() == EDDS_SKEL )
	{
		return m_skm_mesh_count;
	}*/

	return (m_bLightBoxMesh && IsSelected() && ((view->GetDocument()->GetViewLightingFlag()==false)||(view->GetViewType() != VIEW_TYPE_3D))) ? 3 : 1;
}

CRenderMesh* CEntity::GetRenderMesh( int num, CMapView* view )
{
	if( m_pDef )
	{
		/*if( m_pDef->GetDrawStyle() == EDDS_SKEL &&
			m_skm_mesh_count && m_skm )
		{
			CRenderMesh* m = &m_skm_meshes[num];

			return m;
		}
		else
		if( m_pDef->GetDrawStyle() == EDDS_JMODEL &&
			m_jmodel )
		{
			return &m_jmdlmesh;
		}*/

		if( TC_VALID( m_iconhandle, m_iconkey ) &&
			m_icon == 0 )
		{
			m_icon = TC_AllocRTex( m_iconhandle, m_iconkey, TCRTEX_ICON, "icon", IconTexUpload, 0, 0 );
			if( m_icon )
			{
				m_icon->user_data[0] = (void*)m_iconhandle;
				m_icon->user_data[1] = (void*)m_iconkey;
			}
		}

		if( m_icon && m_pDef && num == 0 )
		{
			//
			// setup the icon mesh...
			//
			vec3 up, lf;
			vec2 size;

			if( view->GetViewType() == VIEW_TYPE_3D )
			{
				up = view->View.or3d.up;
				lf = view->View.or3d.lft;
			}
			else
			{
				up = view->View.or2d.up;
				lf = view->View.or2d.lft;
			}

			m_pDef->GetIconSize( &size );
			size *= 0.5;

			up *= size.x;
			lf *= size.y;

			m_icomesh.xyz[0] = m_pos + up + lf;
			m_icomesh.xyz[1] = m_pos + up - lf;
			m_icomesh.xyz[2] = m_pos - up - lf;
			m_icomesh.xyz[3] = m_pos - up + lf;

			m_icomesh.texture = m_icon;

			return &m_icomesh;
		}
	}

	if( num == 1 && m_bLightBoxMesh )
	{
		return &m_lightbox_outline_mesh;
	}
	if( num == 2 && m_bLightBoxMesh )
	{
		return &m_lightbox_mesh;
	}

	return &m_boxmesh;
}

void CEntity::FlipObject( const vec3& origin, const vec3& axis )
{
	CMapObject::FlipObject( origin, axis );

	float d;
		
	d = dot( m_pos, axis ) - dot( origin, axis );
	m_pos = (axis*(2*-d))+m_pos;

	UpdateBoxMesh();
	/*UpdateSkelMesh();
	UpdateJmdlMesh();*/
	SetupLightBoxMesh();
}

const char* CEntity::GetRootName()
{
	if( m_pDef )
	{
		if( m_pDef->GetDrawStyle() == EDDS_JMODEL )
		{
			return "jmodel";
		}

		return GetDisplayName();
	}

	return CMapObject::GetRootName();
}

void CEntity::SnapToGrid( float fGridSize, bool x, bool y, bool z )
{
	vec3 p = m_pos;

	if( x )
		p.x = Sys_Snapf( p.x, fGridSize );
	if( y )
		p.y = Sys_Snapf( p.y, fGridSize );
	if( z )
		p.y = Sys_Snapf( p.z, fGridSize );

	m_pos = p;
	UpdateBoxMesh();
	/*UpdateSkelMesh();
	UpdateJmdlMesh();*/
	SetupLightBoxMesh();

	CMapObject::SnapToGrid( fGridSize, x, y, z );
}

CMapObject* CEntity::MakeEntity( CTreadDoc* pDoc, void* parm )
{
	CEntDef* def = (CEntDef*)parm;

	CEntity* ent = new CEntity();

	ent->m_sClassname = def->GetClassName();
	ent->OnConnectToEntDefs(pDoc);
	ent->SetShaderName( pDoc->SelectedShaderName(), pDoc );

	return ent;
}

//////////////////////////////////////////////////////////////////////
// CEntDef															//
//////////////////////////////////////////////////////////////////////

class CEntityObjectCreator : public CObjectCreator
{
public:
	CEntityObjectCreator(CEntDef *def) : m_def(def) 
	{
		m_name = CString("Entities\n") + def->GetTreeDisplayName();
	}
	virtual void Release() { delete this; }
	virtual const char *Name() { return m_name; }
	virtual CMapObject *CreateObject(CTreadDoc *doc)
	{
		return CEntity::MakeEntity(doc, m_def);
	}
private:
	CEntDef *m_def;
	CString m_name;
};

IMPLEMENT_TREAD_NEW(CEntDef);

CEntDef::CEntDef() : CLLObject()
{
	m_owner = false;
	m_bIsBaseClass = false;
	m_boxcolor = vec3( 0, 0, 1 );
	m_style = EDDS_BOX;
	m_boxmins = vec3( -8, -8, -8 );
	m_boxmaxs = vec3( 8, 8, 8 );
	m_skelofs = vec3::zero;
	m_sRotation = "";
	m_attflags = 0;
}

CEntDef::~CEntDef()
{
}

CObjectCreator *CEntDef::MakeObjectFactory()
{
	if (IsBaseClass() || IsOwner()) return 0;
	return new CEntityObjectCreator(this);
}

bool CEntDef::IsBaseClass()
{
	return m_bIsBaseClass;
}

bool CEntDef::IsSkelFixedOffset()
{
	return m_skel_fixed_offset;
}

void CEntDef::GetSkelOfs( vec3* ofs )
{
	*ofs = m_skelofs;
}

const char* CEntDef::GetClassName()
{
	return m_sClassname;
}

const char* CEntDef::GetDisplayName()
{
	return m_sDisplayName;
}

const char* CEntDef::GetTreeDisplayName()
{
	return m_sTreeDisplayName;
}

CLinkedList<CObjProp>* CEntDef::GetKeys()
{
	return &m_keys;
}

int  CEntDef::GetDrawStyle()
{
	return m_style;
}

void CEntDef::GetBoxInfo( vec3* mins, vec3* maxs, vec3* color )
{
	*mins = m_boxmins;
	*maxs = m_boxmaxs;
	*color = m_boxcolor;
}

void CEntDef::GetIconSize( vec2* size )
{
	*size = m_iconsize;
}

const char* CEntDef::GetIcon()
{
	if( m_sIcon == "" )
		return 0;

	return m_sIcon;
}

const char* CEntDef::GetModel()
{
	if( m_sModel == "" )
		return 0;

	return m_sModel;
}

const char* CEntDef::GetAnimName()
{
	return m_sAnim;
}

bool CEntDef::IsOwner()
{
	return m_owner;
}

bool CEntDef::Inherit(CEntDef *def)
{
	if (def->m_attflags & Att_BoxSize)
	{	
		m_boxmins  = def->m_boxmins;
		m_boxmaxs  = def->m_boxmaxs;
	}
	if (def->m_attflags & Att_BoxColor)
	{
		m_boxcolor = def->m_boxcolor;
	}
	if (def->m_attflags & Att_Owner)
	{
		m_owner = def->m_owner;
	}
	if (def->m_attflags & Att_ModelType)
	{
		// doesn't do anything right now.
	}
	if (def->m_attflags & Att_Icon)
	{
		m_iconsize = def->m_iconsize;
		m_sIcon = def->m_sIcon;
		m_style = def->m_style;
	}
	if (def->m_attflags & Att_TargetSrc)
	{
		m_sTargetSrc = def->m_sTargetSrc;
	}
	if (def->m_attflags & Att_TargetDst)
	{
		m_sTargetDst = def->m_sTargetDst;
	}
	if (def->m_attflags & Att_Category)
	{
		m_sCategory = def->m_sCategory;
	}
	if (def->m_attflags & Att_Repaint)
	{
		m_sRepaint = def->m_sRepaint;
	}

	m_attflags |= def->m_attflags;

	for (CObjProp *p = def->GetKeys()->ResetPos(); p; p = def->GetKeys()->GetNextItem())
	{
		InheritProp(p);
	}

	return true;
}

void CEntDef::InheritProp(CObjProp *prop)
{
	CObjProp *exists = FindKey(prop->GetName());
	if (!exists)
	{
		exists = new CObjProp();
		GetKeys()->AddItem(exists);
	}
	exists->Inherit(*prop);
}

CObjProp *CEntDef::FindKey(const char *name)
{
	for (CObjProp *p = GetKeys()->ResetPos(); p; p = GetKeys()->GetNextItem())
	{
		if (!strcmp(p->GetName(), name)) return p;
	}
	return 0;
}

bool CEntDef::Parse( C_Tokenizer* script, CGameDef *def )
{
	CString t;

	if( !script->GetToken( t ) )
		return false;

	// parse pre-amble.

	if (t == "@base")
	{
		m_bIsBaseClass = true;
		if( !script->GetToken( t ) )
			return false;
	}
	else if (t == "@worldspawn")
	{
		if (def->m_worldspawn)
		{
			Sys_printf("multiple worldspawns!\n");
			return false;
		}
		else
		{
			m_bIsBaseClass = true;
			def->m_worldspawn = this;
		}
	}

	m_sClassname = t;

	if( !script->GetToken( t ) )
		return false;

	if (t == ":") // helper name.
	{
		if( !script->GetToken( t ) )
			return false;
		m_sDisplayName = t;
	}
	else
	{
		m_sDisplayName = m_sClassname;
		script->UngetToken();
	}

	m_sCategory = "";

	if( !script->GetToken( t ) )
		return false;

	if (t == "extends")
	{
		if( !script->GetToken( t ) )
			return false;

		while (t != "{" && t != "{}")
		{
			CEntDef *extends = def->EntDefForName(t);
			if (!extends) return false;
			if (!Inherit(extends)) return false;
			if( !script->GetToken( t ) )
				return false;
		}
		script->UngetToken();
	}
	else
	{
		script->UngetToken();
	}

	if( !script->GetToken( t ) )
		return false;

	// parse keys

	if (t == "{")
	{
		for( ;; )
		{
			if( !script->GetToken(t) )
				return false;

			if( t == "}" )
				break;

			if( t == "$key" )
			{
				if( ParseKey( script ) == false )
					return false;
			}
			else
			if( t == "$att" )
			{
				if( ParseAtt( script ) == false )
					return false;
			}
			else
			{
				return false;
			}
		}
	}
	else if (t != "{}")
	{
		return false;
	}

	//
	// setup display name.
	//
	CString temp;

	if( m_sCategory != "" )
		temp.Format( "%s\n%s", m_sCategory, m_sDisplayName );
	else
		temp = m_sDisplayName;

	temp.Replace( ':', '\n' );
	m_sTreeDisplayName = temp;

	//Sys_printf( "parsed '%s'\n", m_sClassname );

	return true;
}

bool CEntDef::ParseKey( C_Tokenizer* script )
{
	CString t;

	if( !script->GetToken( t ) )
		goto ohcrap;
	
	CObjProp* key = FindKey(t);
	bool overideKey = key != 0;
	if (!key) key = new CObjProp();

	key->SetName( t );

	if (!script->GetToken(t))
		goto ohcrap;

	if (t == ":")
	{
		if (!script->GetToken(t))
			goto ohcrap;
		key->SetDisplayName(t);
	}
	else
	{
		if (key->GetDisplayName()[0] == 0)
		{
			key->SetDisplayName(key->GetName());
		}
		script->UngetToken();
	}

	script->Skip();
	if( !script->GetToken( t ) )
		goto ohcrap;

	key->SetString( t );

	script->Skip();

	if( !script->GetToken( t ) )
		goto ohcrap;

	if( t == "string" )
		key->SetType( CObjProp::string );
	else
	if( t == "integer" )
		key->SetType( CObjProp::integer );
	else
	if( t == "color" )
		key->SetType( CObjProp::color );
	else
	if( t == "facing" )
		key->SetType( CObjProp::facing );
	else
	if( t == "float" )
		key->SetType( CObjProp::floatnum );
	else
	if( t == "vector" )
		key->SetType( CObjProp::vector );
	else
	if( t == "script" )
		key->SetType( CObjProp::script );
	else
		goto ohcrap;

	if (!overideKey)
	{
		m_keys.AddItem( key );
	}

	if( !script->GetToken( t ) )
		return false;

	bool parseChoices = false;

	if( t == "flags" || t == "choices" )
	{
		if( t == "flags" )
			key->SetSubType( FALSE );
		else
			key->SetSubType( TRUE );

		parseChoices = true;
	}
	else
	{
		script->UngetToken();
	}

	if( !script->GetToken( t ) )
		return false;

	if (t == "for")
	{
		if( !script->GetToken( t ) )
			return false;
		if (t == "target_dst")
		{
			m_sTargetDst = key->GetName();
			m_attflags |= Att_TargetDst;
		}
		else if (t == "target_src")
		{
			m_sTargetSrc = key->GetName();
			m_attflags |= Att_TargetSrc;
		}
	}
	else
	{
		script->UngetToken();
	}

	if (parseChoices)
	{
		//
		// parse key list
		//
		script->Skip(); // {

		for(;;)
		{
			if( !script->GetToken( t ) )
				goto ohcrap;

			if( t == "}" )
				break;

			CObjProp* sub = new CObjProp();
						
			sub->SetName(t);
			sub->SetDisplayName(t);
			script->Skip(); // =

			if( !script->GetToken( t ) )
				goto ohcrap;

			sub->SetString( t );

			key->AddChoice( sub );
		}
	}

	return true;

ohcrap:

	delete key;
	return false;
}

bool CEntDef::ModelFromKey()
{
	return m_bModelFromKey;
}

bool CEntDef::AnimFromKey()
{
	return m_bAnimFromKey;
}

bool CEntDef::ParseAtt( C_Tokenizer* script )
{
	CString type;
	CString t;

	if( !script->GetToken( type ) )
		return false;

	if( type == "category" )
	{
		if( !script->GetToken( t ) )
			return false;

		m_sCategory = t;
		m_attflags |= Att_Category;
	}
	else
	if( type == "box" )
	{
		if( !script->GetToken( t ) )
			return false;

		if (t == "size")
		{
			if( !script->GetToken( t ) )
				return false;

			sscanf( t, "%f %f %f %f %f %f", &m_boxmins.x, &m_boxmins.y, &m_boxmins.z, 
											&m_boxmaxs.x, &m_boxmaxs.y, &m_boxmaxs.z );

			m_attflags |= Att_BoxSize;
		}
		else
		{
			script->UngetToken();
		}
		
		if( !script->GetToken( t ) )
			return false;

		if (t == "color" || t == "colori")
		{
			bool normalize = (t == "colori");

			if( !script->GetToken( t ) )
				return false;

			sscanf( t, "%f %f %f", &m_boxcolor.x, &m_boxcolor.y, &m_boxcolor.z );
			if (normalize)
			{
				m_boxcolor /= 255.0f;
			}

			m_attflags |= Att_BoxColor;
		}
		else
		{
			script->UngetToken();
		}
	}
	else
	if( type == "owner" )
	{
		if( !script->GetToken( t ) )
			return false;

		if( t == "true" )
			m_owner = true;

		m_attflags |= Att_Owner;
	}
	else
	if( type == "modeltype" )
	{
		if( !script->GetToken( t ) )
			return false;

		if( t == "skel" )
		{
			m_style = EDDS_SKEL;

			if( !script->GetToken( m_sModel ) )
				return false;
			
			if( !script->GetToken( t ) )
				return false;

			if( t == "from_key" )
			{
				script->Skip();
				if( !script->GetToken( t ) )
					return false;

				m_bModelFromKey = (t=="true")?true:false;
			}
			else
				script->UngetToken();

			script->Skip();
			if( !script->GetToken( m_sAnim ) )
				return false;
			
			if( !script->GetToken( t ) )
				return false;

			if( t == "from_key" )
			{
				script->Skip();
				if( !script->GetToken( t ) )
					return false;

				m_bAnimFromKey = (t=="true")?true:false;
			}
			else
				script->UngetToken();

			script->Skip();
			if( !script->GetToken( t ) )
				return false;

			sscanf( t, "%f %f %f", &m_skelofs.x, &m_skelofs.y, &m_skelofs.z );
	
			script->Skip(2);
			if( !script->GetToken( m_sRotation ) )
				return false;

			script->Skip(2);
			if( !script->GetToken( t ) )
				return false;

			m_skel_fixed_offset = ( t == "true" ) ? true : false;
		}
		/*else
		if( t == "jmodel" )
		{
			m_style = EDDS_JMODEL;

			if( script->GetToken( m_sModel ) )
				return false;
		}*/
		else
		{
			return false;
		}

		m_attflags |= Att_ModelType;

	}
	else
	if( type == "icon" )
	{
		if( m_style == EDDS_BOX ) m_style = EDDS_ICON;

		if( !script->GetToken( m_sIcon ) )
			return false;

		script->Skip();
		if( !script->GetToken( t ) )
			return false;

		sscanf(t, "%f %f", &m_iconsize.x, &m_iconsize.y );

		m_attflags |= Att_Icon;
	}
	else if (type == "target_src")
	{
		if (!script->GetToken(m_sTargetSrc)) return false;
		m_attflags |= Att_TargetSrc;
	}
	else if (type == "target_dst")
	{
		if (!script->GetToken(m_sTargetDst)) return false;
		m_attflags |= Att_TargetDst;
	}
	else if (type == "repaint")
	{
		if (!script->GetToken(m_sRepaint)) return false;
		m_attflags |= Att_Repaint;
	}
	else
	{
		return false;
	}

	return true;
}

const char* CEntDef::GetRotationKey()
{
	return m_sRotation;
}

const char *CEntDef::GetTargetSrcKey()
{
	return m_sTargetSrc;
}

const char *CEntDef::GetTargetDstKey()
{
	return m_sTargetDst;
}

const char *CEntDef::GetRepaint()
{
	return m_sRepaint;
}

IMPLEMENT_TREAD_NEW(CGameDef);

CGameDef *CGameDef::LoadFromFile(const char *filename)
{
	void* data;
	LongWord len;
	C_Tokenizer script;

	Sys_printf("Parsing entity definitions from %s...\n", filename);

	data = FS_LoadFile( filename, &len );
	if( !data )
	{
		OS_ExitMsg( "Unable to load %s", filename );
		return 0;
	}

	script.InitParsing( (const char*)data, (int)len );
	
	OS_DeallocAPointer( data );

	int num = 0;

	CString t;
	
	if (!script.GetToken(t))
	{
		OS_ExitMsg( "Entity Script Parsing Error: file %s line %d", filename, script.GetLine() );
		return 0;
	}

	CGameDef *gameDef = new CGameDef();
	gameDef->m_name = t;

	if (!script.GetToken(t))
	{
		OS_ExitMsg( "Entity Script Parsing Error: file %s line %d", filename, script.GetLine() );
		return 0;
	}

	gameDef->m_plugin = t;

	for( ;; )
	{
		if( !script.GetToken( t ) )
			break;

		script.UngetToken();
		CEntDef* def = new CEntDef();

		if( def->Parse( &script, gameDef ) == false )
		{
			OS_ExitMsg( "Entity Script Parsing Error: file %s line %d", filename, script.GetLine() );
			return 0;
		}

		gameDef->m_defs.AddItem( def );
		num++;
	}

	Sys_printf("parsed %d definition(s)\n", num );
	return gameDef;
}

CEntDef* CGameDef::EntDefForName( const char* szName )
{
	CEntDef* def;

	if( !szName || !szName[0] )
		return 0;

	for( def = m_defs.ResetPos(); def; def = m_defs.GetNextItem() )
	{
		if( !stricmp( def->GetClassName(), szName ) )
			return def;
	}

	return 0;
}

CLinkedList<CEntDef>* CGameDef::EntDefList()
{
	return &m_defs;
}

