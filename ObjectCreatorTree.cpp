///////////////////////////////////////////////////////////////////////////////
// ObjectCreatorTree.cpp
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
#include "ObjectCreatorTree.h"
#include "System.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectCreatorTree

CObjectCreatorTree::CObjectCreatorTree()
{
}

CObjectCreatorTree::~CObjectCreatorTree()
{
}

HTREEITEM CObjectCreatorTree::FindItem( CString s, HTREEITEM root )
{
	HTREEITEM item = GetChildItem( root );

	while ( item )
	{
		CObjectCreator* c = (CObjectCreator*)GetItemData( item );
		if( c )
		{
			if( c->Name() == s )
				return item;
		}

		HTREEITEM find = FindItem( s, item );
		if( find )
			return find;

		item = GetNextSiblingItem( item );
	}

	return 0;
}

HTREEITEM CObjectCreatorTree::FindRootItem( CString s, HTREEITEM root )
{
	HTREEITEM item = GetChildItem( root );

	while( item )
	{
		if( GetItemData( item ) == 0 )
		{
			CString t = GetItemText(item);
			if( t == s )
				return item;
		}

		HTREEITEM find = FindRootItem( s, item );
		if( find )
			return find;

		item = GetNextSiblingItem( item );
	}

	return 0;
}

void CObjectCreatorTree::SortTree( HTREEITEM root )
{
	SortChildren( root );

	HTREEITEM item = GetChildItem( root );

	while( item )
	{
		SortTree( item );
		item = GetNextSiblingItem( item );
	}
}

void CObjectCreatorTree::LoadTree(CTreadDoc *doc)
{
	DeleteItem( TVI_ROOT );

	m_pRoot = InsertItem( "Current Creation Type" );

	if (!doc) { return; }

	CLinkedList<CObjectCreator>* list = doc->ObjectCreatorList();
	CObjectCreator* item;

	CString str;
	CString title;
	HTREEITEM treeitem;
	int ofs;

	for( item = list->ResetPos(); item; item = list->GetNextItem() )
	{
		str = item->Name();
		treeitem = m_pRoot;

		for(;;)
		{
			title = str;
			ofs = str.Find('\n');
			if( ofs == -1 )
				break;
			title = str.Left(ofs);
			str.Delete(0, ofs+1);

			HTREEITEM sub = FindRootItem( title, treeitem );
			if(!sub)
			{
				sub = InsertItem( title, treeitem );
			}

			treeitem = sub;
		}

		treeitem = InsertItem( title, treeitem );
		SetItemData( treeitem, (DWORD)item );
	}

	SortTree( m_pRoot );

	//
	// select?
	//
	{
		HTREEITEM item;

		CObjectCreator* c = doc->CurrentObjectCreator();
		if( c )
		{
			item = FindItem( c->Name(), m_pRoot );
			if( item )
				SelectItem( item );
		}
	}
}

BEGIN_MESSAGE_MAP(CObjectCreatorTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CObjectCreatorTree)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectCreatorTree message handlers

void CObjectCreatorTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	TVHITTESTINFO hitTest;
	hitTest.pt = point;
	HitTest( &hitTest );

	if( hitTest.hItem == TVI_ROOT || hitTest.hItem == m_pRoot || hitTest.hItem == 0 )
		return;
	
	if( (hitTest.flags&TVHT_ONITEMBUTTON) )
	{
		CTreeCtrl::OnLButtonDown(nFlags, point);
		return;
	}

	CObjectCreator* c = (CObjectCreator*)GetItemData( hitTest.hItem );
	if( !c )
		return;

	Sys_GetActiveDocument()->SetCurrentObjectCreator(c);

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CObjectCreatorTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	// TODO: Add your message handler code here and/or call default
	TVHITTESTINFO hitTest;
	hitTest.pt = point;
	HitTest( &hitTest );

	if( hitTest.hItem == TVI_ROOT || hitTest.hItem == m_pRoot || hitTest.hItem == 0 )
		return;
	
	if( (hitTest.flags&TVHT_ONITEMBUTTON) )
	{
		CTreeCtrl::OnLButtonDblClk(nFlags, point);
		return;
	}

	//CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void CObjectCreatorTree::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	//
	// select?
	//
	if( pNMTreeView->action == TVE_EXPAND )
	{
		HTREEITEM item;

		CObjectCreator* c = Sys_GetActiveDocument()->CurrentObjectCreator();
		if( c )
		{
			item = FindItem( c->Name(), m_pRoot );
			if( item )
				SelectItem( item );
		}
	}

	*pResult = 0;
}
