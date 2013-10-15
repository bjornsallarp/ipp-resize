
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

#ifndef __DECIMETRIC_H__
#define __DECIMETRIC_H__

#include "auxmath.h"
#include "geometry2d.h"

inline unsigned int DecimateSize (unsigned int    size,  unsigned int    sampleSize) { return DivCeil(size, sampleSize); }
inline RectSize     DecimateSize (const RectSize &size,  const RectSize &sampleSize) { return RectSize(DecimateSize (size.Width(),  sampleSize.Width()), DecimateSize(size.Height(), sampleSize.Height())); }
inline Point        DecimatePoint(const Point    &point, const RectSize &sampleSize) { return Point   (DecimateSize (point.X(),     sampleSize.Width()), DecimateSize(point.Y(),     sampleSize.Height())); }
inline Rect         DecimateRect (const Rect     &rect,  const RectSize &sampleSize) { return Rect    (DecimatePoint(rect.Origin(), sampleSize),         DecimateSize(rect.Size(), sampleSize)); }

#endif // __DECIMETRIC_H__
