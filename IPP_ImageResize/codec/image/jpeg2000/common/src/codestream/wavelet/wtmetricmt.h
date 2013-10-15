
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

#ifndef __WTMETRICMT_H__
#define __WTMETRICMT_H__

#include "wtmetric.h"
#include "genalg.h"

static const unsigned int WT97MaxBegOverstepL = 2;
static const unsigned int WT97MaxBegOverstepH = 2;
static const unsigned int WT97MaxEndOverstepL = 2;
static const unsigned int WT97MaxEndOverstepH = 2;


inline unsigned int WT97BegOverstepL(
    const UIntRange &blockRange ,
    unsigned int     toBegBorder,
    unsigned int     toEndBorder)
{
    UIntRange allRange(
        blockRange.Origin() - toBegBorder,
        blockRange.Size  () + toBegBorder + toEndBorder);

    UIntRange allRangeL   = ScaleL(allRange);
    UIntRange blockRangeL = ScaleL(blockRange);

    unsigned int toBegBorderL = blockRangeL.Origin() - allRangeL.Origin();

    if(blockRange.Origin() & 1)
        return Min(toBegBorderL, 2u);
    else
        return Min(toBegBorderL, 1u);
}

inline unsigned int WT97EndOverstepL(
    const UIntRange &blockRange ,
    unsigned int     toBegBorder,
    unsigned int     toEndBorder)
{
    UIntRange allRange(
        blockRange.Origin() - toBegBorder,
        blockRange.Size  () + toBegBorder + toEndBorder);

    UIntRange allRangeL   = ScaleL(allRange);
    UIntRange blockRangeL = ScaleL(blockRange);

    unsigned int toEndBorderL = (blockRangeL.Origin() + blockRangeL.Size() - 1) - (allRangeL.Origin() + allRangeL.Size() - 1);

    if(blockRange.Origin() & 1)
    {
        if(blockRange.Size() & 1)
            return Min(toEndBorderL, 2u);
        else
            return Min(toEndBorderL, 1u);

    }
    else
    {
        if(blockRange.Size() & 1)
            return Min(toEndBorderL, 1u);
        else
            return Min(toEndBorderL, 2u);
    }
}

inline unsigned int WT97BegOverstepH(
    const UIntRange &blockRange ,
    unsigned int     toBegBorder,
    unsigned int     toEndBorder)
{
    UIntRange allRange(
        blockRange.Origin() - toBegBorder,
        blockRange.Size  () + toBegBorder + toEndBorder);

    UIntRange allRangeH   = ScaleH(allRange);
    UIntRange blockRangeH = ScaleH(blockRange);

    unsigned int toBegBorderH = blockRangeH.Origin() - allRangeH.Origin();

    return Min(toBegBorderH, 2u);
}

inline unsigned int WT97EndOverstepH(
    const UIntRange &blockRange ,
    unsigned int     toBegBorder,
    unsigned int     toEndBorder)
{
    UIntRange allRange(
        blockRange.Origin() - toBegBorder,
        blockRange.Size  () + toBegBorder + toEndBorder);

    UIntRange allRangeH   = ScaleH(allRange);
    UIntRange blockRangeH = ScaleH(blockRange);

    unsigned int toEndBorderH = (blockRangeH.Origin() + blockRangeH.Size() - 1) - (allRangeH.Origin() + allRangeH.Size() - 1);

    return Min(toEndBorderH, 2u);
}

#endif // __WTMETRICMT_H__
