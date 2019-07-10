// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__75A839E8_589B_11D2_AB9F_C40300C10000__INCLUDED_)
#define AFX_STDAFX_H__75A839E8_589B_11D2_AB9F_C40300C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0403)
	#undef _WIN32_WINNT
	#define _WIN32_WINNT 0x0403
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#if _MSC_VER >= 1200 // VC6
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#if defined(MFCXLIB_STATIC) // CJ60Lib 
  #define CLASS_EXPORT 
#elif defined(MFCX_PROJ) 
  #define CLASS_EXPORT AFX_CLASS_IMPORT 
#else 
  #define CLASS_EXPORT AFX_CLASS_EXPORT 
#endif 

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__75A839E8_589B_11D2_AB9F_C40300C10000__INCLUDED_)
