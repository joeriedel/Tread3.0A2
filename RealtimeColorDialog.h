///////////////////////////////////////////////////////////////////////////////
// RealtimeColorDialog.h
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

#if !defined(AFX_REALTIMECOLORDIALOG_H__D76A0A32_B4DD_4AD7_A3A7_2A29178C0E93__INCLUDED_)
#define AFX_REALTIMECOLORDIALOG_H__D76A0A32_B4DD_4AD7_A3A7_2A29178C0E93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RealtimeColorDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRealtimeColorDialog dialog

class CRealtimeColorDialog : public CColorDialog
{
	DECLARE_DYNAMIC(CRealtimeColorDialog)

private:

	static CRealtimeColorDialog* m_Dialog;
	void* m_Data;
	void (*m_Callback) ( COLORREF clr, void* data );

	static void ControlCallback( void* data );
	static UINT_PTR CALLBACK dlgProc( HWND dlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );

public:
	CRealtimeColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0,
			CWnd* pParentWnd = NULL);

	void SetColorChangeCallback( void (*Callback) ( COLORREF clr, void* data ), void* data );

	virtual INT_PTR DoModal( );

protected:
	//{{AFX_MSG(CRealtimeColorDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REALTIMECOLORDIALOG_H__D76A0A32_B4DD_4AD7_A3A7_2A29178C0E93__INCLUDED_)
