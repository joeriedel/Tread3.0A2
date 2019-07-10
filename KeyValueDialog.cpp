///////////////////////////////////////////////////////////////////////////////
// KeyValueDialog.cpp
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
#include "KeyValueDialog.h"
#include "RealtimeColorDialog.h"
#include "ObjPropView.h"
#include "TreadDoc.h"
#include "ScriptDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyValueDialog dialog


CKeyValueDialog::CKeyValueDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyValueDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyValueDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nFilter = 0;
	m_bAllowEmpty = true;
	m_pProp = 0;
}


void CKeyValueDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyValueDialog)
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_LIST1, m_lcList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeyValueDialog, CDialog)
	//{{AFX_MSG_MAP(CKeyValueDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_BN_CLICKED(IDC_COLOR_STATIC, OnColorStatic)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyValueDialog message handlers
void CKeyValueDialog::SetTitle( const char* title )
{
	m_sTitle = title;
}

void CKeyValueDialog::SetProp( CObjProp* prop )
{
	m_pProp = prop;
}

CString CKeyValueDialog::GetValue()
{
	return m_sVal;
}

void CKeyValueDialog::SetFilter( int filter )
{
	m_nFilter = filter;
}

void CKeyValueDialog::AllowEmpty( bool allow )
{
	m_bAllowEmpty = allow;
}


void CKeyValueDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CKeyValueDialog::UpdateSubItemStates()
{
	CObjProp* sub;
	
	if( m_pProp->GetChoices()->IsEmpty() )
		return;

	if( m_pProp->GetSubType() )
	{
		for( sub = m_pProp->GetChoices()->ResetPos(); sub; sub = m_pProp->GetChoices()->GetNextItem() )
		{
			if( !strcmp( sub->GetString(), m_sVal ) )
			{
				LVITEM item;

				item.mask = LVIF_STATE;
				item.iItem = sub->GetListItem();
				item.iSubItem = 0;
				item.stateMask = LVIS_SELECTED;
				item.state = LVIS_SELECTED;

				m_lcList.SetItemState( sub->GetListItem(), &item );
			}
			else
			{
				LVITEM item;

				item.mask = LVIF_STATE;
				item.iItem = sub->GetListItem();
				item.iSubItem = 0;
				item.stateMask = LVIS_SELECTED;
				item.state = 0;

				m_lcList.SetItemState( sub->GetListItem(), &item );
			}
		}
	}
	else
	{
		for( sub = m_pProp->GetChoices()->ResetPos(); sub; sub = m_pProp->GetChoices()->GetNextItem() )
		{
			LVITEM item;

			item.mask = LVIF_STATE;
			item.iItem = sub->GetListItem();
			item.iSubItem = 0;
			item.stateMask = LVIS_SELECTED;
			item.state = 0;
			
			m_lcList.SetItemState( sub->GetListItem(), &item );

			bool check = (m_iVal&sub->GetInt())?true:false;
			m_lcList.SetCheck( sub->GetListItem(), check );
		}
	}
}

BOOL CKeyValueDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bUserChange = false;
	
	if(m_pProp)
	{
		m_sVal = m_pProp->GetString();
		m_iVal = atoi( m_sVal );
	}
	
	m_colorstatic.SubclassDlgItem( IDC_COLOR_STATIC, this );

	if( m_pProp->GetType() != CObjProp::color )
	{
		m_colorstatic.ShowWindow( SW_HIDE );
	}
	
	if( m_pProp->GetType() != CObjProp::script )
	{
		GetDlgItem( IDC_BUTTON1 )->ShowWindow( SW_HIDE );
	}

	m_Edit.SetWindowText( m_sVal );
	m_Edit.SetSel( 0, -1, true );
	m_Edit.SetFilter( m_nFilter );
	
	m_bUserChange = true;

	SetWindowText( m_sTitle );
	
	m_SubImageList.Create( IDB_CHECK_BUTTONS, 16, 0, RGB(255, 0, 255) );
	m_lcList.InsertColumn( 0, "", LVCFMT_LEFT, 500, -1 );
	m_lcList.SetImageList( &m_SubImageList, LVSIL_STATE );
	
	if( m_pProp )
	{
		//
		// load the choice window.
		//
		if( m_pProp->GetSubType() == false &&
			m_pProp->GetType() != CObjProp::integer )
		{
			//
			// it is illegal to have a set of bitwise orable options on
			// anything but an integer type.
			//
			goto nosublist;
		}

		m_lcList.DeleteAllItems();

		CObjProp* sub;
		int c, n;
		bool check = false;

		for( c = 0, sub = m_pProp->GetChoices()->ResetPos(); sub; sub = m_pProp->GetChoices()->GetNextItem() )
		{
			n = m_lcList.InsertItem( c++, sub->GetDisplayName(), 0 );
			m_lcList.SetItemData( n, (DWORD)sub );
			sub->SetListItem( n );
		}

		UpdateSubItemStates();
	}

nosublist:

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CKeyValueDialog::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NMLISTVIEW* pNMItem = (NMLISTVIEW*)pNMHDR;
	LVHITTESTINFO hitTest;
	int nPos;

	hitTest.pt = pNMItem->ptAction;
	m_lcList.SubItemHitTest(&hitTest);
	nPos = hitTest.iItem;

	if(nPos == -1)
		return;

	CObjProp* prop = (CObjProp*)m_lcList.GetItemData( nPos );

	if( m_pProp->GetSubType() )
	{
		m_sVal = prop->GetString();
		
		m_bUserChange = false;
		m_Edit.SetWindowText( m_sVal );

		*pResult = 1;
	}
	else
	{
		if( hitTest.flags&LVHT_ONITEMSTATEICON )
		{
			if( m_iVal&prop->GetInt() )
			{
				m_iVal = m_iVal&~prop->GetInt();
				m_lcList.SetCheck( nPos, false );
			}
			else
			{
				m_iVal = m_iVal|prop->GetInt();
				m_lcList.SetCheck( nPos, true );
			}

			m_sVal.Format("%d", m_iVal);

			m_bUserChange = false;
			m_Edit.SetWindowText( m_sVal );
		}

		*pResult = 0;
	}
}

void CKeyValueDialog::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if( m_bUserChange )
	{
		m_Edit.GetWindowText( m_sVal );
		m_iVal = atoi( m_sVal );
		UpdateSubItemStates();

		if( m_pProp && m_pProp->GetType() == CObjProp::color )
		{
			vec3 rgb;
			sscanf( m_sVal, "%f %f %f", &rgb.x, &rgb.y, &rgb.z );
			m_colorstatic.SetColor( rgb );
		}
	}

	m_bUserChange = true;

	GetDlgItem(IDOK)->EnableWindow( m_bAllowEmpty || ( m_sVal != "" ) );
}

void CKeyValueDialog::ColorDialogCallback( COLORREF clr, void* data )
{
	CKeyValueDialog* dialog = (CKeyValueDialog*)data;

	if( dialog->m_pProp )
	{
		int r, g, b;
		vec3 rgb;

		r = GetRValue(clr);
		g = GetGValue(clr);
		b = GetBValue(clr);
		
		rgb.x = ((float)r)/255.0f;
		rgb.y = ((float)g)/255.0f;
		rgb.z = ((float)b)/255.0f;

		CObjProp temp_prop;
		temp_prop.SetVector( rgb );
		temp_prop.SetName( dialog->m_pProp->GetName() );
		temp_prop.SetDisplayName( dialog->m_pProp->GetDisplayName() );

		Sys_GetPropView()->TempApplySingleProp( &temp_prop, Sys_GetPropView()->GetDoc() );
		if( Sys_GetPropView()->GetDoc()->GetViewLightingFlag() )
			Sys_RedrawWindows( VIEW_TYPE_3D );
	}
}

void CKeyValueDialog::OnColorStatic() 
{
	// TODO: Add your control notification handler code here
	if( !m_pProp )
		return;

	int r, g, b;
	vec3 rgb;

	sscanf( m_sVal, "%f %f %f", &rgb.x, &rgb.y, &rgb.z );

	r = (int)(rgb.x*255.0f);
	g = (int)(rgb.y*255.0f);
	b = (int)(rgb.z*255.0f);

	CRealtimeColorDialog dlgTemp;
	dlgTemp.m_cc.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR | CC_SOLIDCOLOR | CC_ENABLEHOOK;
	dlgTemp.m_cc.rgbResult = RGB( r, g, b );
	dlgTemp.SetColorChangeCallback( ColorDialogCallback, this );

	if( dlgTemp.DoModal() != IDOK )
	{
		CObjProp temp_prop;
		
		temp_prop.SetVector( rgb );
		temp_prop.SetName( m_pProp->GetName() );
		temp_prop.SetDisplayName( m_pProp->GetDisplayName() );
		Sys_GetPropView()->TempApplySingleProp( &temp_prop, Sys_GetPropView()->GetDoc() );
		if( Sys_GetPropView()->GetDoc()->GetViewLightingFlag() )
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

	m_sVal.Format("%f %f %f", rgb.x, rgb.y, rgb.z );
	m_iVal = atoi( m_sVal );
	m_colorstatic.SetColor( rgb );
	m_bUserChange = false;
	m_Edit.SetWindowText( m_sVal );
	OnOK(); // cancel dialog.
}

void CKeyValueDialog::OnButton1() 
{
	// TODO: Add your control notification handler code here
	if( !m_pProp )
		return;

	// TODO: Add your control notification handler code here
	CScriptDialog dlg;

	dlg.m_sScript = m_sVal;
	if( dlg.DoModal() != IDOK )
		return;

	m_sVal = dlg.m_sScript;
	m_iVal = atoi( m_sVal );

	m_bUserChange = false;
	m_Edit.SetWindowText( dlg.m_sScript );
	OnOK();
}
