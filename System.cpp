///////////////////////////////////////////////////////////////////////////////
// System.cpp
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

#include "StdAfx.h"
#include "Resource.h"
#include "System.h"
#include "os.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "TreadDoc.h"
#include "Shaders.h"
#include "MapView.h"
#include "ConsoleDialog.h"
#include "mapfile.h"
#include "ents.h"
#include "files.h"
#include "texcache.h"
#include "objpropview.h"
#include "childfrm.h"
#include "splinetrack.h"
#include <vector>
#include <hash_map>

IMPLEMENT_TREAD_NEW(CShader);

///////////////////////////////////////////////////////////////////////////////
// Change this number to increase/decrease the number of undo levels.        //
// The system will automatically purge the undo history of a map when loaded //
// if it's undo chain exceeds this number.                                   //
///////////////////////////////////////////////////////////////////////////////
#define MAX_UNDOREDO_LEVEL			(64)

OS_FNEXP const vec3 sysAxisX = vec3(1.0f, 0.0f, 0.0f);
OS_FNEXP const vec3 sysAxisY = vec3(0.0f, 1.0f, 0.0f);
OS_FNEXP const vec3 sysAxisZ = vec3(0.0f, 0.0f, 1.0f);

static LPCTSTR c_cursors[TC_NUMCURSORS] = 
{
	IDC_ARROW, IDC_IBEAM, IDC_WAIT, IDC_CROSS, 
	IDC_UPARROW, IDC_SIZEALL, IDC_SIZENWSE, 
	IDC_SIZENESW, IDC_SIZEWE, IDC_SIZENS
};

//////////////////////////////////////////////////////////////////////
// OBJECTFILTER														//
//////////////////////////////////////////////////////////////////////
IMPLEMENT_TREAD_NEW(CObjectFilter);

CObjectFilter::CObjectFilter()
{
	SetBrushFilters(false);
}

CObjectFilter::~CObjectFilter()
{
}

void CObjectFilter::SetBrushFilters(  bool on )
{
	BrushFilter.nodraw = on;
	BrushFilter.solid = on;
	BrushFilter.window = on;
	BrushFilter.areaportal = on;
	BrushFilter.detail = on;
	BrushFilter.noshadow = on;
	BrushFilter.always_shadow = on;
	BrushFilter.monster_clip = on;
	BrushFilter.player_clip = on;
	BrushFilter.corona_block = on;
	BrushFilter.camera_clip = on;
	BrushFilter.skybox = on;
	BrushFilter.skyportal = on;
	BrushFilter.water = on;
}

//////////////////////////////////////////////////////////////////////
// OBJECTGROUP														//
//////////////////////////////////////////////////////////////////////

IMPLEMENT_TREAD_NEW(CObjectGroup);

CObjectGroup::CObjectGroup() : CLLObject()
{
	m_nUIDs = 0;
	m_nNumUIDs = 0;
	m_nUID = 0;
	m_sName = "";
	m_bVisible = true;
	m_hTreeItem = 0;
	m_bMarked = false;
}

CObjectGroup::~CObjectGroup()
{
	if( m_nUIDs )
		delete[] m_nUIDs;
}

bool CObjectGroup::IsMarked()
{
	return m_bMarked;
}

void CObjectGroup::SetMarked( bool marked )
{
	m_bMarked = marked;
}

void CObjectGroup::OrphanObjects( CTreadDoc* pDoc )
{
	int i;
	
	if( !m_nUIDs )
		return;

	CMapObject* obj;
	CMapObject** objs = pDoc->GetObjectsFromUIDs( m_nUIDs, m_nNumUIDs, true, true );

	for(i = 0; i < m_nNumUIDs; i++)
	{
		obj = objs[i];
		if( obj )
		{
			obj->SetGroupUID( -1 );
			pDoc->Prop_RemoveObject( obj );
			pDoc->Prop_AddObject( obj );
		}
	}

	delete[] objs;
	delete[] m_nUIDs;
	m_nUIDs = 0;
	m_nNumUIDs = 0;
}

bool CObjectGroup::IsNullGroup( CTreadDoc* pDoc )
{
	int i;
	int nullcount;

	if( m_nNumUIDs < 1 )
		return true;

	CMapObject** objs = pDoc->GetObjectsFromUIDs( m_nUIDs, m_nNumUIDs, true, true );

	nullcount = 0;

	for(i = 0; i < m_nNumUIDs; i++)
	{
		if( objs[i] == 0 )
		{
			m_nUIDs[i] = -1;
			nullcount++;
		}
	}

	delete[] objs;

	return nullcount == m_nNumUIDs;
}

void CObjectGroup::RemoveObject( int uid )
{
	int i;

	for( i = 0; i < m_nNumUIDs; i++)
	{
		if( m_nUIDs[i] == uid )
			m_nUIDs[i] = -1;
	}
}

const char* CObjectGroup::GetName()
{
	return m_sName;
}

void CObjectGroup::SetName( const char* name )
{
	m_sName = name;
}

void CObjectGroup::MakeList( CLinkedList<CMapObject>* pList )
{
	if( m_nUIDs )
		delete[] m_nUIDs;

	m_nUIDs = 0;
	m_nNumUIDs = pList->GetCount();

	if( m_nNumUIDs > 0 )
	{
		int i;
		CMapObject* obj;
		m_nUIDs = new int[m_nNumUIDs];

		for( i = 0, obj = pList->ResetPos(); obj; i++, obj = pList->GetNextItem() )
		{
			m_nUIDs[i] = obj->GetUID();
		}
	}
}

bool CObjectGroup::IsVisible()
{
	return m_bVisible;
}

int CObjectGroup::GetUID()
{
	return m_nUID;
}

void CObjectGroup::SetUID( int uid )
{
	m_nUID = uid;
}

bool CObjectGroup::IsSelected( CTreadDoc* pDoc )
{
	int i;

	if( m_nNumUIDs < 1)
		return false;

	CMapObject* obj;
	CMapObject** objs = pDoc->GetObjectsForGroup( GetUID(), m_nNumUIDs, true, false );

	for(i = 0; i < m_nNumUIDs; i++)
	{
		obj = objs[i];
		if( obj )
		{
			if( !obj->IsSelected() )
			{
				delete[] objs;
				return false;
			}
		}
	}

	delete[] objs;
	return true;
}

void CObjectGroup::SelectObjects( CTreadDoc* pDoc )
{
	if( m_nNumUIDs > 0 )
	{
		int i;
		CMapObject* obj;
		CMapObject** objs = pDoc->GetObjectsForGroup( GetUID(), m_nNumUIDs, true, false );

		for(i = 0; i < m_nNumUIDs; i++)
		{
			obj = objs[i];
			if( obj && !obj->IsSelected() )
			{
				obj->SetEntGroupCall( true );
				obj->Select( pDoc );
			}
		}

		for(i = 0; i < m_nNumUIDs; i++)
		{
			obj = objs[i];
			if( obj )
			{
				obj->SetEntGroupCall( false );
			}
		}

		delete[] objs;
	}
}

void CObjectGroup::DeselectObjects( CTreadDoc* pDoc )
{
	if( m_nNumUIDs > 0 )
	{
		int i;

		CMapObject* obj;
		CMapObject** objs = pDoc->GetObjectsForGroup( GetUID(), m_nNumUIDs, false, true );

		for(i = 0; i < m_nNumUIDs; i++)
		{
			obj = objs[i];
			if( obj && obj->IsSelected() )
			{
				obj->SetEntGroupCall( true );
				obj->Deselect( pDoc );
			}
		}

		for(i = 0; i < m_nNumUIDs; i++)
		{
			obj = objs[i];
			if( obj )
			{
				obj->SetEntGroupCall( false );
			}
		}

		delete[] objs;
	}
}

void CObjectGroup::SetVisible( CTreadDoc* pDoc, bool vis )
{
	m_bVisible = vis;

	if( m_nNumUIDs > 0 )
	{
		int i;
		bool madeundo = false;

		if( !vis )
			pDoc->ClearAllTrackPicks();
		
		CMapObject* obj;
		CMapObject** objs = pDoc->GetObjectsForGroup( GetUID(), m_nNumUIDs, true, true );

		for(i = 0; i < m_nNumUIDs; i++)
		{
			obj = objs[i];
			if( obj )
			{
				if( obj->IsSelected() && !vis )
				{
					if( madeundo == false )
					{
						pDoc->MakeUndoDeselectAction();
						madeundo = true;
					}

					obj->Deselect( pDoc );
				}

				obj->SetVisible( pDoc, vis );
				pDoc->Prop_UpdateObjectState( obj );
			}
		}

		delete[] objs;
	}
}

void CObjectGroup::WriteToFile( CFile* file, int nVersion )
{
	MAP_WriteString( file, m_sName );
	MAP_WriteInt( file, m_nUID );
	MAP_WriteInt( file, m_bVisible );
	MAP_WriteInt( file, m_nNumUIDs );

	int i;
	for(i = 0; i < m_nNumUIDs; i++)
		MAP_WriteInt( file, m_nUIDs[i] );
}

void CObjectGroup::ReadFromFile( CFile* file, int nVersion )
{
	m_sName = MAP_ReadString( file );
	m_nUID = MAP_ReadInt( file );
	m_bVisible = MAP_ReadInt( file );
	m_nNumUIDs = MAP_ReadInt( file );

	if( m_nNumUIDs > 0 )
	{
		m_nUIDs = new int[m_nNumUIDs];
		int i;
		for(i = 0; i < m_nNumUIDs; i++)
			m_nUIDs[i] = MAP_ReadInt( file );
	}
}

void CObjectGroup::SetTreeItem( HTREEITEM hItem )
{
	m_hTreeItem = hItem;
}

HTREEITEM CObjectGroup::GetTreeItem()
{
	return m_hTreeItem;
}

//////////////////////////////////////////////////////////////////////
// UNDOREDOACTION													//
//////////////////////////////////////////////////////////////////////

IMPLEMENT_TREAD_NEW(CUndoRedoAction);

CUndoRedoAction::CUndoRedoAction() : CLLObject()
{
	m_sTitle = "Unamed Action";
}

CUndoRedoAction::~CUndoRedoAction()
{
}

void CUndoRedoAction::ReconnectEntDefs(CTreadDoc *doc)
{
}

void CUndoRedoAction::SetTitle( const char* title )
{
	m_sTitle = title;
}

const char* CUndoRedoAction::GetTitle()
{
	return m_sTitle;
}

bool CUndoRedoAction::WriteToFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	MAP_WriteString( file, m_sTitle );
	return true;
}

bool CUndoRedoAction::ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	m_sTitle = MAP_ReadString( file );
	return true;
}

CUndoRedoAction* CUndoRedoAction::UndoRedoFromClass( int classbits )
{
	switch( classbits )
	{
	case UNDOREDO_CLASS_GENERIC:

		return new CGenericUndoRedoAction();

	break;
	case UNDOREDO_CLASS_DESELECT:

		return new CDeselectUndoRedoAction();

	break;
	case UNDOREDO_CLASS_CREATE:

		return new CCreateUndoRedoAction();

	break;
	case UNDOREDO_CLASS_DELETE:

		return new CDeleteUndoRedoAction();

	break;
	case UNDOREDO_CLASS_PASTE:

		return new CPasteUndoRedoAction();

	break;
	case UNDOREDO_CLASS_LINK:

		return new CLinkUndoRedoAction();

	break;
	case UNDOREDO_CLASS_UNLINK:

		return new CUnlinkUndoRedoAction();

	break;
	}

	return 0;
}

CUndoRedoAction* CUndoRedoAction::ReadUndoRedoFromFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction* obj;
	int classbits;

	classbits = MAP_ReadInt(file);
	obj = doc->GamePlugin()->UndoRedoActionForClass(classbits);
	if (!obj) obj = CUndoRedoAction::UndoRedoFromClass(classbits);
	if(obj)
	{
		obj->ReadFromFile( file, doc, nVersion );
		return obj;
	}

	return 0;
}

bool CUndoRedoAction::WriteUndoRedoToFile( CFile* file, CTreadDoc* doc, int nVersion, CUndoRedoAction* obj )
{
	MAP_WriteInt( file, obj->GetClass() );

	return obj->WriteToFile( file, doc, nVersion );
}

//////////////////////////////////////////////////////////////////////
// PASTEUNDOREDOACTION												//
//////////////////////////////////////////////////////////////////////

CPasteUndoRedoAction::CPasteUndoRedoAction() : CUndoRedoAction()
{
	m_nUndoUIDs = 0;
	m_nNumUndoUIDs = 0;
	m_nObjUIDs = 0;
	m_nNumObjUIDs = 0;
}

CPasteUndoRedoAction::~CPasteUndoRedoAction()
{
	if( m_nUndoUIDs )
		delete[] m_nUndoUIDs;
	if( m_nObjUIDs )
		delete[] m_nObjUIDs;
}

void CPasteUndoRedoAction::ReconnectEntDefs(CTreadDoc *doc)
{
	m_RedoList.WalkList( CTreadDoc::ReconnectEntity, doc );
}

int CPasteUndoRedoAction::GetClass()
{
	return UNDOREDO_CLASS_PASTE;
}

bool CPasteUndoRedoAction::WriteToFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::WriteToFile( file, doc, nVersion );

	int i;

	MAP_WriteInt( file, m_nNumUndoUIDs );
	for(i = 0; i < m_nNumUndoUIDs; i++)
		MAP_WriteInt( file, m_nUndoUIDs[i] );

	MAP_WriteInt( file, m_nNumObjUIDs );
	for(i = 0; i < m_nNumObjUIDs; i++)
		MAP_WriteInt( file, m_nObjUIDs[i] );

	MAP_WriteObjectList( file, doc, nVersion, &m_RedoList );
	return true;
}

bool CPasteUndoRedoAction::ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::ReadFromFile( file, doc, nVersion );

	int i;

	m_nNumUndoUIDs = MAP_ReadInt( file );
	m_nUndoUIDs = 0;

	if( m_nNumUndoUIDs > 0 )
	{
		m_nUndoUIDs = new int[m_nNumUndoUIDs];
		for(i = 0; i < m_nNumUndoUIDs; i++)
			m_nUndoUIDs[i] = MAP_ReadInt( file );
	}
	
	m_nNumObjUIDs = MAP_ReadInt( file );
	m_nObjUIDs = 0;

	if( m_nNumObjUIDs > 0 )
	{
		m_nObjUIDs = new int[m_nNumObjUIDs];
		for(i = 0; i < m_nNumObjUIDs; i++)
			m_nObjUIDs[i] = MAP_ReadInt( file );
	}

	MAP_ReadObjectList( file, doc, nVersion, &m_RedoList );
	return true;
}

void CPasteUndoRedoAction::Undo( CTreadDoc* pDoc )
{
	//
	// undo the paste.
	//
	pDoc->ClearSelection();
	CUndoRedoManager::SelectObjectsByUIDs( pDoc, m_nObjUIDs, m_nNumObjUIDs );
	pDoc->DeleteSelection();

	CUndoRedoManager::SelectObjectsByUIDs( pDoc, m_nUndoUIDs, m_nNumUndoUIDs );

	pDoc->UpdateSelectionInterface();
	pDoc->Prop_UpdateSelection();
}

void CPasteUndoRedoAction::Redo( CTreadDoc* pDoc )
{
	//
	// copy the object lists.
	//
	if( m_nUndoUIDs )
		delete[] m_nUndoUIDs;

	m_nUndoUIDs = CUndoRedoManager::GetObjectUIDs( pDoc->GetSelectedObjectList(), &m_nNumUndoUIDs );

	pDoc->ClearSelection();

	m_nNumObjUIDs = 0;

	CMapObject* obj, *copy;
	CLinkedList<CMapObject> temp_list;

	for( obj = m_RedoList.ResetPos(); obj; obj = m_RedoList.GetNextItem() )
	{
		copy = obj->Clone();
		copy->CopyState( obj, pDoc );

		pDoc->AddObjectToMap( copy );
		pDoc->AddObjectToSelection( copy );
		
		copy->SetName( obj->GetName() );

		pDoc->Prop_AddObject( copy );

		m_nObjUIDs[m_nNumObjUIDs++] = copy->GetUID();
	}

	pDoc->UpdateSelectionInterface();
	pDoc->Prop_UpdateSelection();
}

void CPasteUndoRedoAction::InitUndo( CLinkedList<CMapObject>* pPaste, CLinkedList<CMapObject>* pList, CTreadDoc* pDoc )
{
	//
	// save the selection.
	//
	m_nUndoUIDs = CUndoRedoManager::GetObjectUIDs( pList, &m_nNumUndoUIDs );

	//
	// duplicate the paste list.
	//
	m_nObjUIDs = CUndoRedoManager::GetObjectUIDs( pPaste, &m_nNumObjUIDs );
	CUndoRedoManager::CopyObjectList( pPaste, &m_RedoList, pDoc );
}

//////////////////////////////////////////////////////////////////////
// DELETEUNDOREDOACTION												//
//////////////////////////////////////////////////////////////////////

CDeleteUndoRedoAction::CDeleteUndoRedoAction() : CUndoRedoAction()
{
	m_nUndoUIDs = 0;
	m_nNumUndoUIDs = 0;
	m_nObjs = 0;
	m_nNumObjs = 0;
}

CDeleteUndoRedoAction::~CDeleteUndoRedoAction()
{
	if( m_nUndoUIDs )
		delete[] m_nUndoUIDs;
	if( m_nObjs )
		delete[] m_nObjs;
}

void CDeleteUndoRedoAction::ReconnectEntDefs(CTreadDoc *doc)
{
	m_UndoList.WalkList( CTreadDoc::ReconnectEntity, doc );
}

bool CDeleteUndoRedoAction::WriteToFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::WriteToFile( file, doc, nVersion );

	int i;

	MAP_WriteInt( file, m_nNumUndoUIDs );
	for(i = 0; i < m_nNumUndoUIDs; i++)
		MAP_WriteInt( file, m_nUndoUIDs[i] );

	MAP_WriteInt( file, m_nNumObjs );
	for(i = 0; i < m_nNumObjs; i++)
		MAP_WriteInt( file, m_nObjs[i] );

	MAP_WriteObjectList( file, doc, nVersion, &m_UndoList );
	return true;
}

bool CDeleteUndoRedoAction::ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::ReadFromFile( file, doc, nVersion );

	int i;

	m_nNumUndoUIDs = MAP_ReadInt( file );
	m_nUndoUIDs = 0;

	if( m_nNumUndoUIDs > 0 )
	{
		m_nUndoUIDs = new int[m_nNumUndoUIDs];
		for(i = 0; i < m_nNumUndoUIDs; i++)
			m_nUndoUIDs[i] = MAP_ReadInt( file );
	}
	
	m_nNumObjs = MAP_ReadInt( file );
	m_nObjs = 0;

	if( m_nNumObjs > 0 )
	{
		m_nObjs = new int[m_nNumObjs];
		for(i = 0; i < m_nNumObjs; i++)
			m_nObjs[i] = MAP_ReadInt( file );
	}

	MAP_ReadObjectList( file, doc, nVersion, &m_UndoList );
	return true;
}

int CDeleteUndoRedoAction::GetClass()
{
	return UNDOREDO_CLASS_DELETE;
}

void CDeleteUndoRedoAction::Undo( CTreadDoc* pDoc )
{
	//
	// save the selected object list.
	//
	if( m_nUndoUIDs )
		delete[] m_nUndoUIDs;

	m_nUndoUIDs = CUndoRedoManager::GetObjectUIDs( pDoc->GetSelectedObjectList(), &m_nNumUndoUIDs );

	//
	// undo the deletion.
	//
	pDoc->ClearSelection();
	CUndoRedoManager::DuplicateListIntoDocument( &m_UndoList, pDoc );

	//
	// copy these for redo.
	//
	m_nObjs = CUndoRedoManager::GetObjectUIDs( pDoc->GetSelectedObjectList(), &m_nNumObjs );

	pDoc->UpdateSelectionInterface();
	pDoc->Prop_UpdateSelection();
}

void CDeleteUndoRedoAction::Redo( CTreadDoc* pDoc )
{
	//
	// redo the deletion.
	//
	pDoc->ClearSelection();
	CUndoRedoManager::SelectObjectsByUIDs( pDoc, m_nObjs, m_nNumObjs );
	pDoc->DeleteSelection();
	CUndoRedoManager::SelectObjectsByUIDs( pDoc, m_nUndoUIDs, m_nNumUndoUIDs );

	delete[] m_nObjs;
	m_nObjs = 0;

	pDoc->UpdateSelectionInterface();
	pDoc->Prop_UpdateSelection();
}

void CDeleteUndoRedoAction::InitUndo( CLinkedList<CMapObject>* pList, CTreadDoc* pDoc )
{
	//
	// duplicate the selected items for undo.
	//
	CUndoRedoManager::CopyObjectList( pList, &m_UndoList, pDoc );
}

//////////////////////////////////////////////////////////////////////
// CREATEUNDOREDOACTION	     										//
//////////////////////////////////////////////////////////////////////

CCreateUndoRedoAction::CCreateUndoRedoAction() : CUndoRedoAction()
{
	m_nUndoUIDs = 0;
	m_nNumUndoUIDs = 0;
	m_pRedoObj = 0;
	m_nCreateUID = -1;
}

CCreateUndoRedoAction::~CCreateUndoRedoAction()
{
	if( m_nUndoUIDs )
		delete[] m_nUndoUIDs;
	if( m_pRedoObj )
		delete m_pRedoObj;
}

void CCreateUndoRedoAction::ReconnectEntDefs(CTreadDoc *doc)
{
	if( m_pRedoObj )
	{
		CTreadDoc::ReconnectEntity( m_pRedoObj, doc, 0 );
	}
}

bool CCreateUndoRedoAction::WriteToFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::WriteToFile( file, doc, nVersion );

	int i;

	MAP_WriteInt( file, m_nNumUndoUIDs );
	for(i = 0; i < m_nNumUndoUIDs; i++)
		MAP_WriteInt( file, m_nUndoUIDs[i] );

	MAP_WriteInt( file, m_nCreateUID );

	if( m_pRedoObj )
	{
		MAP_WriteInt( file, 1 );
		MAP_WriteObject( file, doc, nVersion, m_pRedoObj );
	}
	else
	{
		MAP_WriteInt( file, 0 );
	}
	
	return true;
}

bool CCreateUndoRedoAction::ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::ReadFromFile( file, doc, nVersion );

	int i;

	m_nNumUndoUIDs = MAP_ReadInt( file );
	m_nUndoUIDs = 0;
	
	if( m_nNumUndoUIDs > 0 )
	{
		m_nUndoUIDs = new int[m_nNumUndoUIDs];
		for(i = 0; i < m_nNumUndoUIDs; i++)
			m_nUndoUIDs[i] = MAP_ReadInt( file );
	}

	m_nCreateUID = MAP_ReadInt( file );

	int exists = MAP_ReadInt( file );

	if( exists )
	{
		m_pRedoObj = MAP_ReadObject( file, doc, nVersion );
	}
	else
	{
		m_pRedoObj = 0;
	}

	return true;
}

int CCreateUndoRedoAction::GetClass()
{
	return UNDOREDO_CLASS_CREATE;
}

void CCreateUndoRedoAction::Undo( CTreadDoc* pDoc )
{
	//
	// find the createUID object and remove it. and select anything that was selected before.
	//
	pDoc->ClearSelection();
	m_pRedoObj = pDoc->ObjectForUID( m_nCreateUID );
	if( m_pRedoObj )
	{
		pDoc->AddObjectToMap( m_pRedoObj );
		pDoc->DetachObject( m_pRedoObj );
		pDoc->Prop_RemoveObject( m_pRedoObj );
	}

	CUndoRedoManager::SelectObjectsByUIDs( pDoc, m_nUndoUIDs, m_nNumUndoUIDs );

	pDoc->UpdateSelectionInterface();
	pDoc->Prop_UpdateSelection();
}

void CCreateUndoRedoAction::Redo( CTreadDoc* pDoc )
{
	//
	// store the current selection.
	//
	if( m_nUndoUIDs )
		delete[] m_nUndoUIDs;

	m_nUndoUIDs = CUndoRedoManager::GetObjectUIDs( pDoc->GetSelectedObjectList(), &m_nNumUndoUIDs );

	pDoc->ClearSelection();

	//
	// place the object back in the map.
	//
	if( m_pRedoObj )
	{
		pDoc->AddObjectToMap( m_pRedoObj );
		pDoc->AddObjectToSelection( m_pRedoObj );
		pDoc->Prop_AddObject( m_pRedoObj );
		m_pRedoObj = 0;
	}

	pDoc->UpdateSelectionInterface();
	pDoc->Prop_UpdateSelection();
}

void CCreateUndoRedoAction::InitUndo( CMapObject* pCreateObj, CLinkedList<CMapObject>* pList, CTreadDoc* pDoc )
{
	//
	// get the selected object list.
	//
	m_nUndoUIDs = CUndoRedoManager::GetObjectUIDs( pList, &m_nNumUndoUIDs );

	m_nCreateUID = pCreateObj->GetUID();
}

//////////////////////////////////////////////////////////////////////
// DESELECTUNDOREDOACTION											//
//////////////////////////////////////////////////////////////////////

CDeselectUndoRedoAction::CDeselectUndoRedoAction() : CUndoRedoAction()
{
	m_nUndoUIDs = 0;
	m_nRedoUIDs = 0;
	m_nNumUndoUIDs = 0;
	m_nNumRedoUIDs = 0;
}

CDeselectUndoRedoAction::~CDeselectUndoRedoAction()
{
	if( m_nUndoUIDs )
		delete[] m_nUndoUIDs;
	if( m_nRedoUIDs )
		delete[] m_nRedoUIDs;
}

bool CDeselectUndoRedoAction::WriteToFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::WriteToFile( file, doc, nVersion );

	int i;

	MAP_WriteInt( file, m_nNumUndoUIDs );
	for(i = 0; i < m_nNumUndoUIDs; i++)
		MAP_WriteInt( file, m_nUndoUIDs[i] );

	MAP_WriteInt( file, m_nNumRedoUIDs );
	for(i = 0; i < m_nNumRedoUIDs; i++)
		MAP_WriteInt( file, m_nRedoUIDs[i] );
	
	return true;
}

bool CDeselectUndoRedoAction::ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::ReadFromFile( file, doc, nVersion );

	int i;

	m_nNumUndoUIDs = MAP_ReadInt( file );
	m_nUndoUIDs = 0;

	if( m_nNumUndoUIDs > 0 )
	{
		m_nUndoUIDs = new int[m_nNumUndoUIDs];
		for(i = 0; i < m_nNumUndoUIDs; i++)
			m_nUndoUIDs[i] = MAP_ReadInt( file );
	}

	m_nNumRedoUIDs = MAP_ReadInt( file );
	m_nRedoUIDs = 0;

	if( m_nNumRedoUIDs > 0 )
	{
		m_nRedoUIDs = new int[m_nNumRedoUIDs];
		for(i = 0; i < m_nNumRedoUIDs; i++)
			m_nRedoUIDs[i] = MAP_ReadInt( file );
	}

	return true;
}


int CDeselectUndoRedoAction::GetClass()
{
	return UNDOREDO_CLASS_DESELECT;
}

void CDeselectUndoRedoAction::Undo( CTreadDoc* pDoc )
{
	//
	// store the current selection for redo.
	//
	if( m_nRedoUIDs )
		delete[] m_nRedoUIDs;

	m_nRedoUIDs = CUndoRedoManager::GetObjectUIDs( pDoc->GetSelectedObjectList(), &m_nNumRedoUIDs );

	pDoc->ClearSelection();
	CUndoRedoManager::SelectObjectsByUIDs( pDoc, m_nUndoUIDs, m_nNumUndoUIDs );
	
	pDoc->UpdateSelectionInterface();
	pDoc->Prop_UpdateSelection();
}

void CDeselectUndoRedoAction::Redo( CTreadDoc* pDoc )
{
	pDoc->ClearSelection();

	CUndoRedoManager::SelectObjectsByUIDs( pDoc, m_nRedoUIDs, m_nNumRedoUIDs );

	pDoc->UpdateSelectionInterface();
	pDoc->Prop_UpdateSelection();
}

void CDeselectUndoRedoAction::InitUndo( CLinkedList<CMapObject>* pList, CTreadDoc* pDoc )
{
	if( pList->IsEmpty() )
		return;

	m_nUndoUIDs = CUndoRedoManager::GetObjectUIDs( pList, &m_nNumUndoUIDs );
}

//////////////////////////////////////////////////////////////////////
// GENERICUNDOREDOACTION											//
//////////////////////////////////////////////////////////////////////
CGenericUndoRedoAction::CGenericUndoRedoAction() : CUndoRedoAction()
{
	m_objs = 0;
	m_numobjs = 0;
}

CGenericUndoRedoAction::~CGenericUndoRedoAction()
{
	if( m_objs )
		delete[] m_objs;
}

void CGenericUndoRedoAction::ReconnectEntDefs(CTreadDoc *doc)
{
	m_UndoList.WalkList( CTreadDoc::ReconnectEntity, doc );
	m_RedoList.WalkList( CTreadDoc::ReconnectEntity, doc );
}

bool CGenericUndoRedoAction::WriteToFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::WriteToFile( file, doc, nVersion );

	int i;

	MAP_WriteInt( file, m_numobjs );
	for(i = 0; i < m_numobjs; i++)
		MAP_WriteInt( file, m_objs[i] );

	MAP_WriteObjectList( file, doc, nVersion, &m_UndoList );
	MAP_WriteObjectList( file, doc, nVersion, &m_RedoList );

	return true;
}

bool CGenericUndoRedoAction::ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::ReadFromFile( file, doc, nVersion );

	int i;

	m_numobjs = MAP_ReadInt( file );
	m_objs = 0;

	if( m_numobjs > 0 )
	{
		m_objs = new int[m_numobjs];
		for(i = 0; i < m_numobjs; i++)
			m_objs[i] = MAP_ReadInt( file );
	}

	MAP_ReadObjectList( file, doc, nVersion, &m_UndoList );
	MAP_ReadObjectList( file, doc, nVersion, &m_RedoList );

	return true;
}

int CGenericUndoRedoAction::GetClass()
{
	return UNDOREDO_CLASS_GENERIC;
}

void CGenericUndoRedoAction::InitUndo( CLinkedList<CMapObject>* pList, CTreadDoc* pDoc )
{
	CUndoRedoManager::CopyObjectList( pList, &m_UndoList, pDoc );
	m_objs = CUndoRedoManager::GetObjectUIDs( pList, &m_numobjs );
}

void CGenericUndoRedoAction::MakeRedoList( CTreadDoc* pDoc )
{
	CUndoRedoManager::CopyObjectList( pDoc->GetSelectedObjectList(), &m_RedoList, pDoc );
}

void CGenericUndoRedoAction::Undo( CTreadDoc* pDoc )
{
	//
	// take the selected objects and copy them for our redo state.
	//
	pDoc->ClearSelection();
	CUndoRedoManager::SelectObjectsByUIDs( pDoc, m_objs, m_numobjs );
	MakeRedoList( pDoc );
	pDoc->DeleteSelection();

	//
	// they should always be the same.
	//
	/*delete[] m_objs;
	m_objs = 0;*/

	//
	// the selection list is now nil. fill it with our undo list.
	//
	CUndoRedoManager::DuplicateListIntoDocument( &m_UndoList, pDoc );

	pDoc->UpdateSelectionInterface();
	pDoc->Prop_UpdateSelection();
}

void CGenericUndoRedoAction::Redo( CTreadDoc* pDoc )
{
	pDoc->ClearSelection();
	CUndoRedoManager::SelectObjectsByUIDs( pDoc, m_objs, m_numobjs );
	pDoc->DeleteSelection();

	//
	// move the redo list into the selection list.
	//
	CUndoRedoManager::DuplicateListIntoDocument( &m_RedoList, pDoc );

	pDoc->UpdateSelectionInterface();
	pDoc->Prop_UpdateSelection();
}

//////////////////////////////////////////////////////////////////////
// CLINKUNDOREDOACTION												//
//////////////////////////////////////////////////////////////////////
CLinkUndoRedoAction::CLinkUndoRedoAction() : CUndoRedoAction()
{
	m_ent = 0;
	m_uid = -1;
}

CLinkUndoRedoAction::~CLinkUndoRedoAction()
{
	if( m_ent )
		delete m_ent;
}

void CLinkUndoRedoAction::ReconnectEntDefs(CTreadDoc *doc)
{
	if( m_ent )
	{
		CTreadDoc::ReconnectEntity( m_ent, doc, 0 );
	}
}

bool CLinkUndoRedoAction::WriteToFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::WriteToFile( file, doc, nVersion );

	MAP_WriteInt( file, m_uid );
	if( m_ent )
	{
		MAP_WriteInt( file, 1 );
		MAP_WriteObject( file, doc, nVersion, m_ent );
	}
	else
	{
		MAP_WriteInt( file, 0 );
	}

	return true;
}

bool CLinkUndoRedoAction::ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::ReadFromFile( file, doc, nVersion );

	m_uid = MAP_ReadInt( file );
	int exists = MAP_ReadInt(file);

	m_ent = 0;

	if( exists )
	{
		CMapObject* obj = MAP_ReadObject( file, doc, nVersion );
		if( obj )
			m_ent = dynamic_cast<CEntity*>(obj);
	}

	return true;
}

int CLinkUndoRedoAction::GetClass()
{
	return UNDOREDO_CLASS_LINK;
}

void CLinkUndoRedoAction::InitUndo( CEntity* entity )
{
	m_uid = entity->GetUID();
}

void CLinkUndoRedoAction::Undo( CTreadDoc* pDoc )
{
	//
	// get the entity and unlink the brush list,
	// then remove us from the map.
	//
	CMapObject* obj = pDoc->ObjectForUID( m_uid );
	if( obj )
	{
		m_ent = dynamic_cast<CEntity*>(obj);
		if( m_ent )
		{
			m_ent->UnlinkOwnedObjects( pDoc );
			pDoc->DetachObject( m_ent );
			pDoc->Prop_UpdateSelection();
			pDoc->UpdateSelectionInterface();
		}
	}
}

void CLinkUndoRedoAction::Redo( CTreadDoc* pDoc )
{
	if( m_ent )
	{
		//
		// add back into map.
		//
		pDoc->AddObjectToMap( m_ent );
		m_ent->RelinkOwnedObjects( pDoc );
		pDoc->ClearSelection();
		m_ent->SelectOwnedObjects( pDoc );
		pDoc->Prop_UpdateSelection();
		pDoc->UpdateSelectionInterface();

		m_ent = 0;
	}
}

//////////////////////////////////////////////////////////////////////
// CUNLINKUNDOREDOACTION											//
//////////////////////////////////////////////////////////////////////
CUnlinkUndoRedoAction::CUnlinkUndoRedoAction() : CUndoRedoAction()
{
	m_ent = 0;
	m_uid = -1;
}

CUnlinkUndoRedoAction::~CUnlinkUndoRedoAction()
{
	if( m_ent )
		delete m_ent;
}

void CUnlinkUndoRedoAction::ReconnectEntDefs(CTreadDoc *doc)
{
	if( m_ent )
	{
		CTreadDoc::ReconnectEntity( m_ent, doc, 0 );
	}
}

bool CUnlinkUndoRedoAction::WriteToFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::WriteToFile( file, doc, nVersion );

	MAP_WriteInt( file, m_uid );
	if( m_ent )
	{
		MAP_WriteInt( file, 1 );
		MAP_WriteObject( file, doc, nVersion, m_ent );
	}
	else
	{
		MAP_WriteInt( file, 0 );
	}

	return true;
}

bool CUnlinkUndoRedoAction::ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion )
{
	CUndoRedoAction::ReadFromFile( file, doc, nVersion );

	m_uid = MAP_ReadInt( file );
	int exists = MAP_ReadInt(file);

	m_ent = 0;

	if( exists )
	{
		CMapObject* obj = MAP_ReadObject( file, doc, nVersion );
		if( obj )
			m_ent = dynamic_cast<CEntity*>(obj);
	}

	return true;
}

int CUnlinkUndoRedoAction::GetClass()
{
	return UNDOREDO_CLASS_UNLINK;
}

void CUnlinkUndoRedoAction::InitUndo( CEntity* entity )
{
	m_ent = entity;
	m_uid = entity->GetUID();
}

void CUnlinkUndoRedoAction::Undo( CTreadDoc* pDoc )
{
	if( m_ent )
	{
		pDoc->AddObjectToMap( m_ent );
		m_ent->RelinkOwnedObjects( pDoc );
		pDoc->ClearSelection();
		m_ent->SelectOwnedObjects( pDoc );
		pDoc->Prop_UpdateSelection();
		pDoc->UpdateSelectionInterface();

		m_ent = 0;
	}
}

void CUnlinkUndoRedoAction::Redo( CTreadDoc* pDoc )
{
	CMapObject* obj = pDoc->ObjectForUID( m_uid );
	if( obj )
	{
		m_ent = dynamic_cast<CEntity*>(obj);
		if( m_ent )
		{
			m_ent->UnlinkOwnedObjects( pDoc );
			pDoc->DetachObject( m_ent );
			pDoc->Prop_UpdateSelection();
			pDoc->UpdateSelectionInterface();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// UNDOREDOMANAGER													//
//////////////////////////////////////////////////////////////////////
CUndoRedoManager::CUndoRedoManager()
{
	m_pUndo = m_pRedo = 0;
	m_nNum = 0;
}

CUndoRedoManager::~CUndoRedoManager()
{
}

int CUndoRedoManager::GetUndoHistoryCount()
{
	return m_nNum;
}

void CUndoRedoManager::ReconnectEntDefs(CTreadDoc *doc)
{
	CUndoRedoAction* obj;

	for( obj = m_UndoRedo.ResetPos(); obj; obj = m_UndoRedo.GetNextItem() )
	{
		obj->ReconnectEntDefs(doc);
	}
}

bool CUndoRedoManager::WriteUndoHistoryToFile( CFile* file, CTreadDoc* doc, int version )
{
	CUndoRedoAction* obj = 0;

	//
	// write the number of undo/redo items.
	//
	MAP_WriteInt( file, m_UndoRedo.GetCount() );

	//
	// find the undo position.
	//
	if( m_pUndo == 0 )
		MAP_WriteInt( file, -1 );
	else
	{
		int c=0;
		for( obj = m_UndoRedo.ResetPos(); obj; obj = m_UndoRedo.GetNextItem() )
		{
			if( obj == m_pUndo )
				break;
			c++;
		}

		MAP_WriteInt( file, c );
	}

	//
	// find the redo position.
	//
	if( m_pRedo == 0 )
		MAP_WriteInt( file, -1 );
	else
	{
		int c=0;
		for( obj = m_UndoRedo.ResetPos(); obj; obj = m_UndoRedo.GetNextItem() )
		{
			if( obj == m_pRedo )
				break;
			c++;
		}

		MAP_WriteInt( file, c );
	}

	//
	// write history.
	//
	for( obj = m_UndoRedo.ResetPos(); obj; obj = m_UndoRedo.GetNextItem() )
	{
		CUndoRedoAction::WriteUndoRedoToFile( file, doc, version, obj );
		Sys_GetMainFrame()->GetStatusBar()->StepIt();
	}

	return true;
}

bool CUndoRedoManager::ReadUndoHistroryFromFile( CFile* file, CTreadDoc* doc, int version )
{
	int i, num, undo_pos, redo_pos, skip_size, skip_count;
	CUndoRedoAction* obj;
	//CString debug_string;

	m_UndoRedo.DestroyList();

	num = MAP_ReadInt( file );
	undo_pos = MAP_ReadInt( file );
	redo_pos = MAP_ReadInt( file );

	skip_size = num-MAX_UNDOREDO_LEVEL;
	if( skip_size < 0 )
		skip_size = 0;

	undo_pos -= skip_size;
	redo_pos -= skip_size;
	skip_count = skip_size;

	for( i = 0; i < num; i++ )
	{
		//debug_string.Format("UndoObjectRead: %d\n", i);
		//OutputDebugString( debug_string );

		obj = CUndoRedoAction::ReadUndoRedoFromFile( file, doc, version );
		if( obj )
		{
			if( skip_count > 0 )
			{
				skip_count--;
				delete obj;
			}
			else
			{
				m_UndoRedo.AddItem( obj );
			}
		}

		Sys_GetMainFrame()->GetStatusBar()->StepIt();
	}

	m_pUndo = 0;
	if( undo_pos > -1 )
		m_pUndo = m_UndoRedo.GetItem( undo_pos );

	m_pRedo = 0;
	if( redo_pos > -1 )
		m_pRedo = m_UndoRedo.GetItem( redo_pos );

	m_nNum = num-skip_size;

	/*if( skip_size > 0 )
	{
		MessageBox( 0, "The undo-history on this map exceeded the maximum size. Some actions have been purged.", "NOTE", MB_TASKMODAL|MB_ICONINFORMATION );
	}*/

	return true;
}

void CUndoRedoManager::PurgeFromPos( CUndoRedoAction* pos )
{
	if( !pos )
	{
		m_UndoRedo.SetPosition( LL_HEAD );
	}
	else
	{
		m_UndoRedo.SetPosition( pos );
		m_UndoRedo.GetNextItem();
	}

	for( ;; )
	{
		pos = m_UndoRedo.RemoveItem( LL_CURRENT );
		if( !pos )
			break;

		delete pos;
		m_nNum--;
	}
}

void CUndoRedoManager::AddUndoItem( CUndoRedoAction* item )
{
	PurgeFromPos( m_pUndo );
	while( m_nNum >= MAX_UNDOREDO_LEVEL )
	{
		m_UndoRedo.DeleteItem(LL_HEAD);
		m_nNum--;
	}

	m_nNum++;
	m_UndoRedo.AddItem( item );
	m_pUndo = item;
	m_pRedo = 0;
}

void CUndoRedoManager::ClearUndoStack()
{
	m_pUndo = m_pRedo = 0;
	m_UndoRedo.DestroyList();
	m_nNum = 0;
}

void CUndoRedoManager::UpdateRedoMenu( CCmdUI* pUI )
{
	pUI->Enable( m_pRedo != 0 );
	if( m_pRedo )
	{
		char buff[256];
		sprintf( buff, "Redo %s\tCtrl+Y", m_pRedo->GetTitle() );

		pUI->SetText( buff );
	}
	else
	{
		pUI->SetText( "Redo Action\tCtrl+Y" );
	}
}

void CUndoRedoManager::UpdateUndoMenu( CCmdUI* pUI )
{
	pUI->Enable( m_pUndo != 0 );
	if( m_pUndo != 0 )
	{
		char buff[256];
		sprintf( buff, "Undo %s\tCtrl+Z", m_pUndo->GetTitle() );
		pUI->SetText( buff );
	}
	else
	{
		pUI->SetText( "Undo Action\tCtrl+Z" );
	}
}

void CUndoRedoManager::PerformUndo( CTreadDoc* pDoc )
{
	if( m_pUndo )
	{
		m_pUndo->Undo( pDoc );
		m_pRedo = m_pUndo;
		m_pUndo = (CUndoRedoAction*)m_pUndo->prev;
	}
}

void CUndoRedoManager::PerformRedo( CTreadDoc* pDoc )
{
	if( m_pRedo )
	{
		m_pRedo->Redo( pDoc );
		m_pUndo = m_pRedo;
		m_pRedo = (CUndoRedoAction*)m_pRedo->next;
	}
}

void CUndoRedoManager::CopyObjectList( CLinkedList<CMapObject>* pSrc, CLinkedList<CMapObject>* pDst, CTreadDoc* pDoc )
{
	CMapObject* obj, *copy;

	pDst->DestroyList();
	for( obj = pSrc->ResetPos(); obj; obj = pSrc->GetNextItem() )
	{
		copy = obj->Clone();
		copy->CopyState( obj, pDoc );
		pDst->AddItem( copy );
	}
}

void CUndoRedoManager::DuplicateListIntoDocument( CLinkedList<CMapObject>* pSrc, CTreadDoc* pDoc )
{
	CMapObject* obj, *copy;

	for( obj = pSrc->ResetPos(); obj; obj = pSrc->GetNextItem() )
	{
		copy = obj->Clone();
		pDoc->AddObjectToMap( copy );
		pDoc->AddObjectToSelection( copy );
		copy->CopyState( obj, pDoc );
		pDoc->Prop_AddObject( copy );
	}
}

void CUndoRedoManager::DeleteUIDs(int *ids)
{
	delete [] ids;
}

int* CUndoRedoManager::AllocateUIDs(int num)
{
	return new int[num];
}

int* CUndoRedoManager::GetObjectUIDs( CLinkedList<CMapObject>* pList, int* num )
{
	int* uids;
	
	*num = pList->GetCount();
	if( *num < 1 )
		return 0;

	uids = AllocateUIDs(*num);
	
	int i;
	CMapObject* obj;

	for(i = 0, obj = pList->ResetPos(); obj; i++, obj = pList->GetNextItem() )
	{
		uids[i] = obj->GetUID();
	}

	return uids;
}

void CUndoRedoManager::SelectObjectsByUIDs( CTreadDoc* pDoc, int* uids, int num_uids )
{
	int i;
	CMapObject* obj;

	for(i = 0; i < num_uids; i++)
	{
		obj = pDoc->ObjectForUID( uids[i] );
		if( obj )
		{
			obj->SetVisible( pDoc );
			pDoc->Prop_UpdateObjectState( obj );
			pDoc->AddObjectToMap( obj );
			pDoc->AddObjectToSelection( obj );
		}
	}
}

//////////////////////////////////////////////////////////////////////
// OBJPROP  														//
//////////////////////////////////////////////////////////////////////

IMPLEMENT_TREAD_NEW(CObjProp);

const int CObjProp::integer = 1;
const int CObjProp::floatnum = 2;
const int CObjProp::string = 3;
const int CObjProp::vector = 4;
const int CObjProp::facing = 5;
const int CObjProp::color = 6;
const int CObjProp::script = 7;

CObjProp::CObjProp() : CLLObject()
{
	m_nType = string;
	m_bSubTypeChoices = false;
	m_nListItem = -1;
}

CObjProp::CObjProp( const CObjProp& c ) : CLLObject()
{
	Copy( c );
}

CObjProp& CObjProp::operator = ( const CObjProp& p )
{
	Copy( p );
	return *this;
}

CObjProp::~CObjProp()
{
}

CObjProp* CObjProp::FindProp( CLinkedList<CObjProp>* list, const char* name )
{
	CObjProp* p;

	if( name == 0 || name[0] == 0 )
		return 0;

	for( p = list->ResetPos(); p; p = list->GetNextItem() )
	{
		if( !stricmp( p->GetName(), name ) )
			return p;
	}

	return 0;
}


void CObjProp::SetListItem( int item )
{
	m_nListItem = item;
}

int CObjProp::GetListItem()
{
	return m_nListItem;
}

void CObjProp::Copy( const CObjProp& t )
{
	CObjProp& c = (CObjProp&)t;

	m_sVal = c.m_sVal;
	m_sName = c.m_sName;
	m_sDisplayName = c.m_sDisplayName;
	m_nType = c.m_nType;
	m_bSubTypeChoices = c.m_bSubTypeChoices;

	//
	// copy choice list.
	//
	m_Choices.DestroyList();

	CObjProp* prop, *cp;

	for( prop = c.m_Choices.ResetPos(); prop; prop = c.m_Choices.GetNextItem() )
	{
		cp = new CObjProp(*prop);
		m_Choices.AddItem( cp );
	}
}

void CObjProp::Inherit(const CObjProp &t)
{
	CObjProp& c = (CObjProp&)t;

	m_sVal = c.m_sVal;
	m_sName = c.m_sName;
	m_sDisplayName = c.m_sDisplayName;
	m_nType = c.m_nType;
	m_bSubTypeChoices = c.m_bSubTypeChoices;

	//
	// copy choice list.
	//

	CObjProp* prop, *cp;

	for( prop = c.m_Choices.ResetPos(); prop; prop = c.m_Choices.GetNextItem() )
	{
		cp = FindProp(&m_Choices, prop->GetName());
		if (!cp)
		{
			cp = new CObjProp(*prop);
			m_Choices.AddItem( cp );
		}
		cp->Inherit(*prop);
	}
}

void CObjProp::SetValue( CObjProp* prop )
{
	m_sVal = prop->m_sVal;
}

CLinkedList<CObjProp>* CObjProp::GetChoices()
{
	return &m_Choices;
}

void CObjProp::AddChoice( CObjProp* choice )
{
	// this choice will replace one that already exists.
	for (CObjProp *p = m_Choices.ResetPos(); p; p = m_Choices.GetNextItem())
	{
		if (!strcmp(p->GetString(), choice->GetString()))
		{
			p = m_Choices.ReplaceNode(choice, true);
			return;
		}
	}
	m_Choices.AddItem( choice );
}

const char* CObjProp::GetName()
{
	return m_sName;
}

void CObjProp::SetName( const char* name )
{
	if( !name || !name[0] )
		m_sName = "";
	else
		m_sName = name;
}

const char* CObjProp::GetDisplayName()
{
	return m_sDisplayName;
}

void CObjProp::SetDisplayName( const char* name )
{
	if( !name || !name[0] )
		m_sDisplayName = "";
	else
		m_sDisplayName = name;
}

int CObjProp::GetType()
{
	return m_nType;
}

void CObjProp::SetType( int type )
{
	m_nType = type;
}

int CObjProp::GetInt()
{
	int i;
	sscanf( m_sVal, "%d", &i );
	return i;
}

void CObjProp::SetInt( int i )
{
	char buff[64];
	sprintf( buff, "%d", i );
	m_sVal = buff;
}

float CObjProp::GetFloat()
{
	float f;
	sscanf( m_sVal, "%f", &f );
	return f;
}

void CObjProp::SetFloat( float f )
{
	char buff[64];
	sprintf(buff, "%f", f);
	m_sVal = buff;
}

const char* CObjProp::GetString()
{
	return m_sVal;
}

void CObjProp::SetString( const char* string )
{
	if( !string || !string[0] )
		m_sVal = "";
	else
		m_sVal = string;
}

vec3 CObjProp::GetVector()
{
	vec3 c;

	sscanf( m_sVal, "%f %f %f", &c.x, &c.y, &c.z );
	return c;
}

void CObjProp::SetVector( const vec3& v )
{
	char buff[64];
	sprintf( buff, "%f %f %f", v.x, v.y, v.z );
	m_sVal = buff;
}

bool CObjProp::GetSubType()
{
	return m_bSubTypeChoices;
}

void CObjProp::SetSubType( bool b )
{
	m_bSubTypeChoices = b;
}

//////////////////////////////////////////////////////////////////////
// PICKOBJECT  														//
//////////////////////////////////////////////////////////////////////

IMPLEMENT_TREAD_NEW(CPickObject);

CPickObject::CPickObject()
{
	m_parent = 0;
	m_viewflags = 0;
}

CPickObject::~CPickObject()
{
}

void CPickObject::SetViewFlags( int flags )
{
	m_viewflags = flags;
}

int CPickObject::GetViewFlags()
{
	return m_viewflags;
}

CPickObject* CPickObject::GetParent( bool immediate  )
{
	if( !m_parent )
		return 0;

	if( immediate )
		return m_parent;

	CPickObject* p = m_parent;
	while( p->GetParent( true ) )
	{
		p = p->GetParent( true );
	}

	return p;
}

void CPickObject::SetParent( CPickObject* p )
{
	m_parent = p;
}

bool CPickObject::Select( CTreadDoc* pDoc, CPickObject* pSrc, bool message_parent )
{
	if( !pSrc ) pSrc = this;

	if( message_parent )
	{
		CPickObject* p = GetParent(false);
		if( p )
			return p->Select( pDoc, pSrc, false );
	}

	return OnSelect( pDoc, pSrc );
}

bool CPickObject::Deselect ( CTreadDoc* pDoc, CPickObject* pSrc, bool message_parent )
{
	if( !pSrc ) pSrc = this;

	if( message_parent )
	{
		CPickObject* p = GetParent(false);
		if( p )
			return p->Deselect( pDoc, pSrc, false );
	}

	return OnDeselect( pDoc, pSrc );
}

void CPickObject::MouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc, bool message_parent )
{
	if( !pSrc ) pSrc = this;

	if( message_parent )
	{
		CPickObject* p = GetParent(false);
		if( p )
		{
			p->MouseDown( pView, nMX, nMY, nButtons, pSrc, false );
			return;
		}
	}

	OnMouseDown( pView, nMX, nMY, nButtons, pSrc );
}

void CPickObject::MouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc, bool message_parent )
{
	if( !pSrc ) pSrc = this;

	if( message_parent )
	{
		CPickObject* p = GetParent(false);
		if( p )
		{
			p->MouseMove( pView, nMX, nMY, nButtons, pSrc, false );
			return;
		}
	}

	OnMouseMove( pView, nMX, nMY, nButtons, pSrc );
}

void CPickObject::MouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc, bool message_parent )
{
	if( !pSrc ) pSrc = this;

	if( message_parent )
	{
		CPickObject* p = GetParent(false);
		if( p )
		{
			p->MouseUp( pView, nMX, nMY, nButtons, pSrc, false );
			return;
		}
	}

	OnMouseUp( pView, nMX, nMY, nButtons, pSrc );
}

void CPickObject::MouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc, bool message_parent )
{
	if( !pSrc ) pSrc = this;

	if( message_parent )
	{
		CPickObject* p = GetParent(false);
		if( p )
		{
			p->MouseEnter( pView, nMX, nMY, nButtons, pSrc, false );
			return;
		}
	}

	OnMouseEnter( pView, nMX, nMY, nButtons, pSrc );
}

void CPickObject::MouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc, bool message_parent )
{
	if( !pSrc ) pSrc = this;

	if( message_parent )
	{
		CPickObject* p = GetParent(false);
		if( p )
		{
			p->MouseLeave( pView, nMX, nMY, nButtons, pSrc, false );
			return;
		}
	}

	OnMouseLeave( pView, nMX, nMY, nButtons, pSrc );
}

bool CPickObject::PopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc, bool message_parent )
{
	if( !pSrc ) pSrc = this;

	if( message_parent )
	{
		CPickObject* p = GetParent(false);
		if( p )
		{
			return p->PopupMenu( pView, nMX, nMY, nButtons, pSrc, false );
		}
	}

	return OnPopupMenu( pView, nMX, nMY, nButtons, pSrc );
}

bool CPickObject::OnPopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
	return false;
}

bool CPickObject::OnSelect( CTreadDoc* pDoc, CPickObject* pSrc )
{
	return false;
}

bool CPickObject::OnDeselect( CTreadDoc* pDoc, CPickObject* pSrc )
{
	return false;
}

void CPickObject::OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
}

void CPickObject::OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
}

void CPickObject::OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
}

void CPickObject::OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
}

void CPickObject::OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc )
{
}

//////////////////////////////////////////////////////////////////////
// OBJECTMENU                                                       //
//////////////////////////////////////////////////////////////////////

IMPLEMENT_TREAD_NEW(CObjectMenu);

CObjectMenu::CObjectMenu()
{
}

CObjectMenu::~CObjectMenu()
{
}

void CObjectMenu::OnUpdateCmdUI(int id, CCmdUI* pUI )
{
}

void CObjectMenu::OnMenuItem(int id)
{
}

void CObjectMenu::AddMenuItem( int id, const char* string, bool absolute_id )
{
	CObjectMenuItem* p = new CObjectMenuItem();
	p->id = (absolute_id) ? id : (id+ID_OBJMENU_MIN);
	p->sMenuString = string;
	m_MenuList.AddItem( p );
}

CMenu* CObjectMenu::FindMenuItem( CMenu* pBaseMenu, const char* title )
{
	UINT i, c;

	c = pBaseMenu->GetMenuItemCount();
	for( i = 0; i < c; i++ )
	{
		CMenu* pM = pBaseMenu->GetSubMenu( i );
		if( pM )
		{
			CString string;
			pBaseMenu->GetMenuString( i, string, MF_BYPOSITION );
			if( string == title )
				return pM;
		}
	}

	return 0;
}

void CObjectMenu::BuildMenuFromObjectMenu( CMenu* pMenu, CObjectMenu* pObjMenu )
{
	CMenu* pSub;
	CLinkedList<CObjectMenuItem>* pList;
	CObjectMenuItem* pItem;
	int ofs;
	CString str, title;

	pObjMenu->m_SubMenuList.DestroyList();

	//
	// delete menu contents.
	//
	while( pMenu->DeleteMenu( 0, MF_BYPOSITION ) != 0 ) {}

	pList = &pObjMenu->m_MenuList;
	if( !pList )
		return;

	for( pItem = pList->ResetPos(); pItem; pItem = pList->GetNextItem() )
	{
		str = pItem->sMenuString;
		pSub = pMenu;

		for(;;)
		{
			title = str;
			ofs = str.Find('\n');
			if( ofs == -1 )
				break;
			title = str.Left(ofs);
			str.Delete(0, ofs+1);

			CMenu* pPop = FindMenuItem( pSub, title );
			if(!pPop)
			{
				pPop = new CMenu();
				pPop->CreatePopupMenu();
				pSub->AppendMenu(MF_POPUP, (UINT)pPop->GetSafeHmenu(), title );
				
				CObjectSubMenu* pWrapper = new CObjectSubMenu();
				pWrapper->m_pMenu = pPop;
				pObjMenu->m_SubMenuList.AddItem( pWrapper );
			}

			pSub = pPop;
		}

		if( title == "@SEP@" )
			pSub->AppendMenu(MF_SEPARATOR);
		else
			pSub->AppendMenu(MF_STRING, pItem->id, title );
	}
}

IMPLEMENT_TREAD_NEW(CObjectMenuItem);

CObjectMenuItem::CObjectMenuItem() : CLLObject()
{
	id = ID_OBJMENU_MIN;
	sMenuString = "";
}

CObjectMenuItem::~CObjectMenuItem()
{
}

CObjectSubMenu::CObjectSubMenu() : CLLObject()
{
	m_pMenu = 0;
}

CObjectSubMenu::~CObjectSubMenu()
{
	if( m_pMenu )
		delete m_pMenu;
}

//////////////////////////////////////////////////////////////////////
// RENDERMESH  														//
//////////////////////////////////////////////////////////////////////

IMPLEMENT_TREAD_NEW(CRenderMesh);

CRenderMesh::CRenderMesh()
{
	num_pts = 0;
	xyz = 0;
	normals[0] = normals[1] = normals[2] = 0;
	st = 0;
	rgba = 0;
	num_tris = 0;
	tris = 0;
	shader = 0;
	pick = 0;
	texture = 0;
	color2d = 0xFFFFFFFF;
	color3d = 0xFFFFFFFF;
	wireframe3d = 0xFFFFFFFF;
	fcolor3d[0] = fcolor3d[1] = fcolor3d[2] = fcolor3d[3] = 1.0f;
	solid2d = false;
	cmds = GL_TRIANGLES;
	allow_selected = true;
	allow_wireframe = true;
	line_size = 1;
}

CRenderMesh::~CRenderMesh()
{
}

void CRenderMesh::FreeMesh()
{
	if( xyz )
		delete[] xyz;
	if( normals[0] )
		delete[] normals[0];
	if( normals[1] )
		delete[] normals[1];
	if( normals[2] )
		delete[] normals[2];
	if( st )
		delete[] st;
	if( rgba )
		delete[] rgba;
	if( tris )
		delete[] tris;

	num_pts = 0;
	xyz = 0;
	normals[0] = normals[1] = normals[2] = 0;
	st = 0;
	rgba = 0;
	num_tris = 0;
	tris = 0;
	polys.DestroyList();
}

void CRenderMesh::AllocMesh( int n_pts, int n_tris )
{
	FreeMesh();

	num_pts = n_pts;
	num_tris = n_tris;
		
	xyz = new vec3[n_pts];
	normals[0] = new vec3[n_pts];
	normals[1] = new vec3[n_pts];
	normals[2] = new vec3[n_pts];
	st = new vec2[n_pts];
	rgba = new unsigned int[n_pts];

	if( n_tris > 0 )
		tris = new unsigned short[n_tris * 3];
}

//////////////////////////////////////////////////////////////////////
// MAPOBJECT														//
//////////////////////////////////////////////////////////////////////

CMapObject::CMapObject() : CPickObject(), CLLObject()
{
	m_sName = "";
	m_pMapList = 0;
	m_bSelected = false;
	m_bVisible = true;
	m_bAttached = false;
	m_nUID = -1;
	m_nGroupUID = -1;
	m_hTreeItem = 0;
	m_nOwnerUID = -1;
	m_bAnimating = false;
	m_anim_obj = 0;
	m_bEntOwnerCall = false;
	m_bEntGroupCall = false;
}

CMapObject::~CMapObject()
{
	if( m_anim_obj )
		delete m_anim_obj;
}

CMapObject::CMapObject( const CMapObject& mo ) : CPickObject( mo ), CLLObject( mo )
{
	m_sName = "";//mo.m_sName + " Copy";
	m_pMapList = 0;
	m_bSelected = false;
	m_bVisible = mo.m_bVisible;
	m_bAttached = false;
	m_nUID = -1;
	m_nGroupUID = mo.m_nGroupUID;
	m_hTreeItem = 0;
	m_nOwnerUID = -1;
	m_bAnimating = false;
	m_anim_obj = 0;
	m_bEntOwnerCall = false;
	m_bEntGroupCall = false;
}

void CMapObject::CopyObject( CMapObject* obj, CTreadDoc* pDoc )
{
}

void CMapObject::SetInAnimationMode( CTreadDoc* pDoc, bool animating )
{
	if( m_bAnimating == animating )
		return;

	m_bAnimating = animating;
	m_anim_angles = vec3::zero;

	//
	// duplicate this object...
	//
	if( animating )
	{
		m_anim_obj = Clone();
		m_anim_obj->CopyState( this, pDoc );

		OnAnimationMode( pDoc, true );
	}
	else
	{
		//
		// switch out of animation mode.
		// copy the state of our duplicated object.
		//
		OnAnimationMode( pDoc, false );
		CopyObject( m_anim_obj, pDoc );

		delete m_anim_obj;
		m_anim_obj = 0;
	}
}

bool CMapObject::IsInAnimationMode()
{
	return m_bAnimating;
}

void CMapObject::OnAnimationMode( CTreadDoc* pDoc, bool animating )
{
}

void CMapObject::RestoreAnimTransform( CTreadDoc* pDoc )
{
	m_anim_angles = vec3::zero;
	CopyObject( m_anim_obj, pDoc );
}

void CMapObject::SetAnimTransform( CTreadDoc* pDoc, const vec3& pos, const vec3& angles )
{
	CopyObject( m_anim_obj, pDoc );

	m_anim_angles = angles;

	SetObjectWorldPos( pos, pDoc );
	SetObjectTransform( build_rotation_matrix( 0.0f, 1.0f, 0.0f, DEGREES_TO_RADIANS( angles[1] )) *
						build_rotation_matrix( 1.0f, 0.0f, 0.0f, DEGREES_TO_RADIANS( angles[0] )) *
						build_rotation_matrix( 0.0f, 0.0f, 1.0f, DEGREES_TO_RADIANS( angles[2] )), pDoc );

	m_anim_angles = angles;
}

void CMapObject::GetAnimAngles( vec3* angles )
{
	*angles = m_anim_angles;
}

int CMapObject::GetGroupUID()
{
	return m_nGroupUID;
}

void CMapObject::SetGroupUID( int uid )
{
	m_nGroupUID = uid;
}

bool CMapObject::WriteToFile( CFile* pFile, CTreadDoc* pDoc, int nVersion )
{
	MAP_WriteString( pFile, m_sName );
	MAP_WriteInt( pFile, GetUID() );
	MAP_WriteInt( pFile, GetGroupUID() );
	MAP_WriteInt( pFile, IsVisible() );
	MAP_WriteInt( pFile, GetOwnerUID() );
	return true;
}

bool CMapObject::ReadFromFile( CFile* pFile, CTreadDoc* pDoc, int nVersion )
{
	m_sName = MAP_ReadString( pFile );
	
	SetUID( MAP_ReadInt( pFile ) );
	SetGroupUID( MAP_ReadInt( pFile ) );
		
	//
	// we do not call SetVisible() for a reason here!
	//
	m_bVisible = MAP_ReadInt( pFile );

	SetOwnerUID( MAP_ReadInt( pFile ) );

	return true;
}

void CMapObject::SetUID( int uid )
{
	m_nUID = uid;
}

int CMapObject::GetUID()
{
	return m_nUID;
}

bool CMapObject::IsSelected()
{
	return m_bSelected;
}

CLinkedList<CObjProp>* CMapObject::GetPropList( CTreadDoc* pDoc )
{
	return 0;
}

void CMapObject::SetProp(  CTreadDoc* pDoc, CObjProp* prop )
{
}

void CMapObject::ScaleObject( const vec3& scale, const vec3& origin, const vec3& mins_before, const vec3& maxs_before, const vec3& mins_after, const vec3& maxs_after, CTreadDoc* pDoc )
{
}

void CMapObject::SetObjectTransform( const mat3x3& m, CTreadDoc* pDoc )
{
	if( IsInAnimationMode() )
	{
		vec3 a = euler_from_matrix( m );

		m_anim_angles[0] += RADIANS_TO_DEGREES( a[2] );
		m_anim_angles[1] += RADIANS_TO_DEGREES( a[0] );
		m_anim_angles[2] += RADIANS_TO_DEGREES( a[1] );
	}
}

void CMapObject::SetObjectWorldPos( const vec3& pos, CTreadDoc* pDoc )
{
}

void CMapObject::SetEntOwnerCall( bool owner_call )
{
	m_bEntOwnerCall = owner_call;
}

void CMapObject::SetEntGroupCall( bool group_call )
{
	m_bEntGroupCall = group_call;
}

bool CMapObject::OnSelect( CTreadDoc* pDoc, CPickObject* pSrc )
{
	//
	// default action selects us.
	//
	if( IsSelected() )
		return false;

	if( m_bEntOwnerCall == false )
	{
		CEntity* ent = GetOwner( pDoc );
		if( ent )
		{
			ent->SelectOwnedObjects( pDoc );
			return true;
		}
	}
	if( m_bEntGroupCall == false && pDoc->AreGroupsOpen() == false )
	{
		CObjectGroup* grp = GetGroup( pDoc );
		if( grp )
		{
			grp->SelectObjects( pDoc );
		}
	}

	pDoc->AddObjectToSelection( this );
	
	if( pDoc->IsInTrackAnimationMode() )
	{
		SetInAnimationMode( pDoc, true );
	}

	return true;
}

bool CMapObject::OnDeselect( CTreadDoc* pDoc, CPickObject* pSrc )
{
	//
	// default action removes us from selection.
	//
	if( !IsSelected() )
		return false;

	if( m_bEntOwnerCall == false )
	{
		CEntity* ent = GetOwner( pDoc );
		if( ent )
		{
			ent->DeselectOwnedObjects( pDoc );
			return true;
		}
	}
	if( m_bEntGroupCall == false && pDoc->AreGroupsOpen() == false )
	{
		CObjectGroup* grp = GetGroup( pDoc );
		if( grp )
		{
			grp->DeselectObjects( pDoc );
		}
	}

	pDoc->AddObjectToMap( this );
	
	if( pDoc->IsInTrackAnimationMode() )
	{
		SetInAnimationMode( pDoc, false );
	}

	return true;
}

int CMapObject::GetFilterState( CTreadDoc* doc, CObjectFilter* filter )
{
	return OBJFILTER_IGNORE;
}

const char* CMapObject::GetName()
{
	return m_sName;
}

void CMapObject::SetName( const char* szName )
{
	m_sName = szName;
}

void CMapObject::RepaintShader( const char* szShader, CTreadDoc* pDoc )
{
}

void CMapObject::SetShaderName( const char* szShaderName, CTreadDoc* pDoc )
{
}

bool CMapObject::IsVisible()
{
	return m_bVisible;
}

void CMapObject::SetVisible( CTreadDoc* pDoc, bool bVisible )
{
	if( m_bVisible == bVisible )
		return;

	m_bVisible = bVisible;
	OnSetVisible( bVisible, pDoc );
}

void CMapObject::OnSetVisible( bool bVisible, CTreadDoc* pDoc )
{
}

int CMapObject::GetObjectTypeBits()
{
	return 0;
}

int CMapObject::GetSubClass()
{
	return MAPOBJ_SUBCLASS_NONE;
}

bool CMapObject::IsAttached()
{
	return m_bAttached;
}

void CMapObject::OnAddToMap( CTreadDoc* pDoc )
{
	m_bAttached = true;
}

void CMapObject::OnRemoveFromMap( CTreadDoc* pDoc )
{
	m_bAttached = false;
}

void CMapObject::OnAddToSelection( CTreadDoc* pDoc )
{
	m_bAttached = true;
}

void CMapObject::CopyState( CMapObject* src, CTreadDoc* pDoc )
{
	SetUID( src->GetUID() );
	SetName( src->GetName() );
	SetOwnerUID( src->GetOwnerUID() );
}

int CMapObject::GetOwnerUID()
{
	return m_nOwnerUID;
}

CObjectGroup* CMapObject::GetGroup( CTreadDoc* pDoc )
{
	if( m_nGroupUID == -1 )
		return 0;
	if( !IsAttached() )
		return 0;

	return pDoc->GroupForUID( m_nGroupUID );
}

void CMapObject::SetOwnerUID( int uid )
{
	m_nOwnerUID = uid;
}

CEntity* CMapObject::GetOwner( CTreadDoc* pDoc )
{
	CEntity* ent = 0;
	CMapObject* obj;

	if( m_nOwnerUID == -1 )
		return 0;
	if( !IsAttached() )
		return 0;

	obj = pDoc->ObjectForUID( m_nOwnerUID );
	if( obj )
	{
		ent = dynamic_cast<CEntity*>(obj);
	}

	return ent;
}

void CMapObject::UpdateOwnerContents( CTreadDoc* pDoc )
{
}

void CMapObject::SetTreeItem( HTREEITEM hItem )
{
	m_hTreeItem = hItem;
}

HTREEITEM CMapObject::GetTreeItem()
{
	return m_hTreeItem;
}

const char* CMapObject::GetRootName()
{
	return "Object";
}

void CMapObject::FlipObject( const vec3& origin, const vec3& axis )
{
}

void CMapObject::SelectByShader( const char* szShader, CTreadDoc* pDoc )
{
}

void CMapObject::SnapToGrid( float fGridSize, bool x, bool y, bool z )
{
}

void CMapObject::OnConnectToEntDefs(CTreadDoc *doc)
{
}

bool CMapObject::CheckObject( CString& message, CTreadDoc* pDoc )
{
	return false;
}

bool CMapObject::CanAddToTree()
{
	return true;
}

int CMapObject::GetNumRenderMeshes( CMapView* )
{
	return 0;
}

CRenderMesh* CMapObject::GetRenderMesh( int, CMapView* )
{
	return 0;
}

void CMapObject::Nudge( const vec3& amt, CTreadDoc* doc )
{
	SetObjectWorldPos(GetObjectWorldPos() + amt, doc);
}

//////////////////////////////////////////////////////////////////////
// MANIPULATOR														//
//////////////////////////////////////////////////////////////////////

IMPLEMENT_TREAD_NEW(CManipulator);

CManipulator::CManipulator() : CPickObject(), CLLObject()
{
	m_pMapList = 0;
	m_bSelected = false;
}

CManipulator::~CManipulator()
{
}

void CManipulator::OnDraw( CMapView* pView )
{
}

//////////////////////////////////////////////////////////////////////
// ORIENTATION														//
//////////////////////////////////////////////////////////////////////
COrientation::COrientation()
{
	delta_angles = vec3::zero;
	org_angles = vec3::zero;
}

COrientation::~COrientation()
{
}

void COrientation::set_q( const quat& qt )
{
	this->q = qt;
	quaternion_to_matrix( &m, &q );
	angles = euler_from_matrix( m );
	delta_angles = vec3::zero;
	org_angles = angles;

	make_vecs();
}

void COrientation::set_angles( const vec3& x )
{
	vec3 a = x;

	if( a[0] > 360.0f )
		a[0] -= 360.0f;
	if( a[0] < 0.0f )
		a[0] += 360.0f;
	
	if( a[1] > 360.0f )
		a[1] -= 360.0f;
	if( a[1] < 0.0f )
		a[1] += 360.0f;

	if( a[2] > 360.0f )
		a[2] -= 360.0f;
	if( a[2] < 0.0f )
		a[2] += 360.0f;

	this->angles = a;

	if( equals( a, vec3::zero, 0.0f ) )
	{
		m = mat3x3::identity;
	}
	else
	{
		m  = build_rotation_matrix( 1.0f, 0.0f, 0.0f, DEGREES_TO_RADIANS( a[0] ) );
		m *= build_rotation_matrix( 0.0f, 1.0f, 0.0f, DEGREES_TO_RADIANS( a[1] ) );
		m *= build_rotation_matrix( 0.0f, 0.0f, 1.0f, DEGREES_TO_RADIANS( a[2] ) );
	}
	matrix_to_quaternion( &q, &m );

	delta_angles = vec3::zero;
	org_angles = x;

	make_vecs();
}

void COrientation::make_vecs()
{
	up = sysAxisZ;
	lft = sysAxisY;
	frw = sysAxisX;

	up  *= m;
	lft *= m;
	frw *= m;
}

//////////////////////////////////////////////////////////////////////
// SYSTEM        													//
//////////////////////////////////////////////////////////////////////

static PrefFileSection_t *SettingsForPlugin(CPluginGame *game);
static PrefFileSection_t *SettingsForPluginClear(CPluginGame *game);

class CDynamicMenuItem;
typedef stdext::hash_map<int, CDynamicMenuItem*> DynamicMenuHash;

class CDynamicMenuItem
{
public:
	virtual ~CDynamicMenuItem() {}
	virtual void Invoke() = 0;
};

class CDynamicMenuPluginFileImport : public CDynamicMenuItem
{
public:
	CDynamicMenuPluginFileImport(CPluginFileImport *p) : m_p(p) {}
	virtual ~CDynamicMenuPluginFileImport() {} // don't clean up importers, they exist for the lifetime of Tread.

	virtual void Invoke() { Sys_GetMainFrame()->OnFileImportmap(m_p); }

private:

	CPluginFileImport *m_p;
};

class CDynamicMenuPluginFileExport : public CDynamicMenuItem
{
public:
	CDynamicMenuPluginFileExport(CPluginFileExport *p) : m_p(p) {}
	virtual ~CDynamicMenuPluginFileExport() { m_p->Release(); }

	virtual void Invoke() { Sys_GetActiveDocument()->OnFileExportmap(m_p); }

private:

	CPluginFileExport *m_p;
};

class CDynamicMenuPluginConfigure : public CDynamicMenuItem
{
public:
	CDynamicMenuPluginConfigure(CPluginGame *p) : m_p(p) {}
	virtual ~CDynamicMenuPluginConfigure() {}

	virtual void Invoke() 
	{ 
		m_p->Configure(); 
		m_p->SaveSettings(SettingsForPluginClear(m_p));
		Sys_SaveSettings();
	}

private:

	CPluginGame *m_p;
};

static CMainFrame* c_pMainFrame = 0;
static CObjectMenu* c_objectmenu=0;
static CConsoleDialog* c_console=0;
static char c_consolebuff[1024*16];
static CObjPropView* c_propview;
static CLinkedList<CPlugin> s_plugins;
static CLinkedList<CPluginGame> s_games;
static CLinkedList<CPluginFileImport> s_importers;
static std::vector<LibRef_t*> s_pluginLibRefs;
static PrefFile_t *s_pluginsINI = 0;
static CLinkedList<CGameDef> s_gameDefs;
static CString s_gameDefName;
static DynamicMenuHash s_dynamicMenus;
static CPluginFileImport *s_import=0;
static CPluginFileExport *s_export=0;

static void DisplayPopup( CMenu* pMenu, CMapView* pView, int mx, int my );
static void LoadIcons();

void Sys_SetImporter(CPluginFileImport *plugin)
{
	s_import = plugin;
}

CPluginFileImport *Sys_Importer()
{
	return s_import;
}

void Sys_SetExporter(CPluginFileExport *plugin)
{
	s_export = plugin;
}

CPluginFileExport *Sys_Exporter()
{
	return s_export;
}

static PrefFileSection_t *SettingsForPlugin(CPluginGame *game)
{
	return PrefFileFindSectionAlways(s_pluginsINI, game->Name());
}

static PrefFileSection_t *SettingsForPluginClear(CPluginGame *game)
{
	PrefFileDeleteSection(s_pluginsINI, game->Name());
	return SettingsForPlugin(game);
}

static void KillDynamicMenus()
{
	for (DynamicMenuHash::iterator it = s_dynamicMenus.begin(); it != s_dynamicMenus.end(); ++it)
	{
		delete it->second;
	}
	s_dynamicMenus.clear();
}

void Sys_OnPluginCommand(int id)
{
	DynamicMenuHash::iterator it = s_dynamicMenus.find(id);
	if (it != s_dynamicMenus.end())
	{
		it->second->Invoke();
	}
}

void Sys_BuildPluginMenu(CMenu *root, CTreadDoc *doc)
{
	KillDynamicMenus();

	int menuID = ID_PLUGIN_GLOBAL_MENU_MIN;
	int menuOfs = 0;
	{
		CString s;
		for (int i = 0; ; ++i)
		{
			if (!root->GetMenuString(i, s, MF_BYPOSITION) && i > 0) break;
			if (s == "&File")
			{
				menuOfs = i;
				break;
			}
		}
	}
	CMenu *menu = root->GetSubMenu(menuOfs); // File.

	// Do File Menu.

	if (doc)
	{
		// kill all the importers.
		MENUITEMINFOA x;
		for (;;)
		{
			memset(&x, 0, sizeof(x));
			x.cbSize = sizeof(MENUITEMINFOA);
			x.fMask = MIIM_TYPE;
			if (!menu->GetMenuItemInfoA(6, &x, TRUE)) break;
			if (x.fType == MF_SEPARATOR) break;
			if (menu->DeleteMenu(6, MF_BYPOSITION) == 0) break;
		}

		// kill all the exporters.
		for (;;)
		{
			memset(&x, 0, sizeof(x));
			x.cbSize = sizeof(MENUITEMINFOA);
			x.fMask = MIIM_TYPE;
			if (!menu->GetMenuItemInfoA(7, &x, TRUE)) break;
			if (x.fType == MF_SEPARATOR) break;
			if (menu->DeleteMenu(7, MF_BYPOSITION) == 0) break;
		}

		int pos = 6;
		for (CPluginFileImport *p = s_importers.ResetPos(); p; p = s_importers.GetNextItem())
		{
			int id = menuID++;
			s_dynamicMenus[id] = new CDynamicMenuPluginFileImport(p);
			menu->InsertMenu(pos++, MF_STRING|MF_BYPOSITION, id, CString("Import ") + p->Type() + CString("..."));
		}

		++pos; // skip separator.
		
		for (int i = 0; ; ++i)
		{
			CPluginFileExport *p = doc->GamePlugin()->FileExporter(i);
			if (!p) break;
			int id = menuID++;
			s_dynamicMenus[id] = new CDynamicMenuPluginFileExport(p);
			menu->InsertMenu(pos++, MF_STRING|MF_BYPOSITION, id, CString("Export ") + p->Type() + CString("..."));
		}
	}
	else
	{
		// kill all the importers.
		MENUITEMINFOA x;
		for (;;)
		{
			memset(&x, 0, sizeof(x));
			x.cbSize = sizeof(MENUITEMINFOA);
			x.fMask = MIIM_TYPE;
			if (!menu->GetMenuItemInfoA(3, &x, TRUE)) break;
			if (x.fType == MF_SEPARATOR) break;
			if (menu->DeleteMenu(3, MF_BYPOSITION) == 0) break;
		}

		int pos = 3;
		for (CPluginFileImport *p = s_importers.ResetPos(); p; p = s_importers.GetNextItem())
		{
			int id = menuID++;
			s_dynamicMenus[id] = new CDynamicMenuPluginFileImport(p);
			menu->InsertMenu(pos++, MF_STRING|MF_BYPOSITION, id, CString("Import ") + p->Type() + CString("..."));
		}
	}

	// Do Edit Menu.
	{
		CString s;
		for (int i = 0; ; ++i)
		{
			if (!root->GetMenuString(i, s, MF_BYPOSITION) && i > 0) break;
			if (s == "&Edit")
			{
				menuOfs = i;
				break;
			}
		}
	}

	menu = root->GetSubMenu(menuOfs);
	
	if (doc)
	{
		const int DOC_EDIT_START = 11;
		while (menu->DeleteMenu(DOC_EDIT_START, MF_BYPOSITION) != 0) {}
	}
	else
	{
		while (menu->DeleteMenu(0, MF_BYPOSITION) != 0) {}
	}

	for (CGameDef *gd = s_gameDefs.ResetPos(); gd; gd = s_gameDefs.GetNextItem())
	{
		CPluginGame *game = Sys_FindPluginGame(gd->PluginGame());
		if (game && game->WantsConfiguration())
		{
			int id = menuID++;
			s_dynamicMenus[id] = new CDynamicMenuPluginConfigure(game);
			menu->AppendMenuA(MF_STRING, id, CString("Configure ") + game->Name() + CString("..."));
		}
	}
}

static void LoadIcons()
{
	int crap;
	FS_FileSearch_t search;
	FS_FileSearchData_t file;
	
	if( FS_OpenSearch( "editor:icons", 0, &search, _fs_osf_recurse|_fs_osf_disk ) == FALSE )
	{
		while( FS_GetNextFile( &search, &file ) == FALSE )
		{
			//
			// valid texture?
			//
			char* ext = StrGetFileExtension( file.name );
			if( !ext || ext == file.name )
				continue;

			//
			// not an image?
			if( stricmp( ext, "jpg" ) && stricmp( ext, "tga" ) && stricmp( ext, "bmp" ) )
				continue;

			TC_CacheTexture( &crap, &crap, file.name, GL_LUMINANCE, TRUE );
		}
		
		FS_CloseSearch( &search );
	}
}

static void LoadPlugins()
{
	FS_FileSearch_t search;
	FS_FileSearchData_t file;
	
	if( FS_OpenSearch( "base:plugins", 0, &search, _fs_osf_disk ) == FALSE )
	{
		while( FS_GetNextFile( &search, &file ) == FALSE )
		{
			//
			// valid texture?
			//
			char* ext = StrGetFileExtension( file.name );
			if( !ext || ext == file.name )
				continue;

			//
			// not an image?
			if(stricmp( ext, "dll" ))
				continue;

			CString buff;
			buff.Format("9%s", file.name);
			LibRef_t *lib = LibRefInit(buff);
			if (lib)
			{
				PLUGIN_APIVERSION_FP apiVerProc = (PLUGIN_APIVERSION_FP)LibRefGetProc(lib, "PluginAPIVersion");
				PLUGIN_CREATE_FP     createProc = (PLUGIN_CREATE_FP)LibRefGetProc(lib, "PluginCreate");

				bool keepLibRef = false;

				if (apiVerProc && createProc)
				{
					if (apiVerProc() == TREAD_API_VERSION)
					{
						for (int i = 0; ; ++i)
						{
							CPlugin *plugin = createProc(i);
							if (!plugin) break;
							keepLibRef = true;
							s_plugins.AddItem(plugin);
						}
					}
				}

				if (!keepLibRef)
				{
					LibRefDelete(lib);
				}
				else
				{
					s_pluginLibRefs.push_back(lib);
				}
			}
		}
		
		FS_CloseSearch( &search );
	}

	for (CPlugin *plugin = s_plugins.ResetPos(); plugin; plugin = s_plugins.GetNextItem())
	{
		for (int i = 0;; ++i)
		{
			CPluginGame *p = plugin->GamePlugin(i);
			if (!p) break;
			s_games.AddItem(p);
		}
		for (int i = 0;; ++i)
		{
			CPluginFileImport *p = plugin->FileImporter(i);
			if (!p) break;
			s_importers.AddItem(p);
		}
	}
}

template <typename T>
static void ReleasePluginObjects(CLinkedList<T> &x)
{
	T *i;
	while (i=x.RemoveItem(LL_HEAD))
	{
		i->Release();
	}
}

static void FreePlugins()
{
	ReleasePluginObjects(s_importers);
	
	{
		CPluginGame *i;
		while (i=s_games.RemoveItem(LL_HEAD))
		{
			i->Release();
		}
	}

	ReleasePluginObjects(s_plugins);

	for (std::vector<LibRef_t*>::iterator it = s_pluginLibRefs.begin(); it != s_pluginLibRefs.end(); ++it)
	{
		LibRefDelete(*it);
	}

	s_pluginLibRefs.clear();
}

static void LoadGameDefs()
{
	FS_FileSearch_t search;
	FS_FileSearchData_t file;
	
	if( FS_OpenSearch( "base", 0, &search, _fs_osf_disk ) == FALSE )
	{
		while( FS_GetNextFile( &search, &file ) == FALSE )
		{
			//
			// valid file?
			//
			char* ext = StrGetFileExtension( file.name );
			if( !ext || ext == file.name )
				continue;

			//
			// not txt file?
			if(stricmp( ext, "txt" ))
				continue;

			CGameDef *gd = CGameDef::LoadFromFile(file.name);
			if (gd)
			{
				if (Sys_FindPluginGame(gd->PluginGame()))
				{
					s_gameDefs.AddItem(gd);
				}
				else
				{
					CString s;
					s.Format("Cannot find plugin '%s' referenced from game type '%s'. Game type will be ignored.", gd->PluginGame(), gd->Name());
					MessageBox(0, s, "Error", MB_OK);
					delete gd;
				}
			}
		}
		
		FS_CloseSearch( &search );
	}
}

void Sys_Init(void)
{
	LoadIcons();
	LoadPlugins();
	LoadGameDefs();
	s_pluginsINI = PrefFileNewFromFileAlways("9:plugins.ini");

	for (CPluginGame *game = s_games.ResetPos(); game; game = s_games.GetNextItem())
	{
		game->Initialize(SettingsForPlugin(game));
	}
}

void Sys_Shutdown(void)
{
	KillDynamicMenus();
	FreePlugins();
	Sys_SaveSettings();
	PrefFileDelete(s_pluginsINI);
}

OS_FNEXP void Sys_DisplayWindowsError(DWORD code)
{
	PVOID buffer;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		0,
		code,
		0,
		(LPSTR)&buffer,
		0,
		0
	);
	MessageBoxA(*Sys_GetMainFrame(), (LPCTSTR)buffer, "Error!", MB_OK);
	LocalFree(buffer);
}

OS_FNEXP void Sys_GetDirectory(const char *path, char *buff, int buffSize)
{
	if (buffSize < 1) return;
	buff[0] = 0;
	int ofs = strlen(path);
	if (ofs < 1) return;
	--ofs;
	while (path[ofs] != '\\' && path[ofs] != '/' && ofs > 0) { --ofs; }
	strncpy(buff, path, ofs);
	buff[ofs] = 0;
}

OS_FNEXP void Sys_GetFilename(const char *path, char *buff, int buffSize)
{
	if (buffSize < 1) return;
	buff[0] = 0;
	int len = strlen(path);
	if (len < 1) return;
	int ofs = len;
	--ofs;
	while (path[ofs] != '\\' && path[ofs] != '/' && ofs > 0) { --ofs; }
	if (path[ofs] == '\\' || path[ofs] == '/') { ++ofs; }
	int count = len-ofs;
	if (count > buffSize) { count = buffSize; }
	strncpy(buff, &path[ofs], count);
	buff[count] = 0;
}

OS_FNEXP bool Sys_FileExists(const char *path)
{
	FILE *fp = fopen(path, "rb");
	if (fp) { fclose(fp); }
	return fp != 0;
}

OS_FNEXP bool Sys_WriteFile(const char *path, const void *data, int len)
{
	FILE *fp = fopen(path, "wb");
	if (!fp) { return false; }
	bool s = fwrite(data, 1, len, fp) == len;
	fclose(fp);
	return s;
}

OS_FNEXP int Sys_ToWideString(const char *str, wchar_t **wstring)
{
	*wstring = 0;
	if (str && str[0])
	{
		size_t size = ::mbstowcs(NULL, str, strlen(str)) + 1; 
		*wstring = new wchar_t[size];
		::mbstowcs(*wstring, str, size);
		return size;
	}
	return 0;
}

OS_FNEXP void Sys_FreeWideString(wchar_t *wstring)
{
	if (wstring)
	{
		delete[] wstring;
	}
}

static const char *s_browseStartDir = 0;
static int CALLBACK ShellBrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		wchar_t *wstr;
		Sys_ToWideString(s_browseStartDir, &wstr);
		if (wstr)
		{
			LPITEMIDLIST pidl;
			SHILCreateFromPath(wstr, &pidl, 0);
			SendMessageA(hwnd, BFFM_SETSELECTION, FALSE, (LPARAM)pidl);
			Sys_FreeWideString(wstr);
		}
	}
	return 0;
}

OS_FNEXP bool Sys_BrowseForFolder(const char *title, char *inoutDir, int buffLen)
{
	CoInitialize(0);

	bool s = false;
	s_browseStartDir = inoutDir;
	// TODO: Add your control notification handler code here
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));
    bi.lpszTitle = title;
	bi.lpfn = ShellBrowseFolderCallback;
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
    if ( pidl != 0 )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        if ( SHGetPathFromIDList ( pidl, path ) )
        {
            strcpy(inoutDir, path);
			s = true;
        }

        // free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }
    }

	return s;
}

OS_FNEXP void Sys_SaveSettings()
{
	PrefFileSaveFile(s_pluginsINI, "9:plugins.ini");
}

CLinkedList<CPlugin> *Sys_Plugins()
{
	return &s_plugins;
}

CLinkedList<CPluginGame> *Sys_PluginGames()
{
	return &s_games;
}

CLinkedList<CPluginFileImport> *Sys_PluginImporters()
{
	return &s_importers;
}

CPluginGame *Sys_FindPluginGame(const char *name)
{
	for (CPluginGame *game = Sys_PluginGames()->ResetPos(); game; game = Sys_PluginGames()->GetNextItem())
	{
		if (!strcmp(game->Name(), name)) return game;
	}
	return 0;
}

CLinkedList<CGameDef> *Sys_GameDefs()
{
	return &s_gameDefs;
}

CGameDef *Sys_FindGameDef(const char *name)
{
	for (CGameDef *def = s_gameDefs.ResetPos(); def; def = s_gameDefs.GetNextItem())
	{
		if (!strcmp(def->Name(), name)) return def;
	}
	return 0;
}

void Sys_SetGameDef(const char *name)
{
	s_gameDefName = name;
}

CString Sys_GameDef()
{
	return s_gameDefName;
}

OS_FNEXP float ENT_Random( float min, float max )
{
	float range = max-min;
	float val;

	if( range == 0 )
		return min;
			
	val = (float)GetRandom(65535);
	
	val = (val/65535.0f)*range;
	val += min;
	
	val = CLAMP_VAL( val, min, max );
	return val;
}

OS_FNEXP void Sys_RepaintObjectsWithShader( const char* szShader )
{
	CWnd* wnd = Sys_GetActiveFrame()->GetWindow( GW_HWNDFIRST );
	while( wnd )
	{
		if( wnd->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) )
		{
			CChildFrame* pFrame = dynamic_cast<CChildFrame*>(wnd);
			if( pFrame )
			{
				pFrame->GetMapView(0)->GetDocument()->RepaintObjectsWithShader( szShader );
			}
		}

		wnd = wnd->GetWindow(GW_HWNDNEXT);
	}
}

OS_FNEXP void Sys_RepaintAllShaders()
{
	CChildFrame* pFrame = Sys_GetActiveFrame();

	if (pFrame)
	{
		CWnd* wnd = pFrame->GetWindow( GW_HWNDFIRST );
		while( wnd )
		{
			if( wnd->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) )
			{
				CChildFrame* pFrame = dynamic_cast<CChildFrame*>(wnd);
				if( pFrame )
				{
					pFrame->GetMapView(0)->GetDocument()->RepaintObjectsWithShader(0);
					pFrame->GetMapView(0)->RedrawWindow();
					pFrame->GetMapView(1)->RedrawWindow();
					pFrame->GetMapView(2)->RedrawWindow();
					pFrame->GetMapView(3)->RedrawWindow();
				}
			}

			wnd = wnd->GetWindow(GW_HWNDNEXT);
		}
	}
}

void Sys_ReloadStuff(void)
{
	//c_ObjectCreatorList.DestroyList();
	//c_selobjentry = 0;

	//Sys_InitCreators();
	Sys_GetPropView()->ObjTreeLoad();

	CChildFrame* pFrame = Sys_GetActiveFrame();

	if (pFrame)
	{
		CWnd* wnd = pFrame->GetWindow( GW_HWNDFIRST );
		while( wnd )
		{
			if( wnd->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) )
			{
				CChildFrame* pFrame = dynamic_cast<CChildFrame*>(wnd);
				if( pFrame )
				{
					pFrame->GetMapView(0)->GetDocument()->ReconnectEntities();
					pFrame->GetMapView(0)->RedrawWindow();
					pFrame->GetMapView(1)->RedrawWindow();
					pFrame->GetMapView(2)->RedrawWindow();
					pFrame->GetMapView(3)->RedrawWindow();
				}
			}

			wnd = wnd->GetWindow(GW_HWNDNEXT);
		}

		Sys_printf("done.\n");
	}
}

void Sys_RegisterPropView( CObjPropView* view )
{
	c_propview = view;
}

CObjPropView* Sys_GetPropView(void)
{
	return c_propview;
}

void Sys_OnPopupMenu( int id )
{
	if( !c_objectmenu )
		return;

	c_objectmenu->OnMenuItem( id-ID_OBJMENU_MIN );
}

void Sys_UpdatePopupMenu( int id, CCmdUI* pUI )
{
	if( !c_objectmenu )
		return;

	c_objectmenu->OnUpdateCmdUI( id-ID_OBJMENU_MIN, pUI );
}

static void DisplayPopup( CMenu* pMenu, CMapView* pView, int mx, int my )
{
	POINT p = {mx, my};
	
	pView->ClientToScreen(&p);
	pMenu->TrackPopupMenu( TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON, p.x, p.y, AfxGetMainWnd() );
}

OS_FNEXP void Sys_DisplayObjectMenu( CMapView* pView, int mx, int my, CObjectMenu* pObjMenu )
{
	CMenu* pMenu = Sys_GetMainFrame()->GetObjectMenuPopup();
	pObjMenu->BuildMenuFromObjectMenu( pMenu, pObjMenu );	

	c_objectmenu = pObjMenu;
	DisplayPopup( pMenu, pView, mx, my );
}

//void Sys_InitCreators(void)
//{
//	Sys_RegisterObjectCreator( "Brushes\nCube", 0, CQBrush::MakeCube, true );
//	Sys_RegisterObjectCreator( "Brushes\nCylinder", 0, CQBrush::MakeCylinder, false );
//	Sys_RegisterObjectCreator( "Brushes\nCone", 0, CQBrush::MakeCone, false );
//}

void Sys_CreateSelectedObject(CMapView* pView, int mx, int my)
{
	if( pView->GetDocument()->CurrentObjectCreator() )
	{
		CMapObject* obj = pView->GetDocument()->CurrentObjectCreator()->CreateObject(pView->GetDocument());
		if( obj )
		{
			vec3 pos;
			pView->WinXYToVec3( mx, my, &pos );

			//
			// 3D?
			//
			if( pView->GetViewType() == VIEW_TYPE_3D )
			{
				//
				// push it forward by the size.
				//
				vec3 mins, maxs;
				obj->GetObjectMinsMaxs( &mins, &maxs );

				pos += (pView->View.or3d.frw*(vec_length(maxs-mins)*0.5f+64.0f));
			}

			//
			// snap it.
			//
			if( pView->GetGridSnap() )
				pos = Sys_SnapVec3( pos, pView->GetGridSize() );

			pView->GetDocument()->AssignUID( obj );
			obj->SetName( pView->GetDocument()->MakeUniqueObjectName( obj ) );

			pView->GetDocument()->MakeUndoCreateAction( "Create Object", obj );
			pView->GetDocument()->ClearSelection();
			pView->GetDocument()->AddObjectToMap(obj);
			
			obj->SetObjectWorldPos(pos, pView->GetDocument() );

			pView->GetDocument()->Prop_AddObject( obj );
			obj->Select( pView->GetDocument() );

			pView->GetDocument()->UpdateSelectionInterface();
			pView->GetDocument()->Prop_UpdateSelection();
			
		}
	}
}

OS_FNEXP void Sys_ResetTextureView( void )
{
	int i;
	CMapView* pView;
	CChildFrame* pFrame = Sys_GetActiveFrame();

	if (pFrame)
	{
		for(i = 0; i < 4; i++)
		{
			pView = pFrame->GetMapView( i );
			pView->TexView.fYofs = 0;
		}
	}
}

OS_FNEXP void Sys_AdjustToViewTexture( CShader* shader )
{
	int i;
	CMapView* pView;
	CChildFrame* pFrame = Sys_GetActiveFrame();

	if( !shader || !pFrame )
		return;

	for(i = 0; i < 4; i++)
	{
		pView = pFrame->GetMapView( i );
		if( pView->GetViewType()&VIEW_TYPE_TEXTURE )
			R_AdjustToViewTexture( pView, shader );	
	}
}

OS_FNEXP void Sys_ShowCursor( bool show )
{
	if( show )
	{
		while( ShowCursor(TRUE) < 0 ) {}
	}
	else
	{
		while( ShowCursor(FALSE) > -1 ) {}
	}
}

OS_FNEXP void Sys_RedrawWindows( int type )
{
	int i;
	CMapView* pView;
	CChildFrame* pFrame = Sys_GetActiveFrame();

	if (pFrame)
	{
		for(i = 0; i < 4; i++)
		{
			pView = pFrame->GetMapView( i );
			if( pView->GetViewType()&type )
				pView->RedrawWindow();
		}
	}
}




CChildFrame* Sys_GetActiveFrame(void)
{
	return (CChildFrame*)Sys_GetMainFrame()->MDIGetActive();
}

OS_FNEXP CTreadDoc* Sys_GetActiveDocument(void)
{
	if( Sys_GetActiveFrame() == 0 )
		return 0;

	return (CTreadDoc*)Sys_GetActiveFrame()->GetMapView(0)->GetDocument();
}

void Sys_RegisterMainFrame( CMainFrame* pFrame )
{
	c_pMainFrame = pFrame;
}

CMainFrame* Sys_GetMainFrame(void)
{
	return c_pMainFrame;
}

OS_FNEXP void Sys_SetStatusBarRange(int nLower, int nUpper)
{
	c_pMainFrame->GetStatusBar()->SetRange(nLower, nUpper);
}

OS_FNEXP void Sys_SetStatusBarPos(int nPos)
{
	c_pMainFrame->GetStatusBar()->SetPos(nPos);
}

OS_FNEXP void Sys_OffsetStatusBarPos(int nPos)
{
	c_pMainFrame->GetStatusBar()->OffsetPos(nPos);
}

OS_FNEXP void Sys_SetStatusBarStep(int nStep)
{
	c_pMainFrame->GetStatusBar()->SetStep(nStep);
}

OS_FNEXP void Sys_StepStatusBar()
{
	c_pMainFrame->GetStatusBar()->StepIt();
}

vec3 Sys_LookAngles( const vec3& pos, const vec3& look, const vec3& upin )
{
	vec3 lft, frw, up;

	up = upin;
	frw = look - pos;
	frw.normalize();

	lft = cross( up, frw );
	lft.normalize();

	up = cross( frw, lft );
	up.normalize();

	if( dot( up, upin ) < 0.0f )
	{
		up = -up;
		lft = -lft;
	}

	mat3x3 m( frw, lft, up );

	vec3 angles = euler_from_matrix( m ) * (180.0f / PI);
	vec3 out;

	out[0] = 0.0f;
	out[1] = -angles[0];
	out[2] = -angles[1];

	return out;
}

void Sys_SetConsole( CConsoleDialog* console )
{
	c_console = console;
}

OS_FNEXP void Sys_printf( const char* fmt, ... )
{
	if( c_console )
	{
		va_list argptr;
		va_start(argptr, fmt);
		vsprintf(c_consolebuff, fmt, argptr);
		va_end(argptr);
		
		c_console->WriteText( c_consolebuff );
	}
}

OS_FNEXP float Sys_Snapf( float map_coord, float grid_size )
{
	float f, c, m;

	m = map_coord/grid_size;
	f = floorf( m ) * grid_size;
	c = ceilf ( m ) * grid_size;

	float fm = fabsf( map_coord-f );
	float fc = fabsf( map_coord-c );

	if( fm < fc )
		return f;

	return c;
}

OS_FNEXP vec3  Sys_SnapVec3( const vec3& v, float grid_size )
{
	vec3 out;

	out[0] = Sys_Snapf( v[0], grid_size );
	out[1] = Sys_Snapf( v[1], grid_size );
	out[2] = Sys_Snapf( v[2], grid_size );

	return out;
}

OS_FNEXP int Sys_Translate( CMapObject* obj, void* parm, void* parm2 )
{
	vec3* t = (vec3*)parm;
	obj->SetObjectWorldPos(obj->GetObjectWorldPos() + (*t), (CTreadDoc*)parm2 );
	return 0;
}

OS_FNEXP int Sys_Nudge( CMapObject* obj, void* parm, void* parm2 )
{
	obj->Nudge( *((vec3*)parm), (CTreadDoc*)parm2 );
	return 0;
}

OS_FNEXP int Sys_Scale( CMapObject* obj, void* parm, void* parm2 )
{
	SysScaleInfo_t* inf = (SysScaleInfo_t*)parm;
	obj->ScaleObject( inf->scale, inf->pos, inf->mins_before, inf->maxs_before, inf->mins_after, inf->maxs_after, inf->doc );
	return 0;
}

OS_FNEXP int Sys_Rotate( CMapObject* obj, void* parm, void* parm2 )
{
	SysRotateInfo_t* info = (SysRotateInfo_t*)parm;

	obj->SetObjectTransform( info->m, info->doc );

	//
	// rotate the origin.
	//
	vec3 pos;
	vec3 org = info->org;

	pos = obj->GetObjectWorldPos();
	pos -= org;
	pos *= info->m;
	pos += org;
	obj->SetObjectWorldPos( pos, info->doc );

	return 0;
}

OS_FNEXP void OS_ExitMsg( const char* fmt, ... )
{
	va_list argptr;
	char temp_string[1024];
	
	va_start(argptr, fmt);
	vsprintf(temp_string, fmt, argptr);
	va_end(argptr);

	OS_OkAlertMessage( "OS_Exit:", temp_string );
	OS_Exit();
}

OS_FNEXP void Sys_SetCursor(CMapView* pView, int nCursor)
{
	if(nCursor == TC_DEFAULT)
	{
		pView->m_hcursor = NULL;
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return;
	}
	else if(nCursor <= TC_IDC_SIZENS && nCursor > 0)
		pView->m_hcursor = LoadCursor(NULL, c_cursors[nCursor]);
	else
		pView->m_hcursor = LoadCursor((HINSTANCE)GetModuleHandle(NULL), c_cursors[nCursor]);
}

OS_FNEXP void Sys_Sort( int in1, int in2, int* min, int* max )
{
	if( in1 <= in2 )
	{
		*min = in1;
		*max = in2;
		return;
	}
	
	*min = in2;
	*max = in1;
}

static float c_selstartx;
static float c_selstarty;
static int c_selstartmx;
static int c_selstartmy;
static bool  c_selstartdrag;
static bool  c_selsnap;
static bool  c_selmove;

OS_FNEXP void Sys_SetMouseCapture( CMapView* pView )
{
	if( pView )
	{
		pView->SetCapture();
	}
	else
	{
		ReleaseCapture();
	}
}

OS_FNEXP void Sys_BeginDragSel( CMapView* pView, int mx, int my, int buttons )
{
	Sys_SetMouseCapture( pView );
	pView->WinXYToMapXY( mx, my, &c_selstartx, &c_selstarty );

	c_selmove = false;
	c_selstartdrag = true;
	c_selstartmx = mx;
	c_selstartmy = my;

	if( pView->GetGridSnap() )
	{
		c_selsnap = false;
	}
}

OS_FNEXP void Sys_EndDragSel( CMapView* pView, int mx, int my, int buttons )
{
	Sys_SetMouseCapture( 0 );
}

OS_FNEXP bool Sys_DragSel( CMapView* pView, int mx, int my, int buttons )
{
	float mapx, mapy;
	CTreadDoc* pDoc = pView->GetDocument();

	if( c_selstartdrag )
	{
		//
		// ignore DragSel calls when the mouse didn't move much to avoid
		// snapping objects on a click.
		//
		if( abs(c_selstartmx-mx) < 3.0f &&
			abs(c_selstartmy-my) < 3.0f )
		{
			return false;
		}

		c_selstartdrag = false;
	}

	pView->WinXYToMapXY( mx, my, &mapx, &mapy );

	if( pView->GetGridSnap() && !c_selsnap )
	{
		vec3 trans = vec3::zero;

		int count = pDoc->GetSelectedObjectCount();
		if( (count == pDoc->GetSelectedObjectCount(MAPOBJ_CLASS_ENTITY, MAPOBJ_SUBCLASS_NONE)) )
		{
			//
			// we only have entities, so snap them each to the grid.
			//
			vec3 pos, snapp;
			CMapObject* m;
			CLinkedList<CMapObject>* list = pView->GetDocument()->GetSelectedObjectList();

			for( m = list->ResetPos(); m; m = list->GetNextItem() )
			{
				pos = m->GetObjectWorldPos();
				snapp = Sys_SnapVec3( pos, pView->GetGridSize() );

				if( equals( snapp, pos, 0.00001f ) == false )
					break;
			}

			c_selstartx = Sys_Snapf( c_selstartx, pView->GetGridSize() );
			c_selstarty = Sys_Snapf( c_selstarty, pView->GetGridSize() );

			if( !m ) // all are on the grid.
			{
				c_selsnap = true;
				goto seldrag;
			}

			//
			// they are going to move, so clone them/make undo.
			//
			if( buttons&MS_SHIFT )
				pDoc->CloneSelectionInPlace();
			else
				pDoc->GenericUndoRedoFromSelection()->SetTitle("Drag");
			
			int x_axis, y_axis;

			x_axis = QUICK_AXIS( pView->View.or2d.lft );
			y_axis = QUICK_AXIS( pView->View.or2d.up );

			for( m = list->ResetPos(); m; m = list->GetNextItem() )
			{
				snapp = m->GetObjectWorldPos();
				snapp[x_axis] = Sys_Snapf( snapp[x_axis], pView->GetGridSize() );
				snapp[y_axis] = Sys_Snapf( snapp[y_axis], pView->GetGridSize() );
				m->SetObjectWorldPos( snapp, pView->GetDocument() );
			}

			Sys_RedrawWindows();

			c_selsnap = true;
			c_selmove = true;
		}
		else
		{
			//
			// snap the grid selection.
			//
			float xdir, ydir;

			xdir = SIGN(mapx-c_selstartx);
			ydir = SIGN(mapy-c_selstarty);
			
			if( !xdir )
				xdir = 1;
			if( !ydir )
				ydir = 1;

			//
			// what side of the box are we snapping.
			//
			float boxin_x;
			float boxin_y;
			float boxout_x;
			float boxout_y;
			int x_axis, y_axis;

			x_axis = QUICK_AXIS( pView->View.or2d.lft );
			y_axis = QUICK_AXIS( pView->View.or2d.up );

			if( xdir > 0 )
				boxin_x = pDoc->m_selmaxs[x_axis];
			else
				boxin_x = pDoc->m_selmins[x_axis];

			if( ydir > 0 )
				boxin_y = pDoc->m_selmaxs[y_axis];
			else
				boxin_y = pDoc->m_selmins[y_axis];

			//
			// snap...
			//
			boxout_x = Sys_Snapf( boxin_x, pView->GetGridSize() );
			boxout_y = Sys_Snapf( boxin_y, pView->GetGridSize() );

			trans[x_axis] = boxout_x-boxin_x;
			trans[y_axis] = boxout_y-boxin_y;

			c_selstartx = Sys_Snapf( c_selstartx, pView->GetGridSize() );
			c_selstarty = Sys_Snapf( c_selstarty, pView->GetGridSize() );
		
			if( trans[x_axis] == 0.0f && trans[y_axis] == 0.0f )
			{
				c_selsnap = true;
				goto seldrag;
			}

			if( buttons&MS_SHIFT )
				pDoc->CloneSelectionInPlace();
			else
			{
				//
				// since we haven't moved yet, then make an undo item.
				//
				pDoc->GenericUndoRedoFromSelection()->SetTitle( "Drag" );
			}

			pDoc->GetSelectedObjectList()->WalkList( Sys_Translate, &trans, pDoc );
			pDoc->UpdateSelectionInterface();
			Sys_RedrawWindows();

			c_selsnap = true;
			c_selmove = true;
		}
	}

seldrag:

	if( pView->GetGridSnap() )
	{
		mapx = Sys_Snapf( mapx, pView->GetGridSize() );
		mapy = Sys_Snapf( mapy, pView->GetGridSize() );
	}

	if( mapx == c_selstartx && mapy == c_selstarty )
		return false;

	vec3 trans = vec3::zero;
	trans[QUICK_AXIS(pView->View.or2d.lft)] = mapx-c_selstartx;
	trans[QUICK_AXIS(pView->View.or2d.up)] = mapy-c_selstarty;

	c_selstartx = mapx;
	c_selstarty = mapy;

	if( !c_selmove )
	{
		if( buttons&MS_SHIFT )
			pDoc->CloneSelectionInPlace();
		else
			pDoc->GenericUndoRedoFromSelection()->SetTitle( "Drag" );
	}

	c_selmove = true;

	pDoc->GetSelectedObjectList()->WalkList( Sys_Translate, &trans, pDoc );
	pDoc->UpdateSelectionInterface();
	Sys_RedrawWindows();
	return true;
}

OS_FNEXP void OS_Exit(void)
{	
	exit(-1);
}

OS_FNEXP void OS_BreakMsg( const char* fmt, ... )
{
	va_list argptr;
	char temp_string[1024];
	
	va_start(argptr, fmt);
	vsprintf(temp_string, fmt, argptr);
	va_end(argptr);

	OS_OkAlertMessage( "OS_Break:", temp_string );
	OS_Break();
}

OS_FNEXP void OS_Break(void)
{	
#ifdef __OPT_DEBUG__
	OS_EnterDebugger();
#endif
	OS_Exit();
}
