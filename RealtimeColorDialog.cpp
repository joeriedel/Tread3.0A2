///////////////////////////////////////////////////////////////////////////////
// RealtimeColorDialog.cpp
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
#include "RealtimeColorDialog.h"
#include <ColorDlg.h>
#include "System.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRealtimeColorDialog

IMPLEMENT_DYNAMIC(CRealtimeColorDialog, CColorDialog)

CRealtimeColorDialog* CRealtimeColorDialog::m_Dialog = 0;

CRealtimeColorDialog::CRealtimeColorDialog(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd) :
	CColorDialog(clrInit, dwFlags, pParentWnd)
{
	m_Callback = 0;
	m_Data = 0;
	m_cc.lpfnHook = dlgProc;
}

UINT_PTR CALLBACK CRealtimeColorDialog::dlgProc( HWND dlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( m_Dialog )
	{
		bool callback = false;
		if( uiMsg == WM_PAINT )
		{
			callback = true;
		}

		if( callback )
		{
			m_Dialog->ControlCallback( m_Dialog );
		}
	}

	return 0;
}

void CRealtimeColorDialog::SetColorChangeCallback( void (*Callback) ( COLORREF clr, void* data ), void* data )
{
	m_Callback = Callback;
	m_Data = data;
}

INT_PTR CRealtimeColorDialog::DoModal( )
{
	m_Dialog = this;
	return CColorDialog::DoModal();
}


BEGIN_MESSAGE_MAP(CRealtimeColorDialog, CColorDialog)
	//{{AFX_MSG_MAP(CRealtimeColorDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CRealtimeColorDialog::ControlCallback( void* data )
{
	CRealtimeColorDialog* dlg = (CRealtimeColorDialog*)data;
	if( dlg )
	{
		CString rs, gs, bs;
		dlg->GetDlgItem( COLOR_RED )->GetWindowText( rs );
		dlg->GetDlgItem( COLOR_GREEN )->GetWindowText( gs );
		dlg->GetDlgItem( COLOR_BLUE )->GetWindowText( bs );

		COLORREF clr = RGB( atoi( rs ), atoi( gs ), atoi( bs ) );

		if( dlg->m_Callback )
			dlg->m_Callback( clr, dlg->m_Data );
	}
}

