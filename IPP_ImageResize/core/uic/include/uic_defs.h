/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_DEFS_H__
#define __UIC_DEFS_H__

#include "ippdefs.h"

namespace UIC {


#if !defined( UIC_DLL_EXPORTS )

  #if defined (UIC_W32DLL) && (defined( _WIN32 ) || defined( _WIN64 ))
    #if defined( _MSC_VER ) || defined( __ICL )
      #define UICAPI __declspec(dllimport)
    #else
      #define UICAPI
    #endif
  #else
    #define   UICAPI
  #endif

#else

  #if defined (UIC_W32DLL) && (defined( _WIN32 ) || defined( _WIN64 ))
    #if defined( _MSC_VER ) || defined( __ICL )
      #define UICAPI __declspec(dllexport)
    #else
      #define UICAPI
    #endif
  #else
    #define   UICAPI
  #endif

#endif

} // namespace UIC

#endif // __UIC_DEFS_H__
