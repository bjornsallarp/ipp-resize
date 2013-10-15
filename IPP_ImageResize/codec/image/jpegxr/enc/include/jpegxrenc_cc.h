/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __JPEGXRENC_CC_H__
#define __JPEGXRENC_CC_H__


void ccGrayToY_8u16s(Ipp8u* pSrc, Ipp32u srcStep, Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccGrayToY_8u32s(Ipp8u* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccGrayToY_16u32s(Ipp16u* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccGrayToY_16s32s(Ipp16s* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccGrayToY_16f32s(Ipp16u* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccGrayToY_32s32s(Ipp32s* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccGrayToY_32f32s(Ipp32f* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant);
void ccBGRToYUV_8u16s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccBGRToYUV_8u16s_C4P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccBGRToYUV_8u16s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccBGRToYUV_8u32s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccBGRToYUV_8u32s_C4P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccBGRToYUV_8u32s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccRGBToYUV_8u16s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccRGBToYUV_8u16s_C4P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccRGBToYUV_8u16s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccRGBToYUV_8u32s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccRGBToYUV_8u32s_C4P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccRGBToYUV_8u32s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccRGBToYUV_16u32s_C3P3(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccRGBToYUV_16u32s_C4P4(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccRGBToYUV_16s32s_C3P3(Ipp16s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccRGBToYUV_16s32s_C4P3(Ipp16s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccRGBToYUV_16s32s_C4P4(Ipp16s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccRGBToYUV_16f32s_C3P3(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccRGBToYUV_16f32s_C4P3(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccRGBToYUV_16f32s_C4P4(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccRGBToYUV_32s32s_C3P3(Ipp32s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccRGBToYUV_32s32s_C4P3(Ipp32s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccRGBToYUV_32s32s_C4P4(Ipp32s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccRGBToYUV_32f32s_C3P3(Ipp32f *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant);
void ccRGBToYUV_32f32s_C4P3(Ipp32f *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant);
void ccRGBToYUV_32f32s_C4P4(Ipp32f *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant);
void ccRGBEToYUV_8u32s_C4P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccCMYKToYUVK_8u16s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccCMYKToYUVK_8u16s_C5P5(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccCMYKToYUVK_8u32s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccCMYKToYUVK_8u32s_C5P5(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccCMYKToYUVK_16u32s_C4P4(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccCMYKToYUVK_16u32s_C5P5(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccCMYKDToYUVK_8u16s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccCMYKDToYUVK_8u16s_C5P5(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccCMYKDToYUVK_8u32s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccCMYKDToYUVK_8u32s_C5P5(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccCMYKDToYUVK_16u32s_C4P4(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccCMYKDToYUVK_16u32s_C5P5(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccNCHToNCH_8u16s(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp16u channels);
void ccNCHToNCH_8u32s(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp16u channels);
void ccNCHToNCH_16u32s(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift, Ipp16u channels);
void ccYUVToYUV_8u16s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToYUV_8u16s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToYUV_8u32s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToYUV_8u32s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToYUV_16u32s_C3P3(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToYUV_16u32s_C4P4(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToYUV_16s32s_C3P3(Ipp16s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToYUV_16s32s_C4P4(Ipp16s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);


#endif // __JPEGXRENC_CC_H__
