
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

#ifndef __STRNATURE_H__
#define __STRNATURE_H__

// NOTE: you should define template constants for your CHARTYPE
template<typename CHARTYPE>
class StrNature
{
public:
    static const CHARTYPE  m_terminator;
    static const CHARTYPE *m_digit;
};


#endif // __STRNATURE_H__
