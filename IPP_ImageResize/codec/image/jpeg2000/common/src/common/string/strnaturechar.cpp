
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __STRNATURECHAR_H__
#define __STRNATURECHAR_H__

#include "strnature.h"

template <>
const char  StrNature<char>::m_terminator = '\0';

template <>
const char *StrNature<char>::m_digit      = "0123456789";

#endif // __STRNATURECHAR_H__
