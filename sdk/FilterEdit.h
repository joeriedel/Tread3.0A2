///////////////////////////////////////////////////////////////////////////////
// FilterEdit.h
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

#if !defined(AFX_FILTEREDIT_H__5386B7C1_9BB5_11D3_BD51_00A0CC582962__INCLUDED_)
#define AFX_FILTEREDIT_H__5386B7C1_9BB5_11D3_BD51_00A0CC582962__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilterEdit window
#define FILTER_CHAR			1
#define FILTER_NUMBER		2
#define FILTER_PUNCTUATION	4
#define FILTER_BACKSPACE	8
#define FILTER_DASH			16
#define FILTER_SPACE		32
#define FILTER_PERIOD		64

#define FILTER_NONE			0
#define FILTER_INT			(FILTER_CHAR|FILTER_PUNCTUATION|FILTER_SPACE|FILTER_PERIOD)
#define FILTER_TEXT			FILTER_NONE
#define FILTER_FLOAT		(FILTER_CHAR|FILTER_PUNCTUATION|FILTER_SPACE)
#define FILTER_MULTI_INT	(FILTER_INT&~FILTER_SPACE)
#define FILTER_MULTI_FLOAT	(FILTER_FLOAT&~FILTER_SPACE)

class OS_CLEXP CFilterEdit : public CEdit
{
private:
	int m_nFilter;

// Construction
public:
	CFilterEdit();
	void SetFilter(int nFilter);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFilterEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFilterEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTEREDIT_H__5386B7C1_9BB5_11D3_BD51_00A0CC582962__INCLUDED_)
