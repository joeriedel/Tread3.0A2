///////////////////////////////////////////////////////////////////////////////
// QuakeBuildOptionsDialog.cpp
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
#include "QuakeBuildOptionsDialog.h"


// CQuakeBuildOptionsDialog dialog

IMPLEMENT_DYNAMIC(CQuakeBuildOptionsDialog, CDialog)

CQuakeBuildOptionsDialog::CQuakeBuildOptionsDialog(QuakeToolsList &tools, CWnd* pParent /*=NULL*/)
	: m_curTool(0), m_tools(tools), CDialog(CQuakeBuildOptionsDialog::IDD, pParent)
{

}

CQuakeBuildOptionsDialog::~CQuakeBuildOptionsDialog()
{
}

void CQuakeBuildOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_QUAKE_BUILDOPTIONS_TOOL_LIST, m_lcTools);
	DDX_Control(pDX, IDC_QUAKE_BUILDOPTIONS_OPTION_LIST, m_lcOptions);
}


BEGIN_MESSAGE_MAP(CQuakeBuildOptionsDialog, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_QUAKE_BUILDOPTIONS_OPTION_LIST, &CQuakeBuildOptionsDialog::OnNMClickQuakeBuildoptionsOptionList)
	ON_NOTIFY(NM_CLICK, IDC_QUAKE_BUILDOPTIONS_TOOL_LIST, &CQuakeBuildOptionsDialog::OnNMClickQuakeBuildoptionsToolList)
	ON_BN_CLICKED(IDOK, &CQuakeBuildOptionsDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CQuakeBuildOptionsDialog message handlers

BOOL CQuakeBuildOptionsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_lcTools.InsertColumn(0, "", LVCFMT_LEFT, 150, -1);
	m_lcOptions.InsertColumn(0, "", LVCFMT_LEFT, 150, -1);

	m_imgList.Create(IDB_QUAKE_CHECK_BUTTONS, 16, 0, RGB(255, 0, 255));
	m_lcTools.SetImageList(&m_imgList, LVSIL_STATE);
	m_imgList2.Create(IDB_QUAKE_CHECK_BUTTONS, 16, 0, RGB(255, 0, 255));
	m_lcOptions.SetImageList(&m_imgList2, LVSIL_STATE);

	for (int i = 0; i < m_tools.size(); ++i)
	{
		m_lcTools.InsertItem(i, m_tools[i].name);
		m_lcTools.SetCheck(i, m_tools[i].enabled);
		m_lcTools.SetItemData(i, (DWORD_PTR)&m_tools[i]);
	}

	if (!m_tools.empty())
	{
		m_lcTools.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_curTool = &m_tools[0];
		LoadOptions(*m_curTool);
		GetDlgItem(IDC_QUAKE_BUILD_OPTIONS_COMMAND_LINE)->SetWindowText(m_curTool->runCmdLine);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CQuakeBuildOptionsDialog::OnNMClickQuakeBuildoptionsOptionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	NMLISTVIEW* pNMItem = (NMLISTVIEW*)pNMHDR;
	LVHITTESTINFO hitTest;
	int nPos;

	hitTest.pt = pNMItem->ptAction;
	m_lcOptions.SubItemHitTest(&hitTest);
	nPos = hitTest.iItem;

	if(nPos == -1)
		return;

	CQuakeToolCmdLineOption *option = (CQuakeToolCmdLineOption*)(m_lcOptions.GetItemData(nPos));

	if (hitTest.flags & LVHT_ONITEMSTATEICON)
	{
		if (m_lcOptions.GetCheck(nPos))
		{
			m_curTool->val &= ~option->val;
			m_lcOptions.SetCheck(nPos, false);
		}
		else
		{
			m_curTool->val |= option->val;
			m_lcOptions.SetCheck(nPos, true);
		}
	}

	*pResult = 0;
}

void CQuakeBuildOptionsDialog::OnNMClickQuakeBuildoptionsToolList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	NMLISTVIEW* pNMItem = (NMLISTVIEW*)pNMHDR;
	LVHITTESTINFO hitTest;
	int nPos;

	hitTest.pt = pNMItem->ptAction;
	m_lcTools.SubItemHitTest(&hitTest);
	nPos = hitTest.iItem;

	if(nPos == -1)
		return;

	CQuakeTool *tool = (CQuakeTool*)(m_lcTools.GetItemData(nPos));

	if (hitTest.flags & LVHT_ONITEMSTATEICON)
	{
		if (m_lcTools.GetCheck(nPos))
		{
			tool->enabled = false;
			m_lcTools.SetCheck(nPos, false);
		}
		else
		{
			tool->enabled = true;
			m_lcTools.SetCheck(nPos, true);
		}
	}
	else
	{
		GetDlgItem(IDC_QUAKE_BUILD_OPTIONS_COMMAND_LINE)->GetWindowText(m_curTool->runCmdLine);
		GetDlgItem(IDC_QUAKE_BUILD_OPTIONS_COMMAND_LINE)->SetWindowText(tool->runCmdLine);
		LoadOptions(*tool);
		m_curTool = tool;
	}

	*pResult = 0;
}

void CQuakeBuildOptionsDialog::LoadOptions(CQuakeTool &tool)
{
	m_lcOptions.DeleteAllItems();

	int x = 0;
	for (int i = 0; i < tool.opts.size(); i++)
	{
		if (!tool.opts[i].always)
		{
			m_lcOptions.InsertItem(x, tool.opts[i].name);
			m_lcOptions.SetCheck(x, tool.val & tool.opts[i].val);
			m_lcOptions.SetItemData(x, (DWORD_PTR)&tool.opts[i]);
			++x;
		}
	}
}
void CQuakeBuildOptionsDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if (m_curTool)
	{
		GetDlgItem(IDC_QUAKE_BUILD_OPTIONS_COMMAND_LINE)->GetWindowText(m_curTool->runCmdLine);
	}
	OnOK();
}
