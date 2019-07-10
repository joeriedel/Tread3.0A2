///////////////////////////////////////////////////////////////////////////////
// QuakeConfig.h
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
#include "afxwin.h"
#include "Quake.h"
#include "HScrollListBox.h"

// CQuakeConfig dialog

class CQuakeConfig : public CDialog
{
	DECLARE_DYNAMIC(CQuakeConfig)

public:
	CQuakeConfig(const char *gameName, const char *pakType, CString &exePath, CString &gameDir, CString &exeCmdLine, QuakeToolsList &tools, QuakePakList &pakList, CWnd* pParent = NULL);   // standard constructor
	virtual ~CQuakeConfig();

// Dialog Data
	enum { IDD = IDD_QUAKECONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangePakfiles();
	afx_msg void OnBnClickedAddPak();
	afx_msg void OnBnClickedRemovePak();
	CHScrollListBox m_lbPakList;
	CHScrollListBox m_lbToolList;

private:

	CString m_gameName;
	CString m_pakType;
	CString &m_exePath;
	CString &m_gameDir;
	CString &m_exeCmdLine;
	QuakeToolsList &m_tools;
	QuakePakList &m_pakList;
	CQuakeTool *m_curTool;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLocateGame();
	afx_msg void OnBnClickedLocateTool();
	afx_msg void OnEnChangeGameexe();
	afx_msg void OnLbnSelchangeTools();
	afx_msg void OnEnChangeToolLoc();
	afx_msg void OnEnChangeCmdline();
	afx_msg void OnEnChangeExeCmdline();
	afx_msg void OnBnClickedLocateDir();
	afx_msg void OnEnChangeGameDir();
};
