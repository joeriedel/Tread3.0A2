///////////////////////////////////////////////////////////////////////////////
// os.h
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

#ifndef OS_H
#define OS_H

#if !defined(__BURGER__)
#define __BURGERNONEW__
#include "Burger.h"
#endif

#if defined(NDEBUG) // burger madness.
#undef _DEBUG
#endif

// $sb "os.h block1"

#ifdef _WIN32
#pragma warning(disable : 4018)		// signed unsigned
#pragma warning(disable : 4244)     // truncate from double to float
#pragma warning(disable : 4237)		// true, false extensions.
#pragma warning(disable : 4800)		// forcing int to bool.
#pragma warning(disable : 4251)     // x needs dll-interface to be used by clients of y.
#pragma warning(disable : 4275)     // non-dll interface class x used as base for y.
#pragma warning(disable : 4996)     // depricated warning
#pragma warning(disable : 4311)     // pointer truncation from 'x *' to 'y'
#pragma warning(disable : 4312)     // 'type cast' : conversion from 'x' to 'y *' of greater size
#endif

#if defined(__BUILDING_TREAD__)
	#define __OPT_EXPORT__
#endif

/*
setup defines for compiler.
*/

#if _MSC_VER

	#define __OPT_VC__
	#define __OPT_MACHINESIZE_32BITS__

	#define OS_EXTERN_C extern "C"
	#define OS_PLUGIN_EXPORT_FN extern "C" __declspec(dllexport)
	
	#if defined(__OPT_EXPORT__)
		#define OS_CLEXP __declspec(dllexport)
		#define OS_FNEXP __declspec(dllexport)
	#else
		#define OS_CLEXP __declspec(dllimport)
		#define OS_FNEXP __declspec(dllimport)
	#endif
	
	#ifdef WIN32
		#define __OPT_INTEL__
		#define __OPT_WINOS__
	#else
		#error "unkown vc target"
	#endif
	
	#if defined(_DEBUG) && !defined(NDEBUG)
		#define __OPT_DEBUG__
	#endif

#else
	
	#error "uknown target/compiler"
	
#endif

#if defined(__OPT_WINOS__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

///////////////////////////////////////////////////////////////////////////////
// These macros let us use new/delete for objects in DLL's.
///////////////////////////////////////////////////////////////////////////////

#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)

#define DECLARE_TREAD_NEW()\
	void* operator new(size_t size);\
	void* operator new[](size_t size);\
	void* operator new(size_t size, LPCSTR lpszFileName, int nLine);\
	void* operator new[](size_t size, LPCSTR lpszFileName, int nLine);\
	void operator delete(void *p);\
	void operator delete[](void *p);\
	void operator delete(void *p, LPCSTR lpszFileName, int nLine);\
	void operator delete[](void *p, LPCSTR lpszFileName, int nLine)

#define IMPLEMENT_TREAD_NEW(_class)\
	void* _class::operator new(size_t size)\
	{\
		return ::malloc(size);\
	}\
	void* _class::operator new[](size_t size)\
	{\
		return ::malloc(size);\
	}\
	void* _class::operator new(size_t size, LPCSTR lpszFileName, int nLine)\
	{\
		return ::AfxAllocMemoryDebug(size, FALSE, lpszFileName, nLine);\
	}\
	void* _class::operator new[](size_t size, LPCSTR lpszFileName, int nLine)\
	{\
		return ::AfxAllocMemoryDebug(size, FALSE, lpszFileName, nLine);\
	}\
	void _class::operator delete(void *p)\
	{\
		if (p) ::free(p);\
	}\
	void _class::operator delete[](void *p)\
	{\
		if (p) ::free(p);\
	}\
	void _class::operator delete(void *p, LPCSTR lpszFileName, int nLine)\
	{\
		::AfxFreeMemoryDebug(p, FALSE);\
	}\
	void _class::operator delete[](void *p, LPCSTR lpszFileName, int nLine)\
	{\
		::AfxFreeMemoryDebug(p, FALSE);\
	}
#else

#define DECLARE_TREAD_NEW()\
	void* operator new(size_t size);\
	void* operator new[](size_t size);\
	void operator delete(void *p);\
	void operator delete[](void *p);\

#define IMPLEMENT_TREAD_NEW(_class)\
	void* _class::operator new(size_t size)\
	{\
		return ::malloc(size);\
	}\
	void* _class::operator new[](size_t size)\
	{\
		return ::malloc(size);\
	}\
	void _class::operator delete(void *p)\
	{\
		if (p) ::free(p);\
	}\
	void _class::operator delete[](void *p)\
	{\
		if (p) ::free(p);\
	}

#endif
#endif

#if defined(__OPT_MACOS__)
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#endif

#define MEMFAIL( a ) if( !(a) ) OS_BreakMsg( "Memory Allocation Failure: expr \"%s\", file %s, line %d\n", #a, __FILE__, __LINE__ )
#define FAIL( a ) if( !(a) ) OS_BreakMsg( "CHECK FAILED EXPR: \"%s\", file: %s, line: %d\n", #a, __FILE__, __LINE__ )
#define FAILMSG( a, b ) if( !(a) ) OS_BreakMsg("CHECK FAILED MSG: \"%s\", file: %s, line: %d\n", b, __FILE__, __LINE__ )
#define CHKSTRLEN( a, b, c ) if( FastStrLen( a ) > b ) OS_BreakMsg("The string '%s' exceeds the maximum size of %d characters for a %s. file: %s, line: %d\n", a, b, c, __FILE__, __LINE__)

/*
** the system uses OS_VPTR() to validate pointers in debug mode.
** this can be a costly operation, depending on the number of allocated
** blocks. setting this level to 2 does a full check on the pointer to
** check if it is currently in the allocated pool. setting this to 1 does
** a simple non-null check.
**
** setting this to 0 disables the pointer checks completely.
**
** the OS_VPTR() macro is only used in debug builds.
*/
#if defined(__OPT_DEBUG__)
	#define __OPT_VPTR_LEVEL__	1
	#define OS_ASSERT( f )	if( !(f) ) OS_BreakMsg( "ASSERT FAILED EXPR: \"%s\", file: %s, line: %d\n", #f, __FILE__, __LINE__ )
	#if __OPT_VPTR_LEVEL__ == 2
		#define OS_VPTR( p ) if(DebugMemoryIsPointerValid((void*)(p))==FALSE) {OS_BreakMsg( "Bad Pointer: \"%s\" == 0x%X, file: %s, line: %d\n", #p, p, __FILE__, __LINE__);} (void)0
	#elif __OPT_VPTR_LEVEL__ == 1
		#define OS_VPTR OS_ASSERT
	#else
		#define OS_VPTR(p)
	#endif
	#if defined(__OPT_WINOS__)
		#if defined(__OPT_INTEL__) && (defined(__OPT_MW__) || defined(__OPT_VC__))
			#define OS_EnterDebugger() __asm {int 3}
		#else
			#error "unkown machine/compiler!, unable to define OS_EnterDebugger()"
		#endif
	#elif defined(__OPT_MACOS__)
		#define OS_EnterDebugger() Debugger()
	#else
		#error "unknown os target!"
	#endif
#else
	#define __OPT_VPTR_LEVEL__	0
	#define OS_ASSERT( f )
	#define OS_EnterDebugger()
	#define OS_VPTR( p )
#endif

#define OS_STATIC_ASSERT_HELPER(x, y, z) static int OS_STRINGIZE(y,z)[(x) ? 1 : 0]
#define OS_STRINGIZE(x, y) x##y
#define OS_STATIC_ASSERT(x) OS_STATIC_ASSERT_HELPER(x, __static_assert_, __LINE__)

#define OS_LOWBYTE( a ) ( a&0xff );
#define OS_HIGHBYTE( a ) ( (a&0xff00)>>8 );
#define OS_LOWWORD( a ) ( a&0xffff );
#define OS_HIGHWORD( a ) ( (a>>16)&0xffff );

#define CLAMP_VAL(a, low, high) ( ((a)<(low))?(low):((a)>(high))?(high):(a) )
#define MAX_VAL(a, max) ( (a)>(max)?(a):(max) )
#define MIN_VAL(a, min) ( (a)<(min)?(a):(min) )

/*
Memory allocation
*/
#if defined(__OPT_DEBUG__)
#define OS_AllocAPointer(x) OS_AllocAPointer_Debug( (x), __FILE__, __LINE__ )
#define OS_AllocAPointerClear(x) OS_AllocAPointerClear_Debug( (x), __FILE__, __LINE__ )
#define OS_ResizeAPointer(x,y) OS_ResizeAPointer_Debug( (x), (y), __FILE__, __LINE__ )
#define OS_DeallocAPointer(x) OS_DeallocAPointer_Debug( (x), __FILE__, __LINE__ )
#else
#define OS_AllocAPointer(x) OS_AllocAPointer_Release( (x), __FILE__, __LINE__ )
#define OS_AllocAPointerClear(x) OS_AllocAPointerClear_Release( (x), __FILE__, __LINE__ )
#define OS_ResizeAPointer(x,y) OS_ResizeAPointer_Release( (x), (y), __FILE__, __LINE__ )
#define OS_DeallocAPointer(x) OS_DeallocAPointer_Release( (x), __FILE__, __LINE__ )
#endif

// $eb

OS_FNEXP void* OS_AllocAPointer_Debug( unsigned int size, const char* file, const int line );
OS_FNEXP void* OS_AllocAPointerClear_Debug( unsigned int size, const char* file, const int line );
OS_FNEXP void* OS_ResizeAPointer_Debug( void* data, unsigned int size, const char* file, const int line );
OS_FNEXP void  OS_DeallocAPointer_Debug( void* ptr, const char* file, const int line );

OS_FNEXP void* OS_AllocAPointer_Release( unsigned int size, const char* file, const int line );
OS_FNEXP void* OS_AllocAPointerClear_Release( unsigned int size, const char* file, const int line );
OS_FNEXP void* OS_ResizeAPointer_Release( void* data, unsigned int size, const char* file, const int line );
OS_FNEXP void  OS_DeallocAPointer_Release( void* ptr, const char* file, const int line );

//
// change this to define how much memory the game will be allowed to use.
//

OS_FNEXP void OS_OkAlertMessage( const char* title, const char* message );
OS_FNEXP int OS_OkCancelAlertMessage( const char* title, const char* message );

OS_FNEXP void OS_Break();
OS_FNEXP void OS_BreakMsg( const char* fmt, ... );
OS_FNEXP void OS_Exit();
OS_FNEXP void OS_ExitMsg( const char* fmt, ... );

#endif