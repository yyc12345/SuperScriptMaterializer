// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma once

// Check build macros
#if !defined(MATERIALIZER_PLUGIN) && !defined(MATERIALIZER_STANDALONE)
#error "You must define one of MATERIALIZER_PLUGIN or MATERIALIZER_STANDALONE to represent the build type of materializer!"
#endif
#if !defined(MATERIALIZER_DEBUG) && !defined(MATERIALIZER_RELEASE)
#error "You must define one of MATERIALIZER_DEBUG or MATERIALIZER_RELEASE!"
#endif
#if !defined(VIRTOOLS_50) && !defined(VIRTOOLS_40) && !defined(VIRTOOLS_35) && !defined(VIRTOOLS_30) && !defined(VIRTOOLS_25) && !defined(VIRTOOLS_21)
#error "Lost essential Virtools version macro!"
#endif

#pragma region Windows and Virtools
#include <WinImportPrefix.hpp>

#pragma region Windows Headers

#include <SDKDDKVer.h>

// Only plugin mode involve MFC headers
#if defined(MATERIALIZER_PLUGIN)
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

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#endif

#pragma endregion

#pragma region Virtools Headers

#include "CKAll.h"

// Only plugi mode need include extra headers
#if defined(MATERIALIZER_PLUGIN)
#include "VIControls.h"
#include "CKControlsAll.h"
#include "VEP_ScriptActionMenu.h"
#include "VEP_KeyboardShortcutManager.h"
#include "VEP_All.h"
#endif

#pragma endregion

#include <WinImportSuffix.hpp>
#pragma endregion

#pragma region YYCC Headers
#include <YYCCommonplace.hpp>
#pragma endregion

