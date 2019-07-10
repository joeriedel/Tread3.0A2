///////////////////////////////////////////////////////////////////////////////
// InputLineDialog.h
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

#if !defined(AFX_INPUTLINEDIALOG_H__A8285086_F869_4AD6_9289_99276CB49023__INCLUDED_)
#define AFX_INPUTLINEDIALOG_H__A8285086_F869_4AD6_9289_99276CB49023__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputLineDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputLineDialog dialog
#include "FilterEdit.h"

class OS_CLEXP CInputLineDialog : public CDialog
{
// Construction
public:

	static CInputLineDialog *New();
	static void Delete(CInputLineDialog *dlg);

	CInputLineDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputLineDialog)
	enum { IDD = IDD_INPUT_FIELD_DIALOG };
	CFilterEdit	m_Edit;
	//}}AFX_DATA

	void SetValue( CString val );
	const char *GetValue();

	void SetTitle( const char* title );

	void SetFilter( int filter );
	void AllowEmpty( bool allow = true );
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputLineDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString m_sVal;
	CString m_sTitle;
	int m_nFilter;
	bool m_bAllowEmpty;

	// Generated message map functions
	//{{AFX_MSG(CInputLineDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTLINEDIALOG_H__A8285086_F869_4AD6_9289_99276CB49023__INCLUDED_)
