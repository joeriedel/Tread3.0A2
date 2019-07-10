///////////////////////////////////////////////////////////////////////////////
// QuakeConfig.cpp
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
#include "QuakeConfig.h"


// CQuakeConfig dialog

IMPLEMENT_DYNAMIC(CQuakeConfig, CDialog)

CQuakeConfig::CQuakeConfig(const char *gameName, const char *pakType, CString &exePath, CString &gameDir, CString &exeCmdLine, QuakeToolsList &tools, QuakePakList &pakList, CWnd* pParent /*=NULL*/)
	: CDialog(CQuakeConfig::IDD, pParent), m_gameName(gameName), m_pakType(pakType), m_exePath(exePath), m_gameDir(gameDir), m_exeCmdLine(exeCmdLine), m_tools(tools), m_pakList(pakList), m_curTool(0)
{

}

CQuakeConfig::~CQuakeConfig()
{
}

void CQuakeConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PAKFILES, m_lbPakList);
	DDX_Control(pDX, IDC_TOOLS, m_lbToolList);
}


BEGIN_MESSAGE_MAP(CQuakeConfig, CDialog)
	ON_LBN_SELCHANGE(IDC_PAKFILES, &CQuakeConfig::OnLbnSelchangePakfiles)
	ON_BN_CLICKED(IDC_ADD_PAK, &CQuakeConfig::OnBnClickedAddPak)
	ON_BN_CLICKED(IDC_REMOVE_PAK, &CQuakeConfig::OnBnClickedRemovePak)
	ON_BN_CLICKED(IDC_LOCATE_GAME, &CQuakeConfig::OnBnClickedLocateGame)
	ON_BN_CLICKED(IDC_LOCATE_TOOL, &CQuakeConfig::OnBnClickedLocateTool)
	ON_EN_CHANGE(IDC_GAMEEXE, &CQuakeConfig::OnEnChangeGameexe)
	ON_LBN_SELCHANGE(IDC_TOOLS, &CQuakeConfig::OnLbnSelchangeTools)
	ON_EN_CHANGE(IDC_TOOL_LOC, &CQuakeConfig::OnEnChangeToolLoc)
	ON_EN_CHANGE(IDC_CMDLINE, &CQuakeConfig::OnEnChangeCmdline)
	ON_EN_CHANGE(IDC_EXE_CMDLINE, &CQuakeConfig::OnEnChangeExeCmdline)
	ON_BN_CLICKED(IDC_LOCATE_DIR, &CQuakeConfig::OnBnClickedLocateDir)
	ON_EN_CHANGE(IDC_GAME_DIR, &CQuakeConfig::OnEnChangeGameDir)
END_MESSAGE_MAP()


BOOL CQuakeConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(CString("Configure ") + m_gameName);

	// TODO:  Add extra initialization here
	for (QuakePakList::iterator it = m_pakList.begin(); it != m_pakList.end(); ++it)
	{
		m_lbPakList.AddString(*it);
	}

	if (m_pakList.empty())
	{
		GetDlgItem(IDC_REMOVE_PAK)->EnableWindow(FALSE);
	}
	else
	{
		m_lbPakList.SetCurSel(0);
	}

	for (QuakeToolsList::iterator it = m_tools.begin(); it != m_tools.end(); ++it)
	{
		m_lbToolList.AddString(it->name);
	}

	if (m_tools.empty())
	{
		GetDlgItem(IDC_TOOLS)->EnableWindow(FALSE);
		GetDlgItem(IDC_TOOL_LOC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LOCATE_TOOL)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMDLINE)->EnableWindow(FALSE);
	}
	else
	{
		m_lbToolList.SetCurSel(0);
		OnLbnSelchangeTools();
	}

	GetDlgItem(IDC_GAMEEXE)->SetWindowTextA(m_exePath);
	GetDlgItem(IDC_EXE_CMDLINE)->SetWindowTextA(m_exeCmdLine);
	GetDlgItem(IDC_GAME_DIR)->SetWindowTextA(m_gameDir);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CQuakeConfig message handlers

void CQuakeConfig::OnLbnSelchangePakfiles()
{
	// TODO: Add your control notification handler code here
}

void CQuakeConfig::OnBnClickedAddPak()
{
	// TODO: Add your control notification handler code here
	CString s;
	s.Format("%s Texture Files (%s)|%s|All Files (*.*)|*.*||", m_gameName, m_pakType, m_pakType);
	CFileDialog dlg(true, m_pakType, NULL,
			OFN_NONETWORKBUTTON|OFN_HIDEREADONLY, s, this);
	
	s.Format("Add %s Pak File", m_gameName);
	dlg.m_ofn.lpstrTitle = s;

	if(dlg.DoModal() != IDOK)
		return;

	m_pakList.push_back(dlg.GetPathName());
	m_lbPakList.AddString(dlg.GetPathName());
	m_lbPakList.SetCurSel(m_lbPakList.GetCount()-1);
	GetDlgItem(IDC_REMOVE_PAK)->EnableWindow();
}

void CQuakeConfig::OnBnClickedRemovePak()
{
	// TODO: Add your control notification handler code here
	int idx = m_lbPakList.GetCurSel();
	if (idx != -1)
	{
		m_lbPakList.DeleteString(idx);

		int i = 0;
		for (QuakePakList::iterator it = m_pakList.begin(); it != m_pakList.end(); ++it)
		{
			if (i++ == idx)
			{
				m_pakList.erase(it);
				break;
			}
		}

		if (idx >= m_lbPakList.GetCount())
		{
			idx--;
		}
		if (idx < m_lbPakList.GetCount())
		{
			m_lbPakList.SetCurSel(idx);
		}

		if (m_lbPakList.GetCount() == 0)
		{
			GetDlgItem(IDC_REMOVE_PAK)->EnableWindow(FALSE);
		}
	}
}
void CQuakeConfig::OnBnClickedLocateGame()
{
	// TODO: Add your control notification handler code here
	CString s;
	s.Format("Executable Files (*.exe)|*.exe|All Files (*.*)|*.*||", m_gameName);
	CString path;
	GetDlgItem(IDC_GAMEEXE)->GetWindowTextA(path);
	CFileDialog dlg(true, "exe", path,
			OFN_NONETWORKBUTTON|OFN_HIDEREADONLY, s, this);
	
	s.Format("Locate %s Executable", m_gameName);
	dlg.m_ofn.lpstrTitle = s;

	if(dlg.DoModal() != IDOK)
		return;

	GetDlgItem(IDC_GAMEEXE)->SetWindowTextA(dlg.GetPathName());
	{
		char buff[1024];
		buff[0] = 0;
		Sys_GetDirectory(dlg.GetPathName(), buff, 1024);
		GetDlgItem(IDC_GAME_DIR)->SetWindowTextA(buff);
	}
}

void CQuakeConfig::OnBnClickedLocateTool()
{
	if (m_curTool != 0)
	{
		// TODO: Add your control notification handler code here
		CString s;
		s.Format("Executable Files (*.exe)|*.exe|All Files (*.*)|*.*||", m_gameName);
		CFileDialog dlg(true, "exe", m_curTool->path,
				OFN_NONETWORKBUTTON|OFN_HIDEREADONLY, s, this);
		
		s.Format("Locate %s Executable", m_curTool->name);
		dlg.m_ofn.lpstrTitle = s;

		if(dlg.DoModal() != IDOK)
			return;

		GetDlgItem(IDC_TOOL_LOC)->SetWindowTextA(dlg.GetPathName());
	}
}

void CQuakeConfig::OnEnChangeGameexe()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_GAMEEXE)->GetWindowTextA(m_exePath);
}

void CQuakeConfig::OnLbnSelchangeTools()
{
	// TODO: Add your control notification handler code here
	int idx = m_lbToolList.GetCurSel();
	if (idx != -1)
	{
		int i = 0; 
		for (QuakeToolsList::iterator it = m_tools.begin(); it != m_tools.end(); ++it)
		{
			if (i++ == idx)
			{
				m_curTool = &(*it);
				break;
			}
		}

		GetDlgItem(IDC_TOOL_LOC)->SetWindowTextA(m_curTool->path);
		GetDlgItem(IDC_CMDLINE)->SetWindowTextA(m_curTool->cmdLine);
	}
}

void CQuakeConfig::OnEnChangeToolLoc()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_TOOL_LOC)->GetWindowTextA(m_curTool->path);
}

void CQuakeConfig::OnEnChangeCmdline()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_CMDLINE)->GetWindowTextA(m_curTool->cmdLine);
}

void CQuakeConfig::OnEnChangeExeCmdline()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_EXE_CMDLINE)->GetWindowTextA(m_exeCmdLine);
}

void CQuakeConfig::OnBnClickedLocateDir()
{
	char buff[1024];
	strcpy(buff, m_gameDir);
	if (Sys_BrowseForFolder("Browse For Game Directory", buff, 1024))
	{
		GetDlgItem(IDC_GAME_DIR)->SetWindowTextA(buff);
	}
}

void CQuakeConfig::OnEnChangeGameDir()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_GAME_DIR)->GetWindowTextA(m_gameDir);
}
