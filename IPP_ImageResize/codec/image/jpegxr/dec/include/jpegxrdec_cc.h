/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __JPEGXRDEC_CC_H__
#define __JPEGXRDEC_CC_H__


void ccYToGray_16s8u(Ipp16s* pSrc, Ipp32u srcStep, Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYToGray_32s8u(Ipp32s* pSrc, Ipp32u srcStep, Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYToGray_32s16u(Ipp32s* pSrc, Ipp32u srcStep, Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYToGray_32s16s(Ipp32s* pSrc, Ipp32u srcStep, Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYToGray_32s16f(Ipp32s* pSrc, Ipp32u srcStep, Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYToGray_32s32s(Ipp32s* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYToGray_32s32f(Ipp32s* pSrc, Ipp32u srcStep, Ipp32f* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant);
void ccYUVToBGR_16s8u_P3C3(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToBGR_16s8u_P3C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToBGR_16s8u_P4C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToBGR_32s8u_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToBGR_32s8u_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToBGR_32s8u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToRGB_16s8u_P3C3(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToRGB_16s8u_P3C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToRGB_16s8u_P4C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToRGB_32s8u_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToRGB_32s8u_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToRGB_32s8u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToRGB_32s16u_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToRGB_32s16u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToRGB_32s16s_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToRGB_32s16s_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToRGB_32s16s_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToRGB_32s16f_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToRGB_32s16f_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToRGB_32s16f_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToRGB_32s32s_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToRGB_32s32s_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToRGB_32s32s_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToRGB_32s32f_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32f* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant);
void ccYUVToRGB_32s32f_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32f* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant);
void ccYUVToRGB_32s32f_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32f* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant);
void ccYUVToRGBE_32s8u_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVKToCMYK_16s8u_P4C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVKToCMYK_16s8u_P5C5(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVKToCMYK_32s8u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVKToCMYK_32s8u_P5C5(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVKToCMYK_32s16u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVKToCMYK_32s16u_P5C5(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVKToCMYKD_16s8u_P4C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVKToCMYKD_16s8u_P5C5(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVKToCMYKD_32s8u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVKToCMYKD_32s8u_P5C5(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVKToCMYKD_32s16u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVKToCMYKD_32s16u_P5C5(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccNCHToNCH_16s8u(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp16u channels);
void ccNCHToNCH_32s8u(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp16u channels);
void ccNCHToNCH_32s16u(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift, Ipp16u channels);
void ccYUVToYUV_16s8u_P3C3(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToYUV_16s8u_P4C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToYUV_32s8u_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToYUV_32s8u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor);
void ccYUVToYUV_32s16u_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToYUV_32s16u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToYUV_32s16s_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);
void ccYUVToYUV_32s16s_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift);


#endif // __JPEGXRDEC_CC_H__
