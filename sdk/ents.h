///////////////////////////////////////////////////////////////////////////////
// ents.h
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

#ifndef ENTS_H
#define ENTS_H

#include "System.h"

class C_Tokenizer;
class CSkelModel;

#define EDDS_BOX		0
#define EDDS_JMODEL		1
#define EDDS_ICON		2
#define EDDS_SKEL		3

class OS_CLEXP CEntKey : public CLLObject
{
public:

	DECLARE_TREAD_NEW();

	CEntKey();
	virtual ~CEntKey();

	CEntKey *Clone();

	const char* GetName();
	void SetName( const char* name );

	const char *GetDisplayName();
	void SetDisplayName(const char *name);

	const char* StringForKey();
	int IntForKey();
	vec3 VecForKey();
	float FloatForKey();
	void SetKeyValue( const char* value );

private:

	CString m_sName;
	CString m_sDisplayName;
	CString m_sValue;
};

class OS_CLEXP CEntDef : public CLLObject
{
	friend class CGameDef;
public:

	DECLARE_TREAD_NEW();

	CEntDef();
	virtual ~CEntDef();

	CObjectCreator *MakeObjectFactory();

	const char* GetClassName();
	const char* GetDisplayName();
	const char* GetTreeDisplayName();
	CLinkedList<CObjProp>* GetKeys();
	CObjProp *FindKey(const char *name);
	// copies the specified property into an existing key of the same name.
	// if the key doesn't exist, one is created.
	void InheritProp(CObjProp *prop);

	int  GetDrawStyle();
	void GetBoxInfo( vec3* mins, vec3* maxs, vec3* color );
	const char *GetIcon();
	const char *GetModel();
	const char *GetAnimName();
	const char *GetRotationKey();
	const char *GetTargetSrcKey();
	const char *GetTargetDstKey();
	const char *GetRepaint();
	void GetIconSize( vec2* size );
	void GetSkelOfs( vec3* ofs );
	bool IsSkelFixedOffset();
	bool IsBaseClass();

	//void SetupFromJModel( CJModel* model );

	bool IsOwner();
	
	bool ModelFromKey();
	bool AnimFromKey();

	bool Parse( C_Tokenizer* script, CGameDef *def );

private:

	enum AttBits
	{
		Att_BoxSize = 0x1,
		Att_BoxColor = 0x2,
		Att_Owner = 0x4,
		Att_ModelType = 0x8,
		Att_Icon = 0x10,
		Att_TargetSrc = 0x20,
		Att_TargetDst = 0x40,
		Att_Category  = 0x80,
		Att_Repaint = 0x100
	};

	int m_attflags;

	CString m_sClassname;
	CString m_sDisplayName;
	CString m_sCategory;
	CString m_sTreeDisplayName;
	CString m_sIcon;
	CString m_sModel;
	CString m_sAnim;
	CString m_sRotation;
	CString m_sTargetSrc;
	CString m_sTargetDst;
	CString m_sRepaint;

	vec3 m_boxcolor;
	vec3 m_boxmins, m_boxmaxs;
	vec3 m_skelofs;
	int m_style;
	bool m_owner;
	vec2 m_iconsize;
	bool m_skel_fixed_offset;

	bool m_bModelFromKey;
	bool m_bAnimFromKey;
	bool m_bIsBaseClass;

	bool ParseAtt( C_Tokenizer* script );
	bool ParseKey( C_Tokenizer* script );
	

	CLinkedList<CObjProp> m_keys;

	bool Inherit(CEntDef *def);
};

class CEntity;
class OS_CLEXP CGameDef : public CLLObject
{
public:

	DECLARE_TREAD_NEW();

	~CGameDef() {}
	static CGameDef *LoadFromFile(const char *filename);
	CEntDef* EntDefForName( const char* szName );
	CEntDef *Worldspawn() { return m_worldspawn; }
	CLinkedList<CEntDef>* EntDefList();
	const char *Name() { return m_name; }
	const char *PluginGame() { return m_plugin; }

private:

	CGameDef() : m_worldspawn(0) {}
	CString m_name;
	CString m_plugin;
	CLinkedList<CEntDef> m_defs;
	CEntDef *m_worldspawn;

	friend class CEntDef;
};

class OS_CLEXP CEntity : public CMapObject
{
public:

	DECLARE_TREAD_NEW();

	CEntity();
	CEntity( const CEntity& e );
	virtual ~CEntity();

	const char *GetClassname();
	const char *GetDisplayName();

	void GetWorldMinsMaxs( vec3* pMins, vec3* pMaxs );
	void GetObjectMinsMaxs( vec3* pMins, vec3* pMaxs );
	vec3 GetObjectWorldPos();

	void OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	bool OnPopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );

	void SetObjectWorldPos( const vec3& pos, CTreadDoc* pDoc );
	void SetObjectTransform( const mat3x3& m, CTreadDoc* pDoc );

	bool WriteToFile( CFile* pFile, CTreadDoc* pDoc, int nVersion );
	bool ReadFromFile( CFile* pFile, CTreadDoc* pDoc, int nVersion );

	int GetClass();
	int GetSubClass();

	bool CanAddToTree();

	void OnAddToMap( CTreadDoc* pDoc );

	CMapObject* Clone();

	int GetNumRenderMeshes( CMapView* pView );
	CRenderMesh* GetRenderMesh(int num, CMapView* pView);

	void FlipObject( const vec3& origin, const vec3& axis );
	void SetShaderName( const char* name, CTreadDoc* pDoc );

	const char* GetRootName();

	void SnapToGrid( float fGridSize, bool x, bool y, bool z );
	void SelectByShader( const char* szShaderName, CTreadDoc* pDoc );

	void OnConnectToEntDefs(CTreadDoc* doc);

	CLinkedList<CObjProp>* GetPropList( CTreadDoc* pDoc );
	void SetProp( CTreadDoc* pDoc, CObjProp* prop );

	void OwnObjectList(  CTreadDoc* pDoc, CLinkedList<CMapObject>* list );
	void UnlinkOwnedObjects( CTreadDoc* pDoc );
	void RelinkOwnedObjects( CTreadDoc* pDoc );
	void SelectOwnedObjects( CTreadDoc* pDoc );
	void DeselectOwnedObjects( CTreadDoc* pDoc );
	bool ObjectIsFirstOwned( CMapObject* b );
	int* GetOwnedObjectUIDs( int* num );

	void OnSetVisible( bool bVisible, CTreadDoc* pDoc );
	void OnRemoveFromMap( CTreadDoc* pDoc );

	int GetObjectTypeBits();

	static CMapObject* MakeEntity( CTreadDoc* pDoc, void* parm );
	static CMapObject* MakeFromKeyList( CTreadDoc* pDoc, CLinkedList<CEntKey>* list );
	static CEntKey* FindKey( CLinkedList<CEntKey>* list, const char* name );

	bool CheckObject( CString& message, CTreadDoc* pDoc );

	void OnAnimationMode( CTreadDoc* pDoc, bool animating = true );

	void RepaintShader( const char* szShader, CTreadDoc* pDoc );

protected:

	bool m_bDrag;
	bool m_bLightBoxMesh;
	CString m_sClassname;	// we'll look up the definition from this.
	CString m_sShader;
	CEntDef* m_pDef;		// can be null.

	//
	// owned objects.
	//
	int m_numobjs;
	int* m_objs;

	int m_iconhandle;
	int m_iconkey;
	RTex_t* m_icon;			// icon texture.
	CRenderMesh m_boxmesh;	// for rendering :)
	CRenderMesh m_icomesh;	
	CRenderMesh m_lightbox_mesh;
	CRenderMesh m_lightbox_outline_mesh;

	vec3 m_model_angles;

	vec3* m_shadow_xyz;
	plane3* m_shadow_planes;
	unsigned short* m_skm_tris;

	bool* m_plane_facings;

	vec3 m_pos;
	vec3 m_boxpos;

	CLinkedList<CObjProp> m_props;

	vec3 GetModelAngles();
	void UpdateBoxMesh();
	void SetupLightBoxMesh();
	void SetupIconMesh();
	void SetupProps( CLinkedList<CObjProp>* list );
	static void CopyPropList( CLinkedList<CObjProp>* src, CLinkedList<CObjProp>* dst );

	class CEntityParmsMenu : public CObjectMenu
	{
	public:

		CEntityParmsMenu();
		virtual ~CEntityParmsMenu();

		CEntity* entity;
		CMapView* view;

		void OnUpdateCmdUI( int id, CCmdUI* pUI );
		void OnMenuItem( int id );
	};

	friend class CEntity::CEntityParmsMenu;
	static CEntityParmsMenu m_EntityMenu;
	static bool m_bMenuCreated;
	static void MakeEntityMenu();

	//
	// called by CMapObject
	//
	void CopyObject( CMapObject* obj, CTreadDoc* pDoc );

	static void GenTriMeshTXSpaceVecs( CRenderMesh* mesh );
};

#endif