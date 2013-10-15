
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2003-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/


#include "dsubband.h"
#include "quant.h"
#include "wtmetric.h"

inline unsigned int DynRangeGain(IppiWTSubband type)
{
    switch(type)
    {
        case ippWTSubbandHxLy: return DYN_RANGE_GAIN_HxLy;
        case ippWTSubbandLxHy: return DYN_RANGE_GAIN_LxHy;
        case ippWTSubbandHxHy: return DYN_RANGE_GAIN_HxHy;
        default:               return DYN_RANGE_GAIN_LxLy;
    }
}

inline Rect Scale(const Rect rectR, IppiWTSubband type)
{
    switch(type)
    {
        case ippWTSubbandHxLy: return ScaleHxLy(rectR);
        case ippWTSubbandLxHy: return ScaleLxHy(rectR);
        case ippWTSubbandHxHy: return ScaleHxHy(rectR);
        default:               return rectR;
    }
}

void DSubband::Init(
    const ImageCore32sC1 &subband,
    IppiWTSubband         type,
    const Rect           &rectR,
    const RectSize       &precStepOrder,
    const RectSize       &cbStepOrder,
    const CBCodingStyle  &codingStyle,
    unsigned int          nOfLayers,
    unsigned int          cmpGuardBits,
    unsigned int          bitDepth,
    int                   quantExp)
{
    unsigned int dynRange        = bitDepth + 1 + DynRangeGain(type);
    unsigned int lowestBitOffset = (int)quantExp - dynRange;


    GridPow2 prGrid;
    // we allocate memory only for non-empty precincts
    prGrid.Init(rectR, precStepOrder);
    m_precincts.ReAlloc(prGrid.NOfRows(), prGrid.NOfCols());

    GridPow2RelativeIterator ii(prGrid);

    Rect sbRect = Scale(rectR, type);

    m_isEmpty = sbRect.Width() == 0 || sbRect.Height() == 0;

    for(; ii.IsInBounds(); ++ii)
    {
        Rect prcRect(ii->Origin() + rectR.Origin(), ii->Size());
        Rect rect = Scale(prcRect, type);

        if(precStepOrder.Width() == 0)
        {
            if(ii.Row() == 0)
                rect.SetSize(RectSize(sbRect.Width(), rect.Height()));
            else
                rect.SetSize(RectSize(0, rect.Height()));
        }
        if(precStepOrder.Height() == 0)
        {
            if(ii.Col() == 0)
                rect.SetSize(RectSize(rect.Width(), sbRect.Height()));
            else
                rect.SetSize(RectSize(rect.Width(), 0));
        }

        ImageCore32sC1 prcImg  = subband.SubImage(rect.Origin() - sbRect.Origin());

        m_precincts[ii.Row()][ii.Col()].Init(
            prcImg,
            rect,
            cbStepOrder,
            nOfLayers,
            type,
            cmpGuardBits,
            dynRange,
            lowestBitOffset,
            codingStyle);
    }
}

