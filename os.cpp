///////////////////////////////////////////////////////////////////////////////
// os.cpp
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

#define LOG_MEMORY 		0

void Log_Msg(const char* fmt, ...)
{
}

void Con_LPrintf(const char* fmt, ...)
{
}

OS_FNEXP void OS_OkAlertMessage( const char* title, const char* message )
{
	MessageBox( 0, message, title, MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
}

OS_FNEXP int OS_OkCancelAlertMessage( const char* title, const char* message )
{
	return MessageBox( 0, message, title, MB_TASKMODAL|MB_OKCANCEL|MB_ICONEXCLAMATION );
}

OS_FNEXP void* OS_ResizeAPointer_Debug( void* data, unsigned int size, const char* file, const int line )
{
	void* ptr = DebugResizeAPointer( data, size, file, line );	
	if( ptr == 0 ) OS_BreakMsg("Memory Allocation Failure! file: %s, line %d", file, line );
	return ptr;
}

OS_FNEXP void* OS_AllocAPointer_Debug( unsigned int size, const char* file, const int line )
{
	void* ptr = DebugAllocAPointer( size, file, line );
	
	if( ptr == 0 ) OS_BreakMsg("Memory Allocation Failure! file: %s, line %d", file, line );

#if LOG_MEMORY == 1
	Log_Msg("*** MemoryAllocDebug: %d byte(s) : FILE %s LINE %d ***\n", GetAPointerSize(ptr), file, line );
#endif
	
	return ptr;
}

OS_FNEXP void* OS_AllocAPointerClear_Debug( unsigned int size, const char* file, const int line )
{
	void* ptr = DebugAllocAPointerClear( size, file, line );
	
	if( ptr == 0 ) OS_BreakMsg("Memory Allocation Failure! file: %s, line %d", file, line );

#if LOG_MEMORY == 1
	Log_Msg("*** MemoryAllocDebug: %d byte(s) : FILE %s LINE %d ***\n", GetAPointerSize(ptr), file, line );
#endif

	return ptr;
}

OS_FNEXP void  OS_DeallocAPointer_Debug( void* ptr, const char* file, const int line )
{
	OS_VPTR( ptr );
		
#if LOG_MEMORY == 1
	Log_Msg("*** MemoryFreeDebug: %d byte(s) : FILE %s LINE %d ***\n", GetAPointerSize(ptr), file, line );
#endif
	
	DebugDeallocAPointer( ptr, file, line );
}

OS_FNEXP void* OS_AllocAPointer_Release( unsigned int size, const char* file, const int line )
{
	void* ptr = AllocAPointer( size );
	
	if( ptr == 0 ) OS_BreakMsg("Memory Allocation Failure! file: %s, line %d", file, line );

#if LOG_MEMORY == 1
	Log_Msg("*** MemoryAllocRelease: %d byte(s) : FILE %s LINE %d ***\n", GetAPointerSize(ptr), file, line );
#endif

	return ptr;
}

OS_FNEXP void* OS_AllocAPointerClear_Release( unsigned int size, const char* file, const int line )
{
	void* ptr = AllocAPointerClear( size );
	
	if( ptr == 0 ) OS_BreakMsg("Memory Allocation Failure! file: %s, line %d", file, line );
	
#if LOG_MEMORY == 1
	Log_Msg("*** MemoryAllocRelease: %d byte(s) : FILE %s LINE %d ***\n", GetAPointerSize(ptr), file, line );
#endif
	
	return ptr;
}

OS_FNEXP void  OS_DeallocAPointer_Release( void* ptr, const char* file, const int line )
{
#if LOG_MEMORY == 1
	Log_Msg("*** MemoryAllocRelease: %d byte(s) : FILE %s LINE %d ***\n", GetAPointerSize(ptr), file, line );
#endif
	
	if( ptr )
	{
		DeallocAPointer( ptr );
		return;
	}
}

OS_FNEXP void* OS_ResizeAPointer_Release( void* data, unsigned int size, const char* file, const int line )
{
	
	void* ptr = ResizeAPointer( data, size );
	
	if( ptr == 0 ) OS_BreakMsg("Memory Allocation Failure! file: %s, line %d", file, line );
	
#if LOG_MEMORY == 1
	Log_Msg("*** MemoryAllocRelease: %d byte(s) : FILE %s LINE %d ***\n", GetAPointerSize(ptr), file, line );
#endif
	
	return ptr;
}