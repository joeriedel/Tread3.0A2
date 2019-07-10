///////////////////////////////////////////////////////////////////////////////
// splinetrack.h
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

#ifndef SPLINETRACK_H
#define SPLINETRACK_H

#include "system.h"
#include "vec3d_manipulator.h"

#define CTRLA_POINT			0
#define CTRLA_HANDLE		1
#define CTRLB_POINT			3
#define CTRLB_HANDLE		2

//
// all times/ticks are in milliseconds.
// the track control can only handle things at a 10th of a second.
//
class CSplineSegment : public CPickObject, public CLLObject
{
public:

	CSplineSegment();
	CSplineSegment( const CSplineSegment& s );
	virtual ~CSplineSegment();

	vec3 ctrls[4];
	int start;	// start tick.
	int end;		// end tick.
	int ticks;	// number of ticks.
	int length;

	bool rebuild;

	void MakeRenderMesh( CRenderMesh* mesh );
};

class CSplineKeyFrame : public CLLObject
{
public:

	CSplineKeyFrame();
	CSplineKeyFrame( const CSplineKeyFrame& f );
	virtual ~CSplineKeyFrame();
	
	vec3 v;
	CString s;
	
	int ticks;
};

class CSplineControlPoint_Manipulator;
class CSplineControlPoint_Manipulator3D : public CVec3D_Manipulator
{
private:

	bool m_bMoved;

public:

	CSplineControlPoint_Manipulator* src;

	CSplineControlPoint_Manipulator3D();
	virtual ~CSplineControlPoint_Manipulator3D();

	void OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	bool OnDrag( CMapView* pView, int nButtons, const vec3& move );
};

class CSplineControlPoint_Manipulator : public CManipulator
{
private:

	bool m_bHover;
	bool m_bMoved;
	bool m_bDrag;

public:

	CSplineControlPoint_Manipulator();
	virtual ~CSplineControlPoint_Manipulator();

	bool m_bHandle; // == true means that its a vector manipulator.

	float size;
	vec3* drag_pt;
	vec3* drag_pt2;
	vec3* t_pt;
	vec3* t_pt2;
	vec3* reflect_pt; // for handle draggers.

	unsigned int color, hlcolor;

	CSplineSegment* segA, *segB;

	void OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );

	void OnDraw( CMapView* pView );
};

#define KEYFRAME_ROTS		0
#define KEYFRAME_FOVS		1
#define KEYFRAME_EVENTS		2

#define KEYFRAME_MASK_VEC	0x1
#define KEYFRAME_MASK_EVENT 0x2
#define KEYFRAME_MASK_TICKS 0x4
#define KEYFRAME_MASK_ALL	0xFF

class CSplineTrack : public CMapObject
{
friend class CSplineControlPoint_Manipulator;
private:

	CSplineKeyFrame* FindKeyFrame( CLinkedList<CSplineKeyFrame>* list, int ticks );
	void GetTickBounds( CLinkedList<CSplineKeyFrame>* list, int ticks, CSplineKeyFrame** front, CSplineKeyFrame** back );

	CLinkedList<CSplineSegment>  m_segs;

	//
	// rotational segment curve, used to auto-smooth rotation keyframes.
	//
	CLinkedList<CSplineKeyFrame> m_rots;
	CLinkedList<CSplineKeyFrame> m_fovs;
	CLinkedList<CSplineKeyFrame> m_events;

	void CopySegs( CLinkedList<CSplineSegment>* list );
	void CopyKeyFrames( CLinkedList<CSplineKeyFrame>* src, CLinkedList<CSplineKeyFrame>* dst );

	void FreeMeshes();
	void BuildMeshes();

	void EvaluateCurve3D( vec3* ctrls, vec3* out, float f );
	void EvaluateCurve1D( float* ctrls, float* out, float f );
	
	CSplineSegment* SegmentForTicks( CLinkedList<CSplineSegment>* segs, int ticks );
	float TemperalForTicks( CSplineSegment* seg, float ticks );
	void GetRotationBorders( CLinkedList<CSplineKeyFrame>* rots, int ticks, vec3* start, vec3* end, float* t );
	void GetFovBorders( CLinkedList<CSplineKeyFrame>* fovs, int ticks, float* start, float* end, float* t );

	CLinkedList<CSplineSegment>* GetKeyCurveSegs( int type );

	CSplineSegment* m_selected_seg;

	void SelectSegment( CSplineSegment* seg );
	void SplitSegment( CSplineSegment* seg );
	void DeleteSegment( CSplineSegment* seg );

	//
	// one mesh per segment.
	//
	bool m_motion;
	int m_mesh_count;
	CRenderMesh* m_meshes;

	vec3 m_pos;

	int m_num_manips;
	CSplineControlPoint_Manipulator** m_manips;

	void FreeManipulators( CTreadDoc* doc );
	void MakeManipulators( CTreadDoc* doc );

	static void Clear3DManipulators( CTreadDoc* doc );

	bool m_bDrag;

	class CSplineParmsMenu : public CObjectMenu
	{
	public:

		CSplineParmsMenu();
		virtual ~CSplineParmsMenu();
		
		CSplineTrack* track;
		CSplineSegment* seg;
		CMapView* view;

		void OnUpdateCmdUI( int id, CCmdUI* pUI );
		void OnMenuItem( int id );
	};

	friend class CSplineTrack::CSplineParmsMenu;
	static CSplineParmsMenu m_SplineMenu;
	static bool m_bMenuCreated;
	static void MakeSplineMenu();

	//
	// our property list, that holds a single property.
	//
	CLinkedList<CObjProp> m_Props;
	CObjProp* m_NameProp;

	bool WriteKeyframeList( CLinkedList<CSplineKeyFrame>* list, CFile* file, CTreadDoc* doc, int nVersion );
	bool ReadKeyframeList( CLinkedList<CSplineKeyFrame>* list, CFile* file, CTreadDoc* doc, int nVersion );

public:

	CSplineTrack();
	CSplineTrack( const CSplineTrack& t );
	virtual ~CSplineTrack();

	CLinkedList<CSplineKeyFrame>* GetKeyFrames( int type );
	void AddKeyFrame( int ticks, vec3 v, const char* event, int type );
	void RemoveKeyFrame( int ticks, int type );
	void SetKeyFrameInfo( int ticks, int new_ticks, vec3 v, const char* event, int type, int flags = KEYFRAME_MASK_ALL );
	void EvaluateSpline( int ticks, vec3* pos, vec3* rot, float* fov );
	bool KeyFrameExists( int ticks, int type );
	bool HasMotion();
	void SetMotion( bool motion = true );

	//
	// returns total milliseconds that this spline runs for.
	//
	int GetTotalTicks();
	
	//
	// this will cause the spline to dynamically resize all keyframes and splines.
	//
	void ScaleTimes( float scale );

	//
	// computes spline start/end times based on spline lengths.
	void CalcSplineTimes();

	void GetWorldMinsMaxs( vec3* pMins, vec3* pMaxs );
	void GetObjectMinsMaxs( vec3* pMins, vec3* pMaxs );
	vec3 GetObjectWorldPos();
	void SetObjectWorldPos( const vec3& pos, CTreadDoc* pDoc );

	void OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	void OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	bool OnPopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );

	void OnAddToMap( CTreadDoc* pDoc );
	void OnRemoveFromMap( CTreadDoc* pDoc );
	void OnAddToSelection( CTreadDoc* pDoc );

	bool WriteToFile( CFile* pFile, CTreadDoc* pDoc, int nVersion );
	bool ReadFromFile( CFile* pFile, CTreadDoc* pDoc, int nVersion );

	CLinkedList<CObjProp>* GetPropList( CTreadDoc* pDoc );
	void SetProp(  CTreadDoc* pDoc, CObjProp* prop );

	int GetNumRenderMeshes( CMapView* pView );
	CRenderMesh* GetRenderMesh( int num, CMapView* pView );

	int GetClass();
	const char* GetRootName();

	CMapObject* Clone();
	void CopyState( CMapObject* src, CTreadDoc* pDoc );

	void WriteToMapFile( std::fstream& file, CTreadDoc* doc );

	static CMapObject* MakeSplineTrack( CTreadDoc* doc, void* parm );
};

#endif