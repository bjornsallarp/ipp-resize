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

#ifndef __DPOSTPROCESS_H__
#define __DPOSTPROCESS_H__

#include "imagecore.h"
#include "pp.h"
#include "rowiterator.h"
#include "jp2bitspercomp.h"


template <class T>
void LevShiftInv(unsigned int bitDepth, T *srcDst, unsigned int width) { Add((T)(1 << bitDepth), srcDst, width); }

template <class T>
void LevShiftInv(unsigned int bitDepth, T *srcDst, unsigned int width, int scaleFactor)
{
    Add((T)(1 << (bitDepth + scaleFactor)), srcDst, width, scaleFactor);
}

template <class T>
void LevShiftInvClip(unsigned int bitDepth, T *srcDst, unsigned int size)
{
    LevShiftInv(bitDepth, srcDst, size);
    Threshold  ((T)0, (T)((1 << (bitDepth + 1)) - 1), srcDst, size);
}

template <class T>
void LevShiftInv(unsigned int bitDepth, const ImageCoreC<T, 1> &srcDst, const RectSize &size)
{
    unsigned int   width   = size.Width();
    unsigned int   height  = size.Height();
    T              offset  = (T)(1 << bitDepth);
    RowIterator<T> row     = srcDst;
    unsigned int   y       = 0;

    for( ; y < height; y++, ++row) Add(offset, row, width);
}

template <class T>
void LevShiftInvClip(unsigned int bitDepth, const ImageCoreC<T, 1> &srcDst, const RectSize &size)
{
    unsigned int   width   = size.Width();
    unsigned int   height  = size.Height();
    T              offset  = (T)(1 << bitDepth);
    T              thresh  = (T)((1 << (bitDepth + 1)) - 1);
    RowIterator<T> row     = srcDst;
    unsigned int   y       = 0;

    for( ; y < height; y++, ++row)
    {
        Add      (offset, row, width);
        Threshold((T)0, thresh, (T*)row, width);
    }
}

template <class T>
void LevShiftInv(unsigned int bitDepth, const ImageCoreC<T, 1> &srcDst, const RectSize &size, int scaleFactor)
{
    unsigned int   width   = size.Width();
    unsigned int   height  = size.Height();
    T              offset  = (T)(1 << (bitDepth + scaleFactor));
    RowIterator<T> row     = srcDst;
    unsigned int   y       = 0;

    for( ; y < height; y++, ++row) Add(offset, row, width, scaleFactor);
}

inline void LevShiftInvClip32f32s(unsigned int bitDepth, Ipp32s *srcDst, unsigned int size)
{
    Convert32f32s( (Ipp32f*)srcDst, size);
    LevShiftInvClip(bitDepth, srcDst, size);
}

inline void SignedConvertClip32f32s(unsigned int bitDepth, Ipp32s *srcDst, unsigned int size)
{
    Convert32f32s( (Ipp32f*)srcDst, size);
    Threshold    (- (1 << bitDepth), (1 << bitDepth) - 1, srcDst, size);
}

inline void LevShiftInvClip32f32s(unsigned int bitDepth, const ImageCoreC<Ipp32s, 1> &srcDst, const RectSize &size)
{
    unsigned int        width   = size.Width();
    unsigned int        height  = size.Height();
    RowIterator<Ipp32s> row     = srcDst;
    unsigned int        y       = 0;

    for( ; y < height; y++, ++row) 
        LevShiftInvClip32f32s(bitDepth, row, width);
}

inline void SignedConvertClip32f32s(unsigned int bitDepth, const ImageCoreC<Ipp32s, 1> &srcDst, const RectSize &size)
{
    unsigned int        width   = size.Width();
    unsigned int        height  = size.Height();
    RowIterator<Ipp32s> row     = srcDst;
    unsigned int        y       = 0;

    for( ; y < height; y++, ++row) 
        SignedConvertClip32f32s(bitDepth, row, width);
}

template<class T>
void SignedClip(unsigned int bitDepth, const ImageCoreC<T, 1> &srcDst, const RectSize &size)
{
    Threshold((T)(- (1 << bitDepth)), (T)((1 << bitDepth) - 1), srcDst, size);
}

template <class T>
void LevShiftRCTInv(
    const JP2BitsPerComp &bitsPerComp0,
    const JP2BitsPerComp &bitsPerComp1,
    const JP2BitsPerComp &bitsPerComp2,
    T                    *srcDst0,
    T                    *srcDst1,
    T                    *srcDst2,
    unsigned int          size)
{
    RCTInv(srcDst0, srcDst1, srcDst2, size);

    if(!bitsPerComp0.IsSigned()) LevShiftInvClip(bitsPerComp0.BitDepth(), srcDst0, size);
    if(!bitsPerComp1.IsSigned()) LevShiftInvClip(bitsPerComp1.BitDepth(), srcDst1, size);
    if(!bitsPerComp2.IsSigned()) LevShiftInvClip(bitsPerComp2.BitDepth(), srcDst2, size);
}

template<class T>
void LevShiftICTInv(
    const JP2BitsPerComp &bitsPerComp0,
    const JP2BitsPerComp &bitsPerComp1,
    const JP2BitsPerComp &bitsPerComp2,
    T                    *srcDst0,
    T                    *srcDst1,
    T                    *srcDst2,
    unsigned int          size,
    int                   scaleFactor)
{
    ICTInv(srcDst0, srcDst1, srcDst2, size);

    if(!bitsPerComp0.IsSigned()) LevShiftInv(bitsPerComp0.BitDepth(), srcDst0, size, scaleFactor);
    else                         RShift(scaleFactor, srcDst0, size);
    if(!bitsPerComp1.IsSigned()) LevShiftInv(bitsPerComp1.BitDepth(), srcDst1, size, scaleFactor);
    else                         RShift(scaleFactor, srcDst1, size);
    if(!bitsPerComp2.IsSigned()) LevShiftInv(bitsPerComp2.BitDepth(), srcDst2, size, scaleFactor);
    else                         RShift(scaleFactor, srcDst2, size);
}

inline void LevShiftICTInv32f32s(
    const JP2BitsPerComp &bitsPerComp0,
    const JP2BitsPerComp &bitsPerComp1,
    const JP2BitsPerComp &bitsPerComp2,
    Ipp32s               *srcDst0,
    Ipp32s               *srcDst1,
    Ipp32s               *srcDst2,
    unsigned int          size)
{
    ICTInv((Ipp32f*)srcDst0, (Ipp32f*)srcDst1, (Ipp32f*)srcDst2, size);

    if(!bitsPerComp0.IsSigned())
        LevShiftInvClip32f32s  (bitsPerComp0.BitDepth(), srcDst0, size);
    else
        SignedConvertClip32f32s(bitsPerComp0.BitDepth(), srcDst0, size);

    if(!bitsPerComp1.IsSigned())
        LevShiftInvClip32f32s  (bitsPerComp1.BitDepth(), srcDst1, size);
    else
        SignedConvertClip32f32s(bitsPerComp1.BitDepth(), srcDst1, size);

    if(!bitsPerComp2.IsSigned())
        LevShiftInvClip32f32s  (bitsPerComp2.BitDepth(), srcDst2, size);
    else
        SignedConvertClip32f32s(bitsPerComp2.BitDepth(), srcDst2, size);
}

template <class T>
void LevShiftRCTInv(const ImageCoreC<T, 1> *srcDst, const RectSize &size, const JP2BitsPerComp *bitsPerComp)
{
    unsigned int width  = size.Width();
    unsigned int height = size.Height();

    RowIterator<T> rowCh0 = srcDst[0];
    RowIterator<T> rowCh1 = srcDst[1];
    RowIterator<T> rowCh2 = srcDst[2];

    for(unsigned int y = 0; y < height; y++, ++rowCh0, ++rowCh1, ++rowCh2)
        LevShiftRCTInv(
            bitsPerComp[0],  bitsPerComp[1],  bitsPerComp[2],
            (T*)rowCh0    , (T*)rowCh1     , (T*)rowCh2     ,
            width);
}

template <class T>
void LevShiftICTInv(const ImageCoreC<T, 1> *srcDst, const RectSize &size, const JP2BitsPerComp *bitsPerComp, int scaleFactor)
{
    unsigned int width  = size.Width();
    unsigned int height = size.Height();

    RowIterator<T> rowCh0 = srcDst[0];
    RowIterator<T> rowCh1 = srcDst[1];
    RowIterator<T> rowCh2 = srcDst[2];

    for(unsigned int y = 0; y < height; y++, ++rowCh0, ++rowCh1, ++rowCh2)
        LevShiftICTInv(
            bitsPerComp[0],  bitsPerComp[1],  bitsPerComp[2],
            (T*)rowCh0, (T*)rowCh1, (T*)rowCh2,
            width, scaleFactor);
}

inline void LevShiftICTInv32f32s(const ImageCoreC<Ipp32s, 1> *srcDst, const RectSize &size, const JP2BitsPerComp *bitsPerComp)
{
    unsigned int width  = size.Width();
    unsigned int height = size.Height();

    RowIterator<Ipp32s> rowCh0 = srcDst[0];
    RowIterator<Ipp32s> rowCh1 = srcDst[1];
    RowIterator<Ipp32s> rowCh2 = srcDst[2];

    for(unsigned int y = 0; y < height; y++, ++rowCh0, ++rowCh1, ++rowCh2)
        LevShiftICTInv32f32s(
            bitsPerComp[0],  bitsPerComp[1],  bitsPerComp[2],
            (Ipp32s*)rowCh0, (Ipp32s*)rowCh1, (Ipp32s*)rowCh2,
            width);
}

#endif // __DPOSTPROCESS_H__
