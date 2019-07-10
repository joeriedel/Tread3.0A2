///////////////////////////////////////////////////////////////////////////////
// TreadDoc.cpp
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
#include "Tread.h"
#include "ObjPropView.h"
#include "ChildFrm.h"
#include "MapView.h"

#include "TreadDoc.h"
#include "ConsoleDialog.h"
#include "TrackAnimDialog.h"

#include "SelScale_Manipulator.h"
#include "SelDrag3D_Manipulator.h"
#include "SelScale3D_Manipulator.h"
#include "MainFrm.h"
#include "MapFile.h"
#include "ents.h"
#include "files.h"
#include "map_build.h"
#include "CheckMapDialog.h"
#include "c_tokenizer.h"
#include "InputLineDialog.h"
#include "ChooseGameType.h"
#include "ObjectPropertiesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int s_numDocs = 0;

/////////////////////////////////////////////////////////////////////////////
// CTreadDoc

IMPLEMENT_DYNCREATE(CTreadDoc, CDocument)

BEGIN_MESSAGE_MAP(CTreadDoc, CDocument)
	//{{AFX_MSG_MAP(CTreadDoc)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_DESELECT, OnEditDeselect)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DESELECT, OnUpdateEditDeselect)
	ON_COMMAND(ID_TOOLS_HIDE, OnToolsHide)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_HIDE, OnUpdateToolsHide)
	ON_COMMAND(ID_TOOLS_HIDEALLINGROUP, OnToolsHideallingroup)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_HIDEALLINGROUP, OnUpdateToolsHideallingroup)
	ON_COMMAND(ID_TOOLS_MAKEGROUP, OnToolsMakegroup)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MAKEGROUPANDHIDE, OnUpdateToolsMakegroupandhide)
	ON_COMMAND(ID_TOOLS_ROTATE180, OnToolsRotate180)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ROTATE180, OnUpdateToolsRotate180)
	ON_COMMAND(ID_TOOLS_ROTATE90CCW, OnToolsRotate90ccw)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ROTATE90CCW, OnUpdateToolsRotate90ccw)
	ON_COMMAND(ID_TOOLS_ROTATE90CW, OnToolsRotate90cw)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ROTATE90CW, OnUpdateToolsRotate90cw)
	ON_COMMAND(ID_TOOLS_SELECTALLINGROUPS, OnToolsSelectallingroups)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTALLINGROUPS, OnUpdateToolsSelectallingroups)
	ON_COMMAND(ID_TOOLS_SHOWALLINGROUPS, OnToolsShowallingroups)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SHOWALLINGROUPS, OnUpdateToolsShowallingroups)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MAKEGROUP, OnUpdateToolsMakegroup)
	ON_COMMAND(ID_TOOLS_FLIPHORIZONTAL, OnToolsFliphorizontal)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FLIPHORIZONTAL, OnUpdateToolsFliphorizontal)
	ON_COMMAND(ID_TOOLS_FLIPONXAXIS, OnToolsFliponxaxis)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FLIPONXAXIS, OnUpdateToolsFliponxaxis)
	ON_COMMAND(ID_TOOLS_FLIPONYAXIS, OnToolsFliponyaxis)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FLIPONYAXIS, OnUpdateToolsFliponyaxis)
	ON_COMMAND(ID_TOOLS_FLIPONZAXIS, OnToolsFliponzaxis)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FLIPONZAXIS, OnUpdateToolsFliponzaxis)
	ON_COMMAND(ID_TOOLS_FLIPVERTICAL, OnToolsFlipvertical)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FLIPVERTICAL, OnUpdateToolsFlipvertical)
	ON_COMMAND(ID_TOOLS_MAKEGROUPANDHIDE, OnToolsMakegroupandhide)
	ON_COMMAND(ID_TOOLS_REMOVEFROMGROUPS, OnToolsRemovefromgroups)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_REMOVEFROMGROUPS, OnUpdateToolsRemovefromgroups)
	ON_COMMAND(ID_TOOLS_COMPILEMAP, OnToolsCompilemap)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_COMPILEMAP, OnUpdateToolsCompilemap)
	ON_COMMAND(ID_TOOLS_COMPILERUNMAP, OnToolsCompilerunmap)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_COMPILERUNMAP, OnUpdateToolsCompilerunmap)
	ON_COMMAND(ID_TOOLS_RUNMAP, OnToolsRunmap)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_RUNMAP, OnUpdateToolsRunmap)
	ON_COMMAND(ID_TOOLS_ABORTACTIVECOMPILE, OnToolsAbortactivecompile)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ABORTACTIVECOMPILE, OnUpdateToolsAbortactivecompile)
	ON_COMMAND(ID_VIEW_MAPCOMPILEROUTPUT, OnViewMapcompileroutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MAPCOMPILEROUTPUT, OnUpdateViewMapcompileroutput)
	ON_COMMAND(ID_TOOLS_RUNABDUCTED, OnToolsRunabducted)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_ALWAYSSHADOW, OnToolsBrushfilterAlwaysshadow)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_ALWAYSSHADOW, OnUpdateToolsBrushfilterAlwaysshadow)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_AREAPORTAL, OnToolsBrushfilterAreaportal)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_AREAPORTAL, OnUpdateToolsBrushfilterAreaportal)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_CAMERACLIP, OnToolsBrushfilterCameraclip)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_CAMERACLIP, OnUpdateToolsBrushfilterCameraclip)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_CORONABLOCK, OnToolsBrushfilterCoronablock)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_CORONABLOCK, OnUpdateToolsBrushfilterCoronablock)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_DETAIL, OnToolsBrushfilterDetail)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_DETAIL, OnUpdateToolsBrushfilterDetail)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_FILTERALL, OnToolsBrushfilterFilterall)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_FILTERALL, OnUpdateToolsBrushfilterFilterall)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_FILTERALLCLIP, OnToolsBrushfilterFilterallclip)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_FILTERALLCLIP, OnUpdateToolsBrushfilterFilterallclip)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_FILTERALLSKY, OnToolsBrushfilterFilterallsky)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_FILTERALLSKY, OnUpdateToolsBrushfilterFilterallsky)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_FILTERNONE, OnToolsBrushfilterFilternone)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_FILTERNONE, OnUpdateToolsBrushfilterFilternone)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_MONSTECLIP, OnToolsBrushfilterMonsteclip)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_MONSTECLIP, OnUpdateToolsBrushfilterMonsteclip)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_NODRAW, OnToolsBrushfilterNodraw)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_NODRAW, OnUpdateToolsBrushfilterNodraw)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_NOSHADOW, OnToolsBrushfilterNoshadow)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_NOSHADOW, OnUpdateToolsBrushfilterNoshadow)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_PLAYERCLIP, OnToolsBrushfilterPlayerclip)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_PLAYERCLIP, OnUpdateToolsBrushfilterPlayerclip)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_REAPPLYFILTER, OnToolsBrushfilterReapplyfilter)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_REAPPLYFILTER, OnUpdateToolsBrushfilterReapplyfilter)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_SKYBOX, OnToolsBrushfilterSkybox)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_SKYBOX, OnUpdateToolsBrushfilterSkybox)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_SKYPORTAL, OnToolsBrushfilterSkyportal)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_SKYPORTAL, OnUpdateToolsBrushfilterSkyportal)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_SOLID, OnToolsBrushfilterSolid)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_SOLID, OnUpdateToolsBrushfilterSolid)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_WATER, OnToolsBrushfilterWater)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_WATER, OnUpdateToolsBrushfilterWater)
	ON_COMMAND(ID_TOOLS_BRUSHFILTER_WINDOW, OnToolsBrushfilterWindow)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSHFILTER_WINDOW, OnUpdateToolsBrushfilterWindow)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_ALWAYSSHADOW, OnToolsSelectbrushesbyattributesAlwaysshadow)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_ALWAYSSHADOW, OnUpdateToolsSelectbrushesbyattributesAlwaysshadow)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_AREAPORTAL, OnToolsSelectbrushesbyattributesAreaportal)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_AREAPORTAL, OnUpdateToolsSelectbrushesbyattributesAreaportal)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_CAMERACLIP, OnToolsSelectbrushesbyattributesCameraclip)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_CAMERACLIP, OnUpdateToolsSelectbrushesbyattributesCameraclip)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_CORONABLOCK, OnToolsSelectbrushesbyattributesCoronablock)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_CORONABLOCK, OnUpdateToolsSelectbrushesbyattributesCoronablock)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_DETAIL, OnToolsSelectbrushesbyattributesDetail)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_DETAIL, OnUpdateToolsSelectbrushesbyattributesDetail)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_MONSTERCLIP, OnToolsSelectbrushesbyattributesMonsterclip)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_MONSTERCLIP, OnUpdateToolsSelectbrushesbyattributesMonsterclip)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_NODRAW, OnToolsSelectbrushesbyattributesNodraw)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_NODRAW, OnUpdateToolsSelectbrushesbyattributesNodraw)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_NOSHADOW, OnToolsSelectbrushesbyattributesNoshadow)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_NOSHADOW, OnUpdateToolsSelectbrushesbyattributesNoshadow)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_PLAYERCLIP, OnToolsSelectbrushesbyattributesPlayerclip)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_PLAYERCLIP, OnUpdateToolsSelectbrushesbyattributesPlayerclip)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SKYBOX, OnToolsSelectbrushesbyattributesSkybox)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SKYBOX, OnUpdateToolsSelectbrushesbyattributesSkybox)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SKYPORTAL, OnToolsSelectbrushesbyattributesSkyportal)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SKYPORTAL, OnUpdateToolsSelectbrushesbyattributesSkyportal)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SOLID, OnToolsSelectbrushesbyattributesSolid)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SOLID, OnUpdateToolsSelectbrushesbyattributesSolid)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_WATER, OnToolsSelectbrushesbyattributesWater)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_WATER, OnUpdateToolsSelectbrushesbyattributesWater)
	ON_COMMAND(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_WINDOW, OnToolsSelectbrushesbyattributesWindow)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_WINDOW, OnUpdateToolsSelectbrushesbyattributesWindow)
	ON_COMMAND(ID_TOOLS_HIDEALL, OnToolsHideall)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_HIDEALL, OnUpdateToolsHideall)
	ON_COMMAND(ID_TOOLS_SHOWALL, OnToolsShowall)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SHOWALL, OnUpdateToolsShowall)
	ON_COMMAND(ID_TOOLS_ANIMATIONMODE, OnToolsAnimationmode)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ANIMATIONMODE, OnUpdateToolsAnimationmode)
	ON_COMMAND(ID_TOOLS_CENTERONSELECTION, OnToolsCenteronselection)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CENTERONSELECTION, OnUpdateToolsCenteronselection)
	ON_COMMAND(ID_TOOLS_CHECKMAPFORERRORS, OnToolsCheckmapforerrors)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CHECKMAPFORERRORS, OnUpdateToolsCheckmapforerrors)
	ON_COMMAND(ID_TOOLS_CHECKSELECTEDOBJECTS, OnToolsCheckselectedobjects)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CHECKSELECTEDOBJECTS, OnUpdateToolsCheckselectedobjects)
	ON_COMMAND(ID_TOOLS_VIEWLEAKTRACE, OnToolsViewleaktrace)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_VIEWLEAKTRACE, OnUpdateToolsViewleaktrace)
	ON_COMMAND(ID_TOOLS_RELOADLEAKTRACE, OnToolsReloadleaktrace)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_RELOADLEAKTRACE, OnUpdateToolsReloadleaktrace)
	ON_COMMAND(ID_FILE_EXPORTBRUSHFILE, OnFileExportbrushfile)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTBRUSHFILE, OnUpdateFileExportbrushfile)
	ON_COMMAND(ID_FILE_EXPORTSELECTEDTOBRUSHFILE, OnFileExportselectedtobrushfile)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTSELECTEDTOBRUSHFILE, OnUpdateFileExportselectedtobrushfile)
	ON_COMMAND(ID_TOOLS_INSERTJMODELSCENEFILE, OnToolsInsertjmodelscenefile)
	ON_COMMAND(ID_TOOLS_VIEWLIGHTING, OnToolsViewlighting)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_VIEWLIGHTING, OnUpdateToolsViewlighting)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ANIMATEMATERIALS, OnUpdateToolsAnimatematerials)
	ON_COMMAND(ID_TOOLS_ANIMATEMATERIALS, OnToolsAnimatematerials)
	ON_COMMAND(ID_TOOLS_ENABLESHADOWS, OnToolsEnableshadows)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ENABLESHADOWS, OnUpdateToolsEnableshadows)
	ON_COMMAND(ID_TOOLS_DISPLAYSHADERS, OnToolsDisplayshaders)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_DISPLAYSHADERS, OnUpdateToolsDisplayshaders)
	ON_COMMAND(ID_FILE_IMPORTPLANE, OnFileImportplane)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORTPLANE, OnUpdateFileImportplane)
	ON_COMMAND(ID_TOOLS_RELOADLIGHTTEXTURES, OnToolsReloadlighttextures)
	ON_COMMAND(ID_TOOLS_WRITESHADERFILES, OnToolsWriteshaderfiles)
	ON_COMMAND(ID_TOOLS_SELECTOBJECTBYUID, OnToolsSelectobjectbyuid)
	ON_COMMAND(ID_VIEW_VERTICES, OnViewVertices)
	ON_COMMAND(ID_VIEW_FACES, OnViewFaces)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FACES, OnUpdateViewFaces)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VERTICES, OnUpdateViewVertices)
	ON_COMMAND(ID_VIEW_TOGGLEBRUSHES, OnViewTogglebrushes)
	ON_COMMAND(ID_VIEW_HIDESHOWJMODELS, OnViewHideshowjmodels)
	ON_COMMAND(ID_SELECTION_OPENGROUPMODE, OnSelectionOpengroupmode)
	ON_UPDATE_COMMAND_UI(ID_SELECTION_OPENGROUPMODE, OnUpdateSelectionOpengroupmode)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_VIEW_PROPERTIES, &CTreadDoc::OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, &CTreadDoc::OnUpdateViewProperties)
	ON_COMMAND(ID_EDIT_WORLDSPAWN, &CTreadDoc::OnEditWorldspawn)
	ON_UPDATE_COMMAND_UI(ID_EDIT_WORLDSPAWN, &CTreadDoc::OnUpdateEditWorldspawn)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreadDoc construction/destruction

CTreadDoc* CTreadDoc::m_spAnimDoc = 0;

CTreadDoc::CTreadDoc()
{
	// TODO: add one-time construction code here
	m_pChildFrame = 0;
	m_bEditFaces = false;
	m_bEditVerts = false;
	m_nNextUID = 0;

	m_nNumTextureFilters = 0;
	m_psTextureFilters = new CString[MAX_TEXTURE_FILTER_HISTORY];
	m_sTextureFilter = "";

	m_fNudgeFactor = 1.0f;

	m_pCompileWindow = 0;
	m_compiler = 0;

	m_bReleaseBuildOfGame = true;

	m_spline_manips[0] = 
	m_spline_manips[1] =
	m_spline_manips[2] = 0;

	m_bEditTrackAnimation = false;
	m_editsplinetrack = 0;
	m_editsplinetrackseg = 0;

	m_leakpts = 0;
	m_numleakpts = 0;
	m_compile_opts = 0;
	m_run_opts = 0;

	m_bViewLighting = false;
	m_bViewShadows = true;
	m_bAnimateMaterials = false;
	m_bViewShaders = true;

	m_bOpenGroups = false;
	m_bToggleBrushes = false;
	m_bToggleJmodels = false;

	m_selCreator = 0;
	m_selShader = 0;

	m_gamePlugin = 0;
	m_gameDef = 0;
	m_userData = 0;

	m_created = false;
}

CTreadDoc::~CTreadDoc()
{
	if( m_psTextureFilters )
		delete[] m_psTextureFilters;
	if( m_pCompileWindow )
		delete m_pCompileWindow;

	if( Sys_GetMainFrame() && Sys_GetMainFrame()->m_ClipBoardDoc == this )
	{
		Sys_GetMainFrame()->m_ClipBoard.DestroyList();
	}

	if( m_leakpts )
		delete[] m_leakpts;

	m_bAnimateMaterials = false;

	if( this == m_spAnimDoc )
	{
		m_spAnimDoc = 0;
	}

	{
		CObjectCreator *c;
		while(c=m_creators.RemoveItem(LL_HEAD))
		{
			c->Release();
		}
	}
}

void CTreadDoc::UpdateTitle()
{
	if (m_importTitle.GetLength() > 0)
	{
		SetTitle(m_importTitle);
		SetPathName(m_importTitle, FALSE);
	}
}

void CTreadDoc::SetUserData(void *data)
{
	m_userData = data;
}

void *CTreadDoc::UserData() const
{
	return m_userData;
}

bool CTreadDoc::SetupGameDef(bool allowNewGame)
{
	CPluginFileImport *importer = Sys_Importer();
	if (importer)
	{
		if (m_gamePlugin)
		{
			m_gamePlugin->ReleaseUserData(this);
		}

		m_gamePlugin = Sys_FindPluginGame(importer->PluginGame());
		if (!m_gamePlugin)
		{
			CString s;
			s.Format("%s importer requires a plugin for a %s file which could not be found!", importer->Type(), importer->PluginGame());
			OS_OkAlertMessage("Error", s);
			return false;
		}

		// now find a game that matches, if we have multiple games that match, ask the user which one they want to use.
		int matches = 0;
		for (CGameDef *def = Sys_GameDefs()->ResetPos(); def; def = Sys_GameDefs()->GetNextItem())
		{
			if (!strcmp(def->PluginGame(), m_gamePlugin->Name()))
			{
				++matches;
				m_gameDef = def;
			}
		}

		if (!m_gameDef)
		{
			CString msg;
			msg.Format("Unable to find a game definition to use for importing a %s file. Would you like to select a game type for this map?", importer->Type(), Sys_GameDef());
			if (MessageBox(0, msg, "Missing Game Definition!", MB_TASKMODAL|MB_YESNO) != IDYES) return false;
			CChooseGameType dlg;
			if (dlg.DoModal() != IDOK) return false;
			m_gameDef = Sys_FindGameDef(dlg.GameType());
			OS_ASSERT(m_gameDef);
		}
		else if (matches > 1) // ask the user.
		{
			CChooseGameType dlg(importer->PluginGame());
			dlg.SetWindowTextA("Choose Game Type For File Import:");
			if (dlg.DoModal() != IDOK) return false;
			m_gameDef = Sys_FindGameDef(dlg.GameType());
			OS_ASSERT(m_gameDef);
		}
	}
	else
	{
		m_gameDef = Sys_FindGameDef(Sys_GameDef());
		if (!m_gameDef)
		{
			if (allowNewGame)
			{
				CString msg;
				msg.Format("Unable to find the game definition for '%s'. Would you like to select a different game type for this map?", Sys_GameDef());
				Sys_SetGameDef(0);
				if (MessageBox(0, msg, "Missing Game Definition!", MB_TASKMODAL|MB_YESNO) != IDYES) return false;
				CChooseGameType dlg;
				if (dlg.DoModal() != IDOK) return false;
				m_gameDef = Sys_FindGameDef(dlg.GameType());
				OS_ASSERT(m_gameDef);
			}
			else
			{
				CString msg;
				msg.Format("Unable to find the game definition for '%s'. This map could not be opened as it may contain objects that cannot be serialized without the specified game.", Sys_GameDef());
				MessageBox(0, msg, "Missing Game Definition!", MB_TASKMODAL|MB_OK);
				return false;
			}
		}
		Sys_SetGameDef(0);

		if (m_gamePlugin)
		{
			m_gamePlugin->ReleaseUserData(this);
		}

		m_gamePlugin = Sys_FindPluginGame(m_gameDef->PluginGame());
		OS_ASSERT(m_gamePlugin);
	}

	m_gamePlugin->BindUserData(this);
	m_selCreator = 0;

	for (CPlugin *plugin = Sys_Plugins()->ResetPos(); plugin; plugin = Sys_Plugins()->GetNextItem())
	{
		for (int i = 0; ; ++i)
		{
			CObjectCreator *c = plugin->ObjectCreator(i);
			if (!c) break;
			if (!m_selCreator) { m_selCreator = c; }
			m_creators.AddItem(c);
		}
	}

	for (int i = 0; ; ++i)
	{
		CObjectCreator *c = m_gamePlugin->ObjectCreator(i);
		if (!c) break;
		if (!m_selCreator) { m_selCreator = c; }
		m_creators.AddItem(c);
	}

	for (CEntDef *def = m_gameDef->EntDefList()->ResetPos(); def; def = m_gameDef->EntDefList()->GetNextItem())
	{
		CObjectCreator *c = def->MakeObjectFactory();
		if (c)
		{
			if (!m_selCreator) { m_selCreator = c; }
			m_creators.AddItem(c);
		}
	}

	CLinkedList<CShader> *shaders = ShaderList();
	if (shaders)
	{
		for (CShader *shader = shaders->ResetPos(); shader; shader = shaders->GetNextItem())
		{
			if (shader->Pickable()) 
			{
				SetSelectedShader(shader);
				break;
			}
		}
	}

	return true;
}

CGameDef *CTreadDoc::GameDef() const
{
	return m_gameDef;
}

CShader *CTreadDoc::SelectedShader() const
{
	return m_selShader;
}

const char *CTreadDoc::SelectedShaderName() const
{
	if (m_selShader) { return m_selShader->Name(); }
	return 0;
}

const char *CTreadDoc::SelectedShaderDisplayName() const
{
	if (m_selShader) { return m_selShader->DisplayName(); }
	return 0;
}

void CTreadDoc::SetSelectedShader(CShader *shader)
{
	if (m_selShader)
	{
		m_selShader->selected = false;
	}

	m_selShader = shader;

	if (m_selShader)
	{
		m_selShader->selected = true;
	}
}

CLinkedList<CShader> *CTreadDoc::ShaderList()
{
	if (m_gamePlugin)
	{
		return m_gamePlugin->ShaderList();
	}
	return 0;
}

CShader *CTreadDoc::ShaderForDisplayName(const char *name)
{
	CLinkedList<CShader> *list = ShaderList();

	if (list)
	{
		for (CShader *s = list->ResetPos(); s; s = list->GetNextItem())
		{
			if (!strcmp(s->DisplayName(), name)) return s;
		}
	}

	return 0;
}

CShader *CTreadDoc::ShaderForName(const char *name)
{
	CLinkedList<CShader> *list = ShaderList();

	if (list)
	{
		for (CShader *s = list->ResetPos(); s; s = list->GetNextItem())
		{
			if (!strcmp(s->Name(), name)) return s;
		}
	}

	return 0;
}

CPluginGame *CTreadDoc::GamePlugin() const
{
	return m_gamePlugin;
}

CObjectCreator *CTreadDoc::CurrentObjectCreator() const
{
	return m_selCreator;
}

void CTreadDoc::SetCurrentObjectCreator(CObjectCreator *creator)
{
	m_selCreator = creator;
}

CLinkedList<CObjectCreator> *CTreadDoc::ObjectCreatorList()
{
	return &m_creators;
}

bool CTreadDoc::WalkShowObjectsWithBits( CMapObject* obj, CTreadDoc* pDoc, void* parm )
{
	int bits = (int)parm;

	if( obj->IsVisible() == false )
	{
		if( obj->GetObjectTypeBits()&bits )
		{
			obj->SetVisible( pDoc );
			pDoc->Prop_UpdateObjectState( obj );
		}
	}

	return false;
}

bool CTreadDoc::WalkHideObjectsWithBits( CMapObject* obj, CTreadDoc* pDoc, void* parm )
{
	int bits = (int)parm;

	if( obj->IsVisible() == true )
	{
		if( obj->GetObjectTypeBits()&bits )
		{
			obj->SetVisible( pDoc, false );
			pDoc->Prop_UpdateObjectState( obj );
		}
	}

	return false;
}


void CTreadDoc::OpenGroups( bool open )
{
	m_bOpenGroups = open;
}

bool CTreadDoc::AreGroupsOpen( void )
{
	return m_bOpenGroups;
}

bool CTreadDoc::GetViewShadersFlag()
{
	return m_bViewShaders;
}

void CTreadDoc::SetViewShadersFlag( bool view )
{
	m_bViewShaders = view;
}

bool CTreadDoc::GetViewLightingFlag()
{
	return m_bViewLighting;
}

void CTreadDoc::SetViewLightingFlag( bool view )
{
	m_bViewLighting = view;
}

bool CTreadDoc::GetViewShadowsFlag()
{
	return m_bViewShadows;
}

void CTreadDoc::SetViewShadowsFlag( bool view )
{
	m_bViewShadows = view;
}

bool CTreadDoc::GetAnimateMaterialsFlag()
{
	return m_bAnimateMaterials;
}

void CTreadDoc::SetAnimateMaterialsFlag( bool animate )
{
	m_bAnimateMaterials = animate;
}

int CTreadDoc::WalkWriteBrushMap( CMapObject* obj, void* parm, void* parm2 )
{
	/*WriteBrushMapData_t* data = (WriteBrushMapData_t*)parm;

	if( data->selected )
	{
		if( obj->IsSelected() == false )
			return 0;
	}

	CQBrush* b = dynamic_cast<CQBrush*>(obj);
	if( !b )
		return 0;
	
	b->WriteToDXF( *data->stream, data->doc );*/
	return 0;
}

void CTreadDoc::SetRunOpts( int opts )
{
	m_run_opts = opts;
}

int CTreadDoc::GetRunOpts()
{
	return m_run_opts;
}

void CTreadDoc::SetCompileOpts( int opts )
{
	m_compile_opts = opts;
}

int CTreadDoc::GetCompileOpts()
{
	return m_compile_opts;
}

int CTreadDoc::UIDCompare( const void* p1, const void* p2 )
{
	int a = *((int*)p1);
	int b = *((int*)p2);

	return ( a<b ) ? -1 : ( a>b ) ? 1 : 0;
}

bool CTreadDoc::WalkGetObjectFromUID( CMapObject* obj, CTreadDoc* pDoc, void* parm )
{
	GetObjectFromUIDData_t* od = (GetObjectFromUIDData_t*)parm;

	od->objs[od->num++] = obj;

	return od->num == od->total;
}

bool CTreadDoc::WalkGetObjectForOwner( CMapObject* obj, CTreadDoc* pDoc, void* parm )
{
	GetObjectForGroupOwnerData_t* d = (GetObjectForGroupOwnerData_t*)parm;

	if( obj->GetOwnerUID() == d->uid )
	{
		d->objs[d->num++] = obj;
		if( d->num == d->total )
			return true;
	}

	return false;
}

bool CTreadDoc::WalkGetObjectForGroup( CMapObject* obj, CTreadDoc* pDoc, void* parm )
{
	GetObjectForGroupOwnerData_t* d = (GetObjectForGroupOwnerData_t*)parm;

	if( obj->GetGroupUID() == d->uid )
	{
		d->objs[d->num++] = obj;
		if( d->num == d->total )
			return true;
	}

	return false;
}

CMapObject** CTreadDoc::GetObjectsForOwner( int owner_uid, int max, bool walk_objects, bool walk_selection )
{
	if( max < 1 )
		return 0;
	if( owner_uid == -1 )
		return 0;

	CMapObject** objs;
	GetObjectForGroupOwnerData_t d;

	objs = new CMapObject*[max];
	memset( objs, 0, sizeof(CMapObject*)*max );

	d.num = 0;
	d.total = max;
	d.uid = owner_uid;
	d.objs = objs;

	WalkObjects( WalkGetObjectForOwner, &d, walk_objects, walk_selection );

	return objs;
}

CMapObject** CTreadDoc::GetObjectsForGroup( int group_uid, int max, bool walk_objects, bool walk_selection )
{
	if( max < 1 )
		return 0;
	if( group_uid == -1 )
		return 0;

	CMapObject** objs;
	GetObjectForGroupOwnerData_t d;

	objs = new CMapObject*[max];
	memset( objs, 0, sizeof(CMapObject*)*max );

	d.num = 0;
	d.total = max;
	d.uid = group_uid;
	d.objs = objs;

	WalkObjects( WalkGetObjectForGroup, &d, walk_objects, walk_selection );

	return objs;
}

CMapObject** CTreadDoc::GetObjectsFromUIDs( int* uids, int numuids, bool walk_objects, bool walk_selection )
{
	GetObjectFromUIDData_t od;
	CMapObject** objs;
	
	if( numuids < 1 || uids == 0 )
		return 0;

	objs = new CMapObject*[numuids];
	memset( objs, 0, sizeof(CMapObject*)*numuids );

	od.num = 0;
	od.objs = objs;
	od.total = numuids;

	WalkUIDs( WalkGetObjectFromUID, &od, uids, numuids, walk_objects, walk_selection );

	return objs;
}

bool CTreadDoc::WalkObjectUIDProc( CMapObject* obj, CTreadDoc* pDoc, void* parm )
{
	WalkUIDData_t* wd = (WalkUIDData_t*)parm;
	int i;
	int uid = obj->GetUID();

	//
	// binary search the array.
	//
	int low = 0;
	int high = wd->numuids-1;
	int mid = (high+low)>>1;

	//
	// out of range?
	//
	if( uid < wd->uids[0] || uid > wd->uids[wd->numuids-1] )
		return false;

walkobject_binsearchuid:

	if( (high-low) <= 4 ) // four entries to search?
	{
		//
		// linear search from here.
		//
		for(i = low; i <= high; i++)
		{
			if( wd->uids[i] == uid )
			{
				//
				// found it.
				//
				wd->numfound++;
				if( wd->callback( obj, pDoc, wd->parm ) )
					return true;

				break;
			}
		}
	}
	else
	if( uid > wd->uids[mid] )
	{
		//
		// move to upper half.
		//
		low = mid+1;
		mid = (high+low)>>1;
		goto walkobject_binsearchuid;
	}
	else
	{
		//
		// binary search bottom half.
		//
		high = mid;
		mid = (high+low)>>1;
		goto walkobject_binsearchuid;
	}

	return wd->numfound == wd->numuids;
}

void CTreadDoc::WalkUIDs( MAPOBJECTLISTWALKPROC callback, void* parm, int* uids, int numuids, bool walk_objects, bool walk_selection )
{
	if( numuids < 1 || uids == 0 )
		return;

	WalkUIDData_t wd;
	int* sorted_uids;

	//
	// sort the uids for binary search.
	//
	sorted_uids = new int[numuids];
	memcpy( sorted_uids, uids, sizeof(int)*numuids );
	qsort( sorted_uids, numuids, sizeof(int), UIDCompare );

	wd.callback = callback;
	wd.numuids = numuids;
	wd.parm = parm;
	wd.uids = sorted_uids;
	wd.numfound = 0;

	WalkObjects( WalkObjectUIDProc, &wd, walk_objects, walk_selection );

	delete[] sorted_uids;
}

void CTreadDoc::WalkObjects( MAPOBJECTLISTWALKPROC callback, void* parm, bool walk_objects, bool walk_selection )
{
	CMapObject* obj;

	if( walk_objects )
	{
		for(obj = m_ObjectList.ResetPos(); obj; obj = m_ObjectList.GetNextItem() )
		{
			if( callback( obj, this, parm ) )
				return;
			m_ObjectList.SetPosition( obj );
		}
	}

	if( walk_selection )
	{
		for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
		{
			if( callback( obj, this, parm ) )
				return;
			m_SelectedObjectList.SetPosition( obj );
		}
	}
}

void CTreadDoc::CenterOnSelection()
{
	if( !ObjectsAreSelected() )
		return;

	CenterOnPosition( m_selpos );
}

void CTreadDoc::CenterOnPosition( const vec3& pos )
{
	int i;
	CMapView* v;

	for(i = 0; i < 4; i++)
	{
		v = GetChildFrame()->GetMapView( i );
		if( v && (v->GetViewType()&VIEW_FLAG_MAP) )
		{
			if( v->GetViewType() == VIEW_TYPE_3D )
			{
				v->View.pos3d = pos - vec3( 0, 256, 0 );
				v->View.or3d.set_angles( vec3( 0, 0, 90 ) );
			}
			else
			{
				v->View.fMapCenterX = pos[QUICK_AXIS(v->View.or2d.lft)];
				v->View.fMapCenterY = pos[QUICK_AXIS(v->View.or2d.up)];
				v->CalcSize();
			}

			v->RedrawWindow();
		}
	}
}

CObjectFilter* CTreadDoc::GetObjectFilter()
{
	return &m_objfilter;
}

int CTreadDoc::ApplyObjectFilter( CMapObject* obj, void* p1, void* p2 )
{
	FilterData_t* f = (FilterData_t*)p1;

	//
	// determine if we should show/hide or do anything to this object.
	//
	int fs = obj->GetFilterState( f->doc, f->filter );
	
	switch( fs )
	{
	case OBJFILTER_SHOW:
		obj->SetVisible( f->doc, true );
	break;
	case OBJFILTER_HIDE:
		if( obj->IsSelected() )
			obj->Deselect(f->doc);
		obj->SetVisible( f->doc, false );
	break;
	}

	f->doc->Prop_UpdateObjectState( obj );
	return 0;
}

void CTreadDoc::OnObjectFilterChange()
{
	FilterData_t fd;

	fd.doc = this;
	fd.filter = &m_objfilter;

	m_ObjectList.WalkList( ApplyObjectFilter, &fd );
	m_SelectedObjectList.WalkList( ApplyObjectFilter, &fd );

	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

bool CTreadDoc::OnGainFocus()
{
	if (m_created)
	{
		Sys_GetPropView()->SetDoc( this );
		Sys_GetPropView()->LoadDocument();
		Prop_UpdateSelection();

		SetInTrackAnimationMode( IsInTrackAnimationMode() );
		Sys_GetMainFrame()->GetCheckMapDialog()->ShowWindow(SW_HIDE);
		return true;
	}
	return false;
}

void CTreadDoc::GetEditSplineTrack( CSplineTrack** track, CSplineSegment** seg )
{
	if( track )
		*track = m_editsplinetrack;
	if( seg )
		*seg = m_editsplinetrackseg;
}

void CTreadDoc::SetEditSplineTrack( CSplineTrack* track, CSplineSegment* seg )
{
	m_editsplinetrack = track;
	m_editsplinetrackseg = seg;

	if( IsInTrackAnimationMode() )
	{
		/*if( GetSelectedObjectCount(MAPOBJ_CLASS_SPLINETRACK) > 1 )
		{
			Sys_GetMainFrame()->GetTrackAnimDialog()->Load( 0, 0, this );
		}
		else
		{
			Sys_GetMainFrame()->GetTrackAnimDialog()->Load( m_editsplinetrack, m_editsplinetrackseg, this );
		}*/
	}
}

bool CTreadDoc::IsInTrackAnimationMode()
{
	return m_bEditTrackAnimation;
}

void CTreadDoc::SetInTrackAnimationMode( bool editing )
{
	editing = false;
	CTrackAnimDialog* td = Sys_GetMainFrame()->GetTrackAnimDialog();

	if( ((bool)td->IsWindowVisible()) != editing )
		td->ShowWindow( (editing)?SW_SHOW:SW_HIDE );

	if( editing )
	{
		Sys_GetMainFrame()->GetTrackAnimDialog()->Load( m_editsplinetrack, m_editsplinetrackseg, this );
		
		/*int i;
		for(i = 0; i < 4; i++)
		{
			GetChildFrame()->GetMapView(i)->View.or3d.set_angles( vec3::zero );
		}*/
	}
	else
	{
		int i;
		for(i = 0; i < 4; i++)
		{
			GetChildFrame()->GetMapView(i)->View.fov = 70.0f;
		}
	}

	m_bEditTrackAnimation = editing;
}

CSplineControlPoint_Manipulator3D** CTreadDoc::GetSpline3DManipulators()
{
	return m_spline_manips;
}


void CTreadDoc::SetRunReleaseBuildOfGame( bool release )
{
	m_bReleaseBuildOfGame = release;
}

bool CTreadDoc::GetRunReleaseBuildOfGame()
{
	return m_bReleaseBuildOfGame;
}

bool CTreadDoc::SaveIfNoPath()
{
	if( GetPathName() != "" )
		return true;

	MessageBox( 0, "You can't compile or run a map without saving it first!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );

	return false;
}

void CTreadDoc::GetLeakTrace( vec3** xyz, int* num )
{
	*xyz = m_leakpts;
	*num = m_numleakpts;
}

void CTreadDoc::SetLeakTrace(vec3 *xyz, int num)
{
	if (m_leakpts) { delete[] m_leakpts; m_leakpts = 0; }
	m_numleakpts = num;
	if (num && xyz)
	{
		m_leakpts = new vec3[num];
		memcpy(m_leakpts, xyz, sizeof(vec3)*num);
	}
}

//bool CTreadDoc::LeakFileExists( const char* filename )
//{
//	if( !filename || !filename[0] )
//		return false;
//
//	//
//	// the destructor will close the file.
//	//
//	CFile f;
//
//	if( f.Open( filename, CFile::modeRead|CFile::typeBinary ) == false )
//		return false;
//
//	if( f.GetLength() < 1 )
//		return false;
//
//	return true;
//}
//
//void CTreadDoc::LoadLeakFile( const char* filename )
//{
//	CFile f;
//	char* data;
//	int len;
//
//	if( m_leakpts )
//		delete[] m_leakpts;
//
//	m_leakpts = 0;
//	m_numleakpts = 0;
//
//	if( !filename || !filename[0] )
//		return;
//
//	if( f.Open( filename, CFile::modeRead|CFile::typeBinary ) == false )
//		return;
//
//	len = f.GetLength();
//	if( len < 1 )
//		return;
//
//	data = new char[len];
//
//	f.Read( data, len );
//	f.Close();
//
//	C_Tokenizer sc;
//	CString t;
//
//	if( sc.InitParsing( data, len ) )
//	{
//		delete[] data; 
//		return;
//	}
//
//	delete[] data; 
//
//	for(;;)
//	{
//		if( sc.GetToken(t) || sc.GetToken(t) || sc.GetToken(t) )
//			break;
//
//		m_numleakpts++;
//	}
//
//	if( m_numleakpts < 2 )
//	{
//		m_numleakpts = 0;
//		return;
//	}
//
//	sc.RestartParsing();
//	m_leakpts = new vec3[m_numleakpts];
//
//	int i;
//	for(i = 0; i < m_numleakpts; i++)
//	{
//		sc.GetFloat( &m_leakpts[i].x );
//		sc.GetFloat( &m_leakpts[i].y );
//		sc.GetFloat( &m_leakpts[i].z );
//	}
//
//	sc.FreeScript();
//}

void CTreadDoc::GetMapExportPathName(char *buff2, int buffLen)
{
	CString path = GetPathName();
	char buff[1024];
	FS_FileFromPath(path, buff);
	buff2[0] = 0;
	GamePlugin()->GetExportFile(buff, buff2, buffLen);
}

void CTreadDoc::GetMapExportName(char *buff3, int buffLen)
{
	buff3[0] = 0;
	char buff[1024], buff2[1024];
	GetMapExportPathName(buff, 1024);
	buff2[0] = 0;
	FS_FileFromPath(buff, buff2);
	buff3[buffLen] = 0;
	strncpy(buff3, buff2, buffLen);
}

bool CTreadDoc::SetupForCompile( const char* map )
{
	if(m_compiler)
	{
		int id = MessageBox( 0, "You currently have a compile running for this map. Would you like to abort it?", "Abort Compilation", MB_TASKMODAL|MB_OKCANCEL|MB_ICONQUESTION );
		if( id == IDCANCEL )
			return false;

		KillCompile();
	}

	//
	// open the console window.
	//
	ShowCompileWindow();
	m_pCompileWindow->Clear();
	CPluginFileExport *exporter = GamePlugin()->NativeMapExporter();
	bool s = false;
	if (exporter)
	{
		s = exporter->Export(map, true, this);
		exporter->Release();
	}
	return s;
}

bool CTreadDoc::IsCompileWindowVisible()
{
	return (m_pCompileWindow != 0) && m_pCompileWindow->IsWindowVisible();
}

void CTreadDoc::ShowCompileWindow( bool show )
{
	if( !m_pCompileWindow )
	{
		m_pCompileWindow = new CConsoleDialog();
		m_pCompileWindow->Create( IDD_CONSOLE_DIALOG, Sys_GetMainFrame() );
		m_pCompileWindow->SetWindowText( "Map Compile" );
		m_pCompileWindow->AllowHide( TRUE );
	}

	if( show )
		m_pCompileWindow->ShowWindow( SW_SHOW );
	else
		m_pCompileWindow->ShowWindow( SW_HIDE );
}

void CTreadDoc::WriteToCompileWindow( const char* text, ... )
{
	va_list va;
	va_start(va, text);
	CString s;
	s.FormatV(text, va);
	va_end(va);

	if( m_pCompileWindow )
		m_pCompileWindow->WriteText( s );
}

void CTreadDoc::SetMapCompiler( CPluginMapCompiler* compiler )
{
	m_compileCS.Lock();
	m_compiler = compiler;
	m_compileCS.Unlock();
}

CPluginMapCompiler* CTreadDoc::GetMapCompiler()
{
	return m_compiler;
}

bool CTreadDoc::ExportMapFile( CString filename )
{
	//
	// export our user data.
	//
	OnSaveDocument( filename );
	return true;
}

void CTreadDoc::KillCompile()
{
	m_compileCS.Lock();
	if (m_compiler)
	{
		m_compiler->Abort();
		m_compiler = 0;
	}
	m_compileCS.Unlock();
}

int CTreadDoc::FindUIDPositionInList( int uid, CLinkedList<CMapObject>* list )
{
	int c = 0;
	CMapObject* obj;

	for( obj = list->ResetPos(); obj; obj = list->GetNextItem() )
	{
		if( obj->GetUID() == uid )
			return c;

		c++;
	}

	return -1;
}

void CTreadDoc::FixupDuplication( CTreadDoc* original_doc, CLinkedList<CMapObject>* original, CLinkedList<CMapObject>* duplicate )
{
	//
	// find an entity that owns some objects.
	//
	CEntity* ent;
	CMapObject* obj, *ent_pos;
	int i, *uids, num, pos;
	
	ent_pos = 0;

	for( ;; )
	{
		ent = 0;

		if( ent_pos != 0 )
		{
			duplicate->SetPosition( ent_pos );
			duplicate->GetNextItem();
		}
		else
			duplicate->ResetPos();

		for( obj = duplicate->GetItem(LL_CURRENT); obj; obj = duplicate->GetNextItem() )
		{
			ent = dynamic_cast<CEntity*>(obj);
			if( ent )
			{
				if( ent->GetOwnedObjectUIDs(&num) != 0 )
				{
					ent_pos = ent;
					break;
				}

				ent = 0;
			}
		}

		if( !ent )
			break;

		//
		// this entity owns objects. find the duplicated object positions.
		//
		uids = ent->GetOwnedObjectUIDs( &num );
		
		//
		// the duplication list objects do *not* contain the original uids.
		// they are the new ones. we must look up their originals' list position, and then
		// cross reference by that.
		//
		for(i = 0; i < num; i++)
		{
			pos = original_doc->FindUIDPositionInList( uids[i], original );
			if( pos != -1 )
			{
				obj = duplicate->GetItem( pos );
				if( obj )
				{
					//
					// replace uids.
					//
					uids[i] = obj->GetUID();
					obj->SetOwnerUID( ent->GetUID() );
				}
			}
		}
	}

	//
	// Duplicate groups.
	//
	int guid;
	CObjectGroup* gr;
	CLinkedList<CObjectGroup> group_list;
	CLinkedList<CMapObject> object_list;

	for( obj = duplicate->ResetPos(); obj; obj = duplicate->GetNextItem() )
	{
		guid = obj->GetGroupUID();
		if( guid != -1 )
		{
			//
			// find it in the group list.
			//
			for( gr = group_list.ResetPos(); gr; gr = group_list.GetNextItem() )
			{
				if( gr->GetUID() == guid )
					break;
			}

			if( gr )
				continue;

			gr = new CObjectGroup();
			gr->SetUID( guid ); // for now.
			group_list.AddItem( gr );
		}
	}

	//
	// remap objects.
	//
	for( gr = group_list.ResetPos(); gr; )
	{
		group_list.RemoveItem( gr );
		guid = gr->GetUID();
		AssignUID( gr );

		for( obj = duplicate->ResetPos(); obj; )
		{
			if( obj->GetGroupUID() == guid )
			{
				duplicate->RemoveItem( obj );
				object_list.AddItem( obj );
				obj->SetGroupUID( gr->GetUID() );
				obj = duplicate->GetCurItem();
			}
			else
			{
				obj = duplicate->GetNextItem();
			}
		}

		//
		// own this list.
		//
		gr->MakeList( &object_list );
		gr->SetName( MakeUniqueGroupName( gr ) );

		//
		// transfer back.
		//
		for( obj = object_list.ResetPos(); obj; )
		{
			object_list.RemoveItem( obj );
			duplicate->AddItem( obj );
			obj = object_list.GetCurItem();
		}
	
		m_ObjectGroupList.AddItem( gr );
		Prop_AddGroup( gr );
		gr = group_list.GetCurItem();
	}
}


CUndoRedoManager* CTreadDoc::GetUndoRedoManager()
{
	return &m_undoredo;
}

void CTreadDoc::RemoveSelectionFromGroup()
{
	CMapObject* obj;
	CObjectGroup* gr;

	for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
	{
		if( obj->GetGroupUID() != -1 )
		{
			gr = GroupForUID( obj->GetGroupUID() );
			if( gr )
			{
				Prop_RemoveObject( obj );
				gr->RemoveObject( obj->GetUID() );
				obj->SetGroupUID( -1 );
				Prop_AddObject( obj );
			}
		}
	}
}

int CTreadDoc::ReconnectEntity( CMapObject* pObject, void* p, void* p2 )
{
	if( pObject->GetClass() == MAPOBJ_CLASS_ENTITY )
	{
		CEntity* ent = dynamic_cast<CEntity*>(pObject);
		if( ent )
		{
			ent->OnConnectToEntDefs((CTreadDoc*)p);
		}
	}

	return 0;
}

void CTreadDoc::ReconnectEntities()
{
	m_ObjectList.WalkList( ReconnectEntity, this );
	m_SelectedObjectList.WalkList( ReconnectEntity, this );

	m_undoredo.ReconnectEntDefs(this);

	UpdateSelectionInterface();
}

void CTreadDoc::NudgeSelection( int dirbits ) // uses the active view.
{
	CMapView* v = GetChildFrame()->GetActiveView();
	if( !v || v->GetViewType() == VIEW_TYPE_TEXTURE )
		return;
	if( m_SelectedObjectList.IsEmpty() )
		return;
	if( GetNudgeFactor() <= 0.0f )
		return;

	vec3 maxis[3] = { sysAxisX, sysAxisY, sysAxisZ };
	vec3* test;
	float sign, bitsign;
	float d, bestd;
	int best, i;

	bitsign = 1.0f;
	if( dirbits&(NUDGE_RIGHT|NUDGE_DOWN) )
		bitsign = -1.0f;

	if( v->GetViewType() == VIEW_TYPE_3D )
	{
		if( dirbits&(NUDGE_UP|NUDGE_DOWN) )
		{
			test = &v->View.or3d.up;
		}
		else
		{
			test = &v->View.or3d.lft;
		}
	}
	else
	{
		if( dirbits&(NUDGE_UP|NUDGE_DOWN) )
		{
			test = &v->View.or2d.up;
		}
		else
		{
			test = &v->View.or2d.lft;
		}
	}

	//
	// select best axis.
	//
	best = 0;
	bestd = -99999.0f;
	for(i = 0; i < 3; i++)
	{
		d = dot( maxis[i], *test );
		if( fabs(d) > bestd )
		{
			bestd = fabs(d);
			sign = SIGN(d);
			best = i;
		}
	}

	vec3 nudge = maxis[best]*(sign*bitsign*GetNudgeFactor());

	GenericUndoRedoFromSelection()->SetTitle("Nudge Selection");

	m_SelectedObjectList.WalkList( Sys_Nudge, &nudge, this );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

float CTreadDoc::GetNudgeFactor()
{
	return m_fNudgeFactor;
}

void CTreadDoc::SetNudgeFactor( float nudge )
{
	m_fNudgeFactor = nudge;
}

void CTreadDoc::AddTextureFilterToList( const char* filter )
{
	int topos;
	int i;

	if( !filter || !filter[0] )
		return;

	//
	// does this thing exist?
	//
	for(i = 0, topos = 0; i < m_nNumTextureFilters; i++, topos++)
	{
		if( m_psTextureFilters[i] == "" )
			break;

		if( m_psTextureFilters[i].CompareNoCase( filter ) == 0 )
			break;
	}

	if( topos == m_nNumTextureFilters )
	{
		m_nNumTextureFilters++;
		if( m_nNumTextureFilters > MAX_TEXTURE_FILTER_HISTORY )
			m_nNumTextureFilters = MAX_TEXTURE_FILTER_HISTORY;

		topos = m_nNumTextureFilters-1;
	}

	//
	// copy everything forward one.
	//
	for( i = topos; i > 0; i-- )
	{
		m_psTextureFilters[i] = m_psTextureFilters[i-1];
	}

	m_psTextureFilters[0] = filter;
}

void CTreadDoc::SetTextureFilter( const char* szFilter )
{
	AddTextureFilterToList( szFilter );
	m_sTextureFilter = szFilter;
}

CString* CTreadDoc::GetTextureFilterHistory( int* num )
{
	*num = m_nNumTextureFilters;
	return m_psTextureFilters;
}

void CTreadDoc::SetTextureFilterHistory( CString* filters, int num )
{
	if( m_psTextureFilters )
		delete[] m_psTextureFilters;

	m_psTextureFilters = filters;
	m_nNumTextureFilters = num;
}

CString CTreadDoc::GetTextureFilter()
{
	return m_sTextureFilter;
}

void CTreadDoc::SnapSelectionToGrid( float fGridSize, bool x, bool y, bool z )
{
	CMapObject* obj;
	
	for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
	{
		obj->SnapToGrid( fGridSize, x, y, z );
	}
}

void CTreadDoc::SelectObjectsByShader( const char* szShaderName )
{
	MakeUndoDeselectAction();
	//ClearSelection();

	CMapObject* obj;

	for( obj = m_ObjectList.ResetPos(); obj; )
	{
		if( obj->IsVisible() )
		{
			obj->SelectByShader( szShaderName, this );
			if( obj->IsSelected() )
			{
				obj = m_ObjectList.ResetPos();
				continue;
			}
		}

		obj = m_ObjectList.GetNextItem();
	}
}

CObjectGroup* CTreadDoc::GroupSelection()
{
	if( m_SelectedObjectList.IsEmpty() )
		return 0;

	CObjectGroup* gr = new CObjectGroup();
	AssignUID( gr );

	gr->MakeList( &m_SelectedObjectList );
	gr->SetName( MakeUniqueGroupName( gr ) );
	m_ObjectGroupList.AddItem( gr );

	Prop_AddGroup( gr );

	//
	// remove the objects from the list, and re-add them to make them under the group.
	//
	CMapObject* obj;
	for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
	{
		Prop_RemoveObject( obj );

		//
		// already have a group?
		//
		if( obj->GetGroupUID() != -1 )
		{
			CObjectGroup* gr2 = GroupForUID( obj->GetGroupUID() );
			if( gr2 )
			{
				gr2->RemoveObject( obj->GetUID() );
			}
		}

		obj->SetGroupUID( gr->GetUID() );
		Prop_AddObject( obj );
	}

	Prop_SelectGroup( gr );

	RemoveNullGroups();

	return gr;
}

void CTreadDoc::Prop_SelectGroup( CObjectGroup* obj )
{
	Sys_GetPropView()->TreeShowGroup( obj );
}

void CTreadDoc::Prop_AddObject( CMapObject* obj )
{
	if( obj->CanAddToTree() )
		Sys_GetPropView()->TreeAddObject( obj );
}

void CTreadDoc::Prop_RemoveObject( CMapObject* obj )
{
	if( obj->CanAddToTree() && obj->GetTreeItem() != 0 )
		Sys_GetPropView()->TreeRemoveObject( obj );
}

void CTreadDoc::Prop_AddGroup( CObjectGroup* obj )
{
	Sys_GetPropView()->TreeAddGroup( obj );
}

void CTreadDoc::Prop_RemoveGroup( CObjectGroup* obj )
{
	Sys_GetPropView()->TreeRemoveGroup( obj );
}

void CTreadDoc::Prop_UpdateObjectState( CMapObject* obj )
{
	Sys_GetPropView()->TreeUpdateObjectState( obj );
}

void CTreadDoc::Prop_UpdateGroupState( CObjectGroup* obj )
{
	Sys_GetPropView()->TreeUpdateGroupState( obj );
}

int CTreadDoc::GetUIDOffset()
{
	return m_nNextUID;
}

void CTreadDoc::SetUIDOffset( int ofs )
{
	m_nNextUID = ofs;
}

void CTreadDoc::AssignUID( CMapObject* pObj )
{
	pObj->SetUID( m_nNextUID++ );
}

void CTreadDoc::AssignUID( CObjectGroup* pGroup )
{
	pGroup->SetUID( m_nNextUID++ );
}

CObjectGroup* CTreadDoc::GroupForUID( int uid )
{
	CObjectGroup* group;

	group = 0;
	m_ObjectGroupList.WalkList( FindGroupFromUID, (void*)uid, &group );
	return group;
}

int CTreadDoc::FindGroupFromUID( CObjectGroup* pGroup, void* p, void* p2 )
{
	int uid = (int)p;
	if( pGroup->GetUID() == uid )
	{
		*((CObjectGroup**)p2) = pGroup;
		return WALKLIST_STOP;
	}

	return 0;
}


CMapObject* CTreadDoc::ObjectForUID( int uid )
{
	CMapObject* obj;

	obj = 0;
	m_ObjectList.WalkList( FindObjectFromUID, (void*)uid, &obj );
	if( obj )
		return obj;

	m_SelectedObjectList.WalkList( FindObjectFromUID, (void*)uid, &obj );

	return obj;
}

int CTreadDoc::FindObjectFromUID( CMapObject* pObject, void* p, void* p2 )
{
	int uid = (int)p;
	if( pObject->GetUID() == uid )
	{
		*((CMapObject**)p2) = pObject;
		return WALKLIST_STOP;
	}

	return 0;
}

void CTreadDoc::MakeUndoLinkAction( CEntity* entity )
{
	CLinkUndoRedoAction* item;

	item = new CLinkUndoRedoAction();
	item->InitUndo( entity );

	AddUndoItem( item );

	item->SetTitle( "Link To Entity" );
}

void CTreadDoc::MakeUndoUnlinkAction( CEntity* entity )
{
	CUnlinkUndoRedoAction* item;

	item = new CUnlinkUndoRedoAction();
	item->InitUndo( entity );

	AddUndoItem( item );

	item->SetTitle( "Unlink From Entity" );
}

void CTreadDoc::MakeUndoDeselectAction()
{
	CDeselectUndoRedoAction* item;

	if( m_SelectedObjectList.IsEmpty() )
		return;

	item = new CDeselectUndoRedoAction();
	item->InitUndo( &m_SelectedObjectList, this );

	AddUndoItem( item );
	
	item->SetTitle( "Selection Change" );
}

void CTreadDoc::MakeUndoCreateAction( const char* title, CMapObject* obj )
{
	CCreateUndoRedoAction* item = new CCreateUndoRedoAction();

	item->InitUndo( obj, &m_SelectedObjectList, this );
	AddUndoItem( item );
	item->SetTitle( title );
}

void CTreadDoc::MakeUndoDeleteAction()
{
	if( m_SelectedObjectList.IsEmpty() )
		return;

	CDeleteUndoRedoAction* item = new CDeleteUndoRedoAction();

	item->InitUndo( &m_SelectedObjectList, this );
	AddUndoItem( item );
	item->SetTitle( "Delete" );
}

CGenericUndoRedoAction* CTreadDoc::GenericUndoRedoFromSelection()
{
	CGenericUndoRedoAction* item;

	if( m_SelectedObjectList.IsEmpty() )
		return 0;

	item = new CGenericUndoRedoAction();
	item->InitUndo( &m_SelectedObjectList, this );

	AddUndoItem( item );
	return item;
}

void CTreadDoc::AddUndoItem( CUndoRedoAction* item )
{
	m_undoredo.AddUndoItem( item );
}

void CTreadDoc::ClearUndoStack()
{
	m_undoredo.ClearUndoStack();
}

int CTreadDoc::CheckGroupNames( CObjectGroup* pObject, void* p, void* )
{
	ObjectNameCheck_t* d = (ObjectNameCheck_t*)p;

	if( !( d->sName.CompareNoCase( pObject->GetName() ) ) )
	{
		d->ok = false;
		return WALKLIST_STOP;
	}

	return 0;
}

CString CTreadDoc::MakeUniqueGroupName( CObjectGroup* gr )
{
	/*char buff[256];
	int i;
	ObjectNameCheck_t onc;

	for(i = 1;i < 999999;i++)
	{
		sprintf( buff, "%s%d", prefix, i );
		onc.ok = true;
		onc.sName = buff;

		m_ObjectGroupList.WalkList( CheckGroupNames, &onc );
		if( onc.ok )
		{
			return buff;
		}
	}

	return "Bad Group Name";*/
	
	CString s;
	s.Format( "group %d", gr->GetUID() );
	return s;
}

int CTreadDoc::CheckObjectNames( CMapObject* pObject, void* p, void* )
{
	ObjectNameCheck_t* d = (ObjectNameCheck_t*)p;

	if( !( d->sName.CompareNoCase( pObject->GetName() ) ) )
	{
		d->ok = false;
		return WALKLIST_STOP;
	}

	return 0;
}

CString CTreadDoc::MakeUniqueObjectName( CMapObject* obj )
{
	/*char buff[256];
	int i;
	ObjectNameCheck_t onc;

	for(i = 1;i < 999999;i++)
	{
		sprintf( buff, "%s%d", prefix, i );
		onc.ok = true;
		onc.sName = buff;

		m_SelectedObjectList.WalkList( CheckObjectNames, &onc );
		if( onc.ok )
		{
			m_ObjectList.WalkList( CheckObjectNames, &onc );
			if( onc.ok )
				return buff;
		}
	}

	return "Bad Object Name";*/

	CString s;
	s.Format( "%s %d", obj->GetRootName(), obj->GetUID() );
	return s;
}

void CTreadDoc::Prop_UpdateSelection()
{
	//AfxGetApp()->BeginWaitCursor();
	Sys_GetPropView()->SetDoc( this );
	Sys_GetPropView()->LoadSelection();
	//AfxGetApp()->EndWaitCursor();
}

void CTreadDoc::Prop_PropChange( const char* prop )
{
	Sys_GetPropView()->PropChange( this, prop );
}

void CTreadDoc::Prop_SelectProp( const char* name )
{
	Sys_GetPropView()->TrySelectProp( name );
}

bool CTreadDoc::IsEditingFaces()
{
	return m_bEditFaces;
}

bool CTreadDoc::IsEditingVerts()
{
	return m_bEditVerts;
}

void CTreadDoc::SetEditingFaces( bool editing )
{
	m_bEditFaces = editing;
	m_bEditVerts = false;
}

void CTreadDoc::SetEditingVerts( bool editing )
{
	m_bEditVerts = editing;
	m_bEditFaces = false;
}

void CTreadDoc::DetachObject( CMapObject* pObject )
{
	if( pObject->m_pMapList )
	{
		CLinkedList<CMapObject>* pL = (CLinkedList<CMapObject>*)pObject->m_pMapList;
		pL->RemoveItem( pObject );
		pObject->m_bSelected = false;
		pObject->OnRemoveFromMap( this );
		pObject->m_pMapList = 0;
	}
	else
	{
		OS_Break();
	}
}

void CTreadDoc::DetachManipulator( CManipulator* pMan )
{
	if( pMan->m_pMapList )
	{
		CLinkedList<CManipulator>* pL = (CLinkedList<CManipulator>*)pMan->m_pMapList;
		pL->RemoveItem( pMan );
		pMan->m_bSelected = false;
	}
	else
	{
		OS_Break();
	}

	KillViewTrackPicks( pMan );
}

void CTreadDoc::UpdateVertMode()
{
	m_gamePlugin->UpdateVertexMode(this);	
}

void CTreadDoc::UpdateDragMode()
{
	CSelDrag3D_Manipulator* m;
	float size = 256.0f;
	unsigned hlcolor = 0x604DE6F2;
	
	m_manips.push_back(m = new CSelDrag3D_Manipulator());
	m->color = 0x400000FF;
	m->hlcolor = hlcolor;
	m->size = size;
	m->vec = sysAxisX;
	m->x_in = &m_selpos;
	m->y_in = &m_selpos;
	m->z_in = &m_selpos;
	m->string = "drag/rotate x";
	AddManipulatorToMap( m );

	m_manips.push_back(m = new CSelDrag3D_Manipulator());
	m->color = 0x4000FF00;
	m->hlcolor = hlcolor;
	m->size = size;
	m->vec = sysAxisY;
	m->x_in = &m_selpos;
	m->y_in = &m_selpos;
	m->z_in = &m_selpos;
	m->string = "drag/rotate y";
	AddManipulatorToMap( m );

	m_manips.push_back(m = new CSelDrag3D_Manipulator());
	m->color = 0x40FF0000;
	m->hlcolor = hlcolor;
	m->size = size;
	m->vec = sysAxisZ;
	m->x_in = &m_selpos;
	m->y_in = &m_selpos;
	m->z_in = &m_selpos;
	m->string = "drag/rotate z";
	AddManipulatorToMap( m );
}

void CTreadDoc::UpdateScaleMode()
{
	//
	// attach manipulators to the bounding box.
	//
	{
		CSelScale_Manipulator* m;
		float size = 6.0f;
		unsigned int color = 0xFF800040;
		unsigned int hlcolor = 0xFF4DE6F2;

		//
		// corners.
		//
		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( -1.0f, -1.0f, -1.0f );
		m->size = size;
		m->x_in = &m_selmins;
		m->y_in = &m_selmins;
		m->z_in = &m_selmins;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 1.0f, -1.0f, -1.0f );
		m->size = size;
		m->x_in = &m_selmaxs;
		m->y_in = &m_selmins;
		m->z_in = &m_selmins;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 1.0f, 1.0f, -1.0f );
		m->size = size;
		m->x_in = &m_selmaxs;
		m->y_in = &m_selmaxs;
		m->z_in = &m_selmins;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( -1.0f, 1.0f, -1.0f );
		m->size = size;
		m->x_in = &m_selmins;
		m->y_in = &m_selmaxs;
		m->z_in = &m_selmins;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( -1.0f, -1.0f, 1.0f );
		m->size = size;
		m->x_in = &m_selmins;
		m->y_in = &m_selmins;
		m->z_in = &m_selmaxs;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 1.0f, -1.0f, 1.0f );
		m->size = size;
		m->x_in = &m_selmaxs;
		m->y_in = &m_selmins;
		m->z_in = &m_selmaxs;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 1.0f, 1.0f, 1.0f );
		m->size = size;
		m->x_in = &m_selmaxs;
		m->y_in = &m_selmaxs;
		m->z_in = &m_selmaxs;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( -1.0f, 1.0f, 1.0f );
		m->size = size;
		m->x_in = &m_selmins;
		m->y_in = &m_selmaxs;
		m->z_in = &m_selmaxs;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		//
		// sides.
		//
		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( -1.0f, 0.0f, 1.0f );
		m->size = size;
		m->x_in = &m_selmins;
		m->y_in = &m_selpos;
		m->z_in = &m_selmaxs;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 1.0f, 0.0f, 1.0f );
		m->size = size;
		m->x_in = &m_selmaxs;
		m->y_in = &m_selpos;
		m->z_in = &m_selmaxs;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 0.0f, -1.0f, 1.0f );
		m->size = size;
		m->x_in = &m_selpos;
		m->y_in = &m_selmins;
		m->z_in = &m_selmaxs;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 0.0f, 1.0f, 1.0f );
		m->size = size;
		m->x_in = &m_selpos;
		m->y_in = &m_selmaxs;
		m->z_in = &m_selmaxs;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( -1.0f, 0.0f, -1.0f );
		m->size = size;
		m->x_in = &m_selmins;
		m->y_in = &m_selpos;
		m->z_in = &m_selmins;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 1.0f, 0.0f, -1.0f );
		m->size = size;
		m->x_in = &m_selmaxs;
		m->y_in = &m_selpos;
		m->z_in = &m_selmins;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 0.0f, -1.0f, -1.0f );
		m->size = size;
		m->x_in = &m_selpos;
		m->y_in = &m_selmins;
		m->z_in = &m_selmins;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 0.0f, 1.0f, -1.0f );
		m->size = size;
		m->x_in = &m_selpos;
		m->y_in = &m_selmaxs;
		m->z_in = &m_selmaxs;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( -1.0f, -1.0f, 0.0f );
		m->size = size;
		m->x_in = &m_selmins;
		m->y_in = &m_selmins;
		m->z_in = &m_selpos;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( -1.0f, 1.0f, 0.0f );
		m->size = size;
		m->x_in = &m_selmins;
		m->y_in = &m_selmaxs;
		m->z_in = &m_selpos;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 1.0f, 1.0f, 0.0f );
		m->size = size;
		m->x_in = &m_selmaxs;
		m->y_in = &m_selmaxs;
		m->z_in = &m_selpos;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale_Manipulator());
		m->bConstantSize = true;
		m->color = color;
		m->hlcolor = hlcolor;
		m->vec = vec3( 1.0f, -1.0f, 0.0f );
		m->size = size;
		m->x_in = &m_selmaxs;
		m->y_in = &m_selmins;
		m->z_in = &m_selpos;
		m->SetViewFlags( VIEW_FLAG_2D );
		AddManipulatorToMap( m );
	}

	//
	// 3D
	//
	{
		CSelScale3D_Manipulator* m;
		float size = 64.0f;
		unsigned hlcolor = 0xFF4DE6F2;

		m_manips.push_back(m = new CSelScale3D_Manipulator());
		m->color = 0xFFFFFFFF;
		m->hlcolor = hlcolor;
		m->vec = vec3( -1.0f, 0.0f, 0.0f );
		m->size = size;
		m->string = "scale this way";
		m->x_in = &m_scale_gizmo_mins;
		m->y_in = &m_selpos;
		m->z_in = &m_selpos;
		m->SetViewFlags( VIEW_TYPE_3D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale3D_Manipulator());
		m->color = 0xFFFFFFFF;
		m->hlcolor = hlcolor;
		m->vec = vec3( 1.0f, 0.0f, 0.0f );
		m->size = size;
		m->string = "scale this way";
		m->x_in = &m_scale_gizmo_maxs;
		m->y_in = &m_selpos;
		m->z_in = &m_selpos;
		m->SetViewFlags( VIEW_TYPE_3D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale3D_Manipulator());
		m->color = 0xFFFFFFFF;
		m->hlcolor = hlcolor;
		m->vec = vec3( 0.0f, -1.0f, 0.0f );
		m->size = size;
		m->string = "scale this way";
		m->x_in = &m_selpos;
		m->y_in = &m_scale_gizmo_mins;
		m->z_in = &m_selpos;
		m->SetViewFlags( VIEW_TYPE_3D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale3D_Manipulator());
		m->color = 0xFFFFFFFF;
		m->hlcolor = hlcolor;
		m->vec = vec3( 0.0f, 1.0f, 0.0f );
		m->size = size;
		m->string = "scale this way";
		m->x_in = &m_selpos;
		m->y_in = &m_scale_gizmo_maxs;
		m->z_in = &m_selpos;
		m->SetViewFlags( VIEW_TYPE_3D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale3D_Manipulator());
		m->color = 0xFFFFFFFF;
		m->hlcolor = hlcolor;
		m->vec = vec3( 0.0f, 0.0f, -1.0f );
		m->size = size;
		m->string = "scale this way";
		m->x_in = &m_selpos;
		m->y_in = &m_selpos;
		m->z_in = &m_scale_gizmo_mins;
		m->SetViewFlags( VIEW_TYPE_3D );
		AddManipulatorToMap( m );

		m_manips.push_back(m = new CSelScale3D_Manipulator());
		m->color = 0xFFFFFFFF;
		m->hlcolor = hlcolor;
		m->vec = vec3( 0.0f, 0.0f, 1.0f );
		m->size = size;
		m->string = "scale this way";
		m->x_in = &m_selpos;
		m->y_in = &m_selpos;
		m->z_in = &m_scale_gizmo_maxs;
		m->SetViewFlags( VIEW_TYPE_3D );
		AddManipulatorToMap( m );
	}
}

void CTreadDoc::ClearAllTrackPicks()
{
	CManipulator* m;

	for( m = m_ManipulatorList.ResetPos(); m; m = m_ManipulatorList.GetNextItem() )
	{
		KillViewTrackPicks( m );
	}

	for( m = m_SelectedManipulatorList.ResetPos(); m; m = m_SelectedManipulatorList.GetNextItem() )
	{
		KillViewTrackPicks( m );
	}
}

void CTreadDoc::KillViewTrackPicks( CPickObject* obj )
{
	GetChildFrame()->GetMapView(0)->KillTrackPicks( obj );
	GetChildFrame()->GetMapView(1)->KillTrackPicks( obj );
	GetChildFrame()->GetMapView(2)->KillTrackPicks( obj );
	GetChildFrame()->GetMapView(3)->KillTrackPicks( obj );
}

int CTreadDoc::RepaintObjectWithShaderProc( CMapObject* obj, void* parm, void* parm2 )
{
	obj->RepaintShader( (const char*)parm, (CTreadDoc*)parm2 );
	return 0;
}

void CTreadDoc::RepaintObjectsWithShader( const char* szShader )
{
	m_ObjectList.WalkList( RepaintObjectWithShaderProc, (void*)szShader, this );
	m_SelectedObjectList.WalkList( RepaintObjectWithShaderProc, (void*)szShader, this );
	if (!szShader)
	{
		m_selShader = 0;
		CLinkedList<CShader> *shaders = ShaderList();
		if (shaders)
		{
			for (CShader *shader = shaders->ResetPos(); shader; shader = shaders->GetNextItem())
			{
				if (shader->Pickable()) 
				{
					SetSelectedShader(shader);
					break;
				}
			}
		}
	}
}

void CTreadDoc::UpdateSelectionInterface()
{
	
	BuildSelectionBounds();
	DeleteManipulators();

	if( m_SelectedObjectList.IsEmpty()  )
		return;

	//
	// if we're in brush face mode, and we have any brushes selected, then we don't have any extra
	// manipulators present.
	//
	if( (m_bEditFaces || m_bEditVerts) && ObjectsAreSelected( MAPOBJ_CLASS_BRUSH ) )
	{
		if( m_bEditVerts )
			UpdateVertMode();

		return;
	}

	{
		if( (IsInTrackAnimationMode() == false) && (GetSelectedObjectCount( MAPOBJ_CLASS_BRUSH ) == GetSelectedObjectCount()) )
			UpdateScaleMode();
	}

	UpdateDragMode();
}

void CTreadDoc::DeleteManipulators()
{
	int i;
	for(i = 0; i < m_manips.size(); ++i)
	{
		m_ManipulatorList.RemoveItem( m_manips[i] );
		KillViewTrackPicks( m_manips[i] );
		delete m_manips[i];
	}
	m_manips.clear();
}

CLinkedList<CMapObject>* CTreadDoc::GetObjectList()
{
	return &m_ObjectList;
}

CLinkedList<CMapObject>* CTreadDoc::GetSelectedObjectList()
{
	return &m_SelectedObjectList;
}

CLinkedList<CManipulator>* CTreadDoc::GetManipulatorList()
{
	return &m_ManipulatorList;
}

CLinkedList<CManipulator>* CTreadDoc::GetSelectedManipulatorList()
{
	return &m_SelectedManipulatorList;
}

CLinkedList<CObjectGroup>* CTreadDoc::GetObjectGroupList()
{
	return &m_ObjectGroupList;
}

void CTreadDoc::PaintSelection()
{
	CMapObject* p;

	for( p = m_SelectedObjectList.ResetPos(); p; p = m_SelectedObjectList.GetNextItem() )
	{
		p->SetShaderName( SelectedShaderName(), this );
	}

	Prop_UpdateSelection();
}

bool CTreadDoc::DrawSelBox()
{
	return true;
}

void CTreadDoc::BuildSelectionBounds()
{
	m_selmins = vec3::bogus_max;
	m_selmaxs = vec3::bogus_min;

	CMapObject* pO;
	for( pO = m_SelectedObjectList.ResetPos(); pO; pO = m_SelectedObjectList.GetNextItem() )
	{
		vec3 mn, mx;
		pO->GetWorldMinsMaxs( &mn, &mx );
		m_selmins = vec_mins( m_selmins, mn );
		m_selmaxs = vec_maxs( m_selmaxs, mx );
	}

	m_selpos = (m_selmins+m_selmaxs)/2.0f;

	//
	// setup scale gizmo sizes.
	//
	int i;
	for(i = 0; i < 3; i++)
	{
		float s = m_selmins[i]-m_selpos[i];
		if( s > -112.0f )
			s = -112.0f;

		m_scale_gizmo_mins[i] = m_selpos[i]+s;

		s = m_selmaxs[i]-m_selpos[i];
		if( s < 112.0f )
			s = 112.0f;

		m_scale_gizmo_maxs[i] = m_selpos[i]+s;
	}

	m_gamePlugin->UpdateSelectionBounds(this);
}

void CTreadDoc::AddObjectToSelection( CMapObject* pObject )
{
	//
	// is this sucker attached to any lists?
	//
	CLinkedList<CMapObject>* pL = (CLinkedList<CMapObject>*)pObject->m_pMapList;
	if( pL )
		pL->RemoveItem( pObject );
	
	pObject->m_pMapList = &m_SelectedObjectList;
	m_SelectedObjectList.AddItem( pObject );
	pObject->OnAddToSelection( this );
	pObject->m_bSelected = true;
}

void CTreadDoc::AddObjectToMap( CMapObject* pObject )
{	
	CLinkedList<CMapObject>* pL = (CLinkedList<CMapObject>*)pObject->m_pMapList;
	if( pL )
		pL->RemoveItem( pObject );
		
	pObject->m_pMapList = &m_ObjectList;
	m_ObjectList.AddItem( pObject );
	pObject->OnAddToMap( this );
	pObject->m_bSelected = false;
}

void CTreadDoc::AddManipulatorToMap( CManipulator* pM )
{
	CLinkedList<CManipulator>* pL = (CLinkedList<CManipulator>*)pM->m_pMapList;
	if( pL )
		pL->RemoveItem( pM );

	pM->m_pMapList = &m_ManipulatorList;
	pM->m_bSelected = false;
	m_ManipulatorList.AddItem( pM );
}

void CTreadDoc::AddManipulatorToSelection( CManipulator* pM )
{
	CLinkedList<CManipulator>* pL = (CLinkedList<CManipulator>*)pM->m_pMapList;
	if( pL )
		pL->RemoveItem( pM );

	pM->m_pMapList = &m_SelectedManipulatorList;
	pM->m_bSelected = true;
	m_SelectedManipulatorList.AddItem( pM );
}

void CTreadDoc::ClearSelectedManipulators()
{
	CManipulator* p, *n;

	for( p = m_SelectedManipulatorList.ResetPos(); p; )
	{
		m_SelectedManipulatorList.SetPosition( p );
		n = m_SelectedManipulatorList.GetNextItem();
		AddManipulatorToMap( p );
		p = n;
	}
}

void CTreadDoc::DeleteSelection()
{
	CMapObject* p;

	ClearAllTrackPicks();

	for( ;; )
	{
		p = m_SelectedObjectList.GetItem( LL_HEAD );
		if( !p )
			break;

		Prop_RemoveObject( p );
		DetachObject( p );
		delete p;
	}
}

void CTreadDoc::DeselectTypes( int classbits, int subclassbits )
{
	CMapObject* p, *n;

	for( p = m_SelectedObjectList.ResetPos(); p; )
	{
		m_SelectedObjectList.SetPosition( p );
		n = m_SelectedObjectList.GetNextItem();

		if( (p->GetClass()&classbits) && (p->GetSubClass()&subclassbits) )
		{
			p->Deselect( this );
		}

		p = n;
	}
}

bool CTreadDoc::ClearSelection()
{
	CMapObject* p;//, *n;

	for( ;; )
	{
		/*m_SelectedObjectList.SetPosition( p );
		n = m_SelectedObjectList.GetNextItem();
		p->Deselect( this );
		p = n;*/
		p = m_SelectedObjectList.GetItem(LL_HEAD);
		if( !p )
			break;
		p->Deselect(this);
	}

	return ObjectsAreSelected() == false;
}

void CTreadDoc::RemoveNullGroups()
{
	CObjectGroup* gr, *n;

	n = m_ObjectGroupList.ResetPos();

	for( ;; )
	{
		gr = n;
		if( !gr )
			break;

		n = m_ObjectGroupList.GetNextItem();

		if( gr->IsNullGroup( this ) )
		{
			Prop_RemoveGroup( gr );
			m_ObjectGroupList.DeleteItem( gr );
			m_ObjectGroupList.SetPosition( n );
		}
	}
}

void CTreadDoc::CloneSelectionInPlace()
{
	CLinkedList<CMapObject>* l = CloneSelection();
	if( !l )
		return;

	CMapObject* obj;
	CPasteUndoRedoAction* item = new CPasteUndoRedoAction();
	
	for( obj = l->ResetPos(); obj; obj = l->GetNextItem() )
	{
		AssignUID( obj );
		obj->SetName( MakeUniqueObjectName( obj ) );
		//obj->SetGroupUID( -1 );
	}

	FixupDuplication( this, &m_SelectedObjectList, l );

	item->InitUndo( l, &m_SelectedObjectList, this );
	item->SetTitle( "Clone" );
	AddUndoItem( item );

	ClearSelection();

	//Sys_GetPropView()->TreeEnableDraw( FALSE );

	for( ;; )
	{
		obj = l->RemoveItem( LL_HEAD );
		if( !obj )
			break;
		AddObjectToMap( obj );
		AddObjectToSelection( obj );
		Prop_AddObject( obj );
	}

	//Sys_GetPropView()->TreeEnableDraw( TRUE );

	//UpdateSelectionInterface();
	Prop_UpdateSelection();
	
	delete l;
}

CLinkedList<CMapObject>* CTreadDoc::CloneSelection()
{
	if( m_SelectedObjectList.IsEmpty() )
		return 0;

	CMapObject* obj;
	CLinkedList<CMapObject>* l = new CLinkedList<CMapObject>();

	for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
	{
		obj = obj->Clone();
		if( obj )
			l->AddItem( obj );
	}

	if( l->IsEmpty() )
	{
		delete l;
		return 0;
	}

	return l;
}


int CTreadDoc::ObjectCounter( CMapObject* pObject, void* parm, void* parm2 )
{
	CTreadDoc::ObjectCount_t* c = (CTreadDoc::ObjectCount_t*)parm;

	if(pObject->GetClass()&c->objclass)
	{
		int subclass = pObject->GetSubClass();
		if(c->objsubclass == MAPOBJ_CLASS_ALL ||
			(subclass&c->objsubclass))
		{
			c->count++;
			if( c->singlefind )
				return WALKLIST_STOP;
		}
	}

	return 0;
}

bool CTreadDoc::ObjectsAreSelected( int objclass, int subclass )
{
	if( m_SelectedObjectList.IsEmpty() )
		return false;

	if( objclass == MAPOBJ_CLASS_ALL && subclass == MAPOBJ_CLASS_ALL )
	{
		return true;
	}
	
	ObjectCount_t count;
	count.count = 0;
	count.singlefind = true;
	count.objclass = objclass;
	count.objsubclass = subclass;

	m_SelectedObjectList.WalkList( ObjectCounter, &count, 0 );
	return count.count > 0;
}

int CTreadDoc::GetSelectedObjectCount( int objclass, int subclass )
{
	if( m_SelectedObjectList.IsEmpty() )
		return 0;

	ObjectCount_t count;

	count.count = 0;
	count.singlefind = false;
	count.objclass = objclass;
	count.objsubclass = subclass;

	m_SelectedObjectList.WalkList( ObjectCounter, &count, 0 );
	return count.count;
}

int CTreadDoc::GetObjectCount( int objclass, int subclass )
{
	if( m_ObjectList.IsEmpty() )
		return 0;

	ObjectCount_t count;

	count.count = 0;
	count.singlefind = false;
	count.objclass = objclass;
	count.objsubclass = subclass;

	m_ObjectList.WalkList( ObjectCounter, &count, 0 );
	return count.count;
}

void CTreadDoc::FlipSelection( const vec3& origin, const vec3& axis )
{
	CMapObject* obj;

	for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
	{
		obj->FlipObject( origin, axis );
	}
}

void CTreadDoc::SetChildFrame( CChildFrame* pFrame )
{
	m_pChildFrame = pFrame;
}

CChildFrame* CTreadDoc::GetChildFrame( void )
{
	return m_pChildFrame;
}

BOOL CTreadDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if (!SetupGameDef(false))
	{
		return FALSE;
	}

	++s_numDocs;

	Sys_GetPropView()->SetDoc( this );
	Sys_GetPropView()->LoadDocument();
	Sys_GetPropView()->LoadSelection();
	Prop_UpdateSelection();
	Sys_GetMainFrame()->GetCheckMapDialog()->ShowWindow(SW_HIDE);
	Sys_BuildPluginMenu(Sys_GetMainFrame()->GetMenu(), this);

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	m_created = true;
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTreadDoc serialization

void CTreadDoc::Serialize(CArchive& ar)
{
	AfxGetApp()->BeginWaitCursor();

	if (ar.IsStoring())
	{
		// TODO: add storing code here
		CPluginFileExport *exporter = Sys_Exporter();
		Sys_SetExporter(0);
		if( exporter )
		{
			CString s;
			s.Format("Exporting %s:", exporter->Type());
			Sys_GetMainFrame()->SetStatusText(s);
			Sys_GetMainFrame()->GetStatusBar()->SetPos( 0 );
			Sys_GetMainFrame()->GetStatusBar()->ShowProgressBar();

			CFile *file = ar.GetFile();

			// fuck with AFX file object.
			CString path = file->GetFilePath();
			{
				CFileStatus fs;
				file->GetStatus( fs );
				if( fs.m_attribute & 0x01 )
				{
					CString s;
					s.Format("Export file: '%s' is read only!\n", path );
					OS_OkAlertMessage("Error", s);
					WriteToCompileWindow( s );
					Sys_GetMainFrame()->GetStatusBar()->ShowProgressBar(FALSE);
					Sys_GetMainFrame()->SetStatusText("Ready");
					throw new CArchiveException();
				}
			}

			file->Close();

			bool worked = true;
			if (!exporter->Export(path, false, this))
			{
				worked = false;
				CString s;
				s.Format("Failed to export %s\n", path );
				OS_OkAlertMessage("Error", s);
			}

			Sys_GetMainFrame()->GetStatusBar()->ShowProgressBar(FALSE);
			Sys_GetMainFrame()->SetStatusText("Ready");
			file->Open( path, CFile::typeBinary|CFile::modeWrite );

			if (!worked)
			{
				throw new CArchiveException();
			}
		}
		else
		{
			{
				CFileStatus fs;
				ar.GetFile()->GetStatus( fs );
				if( fs.m_attribute & 0x01 )
				{
					OS_OkAlertMessage("Error", "Map is read only!");
				}
				else
				{
					Sys_GetMainFrame()->SetStatusText("Writing Map:");
					Sys_GetMainFrame()->GetStatusBar()->SetPos( 0 );
					Sys_GetMainFrame()->GetStatusBar()->ShowProgressBar();

					WriteTreadMap( ar.GetFile(), this );

					Sys_GetMainFrame()->GetStatusBar()->ShowProgressBar(FALSE);
					Sys_GetMainFrame()->SetStatusText("Ready");
				}
			}
		}
	}
	else
	{
		CPluginFileImport *importer = Sys_Importer();

		bool mapfile = false;
		if (!importer)
		{
			char name[1024];
			strcpy( name, ar.GetFile()->GetFileName() );
			char* ext = StrGetFileExtension( name );
			if (ext )
			{
				if(_stricmp(ext, "trd"))
					mapfile = true;
			}
			else
			{
				mapfile = true;
			}

			if (mapfile)
			{
				while (true)
				{
					CChooseGameType dlg;
					if (dlg.DoModal() != IDOK) throw new CArchiveException();
					Sys_SetGameDef(dlg.GameType());
					if (!SetupGameDef(false)) throw new CArchiveException();
					for (importer = Sys_PluginImporters()->ResetPos(); importer; importer = Sys_PluginImporters()->GetNextItem())
					{
						if (!strcmp(importer->PluginGame(), m_gameDef->PluginGame())) break;
					}
					if (importer) break;

					CString s;
					s.Format("Tread could not find an importer that supports %s! Would you like to choose another game?", m_gameDef->Name());
					if (MessageBox(0, s, "Error", MB_YESNO) != IDYES) break;
				}

				if (!importer)
				{
					throw new CArchiveException();
				}
			}
		}
		
		// TODO: add loading code here
		if( importer )
		{
			if (!mapfile && !SetupGameDef(false))
			{
				Sys_SetImporter(0);
				return;
			}

			Sys_SetImporter(0);

			CFile *file = ar.GetFile();
			CString path = file->GetFilePath();
			file->Close();

			CString s;
			s.Format("Importing %s:", importer->Type());
			Sys_GetMainFrame()->SetStatusText(s);
			Sys_GetMainFrame()->GetStatusBar()->SetPos( 0 );
			Sys_GetMainFrame()->GetStatusBar()->ShowProgressBar();

			bool worked = true;
			if (!importer->Import(path, this))
			{
				worked = false;
				CString s;
				s.Format("Failed to import %s\n", path );
				OS_OkAlertMessage("Error", s);
			}

			Sys_GetMainFrame()->GetStatusBar()->ShowProgressBar(FALSE);
			Sys_GetMainFrame()->SetStatusText("Ready");
			file->Open( path, CFile::typeBinary|CFile::modeRead );

			if (!worked)
			{
				 throw new CArchiveException();
			}

			{
				char buff[256];
				char *p = buff;
				strcpy(buff, path);
				StrSetFileExtension(buff, ".trd");
				int c = 0;
				while (p[0] != 0) { ++p; ++c; }
				while (p[0] != '\\' && (c-- > 0)) {--p;}
				++p;
				m_importTitle = p;
			}
		}
		else
		{
			Sys_GetMainFrame()->SetStatusText("Loading Map:");
			Sys_GetMainFrame()->GetStatusBar()->SetPos( 0 );
			Sys_GetMainFrame()->GetStatusBar()->ShowProgressBar();

			bool s = ReadTreadMap( ar.GetFile(), this );
			
			Sys_GetMainFrame()->GetStatusBar()->ShowProgressBar(FALSE);
			Sys_GetMainFrame()->SetStatusText("Ready");

			if (!s)
			{
				 throw new CArchiveException();
			}
		}

		++s_numDocs;
		Sys_GetPropView()->SetDoc( this );
		Sys_GetPropView()->LoadDocument();
		Sys_GetPropView()->LoadSelection();
		UpdateSelectionInterface();
		Prop_UpdateSelection();
		
		Sys_GetMainFrame()->GetCheckMapDialog()->ShowWindow(SW_HIDE);
		Sys_BuildPluginMenu(Sys_GetMainFrame()->GetMenu(), this);

		m_created = true;
	}

	AfxGetApp()->EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
// CTreadDoc diagnostics

#ifdef _DEBUG
void CTreadDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTreadDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreadDoc commands

BOOL CTreadDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pHandlerInfo == 0 )
	{
		if( (nID >= ID_OBJMENU_MIN && nID <= ID_OBJMENU_MAX) )
		{
			if( nCode == CN_COMMAND )
			{
				Sys_OnPopupMenu( nID );
				return true;
			}
			else
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				Sys_UpdatePopupMenu( nID, (CCmdUI*)pExtra );
				return true;
			}
		}
	}

	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CTreadDoc::OnEditRedo() 
{
	// TODO: Add your command handler code here
	ClearAllTrackPicks();
	m_undoredo.PerformRedo( this );
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	m_undoredo.UpdateRedoMenu( pCmdUI );
}

void CTreadDoc::OnEditUndo() 
{
	// TODO: Add your command handler code here
	ClearAllTrackPicks();
	m_undoredo.PerformUndo( this );
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	m_undoredo.UpdateUndoMenu( pCmdUI );
}

void CTreadDoc::OnEditDelete() 
{
	// TODO: Add your command handler code here
	MakeUndoDeleteAction();
	DeleteSelection();
	UpdateSelectionInterface();
	Prop_UpdateSelection();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( m_SelectedObjectList.IsEmpty() == false );
}

void CTreadDoc::OnEditCopy() 
{
	// TODO: Add your command handler code here
	CMapObject* obj, *copy;

	Sys_GetMainFrame()->m_ClipBoard.DestroyList();
	Sys_GetMainFrame()->m_ClipBoardMins = m_selmins;
	Sys_GetMainFrame()->m_ClipBoardMaxs = m_selmaxs;
	Sys_GetMainFrame()->m_ClipBoardPos = m_selpos;

	for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
	{
		copy = obj->Clone();
		copy->CopyState( obj, this );
		Sys_GetMainFrame()->m_ClipBoard.AddItem( copy );
	}

	Sys_GetMainFrame()->m_ClipBoardDoc = this;
}

void CTreadDoc::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( m_SelectedObjectList.IsEmpty() == false && !IsInTrackAnimationMode() );
}

void CTreadDoc::OnEditPaste() 
{
	// TODO: Add your command handler code here
	vec3 pos = vec3::zero;

	CMapView* pView = GetChildFrame()->GetActiveView();
	if( pView->GetViewType() == VIEW_TYPE_TEXTURE )
	{
		MessageBox( 0, "Your mouse cursor is over the textures view. Move your cursor over a valid view to paste objects.", "Cannot Paste Objects", MB_TASKMODAL|MB_DEFBUTTON1|MB_OK|MB_ICONHAND );
		return;
	}

	//
	// selected objects?
	//
	if( m_SelectedObjectList.IsEmpty() )
	{
		pView->WinXYToVec3( pView->View.fWinCenterX, pView->View.fWinCenterY, &pos );
	}
	else
	{
		float dist = -8.0f;

		if( pView->GetGridSnap() )
			dist = -pView->GetGridSize();

		//
		// the pos is at the center of the selection.
		//
		pos = m_selpos;
		if( pView && pView->GetViewType() != VIEW_TYPE_3D )
		{
			pos += (pView->View.or2d.up*dist) + (pView->View.or2d.lft*dist);
		}
		else
		{
			pos += (vec3( 1, 1, 0 ) * dist);
		}

		if( pView->GetGridSnap() )
			pos = Sys_SnapVec3( pos, pView->GetGridSize() );
	}

	//
	// duplicate the list.
	//
	CLinkedList<CMapObject> temp_list;
	CMapObject* obj, *clone;
	
	for( obj = Sys_GetMainFrame()->m_ClipBoard.ResetPos(); obj; obj = Sys_GetMainFrame()->m_ClipBoard.GetNextItem() )
	{
		clone = obj->Clone();
		AssignUID( clone );
		clone->SetName( MakeUniqueObjectName( clone ) );
		temp_list.AddItem( clone );
	}

	//
	// fixup the temp_list duplication.
	//
	FixupDuplication( Sys_GetMainFrame()->m_ClipBoardDoc, &Sys_GetMainFrame()->m_ClipBoard, &temp_list );

	//
	// transform the list.
	//
	vec3 t = pos - Sys_GetMainFrame()->m_ClipBoardPos;
	temp_list.WalkList( Sys_Translate, &t, this );

	//
	// now make an undo item.
	//
	CPasteUndoRedoAction* item = new CPasteUndoRedoAction();

	item->InitUndo( &temp_list, &m_SelectedObjectList, this );
	item->SetTitle( "Paste Objects" );
	AddUndoItem( item );

	//
	// clear the selection and move the temp_list into the selection.
	//
	ClearSelection();

	for( ;; )
	{
		obj = temp_list.RemoveItem(LL_HEAD);
		if( !obj )
			break;

		//obj->SetName( MakeUniqueObjectName(obj->GetRootName()) );
				
		AddObjectToMap( obj );
		AddObjectToSelection( obj );
		Prop_AddObject( obj );
	}

	UpdateSelectionInterface();
	Prop_UpdateSelection();

	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( Sys_GetMainFrame()->m_ClipBoard.IsEmpty() == false && !IsInTrackAnimationMode() );
}

void CTreadDoc::OnEditDeselect() 
{
	// TODO: Add your command handler code here
	if( ObjectsAreSelected() )
	{
		MakeUndoDeselectAction();
		ClearSelection();
		UpdateSelectionInterface();
		Prop_UpdateSelection();
		Sys_RedrawWindows();
	}
}

void CTreadDoc::OnUpdateEditDeselect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() );
}

void CTreadDoc::OnToolsHide() 
{
	// TODO: Add your command handler code here
	CMapObject* obj;

	MakeUndoDeselectAction();

	for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
	{
		obj->SetVisible( this, false );
		Prop_UpdateObjectState( obj );
	}
	
	ClearSelection();
	UpdateSelectionInterface();
	Prop_UpdateSelection();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsHide(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() );
}

void CTreadDoc::OnToolsHideallingroup() 
{
	// TODO: Add your command handler code here
	CMapObject* obj;

	obj = m_SelectedObjectList.ResetPos();

	for( ;; )
	{
		if( !obj )
			break;

		//
		// is this sucker in a group?
		//
		if( obj->GetGroupUID() == -1 )
		{
			obj = m_SelectedObjectList.GetNextItem();
			continue;
		}

		CObjectGroup* gr = GroupForUID( obj->GetGroupUID() );
		if( gr )
		{
			gr->SetVisible( this, false );
			Prop_UpdateGroupState( gr );
			obj = m_SelectedObjectList.ResetPos();
		}
		else
		{
			obj = m_SelectedObjectList.GetNextItem();
		}
	}

	UpdateSelectionInterface();
	Prop_UpdateSelection();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsHideallingroup(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() );
}

void CTreadDoc::OnToolsMakegroup() 
{
	// TODO: Add your command handler code here
	GroupSelection();
}

void CTreadDoc::OnUpdateToolsMakegroupandhide(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() && !IsInTrackAnimationMode() );
}

void CTreadDoc::OnToolsRotate180() 
{
	// TODO: Add your command handler code here
	CMapView* v = GetChildFrame()->GetActiveView();

	if(!v)
		return;

	GenericUndoRedoFromSelection()->SetTitle("Rotate 180º");

	//
	// find the best axis.
	//
	vec3 maxis[3] = { sysAxisX, sysAxisY, sysAxisZ };
	float d, bestd, sign;
	int best, i;
	COrientation* or;

	bestd = -99999.0f;
	best = 0;

	if( v->GetViewType() == VIEW_TYPE_3D )
		or = &v->View.or3d;
	else
		or = &v->View.or2d;

	for(i = 0; i < 3; i++)
	{
		d = dot( or->frw, maxis[i] );
		if( fabs(d) > bestd )
		{
			bestd = fabs(d);
			sign = SIGN(d);
			best = i;
		}
	}

	vec3 axis = maxis[best]*sign;

	SysRotateInfo_t info;

	info.doc = this;
	info.m = build_rotation_matrix( axis[0], axis[1], axis[2], PI );
	info.org = m_selpos;

	m_SelectedObjectList.WalkList( Sys_Rotate, &info );

	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsRotate180(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() && !IsInTrackAnimationMode() );
}

void CTreadDoc::OnToolsRotate90ccw() 
{
	// TODO: Add your command handler code here
	CMapView* v = GetChildFrame()->GetActiveView();

	if(!v)
		return;

	GenericUndoRedoFromSelection()->SetTitle("Rotate 90º CCW");

	//
	// find the best axis.
	//
	vec3 maxis[3] = { sysAxisX, sysAxisY, sysAxisZ };
	float d, bestd, sign;
	int best, i;
	COrientation* or;

	bestd = -99999.0f;
	best = 0;

	if( v->GetViewType() == VIEW_TYPE_3D )
		or = &v->View.or3d;
	else
		or = &v->View.or2d;

	for(i = 0; i < 3; i++)
	{
		d = dot( or->frw, maxis[i] );
		if( fabs(d) > bestd )
		{
			bestd = fabs(d);
			sign = SIGN(d);
			best = i;
		}
	}

	vec3 axis = maxis[best]*sign;

	SysRotateInfo_t info;

	info.doc = this;
	info.m = build_rotation_matrix( axis[0], axis[1], axis[2], -PI/2 );
	info.org = m_selpos;

	m_SelectedObjectList.WalkList( Sys_Rotate, &info );

	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsRotate90ccw(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() && !IsInTrackAnimationMode() );
}

void CTreadDoc::OnToolsRotate90cw() 
{
	// TODO: Add your command handler code here
	CMapView* v = GetChildFrame()->GetActiveView();

	if(!v)
		return;

	GenericUndoRedoFromSelection()->SetTitle("Rotate 90º CW");

	//
	// find the best axis.
	//
	vec3 maxis[3] = { sysAxisX, sysAxisY, sysAxisZ };
	float d, bestd, sign;
	int best, i;
	COrientation* or;

	bestd = -99999.0f;
	best = 0;

	if( v->GetViewType() == VIEW_TYPE_3D )
		or = &v->View.or3d;
	else
		or = &v->View.or2d;

	for(i = 0; i < 3; i++)
	{
		d = dot( or->frw, maxis[i] );
		if( fabs(d) > bestd )
		{
			bestd = fabs(d);
			sign = SIGN(d);
			best = i;
		}
	}

	vec3 axis = maxis[best]*sign;

	SysRotateInfo_t info;

	info.doc = this;
	info.m = build_rotation_matrix( axis[0], axis[1], axis[2], PI/2 );
	info.org = m_selpos;

	m_SelectedObjectList.WalkList( Sys_Rotate, &info );

	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsRotate90cw(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() && !IsInTrackAnimationMode() );
}

void CTreadDoc::OnToolsSelectallingroups() 
{
	// TODO: Add your command handler code here
	CMapObject* obj;
	CObjectGroup** grlist;
	int i, guid;
	int count = m_SelectedObjectList.GetCount();
	if( count < 1 )
		return;

	grlist = new CObjectGroup*[count];

	//
	// remove the stuff from the selected list just temporarily.
	//
	i = 0;
	for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
	{
		guid = obj->GetGroupUID();
		if( guid == -1 )
		{
			grlist[i++] = 0;
		}
		else
		{
			grlist[i++] = GroupForUID( guid );
		}
	}

	for( i = 0; i < count; i++ )
	{
		if( grlist[i] )
		{
			if( grlist[i]->IsMarked() )
				continue;

			grlist[i]->SetMarked( true );
			grlist[i]->SelectObjects( this );
		}
	}

	//
	// unmark.
	//
	for(i = 0; i < count; i++)
	{
		if( grlist[i] )
		{
			grlist[i]->SetMarked( false );
		}
	}

	delete[] grlist;

	UpdateSelectionInterface();
	Prop_UpdateSelection();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectallingroups(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() );
}

void CTreadDoc::OnToolsShowallingroups() 
{
	// TODO: Add your command handler code here
	CMapObject* obj;
	CObjectGroup* gr;

	for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
	{
		if( obj->GetGroupUID() != -1 )
		{
			gr = GroupForUID( obj->GetGroupUID() );
			if( gr )
			{
				gr->SetVisible( this );
				Prop_UpdateGroupState( gr );
			}
		}
	}

	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsShowallingroups(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() );
}

void CTreadDoc::OnUpdateToolsMakegroup(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() && !IsInTrackAnimationMode() );
}

void CTreadDoc::OnToolsFliphorizontal() 
{
	// TODO: Add your command handler code here
	CMapView* v = GetChildFrame()->GetActiveView();
	if( v )
	{
		vec3 axis;
		COrientation* or;

		if( v->GetViewType() == VIEW_TYPE_3D )
			or = &v->View.or3d;
		else
			or = &v->View.or2d;

		//
		// what axis maps most the up vector?
		// we do this so the 3D view works naturally.
		//
		vec3 maxis[3] = { sysAxisX, sysAxisY, sysAxisZ };
		
		int i;
		float bestd, d;
		int best;

		bestd = -99999.0f;
		best = 0;

		for( i = 0; i < 3; i++ )
		{
			d = fabs(dot(maxis[i], or->lft));
			if( d > bestd )
			{
				bestd = d;
				best = i;
			}
		}

		axis = maxis[best];
	
		GenericUndoRedoFromSelection()->SetTitle("Flip Horizontal");
		FlipSelection( m_selpos, axis );
		UpdateSelectionInterface();
		Sys_RedrawWindows();
	}
}

void CTreadDoc::OnUpdateToolsFliphorizontal(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CMapView* v = GetChildFrame()->GetActiveView();
	if( v )
	{
		pCmdUI->Enable( ObjectsAreSelected() && (v->GetViewType()&VIEW_FLAG_MAP) && !IsInTrackAnimationMode() );
	}
}

void CTreadDoc::OnToolsFliponxaxis() 
{
	// TODO: Add your command handler code here
	GenericUndoRedoFromSelection()->SetTitle("Flip On X Axis");
	FlipSelection( m_selpos, sysAxisX );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsFliponxaxis(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() && !IsInTrackAnimationMode() );
}

void CTreadDoc::OnToolsFliponyaxis() 
{
	// TODO: Add your command handler code here
	GenericUndoRedoFromSelection()->SetTitle("Flip On Y Axis");
	FlipSelection( m_selpos, sysAxisY );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsFliponyaxis(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() && !IsInTrackAnimationMode() );
}

void CTreadDoc::OnToolsFliponzaxis() 
{
	// TODO: Add your command handler code here
	GenericUndoRedoFromSelection()->SetTitle("Flip On Z Axis");
	FlipSelection( m_selpos, sysAxisZ );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsFliponzaxis(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() && !IsInTrackAnimationMode() );
}

void CTreadDoc::OnToolsFlipvertical() 
{
	// TODO: Add your command handler code here
	CMapView* v = GetChildFrame()->GetActiveView();
	if( v )
	{
		vec3 axis;
		COrientation* or;

		if( v->GetViewType() == VIEW_TYPE_3D )
			or = &v->View.or3d;
		else
			or = &v->View.or2d;

		//
		// what axis maps most the up vector?
		// we do this so the 3D view works naturally.
		//
		vec3 maxis[3] = { sysAxisX, sysAxisY, sysAxisZ };
		
		int i;
		float bestd, d;
		int best;

		bestd = -99999.0f;
		best = 0;

		for( i = 0; i < 3; i++ )
		{
			d = fabs(dot(maxis[i], or->up));
			if( d > bestd )
			{
				bestd = d;
				best = i;
			}
		}

		axis = maxis[best];
	
		GenericUndoRedoFromSelection()->SetTitle("Flip Vertical");
		FlipSelection( m_selpos, axis );
		UpdateSelectionInterface();
		Sys_RedrawWindows();
	}
}

void CTreadDoc::OnUpdateToolsFlipvertical(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CMapView* v = GetChildFrame()->GetActiveView();
	if( v )
	{
		pCmdUI->Enable( ObjectsAreSelected() && (v->GetViewType()&VIEW_FLAG_MAP) );
	}
}

void CTreadDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	Sys_GetPropView()->SetDoc( 0 );
	Sys_GetPropView()->LoadDocument();
	Sys_GetPropView()->LoadSelection();
	Sys_GetMainFrame()->GetTrackAnimDialog()->Load( 0, 0, 0 );
	Sys_GetMainFrame()->GetCheckMapDialog()->Clear();
	Sys_GetMainFrame()->GetCheckMapDialog()->ShowWindow( SW_HIDE );

	if (m_gamePlugin) m_gamePlugin->ReleaseUserData(this);

	CDocument::OnCloseDocument();

	if (--s_numDocs == 0)
	{
		Sys_BuildPluginMenu(Sys_GetMainFrame()->GetMenu(), 0);
	}
}

void CTreadDoc::OnToolsMakegroupandhide() 
{
	// TODO: Add your command handler code here
	CObjectGroup* gr = GroupSelection();
	if( gr )
	{
		gr->SetVisible( this, false );
		Prop_UpdateGroupState( gr );
		UpdateSelectionInterface();
		Prop_UpdateSelection();
		Sys_RedrawWindows();
	}
}

void CTreadDoc::OnToolsRemovefromgroups() 
{
	// TODO: Add your command handler code here
	RemoveSelectionFromGroup();
	RemoveNullGroups();
}

void CTreadDoc::OnUpdateToolsRemovefromgroups(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() && !IsInTrackAnimationMode() );
}

bool CTreadDoc::ExportMapForCompile(char *outmappath, int bufLen)
{
	if( !SaveIfNoPath() )
		return false;

	// TODO: Add your command handler code here
	char map[1024];
	GetMapExportPathName(map, 1024);

	if( SetupForCompile( map ) == false )
		return false;

	strcpy(outmappath, map);

	return true;
}

void CTreadDoc::OnToolsCompilemap() 
{
	GamePlugin()->CompileMap(this, false);
}

void CTreadDoc::OnUpdateToolsCompilemap(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsCompilerunmap() 
{
	GamePlugin()->CompileMap(this, true);
}

void CTreadDoc::OnUpdateToolsCompilerunmap(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsRunmap() 
{
	// TODO: Add your command handler code here
	if( !SaveIfNoPath() )
		return;

	char buff[1024];
	GetMapExportPathName(buff, 1024);
	GamePlugin()->RunGame(buff);
}

void CTreadDoc::OnUpdateToolsRunmap(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( m_compiler == 0 );
}

void CTreadDoc::OnToolsAbortactivecompile() 
{
	// TODO: Add your command handler code here
	ShowCompileWindow();
	KillCompile();
}

void CTreadDoc::OnUpdateToolsAbortactivecompile(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( m_compiler != 0 );
}

void CTreadDoc::OnViewMapcompileroutput() 
{
	// TODO: Add your command handler code here
	ShowCompileWindow( !IsCompileWindowVisible() );
}

void CTreadDoc::OnUpdateViewMapcompileroutput(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( m_pCompileWindow != 0 );
	pCmdUI->SetCheck( IsCompileWindowVisible() );
}

void CTreadDoc::OnFileExportmap(CPluginFileExport *exporter) 
{
	// TODO: Add your command handler code here
	CString sPath;
	CString sDefaultName = "";
	sDefaultName = GetTitle();
	int nIndex = sDefaultName.Find('.');
	if(nIndex != -1)
		sDefaultName = sDefaultName.Left(nIndex);

	sDefaultName += CString(".") + exporter->Extension();

	CString s;
	s.Format("%s Files (*.%s)|*.%s|All Files (*.*)|*.*||", exporter->Type(), exporter->Extension(), exporter->Extension());
	CFileDialog dlgTemp(false, exporter->Extension(), sDefaultName,
			OFN_NONETWORKBUTTON|OFN_OVERWRITEPROMPT, s, Sys_GetMainFrame());

	CString title = CString("Export As ") + exporter->Type();
	dlgTemp.m_ofn.lpstrTitle = title;
	if(dlgTemp.DoModal() != IDOK)
		return;

	sPath = dlgTemp.GetPathName();
	Sys_SetExporter(exporter);
	ExportMapFile( sPath );
}

void CTreadDoc::OnUpdateFileExportmap(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsRunabducted() 
{
	// TODO: Add your command handler code here
	GamePlugin()->RunGame(0);
}

void CTreadDoc::OnToolsBrushfilterAlwaysshadow() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.always_shadow = !m_objfilter.BrushFilter.always_shadow;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterAlwaysshadow(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.always_shadow );
}

void CTreadDoc::OnToolsBrushfilterAreaportal() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.areaportal = !m_objfilter.BrushFilter.areaportal;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterAreaportal(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.areaportal );
}

void CTreadDoc::OnToolsBrushfilterCameraclip() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.camera_clip = !m_objfilter.BrushFilter.camera_clip;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterCameraclip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.camera_clip );
}

void CTreadDoc::OnToolsBrushfilterCoronablock() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.corona_block = !m_objfilter.BrushFilter.corona_block;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterCoronablock(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.corona_block );
}

void CTreadDoc::OnToolsBrushfilterDetail() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.detail= !m_objfilter.BrushFilter.detail;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterDetail(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.detail );
}

void CTreadDoc::OnToolsBrushfilterFilterall() 
{
	// TODO: Add your command handler code here
	m_objfilter.SetBrushFilters(TRUE);
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterFilterall(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsBrushfilterFilterallclip() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.camera_clip = true;
	m_objfilter.BrushFilter.monster_clip = true;
	m_objfilter.BrushFilter.player_clip = true;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterFilterallclip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsBrushfilterFilterallsky() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.skybox = true;
	m_objfilter.BrushFilter.skyportal = true;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterFilterallsky(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsBrushfilterFilternone() 
{
	// TODO: Add your command handler code here
	m_objfilter.SetBrushFilters( FALSE );
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterFilternone(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsBrushfilterMonsteclip() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.monster_clip = !m_objfilter.BrushFilter.monster_clip;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterMonsteclip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.monster_clip );
}

void CTreadDoc::OnToolsBrushfilterNodraw() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.nodraw = !m_objfilter.BrushFilter.nodraw;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterNodraw(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.nodraw );
}

void CTreadDoc::OnToolsBrushfilterNoshadow() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.noshadow = !m_objfilter.BrushFilter.noshadow;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterNoshadow(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.noshadow );
}

void CTreadDoc::OnToolsBrushfilterPlayerclip() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.player_clip = !m_objfilter.BrushFilter.player_clip;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterPlayerclip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.player_clip );
}

void CTreadDoc::OnToolsBrushfilterReapplyfilter() 
{
	// TODO: Add your command handler code here
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterReapplyfilter(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsBrushfilterSkybox() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.skybox = !m_objfilter.BrushFilter.skybox;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterSkybox(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.skybox );
}

void CTreadDoc::OnToolsBrushfilterSkyportal() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.skyportal = !m_objfilter.BrushFilter.skyportal;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterSkyportal(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.skyportal );
}

void CTreadDoc::OnToolsBrushfilterSolid() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.solid = !m_objfilter.BrushFilter.solid;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterSolid(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.solid );
}

void CTreadDoc::OnToolsBrushfilterWater() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.water = !m_objfilter.BrushFilter.water;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterWater(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.water );
}

void CTreadDoc::OnToolsBrushfilterWindow() 
{
	// TODO: Add your command handler code here
	m_objfilter.BrushFilter.window = !m_objfilter.BrushFilter.window;
	OnObjectFilterChange();
}

void CTreadDoc::OnUpdateToolsBrushfilterWindow(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_objfilter.BrushFilter.window );
}

void CTreadDoc::OnToolsSelectbrushesbyattributesAlwaysshadow() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_ALWAYS_SHADOW, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesAlwaysshadow(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesAreaportal() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_AREAPORTAL, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesAreaportal(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesCameraclip() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_CAMERA_CLIP, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesCameraclip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesCoronablock() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_CORONA_BLOCK, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesCoronablock(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesDetail() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_DETAIL, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesDetail(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesMonsterclip() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_MONSTER_CLIP, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesMonsterclip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesNodraw() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( 0, SURF_NO_DRAW );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesNodraw(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesNoshadow() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_NO_SHADOW, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesNoshadow(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesPlayerclip() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_PLAYER_CLIP, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesPlayerclip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesSkybox() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( 0, SURF_SKY_BOX );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesSkybox(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesSkyportal() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( 0, SURF_SKY_PORTAL );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesSkyportal(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesSolid() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_SOLID, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesSolid(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesWater() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_WATER, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesWater(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsSelectbrushesbyattributesWindow() 
{
	// TODO: Add your command handler code here
//	SelectBrushesByAttributes( CONTENTS_WINDOW, 0 );
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsSelectbrushesbyattributesWindow(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsHideall() 
{
	// TODO: Add your command handler code here
	CMapObject* obj;
	CObjectGroup* gr;

	ClearSelection();
	
	for( obj = m_ObjectList.ResetPos(); obj; obj = m_ObjectList.GetNextItem() )
	{
		obj->SetVisible( this, false );
		Prop_UpdateObjectState( obj );
	}

	for( gr = m_ObjectGroupList.ResetPos(); gr; gr = m_ObjectGroupList.GetNextItem() )
	{
		gr->SetVisible( this, false );
		Prop_UpdateGroupState( gr );
	}

	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsHideall(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsShowall() 
{
	// TODO: Add your command handler code here
	CMapObject* obj;
	CObjectGroup* gr;

	ClearAllTrackPicks();

	for( obj = m_ObjectList.ResetPos(); obj; obj = m_ObjectList.GetNextItem() )
	{
		obj->SetVisible( this, true );
		Prop_UpdateObjectState( obj );
	}

	for( gr = m_ObjectGroupList.ResetPos(); gr; gr = m_ObjectGroupList.GetNextItem() )
	{
		gr->SetVisible( this, true );
		Prop_UpdateGroupState( gr );
	}

	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsShowall(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsAnimationmode() 
{
	// TODO: Add your command handler code here
	SetInTrackAnimationMode( !IsInTrackAnimationMode() );

	bool animate = IsInTrackAnimationMode();
	CMapObject* obj;

	for( obj = m_ObjectList.ResetPos(); obj; obj = m_ObjectList.GetNextItem() )
	{
		obj->SetInAnimationMode( this, false );
	}

	for( obj = m_SelectedObjectList.ResetPos(); obj; obj = m_SelectedObjectList.GetNextItem() )
	{
		obj->SetInAnimationMode( this, animate );
	}

	Prop_UpdateSelection();
	UpdateSelectionInterface();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsAnimationmode(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( IsInTrackAnimationMode() );
}

void CTreadDoc::OnToolsCenteronselection() 
{
	// TODO: Add your command handler code here
	CenterOnSelection();
}

void CTreadDoc::OnUpdateToolsCenteronselection(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() );
}

void CTreadDoc::OnToolsCheckmapforerrors() 
{
	// TODO: Add your command handler code here
	Sys_GetMainFrame()->GetCheckMapDialog()->ShowWindow( SW_SHOW );
	Sys_GetMainFrame()->GetCheckMapDialog()->CheckMap( this, false );
}

void CTreadDoc::OnUpdateToolsCheckmapforerrors(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( m_ObjectList.IsEmpty() == false ||
					m_SelectedObjectList.IsEmpty() == false );
}

void CTreadDoc::OnToolsCheckselectedobjects() 
{
	// TODO: Add your command handler code here
	Sys_GetMainFrame()->GetCheckMapDialog()->ShowWindow( SW_SHOW );
	Sys_GetMainFrame()->GetCheckMapDialog()->CheckMap( this, true );
}

void CTreadDoc::OnUpdateToolsCheckselectedobjects(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( ObjectsAreSelected() );
}

void CTreadDoc::OnToolsViewleaktrace() 
{
	// TODO: Add your command handler code here
	if( m_numleakpts > 0 )
	{
		m_numleakpts = 0;
		if( m_leakpts )
			delete[] m_leakpts;
		m_leakpts = 0;

		Sys_RedrawWindows();
	}
	else
	{
		m_gamePlugin->LoadLeakTrace(this);
		Sys_RedrawWindows();
	}
}

void CTreadDoc::OnUpdateToolsViewleaktrace(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( m_numleakpts > 0 || m_gamePlugin->LeakTraceExists(this) );
	pCmdUI->SetCheck( m_numleakpts > 0 );
}

void CTreadDoc::OnToolsReloadleaktrace() 
{
	// TODO: Add your command handler code here
	if( m_numleakpts > 0 )
	{
		m_numleakpts = 0;
		if( m_leakpts )
		{
			delete[] m_leakpts;
		}
		m_leakpts = 0;
	}

	m_gamePlugin->LoadLeakTrace(this);
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateToolsReloadleaktrace(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( m_numleakpts > 0 && m_gamePlugin->LeakTraceExists(this) );
}

bool CTreadDoc::ExportBrushFile( CString filename, bool selected )
{
	std::fstream fs;
	WriteBrushMapData_t data;

	fs.open( filename, std::ios::out );
	data.selected = selected;
	data.stream = &fs;
	data.doc = this;

	fs << "0\nSECTION\n2\nHEADER\n";
	fs << "9\n$ACADVER\n1\nAC1008\n";
	fs << "9\n$UCSORG\n10\n0.0\n20\n0.0\n30\n0.0\n";
	fs << "9\n$UCSXDIR\n10\n1.0\n20\n0.0\n30\n0.0\n";
	fs << "9\n$TILEMODE\n70\n1\n";
	fs << "9\n$UCSYDIR\n10\n0.0\n20\n1.0\n30\n0.0\n";
	fs << "9\n$EXTMIN\n10\n0\n20\n0\n30\n0\n";
	fs << "9\n$EXTMAX\n10\n0\n20\n0\n30\n0\n";
	fs << "0\nENDSEC\n";
	fs << "0\nSECTION\n2\nTABLES\n";
	fs << "0\nTABLE\n2\nLTYPE\n70\n1\n0\nLTYPE\n2\nCONTINUOUS\n70\n64\n3\nSolid line\n72\n65\n73\n0\n40\n0.0\n";
	fs << "0\nENDTAB\n";
	
	int count = GetSelectedObjectCount( MAPOBJ_CLASS_BRUSH );
	if( selected == false )
		count += GetObjectCount( MAPOBJ_CLASS_BRUSH );

	fs << "0\nTABLE\n2\nLAYER\n70\n" << count << "\n";
	fs << "0\nLAYER\n2\n0\n70\n0\n62\n7\n6\nCONTINUOUS\n";
	fs << "0\nENDTAB\n";
	fs << "0\nTABLE\n2\nSTYLE\n70\n1\n0\nSTYLE\n2\nSTANDARD\n70\n0\n40\n0.0\n41\n1.0\n50\n0.0\n71\n0\n42\n0.2\n3\ntxt\n4\n\n0\nENDTAB\n";
	fs << "0\nTABLE\n2\nUCS\n70\n0\n0\nENDTAB\n";
	fs << "0\nENDSEC\n";
	fs << "0\nSECTION\n2\nENTITIES\n";

	m_ObjectList.WalkList( WalkWriteBrushMap, &data );
	m_SelectedObjectList.WalkList( WalkWriteBrushMap, &data );

	fs << "0\nENDSEC\n0\nEOF\n";

	fs.close();

	return false;
}

void CTreadDoc::OnFileExportbrushfile() 
{
	// TODO: Add your command handler code here
	CString sPath;
	CString sDefaultName = "";
	sDefaultName = GetTitle();
	int nIndex = sDefaultName.Find('.');
	if(nIndex != -1)
		sDefaultName = sDefaultName.Left(nIndex);

	sDefaultName += ".dxf";

	CFileDialog dlgTemp(false, "dxf", sDefaultName,
			OFN_NONETWORKBUTTON|OFN_OVERWRITEPROMPT, "DXF Files (*.dxf)|*.dxf|All Files (*.*)|*.*||", Sys_GetMainFrame());

	dlgTemp.m_ofn.lpstrTitle = "Export As .DXF File";
	if(dlgTemp.DoModal() != IDOK)
		return;

	sPath = dlgTemp.GetPathName();
	ExportBrushFile( sPath, false );
}

void CTreadDoc::OnUpdateFileExportbrushfile(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( (m_ObjectList.IsEmpty()==FALSE) || (m_SelectedObjectList.IsEmpty()==FALSE) );
}

void CTreadDoc::OnFileExportselectedtobrushfile() 
{
	// TODO: Add your command handler code here
	CString sPath;
	CString sDefaultName = "";
	sDefaultName = GetTitle();
	int nIndex = sDefaultName.Find('.');
	if(nIndex != -1)
		sDefaultName = sDefaultName.Left(nIndex);

	sDefaultName += ".dxf";

	CFileDialog dlgTemp(false, "dxf", sDefaultName,
			OFN_NONETWORKBUTTON|OFN_OVERWRITEPROMPT, "DXF Files (*.dxf)|*.dxf|All Files (*.*)|*.*||", Sys_GetMainFrame());

	dlgTemp.m_ofn.lpstrTitle = "Export As .DXF File";
	if(dlgTemp.DoModal() != IDOK)
		return;

	sPath = dlgTemp.GetPathName();
	ExportBrushFile( sPath, true );
}

void CTreadDoc::OnUpdateFileExportselectedtobrushfile(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( m_SelectedObjectList.IsEmpty()==FALSE );
}

void CTreadDoc::OnToolsInsertjmodelscenefile() 
{
	// TODO: Add your command handler code here
	CString sPath;
	CString sDefaultName = "";
	sDefaultName = GetTitle();
	int nIndex = sDefaultName.Find('.');
	if(nIndex != -1)
		sDefaultName = sDefaultName.Left(nIndex);

	sDefaultName += ".jscene";

	CFileDialog dlgTemp(true, "jscene", sDefaultName,
			OFN_NONETWORKBUTTON|OFN_HIDEREADONLY, "JModel Scene Files (*.jscene)|*.jscene|All Files (*.*)|*.*||", Sys_GetMainFrame());

	dlgTemp.m_ofn.lpstrTitle = "Insert JModel Scene File";
	if(dlgTemp.DoModal() != IDOK)
		return;

	sPath = dlgTemp.GetPathName();
	//if( LoadJModelScene( sPath, this ) == false )
	//{
	//	MessageBox( 0, "Scene Import Failed!", "Warning!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
	//}

	Sys_RedrawWindows();
}

void CTreadDoc::OnToolsViewlighting() 
{
	// TODO: Add your command handler code here
	/*m_bViewLighting = !m_bViewLighting;

	R_EnableLightingPreview( this, m_bViewLighting );

	if( m_bAnimateMaterials && m_bViewLighting )
	{
		AnimateMaterials();
	}
	else
	{
		Sys_RedrawWindows( VIEW_TYPE_3D );
	}*/
}

void CTreadDoc::OnUpdateToolsViewlighting(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	/*if( R_SupportsLightingPreview() )
	{
		pCmdUI->Enable();
		pCmdUI->SetCheck( m_bViewLighting );
	}
	else*/
	{
		pCmdUI->Enable( FALSE );
		pCmdUI->SetCheck( 0 );
	}
}

void CTreadDoc::OnToolsAnimatematerials() 
{
	// TODO: Add your command handler code here
	m_bAnimateMaterials = !m_bAnimateMaterials;

	if( m_bAnimateMaterials && m_bViewLighting )
		AnimateMaterials();
	else
	if( m_bViewLighting )
	{
		m_spAnimDoc = 0;
		//R_EnableLightingPreview( this, false );
		//R_EnableLightingPreview( this, true );
		Sys_RedrawWindows( VIEW_TYPE_3D );
	}
}

void CTreadDoc::OnUpdateToolsAnimatematerials(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	//if( R_SupportsLightingPreview() )
	//{
	//	pCmdUI->Enable();
	//	pCmdUI->SetCheck( m_bAnimateMaterials );
	//}
	//else
	{
		pCmdUI->Enable( FALSE );
		pCmdUI->SetCheck( 0 );
	}
}

void CTreadDoc::OnToolsEnableshadows() 
{
	// TODO: Add your command handler code here
	m_bViewShadows = !m_bViewShadows;
	Sys_RedrawWindows( VIEW_TYPE_3D );
}

void CTreadDoc::OnUpdateToolsEnableshadows(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	/*if( R_SupportsLightingPreview() )
	{
		pCmdUI->Enable();
		pCmdUI->SetCheck( m_bViewShadows );
	}
	else*/
	{
		pCmdUI->Enable( FALSE );
		pCmdUI->SetCheck( 0 );
	}
}

void CTreadDoc::OnToolsDisplayshaders() 
{
	// TODO: Add your command handler code here
	m_bViewShaders = !m_bViewShaders;
	Sys_RedrawWindows( VIEW_TYPE_3D );
}

void CTreadDoc::OnUpdateToolsDisplayshaders(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	/*if( R_SupportsLightingPreview() )
	{
		pCmdUI->Enable();
		pCmdUI->SetCheck( m_bViewShaders );
	}
	else*/
	{
		pCmdUI->Enable( FALSE );
		pCmdUI->SetCheck( 0 );
	}
}

void CTreadDoc::AnimateMaterials()
{
	unsigned int last_micro, cur_micro;
	float elapsed;

	if( m_spAnimDoc )
	{
		m_spAnimDoc->SetAnimateMaterialsFlag( FALSE );
	}

	m_spAnimDoc = this;

	last_micro = ReadTickMicroseconds();
	cur_micro = last_micro;

	for(;;)
	{
		if( m_spAnimDoc != this || !m_bViewLighting || !m_bAnimateMaterials )
			break;

		cur_micro = ReadTickMicroseconds();
		elapsed = (float)(cur_micro-last_micro)/(float)MICROSECONDS_PER_MILLISECOND;
		last_micro = cur_micro;

//		R_TimeUpdate( this, elapsed );
		Sys_RedrawWindows( VIEW_TYPE_3D );

		if( !Sys_GetMainFrame()->PumpMessages() )
		{
			if( m_spAnimDoc == this )
				m_spAnimDoc = 0;
			return;
		}
	}

	if( m_spAnimDoc == this )
		m_spAnimDoc = 0;
}

void CTreadDoc::OnFileImportplane() 
{
	// TODO: Add your command handler code here
	
	/*CString sPath;
	CString sDefaultName = "";
		
	CFileDialog dlgTemp(true, "plane", sDefaultName,
			OFN_NONETWORKBUTTON|OFN_HIDEREADONLY, "Plane Files (*.plane)|*.plane|All Files (*.*)|*.*||", Sys_GetMainFrame());

	dlgTemp.m_ofn.lpstrTitle = "Insert Plane File";
	if(dlgTemp.DoModal() != IDOK)
		return;

	sPath = dlgTemp.GetPathName();
	C_Tokenizer script;
	char* data;
	DWORD size;
	CFile hFile;

	if( hFile.Open( sPath, CFile::typeBinary|CFile::modeRead ) == false )
	{
		MessageBox( 0, "Error opening file for read!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	size = hFile.GetLength();
	if( size < 1 )
	{
		MessageBox( 0, "Bad File Data!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	data = new char[size];
	hFile.Read( data, size );
	hFile.Close();

	script.InitParsing( data, size );
	delete[] data;

	CString t;
	if( script.GetToken( t ) || t != "PLANE_FILE" )
	{
		MessageBox( 0, "Not a plane file!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
		return;
	}	

	ClearSelection();

	CMapObject* obj;
	for(;;)
	{
		obj = CQBrush::BrushFromPlaneFile( this, script );
		if( !obj )
			break;

		AssignUID( obj );
		obj->SetName( MakeUniqueObjectName( obj ) );
		AddObjectToMap( obj );
		Prop_AddObject( obj );
		obj->Select( this );
	}

	GroupSelection();
	UpdateSelectionInterface();
	Prop_UpdateSelection();
	Sys_RedrawWindows();*/
}

void CTreadDoc::OnUpdateFileImportplane(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnToolsReloadlighttextures() 
{
	// TODO: Add your command handler code here
	Sys_GetMainFrame()->GlobalGLLock();

//	R_ReloadLightTextures();

	Sys_GetMainFrame()->GlobalGLUnlock();
	Sys_RedrawWindows( VIEW_TYPE_3D );
}

void CTreadDoc::OnToolsWriteshaderfiles() 
{
	// TODO: Add your command handler code here
//	WriteShaderFiles();
//	Sys_SetShadersDirty( FALSE );
}

void CTreadDoc::OnToolsSelectobjectbyuid() 
{
	// TODO: Add your command handler code here
	CInputLineDialog dlg;

	dlg.AllowEmpty(false);
	dlg.SetFilter( FILTER_INT );
	dlg.SetTitle( "Select Object By UID" );

	if( dlg.DoModal() == IDOK )
	{
		int uid = atoi( dlg.GetValue() );
		CMapObject* obj = ObjectForUID( uid );
		if( obj )
		{
			MakeUndoDeselectAction();
			ClearSelection();
			obj->Select( this );
			CEntity* ent = dynamic_cast<CEntity*>(obj);
			if( ent ) ent->SelectOwnedObjects(this);
			UpdateSelectionInterface();

			Sys_RedrawWindows(VIEW_FLAG_MAP);
		}
	}
}

void CTreadDoc::OnViewVertices() 
{
	// TODO: Add your command handler code here
	if( IsEditingVerts() )
	{
		SetEditingVerts( FALSE );
		m_gamePlugin->EnterVertexMode(this, false);
	}
	else
	{
		if( IsEditingFaces() )
		{
			SetEditingFaces( FALSE );
			m_gamePlugin->EnterFaceMode(this, false);
		}

		SetEditingVerts();
		m_gamePlugin->EnterFaceMode(this, true);
	}

	UpdateSelectionInterface();
	Prop_UpdateSelection();
	Sys_RedrawWindows();
}

void CTreadDoc::OnViewFaces() 
{
	// TODO: Add your command handler code here
	if( IsEditingFaces() )
	{
		SetEditingFaces( FALSE );
		m_gamePlugin->EnterFaceMode(this, false);
	}
	else
	{
		if( IsEditingVerts() )
		{
			SetEditingVerts( FALSE );
			m_gamePlugin->EnterVertexMode(this, false);
		}

		SetEditingFaces();
		m_gamePlugin->EnterFaceMode(this, true);
	}

	UpdateSelectionInterface();
	Prop_UpdateSelection();
	Sys_RedrawWindows();
}

void CTreadDoc::OnUpdateViewFaces(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( IsEditingFaces() );
}

void CTreadDoc::OnUpdateViewVertices(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( IsEditingVerts() );
}

void CTreadDoc::OnViewTogglebrushes() 
{
	// TODO: Add your command handler code here
	if( m_bToggleBrushes )
	{
		WalkObjects( WalkShowObjectsWithBits, (void*)(OBJECT_TYPE_WORLD|OBJECT_TYPE_BMODEL), true, false );
	}
	else
	{
		WalkObjects( WalkHideObjectsWithBits, (void*)(OBJECT_TYPE_WORLD|OBJECT_TYPE_BMODEL), true, false );
	}

	m_bToggleBrushes = !m_bToggleBrushes;

	UpdateSelectionInterface();
	Prop_UpdateSelection();
	Sys_RedrawWindows();
}

void CTreadDoc::OnViewHideshowjmodels() 
{
	// TODO: Add your command handler code here
	if( m_bToggleJmodels )
	{
		WalkObjects( WalkShowObjectsWithBits, (void*)(OBJECT_TYPE_JMODEL), true, false );
	}
	else
	{
		WalkObjects( WalkHideObjectsWithBits, (void*)(OBJECT_TYPE_JMODEL), true, false );
	}

	m_bToggleJmodels = !m_bToggleJmodels;

	UpdateSelectionInterface();
	Prop_UpdateSelection();
	Sys_RedrawWindows();
}

void CTreadDoc::OnSelectionOpengroupmode() 
{
	// TODO: Add your command handler code here
	m_bOpenGroups = !m_bOpenGroups;
}

void CTreadDoc::OnUpdateSelectionOpengroupmode(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck( AreGroupsOpen() );
}

void CTreadDoc::OnViewProperties()
{
	// TODO: Add your command handler code here
	if( Sys_GetMainFrame()->GetObjectPropertiesDialog()->IsWindowVisible() )
		Sys_GetMainFrame()->GetObjectPropertiesDialog()->ShowWindow( SW_HIDE );
	else
		Sys_GetMainFrame()->GetObjectPropertiesDialog()->ShowWindow( SW_SHOW );
	
	Sys_GetPropView()->UpdateExpandPropsButton();
}

void CTreadDoc::OnUpdateViewProperties(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}

void CTreadDoc::OnEditWorldspawn()
{
	// TODO: Add your command handler code here
	Sys_GetMainFrame()->DisplayWorldspawnProperties(this);
}

void CTreadDoc::OnUpdateEditWorldspawn(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GamePlugin()->GetWorldspawnProps(this) != 0);
}
