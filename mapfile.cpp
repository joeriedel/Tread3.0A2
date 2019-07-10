///////////////////////////////////////////////////////////////////////////////
// mapfile.cpp
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
#include "system.h"
#include "TreadDoc.h"
#include "MapView.h"
#include "ChildFrm.h"
#include "ents.h"
#include "mapfile.h"
#include "splinetrack.h"
#include "MainFrm.h"

#define MAP_TAG		'tred'
#define MAP_MAGIC	84654673

#define MAX_STRING_LEN	(1024*64)
static char c_stringbuff[MAX_STRING_LEN];

typedef struct
{
	int version;
	CFile* file;
	CTreadDoc* doc;
} ReadWriteContext_t;

static CMapObject* ObjectForClass( CPluginGame *game, int classbits, int subclassbits );
static void WriteViewData( CFile* file, CMapView* pView, int nVersion );
static void ReadViewData( CFile* file, CMapView* pView, int nVersion );
static void WriteObjectClass( CFile* file, CMapObject* obj );
static void ReadObjectClass( CFile* file, int* classbits, int* subclassbits );
static int WriteObjectListWalk( CMapObject* obj, void* p, void* p2 );
static int WriteObjectGroupListWalk( CObjectGroup* obj, void* p, void* p2 );
static bool c_want_status = false;

bool WriteTreadMap( CFile* file, CTreadDoc* pDoc )
{
	ReadWriteContext_t context;

	context.doc = pDoc;
	context.file = file;
	context.version = MAKE_FILE_VERSION(MAP_VERSION, pDoc->GamePlugin()->FileVersion());

	MAP_WriteInt( file, MAP_TAG );
	MAP_WriteInt( file, MAP_MAGIC );
	MAP_WriteInt( file, context.version );
	MAP_WriteString(file, pDoc->GameDef()->Name());

	WriteViewData( file, pDoc->GetChildFrame()->GetMapView(0), context.version );
	WriteViewData( file, pDoc->GetChildFrame()->GetMapView(1), context.version );
	WriteViewData( file, pDoc->GetChildFrame()->GetMapView(2), context.version );
	WriteViewData( file, pDoc->GetChildFrame()->GetMapView(3), context.version );

	MAP_WriteInt( file, pDoc->GetUIDOffset() );

	{
		int sizes[6];
		pDoc->GetChildFrame()->GetRowColSizes( sizes );
		MAP_WriteInt( file, sizes[0] );
		MAP_WriteInt( file, sizes[1] );
		MAP_WriteInt( file, sizes[2] );
		MAP_WriteInt( file, sizes[3] );
		MAP_WriteInt( file, sizes[4] );
		MAP_WriteInt( file, sizes[5] );
	}

	MAP_WriteInt( file, pDoc->GetRunReleaseBuildOfGame() );
	MAP_WriteInt( file, pDoc->GetCompileOpts() );
	MAP_WriteInt( file, pDoc->GetRunOpts() );

	//
	// write texture filter history.
	//
	{
		int i, num;
		CString* filter;

		MAP_WriteString( file, pDoc->GetTextureFilter() );

		filter = pDoc->GetTextureFilterHistory( &num );
		MAP_WriteInt( file, num );

		for(i = 0; i < num; i++)
		{
			MAP_WriteString( file, filter[i] );
		}
	}

	//
	// nudge factor.
	//
	MAP_WriteFloat( file, pDoc->GetNudgeFactor() );

	//
	// write filter data.
	//
	{
		CObjectFilter* f = pDoc->GetObjectFilter();
		MAP_WriteInt( file, f->BrushFilter.always_shadow );
		MAP_WriteInt( file, f->BrushFilter.areaportal );
		MAP_WriteInt( file, f->BrushFilter.camera_clip );
		MAP_WriteInt( file, f->BrushFilter.corona_block );
		MAP_WriteInt( file, f->BrushFilter.detail );
		MAP_WriteInt( file, f->BrushFilter.monster_clip );
		MAP_WriteInt( file, f->BrushFilter.nodraw );
		MAP_WriteInt( file, f->BrushFilter.noshadow );
		MAP_WriteInt( file, f->BrushFilter.player_clip );
		MAP_WriteInt( file, f->BrushFilter.skybox );
		MAP_WriteInt( file, f->BrushFilter.skyportal );
		MAP_WriteInt( file, f->BrushFilter.solid );
		MAP_WriteInt( file, f->BrushFilter.water );
		MAP_WriteInt( file, f->BrushFilter.window );
	}

	pDoc->GamePlugin()->WriteMapHeader(file, pDoc, context.version);

	{
		int num = pDoc->GetObjectList()->GetCount() + pDoc->GetSelectedObjectList()->GetCount() +
			pDoc->GetObjectGroupList()->GetCount() + pDoc->GetUndoRedoManager()->GetUndoHistoryCount();

		c_want_status = true;
		Sys_GetMainFrame()->GetStatusBar()->SetRange( 0, num );
		Sys_GetMainFrame()->GetStatusBar()->SetPos( 0 );

		MAP_WriteInt( file, num );
	}

	//
	// write the selected and unselected lists.
	//
	MAP_WriteObjectList( file, pDoc, context.version, pDoc->GetObjectList() );
	MAP_WriteObjectList( file, pDoc, context.version, pDoc->GetSelectedObjectList() );

	//
	// write the object group list.
	//
	int c = pDoc->GetObjectGroupList()->GetCount();
	
	MAP_WriteInt( file, c );
	if( c > 0 )
	{
		pDoc->GetObjectGroupList()->WalkList( WriteObjectGroupListWalk, &context );
	}
	
	c_want_status = false;

	//
	// write undo history.
	//
	pDoc->GetUndoRedoManager()->WriteUndoHistoryToFile( file, pDoc, context.version );

	return true;
}

bool ReadTreadMap( CFile* file, CTreadDoc* pDoc )
{
	int tag, magic, version;

	tag = MAP_ReadInt( file );
	magic = MAP_ReadInt( file );
	version = MAP_ReadInt( file );

	if( tag != MAP_TAG || magic != MAP_MAGIC )
		return false;

	if( TREAD_FILE_VERSION(version) > MAP_VERSION )
		return false;

	Sys_SetGameDef(MAP_ReadString(file));
	if (!pDoc->SetupGameDef(false))
	{
		return false;
	}

	if (PLUGIN_FILE_VERSION(version) > pDoc->GamePlugin()->FileVersion())
		return false;

	ReadViewData( file, pDoc->GetChildFrame()->GetMapView(0), version );
	ReadViewData( file, pDoc->GetChildFrame()->GetMapView(1), version );
	ReadViewData( file, pDoc->GetChildFrame()->GetMapView(2), version );
	ReadViewData( file, pDoc->GetChildFrame()->GetMapView(3), version );
	
	pDoc->SetUIDOffset( MAP_ReadInt( file ) );
	
	{
		int sizes[6];
		sizes[0] = MAP_ReadInt( file );
		sizes[1] = MAP_ReadInt( file );
		sizes[2] = MAP_ReadInt( file );
		sizes[3] = MAP_ReadInt( file );
		sizes[4] = MAP_ReadInt( file );
		sizes[5] = MAP_ReadInt( file );

		pDoc->GetChildFrame()->SetRowColSizes( sizes, true );
	}

	pDoc->SetRunReleaseBuildOfGame( MAP_ReadInt( file ) ? true : false );
	pDoc->SetCompileOpts( MAP_ReadInt( file ) );
	pDoc->SetRunOpts( MAP_ReadInt( file ) );
	
	{
		//
		// read texture filter history.
		//
		int i, num;
		CString curfilter;
		CString* filters;

		curfilter = MAP_ReadString( file );
		
		filters = new CString[MAX_TEXTURE_FILTER_HISTORY];
		num = MAP_ReadInt( file );

		for(i = 0; i < num; i++)
			filters[i] = MAP_ReadString( file );

		pDoc->SetTextureFilterHistory( filters, num );
		pDoc->SetTextureFilter( curfilter );
	}
	
	pDoc->SetNudgeFactor( MAP_ReadFloat( file ) );
	
	{
		CObjectFilter* f = pDoc->GetObjectFilter();

		f->BrushFilter.always_shadow = MAP_ReadInt( file );
		f->BrushFilter.areaportal= MAP_ReadInt( file );
		f->BrushFilter.camera_clip = MAP_ReadInt( file );
		f->BrushFilter.corona_block = MAP_ReadInt( file );
		f->BrushFilter.detail = MAP_ReadInt( file );
		f->BrushFilter.monster_clip = MAP_ReadInt( file );
		f->BrushFilter.nodraw = MAP_ReadInt( file );
		f->BrushFilter.noshadow = MAP_ReadInt( file );
		f->BrushFilter.player_clip = MAP_ReadInt( file );
		f->BrushFilter.skybox = MAP_ReadInt( file );
		f->BrushFilter.skyportal = MAP_ReadInt( file );
		f->BrushFilter.solid = MAP_ReadInt( file );
		f->BrushFilter.water = MAP_ReadInt( file );
		f->BrushFilter.window = MAP_ReadInt( file );
	}

	if (!pDoc->GamePlugin()->ReadMapHeader(file, pDoc, version))
	{
		return false;
	}

	{
		int num = MAP_ReadInt( file );

		c_want_status = true;
		Sys_GetMainFrame()->GetStatusBar()->SetRange( 0, num );
		Sys_GetMainFrame()->GetStatusBar()->SetPos( 0 );
	}

	//
	// read the objects.
	//
	CMapObject* obj;
	CLinkedList<CMapObject> temp_list;

	MAP_ReadObjectList( file, pDoc, version, &temp_list );

	//
	// transfer to normal map.
	//
	for( ;; )
	{
		obj = temp_list.RemoveItem( LL_HEAD );
		if( !obj )
			break;

		pDoc->AddObjectToMap( obj );
	}

	MAP_ReadObjectList( file, pDoc, version, &temp_list );

	//
	// transfer to selection list.
	//
	for( ;; )
	{
		obj = temp_list.RemoveItem( LL_HEAD );
		if( !obj )
			break;

		pDoc->AddObjectToMap( obj );
		pDoc->AddObjectToSelection( obj );
	}

	//
	// read groups.
	//
	{
		int i;
		int c = MAP_ReadInt( file );
		CObjectGroup* gr;

		for(i = 0; i < c; i++)
		{
			gr = new CObjectGroup();
			gr->ReadFromFile( file, version );
			Sys_GetMainFrame()->GetStatusBar()->StepIt();
			if( gr->IsNullGroup( pDoc ) )
			{
				delete gr;
				continue;
			}

			pDoc->GetObjectGroupList()->AddItem( gr );
		}
	}

	c_want_status = false;

	//
	// read undo history.
	//
	pDoc->GetUndoRedoManager()->ReadUndoHistroryFromFile( file, pDoc, version );
	
	return true;
}

OS_FNEXP bool MAP_WriteObject( CFile* file, CTreadDoc* pDoc, int nVersion, CMapObject* obj )
{
	WriteObjectClass( file, obj );
	obj->WriteToFile( file, pDoc, nVersion );
	if( c_want_status ) Sys_GetMainFrame()->GetStatusBar()->StepIt();
	return true;
}

OS_FNEXP CMapObject* MAP_ReadObject( CFile* file, CTreadDoc* pDoc, int nVersion )
{
	int classbits, subclassbits;
	CMapObject* obj;

	ReadObjectClass( file, &classbits, &subclassbits );
	obj = ObjectForClass( pDoc->GamePlugin(), classbits, subclassbits );
	if( !obj )
	{
		OS_BreakMsg("Unable to read map object class %i, subclass %i", classbits, subclassbits);
		return 0;
	}
	obj->ReadFromFile( file, pDoc, nVersion );
	if( c_want_status ) Sys_GetMainFrame()->GetStatusBar()->StepIt();
	return obj;
}

static int WriteObjectGroupListWalk( CObjectGroup* obj, void* p, void* p2 )
{
	ReadWriteContext_t* context = (ReadWriteContext_t*)p;
	obj->WriteToFile( context->file, context->version );
	if( c_want_status ) Sys_GetMainFrame()->GetStatusBar()->StepIt();
	return 0;
}

static void WriteObjectClass( CFile* file, CMapObject* obj )
{
	MAP_WriteInt( file, obj->GetClass() );
	MAP_WriteInt( file, obj->GetSubClass() );
}

static void ReadObjectClass( CFile* file, int* classbits, int* subclassbits )
{
	*classbits = MAP_ReadInt( file );
	*subclassbits = MAP_ReadInt( file );
}

static int WriteObjectListWalk( CMapObject* obj, void* p, void* p2 )
{
	ReadWriteContext_t* context = (ReadWriteContext_t*)p;

	MAP_WriteObject( context->file, context->doc, context->version, obj );
	return 0;
}

OS_FNEXP bool MAP_WriteObjectList( CFile* file, CTreadDoc* pDoc, int nVersion, CLinkedList<CMapObject>* pList )
{
	ReadWriteContext_t context;

	context.file = file;
	context.doc = pDoc;
	context.version = nVersion;

	MAP_WriteInt( file, pList->GetCount() );
	pList->WalkList( WriteObjectListWalk, &context );
	return true;
}

OS_FNEXP bool MAP_ReadObjectList( CFile* file, CTreadDoc* pDoc, int nVersion, CLinkedList<CMapObject>* pList )
{
	int i, c;
	CMapObject* obj;
	
	c = MAP_ReadInt( file );
	for(i = 0; i < c; i++)
	{
		obj = MAP_ReadObject( file, pDoc, nVersion );
		if( obj )
			pList->AddItem( obj );
	}

	return true;
}

OS_FNEXP void MAP_WriteMat3x3( CFile* file, mat3x3* m )
{
	int i, k;
	for(i = 0; i < 3; i++)
	{
		for(k = 0; k < 4; k++)
		{
			MAP_WriteFloat( file, m->m[i][k] );
		}
	}
}

OS_FNEXP void MAP_ReadMat3x3( CFile* file, mat3x3* m )
{
	int i, k;
	for(i = 0; i < 3; i++)
	{
		for(k = 0; k < 4; k++)
		{
			m->m[i][k] = MAP_ReadFloat( file );
		}
	}
}

OS_FNEXP void MAP_WriteQuat( CFile* file, quat* q )
{
	MAP_WriteFloat( file, q->x );
	MAP_WriteFloat( file, q->y );
	MAP_WriteFloat( file, q->z );
	MAP_WriteFloat( file, q->w );
}

OS_FNEXP void MAP_ReadQuat( CFile* file, quat* q )
{
	q->x = MAP_ReadFloat( file );
	q->y = MAP_ReadFloat( file );
	q->z = MAP_ReadFloat( file );
	q->w = MAP_ReadFloat( file );
}

OS_FNEXP void MAP_WriteOR( CFile* file, COrientation* pOR )
{
	MAP_WriteMat3x3( file, &pOR->m );
	MAP_WriteQuat( file, &pOR->q );
	MAP_WriteVec3( file, &pOR->angles );
	MAP_WriteVec3( file, &pOR->frw );
	MAP_WriteVec3( file, &pOR->lft );
	MAP_WriteVec3( file, &pOR->up );
}

OS_FNEXP void MAP_ReadOR( CFile* file, COrientation* pOR )
{
	MAP_ReadMat3x3( file, &pOR->m );
	MAP_ReadQuat( file, &pOR->q );
	MAP_ReadVec3( file, &pOR->angles );
	MAP_ReadVec3( file, &pOR->frw );
	MAP_ReadVec3( file, &pOR->lft );
	MAP_ReadVec3( file, &pOR->up );
}

static void WriteViewData( CFile* file, CMapView* pView, int nVersion )
{
	MAP_WriteInt( file, pView->GetViewType() );
	MAP_WriteInt( file, pView->GetLastViewType() );
	MAP_WriteOR( file, &pView->View.or2d );
	MAP_WriteOR( file, &pView->View.or3d );
	
	MAP_WriteInt( file, pView->View.bGridVisible );
	MAP_WriteFloat( file, pView->View.clipdist );
	MAP_WriteFloat( file, pView->View.fInvScale );
	MAP_WriteFloat( file, pView->View.fScale );
	MAP_WriteFloat( file, pView->View.fMapCenterX );
	MAP_WriteFloat( file, pView->View.fMapCenterY );
	MAP_WriteFloat( file, pView->View.fov );
	MAP_WriteVec3( file, &pView->View.pos3d );
	MAP_WriteFloat( file, pView->TexView.fMax );
	MAP_WriteFloat( file, pView->TexView.fYofs );

	MAP_WriteInt( file, pView->GetGridSnap() );
	MAP_WriteFloat( file, pView->GetGridSize() );
	MAP_WriteInt( file, pView->View.bShowWireframe );
	MAP_WriteInt( file, pView->View.bSelectionWireframe );
	MAP_WriteInt( file, pView->View.bShaded );
	MAP_WriteInt( file, pView->View.bShowEntityNames );
}

static void ReadViewData( CFile* file, CMapView* pView, int nVersion )
{
	int type = MAP_ReadInt( file );
	int last = MAP_ReadInt( file );
	
	MAP_ReadOR( file, &pView->View.or2d );
	MAP_ReadOR( file, &pView->View.or3d );

	pView->View.bGridVisible = MAP_ReadInt( file );
	pView->View.clipdist = MAP_ReadFloat( file );
	pView->View.fInvScale = MAP_ReadFloat( file );
	pView->View.fScale = MAP_ReadFloat( file );
	pView->View.fMapCenterX = MAP_ReadFloat( file );
	pView->View.fMapCenterY = MAP_ReadFloat( file );
	pView->View.fov = MAP_ReadFloat( file );
	MAP_ReadVec3( file, &pView->View.pos3d );
	pView->TexView.fMax = MAP_ReadFloat( file );
	pView->TexView.fYofs = MAP_ReadFloat( file );

	pView->SetGridSnap( MAP_ReadInt( file ) );
	pView->SetGridSize( MAP_ReadFloat( file ) );

	pView->View.bShowWireframe = MAP_ReadInt( file );
	pView->View.bSelectionWireframe = MAP_ReadInt( file );
	pView->View.bShaded = MAP_ReadInt( file );
	pView->View.bShowEntityNames = MAP_ReadInt( file );

	pView->View.bSelBoxVisible = false;
	pView->SetViewType( type );
	pView->SetLastViewType( last );

	//
	// clamp the view pos.
	//
	int i;
	for(i = 0; i < 3; i++)
	{
		if( pView->View.pos3d[i] > (1024*64) ||
			pView->View.pos3d[i] < -(1024*64) )
		{
			pView->View.pos3d[i] = 0;
			pView->View.or3d.set_angles( vec3::zero );
		}
	}
}

static CMapObject* ObjectForClass( CPluginGame *game, int classbits, int subclassbits )
{
	{
		CMapObject *obj = game->ObjectForClass(classbits, subclassbits);
		if (obj) return obj;
	}

	switch( classbits )
	{
	case MAPOBJ_CLASS_ENTITY:
		return new CEntity();
	break;
	//case MAPOBJ_CLASS_SPLINETRACK:
	//	return new CSplineTrack();
	//break;
	};

	return 0;
}

OS_FNEXP const char *MAP_ReadString( CFile* file )
{
	int size;
	
	file->Read( &size, 4 );
	if( size < 1 )
		return "";

	file->Read( c_stringbuff, size );
	c_stringbuff[size] = 0;

	return c_stringbuff;
}

OS_FNEXP void MAP_WriteString( CFile* file, const char* string )
{
	int size = strlen( string );

	file->Write( &size, 4 );
	if( size < 1 )
		return;

	file->Write( string, size );
}

OS_FNEXP int MAP_ReadInt( CFile* file )
{
	int i;
	file->Read( &i, 4 );
	return i;
}

OS_FNEXP void MAP_WriteInt( CFile* file, int i )
{
	file->Write( &i, 4 );
}

OS_FNEXP void MAP_WritePlane( CFile* file, plane3* p )
{
	MAP_WriteFloat( file, p->a );
	MAP_WriteFloat( file, p->b );
	MAP_WriteFloat( file, p->c );
	MAP_WriteFloat( file, p->d );
}

OS_FNEXP void MAP_ReadPlane( CFile* file, plane3* p )
{
	p->a = MAP_ReadFloat( file );
	p->b = MAP_ReadFloat( file );
	p->c = MAP_ReadFloat( file );
	p->d = MAP_ReadFloat( file );
}

OS_FNEXP void MAP_ReadVec3( CFile* file, vec3* vec )
{
	(*vec)[0] = MAP_ReadFloat( file );
	(*vec)[1] = MAP_ReadFloat( file );
	(*vec)[2] = MAP_ReadFloat( file );
}

OS_FNEXP void MAP_WriteVec3( CFile* file, vec3* vec )
{
	MAP_WriteFloat( file, (*vec)[0] );
	MAP_WriteFloat( file, (*vec)[1] );
	MAP_WriteFloat( file, (*vec)[2] );
}

OS_FNEXP void MAP_ReadVec2( CFile* file, vec2* vec )
{
	(*vec)[0] = MAP_ReadFloat( file );
	(*vec)[1] = MAP_ReadFloat( file );
}

OS_FNEXP void MAP_WriteVec2( CFile* file, vec2* vec )
{
	MAP_WriteFloat( file, (*vec)[0] );
	MAP_WriteFloat( file, (*vec)[1] );
}

OS_FNEXP float MAP_ReadFloat( CFile* file )
{
	float f;
	file->Read( &f, 4 );
	return f;
}

OS_FNEXP void MAP_WriteFloat( CFile* file, float f )
{
	file->Write( &f, 4 );
}
