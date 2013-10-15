
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

#ifndef __STRALG_H__
#define __STRALG_H__

#include "strnature.h"

template<class CHARTYPE>
unsigned int StrSize(const CHARTYPE *str)
{
    unsigned int size = 0;

    while(*str != StrNature<CHARTYPE>::m_terminator)
    {
        str++;
        size++;
    }

    return size;
}

template<class CHARTYPE>
void StrCopy(const CHARTYPE *src, CHARTYPE *dst)
{
    unsigned int size = 0;

    while(*src != StrNature<CHARTYPE>::m_terminator)
    {
        *dst = *src;
        src++;
        dst++;
    }
    *dst = StrNature<CHARTYPE>::m_terminator;
}

template<class CHARTYPE>
void StrCopy(const CHARTYPE *src, CHARTYPE *dst, unsigned int size)
{
    const CHARTYPE *src0 = src;
    for(;;)
    {
        if( (unsigned int)(src - src0) >= size )       break;
        if(*src  == StrNature<CHARTYPE>::m_terminator) break;
        *dst = *src;
        src++;
        dst++;
    }
    *dst = StrNature<CHARTYPE>::m_terminator;
}

static const unsigned int INT_TO_STR_MAX_SIZE = 21;

template<class CHARTYPE>
unsigned int IntToStrR10(unsigned int num, CHARTYPE *dst)
{
    unsigned int charPos = 0;

    do
    {
        unsigned int numNext = num / 10u;
        unsigned int mod     = num - 10u * numNext;

        num = numNext;

        dst[charPos] = StrNature<CHARTYPE>::m_digit[mod];
        charPos++;
    }
    while(num);

    dst[charPos] = StrNature<CHARTYPE>::m_terminator;
    Reverse(dst, charPos);
    return charPos;
}


#endif // __STRALG_H__

