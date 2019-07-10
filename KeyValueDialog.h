///////////////////////////////////////////////////////////////////////////////
// KeyValueDialog.cpp
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

#if !defined(AFX_KEYVALUEDIALOG_H__6746C9CC_176D_4A59_A868_953F6DE4FA23__INCLUDED_)
#define AFX_KEYVALUEDIALOG_H__6746C9CC_176D_4A59_A868_953F6DE4FA23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KeyValueDialog.h : header file
//
#include "FilterEdit.h"
#include "System.h"
#include "ColorStatic.h"

/////////////////////////////////////////////////////////////////////////////
// CKeyValueDialog dialog

class CKeyValueDialog : public CDialog
{
public:

	void SetProp( CObjProp* prop );
	CString GetValue();

	void SetTitle( const char* title );
	void SetFilter( int filter );
	void AllowEmpty( bool allow = true );

private:

	CListCtrl* GetList();
	int m_iVal;
	CString m_sVal;
	CString m_sTitle;
	int m_nFilter;
	bool m_bAllowEmpty;
	bool m_bUserChange;
	bool m_bAutoApply;
	CObjProp* m_pProp;
	CImageList m_SubImageList;
	CColorStatic m_colorstatic;

	void UpdateSubItemStates();

	static void ColorDialogCallback( COLORREF clr, void* data );

// Construction
public:
	CKeyValueDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKeyValueDialog)
	enum { IDD = IDD_KEYVALUE_DIALOG };
	CFilterEdit	m_Edit;
	CListCtrl	m_lcList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyValueDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKeyValueDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEdit1();
	afx_msg void OnColorStatic();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYVALUEDIALOG_H__6746C9CC_176D_4A59_A868_953F6DE4FA23__INCLUDED_)
