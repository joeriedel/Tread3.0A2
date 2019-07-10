///////////////////////////////////////////////////////////////////////////////
// ScriptKeyframeDialog.cpp
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
#include "ScriptKeyframeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptKeyframeDialog dialog


CScriptKeyframeDialog::CScriptKeyframeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptKeyframeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptKeyframeDialog)
	m_sScript = _T("");
	//}}AFX_DATA_INIT
}


void CScriptKeyframeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptKeyframeDialog)
	DDX_Control(pDX, IDC_EDIT2, m_Time);
	DDX_Text(pDX, IDC_EDIT1, m_sScript);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScriptKeyframeDialog, CDialog)
	//{{AFX_MSG_MAP(CScriptKeyframeDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptKeyframeDialog message handlers
void CScriptKeyframeDialog::SetTime( CString time )
{
	m_sTime = time;
}

CString CScriptKeyframeDialog::GetTime()
{
	return m_sTime;
}

void CScriptKeyframeDialog::OnOK() 
{
	// TODO: Add extra validation here
	m_Time.GetWindowText( m_sTime );

	UpdateData();

	CDialog::OnOK();
}

BOOL CScriptKeyframeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_Time.SetFilter( FILTER_FLOAT );
	m_Time.SetWindowText( m_sTime );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
