///////////////////////////////////////////////////////////////////////////////
// FilterEdit.cpp
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
#include "FilterEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterEdit

CFilterEdit::CFilterEdit()
{
	m_nFilter = 0;
}

CFilterEdit::~CFilterEdit()
{
}


BEGIN_MESSAGE_MAP(CFilterEdit, CEdit)
	//{{AFX_MSG_MAP(CFilterEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterEdit message handlers

void CFilterEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	if(m_nFilter & FILTER_CHAR)
	{
		if((nChar >= 65) && (nChar <= 90))
			return;
		if((nChar >= 97) && (nChar <= 122))
			return;
	}

	if(m_nFilter & FILTER_NUMBER)
	{
		if((nChar >= 48) && (nChar <= 57))
			return;
	}

	if(m_nFilter & FILTER_SPACE)
		if(nChar == 32)
			return;

	if(m_nFilter & FILTER_DASH)
		if(nChar == 45)
			return;

	if(m_nFilter & FILTER_PUNCTUATION)
	{
		if((nChar >= 33) && (nChar <= 44))
			return;
		if((nChar >= 58) && (nChar <= 64))
			return;
		if((nChar >= 91) && (nChar <= 96))
			return;
		switch(nChar)
		{
		case 47:
			return;
		}
	}

	if( m_nFilter & FILTER_PERIOD )
	{
		if( nChar == '.' )
			return;
	}

	if(m_nFilter & FILTER_BACKSPACE)
	{
		if(nChar == VK_BACK)
			return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CFilterEdit::SetFilter(int nFilter)
{
	m_nFilter = nFilter;
}
