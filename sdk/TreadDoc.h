///////////////////////////////////////////////////////////////////////////////
// TreadDoc.h
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

#if !defined(AFX_ABDUCTEDDOC_H__7AC2BC30_1488_4204_987B_81C68115034A__INCLUDED_)
#define AFX_ABDUCTEDDOC_H__7AC2BC30_1488_4204_987B_81C68115034A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "System.h"
#include "afxmt.h"
#include <vector>

class CMapView;
class CChildFrame;
class CObjProp;
class CSplineControlPoint_Manipulator3D;
class CSplineTrack;
class CSplineSegment;
class CTreadDoc;

#define COMPILE_OPT_NOCSG				0x1
#define COMPILE_OPT_AGGRESSIVE_CSG		0x2
#define COMPILE_OPT_NOFILL				0x4
#define COMPILE_OPT_NOTJUNC				0x8
#define COMPILE_OPT_NOMERGE				0x10
#define COMPILE_OPT_ENTITIES_ONLY		0x20
#define COMPILE_OPT_NOSUBDIVIDE			0x40

#define RUN_OPT_NORMAL_PRIORITY			0x1

//
// return TRUE to terminate the walk
//
typedef bool (*MAPOBJECTLISTWALKPROC) ( CMapObject* object, CTreadDoc* doc, void* parm );

class OS_CLEXP CTreadDoc : public CDocument
{
protected: // create from serialization only
	CTreadDoc();
	DECLARE_DYNCREATE(CTreadDoc)

// Attributes
public:

	static int ReconnectEntity( CMapObject* pObject, void* p, void* p2 );
	void RepaintObjectsWithShader( const char* szShader );

	void OpenGroups( bool open = true );
	bool AreGroupsOpen( void );

	bool GetViewLightingFlag();
	void SetViewLightingFlag( bool view = true );
	bool GetViewShadowsFlag();
	void SetViewShadowsFlag( bool view = true );
	bool GetAnimateMaterialsFlag();
	void SetAnimateMaterialsFlag( bool animate = true );
	bool GetViewShadersFlag();
	void SetViewShadersFlag( bool view = true );

	CMapObject** GetObjectsForOwner( int owner_uid, int max, bool walk_objects, bool walk_selection );
	CMapObject** GetObjectsForGroup( int group_uid, int max, bool walk_objects, bool walk_selection );
	
	//
	// this is a somewhat algorithmically optimized search for objects with an id.
	// instead of walking the list numuid times, we walk the uid list at each object to see if we found them.
	//
	CMapObject** GetObjectsFromUIDs( int* uids, int numuids, bool walk_objects, bool walk_selection );

	//
	// These had better not fool with the lists in any way!
	// That means no selecting/deselecting objects!
	//
	void WalkUIDs( MAPOBJECTLISTWALKPROC callback, void* parm, int* uids, int numuids, bool walk_objects, bool walk_selection );
	void WalkObjects( MAPOBJECTLISTWALKPROC callback, void* parm, bool walk_objects, bool walk_selection );

	void GetLeakTrace(vec3** xyz, int* num);
	void SetLeakTrace(vec3 *xyz, int num);

	void CenterOnSelection();
	void CenterOnPosition( const vec3& pos );

	CObjectFilter* GetObjectFilter();

	void DeselectTypes( int classbits, int subclassbits );

	bool OnGainFocus();

	void GetEditSplineTrack( CSplineTrack** track, CSplineSegment** seg );
	void SetEditSplineTrack( CSplineTrack* track, CSplineSegment* seg );

	CSplineControlPoint_Manipulator3D** GetSpline3DManipulators();

	void SetRunReleaseBuildOfGame( bool release=true );
	bool GetRunReleaseBuildOfGame();

	void SetRunOpts( int opts );
	int GetRunOpts();

	void SetCompileOpts( int opts );
	int GetCompileOpts();

	vec3 m_selmins, m_selmaxs, m_selpos;
	
	CUndoRedoManager* GetUndoRedoManager();

	void SetMapCompiler(CPluginMapCompiler *compiler);
	CPluginMapCompiler *GetMapCompiler();

	void WriteToCompileWindow( const char* text, ... );

	//
	// duplicating a set of objects has one big problem:
	//
	// if the selected objects happen to be a linked set of brushes, then the
	// original linking information is destroyed because of the newly generated object UIDs.
	// to fix this problem, anytime a set of objects is duplicated ( for the purposes of cloning or pasting )
	// then this function should be called on the copy. It will relink the object uid's back-up with the
	// copied entity(s).
	//
	// Also duplicates any groups.
	//
	void FixupDuplication( CTreadDoc* original_doc, CLinkedList<CMapObject>* original, CLinkedList<CMapObject>* duplicate );

	void BuildSelectionBounds();
	void UpdateSelectionInterface();

	void SetChildFrame( CChildFrame* pFrame );
	CChildFrame* GetChildFrame( void );
	
	CLinkedList<CMapObject>* GetObjectList();
	CLinkedList<CMapObject>* GetSelectedObjectList();
	CLinkedList<CManipulator>* GetManipulatorList();
	CLinkedList<CManipulator>* GetSelectedManipulatorList();
	CLinkedList<CObjectGroup>* GetObjectGroupList();

	void SetTextureFilter( const char* szFilter );
	CString* GetTextureFilterHistory( int* num );
	void SetTextureFilterHistory( CString* filters, int num );
	CString GetTextureFilter();

	void ReconnectEntities();

	void DetachObject( CMapObject* pObject );
	void DetachManipulator( CManipulator* pMan );

	void AddObjectToSelection( CMapObject* pObject );
	void AddObjectToMap( CMapObject* pObject );
	void AddManipulatorToMap( CManipulator* pM );
	void AddManipulatorToSelection( CManipulator* pM );
	void PaintSelection();

#define NUDGE_UP		0x1
#define NUDGE_DOWN		0x2
#define NUDGE_LEFT		0x4
#define NUDGE_RIGHT		0x8

	void NudgeSelection( int dirbits ); // uses the active view.
	float GetNudgeFactor();
	void SetNudgeFactor( float nudge );

	bool DrawSelBox();
		
	CLinkedList<CMapObject>* CloneSelection();
	void CloneSelectionInPlace();

	bool ClearSelection();
	void DeleteSelection();
	void ClearSelectedManipulators();
	bool ObjectsAreSelected( int objclass = MAPOBJ_CLASS_ALL, int subclass = MAPOBJ_CLASS_ALL );
	int  GetSelectedObjectCount( int objclass = MAPOBJ_CLASS_ALL, int subclass = MAPOBJ_CLASS_ALL );
	int  GetObjectCount( int objclass = MAPOBJ_CLASS_ALL, int subclass = MAPOBJ_CLASS_ALL );

	bool IsInTrackAnimationMode();
	void SetInTrackAnimationMode( bool editing = true );

	bool IsEditingFaces();
	bool IsEditingVerts();
	void SetEditingFaces( bool editing = true );
	void SetEditingVerts( bool editing = true );

	void Prop_UpdateSelection();
	void Prop_PropChange( const char* prop );
	void Prop_SelectProp( const char* name );
	
	void Prop_AddObject( CMapObject* obj );
	void Prop_RemoveObject( CMapObject* obj );
	void Prop_AddGroup( CObjectGroup* obj );
	void Prop_RemoveGroup( CObjectGroup* obj );
	void Prop_UpdateObjectState( CMapObject* obj );
	void Prop_UpdateGroupState( CObjectGroup* obj );
	void Prop_SelectGroup( CObjectGroup* obj );

	void AddUndoItem( CUndoRedoAction* item );
	void ClearUndoStack();
	CGenericUndoRedoAction* GenericUndoRedoFromSelection();
	void MakeUndoDeselectAction();
	void MakeUndoCreateAction( const char* title, CMapObject* obj );
	void MakeUndoDeleteAction();
	void MakeUndoLinkAction( CEntity* entity );
	void MakeUndoUnlinkAction( CEntity* entity );

	void RemoveNullGroups();

	CString MakeUniqueObjectName( CMapObject* obj );
	CString MakeUniqueGroupName( CObjectGroup* gr );

	void AssignUID( CMapObject* pObj );
	void AssignUID( CObjectGroup* pGroup );
	
	int  GetUIDOffset();
	void SetUIDOffset( int ofs );

	void FlipSelection( const vec3& origin, const vec3& axis );

	CMapObject* ObjectForUID( int uid );
	CObjectGroup* GroupForUID( int uid );

	CObjectGroup* GroupSelection();
	void RemoveSelectionFromGroup();

	void ClearAllTrackPicks();
	void KillViewTrackPicks( CPickObject* obj );
	void SelectObjectsByShader( const char* szShaderName );

	void SnapSelectionToGrid( float fGridSize, bool x, bool y, bool z );

	CObjectCreator *CurrentObjectCreator() const;
	void SetCurrentObjectCreator(CObjectCreator *creator);
	CLinkedList<CObjectCreator> *ObjectCreatorList();

	CShader *SelectedShader() const;
	void SetSelectedShader(CShader *shader);
	const char *SelectedShaderName() const;
	const char *SelectedShaderDisplayName() const;
	CShader *ShaderForName(const char *name);
	CShader *ShaderForDisplayName(const char *name);
	CLinkedList<CShader> *ShaderList();

	CPluginGame *GamePlugin() const;
	CGameDef    *GameDef() const;
	bool SetupGameDef(bool allowNewGame);

	std::vector<CManipulator*> m_manips;

	void SetUserData(void *data);
	void *UserData() const;
	void OnFileExportmap(CPluginFileExport *exporter);
	void UpdateTitle();

	void ShowCompileWindow( bool show = true );
	bool IsCompileWindowVisible();

	bool ExportMapForCompile(char *outmappath, int bufLen);
	void GetMapExportPathName(char *buff, int buffLen);
	void GetMapExportName(char *buff, int buffLen);

private:

	CString m_importTitle;
	bool m_created;

	void *m_userData;
	CGameDef *m_gameDef;

	CShader *m_selShader;
	CPluginGame *m_gamePlugin;

	CObjectCreator *m_selCreator;
	CLinkedList<CObjectCreator> m_creators;

	static CTreadDoc* m_spAnimDoc;

	bool m_bToggleBrushes;
	bool m_bToggleJmodels;
	bool m_bOpenGroups;
	bool m_bViewLighting;
	bool m_bViewShadows;
	bool m_bAnimateMaterials;
	bool m_bViewShaders;

	int m_compile_opts;
	int m_run_opts;

	int m_numleakpts;
	vec3* m_leakpts;

//	void LoadLeakFile( const char* filename );

	CObjectFilter m_objfilter;

	struct FilterData_t
	{
		CObjectFilter* filter;
		CTreadDoc* doc;
	};

	static int ApplyObjectFilter( CMapObject* obj, void* p1, void* p2 );
	void OnObjectFilterChange();

	bool m_bEditTrackAnimation;
	CSplineControlPoint_Manipulator3D* m_spline_manips[3];
	CSplineTrack* m_editsplinetrack;
	CSplineSegment* m_editsplinetrackseg;	
	CChildFrame* m_pChildFrame;

#define MAX_TEXTURE_FILTER_HISTORY	32

	int m_nNumTextureFilters;
	CString* m_psTextureFilters;
	CString m_sTextureFilter;

	float m_fNudgeFactor;

	void AddTextureFilterToList( const char* filter );

	int m_nNextUID;
	
	bool m_bEditFaces;
	bool m_bEditVerts;
	bool m_bReleaseBuildOfGame;

	CLinkedList<CMapObject> m_ObjectList;
	CLinkedList<CMapObject> m_SelectedObjectList;
	CLinkedList<CManipulator> m_ManipulatorList;
	CLinkedList<CManipulator> m_SelectedManipulatorList;
	CLinkedList<CObjectGroup> m_ObjectGroupList;

	struct ObjectCount_t
	{
		int objclass;
		int objsubclass;

		int count;
		bool singlefind;
	};

	static int ObjectCounter( CMapObject* pObject, void* parm, void* parm2 );

	struct ObjectNameCheck_t
	{
		CString sName;
		bool ok;
	};

	static int CheckGroupNames( CObjectGroup* pGroup, void* p, void* );
	static int CheckObjectNames( CMapObject* pObject, void* p, void* );
	static int FindObjectFromUID( CMapObject* pObject, void* p, void* p2 );
	static int FindGroupFromUID( CObjectGroup* pGroup, void* p, void* p2 );
	
	void DeleteManipulators();
	
	void UpdateScaleMode();
	void UpdateDragMode();
	void UpdateVertMode();

	void SetPickTypeFlags( int flags );

	//
	// helpers for the FixupDuplication function.
	//
	static int FindUIDPositionInList( int uid, CLinkedList<CMapObject>* list );

	vec3 m_vertmanpos;

	CUndoRedoManager m_undoredo;

	//
	// compiler stuff.
	//
	CCriticalSection m_compileCS;
	CPluginMapCompiler *m_compiler;
	void KillCompile();

	CConsoleDialog* m_pCompileWindow;

	bool ExportMapFile( CString filename );
	bool ExportBrushFile( CString filename, bool selected );

	bool SaveIfNoPath();
	bool SetupForCompile( const char* map );

	vec3 m_scale_gizmo_mins, m_scale_gizmo_maxs;

	struct WalkUIDData_t
	{
		MAPOBJECTLISTWALKPROC callback;
		void* parm;
		int* uids;
		int numuids;
		int numfound;
	};

	struct GetObjectFromUIDData_t
	{
		int num;
		int total;
		CMapObject** objs;
	};

	struct GetObjectForGroupOwnerData_t
	{
		int num;
		int total;
		int uid;
		CMapObject** objs;
	};

	struct WriteBrushMapData_t
	{
		std::fstream* stream;
		bool selected;
		CTreadDoc* doc;
	};

	static int  UIDCompare( const void* p1, const void* p2 );
	static bool WalkObjectUIDProc( CMapObject* obj, CTreadDoc* pDoc, void* parm );
	static bool WalkGetObjectFromUID( CMapObject* obj, CTreadDoc* pDoc, void* parm );
	static bool WalkGetObjectForOwner( CMapObject* obj, CTreadDoc* pDoc, void* parm );
	static bool WalkGetObjectForGroup( CMapObject* obj, CTreadDoc* pDoc, void* parm );
	static int  WalkWriteBrushMap( CMapObject* obj, void* parm, void* parm2 );

	void AnimateMaterials();

	static int RepaintObjectWithShaderProc( CMapObject* obj, void* parm, void* parm2 );

	static bool WalkShowObjectsWithBits( CMapObject* obj, CTreadDoc* pDoc, void* parm );
	static bool WalkHideObjectsWithBits( CMapObject* obj, CTreadDoc* pDoc, void* parm );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreadDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreadDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTreadDoc)
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditDeselect();
	afx_msg void OnUpdateEditDeselect(CCmdUI* pCmdUI);
	afx_msg void OnToolsHide();
	afx_msg void OnUpdateToolsHide(CCmdUI* pCmdUI);
	afx_msg void OnToolsHideallingroup();
	afx_msg void OnUpdateToolsHideallingroup(CCmdUI* pCmdUI);
	afx_msg void OnToolsMakegroup();
	afx_msg void OnUpdateToolsMakegroupandhide(CCmdUI* pCmdUI);
	afx_msg void OnToolsRotate180();
	afx_msg void OnUpdateToolsRotate180(CCmdUI* pCmdUI);
	afx_msg void OnToolsRotate90ccw();
	afx_msg void OnUpdateToolsRotate90ccw(CCmdUI* pCmdUI);
	afx_msg void OnToolsRotate90cw();
	afx_msg void OnUpdateToolsRotate90cw(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectallingroups();
	afx_msg void OnUpdateToolsSelectallingroups(CCmdUI* pCmdUI);
	afx_msg void OnToolsShowallingroups();
	afx_msg void OnUpdateToolsShowallingroups(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsMakegroup(CCmdUI* pCmdUI);
	afx_msg void OnToolsFliphorizontal();
	afx_msg void OnUpdateToolsFliphorizontal(CCmdUI* pCmdUI);
	afx_msg void OnToolsFliponxaxis();
	afx_msg void OnUpdateToolsFliponxaxis(CCmdUI* pCmdUI);
	afx_msg void OnToolsFliponyaxis();
	afx_msg void OnUpdateToolsFliponyaxis(CCmdUI* pCmdUI);
	afx_msg void OnToolsFliponzaxis();
	afx_msg void OnUpdateToolsFliponzaxis(CCmdUI* pCmdUI);
	afx_msg void OnToolsFlipvertical();
	afx_msg void OnUpdateToolsFlipvertical(CCmdUI* pCmdUI);
	afx_msg void OnToolsMakegroupandhide();
	afx_msg void OnToolsRemovefromgroups();
	afx_msg void OnUpdateToolsRemovefromgroups(CCmdUI* pCmdUI);
	afx_msg void OnToolsCompilemap();
	afx_msg void OnUpdateToolsCompilemap(CCmdUI* pCmdUI);
	afx_msg void OnToolsCompilerunmap();
	afx_msg void OnUpdateToolsCompilerunmap(CCmdUI* pCmdUI);
	afx_msg void OnToolsRunmap();
	afx_msg void OnUpdateToolsRunmap(CCmdUI* pCmdUI);
	afx_msg void OnToolsAbortactivecompile();
	afx_msg void OnUpdateToolsAbortactivecompile(CCmdUI* pCmdUI);
	afx_msg void OnViewMapcompileroutput();
	afx_msg void OnUpdateViewMapcompileroutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileExportmap(CCmdUI* pCmdUI);
	afx_msg void OnToolsRunabducted();
	afx_msg void OnToolsBrushfilterAlwaysshadow();
	afx_msg void OnUpdateToolsBrushfilterAlwaysshadow(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterAreaportal();
	afx_msg void OnUpdateToolsBrushfilterAreaportal(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterCameraclip();
	afx_msg void OnUpdateToolsBrushfilterCameraclip(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterCoronablock();
	afx_msg void OnUpdateToolsBrushfilterCoronablock(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterDetail();
	afx_msg void OnUpdateToolsBrushfilterDetail(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterFilterall();
	afx_msg void OnUpdateToolsBrushfilterFilterall(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterFilterallclip();
	afx_msg void OnUpdateToolsBrushfilterFilterallclip(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterFilterallsky();
	afx_msg void OnUpdateToolsBrushfilterFilterallsky(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterFilternone();
	afx_msg void OnUpdateToolsBrushfilterFilternone(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterMonsteclip();
	afx_msg void OnUpdateToolsBrushfilterMonsteclip(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterNodraw();
	afx_msg void OnUpdateToolsBrushfilterNodraw(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterNoshadow();
	afx_msg void OnUpdateToolsBrushfilterNoshadow(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterPlayerclip();
	afx_msg void OnUpdateToolsBrushfilterPlayerclip(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterReapplyfilter();
	afx_msg void OnUpdateToolsBrushfilterReapplyfilter(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterSkybox();
	afx_msg void OnUpdateToolsBrushfilterSkybox(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterSkyportal();
	afx_msg void OnUpdateToolsBrushfilterSkyportal(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterSolid();
	afx_msg void OnUpdateToolsBrushfilterSolid(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterWater();
	afx_msg void OnUpdateToolsBrushfilterWater(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrushfilterWindow();
	afx_msg void OnUpdateToolsBrushfilterWindow(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesAlwaysshadow();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesAlwaysshadow(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesAreaportal();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesAreaportal(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesCameraclip();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesCameraclip(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesCoronablock();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesCoronablock(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesDetail();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesDetail(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesMonsterclip();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesMonsterclip(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesNodraw();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesNodraw(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesNoshadow();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesNoshadow(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesPlayerclip();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesPlayerclip(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesSkybox();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesSkybox(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesSkyportal();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesSkyportal(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesSolid();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesSolid(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesWater();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesWater(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectbrushesbyattributesWindow();
	afx_msg void OnUpdateToolsSelectbrushesbyattributesWindow(CCmdUI* pCmdUI);
	afx_msg void OnToolsHideall();
	afx_msg void OnUpdateToolsHideall(CCmdUI* pCmdUI);
	afx_msg void OnToolsShowall();
	afx_msg void OnUpdateToolsShowall(CCmdUI* pCmdUI);
	afx_msg void OnToolsAnimationmode();
	afx_msg void OnUpdateToolsAnimationmode(CCmdUI* pCmdUI);
	afx_msg void OnToolsCenteronselection();
	afx_msg void OnUpdateToolsCenteronselection(CCmdUI* pCmdUI);
	afx_msg void OnToolsCheckmapforerrors();
	afx_msg void OnUpdateToolsCheckmapforerrors(CCmdUI* pCmdUI);
	afx_msg void OnToolsCheckselectedobjects();
	afx_msg void OnUpdateToolsCheckselectedobjects(CCmdUI* pCmdUI);
	afx_msg void OnToolsViewleaktrace();
	afx_msg void OnUpdateToolsViewleaktrace(CCmdUI* pCmdUI);
	afx_msg void OnToolsReloadleaktrace();
	afx_msg void OnUpdateToolsReloadleaktrace(CCmdUI* pCmdUI);
	afx_msg void OnFileExportbrushfile();
	afx_msg void OnUpdateFileExportbrushfile(CCmdUI* pCmdUI);
	afx_msg void OnFileExportselectedtobrushfile();
	afx_msg void OnUpdateFileExportselectedtobrushfile(CCmdUI* pCmdUI);
	afx_msg void OnToolsInsertjmodelscenefile();
	afx_msg void OnToolsViewlighting();
	afx_msg void OnUpdateToolsViewlighting(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsAnimatematerials(CCmdUI* pCmdUI);
	afx_msg void OnToolsAnimatematerials();
	afx_msg void OnToolsEnableshadows();
	afx_msg void OnUpdateToolsEnableshadows(CCmdUI* pCmdUI);
	afx_msg void OnToolsDisplayshaders();
	afx_msg void OnUpdateToolsDisplayshaders(CCmdUI* pCmdUI);
	afx_msg void OnFileImportplane();
	afx_msg void OnUpdateFileImportplane(CCmdUI* pCmdUI);
	afx_msg void OnToolsReloadlighttextures();
	afx_msg void OnToolsWriteshaderfiles();
	afx_msg void OnFileImportcg();
	afx_msg void OnToolsSelectobjectbyuid();
	afx_msg void OnViewVertices();
	afx_msg void OnViewFaces();
	afx_msg void OnUpdateViewFaces(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewVertices(CCmdUI* pCmdUI);
	afx_msg void OnViewTogglebrushes();
	afx_msg void OnViewHideshowjmodels();
	afx_msg void OnSelectionOpengroupmode();
	afx_msg void OnUpdateSelectionOpengroupmode(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI *pCmdUI);
	afx_msg void OnEditWorldspawn();
	afx_msg void OnUpdateEditWorldspawn(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABDUCTEDDOC_H__7AC2BC30_1488_4204_987B_81C68115034A__INCLUDED_)
