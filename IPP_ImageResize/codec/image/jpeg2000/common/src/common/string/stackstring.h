
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

#ifndef __STACKSTRING_H__
#define __STACKSTRING_H__

#include "stralg.h"

template<unsigned int SIZE, typename CHARTYPE>
class StackString
{
public:
    StackString()                    { m_data[0] = 0; }
    StackString(const CHARTYPE *src) { StrCopy(src, m_data, SIZE); }

    StackString& operator=(const CHARTYPE *src)
    {
        if(m_data != src) StrCopy(src, m_data, SIZE);
        return *this;
    }

    operator const char *() const { return m_data; }
    operator       char *()       { return m_data; }

protected:
    CHARTYPE m_data[SIZE+1];
};

#endif // __STACKSTRING_H__

