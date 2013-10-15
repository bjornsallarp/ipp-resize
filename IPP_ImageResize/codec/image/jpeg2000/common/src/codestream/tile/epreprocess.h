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

#ifndef __EPREPROCESS_H__
#define __EPREPROCESS_H__

#include "imagecore.h"
#include "pp.h"
#include "rowiterator.h"
#include "jp2bitspercomp.h"

template <class T>
void LevShiftFwd(unsigned int bitDepth, T *srcDst, unsigned int width) { Sub((T)(1 << bitDepth), srcDst, width); }

template <class T>
void LevShiftFwd(unsigned int bitDepth, T *srcDst, unsigned int width, int scaleFactor)
{
    Sub((T)(1 << bitDepth), srcDst, width, -scaleFactor);
}

template <class T>
void LevShiftFwd(unsigned int bitDepth, const ImageCoreC<T, 1> &srcDst, const RectSize &size)
{
    unsigned int   width   = size.Width();
    unsigned int   height  = size.Height();
    T              offset  = (T)(1 << bitDepth);
    RowIterator<T> row     = srcDst;
    unsigned int   y       = 0;

    for( ; y < height; y++, ++row) Sub(offset, row, width);
}

template <class T>
void LevShiftFwd(unsigned int bitDepth, const ImageCoreC<T, 1> &srcDst, const RectSize &size, int scaleFactor)
{
    unsigned int   width   = size.Width();
    unsigned int   height  = size.Height();
    T              offset  = (T)(1 << bitDepth);
    RowIterator<T> row     = srcDst;
    unsigned int   y       = 0;

    for( ; y < height; y++, ++row) Sub(offset, row, width, -scaleFactor);
}

inline void LevShiftFwd32s32f(unsigned int bitDepth, Ipp32s *srcDst, unsigned int size)
{
    LevShiftFwd  (bitDepth, srcDst, size);
    Convert32s32f( (Ipp32f*)srcDst, size);
}

inline void LevShiftFwd32s32f(unsigned int bitDepth, const ImageCoreC<Ipp32s, 1> &srcDst, const RectSize &size)
{
    unsigned int        width   = size.Width();
    unsigned int        height  = size.Height();
    RowIterator<Ipp32s> row     = srcDst;
    unsigned int        y       = 0;

    for( ; y < height; y++, ++row) LevShiftFwd32s32f(bitDepth, row, width);
}

template <class T>
void LevShiftRCTFwd(
    const JP2BitsPerComp &bitsPerComp0,
    const JP2BitsPerComp &bitsPerComp1,
    const JP2BitsPerComp &bitsPerComp2,
    T                    *srcDst0,
    T                    *srcDst1,
    T                    *srcDst2,
    unsigned int          size)
{
    if(!bitsPerComp0.IsSigned())
        LevShiftFwd(bitsPerComp0.BitDepth(), srcDst0, size);

    if(!bitsPerComp1.IsSigned())
        LevShiftFwd(bitsPerComp1.BitDepth(), srcDst1, size);

    if(!bitsPerComp2.IsSigned())
        LevShiftFwd(bitsPerComp2.BitDepth(), srcDst2, size);

    RCTFwd(srcDst0, srcDst1, srcDst2, size);
}

template<class T>
void LevShiftICTFwd(
    const JP2BitsPerComp &bitsPerComp0,
    const JP2BitsPerComp &bitsPerComp1,
    const JP2BitsPerComp &bitsPerComp2,
    T                    *srcDst0,
    T                    *srcDst1,
    T                    *srcDst2,
    unsigned int          size,
    int                   scaleFactor)
{
    if(!bitsPerComp0.IsSigned())
        LevShiftFwd(bitsPerComp0.BitDepth(), srcDst0, size, scaleFactor);
    else
        RShift(scaleFactor, srcDst0, size);

    if(!bitsPerComp1.IsSigned())
        LevShiftFwd(bitsPerComp1.BitDepth(), srcDst1, size, scaleFactor);
    else
        RShift(scaleFactor, srcDst1, size);

    if(!bitsPerComp2.IsSigned())
        LevShiftFwd(bitsPerComp2.BitDepth(), srcDst2, size, scaleFactor);
    else
        RShift(scaleFactor, srcDst2, size);

    ICTFwd(srcDst0, srcDst1, srcDst2, size);
}

inline void LevShiftICTFwd32s32f(
    const JP2BitsPerComp &bitsPerComp0,
    const JP2BitsPerComp &bitsPerComp1,
    const JP2BitsPerComp &bitsPerComp2,
    Ipp32s               *srcDst0,
    Ipp32s               *srcDst1,
    Ipp32s               *srcDst2,
    unsigned int          size)
{
    if(!bitsPerComp0.IsSigned())
        LevShiftFwd32s32f(bitsPerComp0.BitDepth(), srcDst0, size);
    else
        Convert32s32f((Ipp32f*)srcDst0, size);

    if(!bitsPerComp1.IsSigned())
        LevShiftFwd32s32f(bitsPerComp1.BitDepth(), srcDst1, size);
    else
        Convert32s32f((Ipp32f*)srcDst1, size);

    if(!bitsPerComp2.IsSigned())
        LevShiftFwd32s32f(bitsPerComp2.BitDepth(), srcDst2, size);
    else
        Convert32s32f((Ipp32f*)srcDst2, size);

    ICTFwd((Ipp32f*)srcDst0, (Ipp32f*)srcDst1, (Ipp32f*)srcDst2, size);
}

template <class T>
void LevShiftRCTFwd(const ImageCoreC<T, 1> *srcDst, const RectSize &size, const JP2BitsPerComp *bitsPerComp)
{
    unsigned int width  = size.Width();
    unsigned int height = size.Height();

    RowIterator<T> rowCh0 = srcDst[0];
    RowIterator<T> rowCh1 = srcDst[1];
    RowIterator<T> rowCh2 = srcDst[2];

    for(unsigned int y = 0; y < height; y++, ++rowCh0, ++rowCh1, ++rowCh2)
        LevShiftRCTFwd(
            bitsPerComp[0],  bitsPerComp[1],  bitsPerComp[2],
            (T*)rowCh0, (T*)rowCh1, (T*)rowCh2,
            width);
}

template <class T>
void LevShiftICTFwd(const ImageCoreC<T, 1> *srcDst, const RectSize &size, const JP2BitsPerComp *bitsPerComp, int scaleFactor)
{
    unsigned int width  = size.Width();
    unsigned int height = size.Height();

    RowIterator<T> rowCh0 = srcDst[0];
    RowIterator<T> rowCh1 = srcDst[1];
    RowIterator<T> rowCh2 = srcDst[2];

    for(unsigned int y = 0; y < height; y++, ++rowCh0, ++rowCh1, ++rowCh2)
        LevShiftICTFwd(
            bitsPerComp[0],  bitsPerComp[1],  bitsPerComp[2],
            (T*)rowCh0, (T*)rowCh1, (T*)rowCh2,
            width, scaleFactor);
}

inline void LevShiftICTFwd32s32f(const ImageCoreC<Ipp32s, 1> *srcDst, const RectSize &size, const JP2BitsPerComp *bitsPerComp)
{
    unsigned int width  = size.Width();
    unsigned int height = size.Height();

    RowIterator<Ipp32s> rowCh0 = srcDst[0];
    RowIterator<Ipp32s> rowCh1 = srcDst[1];
    RowIterator<Ipp32s> rowCh2 = srcDst[2];

    for(unsigned int y = 0; y < height; y++, ++rowCh0, ++rowCh1, ++rowCh2)
        LevShiftICTFwd32s32f(
            bitsPerComp[0],  bitsPerComp[1],  bitsPerComp[2],
            (Ipp32s*)rowCh0, (Ipp32s*)rowCh1, (Ipp32s*)rowCh2,
            width);
}

template <class T>
void PreProcessWT53(
    const ImageCoreC<T, 1> *srcDst,
    const RectSize         &size,
    const JP2BitsPerComp   *bitsPerComp,
    unsigned int            nOfComponents,
    bool                    isMCTNeeded
    )
{
    if(!isMCTNeeded || nOfComponents != 3)
    {
        for(unsigned int component = 0; component < nOfComponents; component++)
        {
            JP2BitsPerComp bitsPerCompCurr = bitsPerComp[component];

            if(!bitsPerCompCurr.IsSigned()) LevShiftFwd(bitsPerCompCurr.BitDepth(), srcDst[component], size);
        }
    }
    else LevShiftRCTFwd(srcDst, size, bitsPerComp);
}

template <class T>
void PreProcessWT97(
    const ImageCoreC<T, 1> *srcDst,
    const RectSize         &size,
    const JP2BitsPerComp   *bitsPerComp,
    unsigned int            nOfComponents,
    bool                    isMCTNeeded,
    int                     scaleFactor
    )
{
    if(!isMCTNeeded || nOfComponents != 3)
    {
        for(unsigned int component = 0; component < nOfComponents; component++)
        {
            JP2BitsPerComp bitsPerCompCurr = bitsPerComp[component];

            if(!bitsPerCompCurr.IsSigned()) LevShiftFwd(bitsPerCompCurr.BitDepth(), srcDst[component], size, scaleFactor);
            else                            RShift     (                            srcDst[component], size, scaleFactor);
        }
    }
    else LevShiftICTFwd(srcDst, size, bitsPerComp, scaleFactor);
}

inline void PreProcessWT97_32s32f(
    const ImageCoreC<Ipp32s, 1> *srcDst,
    const RectSize              &size,
    const JP2BitsPerComp        *bitsPerComp,
    unsigned int                 nOfComponents,
    bool                         isMCTNeeded
    )
{
    if(!isMCTNeeded || nOfComponents != 3)
    {
        for(unsigned int component = 0; component < nOfComponents; component++)
        {
            JP2BitsPerComp bitsPerCompCurr = bitsPerComp[component];

            if(!bitsPerCompCurr.IsSigned()) LevShiftFwd32s32f(bitsPerCompCurr.BitDepth(), srcDst[component], size);
            else                            Convert32s32f    ( (ImageCoreC<Ipp32f, 1>&)   srcDst[component], size);
        }
    }
    else LevShiftICTFwd32s32f(srcDst, size, bitsPerComp);
}

inline void PreProcess32s32f(
    const ImageCoreC<Ipp32s, 1> *srcDst,
    const RectSize              &size,
    const JP2BitsPerComp        *bitsPerComp,
    unsigned int                 nOfComponents,
    bool                         isMCTNeeded,
    bool                         isWT53Used
    )
{
    if(isWT53Used) PreProcessWT53       (srcDst, size, bitsPerComp, nOfComponents, isMCTNeeded);
    else           PreProcessWT97_32s32f(srcDst, size, bitsPerComp, nOfComponents, isMCTNeeded);
}

#endif // __EPREPROCESS_H__
