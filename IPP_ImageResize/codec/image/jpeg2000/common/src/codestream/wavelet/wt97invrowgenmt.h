
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2007-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __WT97INVROWGENMT_H__
#define __WT97INVROWGENMT_H__

#include "wt97invrowgenlmt.h"
#include "wt97invrowgenhmt.h"

template<class T>
void WT97InvRow(
    const T         *blockL     ,
    const T         *blockH     ,
          T         *dst        ,
    const UIntRange &blockRange ,
    unsigned int     toBegBorder,
    unsigned int     toEndBorder)
{
    if( ippWTFilterFirstLow == Phase(blockRange.Origin()) )
        return WT97InvRowFirstL(blockL, blockH, dst, blockRange, toBegBorder, toEndBorder);
    else
        throw 0;
        //return WT97InvRowFirstL(blockL, blockH, dst, blockRange, toBegBorder, toEndBorder);
}

#endif // __WT97INVROWGENMT_H__
