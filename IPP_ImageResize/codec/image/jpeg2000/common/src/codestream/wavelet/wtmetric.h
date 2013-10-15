
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

#ifndef __WTMETRIC_H__
#define __WTMETRIC_H__

//
//
//  GENERAL TERMS
//
//

//
// 'Resolution level' - this term is defined by standard.
//      It has relation with wavelet transforms subbands
//      with following exception: JPEG 2000 standard
//      allows coding without wavelet transforms at all
//      and this is the case of single resolution level
//      (i.e. image data itself without any wavelet decomposition).
//  The zero resolution level corresponds the most
//      coarse resolution in the 'space-scale' meaning.
//
//  This term syntactically expressed in the program code as
//      'resLevel', 'ResLevel' and so on.
//
//

//
//  'Subband depth' - is term defined in this
//      implementation scope for this implementation purposes.
//      It is used to iterate the resolution levels
//      except one that contains only LxLy subband.
//  The most coarse resolution level is distinct from the other,
//      because it may contain only single subband (LxLy)
//      instead of triplet (LxHy, HxLy, HxHy),
//      so in some cases it is transparent and effective to use
//      special zero-based iteration for 'triplet' resolution levels.
//
//  Note: 'subband depth' has reverse direction in comparision
//      with 'resolution level'.
//
//  This term syntactically expressed in the program code as
//      'sbDepth', 'SBDepth' and so on.
//

//
// 'Wavelet transform level' - is term defined in this
//      implementation scope for this implementation purposes.
//      It can be used as equalent of 'number of wavelet components' term,
//      and somtimes it can be used for iteration of resolution levels
//      as alternative for 'subband depth' and 'resolution level'.
//      It has the same direction in iteration as 'subband depth',
//      but the value of 'wavelet transform level' is greater then
//      'subband depth' by 1.
//  So in difference of 'subband depth' the iteration with 'wavelet transform level'
//      can easily have a deal with situation when no wavelet transform applied.
//
//  This term syntactically expressed in the program code as
//      'wtLevel', 'WTLevel' and so on.
//
//


#include "auxmath.h"
#include "geometry2d.h"


inline Point RShiftFloor(const Point &point, unsigned int order)
{
    return Point(RShiftFloor(point.X(), order), RShiftFloor(point.Y(), order));
}

inline Point RShiftCeil(const Point &point, unsigned int order)
{
    return Point(RShiftCeil(point.X(), order), RShiftCeil(point.Y(), order));
}


// resolution level scale
inline unsigned int ScaleR(unsigned int compCoor, unsigned int resLevel, unsigned int nOfResLevels)
{
    return RShiftCeil(compCoor, nOfResLevels - resLevel - 1);
}

inline UIntRange ScaleR(const UIntRange &src, unsigned int resLevel, unsigned int nOfResLevels)
{
    unsigned int origin = ScaleR(src.Origin(), resLevel, nOfResLevels);
    unsigned int bound  = ScaleR(src.Bound (), resLevel, nOfResLevels);
    unsigned int size   = bound - origin;
    return UIntRange(origin, size);
}

inline Rect ScaleR(const Rect &src, unsigned int resLevel, unsigned int nOfResLevels)
{
    return Rect(
        ScaleR(src.RangeX(), resLevel, nOfResLevels),
        ScaleR(src.RangeY(), resLevel, nOfResLevels));
}

inline Point ScaleR(const Point &src, unsigned int resLevel, unsigned int nOfResLevels)
{
    return Point(
        ScaleR(src.X(), resLevel, nOfResLevels),
        ScaleR(src.Y(), resLevel, nOfResLevels));
}


inline unsigned int NOfPrecincts(const UIntRange &comp, unsigned int resLevel, unsigned int nOfResLevels, unsigned int precStepOrder)
{
    UIntRange resScale = ScaleR(comp, resLevel, nOfResLevels);
    if(resScale.Size() != 0) return RShiftCeil(resScale.Bound(), precStepOrder) - RShiftFloor(resScale.Origin(), precStepOrder);
    return 0;
}

// subband scales

inline unsigned int ScaleL(unsigned int src) { return (src + 1) >> 1; }
inline unsigned int ScaleH(unsigned int src) { return  src      >> 1; }

inline UIntRange ScaleL(const UIntRange &src)
{
    unsigned int origin = ScaleL(src.Origin());
    unsigned int bound  = ScaleL(src.Bound());
    unsigned int size   = bound - origin;
    return UIntRange(origin, size);
}

inline UIntRange ScaleH(const UIntRange &src)
{
    unsigned int origin = ScaleH(src.Origin());
    unsigned int bound  = ScaleH(src.Bound());
    unsigned int size   = bound - origin;
    return UIntRange(origin, size);
}

inline UIntRange ScaleL(const UIntRange &src, unsigned int wtLevel)
{
    unsigned int origin = RShiftCeil(src.Origin(), wtLevel);
    unsigned int bound  = RShiftCeil(src.Bound(),  wtLevel);
    unsigned int size   = bound - origin;
    return UIntRange(origin, size);
}

inline UIntRange ScaleH(const UIntRange &src, unsigned int wtLevel)
{
    if(!wtLevel) return src;
    return ScaleH(ScaleL(src, wtLevel-1));
}

inline Rect ScaleLx  (const Rect &src)                       { return Rect(ScaleL(src.RangeX()), src.RangeY()); }
inline Rect ScaleHx  (const Rect &src)                       { return Rect(ScaleH(src.RangeX()), src.RangeY()); }
inline Rect ScaleLy  (const Rect &src)                       { return Rect(src.RangeX(), ScaleL(src.RangeY())); }
inline Rect ScaleHy  (const Rect &src)                       { return Rect(src.RangeX(), ScaleH(src.RangeY())); }

inline Rect ScaleLx  (const Rect &src, unsigned int wtLevel) { return Rect(ScaleL(src.RangeX(), wtLevel), src.RangeY()); }
inline Rect ScaleHx  (const Rect &src, unsigned int wtLevel) { return Rect(ScaleH(src.RangeX(), wtLevel), src.RangeY()); }
inline Rect ScaleLy  (const Rect &src, unsigned int wtLevel) { return Rect(src.RangeX(), ScaleL(src.RangeY(), wtLevel)); }
inline Rect ScaleHy  (const Rect &src, unsigned int wtLevel) { return Rect(src.RangeX(), ScaleH(src.RangeY(), wtLevel)); }

inline Rect ScaleLxLy(const Rect &src)                       { return Rect(ScaleL(src.RangeX()), ScaleL(src.RangeY())); }
inline Rect ScaleHxLy(const Rect &src)                       { return Rect(ScaleH(src.RangeX()), ScaleL(src.RangeY())); }
inline Rect ScaleLxHy(const Rect &src)                       { return Rect(ScaleL(src.RangeX()), ScaleH(src.RangeY())); }
inline Rect ScaleHxHy(const Rect &src)                       { return Rect(ScaleH(src.RangeX()), ScaleH(src.RangeY())); }

inline Rect ScaleLxLy(const Rect &src, unsigned int wtLevel) { return Rect(ScaleL(src.RangeX(), wtLevel), ScaleL(src.RangeY(), wtLevel)); }
inline Rect ScaleHxLy(const Rect &src, unsigned int wtLevel) { return Rect(ScaleH(src.RangeX(), wtLevel), ScaleL(src.RangeY(), wtLevel)); }
inline Rect ScaleLxHy(const Rect &src, unsigned int wtLevel) { return Rect(ScaleL(src.RangeX(), wtLevel), ScaleH(src.RangeY(), wtLevel)); }
inline Rect ScaleHxHy(const Rect &src, unsigned int wtLevel) { return Rect(ScaleH(src.RangeX(), wtLevel), ScaleH(src.RangeY(), wtLevel)); }



//////
//
// mostly for encoder, that should be modified soon
//
inline unsigned int SizeL(unsigned int size) { return (size + 1) / 2; }

inline unsigned int SizeH(unsigned int size) { return size / 2; }

inline int SizeL(unsigned int size, unsigned int wtLevel)
{
    return RShiftCeil(size, wtLevel);
}

inline unsigned int SizeH(unsigned int size, unsigned int wtLevel)
{
    if(!wtLevel) return size;
    return SizeH(SizeL(size, wtLevel-1));
}

inline RectSize SizeLx(const RectSize &size) { return RectSize(SizeL(size.Width()), size.Height()); }
inline RectSize SizeHx(const RectSize &size) { return RectSize(SizeH(size.Width()), size.Height()); }
inline RectSize SizeLy(const RectSize &size) { return RectSize(size.Width(), SizeL(size.Height())); }
inline RectSize SizeHy(const RectSize &size) { return RectSize(size.Width(), SizeH(size.Height())); }

inline RectSize SizeLxLy(const RectSize &size) { return RectSize(SizeL(size.Width()), SizeL(size.Height())); }
inline RectSize SizeHxLy(const RectSize &size) { return RectSize(SizeH(size.Width()), SizeL(size.Height())); }
inline RectSize SizeLxHy(const RectSize &size) { return RectSize(SizeL(size.Width()), SizeH(size.Height())); }
inline RectSize SizeHxHy(const RectSize &size) { return RectSize(SizeH(size.Width()), SizeH(size.Height())); }

inline RectSize SizeLxLy(const RectSize &size, unsigned int wtLevel) { return RectSize(SizeL(size.Width(), wtLevel), SizeL(size.Height(), wtLevel)); }
inline RectSize SizeHxLy(const RectSize &size, unsigned int wtLevel) { return RectSize(SizeH(size.Width(), wtLevel), SizeL(size.Height(), wtLevel)); }
inline RectSize SizeLxHy(const RectSize &size, unsigned int wtLevel) { return RectSize(SizeL(size.Width(), wtLevel), SizeH(size.Height(), wtLevel)); }
inline RectSize SizeHxHy(const RectSize &size, unsigned int wtLevel) { return RectSize(SizeH(size.Width(), wtLevel), SizeH(size.Height(), wtLevel)); }


inline Point PointLxLy(const Point &point) { return Point(SizeL(point.X()), SizeL(point.Y())); }
inline Point PointHxLy(const Point &point) { return Point(SizeH(point.X()), SizeL(point.Y())); }
inline Point PointLxHy(const Point &point) { return Point(SizeL(point.X()), SizeH(point.Y())); }
inline Point PointHxHy(const Point &point) { return Point(SizeH(point.X()), SizeH(point.Y())); }

inline Point PointLxLy(const Point &point, unsigned int wtLevel) { return Point(SizeL(point.X(), wtLevel), SizeL(point.Y(), wtLevel)); }
inline Point PointHxLy(const Point &point, unsigned int wtLevel) { return Point(SizeH(point.X(), wtLevel), SizeL(point.Y(), wtLevel)); }
inline Point PointLxHy(const Point &point, unsigned int wtLevel) { return Point(SizeL(point.X(), wtLevel), SizeH(point.Y(), wtLevel)); }
inline Point PointHxHy(const Point &point, unsigned int wtLevel) { return Point(SizeH(point.X(), wtLevel), SizeH(point.Y(), wtLevel)); }


inline Rect RectLxLy(const Rect &rect) { return Rect(PointLxLy(rect.Origin()), SizeLxLy(rect.Size())); }
inline Rect RectHxLy(const Rect &rect) { return Rect(PointHxLy(rect.Origin()), SizeHxLy(rect.Size())); }
inline Rect RectLxHy(const Rect &rect) { return Rect(PointLxHy(rect.Origin()), SizeLxHy(rect.Size())); }
inline Rect RectHxHy(const Rect &rect) { return Rect(PointHxHy(rect.Origin()), SizeHxHy(rect.Size())); }

inline Rect RectLxLy(const Rect &rect, unsigned int wtLevel) { return Rect(PointLxLy(rect.Origin(), wtLevel), SizeLxLy(rect.Size(), wtLevel)); }
inline Rect RectHxLy(const Rect &rect, unsigned int wtLevel) { return Rect(PointHxLy(rect.Origin(), wtLevel), SizeHxLy(rect.Size(), wtLevel)); }
inline Rect RectLxHy(const Rect &rect, unsigned int wtLevel) { return Rect(PointLxHy(rect.Origin(), wtLevel), SizeLxHy(rect.Size(), wtLevel)); }
inline Rect RectHxHy(const Rect &rect, unsigned int wtLevel) { return Rect(PointHxHy(rect.Origin(), wtLevel), SizeHxHy(rect.Size(), wtLevel)); }

//
//
////////


inline unsigned int MaxNOfWTLevels(unsigned int size)
{
    int shift = 0;

    if(!(size&1)) size--;
    for(; size > 0; size >>= 1) shift++;

    return shift;
}

inline unsigned int NOfWTLevels(unsigned int nOfResLevels) { return nOfResLevels - 1; }

inline unsigned int NOfResLevels(unsigned int nOfWTLevels) { return nOfWTLevels + 1; }

inline unsigned int WTLevel(unsigned int sbDepth) { return sbDepth + 1; }

inline unsigned int WTLevel(unsigned int resLevel, unsigned int nOfResLevels)
{
    if(!resLevel) return nOfResLevels - 1;
    return nOfResLevels - resLevel;
}

inline unsigned int ResLevel(unsigned int sbDepth, unsigned int nOfWTLevels) { return nOfWTLevels - sbDepth;  }

inline unsigned int SBDepth(unsigned int resLevel, unsigned int nOfWTLevels) { return nOfWTLevels - resLevel; }


#endif // __WTMETRIC_H__
