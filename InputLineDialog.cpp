///////////////////////////////////////////////////////////////////////////////
// InputLineDialog.cpp
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
#include "InputLineDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputLineDialog dialog

CInputLineDialog *CInputLineDialog::New()
{
	return new CInputLineDialog();
}

void CInputLineDialog::Delete(CInputLineDialog *dlg)
{
	delete dlg;
}

CInputLineDialog::CInputLineDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInputLineDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputLineDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nFilter = 0;
	m_bAllowEmpty = true;
}


void CInputLineDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputLineDialog)
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputLineDialog, CDialog)
	//{{AFX_MSG_MAP(CInputLineDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputLineDialog message handlers

void CInputLineDialog::OnOK() 
{
	// TODO: Add extra validation here
	m_Edit.GetWindowText( m_sVal );

	CDialog::OnOK();
}

void CInputLineDialog::SetTitle( const char* title )
{
	m_sTitle = title;
}

void CInputLineDialog::SetValue( CString val )
{
	m_sVal = val;
}

const char *CInputLineDialog::GetValue()
{
	return m_sVal;
}

void CInputLineDialog::SetFilter( int filter )
{
	m_nFilter = filter;
}

void CInputLineDialog::AllowEmpty( bool allow )
{
	m_bAllowEmpty = allow;
}

BOOL CInputLineDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_Edit.SetWindowText( m_sVal );
	m_Edit.SetSel( 0, -1, true );
	m_Edit.SetFilter( m_nFilter );

	SetWindowText( m_sTitle );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputLineDialog::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString s;
	m_Edit.GetWindowText( s );

	GetDlgItem(IDOK)->EnableWindow( m_bAllowEmpty || ( s != "" ) );
}
