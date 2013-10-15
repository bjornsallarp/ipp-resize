
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

#ifndef __TILEMETRIC_H__
#define __TILEMETRIC_H__

#include "auxmath.h"
#include "genalg.h"

UIntRange Intersection(const UIntRange &left, const UIntRange &right)
{
    unsigned int origin = Max(left.Origin(), right.Origin());
    unsigned int bound  = Min(left.Bound(), right.Bound());
    if(bound >= origin) return UIntRange(origin, bound - origin);
    else                return UIntRange(0xFFFFFFFF, 0);
}

Rect Intersection(const Rect &left, const Rect &right)
{
    return Rect(Intersection(left.RangeX(), right.RangeX()), Intersection(left.RangeY(), right.RangeY()));
}


inline Rect TileRect(const Rect &imageRect, const Point &tileGridOrigin, const RectSize &tileSize, unsigned int tileRasterNumber)
{
    unsigned int nOfCols = DivCeil(imageRect.Origin().X() + imageRect.Width() - tileGridOrigin.X(), tileSize.Width());
    unsigned int row     = tileRasterNumber / nOfCols;
    unsigned int col     = tileRasterNumber - row * nOfCols;

    Rect rect(
            tileGridOrigin + RectSize(col, row) * tileSize,
            tileSize
        );

    return Intersection(rect, imageRect);
}

#endif // __TILEMETRIC_H__

