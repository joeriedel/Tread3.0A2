//
// Copyright © 1998 Kirk Stowell ( kstowel@sprynet.com )
// www.geocities.com/SiliconValley/Haven/8230/index.html
//
// You are free to use, modify and distribute this source, as long as
// there is no charge, and this HEADER stays intact. This source is
// supplied "AS-IS", without WARRANTY OF ANY KIND, and the user
// holds Kirk Stowell blameless for any or all problems that may arise
// from the use of this code.
//
//////////////////////////////////////////////////////////////////////

#if !defined(CJ60LIB_H_INCLUDED)
#define CJ60LIB_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

#define MFCXLIB_STATIC

#if 0
#if defined(MFCXLIB_STATIC) 
  #if defined (_DEBUG) 
    #pragma comment(lib,"CJ60StaticLibd.lib") 
    #pragma message("Automatically linking with CJ60StaticLibd.lib") 
  #else 
    #pragma comment(lib,"CJ60StaticLib.lib") 
    #pragma message("Automatically linking with CJ60StaticLib.lib") 
  #endif 
#elif defined(_DEBUG) 
  #pragma comment(lib,"CJ60Libd.lib") 
  #pragma message("Automatically linking with CJ60Libd.dll") 
#else 
  #pragma comment(lib,"CJ60Lib.lib") 
  #pragma message("Automatically linking with CJ60Lib.dll") 
#endif 
#endif

#if defined(MFCXLIB_STATIC) 
  #define CLASS_EXPORT 
#elif defined(MFCX_PROJ) 
  #define CLASS_EXPORT AFX_CLASS_IMPORT 
#else 
  #define CLASS_EXPORT AFX_CLASS_EXPORT 
#endif 

#include "CJButton.h"
#include "CJComboBox.h"
#include "CJControlBar.h"
#include "CJTabCtrlBar.h"
#include "CJDockBar.h"
#include "CJFrameWnd.h"
#include "CJMDIFrameWnd.h"
#include "CJOutlookBar.h"
#include "CJPagerCtrl.h"
#include "CJToolBar.h"
#include "CoolMenu.h"
#include "HyperLink.h"
#include "Subclass.h"

#if defined(AUTO_SUBCLASS)
#define CControlBar		CCJControlBar
#define CTabCtrlBar		CCJTabCtrlBar
#define CDockBar		CCJDockBar
#define CFrameWnd		CCJFrameWnd
#define CMDIFrameWnd	CCJMDIFrameWnd
#define COutlookBar		CCJOutlookBar
#define CPagerCtrl		CCJPagerCtrl
#define CToolBar		CCJToolBar
#endif

#endif // CJ60LIB_H_INCLUDED