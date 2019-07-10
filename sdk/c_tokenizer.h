///////////////////////////////////////////////////////////////////////////////
// c_tokenizer.h
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

#ifndef C_TOKENIZER_H
#define C_TOKENIZER_H

#include "os.h"

// $sb "c_tokenizer.h block1"

#define WHOLE_FILE					-1
#define DEFAULT_TOKENIZER_SIZE		256
// Tokenizes input from a file.
class OS_CLEXP C_Tokenizer
{
private:

	char m_default_buff[DEFAULT_TOKENIZER_SIZE];
	
	bool m_bUnget;
	char* m_pBuffer;
	char* m_pOffset;
	CString m_sToken;
	int m_nLine;
	int m_nNumBytes;
	const char* m_pTokenStart;
	const char* m_pTokenEnd;

	struct m_sr_t
	{
		m_sr_t() {}
		bool m_bUnget;
		char* m_pBuffer;
		char* m_pOffset;
		CString m_sToken;
		int m_nLine;
		int m_nNumBytes;
		const char* m_pTokenStart;
		const char* m_pTokenEnd;
	} m_sr;
	
public:
 
	C_Tokenizer();
	C_Tokenizer( const C_Tokenizer& t );
	virtual ~C_Tokenizer();

	bool InitParsing(const char* szScript, int nNumBytes);
	void FreeScript();
	void Skip(int nNum=1);	// Skips n tokens.
	bool GetToken(CString& sToken);
	bool GetFloat(float* f);
	bool GetInt(int* i);
	int GetNumBytes() { return m_nNumBytes; }
	int GetByteOffset() { return m_pOffset-m_pBuffer; }
	int  GetLine() { return m_nLine+1; }
	void UngetToken() {m_bUnget = TRUE; }
	bool FindToken( const char* token );
	bool FindTokeni( const char* token );
	bool IsNextToken( const char* token );
	bool IsNextTokeni( const char* token );
	void RestartParsing();
	void SaveState();
	void RestoreState();

	const char* GetTokenStart();
	const char* GetTokenEnd();
};

// $eb

#endif