///////////////////////////////////////////////////////////////////////////////
// SelectGameBuildDialog.cpp
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
#include "SelectGameBuildDialog.h"
#include "TreadDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectGameBuildDialog dialog


CSelectGameBuildDialog::CSelectGameBuildDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectGameBuildDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectGameBuildDialog)
	m_nReleaseDebug = -1;
	m_bLowPriority = FALSE;
	//}}AFX_DATA_INIT

	m_bRelease = true;
	m_opts = 0;
}


void CSelectGameBuildDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectGameBuildDialog)
	DDX_Radio(pDX, IDC_RADIO1, m_nReleaseDebug);
	DDX_Check(pDX, IDC_CHECK1, m_bLowPriority);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectGameBuildDialog, CDialog)
	//{{AFX_MSG_MAP(CSelectGameBuildDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectGameBuildDialog message handlers

void CSelectGameBuildDialog::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();

	m_bRelease = (m_nReleaseDebug==1)?false:true;
	
	m_opts = 0;

	if( m_bLowPriority )
		m_opts |= RUN_OPT_NORMAL_PRIORITY;

	CDialog::OnOK();
}

void CSelectGameBuildDialog::SetRunOpts( int opts )
{
	m_opts = opts;
}

int CSelectGameBuildDialog::GetRunOpts()
{
	return m_opts;
}

void CSelectGameBuildDialog::SetReleaseDebug( bool release )
{
	m_bRelease = release;
}

bool CSelectGameBuildDialog::GetReleaseDebug()
{
	return m_bRelease;
}

BOOL CSelectGameBuildDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_nReleaseDebug = (m_bRelease)?0:1;
	m_bLowPriority = (m_opts&RUN_OPT_NORMAL_PRIORITY)?1:0;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
