///////////////////////////////////////////////////////////////////////////////
// ObjectPropertiesDialog.h
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

#if !defined(AFX_OBJECTPROPERTIESDIALOG_H__A071AB97_899B_41CC_B29D_064B55426FFD__INCLUDED_)
#define AFX_OBJECTPROPERTIESDIALOG_H__A071AB97_899B_41CC_B29D_064B55426FFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectPropertiesDialog.h : header file
//
class CObjProp;
class CTreadDoc;

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertiesDialog dialog

class CObjectPropertiesDialog : public CDialog
{
private:

	bool m_bInit;
	CTreadDoc *m_doc;
	
	static void ColorDialogCallback( COLORREF clr, void* data );

public:

	void EnableControls( bool enable = TRUE );
	CListCtrl* GetPropList();
	void UpdateProp( CObjProp* prop );

// Construction
public:
	CObjectPropertiesDialog(CTreadDoc *doc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectPropertiesDialog)
	enum { IDD = IDD_OBJECT_PROPERTIES_DIALOG };
	CListCtrl	m_lcItems;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectPropertiesDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectPropertiesDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTPROPERTIESDIALOG_H__A071AB97_899B_41CC_B29D_064B55426FFD__INCLUDED_)
