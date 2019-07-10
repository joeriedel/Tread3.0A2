///////////////////////////////////////////////////////////////////////////////
// StatusBarWithProgress.cpp
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

#include "StdAfx.H"

#include "StatusBarWithProgress.h"


//--- Debugee ---------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//--- Miscellaneous ---------------------------------------------------------

#define ID_STATUS_PROGRESS  17234


//--- Message map 4 classs CStatusBarWithProgress ---------------------------

IMPLEMENT_DYNCREATE(CStatusBarWithProgress,CStatusBar)

BEGIN_MESSAGE_MAP(CStatusBarWithProgress,CStatusBar)
	//{{AFX_MSG_MAP(CStatusBarWithProgress)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
// Pre     : 
// Post    : 
// Globals : 
// I/O     : 
// Task    : Construct a status bar with a progress control
//---------------------------------------------------------------------------
CStatusBarWithProgress::CStatusBarWithProgress(int nProgressBarWidth):
                        m_nProgressWidth(nProgressBarWidth),
                        m_bProgressVisible(FALSE)
{
}

//---------------------------------------------------------------------------
// Pre     : 
// Post    : 
// Globals : 
// I/O     : 
// Task    : When creating the status bar, also create the progress control,
//           but do not show it yet
//---------------------------------------------------------------------------
BOOL CStatusBarWithProgress::Create(CWnd *pParentWnd, DWORD dwStyle, UINT nID)
{
    // Default creation
    BOOL bCreatedOK =CStatusBar::Create(pParentWnd,dwStyle,nID);
    if(bCreatedOK)
    {
        // Also create the progress bar
        m_Progress.Create(WS_CHILD | WS_EX_STATICEDGE,CRect(0,0,m_nProgressWidth,10),this,ID_STATUS_PROGRESS);
    }

    return bCreatedOK;
}

//---------------------------------------------------------------------------
// Pre     : 
// Post    : Return old visible status
// Globals : 
// I/O     : 
// Task    : Show/hide the progress bar
//---------------------------------------------------------------------------
BOOL CStatusBarWithProgress::ShowProgressBar(BOOL bShow)
{
    // Save old visible status
    BOOL bOldVisible =m_bProgressVisible;

    if((bOldVisible != bShow) && ::IsWindow(m_Progress.m_hWnd))
    {
        // Show/hide
        m_Progress.ShowWindow(bShow ? SW_SHOWNA : SW_HIDE);
        m_bProgressVisible =bShow;

        // If just shown, make sure it's in the right position
        if(bShow)
        {
            AdjustProgressBarPosition();
            RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW);
        }
    }

    return bOldVisible;
}

//---------------------------------------------------------------------------
// Pre     : 
// Post    : 
// Globals : 
// I/O     : 
// Task    : Adjust the progress bar's position, so that it comes after the text 
//           in the first pane
//---------------------------------------------------------------------------
void CStatusBarWithProgress::AdjustProgressBarPosition()
{
    // Make sure the progress bar is created
    if(!::IsWindow(m_Progress.m_hWnd))
        return;

    // Find out the size of the text in first pane
    CString str_pane_text;
    CRect   progress_rect;
    GetItemRect(0,progress_rect);
    GetPaneText(0,str_pane_text);
    
    // Measure the size of the text in the first pane
    CClientDC temp(this);
    CFont *pOldFont =temp.SelectObject(GetFont());

    progress_rect.left  =temp.GetTextExtent(str_pane_text).cx + 10;
    progress_rect.right =progress_rect.left + m_nProgressWidth;
    progress_rect.InflateRect(0,-1);

    temp.SelectObject(pOldFont);

    // Now adjust the size of the progrss control
    m_Progress.SetWindowPos(NULL,progress_rect.left,
                                 progress_rect.top,
                                 progress_rect.Width(),
                                 progress_rect.Height(),SWP_NOZORDER | SWP_NOACTIVATE);
}

//---------------------------------------------------------------------------
// Pre     : 
// Post    : 
// Globals : 
// I/O     : 
// Task    : After each sizing, if the progress bar is visible, adjust 
//           its position
//---------------------------------------------------------------------------
void CStatusBarWithProgress::OnSize(UINT nType, int cx, int cy) 
{
    // Default sizing
	CStatusBar::OnSize(nType, cx, cy);
	
    if(m_bProgressVisible)
        AdjustProgressBarPosition();
}
