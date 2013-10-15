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

/*
//  Performance primitives C++ wrapper.
//
//
//
*/

#ifndef __PP_H__
#define __PP_H__

#include "ipps.h"
#include "ippi.h"
#include "ippj.h"


#include "rowiterator.h"


#if _MSC_VER
#pragma warning( disable : 4514 ) // unreferenced inline/local function has been removed
#pragma warning( disable : 4710 ) // 'function' : function not inlined
#endif

///////////////////////////////////////////////////////////////////////////
//
//  IPP signal-processing primitives (ipps) wrappers
//
//

inline void Add(const Ipp32f *src1, const Ipp32f *src2, Ipp32f *dst, unsigned int size)
{
    ippsAdd_32f(src1, src2, dst, size);
}

inline void Add(const Ipp32f *src, Ipp32f *srcDst, unsigned int size)
{
    ippsAdd_32f_I(src, srcDst, size);
}

inline void Add(Ipp32f value, Ipp32f* srcDst, unsigned int size)
{
    ippsAddC_32f_I(value, srcDst, size);
}

inline void Add(Ipp32f value, const Ipp32f* src, Ipp32f *dst, unsigned int size)
{
    ippsAddC_32f(src, value, dst, size);
}

inline void Add(const Ipp32s *src1, const Ipp32s *src2, Ipp32s *dst, unsigned int size)
{
    ippsAdd_32s_Sfs(src1, src2, dst, size, 0);
}

inline void Add(const Ipp32s *src, Ipp32s *srcDst, unsigned int size)
{
    ippsAdd_32s_ISfs(src, srcDst, size, 0);
}

inline void Add(Ipp32s value, Ipp32s* srcDst, unsigned int size)
{
    ippsAddC_32s_ISfs(value, srcDst, size, 0);
}

inline void Add(Ipp32s value, const Ipp32s* src, Ipp32s *dst, unsigned int size)
{
    ippsAddC_32s_Sfs(src, value, dst, size, 0);
}


inline void Add(const Ipp16s *src1, const Ipp16s *src2, Ipp16s *dst, unsigned int size)
{
    ippsAdd_16s_Sfs(src1, src2, dst, size, 0);
}

inline void Add(const Ipp16s *src, Ipp16s *srcDst, unsigned int size)
{
    ippsAdd_16s_ISfs(src, srcDst, size, 0);
}

inline void Add(Ipp16s value, Ipp16s* srcDst, unsigned int size)
{
    ippsAddC_16s_ISfs(value, srcDst, size, 0);
}

inline void Add(Ipp16s value, const Ipp16s* src, Ipp16s *dst, unsigned int size)
{
    ippsAddC_16s_Sfs(src, value, dst, size, 0);
}


inline void Add(Ipp16s value, Ipp16s* srcDst, unsigned int size, int scaleFactor)
{
    ippsAddC_16s_ISfs(value, srcDst, size, scaleFactor);
}

inline void Add(Ipp32s value, Ipp32s* srcDst, unsigned int size, int scaleFactor)
{
    ippsAddC_32s_ISfs(value, srcDst, size, scaleFactor);
}

inline void Mul(const Ipp32f *src1, const Ipp32f *src2, Ipp32f *dst, unsigned int size)
{
    ippsMul_32f(src1, src2, dst, size);
}

inline void Mul(const Ipp32f *src, Ipp32f *srcDst, unsigned int size)
{
    ippsMul_32f_I(src, srcDst, size);
}

inline void Mul(Ipp32f value, Ipp32f* srcDst, unsigned int size)
{
    ippsMulC_32f_I(value, srcDst, size);
}

inline void Mul(Ipp32f value, const Ipp32f* src, Ipp32f *dst, unsigned int size)
{
    ippsMulC_32f(src, value, dst, size);
}

inline void Mul(Ipp32s value, Ipp32s* srcDst, unsigned int size, int scaleFactor)
{
    ippsMulC_32s_ISfs(value, srcDst, size, scaleFactor);
}

inline void Sub(const Ipp32f *src1, const Ipp32f *src2, Ipp32f *dst, unsigned int size)
{
    ippsSub_32f(src1, src2, dst, size);
}

inline void Sub(const Ipp32f *src, Ipp32f *srcDst, unsigned int size)
{
    ippsSub_32f_I(src, srcDst, size);
}

inline void Sub(Ipp32f value, Ipp32f* srcDst, unsigned int size)
{
    ippsSubC_32f_I(value, srcDst, size);
}

inline void Sub(const Ipp32s *src1, const Ipp32s *src2, Ipp32s *dst, unsigned int size)
{
    ippsSub_32s_Sfs(src1, src2, dst, size, 0);
}

inline void Sub(const Ipp32s *src, Ipp32s *srcDst, unsigned int size)
{
    ippsSub_32s_ISfs(src, srcDst, size, 0);
}

inline void Sub(Ipp32s value, Ipp32s* srcDst, unsigned int size)
{
    ippsSubC_32s_ISfs(value, srcDst, size, 0);
}


inline void Sub(const Ipp16s *src1, const Ipp16s *src2, Ipp16s *dst, unsigned int size)
{
    ippsSub_16s_Sfs(src1, src2, dst, size, 0);
}

inline void Sub(const Ipp16s *src, Ipp16s *srcDst, unsigned int size)
{
    ippsSub_16s_ISfs(src, srcDst, size, 0);
}

inline void Sub(Ipp16s value, Ipp16s* srcDst, unsigned int size)
{
    ippsSubC_16s_ISfs(value, srcDst, size, 0);
}


inline void Sub(Ipp32s value, Ipp32s* srcDst, unsigned int size, int scaleFactor)
{
    ippsSubC_32s_ISfs(value, srcDst, size, scaleFactor);
}


inline void Sub(Ipp16s value, Ipp16s* srcDst, unsigned int size, int scaleFactor)
{
    ippsSubC_16s_ISfs(value, srcDst, size, scaleFactor);
}


inline void LShift(unsigned int value, const Ipp32s *src, Ipp32s *dst, unsigned int size)
{
    ippsLShiftC_32s(src, value, dst, size);
}

inline void LShift(unsigned int value, const Ipp16s *src, Ipp16s *dst, unsigned int size)
{
    ippsLShiftC_16s(src, value, dst, size);
}

inline void LShift(unsigned int value, Ipp16s *srcDst, unsigned int size)
{
    ippsLShiftC_16s_I(value, srcDst, size);
}

inline void RShift(unsigned int value, const Ipp32s *src, Ipp32s *dst, unsigned int size)
{
    ippsRShiftC_32s(src, value, dst, size);
}

inline void RShift(unsigned int value, const Ipp16s *src, Ipp16s *dst, unsigned int size)
{
    ippsRShiftC_16s(src, value, dst, size);
}

inline void RShift(unsigned int value, Ipp32s *srcDst, unsigned int size)
{
    ippsRShiftC_32s_I(value, srcDst, size);
}

inline void RShift(unsigned int value, Ipp16s *srcDst, unsigned int size)
{
    ippsRShiftC_16s_I(value, srcDst, size);
}

inline void ThresholdGT(Ipp32s level, Ipp32s* srcDst, unsigned int size)

{
    ippsThreshold_GT_32s_I(srcDst, size, level);
}

inline void ThresholdLT(Ipp32s level, Ipp32s* srcDst, unsigned int size)

{
    ippsThreshold_LT_32s_I(srcDst, size, level);
}

inline void ThresholdGT(Ipp16s level, Ipp16s* srcDst, unsigned int size)

{
    ippsThreshold_GT_16s_I(srcDst, size, level);
}

inline void ThresholdLT(Ipp16s level, Ipp16s* srcDst, unsigned int size)

{
    ippsThreshold_LT_16s_I(srcDst, size, level);
}

template<class T>
void Threshold(T min, T max, T* srcDst, unsigned int size)
{
    ThresholdLT(min, srcDst, size);
    ThresholdGT(max, srcDst, size);
}

template<class T>
void Threshold(T min, T max, const ImageCoreC<T, 1> &srcDst, const IppiSize &size)
{
    RowIterator<T> rowSrcDst = srcDst;

    for(int i = 0; i < size.height; i++, ++rowSrcDst)
        Threshold(min, max, (T*)rowSrcDst, size.width);
}

inline void Convert(const Ipp16s *src, Ipp32s *dst, unsigned int size)
{
    ippsConvert_16s32s(src, dst, size);
}

inline void Convert(const Ipp32s *src, Ipp16s *dst, unsigned int size)
{
    ippsConvert_32s16s(src, dst, size);
}

inline void Convert(const Ipp32s *src, Ipp32f *dst, unsigned int size)
{
    ippsConvert_32s32f_Sfs(src, dst, size, 0);
}

inline void Convert(const Ipp32f *src, Ipp32s *dst, unsigned int size)
{
    ippsConvert_32f32s_Sfs(src, dst, size, ippRndNear, 0);
}

inline void Convert32s32f(Ipp32f *srcDst, unsigned int size)
{
    ippsConvert_32s32f_Sfs((Ipp32s*)srcDst, srcDst, size, 0);
}

inline void Convert32f32s(Ipp32f *srcDst, unsigned int size)
{
    ippsConvert_32f32s_Sfs(srcDst, (Ipp32s*)srcDst, size, ippRndNear, 0);
}

inline void Convert(const ImageCoreC<Ipp16s, 1> &src, const ImageCoreC<Ipp32s, 1> &dst,
    const IppiSize &size)
{
    RowIterator<Ipp16s> rowSrc = src;
    RowIterator<Ipp32s> rowDst = dst;

    for(int i = 0; i < size.height; i++, ++rowSrc, ++rowDst)
        Convert((Ipp16s*)rowSrc, (Ipp32s*)rowDst, size.width);
}

inline void Convert(const ImageCoreC<Ipp32s, 1> &src, const ImageCoreC<Ipp16s, 1> &dst,
    const IppiSize &size)
{
    RowIterator<Ipp32s> rowSrc = src;
    RowIterator<Ipp16s> rowDst = dst;

    for(int i = 0; i < size.height; i++, ++rowSrc, ++rowDst)
        Convert((Ipp32s*)rowSrc, (Ipp16s*)rowDst, size.width);
}

inline void Convert(const ImageCoreC<Ipp32s, 1> &src, const ImageCoreC<Ipp32f, 1> &dst,
    const IppiSize &size)
{
    RowIterator<Ipp32s> rowSrc = src;
    RowIterator<Ipp32f> rowDst = dst;

    for(int i = 0; i < size.height; i++, ++rowSrc, ++rowDst)
        Convert((Ipp32s*)rowSrc, (Ipp32f*)rowDst, size.width);
}

inline void Convert(const ImageCoreC<Ipp32f, 1> &src, const ImageCoreC<Ipp32s, 1> &dst,
    const IppiSize &size)
{
    RowIterator<Ipp32f> rowSrc = src;
    RowIterator<Ipp32s> rowDst = dst;

    for(int i = 0; i < size.height; i++, ++rowSrc, ++rowDst)
        Convert((Ipp32f*)rowSrc, (Ipp32s*)rowDst, size.width);
}

inline void Convert32s32f(const ImageCoreC<Ipp32f, 1> &srcDst, const IppiSize &size)
{
    RowIterator<Ipp32f> row = srcDst;

    for(int i = 0; i < size.height; i++, ++row)
        Convert32s32f(row, size.width);
}

inline void Convert32f32s(const ImageCoreC<Ipp32f, 1> &srcDst, const IppiSize &size)
{
    RowIterator<Ipp32f> row = srcDst;

    for(int i = 0; i < size.height; i++, ++row)
        Convert32f32s(row, size.width);
}

///////////////////////////////////////////////////////////////////////////
//
//  IPP image-processing primitives (mostly ippi & a few ipps-derived auxiliary) wrappers
//


inline void Copy(const ImageCoreC<Ipp16s, 1> &src, const ImageCoreC<Ipp16s, 1> &dst,
    const IppiSize &size)
{
    ippiCopy_16s_C1R(src.Data(), src.LineStep(),
                     dst.Data(), dst.LineStep(), size);
}

inline void Copy(const ImageCoreC<Ipp32s, 1> &src, const ImageCoreC<Ipp32s, 1> &dst,
    const IppiSize &size)
{
    IppiSize size_x_2 = {size.width * 2, size.height};
    ippiCopy_16s_C1R((Ipp16s*)src.Data(), src.LineStep(),
                     (Ipp16s*)dst.Data(), dst.LineStep(), size_x_2);
}

inline void Copy(const ImageCoreC<Ipp32f, 1> &src, const ImageCoreC<Ipp32f, 1> &dst,
    const IppiSize &size)
{
    ippiCopy_32f_C1R(src.Data(), src.LineStep(),
                     dst.Data(), dst.LineStep(), size);
}

//inline void Mul(const ImageCoreC<Ipp32f, 1> &srcDst, const IppiSize &size, Ipp32f value)
//{
//    ippiMulC_32f_C1IR(value, srcDst.Data(), srcDst.LineStep(),  size);
//}

inline void Mul(const ImageCoreC<Ipp16s, 1> &srcDst, const IppiSize &size, Ipp16s value, int scaleFactor)
{
    ippiMulC_16s_C1IRSfs(value, srcDst.Data(), srcDst.LineStep(), size, scaleFactor);
}

inline void Mul(const ImageCoreC<Ipp32s, 1> &srcDst, const IppiSize &size, Ipp32s value, int scaleFactor)
{
    RowIterator<Ipp32s> row = srcDst;

    for(int i = 0; i < size.height; i++, ++row)
        Mul(value, row, size.width, scaleFactor);
}


/*
inline void LShift(const ImageCoreC<Ipp32s, 1> &srcDst,
    const IppiSize &size, unsigned int shift)
{
    ippiLShiftC_32s_C1IR(shift, srcDst.Data(), srcDst.LineStep(), size);
}
*/

inline void LShift(const ImageCoreC<Ipp32s, 1> &src, const ImageCoreC<Ipp32s, 1> &dst,
    const IppiSize &size, unsigned int shift)
{
    ippiLShiftC_32s_C1R(src.Data(), src.LineStep(), shift, dst.Data(), dst.LineStep(), size);
}

inline void LShift(const ImageCoreC<Ipp16s, 1> &srcDst,
    const IppiSize &size, unsigned int shift)
{
    RowIterator<Ipp16s> row = srcDst;

    for(int i = 0; i < size.height; i++, ++row)
        LShift(shift, row, size.width);
}

inline void RShift(const ImageCoreC<Ipp32s, 1> &srcDst,
    const IppiSize &size, unsigned int shift)
{
    ippiRShiftC_32s_C1IR(shift, srcDst.Data(), srcDst.LineStep(), size);
}

inline void RShift(const ImageCoreC<Ipp16s, 1> &srcDst,
    const IppiSize &size, unsigned int shift)
{
    ippiRShiftC_16s_C1IR(shift, srcDst.Data(), srcDst.LineStep(), size);
}

inline void Shift(const ImageCoreC<Ipp32s, 1> &srcDst,
    const IppiSize &size, int shift)
{
    if(shift > 0)
        ippiLShiftC_32s_C1IR(shift, srcDst.Data(), srcDst.LineStep(), size);
    else if(shift < 0)
        ippiRShiftC_32s_C1IR(-shift, srcDst.Data(), srcDst.LineStep(), size);
    else
        return;
}


template<class T>
void Zero(const ImageCoreC<T, 1> &srcDst, const IppiSize &size)
{
    IppiSize roiSize = {size.width * sizeof(T), size.height};

    ippiSet_8u_C1R(0, (Ipp8u*)srcDst.Data(), srcDst.LineStep(), roiSize);
}

inline void Complement(const ImageCoreC<Ipp32s, 1> &srcDst, const IppiSize &size)
{
    ippiComplement_32s_C1IR(srcDst.Data(), srcDst.LineStep(), size);
}

inline void RCTFwd(const ImageCorePn<Ipp32s> &srcDst, const IppiSize &size)
{
    ippiRCTFwd_JPEG2K_32s_P3IR((Ipp32s**)srcDst.Data(), srcDst.LineStep(), size);
}

inline void RCTFwd(Ipp32s *srcDst0, Ipp32s *srcDst1, Ipp32s *srcDst2, unsigned int size)
{
    IppiSize roiSize = {size, 1};
    Ipp32s *srcDstArr[3] = {srcDst0, srcDst1, srcDst2};
    ippiRCTFwd_JPEG2K_32s_P3IR(srcDstArr, size * sizeof(Ipp32s), roiSize);
}

inline void RCTFwd(const ImageCorePn<Ipp16s> &srcDst, const IppiSize &size)
{
    ippiRCTFwd_JPEG2K_16s_P3IR((Ipp16s**)srcDst.Data(), srcDst.LineStep(), size);
}

inline void RCTFwd(Ipp16s *srcDst0, Ipp16s *srcDst1, Ipp16s *srcDst2, unsigned int size)
{
    IppiSize roiSize = {size, 1};
    Ipp16s *srcDstArr[3] = {srcDst0, srcDst1, srcDst2};
    ippiRCTFwd_JPEG2K_16s_P3IR(srcDstArr, size * sizeof(Ipp32s), roiSize);
}

inline void RCTInv(const ImageCorePn<Ipp32s> &srcDst, const IppiSize &size)
{
    ippiRCTInv_JPEG2K_32s_P3IR((Ipp32s**)srcDst.Data(), srcDst.LineStep(), size);
}

inline void RCTInv(Ipp32s *srcDst0, Ipp32s *srcDst1, Ipp32s *srcDst2, unsigned int size)
{
    IppiSize roiSize = {size, 1};
    Ipp32s *srcDstArr[3] = {srcDst0, srcDst1, srcDst2};
    ippiRCTInv_JPEG2K_32s_P3IR(srcDstArr, size * sizeof(Ipp32s), roiSize);
}

inline void RCTInv(const ImageCorePn<Ipp16s> &srcDst, const IppiSize &size)
{
    ippiRCTInv_JPEG2K_16s_P3IR((Ipp16s**)srcDst.Data(), srcDst.LineStep(), size);
}

inline void RCTInv(Ipp16s *srcDst0, Ipp16s *srcDst1, Ipp16s *srcDst2, unsigned int size)
{
    IppiSize roiSize = {size, 1};
    Ipp16s *srcDstArr[3] = {srcDst0, srcDst1, srcDst2};
    ippiRCTInv_JPEG2K_16s_P3IR(srcDstArr, size * sizeof(Ipp16s), roiSize);
}

inline void RCTInv(
                   const ImageCoreC<Ipp32s, 1> &srcDst0,
                   const ImageCoreC<Ipp32s, 1> &srcDst1,
                   const ImageCoreC<Ipp32s, 1> &srcDst2,
                   const IppiSize              &size)
{
    RowIterator<Ipp32s> row0 = srcDst0;
    RowIterator<Ipp32s> row1 = srcDst1;
    RowIterator<Ipp32s> row2 = srcDst2;

    for(int i = 0; i < size.height; i++, ++row0, ++row1, ++row2)
    {
        RCTInv(row0, row1, row2, size.width);
    }
}

inline void RCTInv(
                   const ImageCoreC<Ipp16s, 1> &srcDst0,
                   const ImageCoreC<Ipp16s, 1> &srcDst1,
                   const ImageCoreC<Ipp16s, 1> &srcDst2,
                   const IppiSize              &size)
{
    RowIterator<Ipp16s> row0 = srcDst0;
    RowIterator<Ipp16s> row1 = srcDst1;
    RowIterator<Ipp16s> row2 = srcDst2;

    for(int i = 0; i < size.height; i++, ++row0, ++row1, ++row2)
    {
        RCTInv(row0, row1, row2, size.width);
    }
}

inline void ICTFwd(const ImageCorePn<Ipp32f> &srcDst, const IppiSize &size)
{
    ippiICTFwd_JPEG2K_32f_P3IR((Ipp32f**)srcDst.Data(), srcDst.LineStep(), size);
}

inline void ICTFwd(Ipp32f *srcDst0, Ipp32f *srcDst1, Ipp32f *srcDst2, unsigned int size)
{
    IppiSize roiSize = {size, 1};
    Ipp32f *srcDstArr[3] = {srcDst0, srcDst1, srcDst2};
    ippiICTFwd_JPEG2K_32f_P3IR(srcDstArr, size * sizeof(Ipp32f), roiSize);
}

inline void ICTFwd(const ImageCorePn<Ipp32s> &srcDst, const IppiSize &size)
{
    ippiICTFwd_JPEG2K_32s_P3IR((Ipp32s**)srcDst.Data(), srcDst.LineStep(), size);
}

inline void ICTFwd(Ipp32s *srcDst0, Ipp32s *srcDst1, Ipp32s *srcDst2, unsigned int size)
{
    IppiSize roiSize = {size, 1};
    Ipp32s *srcDstArr[3] = {srcDst0, srcDst1, srcDst2};
    ippiICTFwd_JPEG2K_32s_P3IR(srcDstArr, size * sizeof(Ipp32s), roiSize);
}

inline void ICTFwd(const ImageCorePn<Ipp16s> &srcDst, const IppiSize &size)
{
    ippiICTFwd_JPEG2K_16s_P3IR((Ipp16s**)srcDst.Data(), srcDst.LineStep(), size);
}

inline void ICTFwd(Ipp16s *srcDst0, Ipp16s *srcDst1, Ipp16s *srcDst2, unsigned int size)
{
    IppiSize roiSize = {size, 1};
    Ipp16s *srcDstArr[3] = {srcDst0, srcDst1, srcDst2};
    ippiICTFwd_JPEG2K_16s_P3IR(srcDstArr, size * sizeof(Ipp16s), roiSize);
}

inline void ICTInv(const ImageCorePn<Ipp32f> &srcDst, const IppiSize &size)
{
    ippiICTInv_JPEG2K_32f_P3IR((Ipp32f**)srcDst.Data(), srcDst.LineStep(), size);
}

inline void ICTInv(Ipp32f *srcDst0, Ipp32f *srcDst1, Ipp32f *srcDst2, unsigned int size)
{
    IppiSize roiSize = {size, 1};
    Ipp32f *srcDstArr[3] = {srcDst0, srcDst1, srcDst2};
    ippiICTInv_JPEG2K_32f_P3IR(srcDstArr, size * sizeof(Ipp32f), roiSize);
}

inline void ICTInv(
    const ImageCoreC<Ipp32f, 1> &srcDst0,
    const ImageCoreC<Ipp32f, 1> &srcDst1,
    const ImageCoreC<Ipp32f, 1> &srcDst2,
    const IppiSize              &size)
{
    RowIterator<Ipp32f> row0 = srcDst0;
    RowIterator<Ipp32f> row1 = srcDst1;
    RowIterator<Ipp32f> row2 = srcDst2;

    for(int i = 0; i < size.height; i++, ++row0, ++row1, ++row2)
    {
        ICTInv(row0, row1, row2, size.width);
    }
}

inline void ICTInv(const ImageCorePn<Ipp32s> &srcDst, const IppiSize &size)
{
    ippiICTInv_JPEG2K_32s_P3IR((Ipp32s**)srcDst.Data(), srcDst.LineStep(), size);
}

inline void ICTInv(Ipp32s *srcDst0, Ipp32s *srcDst1, Ipp32s *srcDst2, unsigned int size)
{
    IppiSize roiSize = {size, 1};
    Ipp32s *srcDstArr[3] = {srcDst0, srcDst1, srcDst2};
    ippiICTInv_JPEG2K_32s_P3IR(srcDstArr, size * sizeof(Ipp32s), roiSize);
}

inline void ICTInv(const ImageCorePn<Ipp16s> &srcDst, const IppiSize &size)
{
    ippiICTInv_JPEG2K_16s_P3IR((Ipp16s**)srcDst.Data(), srcDst.LineStep(), size);
}

inline void ICTInv(Ipp16s *srcDst0, Ipp16s *srcDst1, Ipp16s *srcDst2, unsigned int size)
{
    IppiSize roiSize = {size, 1};
    Ipp16s *srcDstArr[3] = {srcDst0, srcDst1, srcDst2};
    ippiICTInv_JPEG2K_16s_P3IR(srcDstArr, size * sizeof(Ipp16s), roiSize);
}

inline void ICTInv(
                   const ImageCoreC<Ipp16s, 1> &srcDst0,
                   const ImageCoreC<Ipp16s, 1> &srcDst1,
                   const ImageCoreC<Ipp16s, 1> &srcDst2,
                   const IppiSize              &size)
{
    RowIterator<Ipp16s> row0 = srcDst0;
    RowIterator<Ipp16s> row1 = srcDst1;
    RowIterator<Ipp16s> row2 = srcDst2;

    for(int i = 0; i < size.height; i++, ++row0, ++row1, ++row2)
    {
        ICTInv(row0, row1, row2, size.width);
    }
}

inline void CopyReplicateBorder(
             const ImageCoreC<Ipp32s, 1> &src, const IppiSize &srcSize,
             const ImageCoreC<Ipp32s, 1> &dst, const IppiSize &dstSize,
             const Point &dstOrigin)
{
    ippiCopyReplicateBorder_32s_C1R(
        src.Data(), src.LineStep(), srcSize,
        dst.Data(), dst.LineStep(), dstSize,
        dstOrigin.X(), dstOrigin.Y());
}

///////////////////////////////////////////////////////////////////////////
//
//  Wavelet transforms performance primitives wrappers
//
//

typedef IppiWTFilterFirst WTPhase;

inline bool IsPhaseL(WTPhase phase) { return phase == ippWTFilterFirstLow; }
inline bool IsPhaseH(WTPhase phase) { return phase == ippWTFilterFirstHigh; }

inline WTPhase Phase(unsigned int coor) { return coor & 1 ? ippWTFilterFirstHigh : ippWTFilterFirstLow; }

inline void WT53FwdRow(
  const Ipp16s*      src,
        Ipp16s*      low,
        Ipp16s*      high,
        unsigned int dstSize,
        WTPhase      phase)
{
    IppiSize roiSize = {dstSize, 1};

    ippiWTFwdRow_B53_JPEG2K_16s_C1R(
        src,  sizeof(Ipp16s) * 2 * dstSize,
        low,  sizeof(Ipp16s) * dstSize,
        high, sizeof(Ipp16s) * dstSize,
        roiSize, phase);
}

inline void WT53FwdRow(
  const Ipp32s*      src,
        Ipp32s*      low,
        Ipp32s*      high,
        unsigned int dstSize,
        WTPhase      phase)
{
    IppiSize roiSize = {dstSize, 1};

    ippiWTFwdRow_B53_JPEG2K_32s_C1R(
        src,  sizeof(Ipp32s) * 2 * dstSize,
        low,  sizeof(Ipp32s) * dstSize,
        high, sizeof(Ipp32s) * dstSize,
        roiSize, phase);
}

inline void WT53InvRow(
  const Ipp16s*      low,
  const Ipp16s*      high,
        unsigned int srcSize,
        Ipp16s*      dst,
        WTPhase      phase)
{
    IppiSize roiSize = {srcSize, 1};

    ippiWTInvRow_B53_JPEG2K_16s_C1R(
        low , sizeof(Ipp16s) * srcSize,
        high, sizeof(Ipp16s) * srcSize,
        roiSize,
        dst , sizeof(Ipp16s) * 2 * srcSize,
        phase);
}

inline void WT53InvRow(
  const Ipp32s*      low,
  const Ipp32s*      high,
        unsigned int srcSize,
        Ipp32s*      dst,
        WTPhase      phase)
{
    IppiSize roiSize = {srcSize, 1};

    ippiWTInvRow_B53_JPEG2K_32s_C1R(
        low , sizeof(Ipp32s) * srcSize,
        high, sizeof(Ipp32s) * srcSize,
        roiSize,
        dst , sizeof(Ipp32s) * 2 * srcSize,
        phase);
}

inline void WT53FwdColLift(
  const Ipp32s* src0,
  const Ipp32s* src1,
  const Ipp32s* src2,
        Ipp32s* dstLow0,
  const Ipp32s* srcHigh0,
        Ipp32s* dstHigh1,
        int     width)
{
    ippiWTFwdColLift_B53_JPEG2K_32s_C1(src0, src1, src2, dstLow0, srcHigh0, dstHigh1, width);
}

inline void WT53FwdColLift(
  const Ipp16s* src0,
  const Ipp16s* src1,
  const Ipp16s* src2,
        Ipp16s* dstLow0,
  const Ipp16s* srcHigh0,
        Ipp16s* dstHigh1,
        int     width)
{
    ippiWTFwdColLift_B53_JPEG2K_16s_C1(src0, src1, src2, dstLow0, srcHigh0, dstHigh1, width);
}

inline void WT53InvColLift(
  const Ipp16s* srcLow0,
  const Ipp16s* srcHigh0,
  const Ipp16s* srcHigh1,
  const Ipp16s* src0,
        Ipp16s* dst1,
        Ipp16s* dst2,
        int     width)
{
    ippiWTInvColLift_B53_JPEG2K_16s_C1(srcLow0, srcHigh0, srcHigh1, src0, dst1, dst2, width);
}

inline void WT53InvColLift(
  const Ipp32s* srcLow0,
  const Ipp32s* srcHigh0,
  const Ipp32s* srcHigh1,
  const Ipp32s* src0,
        Ipp32s* dst1,
        Ipp32s* dst2,
        int     width)
{
    ippiWTInvColLift_B53_JPEG2K_32s_C1(srcLow0, srcHigh0, srcHigh1, src0, dst1, dst2, width);
}

inline void WT97FwdRow(
  const Ipp16s*      src,
        Ipp16s*      low,
        Ipp16s*      high,
        unsigned int dstSize,
        WTPhase      phase)
{
    IppiSize roiSize = {dstSize, 1};

    ippiWTFwdRow_D97_JPEG2K_16s_C1R(
        src , sizeof(Ipp16s) * 2 * dstSize,
        low , sizeof(Ipp16s) * dstSize,
        high, sizeof(Ipp16s) * dstSize,
        roiSize, phase);
}

inline void WT97FwdRow(
  const Ipp32s*      src,
        Ipp32s*      low,
        Ipp32s*      high,
        unsigned int dstSize,
        WTPhase      phase)
{
    IppiSize roiSize = {dstSize, 1};

    ippiWTFwdRow_D97_JPEG2K_32s_C1R(
        src , sizeof(Ipp32s) * 2 * dstSize,
        low , sizeof(Ipp32s) * dstSize,
        high, sizeof(Ipp32s) * dstSize,
        roiSize, phase);
}

inline void WT97FwdRow(
  const Ipp32f*      src,
        Ipp32f*      low,
        Ipp32f*      high,
        unsigned int dstSize,
        WTPhase      phase)
{
    IppiSize roiSize = {dstSize, 1};

    ippiWTFwdRow_D97_JPEG2K_32f_C1R(
        src , sizeof(Ipp32f) * 2 * dstSize,
        low , sizeof(Ipp32f) * dstSize,
        high, sizeof(Ipp32f) * dstSize,
        roiSize, phase);
}

inline void WT97InvRow(
  const Ipp16s*      low,
  const Ipp16s*      high,
        unsigned int srcSize,
        Ipp16s*      dst,
        WTPhase      phase)
{
    IppiSize roiSize = {srcSize, 1};

    ippiWTInvRow_D97_JPEG2K_16s_C1R(
        low , sizeof(Ipp16s) * srcSize,
        high, sizeof(Ipp16s) * srcSize,
        roiSize,
        dst , sizeof(Ipp16s) * 2 * srcSize,
        phase);
}

inline void WT97InvRow(
  const Ipp32s*      low,
  const Ipp32s*      high,
        unsigned int srcSize,
        Ipp32s*      dst,
        WTPhase      phase)
{
    IppiSize roiSize = {srcSize, 1};

    ippiWTInvRow_D97_JPEG2K_32s_C1R(
        low , sizeof(Ipp32s) * srcSize,
        high, sizeof(Ipp32s) * srcSize,
        roiSize,
        dst , sizeof(Ipp32s) * 2 * srcSize,
        phase);
}

inline void WT97InvRow(
  const Ipp32f*      low,
  const Ipp32f*      high,
        unsigned int srcSize,
        Ipp32f*      dst,
        WTPhase      phase)
{
    IppiSize roiSize = {srcSize, 1};

    ippiWTInvRow_D97_JPEG2K_32f_C1R(
        low , sizeof(Ipp32f) * srcSize,
        high, sizeof(Ipp32f) * srcSize,
        roiSize,
        dst , sizeof(Ipp32f) * 2 * srcSize,
        phase);
}

inline void WT97FwdColLift(
  const Ipp16s* src0,
  const Ipp16s* src1,
  const Ipp16s* src2,
        Ipp16s* srcDstLow0,
        Ipp16s* dstLow1,
        Ipp16s* srcDstHigh0,
        Ipp16s* srcDstHigh1,
        Ipp16s* dstHigh2,
        int     width)
{
    ippiWTFwdColLift_D97_JPEG2K_16s_C1(src0, src1, src2, srcDstLow0, dstLow1, srcDstHigh0, srcDstHigh1, dstHigh2, width);
}

inline void WT97FwdColLift(
  const Ipp32s* src0,
  const Ipp32s* src1,
  const Ipp32s* src2,
        Ipp32s* srcDstLow0,
        Ipp32s* dstLow1,
        Ipp32s* srcDstHigh0,
        Ipp32s* srcDstHigh1,
        Ipp32s* dstHigh2,
        int     width)
{
    ippiWTFwdColLift_D97_JPEG2K_32s_C1(src0, src1, src2, srcDstLow0, dstLow1, srcDstHigh0, srcDstHigh1, dstHigh2, width);
}

inline void WT97FwdColLift(
  const Ipp32f* src0,
  const Ipp32f* src1,
  const Ipp32f* src2,
        Ipp32f* srcDstLow0,
        Ipp32f* dstLow1,
        Ipp32f* srcDstHigh0,
        Ipp32f* srcDstHigh1,
        Ipp32f* dstHigh2,
        int     width)
{
    ippiWTFwdColLift_D97_JPEG2K_32f_C1(src0, src1, src2, srcDstLow0, dstLow1, srcDstHigh0, srcDstHigh1, dstHigh2, width);
}

inline void WT97InvColLift(
  const Ipp16s* srcLow0,
  const Ipp16s* srcHigh0,
  const Ipp16s* srcHigh1,
  const Ipp16s* src0,
        Ipp16s* srcDst1,
        Ipp16s* srcDst2,
        Ipp16s* dst3,
        Ipp16s* dst4,
        int     width)
{
    ippiWTInvColLift_D97_JPEG2K_16s_C1(srcLow0, srcHigh0, srcHigh1, src0, srcDst1, srcDst2, dst3, dst4, width);
}

inline void WT97InvColLift(
  const Ipp32s* srcLow0,
  const Ipp32s* srcHigh0,
  const Ipp32s* srcHigh1,
  const Ipp32s* src0,
        Ipp32s* srcDst1,
        Ipp32s* srcDst2,
        Ipp32s* dst3,
        Ipp32s* dst4,
        int     width)
{
    ippiWTInvColLift_D97_JPEG2K_32s_C1(srcLow0, srcHigh0, srcHigh1, src0, srcDst1, srcDst2, dst3, dst4, width);
}

inline void WT97InvColLift(
  const Ipp32f* srcLow0,
  const Ipp32f* srcHigh0,
  const Ipp32f* srcHigh1,
  const Ipp32f* src0,
        Ipp32f* srcDst1,
        Ipp32f* srcDst2,
        Ipp32f* dst3,
        Ipp32f* dst4,
        int     width)
{
    ippiWTInvColLift_D97_JPEG2K_32f_C1(srcLow0, srcHigh0, srcHigh1, src0, srcDst1, srcDst2, dst3, dst4, width);
}

#endif // __PP_H__

