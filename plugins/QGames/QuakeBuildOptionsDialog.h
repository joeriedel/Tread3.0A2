///////////////////////////////////////////////////////////////////////////////
// QuakeBuildOptionsDialog.h
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

#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "Quake.h"

// CQuakeBuildOptionsDialog dialog

class CQuakeBuildOptionsDialog : public CDialog
{
	DECLARE_DYNAMIC(CQuakeBuildOptionsDialog)
	
private:

	CImageList m_imgList;
	CImageList m_imgList2;
	CQuakeTool *m_curTool;
	void LoadOptions(CQuakeTool &tool);

public:
	CQuakeBuildOptionsDialog(QuakeToolsList &tools, CWnd* pParent = NULL);   // standard constructor
	virtual ~CQuakeBuildOptionsDialog();

// Dialog Data
	enum { IDD = IDD_QUAKE_BUILD_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_lcTools;
	CListCtrl m_lcOptions;
	QuakeToolsList &m_tools;
	afx_msg void OnNMClickQuakeBuildoptionsOptionList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickQuakeBuildoptionsToolList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
};
