///////////////////////////////////////////////////////////////////////////////
// ObjectPropertiesDialog.cpp
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
#include "ObjectPropertiesDialog.h"
#include "InputLineDialog.h"
#include "System.h"
#include "ObjPropView.h"
#include "KeyValueDialog.h"
#include "TreadDoc.h"
#include "RealtimeColorDialog.h"
#include "ScriptDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertiesDialog dialog


CObjectPropertiesDialog::CObjectPropertiesDialog(CTreadDoc *doc, CWnd* pParent /*=NULL*/)
	: CDialog(CObjectPropertiesDialog::IDD, pParent), m_doc(doc)
{
	m_bInit = false;
	//{{AFX_DATA_INIT(CObjectPropertiesDialog)
	//}}AFX_DATA_INIT
}


void CObjectPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectPropertiesDialog)
	DDX_Control(pDX, IDC_LIST1, m_lcItems);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectPropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(CObjectPropertiesDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertiesDialog message handlers

void CObjectPropertiesDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CObjectPropertiesDialog::EnableControls( bool enable )
{
	m_lcItems.EnableWindow( enable );
	//m_ApplyButton.EnableWindow( enable );
}

BOOL CObjectPropertiesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bInit = true;
	
	m_lcItems.InsertColumn( 0, "Property", LVCFMT_LEFT, 300, -1 );
	m_lcItems.InsertColumn( 1, "Value", LVCFMT_LEFT, 160, -1 );

	if (m_doc)
	{
		SetWindowTextA("Worldspawn");
		Sys_GetPropView()->LoadWorldspawn();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CListCtrl* CObjectPropertiesDialog::GetPropList()
{
	return &m_lcItems;
}

void CObjectPropertiesDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( m_bInit )
	{
		CRect rect( 0, 0, cx, cy );
		m_lcItems.MoveWindow(rect);

		/*CRect rect2( cx-100, cy-30, cx-20, cy-5 );
		m_ApplyButton.MoveWindow(rect2);*/
	}
}

void CObjectPropertiesDialog::UpdateProp( CObjProp* prop )
{
	if( prop )
	{
		m_lcItems.SetItemText( prop->GetListItem(), 1, prop->GetString() );
	}
}

void CObjectPropertiesDialog::ColorDialogCallback( COLORREF clr, void* data )
{
	CObjProp* p = (CObjProp*)data;

	if( p )
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
		temp_prop.SetName( p->GetName() );
		temp_prop.SetDisplayName( p->GetDisplayName() );

		Sys_GetPropView()->TempApplySingleProp( &temp_prop, Sys_GetPropView()->GetDoc() );
		if( Sys_GetPropView()->GetDoc()->GetViewLightingFlag() )
			Sys_RedrawWindows( VIEW_TYPE_3D );
	}
}

void CObjectPropertiesDialog::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NMLISTVIEW* pNMItem = (NMLISTVIEW*)pNMHDR;
	LVHITTESTINFO hitTest;
	int nPos;

	*pResult = 0;

	hitTest.pt = pNMItem->ptAction;
	m_lcItems.SubItemHitTest(&hitTest);
	nPos = hitTest.iItem;

	if(nPos == -1)
		return;
	
	m_lcItems.SetHotItem(hitTest.iItem);

	CObjProp* p = (CObjProp*)m_lcItems.GetItemData( nPos );
	if( !p )
		return;
	
	int filter = FILTER_NONE;

	switch( p->GetType() )
	{
	case 1: // integer
	case 5: // facing

		filter = FILTER_INT;

	break;

	case 2: // float

		filter = FILTER_FLOAT;

	break;

	case 6: // color (non-negative) floats

		filter = FILTER_MULTI_FLOAT|FILTER_DASH; 

	break;

	case 4: // vector

		filter = FILTER_MULTI_FLOAT;

	break;

	case 3:
	case 7:

		filter = FILTER_TEXT;

	break;
	}

	if( p->GetChoices()->IsEmpty() == false )
	{
		CKeyValueDialog dlg;

		dlg.SetTitle("Enter New Value");
		dlg.SetFilter( filter );
		dlg.SetProp( p );
		dlg.AllowEmpty();

		if( dlg.DoModal() != IDOK )
			return;

		p->SetString( dlg.GetValue() );
		m_lcItems.SetItemText( nPos, 1, dlg.GetValue() );
		Sys_GetPropView()->ExternalPropChange(p);
		Sys_GetPropView()->ExternalApply();
	}
	else
	{

		if( p->GetType() == CObjProp::color )
		{
			int r, g, b;
			vec3 rgb = p->GetVector();

			r = (int)(rgb.x*255.0f);
			g = (int)(rgb.y*255.0f);
			b = (int)(rgb.z*255.0f);

			CRealtimeColorDialog dlgTemp;
			dlgTemp.m_cc.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR | CC_SOLIDCOLOR | CC_ENABLEHOOK;
			dlgTemp.m_cc.rgbResult = RGB( r, g, b );
			dlgTemp.SetColorChangeCallback( ColorDialogCallback, p );

			if( dlgTemp.DoModal() != IDOK )
			{
				p->SetVector( rgb );
				Sys_GetPropView()->TempApplySingleProp( p, Sys_GetPropView()->GetDoc() );
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

			p->SetVector( rgb );
			m_lcItems.SetItemText( nPos, 1, p->GetString() );
			Sys_GetPropView()->ExternalPropChange(p);
			Sys_GetPropView()->ExternalApply();

			return;
		}
		if( p->GetType() == CObjProp::script )
		{
			CScriptDialog dlg;

			dlg.m_sScript = p->GetString();
			if( dlg.DoModal() != IDOK )
				return;

			p->SetString( dlg.m_sScript );
			m_lcItems.SetItemText( nPos, 1, p->GetString() );
			Sys_GetPropView()->ExternalPropChange(p);
			Sys_GetPropView()->ExternalApply();

			return;
		}
		
		CInputLineDialog dlg;

		dlg.SetTitle("Enter New Value");
		dlg.AllowEmpty();
		dlg.SetValue( p->GetString() );
		dlg.SetFilter( filter );
		if( dlg.DoModal() == IDOK )
		{
			p->SetString( dlg.GetValue() );
			m_lcItems.SetItemText( nPos, 1, dlg.GetValue() );
			Sys_GetPropView()->ExternalPropChange( p );
			Sys_GetPropView()->ExternalApply();
		}
	}
}

void CObjectPropertiesDialog::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NMLISTVIEW* pNMItem = (NMLISTVIEW*)pNMHDR;
	LVHITTESTINFO hitTest;
	int nPos;

	*pResult = 0;

	hitTest.pt = pNMItem->ptAction;
	m_lcItems.SubItemHitTest(&hitTest);
	nPos = hitTest.iItem;

	if(nPos == -1)
		return;
	
	m_lcItems.SetHotItem(hitTest.iItem);

	CObjProp* p = (CObjProp*)m_lcItems.GetItemData( nPos );
	if( !p )
		return;
	
	int filter = FILTER_NONE;

	switch( p->GetType() )
	{
	case 1: // integer
	case 5: // facing

		filter = FILTER_INT;

	break;

	case 2: // float

		filter = FILTER_FLOAT;

	break;

	case 6: // color (non-negative) floats

		filter = FILTER_MULTI_FLOAT|FILTER_DASH; 

	break;

	case 4: // vector

		filter = FILTER_MULTI_FLOAT;

	break;

	case 3:
	case 7:

		filter = FILTER_TEXT;

	break;
	}

	CInputLineDialog dlg;

	dlg.SetTitle("Enter New Value");
	dlg.AllowEmpty();
	dlg.SetValue( p->GetString() );
	dlg.SetFilter( filter );
	if( dlg.DoModal() == IDOK )
	{
		p->SetString( dlg.GetValue() );
		m_lcItems.SetItemText( nPos, 1, dlg.GetValue() );
		Sys_GetPropView()->ExternalPropChange( p );
		Sys_GetPropView()->ExternalApply();
	}
}

BOOL CObjectPropertiesDialog::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
    {
		//
		// is this a repeat?
		//
		if( pMsg->lParam&(1<<30) )
		{
			return true;
		}

		if( pMsg->wParam == VK_RETURN )
		{
			if( GetFocus() )
			{
				if( GetDlgItem(IDC_APPLY_BUTTON)->IsWindowEnabled() )
				{
					SendMessage( WM_COMMAND, IDC_APPLY_BUTTON );
					return true;
				}
			}
		}
		if( pMsg->wParam == 0xBE ) // '.'
		{
			ShowWindow( SW_HIDE );
			Sys_GetPropView()->UpdateExpandPropsButton();
			return true;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
