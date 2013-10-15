/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2011-2012 Intel Corporation. All Rights Reserved.
//
*/
#ifdef WIN32
#include <windows.h>
#include "ippdefs.h"
#include "ippcore.h"

#if defined (UIC_W32DLL)
DWORD WINAPI DllMain(HINSTANCE,ULONG fdwReason,PVOID)
{
  if(DLL_PROCESS_ATTACH == fdwReason)
  {
    ippStaticInit();
  }

  return TRUE;
} // DllMain()
#endif
#endif
