///////////////////////////////////////////////////////////////////////////////
// Brush.h
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

#ifndef BRUSH_H
#define BRUSH_H

#include "System.h"

// from qfiles.h

// contents flags are seperate bits
// a given brush can contribute multiple content bits
// multiple brushes can be in a single leaf

// these definitions also need to be in q_shared.h!

// lower bits are stronger, and will eat weaker brushes completely
#define	CONTENTS_SOLID			1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			2		// translucent, but not watery
#define	CONTENTS_AUX			4
#define	CONTENTS_LAVA			8
#define	CONTENTS_SLIME			16
#define	CONTENTS_WATER			32
#define	CONTENTS_MIST			64
#define	LAST_VISIBLE_CONTENTS	64

// remaining contents are non-visible, and don't eat brushes

#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	CONTENTS_DEADMONSTER	0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_ALL			0xFFFFFFFF


#define	SURF_LIGHT		0x1		// value will hold the light strength

#define	SURF_SLICK		0x2		// effects game physics

#define	SURF_SKY		0x4		// don't draw, but add to skybox
#define	SURF_WARP		0x8		// turbulent water warp
#define	SURF_TRANS33	0x10
#define	SURF_TRANS66	0x20
#define	SURF_FLOWING	0x40	// scroll towards angle
#define	SURF_NODRAW		0x80	// don't bother referencing the texture

#define	SURF_HINT		0x100	// make a primary bsp splitter
#define	SURF_SKIP		0x200	// completely ignore, allowing non-closed brushes
#define SURF_ALL		0xFFFFFFFF

#define USER_WELD_DIST 0.99f
#define PROGRAM_WELD_DIST 0.9f
#define UNDOREDO_CLASS_FIRSTQUAKE   (UNDOREDO_FIRST_USER+1000)
#define UNDOREDO_CLASS_HOLLOW		(UNDOREDO_CLASS_FIRSTQUAKE+0)
#define UNDOREDO_CLASS_CARVE		(UNDOREDO_CLASS_FIRSTQUAKE+1)

///////////////////////////////////////////////////////////////////////////////
// CUndoRedoHollow
///////////////////////////////////////////////////////////////////////////////

class CUndoRedoHollow : public CUndoRedoAction
{
public:
	CUndoRedoHollow();
	virtual ~CUndoRedoHollow();

	virtual void Undo(CTreadDoc *doc);
	virtual void Redo(CTreadDoc *doc);

	virtual void ReconnectEntDefs(CTreadDoc *doc);

	virtual int GetClass() { return UNDOREDO_CLASS_HOLLOW; }

	virtual bool WriteToFile(CFile* file, CTreadDoc* doc, int nVersion);
	virtual bool ReadFromFile(CFile* file, CTreadDoc* doc, int nVersion);

	virtual void InitUndo(CLinkedList<CMapObject>* pSrc, CLinkedList<CMapObject>* pResult, CTreadDoc* pDoc);

private:

	CLinkedList<CMapObject> m_SrcList;
	CLinkedList<CMapObject> m_ResultList;
};

///////////////////////////////////////////////////////////////////////////////
// CUndoRedoCarve
///////////////////////////////////////////////////////////////////////////////

class CUndoRedoCarve : public CUndoRedoAction
{
public:
	CUndoRedoCarve();
	virtual ~CUndoRedoCarve();

	virtual void Undo(CTreadDoc *doc);
	virtual void Redo(CTreadDoc *doc);

	virtual void ReconnectEntDefs(CTreadDoc *doc);

	virtual int GetClass() { return UNDOREDO_CLASS_CARVE; }

	virtual bool WriteToFile(CFile* file, CTreadDoc* doc, int nVersion);
	virtual bool ReadFromFile(CFile* file, CTreadDoc* doc, int nVersion);

	virtual void InitUndo(CLinkedList<CMapObject>* pSelected, CLinkedList<CMapObject>* pSrc, CLinkedList<CMapObject>* pResult, CTreadDoc* pDoc);

private:

	int *m_selectedUIDs;
	int  m_numSelectedUIDs;
	int *m_srcUIDs;
	int  m_numSrcUIDs;
	int *m_resultUIDs;
	int  m_numResultUIDs;
	CLinkedList<CMapObject> m_SrcList;
	CLinkedList<CMapObject> m_ResultList;
};

///////////////////////////////////////////////////////////////////////////////
// CQBrushHandle_Manipulator
///////////////////////////////////////////////////////////////////////////////

class CQBrush;
class CQBrushHandle_Manipulator : public CManipulator
{
public:

	CQBrushHandle_Manipulator();
	virtual ~CQBrushHandle_Manipulator();

	CQBrush* brush;
	
	void OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnDraw( CMapView* pView );
	bool OnPopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );

	bool m_bHover;
	bool m_bDrag;
	bool m_bMoved;
};

///////////////////////////////////////////////////////////////////////////////
// CQBrushVertex_Manipulator
///////////////////////////////////////////////////////////////////////////////

class CQBrushVertex_Manipulator : public CManipulator
{
public:

	CQBrushVertex_Manipulator();
	virtual ~CQBrushVertex_Manipulator();

	float size;
	vec3* pos;
	unsigned int color;
	unsigned int hlcolor;
	CQBrush* brush;

	static int DragVert( CManipulator* m, void* p, void* p2 );

	void OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );

	void OnDraw( CMapView* pView );

private:

	bool m_bHover;
	bool m_bVertDrag;
	bool m_bOnList;
	bool m_bSnapSel;
	bool m_bMoved;

	int m_numon;
	float m_lastx, m_lasty;
	CQBrushVertex_Manipulator** m_onlist;
};

#define MAX_BRUSH_PLANES	256
#define MAX_WINDING_POINTS	256
#define MAX_BRUSH_VERTS		1024
#define MAX_BRUSH_EDGES		1024

///////////////////////////////////////////////////////////////////////////////
// CQBrush
///////////////////////////////////////////////////////////////////////////////

//
// brush manipulation.
//

class CQBrush : public CMapObject
{
public:
	enum Type
	{
		TYPE_Q1,
		TYPE_Q2,
		TYPE_Q3
	};

	CQBrush(Type type);
	CQBrush( const CQBrush& b );
	virtual ~CQBrush();

	static int ExitBrushFaceMode( CMapObject* p, void* parm, void* parm2 );
	static int EnterBrushFaceMode( CMapObject* p, void* parm, void* parm2 );
	static int ExitBrushVertexMode( CMapObject* p, void* parm, void* parm2 );
	static int EnterBrushVertexMode( CMapObject* p, void* parm, void* parm2 );

	bool TouchesBox(const vec3 &mins, const vec3 &maxs);
	bool IsMicroBrush() const;

	void ScaleObject( const vec3& scale, const vec3& origin, const vec3& mins_before, const vec3& maxs_before, const vec3& mins_after, const vec3& maxs_after, CTreadDoc* pDoc );

	void GetWorldMinsMaxs( vec3* pMins, vec3* pMaxs );
	void GetObjectMinsMaxs( vec3* pMins, vec3* pMaxs );
	vec3 GetObjectWorldPos();

	void SetObjectWorldPos( const vec3& pos, CTreadDoc* pDoc );
	void SetObjectTransform( const mat3x3& m, CTreadDoc* pDoc );

	void OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	bool OnPopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );

	bool WriteToFile( CFile* pFile, CTreadDoc* pDoc, int nVersion );
	bool ReadFromFile( CFile* pFile, CTreadDoc* pDoc, int nVersion );

	void SetShaderName( const char* name, CTreadDoc* pDoc );
	void InternalSetShaderName(const char *name, CShader *shader, CTreadDoc *pDoc);
	void RepaintShader( const char* name, CTreadDoc* pDoc );

	int GetClass();
	int GetSubClass();

	void OnAddToMap( CTreadDoc* pDoc );
	void OnRemoveFromMap( CTreadDoc* pDoc );
	void OnAddToSelection( CTreadDoc* pDoc );

	void OnSetVisible( bool bVisible, CTreadDoc* pDoc );

	CMapObject* Clone();
	void CopyState( CMapObject* src, CTreadDoc* pDoc );

	int GetNumRenderMeshes( CMapView* pView );
	CRenderMesh* GetRenderMesh(int num, CMapView* pView);

	void OnVertexDrag();
	void TryWeldVerts( CTreadDoc* pDoc, float dist, bool ask = true );

	CLinkedList<CObjProp>* GetPropList( CTreadDoc* pDoc );
	void SetProp( CTreadDoc* pDoc, CObjProp* prop );

	void FlipObject( const vec3& origin, const vec3& axis );

	const char* GetRootName();
	void SelectByShader( const char* szShaderName, CTreadDoc* pDoc );

	void SnapToGrid( float fGridSize, bool x, bool y, bool z );

	//
	// called during linking/unlinking operations.
	//
	void UpdateOwnerContents( CTreadDoc* pDoc );

	void WriteToDXF( std::fstream& file, CTreadDoc* doc );

	int GetFilterState( CTreadDoc* doc, CObjectFilter* filter );

	int GetContents();
	int AddContents(int bits);
	int RemoveContents(int bits);
	int AddSurface(int bits);
	int RemoveSurface(int bits);
	int GetSurface();
	int ApplyShaderContents(bool selectedFacesOnly);
	int ApplyShaderSurface(bool selectedFacesOnly);
	void ApplyShaderValue(bool selectedFacesOnly);

	int GetObjectTypeBits();
	Type GetType() { return m_type; }

	bool CheckObject( CString& message, CTreadDoc* pDoc );

	static CMapObject* BrushFromPlaneFile( CTreadDoc* doc, C_Tokenizer& script );
	static CQBrush* ParseAbductedBrush( C_Tokenizer* script, CTreadDoc* pDoc );
	static CQBrush* ParseHalfLifeBrush( C_Tokenizer* script, CTreadDoc* pDoc );

	static CMapObject* MakeCube(CTreadDoc* doc, void* parm);
	static CMapObject* MakeCylinder( CTreadDoc* doc, void* parm );
	static CMapObject* MakeCone( CTreadDoc* doc, void* parm );

	struct BrushSelectAttributes
	{
		CTreadDoc* doc;
		int contents;
		int surface;
	};

	static int SelectBrushByAttributes( CMapObject* obj, void* p1, void* p2 );

	struct PlaneInfo
	{
		PlaneInfo() : p_shader(0), surface(0), contents(0), value(0) {}
		plane3 plane;
		CString s_shader;
		CShader *p_shader;
		int surface;
		int contents;
		int value;
		float rot;
		float shift[2];
		float scale[2];
	};

	static CQBrush* BrushFromPlanes(Type type, PlaneInfo *planes, int num_planes);
	
friend class CQBrushHandle_Manipulator;
friend class CQBrushVertex_Manipulator;
friend class CQuakeUserData;
friend class CQuakeMap;
friend class CQuake2Map;
friend class CQuakeGame;
friend class CQuake2Game;
friend class CQuake3Game;

protected:

	static int ms_NumCylinderSides;
	static int ms_NumConeSides;
	static int ms_HollowDepth;

	vec3 m_pos;
	vec3 m_mins, m_maxs;
	CRenderMesh m_mesh;
	bool m_texlock;
	bool m_bDrag;
	bool m_bTexDrag;
	bool m_bTexMoved;
	bool m_bTryWeld;
	bool m_bMoved;
	int  m_nMY;
	Type m_type;

	CLinkedList<CObjProp> m_PropList;

	CQBrushHandle_Manipulator* m_handle;

	struct BrushVert_t
	{
		int xyz;
		vec2 st;
	};
	
	int m_numxyz;
	vec3* m_xyz;

	class BrushFace_t : public CPickObject
	{
	public:
		BrushFace_t();
		virtual ~BrushFace_t();

		int texplane;		// -1 == autogen.
		int num_verts;
		BrushVert_t* verts;
		plane3 plane;

		CString s_shader;
		CShader* p_shader;

		int* edges; // one per edge ( == num_verts ).

		float shift[2];
		float scale[2];
		float rot;
		int surface;
		int contents;
		int value;

		CObjProp props[8];

		bool selected;
	};

	int m_numfaces;
	BrushFace_t* m_faces;

	int m_numvms;
	CQBrushVertex_Manipulator** m_vms;

	static void TextureAxisFromPlane( const plane3& pl, vec3* x, vec3* y);
	static int  HashBrushVert( vec3* xyz, int* num_verts, const vec3& hash_vert, float dist );
	static void ChopWinding( const plane3& pl, const vec3* in, int numin, vec3** out, int* numout, bool fliptest );
	static void SplitWinding( const plane3& pl, const vec3* in, int numin, vec3** outFront, int* numoutFront, vec3** outBack, int* numoutBack, bool fliptest );
	static bool WindingFromPlane( const plane3& pl, vec3* out );

	void TextureFaceFromPlane(BrushFace_t* face);
	void TextureBrush(const char* shader);
	
	typedef std::vector<CMapObject*> BrushList;
	static void GatherBrushes(CLinkedList<CMapObject> *cutBrushes, CLinkedList<CMapObject> *mapList, BrushList &list);
	static void HollowSelected(int depth, CTreadDoc *doc);
	static void CarveSelected(CTreadDoc *doc);
	
	bool SplitBrushByPlane(const PlaneInfo &plane, CQBrush **outFrontBrush, CQBrush **outBackBrush);	
	void Hollow(int depth, CLinkedList<CMapObject> *result, CTreadDoc *doc);
	void Carve(CQBrush *cutBrush, CLinkedList<CMapObject> *result, CTreadDoc *doc);
	void CarveBrushWithList(CLinkedList<CMapObject> *cutBrushes, CLinkedList<CMapObject> *result, CTreadDoc *doc);

	class BrushFaceChange_t
	{
	public:

		BrushFaceChange_t();
		virtual ~BrushFaceChange_t();

		float shift[2];
		float scale[2];
		float rot;

		CString s_shader;
	};

	static int BrushFaceChange( CMapObject* p, void* parm, void* parm2 );
	static int LockTexture( CMapObject* p, void* parm, void* parm2 );

	static int AreBrushFacesTexturedFromPlane( CMapObject* m, void* p, void* p2 );
	static int SetBrushFaceTexturePlane( CMapObject* m, void* p, void* p2 );

	static int AreBrushesLinkedToSameEntity( CMapObject* m, void* p, void* p2 );

	class CQBrushParmsMenu : public CObjectMenu
	{
	public:
		CQBrushParmsMenu();
		virtual ~CQBrushParmsMenu();

		CQBrush* active_brush;
		BrushFace_t* brush_face;
		CMapView* view;

		void OnUpdateCmdUI( int id, CCmdUI* pUI );
		void OnMenuItem( int id );
	};

	friend class CQBrush::CQBrushParmsMenu;

	void BuildInfo();
	void MakeRenderMesh();
	void UpdateRenderMeshTexture();
	void GenTXSpaceVecs();
	void UpdateRenderMeshXYZ();
	void ExitFaceMode();
	void EnterFaceMode();
	void DeselectFaces();
	void SelectFace( int i, bool select = true );
	void AttachFaces();
	void MakePlanes();

	void MakeVertexManipulators( CTreadDoc* pDoc );
	void FreeVertexManipulators( CTreadDoc* pDoc );

	void PaintFaceWithContents( BrushFace_t* f );
	void PaintFacesWithContents();

	//
	// called by CMapObject
	//
	void CopyObject( CMapObject* obj, CTreadDoc* pDoc );
};

///////////////////////////////////////////////////////////////////////////////
// CMakeBrushCube
///////////////////////////////////////////////////////////////////////////////

class CMakeBrushCube : public CObjectCreator
{
public:
	virtual void Release() { delete this; }
	virtual const char *Name() { return "Brushes\nCube"; }
	virtual CMapObject *CreateObject(CTreadDoc *doc) { return CQBrush::MakeCube(doc, 0); }
};

///////////////////////////////////////////////////////////////////////////////
// CMakeBrushCylinder
///////////////////////////////////////////////////////////////////////////////

class CMakeBrushCylinder : public CObjectCreator
{
public:
	virtual void Release() { delete this; }
	virtual const char *Name() { return "Brushes\nCylinder"; }
	virtual CMapObject *CreateObject(CTreadDoc *doc) { return CQBrush::MakeCylinder(doc, 0); }
};

///////////////////////////////////////////////////////////////////////////////
// CMakeBrushCone
///////////////////////////////////////////////////////////////////////////////

class CMakeBrushCone : public CObjectCreator
{
public:
	virtual void Release() { delete this; }
	virtual const char *Name() { return "Brushes\nCone"; }
	virtual CMapObject *CreateObject(CTreadDoc *doc) { return CQBrush::MakeCone(doc, 0); }
};

#endif