///////////////////////////////////////////////////////////////////////////////
// CheckMapDialog.cpp
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
#include "CheckMapDialog.h"
#include "System.h"
#include "TreadDoc.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "MapView.h"
#include "ObjPropView.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckMapDialog dialog


CCheckMapDialog::CCheckMapDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckMapDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckMapDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_doc = 0;
}


void CCheckMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckMapDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckMapDialog, CDialog)
	//{{AFX_MSG_MAP(CCheckMapDialog)
	ON_LBN_DBLCLK(IDC_ERROR_LIST, OnDblclkErrorList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckMapDialog message handlers

void CCheckMapDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	// not compiling?
	CDialog::OnCancel();
}

void CCheckMapDialog::Clear()
{
	m_list.ResetContent();
	m_doc = 0;
}

void CCheckMapDialog::CheckMap( CTreadDoc* doc, bool only_selected )
{
	int i;
	CMapObject* obj, *n;
	CString msg;

	m_doc = doc;

	m_list.ResetContent();
	
	AfxGetMainWnd()->EnableWindow(FALSE);

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

	int err_count = 0;
	int obj_count = 0;

	i = m_list.AddString( "Checking Map..." );
	m_list.SetItemData( i, -1 );

	if( only_selected == false )
	{
		doc->ClearSelection();

		for( obj = doc->GetObjectList()->ResetPos(); obj;  )
		{
			doc->GetObjectList()->SetPosition( obj );
			n = doc->GetObjectList()->GetNextItem();

			if( obj->CheckObject( msg, doc ) )
			{
				i = m_list.AddString( msg );
				m_list.SetItemData( i, (DWORD)obj->GetUID() );
				obj->Select( doc );
				
				err_count++;
			}

			obj = n;
			obj_count++;
		}

		doc->UpdateSelectionInterface();
		doc->Prop_UpdateSelection();
		Sys_RedrawWindows();
	}
	else
	{
		for( obj = doc->GetSelectedObjectList()->ResetPos(); obj; obj = doc->GetSelectedObjectList()->GetNextItem() )
		{
			if( obj->CheckObject( msg, doc ) )
			{
				i = m_list.AddString( msg );
				m_list.SetItemData( i, (DWORD)obj->GetUID() );
				err_count++;
			}
			doc->GetSelectedObjectList()->SetPosition( obj );

			obj_count++;
		}
	}

	msg.Format( "Checked %i object(s), found %i error(s)", obj_count, err_count );
	i = m_list.AddString( msg );
	m_list.SetItemData( i, -1 );

	if( err_count > 0 )
		PlaySound( "SystemExclamation", 0, SND_ALIAS|SND_ASYNC );
	else
		PlaySound( "SystemAsterisk" , 0, SND_ALIAS|SND_ASYNC);

	AfxGetMainWnd()->EnableWindow(TRUE);
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
}

BOOL CCheckMapDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_list.SubclassDlgItem( IDC_ERROR_LIST, this );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCheckMapDialog::OnDblclkErrorList() 
{
	// TODO: Add your control notification handler code here
	int i = m_list.GetCurSel();
	if( i < 0 )
		return;

	int uid = m_list.GetItemData(i);
	if( uid == -1 )
		return;

	m_doc->ClearSelection();

	CMapObject* obj = m_doc->ObjectForUID( uid );
	if( obj )
	{
		obj->Select( m_doc );
		m_doc->UpdateSelectionInterface();
		m_doc->CenterOnPosition(obj->GetObjectWorldPos());
		Sys_RedrawWindows( VIEW_FLAG_MAP );
	}
}
