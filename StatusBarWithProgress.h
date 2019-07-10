///////////////////////////////////////////////////////////////////////////////
// StatusBarWithProgress.h
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

#ifndef __StatusBarWithProgress_Hpp__
#define __StatusBarWithProgress_Hpp__


//--- A status bar with a progress indicator --------------------------------

// The progress indicator is initially hidden, you can show it by 
// calling the ShowProgress member
// When it is visible, th progress bar is positioned after 
// the text in the first pane

class CStatusBarWithProgress : public CStatusBar
{
    DECLARE_DYNCREATE(CStatusBarWithProgress)

// Data members
protected:
    CProgressCtrl m_Progress;
    int           m_nProgressWidth;
    BOOL          m_bProgressVisible;

// Construction
public:
    CStatusBarWithProgress(int nProgressBarWidth =100);
    virtual BOOL Create(CWnd *pParentWnd, DWORD dwStyle =WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, UINT nID =AFX_IDW_STATUS_BAR);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusBarWithProgress)
	//}}AFX_VIRTUAL

// Implementation
public:
    BOOL IsProgressBarVisible() const     { return m_bProgressVisible; }
    void SetProgressBarWidth(int nWidth)  { m_nProgressWidth =nWidth;  }
    BOOL ShowProgressBar(BOOL bShow =TRUE);

    void SetRange(int nLower, int nUpper) { m_Progress.SetRange(nLower,nUpper); }
    int  SetPos(int nPos)                 { return m_Progress.SetPos(nPos);     }
    int  OffsetPos(int nPos)              { return m_Progress.OffsetPos(nPos);  }
    int  SetStep(int nStep)               { return m_Progress.SetStep(nStep);   }
    int  StepIt()                         { return m_Progress.StepIt();         }

// Helpers
private:
    void AdjustProgressBarPosition();

// Generated message map functions
protected:
	//{{AFX_MSG(CStatusBarWithProgress)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif

