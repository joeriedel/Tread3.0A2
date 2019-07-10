///////////////////////////////////////////////////////////////////////////////
// util.cpp
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
#include "os.h"
#include "c_tokenizer.h"
#include "util.h"
#include "files.h"

LongWord Util_PowerOf2(LongWord Input)
{
	if (Input<=0x10000) {				/* Which half? */
		if (Input<=0x100) {				/* Half again */
			if (Input<=0x10) {
				if (Input<=4) {			/* 4/0-2 */
					if (Input<=2) {
						return Input;	/* Return 0,1,2 */
					}
					return 4;
				}
				if (Input<=8) {			/* 8/0x10 */
					return 8;
				}
				return 0x10;
			}
			if (Input<=0x40) {
				if (Input<=0x20) {		/* 0x20/0x40 */
					return 0x20;
				}
				return 0x40;
			}
			if (Input<=0x80) {			/* 0x80/0x100 */
				return 0x80;
			}
			return 0x100;
		}
		if (Input<=0x1000) {
			if (Input<=0x400) {			/* 0x200/0x400 */
				if (Input<=0x200) {
					return 0x200;
				}
				return 0x400;
			}
			if (Input<=0x800) {			/* 0x800/0x1000 */
				return 0x800;
			}
			return 0x1000;
		}
		if (Input<=0x4000) {
			if (Input<=0x2000) {		/* 0x2000/0x4000 */
				return 0x2000;
			}
			return 0x4000;
		}
		if (Input<=0x8000) {			/* 0x8000/0x10000 */
			return 0x8000;
		}
		return 0x10000;
	}
	if (Input<=0x1000000) {
		if (Input<=0x100000) {
			if (Input<=0x40000) {		/* 0x20000/0x40000 */
				if (Input<=0x20000) {
					return 0x20000;
				}
				return 0x40000;
			}
			if (Input<=0x80000) {		/* 0x80000/0x100000 */
				return 0x80000;
			}
			return 0x100000;
		}
		if (Input<=0x400000) {
			if (Input<=0x200000) {		/* 0x200000/0x400000 */
				return 0x200000;
			}
			return 0x400000;
		}
		if (Input<=0x800000) {			/* 0x800000/0x1000000 */
			return 0x800000;
		}
		return 0x1000000;
	}
	if (Input<=0x10000000) {
		if (Input<=0x4000000) {			/* 0x2000000/0x4000000 */
			if (Input<=0x2000000) {
				return 0x2000000;
			}
			return 0x4000000;
		}
		if (Input<=0x8000000) {			/* 0x8000000/0x10000000 */
			return 0x8000000;
		}
		return 0x10000000;
	}
	if (Input<=0x40000000) {
		if (Input<=0x20000000) {		/* 0x20000000/0x40000000 */
			return 0x20000000;
		}
		return 0x40000000;
	}
	return 0x80000000U;					/* For everything else... */
}

void Util_SwapTimeDate( TimeDate_t* td )
{
	td->Year = LoadIntelShort( td->Year );
	td->Milliseconds = LoadIntelShort( td->Milliseconds );
}

bool Util_WriteTimeDate( FILE* fp, TimeDate_t* ptd )
{
	TimeDate_t td = *ptd;
	Util_SwapTimeDate( &td );
	
	return FS_fwrite( fp, &td, sizeof(TimeDate_t) );
}

bool Util_ReadTimeDate( FILE* fp, TimeDate_t* ptd )
{	
	TimeDate_t td;
	
	if( FS_fread( fp, &td, sizeof(TimeDate_t) ) )
		return TRUE;
		
	Util_SwapTimeDate( &td );
	*ptd = td;
	return FALSE;
}

//
// replaces lone '\r' as '\n'
// replaces '\r''\n' as '\n'
//
#define FILLER_CHAR	1
void Util_MakePlatformAgnosticText( char* text, int len )
{
	int i;
	
	if( len == -1 )
		len = strlen( text );
		
	for(i = 0; i < len; i++)
	{
		if( text[i] == '\r' )
		{
			//
			// ahem.
			//
			if( i+1 < len )
			{
				if( text[i+1] == '\n' || text[i+1] == '\r' )
				{
					text[i] = FILLER_CHAR;
					continue;
				}
			}
						
			text[i] = '\n';
		}
	}
}

bool Util_GetLineAndStringFromPos( const char* instring, int len, int errPos, char* outp, int* line )
{
	int nlines = 1;
	int nchar  = 1;
	int i;
	
	for ( i = 0; i < errPos; i++ )
	{
	  if ( instring[i] == '\n' )
	  {
	      nlines++;
	      nchar = 1;
	  }
	  else
	  {
	      nchar++;
	  }
	}
	int start;
	int end;
	int flag = ((instring[errPos]==';') | (instring[errPos-1]==';')) ? 1 : 0;
	for ( i = errPos; i >= 0; i-- )
	{
	  start = i;
	  if ( flag && (start >= errPos-1)  )
	      continue;
	  if ( instring[i] == ';' )
	  {
	      if ( !flag )
	      {
	          start = i+1;
	          if ( instring[start] == '\n' )
	              start++;
	      }
	      break;
	  }
	}
	for ( i = errPos; i < len; i++ )
	{
	  end = i;
	  if ( instring[i] == ';' && end > start)
	  {
	      break;
	  }
	}
	if ( errPos - start > 30 )
	{
	  start = errPos - 30;
	}
	if ( end - errPos > 30 )
	{
	  end = errPos + 30;
	}

	strncpy( outp, &(instring[start]), end-start+1 );
	outp[end-start+1] = 0;
	
	*line = nlines;
	
	return FALSE;
}

bool Util_FindToken( const char* string, const char* token )
{
	OS_ASSERT( string );
	int len = strlen(string);
	
	if( len < 1 )
		return FALSE;
	
	C_Tokenizer tn;
	tn.InitParsing( string, len );
	if( !tn.FindToken( token ) )
		return FALSE;
	
	return TRUE;	
}

bool Util_FindTokeni( const char* string, const char* token )
{
	OS_ASSERT( string );
	int len = strlen(string);
	
	if( len < 1 )
		return FALSE;
	
	C_Tokenizer tn;
	tn.InitParsing( string, len );
	if( !tn.FindTokeni( token ) )
		return FALSE;
	
	return TRUE;	
}


CString Util_GetTokenValue( const char* string, const char* token )
{
	OS_ASSERT( string );
	int len = strlen(string);
	
	if( len < 1 )
		return "";
	
	C_Tokenizer tn;
	tn.InitParsing( string, len );
	if( !tn.FindToken( token ) )
		return "";
	
	CString s;
	if( !tn.GetToken(s) )
		return "";
	
	return s;
}

CString Util_GetTokenValuei( const char* string, const char* token )
{
	OS_ASSERT( string );
	int len = strlen(string);
	
	if( len < 1 )
		return "";
	
	C_Tokenizer tn;
	tn.InitParsing( string, len );
	if( !tn.FindTokeni( token ) )
		return "";
	
	CString s;
	if( !tn.GetToken(s) )
		return "";
	
	return s;
}

void Util_ReplaceChar( char* string, char src, char dst )
{
	if( !string || !string[0] )
		return;
		
	int len = strlen(string);
	int i;
	
	for(i = 0; i < len; i++)
	{
		if( string[i] == src )
			string[i] = dst;
	}
}