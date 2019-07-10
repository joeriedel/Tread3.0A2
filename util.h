///////////////////////////////////////////////////////////////////////////////
// util.h
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

#ifndef UTIL_H
#define UTIL_H

#include "os.h"

CString Util_GetTokenValue( const char* string, const char* token);
CString Util_GetTokenValuei( const char* string, const char* token);
bool Util_FindToken( const char* string, const char* token );
bool Util_FindTokeni( const char* string, const char* token );
bool Util_GetLineAndStringFromPos( const char* script, int script_len, int char_num, char* outp, int* line );
void Util_ReplaceChar( char* string, char src, char dst );
void Util_MakePlatformAgnosticText( char* text, int len=-1 );
void Util_SwapTimeDate( TimeDate_t* td );

//
// DON'T CALL Util_SwapTimeDate() on these!
//
bool Util_WriteTimeDate( FILE* fp, TimeDate_t* td );
bool Util_ReadTimeDate( FILE* fp, TimeDate_t* td );

LongWord Util_PowerOf2( LongWord num );

#endif