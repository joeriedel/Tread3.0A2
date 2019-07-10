///////////////////////////////////////////////////////////////////////////////
// ObjPropView.cpp
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
#include "TreadDoc.h"
#include "ScriptDialog.h"
#include "RealtimeColorDialog.h"
#include "MainFrm.h"
#include "ObjectPropertiesDialog.h"

#include "System.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjPropView

IMPLEMENT_DYNCREATE(CObjPropView, CFormView)

CObjPropView::CObjPropView()
	: CFormView(CObjPropView::IDD)
{
	//{{AFX_DATA_INIT(CObjPropView)
	m_sName = _T("");
	//}}AFX_DATA_INIT
	Sys_RegisterPropView( this );

	m_bUserChange = true;
}

CObjPropView::~CObjPropView()
{
}

void CObjPropView::ObjTreeLoad()
{
	m_createTree.LoadTree(m_pDoc);
}

void CObjPropView::TreeEnableDraw( bool draw )
{
	m_Tree.SetRedraw( draw );
}

void CObjPropView::ClearPropControls()
{
	m_sName = "";
	m_Props.Clear();
	m_Props.ResetContent();
}

void CObjPropView::HidePropControls()
{
	GetDlgItem( IDC_STRING_EDIT )->ShowWindow(SW_HIDE);
	m_lcSubList.ShowWindow(SW_HIDE);
	m_colorstatic.ShowWindow(SW_HIDE);
	GetDlgItem( IDC_EDIT_SCRIPT )->ShowWindow(SW_HIDE);
}

CObjPropView::CDProp::CDProp() : CObjProp()
{
	bSupress = false;
}

CObjPropView::CDProp::~CDProp()
{
}

CObjPropView::CDProp::CDProp( const CDProp& prop ) : CObjProp( prop )
{
	bSupress = prop.bSupress;
}

void CObjPropView::AddProp( CObjProp* srcp, CLinkedList<CMapObject>* objlist, CTreadDoc* pDoc )
{
	CMapObject* obj;
	CDProp* dp;
	CObjProp* p;
	CLinkedList<CObjProp>* propl;

	//
	// is it in the prop list?
	//
	for( dp = m_PropList.ResetPos(); dp; dp = m_PropList.GetNextItem() )
	{
		if( !stricmp(dp->GetName(), srcp->GetName()) )
			break;
	}

	if( dp )
		return;

	//
	// not in the prop list, see if it exists in ALL objects.
	//
	bool supress = false;

	if (objlist)
	{
		for( obj = objlist->ResetPos(); obj; obj = objlist->GetNextItem() )
		{
			propl = obj->GetPropList(pDoc);
			objlist->SetPosition( obj );
			if( !propl )
				continue;
			
			bool found = false;

			for( p = propl->ResetPos(); p; p = propl->GetNextItem() )
			{
				if( !stricmp(p->GetName(), srcp->GetName()) &&
					p->GetType() == srcp->GetType() )
				{
					if( strcmp(p->GetString(), srcp->GetString()) )
					{
						supress = true;
					}

					found = true;
					break;
				}
			}

			if( !found )
				break;
		}

		if( obj )
		{
			//
			// this object did not have the prop in it, so we won't add.
			//
			return;
		}
	}

	//
	// property is compatible, but doesn't exist yet... add it.
	//
	
	dp = new CDProp();
	dp->Copy( *srcp );
	m_PropList.AddItem( dp );
	dp->bSupress = supress;
	if( supress )
		dp->SetString( "" );
}

void CObjPropView::OnPropValChange()
{
	//
	// the user changed the value.
	//
	UpdateSubItemStates( m_pSelProp );
}

void CObjPropView::LoadPropList()
{
	m_Props.ResetContent();
	Sys_GetMainFrame()->GetObjectPropertiesDialog()->GetPropList()->DeleteAllItems();

	if( m_PropList.IsEmpty() )
	{
		m_Props.EnableWindow( FALSE );
		Sys_GetMainFrame()->GetObjectPropertiesDialog()->EnableControls(FALSE);
		
		return;
	}

	m_Props.EnableWindow();
	Sys_GetMainFrame()->GetObjectPropertiesDialog()->EnableControls(TRUE);
	
	CObjProp* p;

	int ofs = 0;

	for( p = m_PropList.ResetPos(); p; p = m_PropList.GetNextItem() )
	{
		int i = m_Props.AddString( p->GetDisplayName() );
		m_Props.SetItemData( i, (DWORD)p );

		int n = Sys_GetMainFrame()->GetObjectPropertiesDialog()->GetPropList()->InsertItem( ofs, p->GetDisplayName(), -1 );
		Sys_GetMainFrame()->GetObjectPropertiesDialog()->GetPropList()->SetItemText( n, 1, p->GetString() );
		Sys_GetMainFrame()->GetObjectPropertiesDialog()->GetPropList()->SetItemData( n, (DWORD)p );
		p->SetListItem( n );

		ofs++;
	}
}

void CObjPropView::MakePropList( CLinkedList<CMapObject>* pList, CTreadDoc* pDoc )
{
	CObjProp* p;
	CMapObject* o;
	CLinkedList<CObjProp>* propl;

	m_PropList.DestroyList();
	m_VirginPropList.DestroyList();

	if (pList)
	{
		for( o = pList->ResetPos(); o; o = pList->GetNextItem() )
		{
			propl = o->GetPropList( pDoc );
			if( propl )
			{
				for( p = propl->ResetPos(); p ; p = propl->GetNextItem() )
				{
					AddProp( p, pList, pDoc );
					propl->SetPosition( p );
				}
			}
			pList->SetPosition( o );
		}
	}
	else
	{
		propl = pDoc->GamePlugin()->GetWorldspawnProps(pDoc);
		if(propl)
		{
			for( p = propl->ResetPos(); p ; p = propl->GetNextItem() )
			{
				AddProp( p, 0, pDoc );
				propl->SetPosition( p );
			}
		}
	}

	m_VirginPropList.Copy(m_PropList);
}

void CObjPropView::PropChange( CTreadDoc* pDoc, const char* name )
{

	//
	// check objects.
	//
	bool supress = false;
	CMapObject* obj;
	CObjProp* p;
	CLinkedList<CObjProp>* propl;
	CString sVal = "";

	for( obj = pDoc->GetSelectedObjectList()->ResetPos(); obj; obj = pDoc->GetSelectedObjectList()->GetNextItem() )
	{
		propl = obj->GetPropList( pDoc );
		if( propl )
		{
			for( p = propl->ResetPos(); p; p = propl->GetNextItem() )
			{
				if( !strcmp( p->GetName(), name ) )
				{
					if( sVal == "" )
					{
						sVal = p->GetString();
					}
					else
					if( sVal != p->GetString() )
					{
						break;
					}
				}
			}

			if( p )
				break;
		}

	}

	if( obj )
		supress = true;

	if( supress )
	{
		sVal = "";
	}
	
	CDProp* dp, *dvp;

	for( dp = m_PropList.ResetPos(), dvp = m_VirginPropList.ResetPos(); dp && dvp; dp = m_PropList.GetNextItem(), dvp = m_VirginPropList.GetNextItem() )
	{
		if( !strcmp( dp->GetName(), name ) )
		{
			if( !supress )
			{
				dp->SetString( sVal );
				dvp->SetString( sVal );
			}

			dp->bSupress = supress;
			Sys_GetMainFrame()->GetObjectPropertiesDialog()->UpdateProp( dp );
		}
	}

	if( m_pSelProp )
	{
		if( !stricmp( m_pSelProp->GetName(), name ) )
		{
			m_bUserChange = false;
			m_String.SetWindowText( sVal );
		}
	}
}

void CObjPropView::EnablePropControls( bool enable )
{
	GetDlgItem( IDC_NAME )->EnableWindow( enable );
	GetDlgItem( IDC_PROPERTIES )->EnableWindow( enable );
	GetDlgItem( IDC_APPLY )->EnableWindow( enable );
}

bool CObjPropView::PropsDiffer()
{
	CDProp* p, *vp;

	for( p = m_PropList.ResetPos(), vp = m_VirginPropList.ResetPos(); p && vp; p = m_PropList.GetNextItem(), vp = m_VirginPropList.GetNextItem() )
	{
		if( strcmp( p->GetString(), vp->GetString() ) )
			return true;
	}

	return false;
}

void CObjPropView::TempApplySingleProp( CObjProp* prop, CTreadDoc* pDoc )
{
	CMapObject* obj;

	for( obj = pDoc->GetSelectedObjectList()->ResetPos(); obj; obj = pDoc->GetSelectedObjectList()->GetNextItem() )
	{
		obj->SetProp( pDoc, prop );
	}
}

void CObjPropView::ApplyProps( CTreadDoc* pDoc )
{
	CMapObject* obj;
	CDProp* p, *vp;

	if (m_worldspawn)
	{
		CLinkedList<CObjProp> *worldspawn = pDoc->GamePlugin()->GetWorldspawnProps(pDoc);
		for( p = m_PropList.ResetPos(); p; p = m_PropList.GetNextItem() )
		{
			if( p->bSupress == false )
			{
				CObjProp *prop = CObjProp::FindProp(worldspawn, p->GetName());
				if (prop)
				{
					prop->SetString(p->GetString());
				}
			}
			m_PropList.SetPosition( p ); // don't let obj->SetProp() change our position.
		}
	}
	else
	{
		for( obj = pDoc->GetSelectedObjectList()->ResetPos(); obj; obj = pDoc->GetSelectedObjectList()->GetNextItem() )
		{
			for( p = m_PropList.ResetPos(); p; p = m_PropList.GetNextItem() )
			{
				if( p->bSupress == false )
					obj->SetProp( pDoc, p );
				m_PropList.SetPosition( p ); // don't let obj->SetProp() change our position.
			}
		}
	}

	for( p = m_PropList.ResetPos(), vp = m_VirginPropList.ResetPos(); p && vp; p = m_PropList.GetNextItem(), vp = m_VirginPropList.GetNextItem() )
	{
		if( p->bSupress == false )
			vp->SetString( p->GetString() );
	}
}

void CObjPropView::UpdateApply()
{
	if( PropsDiffer() )
	{
		GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
	}
}

void CObjPropView::OnSelProp()
{
	int i = m_Props.GetCurSel();
	if( i == -1 )
	{
		m_pSelProp = 0;
		m_sSelProp = "";
		HidePropControls();
		return;
	}

	CDProp* p = m_PropList.GetItem( i );
	if( p )
	{
		if( p == m_pSelProp )
			return;

		HidePropControls();

		//
		// they *all* get to type in the string...
		//
		m_String.EnableWindow();
		m_String.ShowWindow(SW_SHOWNORMAL);	

		m_bUserChange = false;

		if( p->bSupress )
			m_String.SetWindowText( "" );
		else
			m_String.SetWindowText( p->GetString() );

		
		if( p->GetType() == CObjProp::color )
		{
			m_colorstatic.SetColor( p->GetVector() );
			m_colorstatic.ShowWindow( SW_SHOW );
		}
		if( p->GetType() == CObjProp::script )
		{
			GetDlgItem( IDC_EDIT_SCRIPT )->ShowWindow( SW_SHOW );
		}

		//
		// set the filter styles on the different types.
		//
		m_String.SetFilter( 0 );
		switch( p->GetType() )
		{
		case 1: // integer
		case 5: // facing

			m_String.SetFilter( FILTER_INT );

		break;

		case 2: // float

			m_String.SetFilter( FILTER_FLOAT );

		break;

		case 6: // color (non-negative) floats

			m_String.SetFilter( FILTER_MULTI_FLOAT|FILTER_DASH ); 

		break;

		case 4: // vector

			m_String.SetFilter( FILTER_MULTI_FLOAT );

		break;

		case 3:
		case 7:

			m_String.SetFilter( FILTER_TEXT );

		break;
		}


		if( p->GetChoices()->IsEmpty() == false )
		{
			//
			// load the choice window.
			//
			if( p->GetSubType() == false &&
				p->GetType() != CObjProp::integer )
			{
				//
				// it is illegal to have a set of bitwise orable options on
				// anything but an integer type.
				//
				goto nosublist;
			}

			m_lcSubList.ShowWindow(SW_SHOW);
			m_lcSubList.DeleteAllItems();

			CObjProp* sub;
			int c, n;
			bool check = false;

			for( c = 0, sub = p->GetChoices()->ResetPos(); sub; sub = p->GetChoices()->GetNextItem() )
			{
				n = m_lcSubList.InsertItem( c++, sub->GetDisplayName(), 0 );
				m_lcSubList.SetItemData( n, (DWORD)sub );
				sub->SetListItem( n );
			}

			UpdateSubItemStates( p );
		}

nosublist:

		m_pSelProp = p;
		m_sSelProp = p->GetName();
	}
	else
	{
		HidePropControls();
		m_pSelProp = 0;
		m_sSelProp = "";
	}
}

void CObjPropView::UpdateSubItemStates( CObjProp* prop )
{
	CObjProp* sub;
	
	if( prop->GetChoices()->IsEmpty() )
		return;

	if( prop->GetSubType() )
	{
		for( sub = prop->GetChoices()->ResetPos(); sub; sub = prop->GetChoices()->GetNextItem() )
		{
			if( !strcmp( sub->GetString(), prop->GetString() ) )
			{
				LVITEM item;

				item.mask = LVIF_STATE;
				item.iItem = sub->GetListItem();
				item.iSubItem = 0;
				item.stateMask = LVIS_SELECTED;
				item.state = LVIS_SELECTED;

				m_lcSubList.SetItemState( sub->GetListItem(), &item );
			}
			else
			{
				LVITEM item;

				item.mask = LVIF_STATE;
				item.iItem = sub->GetListItem();
				item.iSubItem = 0;
				item.stateMask = LVIS_SELECTED;
				item.state = 0;

				m_lcSubList.SetItemState( sub->GetListItem(), &item );
			}
		}
	}
	else
	{
		for( sub = prop->GetChoices()->ResetPos(); sub; sub = prop->GetChoices()->GetNextItem() )
		{
			LVITEM item;

			item.mask = LVIF_STATE;
			item.iItem = sub->GetListItem();
			item.iSubItem = 0;
			item.stateMask = LVIS_SELECTED;
			item.state = 0;
			
			m_lcSubList.SetItemState( sub->GetListItem(), &item );

			bool check = (prop->GetInt()&sub->GetInt())?true:false;
			m_lcSubList.SetCheck( sub->GetListItem(), check );
		}
	}
}

void CObjPropView::TrySelectProp( const char* szName )
{
	int i;
	CDProp* p;

	i = 0;
	for( p = m_PropList.ResetPos(); p ; p = m_PropList.GetNextItem() )
	{
		if( !stricmp( szName, p->GetName() ) )
			break;

		i++;
	}

	if( p )
	{
		m_Props.SetCurSel( i );
	}

	OnSelProp();
}

void CObjPropView::LoadProperties( CTreadDoc* pDoc, bool worldspawn )
{
	m_worldspawn = worldspawn;

	ClearPropControls();
	HidePropControls();
	EnablePropControls();

	m_pSelProp = 0;

	GetDlgItem( IDC_SET_NAME )->EnableWindow(FALSE);

	if(!worldspawn && pDoc->GetSelectedObjectCount() == 1)
	{
		GetDlgItem( IDC_NAME )->EnableWindow(TRUE);
		m_sName = pDoc->GetSelectedObjectList()->GetItem( LL_HEAD )->GetName();
	}
	else
	{
		GetDlgItem( IDC_NAME )->EnableWindow(FALSE);
	}

	MakePropList( worldspawn ? 0 : pDoc->GetSelectedObjectList(), pDoc );
	LoadPropList();

	m_Props.SetCurSel( 0 );
	TrySelectProp( m_sSelProp );
	GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);

	m_bUserChange = false;
	UpdateData(FALSE);
	m_bUserChange = true;
}

void CObjPropView::TreeSelectObject( int uid )
{
	m_Tree.SelectItemUID( uid );
}

CTreadDoc* CObjPropView::GetDoc()
{
	return m_pDoc;
}

void CObjPropView::SetDoc( CTreadDoc* pDoc )
{
	m_pDoc = pDoc;	
}

void CObjPropView::LoadWorldspawn()
{
	LoadProperties(m_pDoc, true);
}

void CObjPropView::LoadSelection()
{
	if( m_pDoc && m_pDoc->GetSelectedObjectCount() > 0 )
	{
		/*if( m_pDoc->IsInTrackAnimationMode() )
		{
			if( m_pDoc->GetSelectedObjectCount( MAPOBJ_CLASS_SPLINETRACK ) != 
				m_pDoc->GetSelectedObjectCount() )
			{
				goto badselection;
			}
		}*/

		LoadProperties(m_pDoc, false);
	}
	else
	{
//badselection:

		m_pSelProp = 0;
		//m_sSelProp = "";
		ClearPropControls();
		HidePropControls();
		EnablePropControls(FALSE);
		UpdateData(FALSE);

		Sys_GetMainFrame()->GetObjectPropertiesDialog()->EnableControls( FALSE );
		Sys_GetMainFrame()->GetObjectPropertiesDialog()->GetPropList()->DeleteAllItems();

		GetDlgItem( IDC_SET_NAME )->EnableWindow(FALSE);
	}
}

void CObjPropView::ClearDocControls()
{
	m_Tree.DeleteAllItems();
	m_cbTextureFilter.ResetContent();
	m_Nudge.SetWindowText( "" );
}

void CObjPropView::EnableDocControls( bool enable )
{
	m_Tree.EnableWindow( enable );
	m_cbTextureFilter.EnableWindow(enable);
	m_Nudge.EnableWindow(enable);

	/*GetDlgItem( IDC_RADIO_NONE )->EnableWindow(enable);
	GetDlgItem( IDC_RADIO_BRUSH )->EnableWindow(enable);
	GetDlgItem( IDC_RADIO_JMODEL )->EnableWindow(enable);
	GetDlgItem( IDC_RADIO_ENTITY )->EnableWindow(enable);*/

	GetDlgItem( IDC_TEXTURE_FILTER_SET_BUTTON )->EnableWindow(enable);
	m_createTree.EnableWindow( enable );
}

void CObjPropView::LoadDocument()
{
	ObjTreeLoad();

	if( !m_pDoc )
	{
		ClearDocControls();
		EnableDocControls( false );
		return;
	}

	ClearDocControls();
	EnableDocControls( true );
	LoadTextureFilters();

	{
		char buff[256];

		m_Nudge.SetFilter( FILTER_CHAR|FILTER_PUNCTUATION|FILTER_SPACE );
	
		sprintf(buff, "%.3f", m_pDoc->GetNudgeFactor() );
		m_Nudge.SetWindowText( buff );
	}

	TreeLoadObjects( m_pDoc );
}

void CObjPropView::LoadTextureFilters()
{
	m_cbTextureFilter.ResetContent();
	
	int i, num;
	CString* s;

	s = m_pDoc->GetTextureFilterHistory( &num );
	for(i = 0; i < num; i++)
	{
		m_cbTextureFilter.AddString( s[i] );
	}

	m_cbTextureFilter.SetWindowText( m_pDoc->GetTextureFilter() );
	m_cbTextureFilter.SetEditSel( -1, -1 );

	UpdateTextureFilterApply();
}

void CObjPropView::UpdateTextureFilterApply()
{
	CString s;
	m_cbTextureFilter.GetWindowText( s );

	GetDlgItem( IDC_TEXTURE_FILTER_SET_BUTTON )->EnableWindow( s.CompareNoCase( m_pDoc->GetTextureFilter() ) );
}

void CObjPropView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjPropView)
	DDX_Control(pDX, IDC_CHOICE_LIST, m_lcSubList);
	DDX_Control(pDX, IDC_NUDGE_EDIT, m_Nudge);
	DDX_Control(pDX, IDC_TEXTURE_FILTER_COMBO, m_cbTextureFilter);
	DDX_Control(pDX, IDC_STRING_EDIT, m_String);
	DDX_Control(pDX, IDC_PROPERTIES, m_Props);
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDV_MaxChars(pDX, m_sName, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjPropView, CFormView)
	//{{AFX_MSG_MAP(CObjPropView)
	ON_EN_CHANGE(IDC_STRING_EDIT, OnChangeStringEdit)
	ON_CBN_SELCHANGE(IDC_PROPERTIES, OnSelchangeProperties)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_BN_CLICKED(IDC_SET_NAME, OnSetName)
	ON_CBN_SELCHANGE(IDC_TEXTURE_FILTER_COMBO, OnSelchangeTextureFilterCombo)
	ON_BN_CLICKED(IDC_TEXTURE_FILTER_SET_BUTTON, OnTextureFilterSetButton)
	ON_CBN_EDITCHANGE(IDC_TEXTURE_FILTER_COMBO, OnEditchangeTextureFilterCombo)
	ON_EN_CHANGE(IDC_NUDGE_EDIT, OnChangeNudgeEdit)
	ON_NOTIFY(NM_CLICK, IDC_CHOICE_LIST, OnClickChoiceList)
	ON_BN_CLICKED(IDC_COLOR_STATIC, OnColorStatic)
	ON_BN_CLICKED(IDC_EDIT_SCRIPT, OnEditScript)
	ON_BN_CLICKED(IDC_SHOW_PROPERTIES_DIALOG, OnShowPropertiesDialog)
	ON_CBN_CLOSEUP(IDC_PROPERTIES, OnCloseupProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjPropView diagnostics

#ifdef _DEBUG
void CObjPropView::AssertValid() const
{
	CFormView::AssertValid();
}

void CObjPropView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CObjPropView message handlers

void CObjPropView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	m_colorstatic.SubclassDlgItem( IDC_COLOR_STATIC, this );
	//m_colorstatic.SetColor( vec3( 1, 0, 0 ) );

	// TODO: Add your specialized code here and/or call the base class
	m_ImageList.Create(IDB_TREE_IMAGES, 16, 0, RGB(255, 0, 255));
	m_Tree.SubclassDlgItem( IDC_VIS_OBJECTS, this );
	m_Tree.SetImageList( &m_ImageList, TVSIL_NORMAL );
	
	m_createTree.SubclassDlgItem( IDC_CREATOR_TREE, this );
	
	m_SubImageList.Create( IDB_CHECK_BUTTONS, 16, 0, RGB(255, 0, 255) );
	m_lcSubList.SetImageList( &m_SubImageList, LVSIL_STATE );
	m_lcSubList.InsertColumn( 0, "", LVCFMT_LEFT, 500, -1 );

	SetDoc( 0 );
	ObjTreeLoad();
	LoadSelection();
	LoadDocument();
}

void CObjPropView::TreeShowGroup( CObjectGroup* gr )
{
	m_Tree.ShowGroup( gr );
}

void CObjPropView::TreeAddObject( CMapObject* obj )
{
	m_Tree.AddObject( obj );
}

void CObjPropView::TreeRemoveObject( CMapObject* obj )
{
	m_Tree.RemoveObject( obj );
}

void CObjPropView::TreeAddGroup( CObjectGroup* obj )
{
	m_Tree.AddGroup( obj );
}

void CObjPropView::TreeRemoveGroup( CObjectGroup* obj )
{
	m_Tree.RemoveGroup( obj );
}

void CObjPropView::TreeUpdateObjectState( CMapObject* obj )
{
	m_Tree.ObjectStateChange( obj );
}

void CObjPropView::TreeUpdateGroupState( CObjectGroup* obj )
{
	m_Tree.GroupStateChange( obj );
}

void CObjPropView::TreeLoadObjects( CTreadDoc* pDoc )
{
	m_Tree.SetDoc( pDoc );
	m_Tree.BuildTree();
}

void CObjPropView::ExternalPropChange(CObjProp* prop)
{
	m_bUserChange = false;

	CDProp* dp;
	for( dp = m_PropList.ResetPos(); dp ; dp = m_PropList.GetNextItem() )
	{
		if( !stricmp( dp->GetName(), prop->GetName() ) )
		{
			dp->bSupress = false;
		}
	}

	if( m_pSelProp )
	{
		m_String.SetWindowText( m_pSelProp->GetString() );
		
		if( m_pSelProp->GetType() == CObjProp::color )
			m_colorstatic.SetColor( m_pSelProp->GetVector() );

		UpdateSubItemStates( m_pSelProp );
		UpdateApply();
	}
	m_bUserChange = true;
}

void CObjPropView::OnChangeStringEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	//
	// set the color...
	//
	if( m_pSelProp )
	{
		if( m_bUserChange )
		{
			CString s;
			m_String.GetWindowText( s );
			m_pSelProp->SetString( s );

			if( m_pSelProp->GetType() == CObjProp::color )
				m_colorstatic.SetColor( m_pSelProp->GetVector() );

			m_pSelProp->bSupress = false;
			OnPropValChange();
			UpdateApply();
		}

		Sys_GetMainFrame()->GetObjectPropertiesDialog()->UpdateProp( m_pSelProp );
	}

	m_bUserChange = true;
}

void CObjPropView::OnSelchangeProperties() 
{
	// TODO: Add your control notification handler code here
	OnSelProp();
}

void CObjPropView::ExternalApply()
{
	OnApply();
}

void CObjPropView::OnApply() 
{
	// TODO: Add your control notification handler code here
	ApplyProps( m_pDoc );
	GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
	Sys_RedrawWindows();
}

void CObjPropView::OnChangeName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if( m_bUserChange )
	{
		UpdateData();
		UpdateSetName();
	}
}

void CObjPropView::UpdateSetName()
{
	CString s = m_pDoc->GetSelectedObjectList()->GetItem(LL_HEAD)->GetName();
	if( s != m_sName )
	{
		GetDlgItem( IDC_SET_NAME )->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem( IDC_SET_NAME )->EnableWindow(FALSE);
	}
}

void CObjPropView::OnSetName() 
{
	// TODO: Add your control notification handler code here
	CMapObject* obj = m_pDoc->GetSelectedObjectList()->GetItem(LL_HEAD);

	if( m_sName != "" )
	{
		obj->SetName( m_sName );
		m_Tree.ObjectStateChange( obj );
	}
	else
	{
		m_sName = obj->GetName();
		UpdateData(FALSE);
	}

	GetDlgItem( IDC_SET_NAME )->EnableWindow( FALSE );
}

BOOL CObjPropView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
    {
		if( pMsg->wParam == VK_RETURN )
		{
			if( GetFocus() )
			{
				if( GetFocus() == GetDlgItem( IDC_NAME ) )
				{
					if( GetDlgItem(IDC_SET_NAME)->IsWindowEnabled() )
					{
						SendMessage( WM_COMMAND, IDC_SET_NAME );
						return true;
					}
				}
				else
				if( GetFocus() == GetDlgItem( IDC_STRING_EDIT ) )
				{
					if( GetDlgItem(IDC_APPLY)->IsWindowEnabled() )
					{
						SendMessage( WM_COMMAND, IDC_APPLY );
						return true;
					}
				}
				else
				if( GetFocus() )
				{
					CWnd* pWnd = GetFocus();
					pWnd = pWnd->GetOwner();
					if( pWnd == GetDlgItem( IDC_TEXTURE_FILTER_COMBO ) )
					{
						if( GetDlgItem(IDC_TEXTURE_FILTER_SET_BUTTON)->IsWindowEnabled() )
						{
							SendMessage( WM_COMMAND, IDC_TEXTURE_FILTER_SET_BUTTON );
							return true;
						}
					}
				}
			}
		}	
	}

	return CFormView::PreTranslateMessage(pMsg);
}

void CObjPropView::OnSelchangeTextureFilterCombo() 
{
	// TODO: Add your control notification handler code here
	int sel = m_cbTextureFilter.GetCurSel();
	if( sel == -1 )
		return;

	CString s;
	m_cbTextureFilter.GetLBText( sel, s );
	m_pDoc->SetTextureFilter( s );
	Sys_ResetTextureView();

	GetDlgItem( IDC_TEXTURE_FILTER_SET_BUTTON )->EnableWindow( FALSE );

	Sys_RedrawWindows( VIEW_TYPE_TEXTURE );
}

void CObjPropView::OnTextureFilterSetButton() 
{
	// TODO: Add your control notification handler code here
	CString s;

	m_cbTextureFilter.GetWindowText( s );
	m_pDoc->SetTextureFilter( s );
	Sys_ResetTextureView();
	LoadTextureFilters();

	Sys_RedrawWindows( VIEW_TYPE_TEXTURE );
}

void CObjPropView::OnEditchangeTextureFilterCombo() 
{
	// TODO: Add your control notification handler code here
	UpdateTextureFilterApply();
}

void CObjPropView::OnChangeNudgeEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if( m_pDoc )
	{
		CString s;
		m_Nudge.GetWindowText( s );

		if( s != "" )
			m_pDoc->SetNudgeFactor( (float)atof( s ) );
	}
}

void CObjPropView::OnClickChoiceList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NMLISTVIEW* pNMItem = (NMLISTVIEW*)pNMHDR;
	LVHITTESTINFO hitTest;
	int nPos;

	hitTest.pt = pNMItem->ptAction;
	m_lcSubList.SubItemHitTest(&hitTest);
	nPos = hitTest.iItem;

	if(nPos == -1)
		return;

	CObjProp* prop = (CObjProp*)m_lcSubList.GetItemData( nPos );

	if( m_pSelProp->GetSubType() )
	{
		m_pSelProp->SetValue( prop );
		
		if( m_pSelProp->GetType() == CObjProp::color )
			m_colorstatic.SetColor( m_pSelProp->GetVector() );

		m_bUserChange = false;
		m_String.SetWindowText( prop->GetString() );

		*pResult = 1;
	}
	else
	{
		if( hitTest.flags&LVHT_ONITEMSTATEICON )
		{
			if( m_pSelProp->GetInt()&prop->GetInt() )
			{
				m_pSelProp->SetInt( m_pSelProp->GetInt()&~prop->GetInt() );
				m_lcSubList.SetCheck( nPos, false );
			}
			else
			{
				m_pSelProp->SetInt( m_pSelProp->GetInt()|prop->GetInt() );
				m_lcSubList.SetCheck( nPos, true );
			}

			m_bUserChange = false;
			m_String.SetWindowText( m_pSelProp->GetString() );
		}

		*pResult = 0;
	}

	m_pSelProp->bSupress = false;
	UpdateApply();
}

void CObjPropView::ColorDialogCallback( COLORREF clr, void* data )
{
	CObjPropView* propview = (CObjPropView*)data;

	if( propview->m_pSelProp )
	{
		int r, g, b;
		vec3 rgb;

		r = GetRValue(clr);
		g = GetGValue(clr);
		b = GetBValue(clr);
		
		rgb.x = ((float)r)/255.0f;
		rgb.y = ((float)g)/255.0f;
		rgb.z = ((float)b)/255.0f;

		propview->m_pSelProp->SetVector( rgb );
		propview->TempApplySingleProp( propview->m_pSelProp, propview->m_pDoc );
		if( propview->m_pDoc->GetViewLightingFlag() )
			Sys_RedrawWindows( VIEW_TYPE_3D );
	}
}

void CObjPropView::OnColorStatic() 
{
	// TODO: Add your control notification handler code here
	if( !m_pSelProp )
		return;

	int r, g, b;
	vec3 rgb = m_pSelProp->GetVector();

	r = (int)(rgb.x*255.0f);
	g = (int)(rgb.y*255.0f);
	b = (int)(rgb.z*255.0f);

	CRealtimeColorDialog dlgTemp;
	dlgTemp.m_cc.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR | CC_SOLIDCOLOR | CC_ENABLEHOOK;
	dlgTemp.m_cc.rgbResult = RGB( r, g, b );
	dlgTemp.SetColorChangeCallback( ColorDialogCallback, this );

	if( dlgTemp.DoModal() != IDOK )
	{
		m_pSelProp->SetVector( rgb );
		TempApplySingleProp( m_pSelProp, m_pDoc );
		if( m_pDoc->GetViewLightingFlag() )
			Sys_RedrawWindows( VIEW_TYPE_3D );

		return;
	}

	COLORREF clr = dlgTemp.GetColor();

	r = GetRValue(clr);
	g = GetGValue(clr);
	b = GetBValue(clr);
	
	rgb.x = ((float)r)/255.0f;
	rgb.y = ((float)g)/255.0f;
	rgb.z = ((float)b)/255.0f;

	m_pSelProp->SetVector( rgb );
	m_pSelProp->bSupress = false;
	m_colorstatic.SetColor( rgb );
	m_bUserChange = false;
	m_String.SetWindowText( m_pSelProp->GetString() );
	OnApply();
	SetFocus();
}

void CObjPropView::OnEditScript() 
{
	if( !m_pSelProp )
		return;

	// TODO: Add your control notification handler code here
	CScriptDialog dlg;

	dlg.m_sScript = m_pSelProp->GetString();
	if( dlg.DoModal() != IDOK )
		return;

	m_pSelProp->SetString( dlg.m_sScript );
	m_pSelProp->bSupress = false;
	m_bUserChange = false;
	m_String.SetWindowText( dlg.m_sScript );
	OnApply();

	SetFocus();
}

void CObjPropView::UpdateExpandPropsButton()
{
	if( Sys_GetMainFrame()->GetObjectPropertiesDialog()->IsWindowVisible() )
		GetDlgItem(IDC_SHOW_PROPERTIES_DIALOG)->SetWindowText(">");
	else
		GetDlgItem(IDC_SHOW_PROPERTIES_DIALOG)->SetWindowText("<");
}

void CObjPropView::OnShowPropertiesDialog() 
{
	// TODO: Add your control notification handler code here
	if( Sys_GetMainFrame()->GetObjectPropertiesDialog()->IsWindowVisible() )
		Sys_GetMainFrame()->GetObjectPropertiesDialog()->ShowWindow( SW_HIDE );
	else
		Sys_GetMainFrame()->GetObjectPropertiesDialog()->ShowWindow( SW_SHOW );

	UpdateExpandPropsButton();
}

void CObjPropView::OnCloseupProperties() 
{
	// TODO: Add your control notification handler code here
	Invalidate();
}
