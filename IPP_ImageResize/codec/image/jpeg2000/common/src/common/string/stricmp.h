
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2002-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __STRICMP_H__
#define __STRICMP_H__

#include <ctype.h>
#include <stdlib.h>

inline int Stricmp(const char *b, const char *a)
{
    for(;;a++, b++)
    {
        if(!(*a != '\0' && *b != '\0' && tolower(*a) == tolower(*b)))
            break;
    }
    return *a == *b ? 0 : (tolower(*a) < tolower(*b) ? -1 : 1);
}

#endif // __STRICMP_H__
