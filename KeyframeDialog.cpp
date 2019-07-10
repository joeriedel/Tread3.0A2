///////////////////////////////////////////////////////////////////////////////
// KeyframeDialog.cpp
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
#include "KeyframeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyframeDialog dialog


CKeyframeDialog::CKeyframeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyframeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyframeDialog)
	//}}AFX_DATA_INIT

	m_nFilter = 0;
	m_bAllowEmpty = true;
}


void CKeyframeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyframeDialog)
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_EDIT2, m_Edit2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeyframeDialog, CDialog)
	//{{AFX_MSG_MAP(CKeyframeDialog)
	ON_EN_UPDATE(IDC_EDIT2, OnUpdateEdit2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyframeDialog message handlers

void CKeyframeDialog::OnOK() 
{
	// TODO: Add extra validation here
	m_Edit2.GetWindowText( m_sVal );
	m_Edit.GetWindowText( m_sTime );

	CDialog::OnOK();
}

void CKeyframeDialog::SetTitle( const char* title )
{
	m_sTitle = title;
}

void CKeyframeDialog::SetInputLineTitle( const char* title )
{
	m_sInputTitle = title;
}

void CKeyframeDialog::SetValue( CString val )
{
	m_sVal = val;
}

void CKeyframeDialog::SetTime( CString val )
{
	m_sTime = val;
}

CString CKeyframeDialog::GetTime()
{
	return m_sTime;
}

CString CKeyframeDialog::GetValue()
{
	return m_sVal;
}

void CKeyframeDialog::SetFilter( int filter )
{
	m_nFilter = filter;
}

void CKeyframeDialog::AllowEmpty( bool allow )
{
	m_bAllowEmpty = allow;
}

BOOL CKeyframeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_Edit2.SetWindowText( m_sVal );
	m_Edit2.SetSel( 0, -1, true );
	m_Edit2.SetFilter( m_nFilter );

	m_Edit.SetWindowText( m_sTime );
	m_Edit.SetFilter( FILTER_FLOAT );

	GetDlgItem(IDC_INPUT_STATIC)->SetWindowText( m_sInputTitle );

	SetWindowText( m_sTitle );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CKeyframeDialog::OnUpdateEdit2() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CString s;
	m_Edit2.GetWindowText( s );

	GetDlgItem(IDOK)->EnableWindow( m_bAllowEmpty || ( s != "" ) );
}
