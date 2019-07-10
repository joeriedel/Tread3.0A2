///////////////////////////////////////////////////////////////////////////////
// CompileOptionsDialog.cpp
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
#include "CompileOptionsDialog.h"
#include "TreadDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompileOptionsDialog dialog


CCompileOptionsDialog::CCompileOptionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCompileOptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompileOptionsDialog)
	m_bSkipCSG = FALSE;
	m_bAggressiveCSG = FALSE;
	m_bNoFill = FALSE;
	m_bNoTJuncs = FALSE;
	m_bNoMerge = FALSE;
	m_bEntsOnly = FALSE;
	m_bNoSubdivide = FALSE;
	//}}AFX_DATA_INIT

	m_opts = 0;
}


void CCompileOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompileOptionsDialog)
	DDX_Check(pDX, IDC_CHECK1, m_bSkipCSG);
	DDX_Check(pDX, IDC_CHECK2, m_bAggressiveCSG);
	DDX_Check(pDX, IDC_CHECK3, m_bNoFill);
	DDX_Check(pDX, IDC_CHECK4, m_bNoTJuncs);
	DDX_Check(pDX, IDC_CHECK5, m_bNoMerge);
	DDX_Check(pDX, IDC_CHECK6, m_bEntsOnly);
	DDX_Check(pDX, IDC_CHECK16, m_bNoSubdivide);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompileOptionsDialog, CDialog)
	//{{AFX_MSG_MAP(CCompileOptionsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCompileOptionsDialog::SetOpts(int opts)
{
	m_opts = opts;
}

int CCompileOptionsDialog::GetOpts()
{
	return m_opts;
}

/////////////////////////////////////////////////////////////////////////////
// CCompileOptionsDialog message handlers

BOOL CCompileOptionsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bSkipCSG = (m_opts&COMPILE_OPT_NOCSG)?1:0;
	m_bAggressiveCSG = (m_opts&COMPILE_OPT_AGGRESSIVE_CSG)?1:0;
	m_bNoFill = (m_opts&COMPILE_OPT_NOFILL)?1:0;
	m_bNoTJuncs = (m_opts&COMPILE_OPT_NOTJUNC)?1:0;
	m_bNoMerge = (m_opts&COMPILE_OPT_NOMERGE)?1:0;
	m_bEntsOnly = (m_opts&COMPILE_OPT_ENTITIES_ONLY)?1:0;
	m_bNoSubdivide = (m_opts&COMPILE_OPT_NOSUBDIVIDE)?1:0;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCompileOptionsDialog::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();

	m_opts = 0;
	if( m_bSkipCSG )
		m_opts |= COMPILE_OPT_NOCSG;
	if( m_bAggressiveCSG )
		m_opts |= COMPILE_OPT_AGGRESSIVE_CSG;
	if( m_bNoFill )
		m_opts |= COMPILE_OPT_NOFILL;
	if( m_bNoTJuncs )
		m_opts |= COMPILE_OPT_NOTJUNC;
	if( m_bNoMerge )
		m_opts |= COMPILE_OPT_NOMERGE;
	if( m_bEntsOnly )
		m_opts |= COMPILE_OPT_ENTITIES_ONLY;
	if( m_bNoSubdivide )
		m_opts |= COMPILE_OPT_NOSUBDIVIDE;

	CDialog::OnOK();
}
