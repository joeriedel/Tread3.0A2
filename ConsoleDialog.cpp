///////////////////////////////////////////////////////////////////////////////
// ConsoleDialog.cpp
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
#include "ConsoleDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConsoleDialog dialog


CConsoleDialog::CConsoleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConsoleDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConsoleDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bCanHide = true;
	m_bInit = false;

}


void CConsoleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConsoleDialog)
	DDX_Control(pDX, IDC_TEXT_OUTPUT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConsoleDialog, CDialog)
	//{{AFX_MSG_MAP(CConsoleDialog)
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConsoleDialog message handlers

HBRUSH CConsoleDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	/*if( nCtlColor == CTLCOLOR_EDIT )
	{
		HBRUSH hbr = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	}*/

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CConsoleDialog::AllowHide( bool allow )
{
	m_bCanHide = allow;
}

void CConsoleDialog::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bCanHide )
		ShowWindow( SW_HIDE );

	//CDialog::OnClose();
}

int CConsoleDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CenterWindow();

	return 0;
}

void CConsoleDialog::Clear()
{
	m_Text.SetWindowText( "" );
}

void CConsoleDialog::WriteText( const char* szText )
{
	if( !szText || !szText[0] )
		return;

	CString s = szText;
	s.Replace( "\n", "\r\n" );
	m_Text.SetSel( -1, -1 );
	m_Text.ReplaceSel( s );
	m_Text.SetSel( -1, -1 );
}

BOOL CConsoleDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bInit = true;

	m_Font.CreateFont(16, 0, 0, 0, FW_NORMAL, false, false, false, 
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Courier New");

	m_Text.SetLimitText( 1024*1024 );
	m_Text.SetFont( &m_Font );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConsoleDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( m_bInit )
	{
		CRect rect( 0, 0, cx, cy );
		m_Text.MoveWindow(rect);
	}
}
