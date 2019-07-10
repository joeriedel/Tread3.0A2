///////////////////////////////////////////////////////////////////////////////
// ObjGroupTree.cpp
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
#include "ObjGroupTree.h"
#include "System.h"
#include "TreadDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EYE_ICON	3
#define BLANK_ICON	2

/////////////////////////////////////////////////////////////////////////////
// CObjGroupTree

CObjGroupTree::CObjGroupTree()
{
	m_pDoc = 0;
}

CObjGroupTree::~CObjGroupTree()
{
}

void CObjGroupTree::ShowGroup( CObjectGroup* obj )
{
	if( obj->GetTreeItem() )
	{
		EnsureVisible( obj->GetTreeItem() );
		Expand( obj->GetTreeItem(), TVE_COLLAPSE );
	}
}

void CObjGroupTree::SetDoc( CTreadDoc* pDoc )
{
	m_pDoc = pDoc;
}

void CObjGroupTree::SelectItemUID( int uid )
{
	HTREEITEM item = FindItemByUID( uid, TVI_ROOT );
	if( item )
	{
		//Select( item, TVGN_CARET|TVGN_FIRSTVISIBLE );
		SelectItem( item );
		//if( GetChildItem( item ) != 0 )
		//	Expand( item, TVE_EXPAND );
	}
}

int CObjGroupTree::AddObjectProc( CMapObject* obj, void* p, void* p2 )
{
	if( !obj->CanAddToTree() )
		return 0;

	CObjGroupTree* tree = (CObjGroupTree*)p;

	HTREEITEM item;
	int icon;

	item = 0;

	//
	// find the parent?
	//
	if( obj->GetGroupUID() != -1 )
	{
		CObjectGroup* gr = tree->m_pDoc->GroupForUID( obj->GetGroupUID() );
		if( gr )
			item = gr->GetTreeItem();
	}

	if( !item ) item = tree->m_pRoot;
	icon = (obj->IsVisible())?EYE_ICON:BLANK_ICON;
	item = tree->InsertItem( obj->GetName(), icon, icon, item );
	tree->SetItemData( item, obj->GetUID() );

	obj->SetTreeItem( item );

	return 0;
}

int CObjGroupTree::ClearObjectTreeItemProc( CMapObject* obj, void* p, void* p2 )
{
	obj->SetTreeItem( 0 );
	return 0;
}

int CObjGroupTree::ClearGroupTreeItemProc( CObjectGroup* gr, void* p, void* p2 )
{
	gr->SetTreeItem( 0 );
	return 0;
}


void CObjGroupTree::AddObject( CMapObject* obj )
{
	HTREEITEM item;
	int icon;

	item = 0;

	//
	// find the parent?
	//
	if( obj->GetGroupUID() != -1 )
	{
		CObjectGroup* gr = m_pDoc->GroupForUID( obj->GetGroupUID() );
		if( gr )
			item = gr->GetTreeItem();
	}

	if( !item ) item = m_pRoot;
	icon = (obj->IsVisible())?EYE_ICON:BLANK_ICON;
	item = InsertItem( obj->GetName(), icon, icon, item );

	SetItemData( item, obj->GetUID() );

	obj->SetTreeItem( item );

	//Expand( m_pRoot, TVE_EXPAND );
	SelectSetFirstVisible( item );
}

void CObjGroupTree::RemoveObject( CMapObject* obj )
{
	if( obj->GetTreeItem() )
	{
		DeleteItem( obj->GetTreeItem() );
		obj->SetTreeItem( 0 );
	}
}

void CObjGroupTree::AddGroup( CObjectGroup* gr )
{
	HTREEITEM item;
	int icon;

	icon = (gr->IsVisible())?EYE_ICON:BLANK_ICON;
	item = InsertItem( gr->GetName(), icon, icon, m_pRoot );
	SetItemData( item, gr->GetUID() );
	gr->SetTreeItem( item );
	Expand( item, TVE_COLLAPSE );
}

void CObjGroupTree::RemoveGroup( CObjectGroup* gr )
{
	if( gr->GetTreeItem() )
	{
		DeleteItem( gr->GetTreeItem() );
		gr->SetTreeItem( 0 );
	}
}

void CObjGroupTree::ObjectStateChange( CMapObject* obj )
{
	if( !obj->GetTreeItem() )
		return;

	int icon;
	icon = (obj->IsVisible())?EYE_ICON:BLANK_ICON;
	SetItemImage( obj->GetTreeItem(), icon, icon );
	SetItemText( obj->GetTreeItem(), obj->GetName() );
}

void CObjGroupTree::GroupStateChange( CObjectGroup* obj )
{
	if( !obj->GetTreeItem() )
		return;

	int icon;
	icon = (obj->IsVisible())?EYE_ICON:BLANK_ICON;
	SetItemImage( obj->GetTreeItem(), icon, icon );
	SetItemText( obj->GetTreeItem(), obj->GetName() );
}

void CObjGroupTree::BuildTree()
{
	DeleteItem( TVI_ROOT );

	//
	// create the root.
	//
	m_pRoot = InsertItem( "Objects/Groups", 4, 4 );

	//
	// clear all objects.
	//
	m_pDoc->GetObjectGroupList()->WalkList( ClearGroupTreeItemProc );
	m_pDoc->GetObjectList()->WalkList( ClearObjectTreeItemProc );
	m_pDoc->GetSelectedObjectList()->WalkList( ClearObjectTreeItemProc );

	//
	// add all groups.
	//
	int icon;
	HTREEITEM item;

	CObjectGroup* gr;
	for( gr = m_pDoc->GetObjectGroupList()->ResetPos(); gr; gr = m_pDoc->GetObjectGroupList()->GetNextItem() )
	{
		icon = (gr->IsVisible())?EYE_ICON:BLANK_ICON;
		item = InsertItem( gr->GetName(), icon, icon, m_pRoot );
		SetItemData( item, gr->GetUID() );
		gr->SetTreeItem( item );
	}

	m_pDoc->GetObjectList()->WalkList( AddObjectProc, this );
	m_pDoc->GetSelectedObjectList()->WalkList( AddObjectProc, this );

	Expand( m_pRoot, TVE_EXPAND );

	//SortList( TVI_ROOT );
}

HTREEITEM CObjGroupTree::FindItemByUID( int uid, HTREEITEM pRoot )
{
	HTREEITEM item = GetChildItem( pRoot );
	HTREEITEM child;

	while( item != 0 )
	{
		if( item != m_pRoot && GetItemData( item ) == (DWORD)uid )
			return item;

		child = FindItemByUID( uid, item ); // search children.
		if( child )
			return child;

		item = GetNextSiblingItem( item );
	}

	return 0;
}


BEGIN_MESSAGE_MAP(CObjGroupTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CObjGroupTree)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjGroupTree message handlers

void CObjGroupTree::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if( pTVDispInfo->item.hItem == TVI_ROOT ||
		pTVDispInfo->item.hItem == m_pRoot )
	{
		*pResult = 1;
		return;
	}

	*pResult = 0;
}

void CObjGroupTree::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	if( pTVDispInfo->item.pszText == 0 ||
		pTVDispInfo->item.pszText[0] == 0 )
		return;

	CString s = pTVDispInfo->item.pszText;

	HTREEITEM item = pTVDispInfo->item.hItem;
	
	int uid = (int)GetItemData( item );
	CMapObject* obj = m_pDoc->ObjectForUID( uid );
	if( obj )
	{
		obj->SetName( s );
		m_pDoc->Prop_UpdateObjectState( obj );
		m_pDoc->Prop_UpdateSelection();
	}
	else
	{
		CObjectGroup* gr = m_pDoc->GroupForUID( uid );
		if( gr )
		{
			gr->SetName( s );
			m_pDoc->Prop_UpdateGroupState( gr );
		}
	}

	*pResult = 0;
}

void CObjGroupTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//
	// modify visibility.
	//
	TVHITTESTINFO hitTest;
	hitTest.pt = point;
	HitTest( &hitTest );

	if( hitTest.hItem == TVI_ROOT || hitTest.hItem == m_pRoot || hitTest.hItem == 0 )
		return;
	
	if( (hitTest.flags&TVHT_ONITEMICON) )
	{

		//
		// toggle the object.
		//
		CMapObject* obj;
		CObjectGroup* gr;
		int uid, icon;

		uid = GetItemData( hitTest.hItem );
		obj = m_pDoc->ObjectForUID( uid );
		if( obj )
		{
			bool deselect = obj->IsSelected();

			if( deselect )
				m_pDoc->MakeUndoDeselectAction();

			obj->SetVisible( m_pDoc, !obj->IsVisible() );
			if( deselect )
				obj->Deselect( m_pDoc );

			icon = (obj->IsVisible())?EYE_ICON:BLANK_ICON;
		}
		else
		{
			gr = m_pDoc->GroupForUID( uid );
			if( gr )
			{
				gr->SetVisible( m_pDoc, !gr->IsVisible() );
				icon = (gr->IsVisible())?EYE_ICON:BLANK_ICON;
			}
		}

		SetItemImage( hitTest.hItem, icon, icon );
		m_pDoc->UpdateSelectionInterface();
		m_pDoc->Prop_UpdateSelection();
		Sys_RedrawWindows();

		return;
	}

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CObjGroupTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//
	// modify visibility.
	//
	TVHITTESTINFO hitTest;
	hitTest.pt = point;
	HitTest( &hitTest );

	if( hitTest.hItem == TVI_ROOT || hitTest.hItem == m_pRoot || hitTest.hItem == 0 )
		return;
	
	if( (hitTest.flags&TVHT_ONITEMICON) )
	{

		//
		// toggle the object.
		//
		CMapObject* obj;
		CObjectGroup* gr;
		int uid, icon;

		uid = GetItemData( hitTest.hItem );
		obj = m_pDoc->ObjectForUID( uid );
		if( obj )
		{
			bool deselect = obj->IsSelected();

			if( deselect )
				m_pDoc->MakeUndoDeselectAction();

			obj->SetVisible( m_pDoc, !obj->IsVisible() );
			if( deselect )
				obj->Deselect( m_pDoc );

			icon = (obj->IsVisible())?EYE_ICON:BLANK_ICON;
		}
		else
		{
			gr = m_pDoc->GroupForUID( uid );
			if( gr )
			{
				gr->SetVisible( m_pDoc, !gr->IsVisible() );
				icon = (gr->IsVisible())?EYE_ICON:BLANK_ICON;
			}
		}

		SetItemImage( hitTest.hItem, icon, icon );
		m_pDoc->UpdateSelectionInterface();
		m_pDoc->Prop_UpdateSelection();
		Sys_RedrawWindows();

		return;
	}

	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void CObjGroupTree::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CObjGroupTree::DeleteGroup()
{
	HTREEITEM item = GetSelectedItem();
	if( !item )
		return;

	int uid = (int)GetItemData( item );
	CObjectGroup* gr;

	gr = m_pDoc->GroupForUID( uid );
	if( gr )
	{
		//
		// orhpan all the objects.
		//
		gr->OrphanObjects( m_pDoc );
		RemoveGroup( gr );

		m_pDoc->GetObjectGroupList()->RemoveItem( gr );
		delete gr;
	}
}

BOOL CObjGroupTree::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
    {
            // When an item is being edited make sure the edit control
            // receives certain important key strokes
            if( GetEditControl() 
                            && (pMsg->wParam == VK_RETURN 
                                    || pMsg->wParam == VK_DELETE 
                                    || pMsg->wParam == VK_ESCAPE
                                    || GetKeyState( VK_CONTROL) 
                                    )
                    )
            {
                    ::TranslateMessage(pMsg);
                    ::DispatchMessage(pMsg);
                    return TRUE;                            // DO NOT process further
            }
			else
			if( pMsg->wParam == VK_DELETE )
			{
				DeleteGroup();
				return TRUE;
			}
    }

	return CTreeCtrl::PreTranslateMessage(pMsg);
}

void CObjGroupTree::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	TVHITTESTINFO hitTest;
	hitTest.pt = point;
	HitTest( &hitTest );

	if( hitTest.hItem == TVI_ROOT || hitTest.hItem == m_pRoot || hitTest.hItem == 0 )
		return;
	
	//
	// select the object/group.
	//
	CMapObject* obj;
	int uid;
	bool deselect = (nFlags&MS_CONTROL)?false:true;
	bool jump = (nFlags&MS_SHIFT)?true:false;

	uid = GetItemData( hitTest.hItem );
	obj = m_pDoc->ObjectForUID( uid );

	if( obj )
	{
		obj->SetVisible( m_pDoc, true );
		m_pDoc->Prop_UpdateObjectState( obj );
	
		bool wassel = obj->IsSelected();

		if( deselect )
		{
			m_pDoc->MakeUndoDeselectAction();
			m_pDoc->ClearSelection();
		}

		if( wassel && !deselect )
		{
			obj->Deselect( m_pDoc );
		}
		else if( !wassel )
		{
			obj->Select( m_pDoc );
		}
	}
	else
	{
		CObjectGroup* gr = m_pDoc->GroupForUID( uid );
		if( gr )
		{
			bool wassel = gr->IsSelected( m_pDoc );
			
			if( deselect )
			{
				m_pDoc->MakeUndoDeselectAction();
				m_pDoc->ClearSelection();
			}

			gr->SetVisible( m_pDoc, true );
			m_pDoc->Prop_UpdateGroupState( gr );

			if( wassel && !deselect )
			{
				gr->DeselectObjects( m_pDoc );
			}
			else if( !wassel )
			{
				gr->SelectObjects( m_pDoc );
			}
		}
	}

	m_pDoc->UpdateSelectionInterface();
	m_pDoc->Prop_UpdateSelection();
	Sys_RedrawWindows();

	//Select( hitTest.hItem, TVGN_CARET|TVGN_FIRSTVISIBLE );
	SelectItem( hitTest.hItem );

	//CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CObjGroupTree::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnRButtonDown(nFlags, point);

	//CTreeCtrl::OnRButtonDblClk(nFlags, point);
}
