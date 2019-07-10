///////////////////////////////////////////////////////////////////////////////
// C_Tokenizer.cpp
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
#include "c_tokenizer.h"

#ifdef __OPT_VC__
#include <stdlib.h>
#endif

C_Tokenizer::C_Tokenizer()
{
	m_bUnget = FALSE;
	m_pBuffer = 0;
	m_pOffset = 0;
	m_sToken  = "";
	m_pTokenStart = 0;
	m_pTokenEnd = 0;
}

C_Tokenizer::C_Tokenizer( const C_Tokenizer& t )
{
	this->m_nNumBytes = t.m_nNumBytes;
	this->m_nLine = t.m_nLine;
	this->m_bUnget = t.m_bUnget;
	this->m_sToken = t.m_sToken;
	
	if( t.m_nNumBytes > 0 )
	{
		this->m_pBuffer = (char*)malloc(t.m_nNumBytes);
		OS_ASSERT( this->m_pBuffer );
		this->m_pOffset = this->m_pBuffer + (int)(t.m_pOffset-t.m_pBuffer);
	}
	else
	{
		this->m_pBuffer = this->m_pOffset = 0;
	}	
}

C_Tokenizer::~C_Tokenizer()
{
	FreeScript();
}

void C_Tokenizer::SaveState()
{
	m_sr.m_bUnget = m_bUnget;
	m_sr.m_pBuffer = m_pBuffer;
	m_sr.m_pOffset = m_pOffset;
	m_sr.m_sToken = m_sToken;
	m_sr.m_nLine = m_nLine;
	m_sr.m_nNumBytes = m_nNumBytes;
	m_sr.m_pTokenStart = m_pTokenStart;
	m_sr.m_pTokenEnd = m_pTokenEnd;
}

void C_Tokenizer::RestoreState()
{
	m_bUnget = m_sr.m_bUnget;
	m_pBuffer = m_sr.m_pBuffer;
	m_pOffset = m_sr.m_pOffset;
	m_sToken = m_sr.m_sToken;
	m_nLine = m_sr.m_nLine;
	m_nNumBytes = m_sr.m_nNumBytes;
	m_pTokenStart = m_sr.m_pTokenStart;
	m_pTokenEnd = m_sr.m_pTokenEnd;
}

const char* C_Tokenizer::GetTokenStart()
{
	return m_pTokenStart;
}

const char* C_Tokenizer::GetTokenEnd()
{
	return m_pTokenEnd;
}

bool C_Tokenizer::FindToken( const char* token )
{
	CString t;
	while( GetToken(t) )
	{
		if(!strcmp( t, token ) )
			return TRUE;
	}
	return FALSE;
}

bool C_Tokenizer::FindTokeni( const char* token )
{
	CString t;
	while( GetToken(t) )
	{
		if(!stricmp( t, token ) )
			return TRUE;
	}
	return FALSE;
}

bool C_Tokenizer::IsNextToken( const char* token )
{
	CString t;
	return GetToken(t) && !strcmp(t, token);
}

bool C_Tokenizer::IsNextTokeni( const char* token )
{
	CString t;
	return GetToken(t) && !stricmp(t, token);
}
	
void C_Tokenizer::FreeScript()
{
	if(m_pBuffer != 0 && m_pBuffer != m_default_buff )
		delete(m_pBuffer);
	m_pBuffer = 0;
}

void C_Tokenizer::Skip(int nNum)
{
	int i;
	CString s;

	for(i = 0; i < nNum; i++)
		GetToken(s);
}

bool C_Tokenizer::InitParsing(const char* szScript, int nNumBytes)
{
	FreeScript();

	if(nNumBytes == WHOLE_FILE)
		nNumBytes = strlen(szScript);

	if( nNumBytes > DEFAULT_TOKENIZER_SIZE )
	{
		m_pBuffer = (char*)malloc(nNumBytes+1);
		if(m_pBuffer == 0)
			return FALSE;
	}
	else
	{
		m_pBuffer = m_default_buff;
	}
	
	FastMemCpy(m_pBuffer, szScript, nNumBytes);
	m_nNumBytes = nNumBytes;
	m_nLine = 0;
	m_pOffset = m_pBuffer;
	m_pBuffer[nNumBytes] = 0;
	m_pTokenStart = m_pOffset;
	m_pTokenEnd = m_pOffset;

	return TRUE;
}

void C_Tokenizer::RestartParsing()
{
	m_sToken = "";
	m_pOffset = m_pBuffer;
	m_nLine = 0;
}

bool C_Tokenizer::GetFloat( float* f )
{
	CString t;
	if( !GetToken(t) )
		return FALSE;
	*f = (float)atof(t);
	return TRUE;
}

bool C_Tokenizer::GetInt( int* i )
{
	CString t;
	if( !GetToken(t) )
		return FALSE;
	*i = (int)atoi(t);
	return TRUE;
}

// Get's the next token out of the input stream.
bool C_Tokenizer::GetToken(CString& sToken)
{
	if(m_bUnget)
	{
		m_bUnget = FALSE;
		sToken = m_sToken;
		return TRUE;
	}

	sToken = "";
	m_sToken = "";

	if(m_pOffset == NULL || *m_pOffset == '\0')
		return FALSE;

CTokenizer_GetToken_Start:

	// Skip to the next.
	if(*m_pOffset <= 32 && *m_pOffset != '\0')
	{
		while(*m_pOffset <= 32 && *m_pOffset != '\0')
		{
			if(*m_pOffset == '\n')
				m_nLine++;

			m_pOffset++;
		}
	}

	if(*m_pOffset == '\0')
		return FALSE;

	// Skip any comments.
	if(*m_pOffset == '/')
	{
		switch(m_pOffset[1])
		{

		case '/':

			while(*m_pOffset != '\n' && *m_pOffset != '\0')
				m_pOffset++;

			if(*m_pOffset == '\0')	// Bad
				return FALSE;
			else
				m_pOffset++;

			m_nLine++;
			goto CTokenizer_GetToken_Start;

			break;

		case '*':

			// Skip till the next */.
			{
				char *TempPtr;
				TempPtr = m_pOffset;
				while(*TempPtr != '\0' && (*TempPtr != '*' || TempPtr[1] != '/'))
				{
					if(*TempPtr == '\n')
						m_nLine++;

					TempPtr++;
				}
	
				if(*TempPtr == '\0') {
					m_pOffset = TempPtr;
					return FALSE;
				} else
					TempPtr+=2;
				m_pOffset = TempPtr;
				goto CTokenizer_GetToken_Start;
			}

			break;
		}
	}
	else if(*m_pOffset == '"')	// Quoted string?
	{
		m_pTokenStart = m_pOffset;

		// Read till closing ".
		m_pOffset++;
		while(*m_pOffset != '\0' && *m_pOffset != '"')
		{
			// Allow \" for a quote character in a string and a \\ to be a single \*
			if(*m_pOffset == '\\')
			{
				if(m_pOffset[1] == '"' || m_pOffset[1] == '\\')
					m_pOffset++;
			}

			if(*m_pOffset == '\n')
				m_nLine++;
			else
				m_sToken += *m_pOffset;
			m_pOffset++;
		}

		if(*m_pOffset != '\0')
			m_pOffset++;
		
		m_pTokenEnd = m_pOffset;

		sToken = m_sToken;
		return TRUE;
	}

	m_pTokenStart = m_pOffset;

	// Load the next white space delim token.
	while(*m_pOffset > 32 && *m_pOffset != '\0')
	{
		m_sToken += *m_pOffset;
		m_pOffset++;
	}
	
	m_pTokenEnd = m_pOffset;

	// Copy the token.
	sToken = m_sToken;

	return TRUE;
}