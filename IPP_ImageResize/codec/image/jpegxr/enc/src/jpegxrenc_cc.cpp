/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#include "ippcore.h"
#include "jpegxrenc_cc.h"


#define CONVERT_RGB(r, g, b) (b -= r, r += ((b + 1) >> 1) - g, g += ((r + 0) >> 1))
#define CONVERT_CMYK(c, m, y, k) (y -= c, c += ((y + 1) >> 1) - m, m += (c >> 1) - k, k += ((m + 1) >> 1))
#define SCALE16F(x) (Ipp32s)((x & 0x8000)?((Ipp32s)x | 0x80000000):x)


static Ipp32s scalingFloat32(Ipp32f fValue, Ipp8s chExp, Ipp8u chMan)
{
  Ipp32s  iInt = 0;
  Ipp32f* pInt = (Ipp32f*)&iInt;
  Ipp32s  iSign, iTemp, iMan, iExp;

  if(fValue != 0)
  {
    *pInt = fValue;
    iSign = iInt >> 31;
    iTemp = (iInt >> 23) & 0x000000ff;
    iMan  = (iInt & 0x007fffff) | 0x800000;
    if(iTemp == 0)
    {
      iMan ^= 0x800000;
      iTemp++;
    }
    iExp = iTemp - 127 + chExp;

    if(iExp <= 1)
    {
      if(iExp < 1)
        iMan >>= (1 - iExp);

      iExp = 1;
      if((iMan & 0x800000) == 0)
        iExp = 0;
    }
    iMan &= 0x007fffff;

    return (((iExp << chMan) + ((iMan + (1 << (23 - chMan - 1))) >> (23 - chMan))) ^ iSign) - iSign;
  }
  else
    return 0;
}


static Ipp32s unpackRGBE(Ipp8u iSrc, Ipp8u iExp)
{
  Ipp32s iAppend = 1;

  if(iExp == 0 || iSrc == 0)
    return 0;

  iExp--;
  while(((iSrc & 0x80) == 0) && (iExp > 0))
  {
    iSrc = (Ipp8u)((iSrc << 1) + iAppend);
    iAppend = 0;
    iExp--;
  }

  if(iExp == 0)
    return iSrc;
  else
  {
    iExp++;
    return (iSrc & 0x7f) + (iExp << 7);
  }
}


void ccGrayToY_8u16s(Ipp8u* pSrc, Ipp32u srcStep, Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp8u*  pSrcRow = pSrc;
  Ipp16s* pDstRow = pDst;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      pDstRow[j] = (Ipp16s)((pSrcRow[j] << scaleFactor) - iBias);
    }
    pSrcRow = (Ipp8u*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp16s*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccGrayToY_8u32s(Ipp8u* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp8u*  pSrcRow = pSrc;
  Ipp32s* pDstRow = pDst;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      pDstRow[j] = (pSrcRow[j] << scaleFactor) - iBias;
    }
    pSrcRow = (Ipp8u*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp32s*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccGrayToY_16u32s(Ipp16u* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp16u* pSrcRow = pSrc;
  Ipp32s* pDstRow = pDst;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      pDstRow[j] = ((pSrcRow[j] >> shift) << scaleFactor) - iBias;
    }
    pSrcRow = (Ipp16u*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp32s*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccGrayToY_16s32s(Ipp16s* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp16s* pSrcRow = pSrc;
  Ipp32s* pDstRow = pDst;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      pDstRow[j] = ((pSrcRow[j] >> shift) << scaleFactor) - iBias;
    }
    pSrcRow = (Ipp16s*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp32s*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccGrayToY_16f32s(Ipp16u* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp16u* pSrcRow = pSrc;
  Ipp32s* pDstRow = pDst;
  Ipp32s g;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      g = SCALE16F(pSrcRow[j]);
      pDstRow[j] = (g << scaleFactor) - iBias;
    }
    pSrcRow = (Ipp16u*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp32s*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccGrayToY_32s32s(Ipp32s* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pSrcRow = pSrc;
  Ipp32s* pDstRow = pDst;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      pDstRow[j] = ((pSrcRow[j] >> shift) << scaleFactor) - iBias;
    }
    pSrcRow = (Ipp32s*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp32s*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccGrayToY_32f32s(Ipp32f* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32f* pSrcRow = pSrc;
  Ipp32s* pDstRow = pDst;
  Ipp32s g;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      g = scalingFloat32(pSrcRow[j], exp, mant);
      pDstRow[j] = (g >> scaleFactor) - iBias;
    }
    pSrcRow = (Ipp32f*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp32s*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccBGRToYUV_8u16s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      r = pSrcPix[2];
      g = pSrcPix[1];
      b = pSrcPix[0];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = (Ipp16s)g;
      pU[j] = (Ipp16s)(-r);
      pV[j] = (Ipp16s)b;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccBGRToYUV_8u16s_C4P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[2];
      g = pSrcPix[1];
      b = pSrcPix[0];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = (Ipp16s)g;
      pU[j] = (Ipp16s)(-r);
      pV[j] = (Ipp16s)b;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccBGRToYUV_8u16s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp16s* pA = pDst[3];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[2];
      g = pSrcPix[1];
      b = pSrcPix[0];
      a = pSrcPix[3];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      a = (a << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = (Ipp16s)g;
      pU[j] = (Ipp16s)(-r);
      pV[j] = (Ipp16s)b;
      pA[j] = (Ipp16s)a;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp16s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccBGRToYUV_8u32s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      r = pSrcPix[2];
      g = pSrcPix[1];
      b = pSrcPix[0];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccBGRToYUV_8u32s_C4P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[2];
      g = pSrcPix[1];
      b = pSrcPix[0];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccBGRToYUV_8u32s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pA = pDst[3];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[2];
      g = pSrcPix[1];
      b = pSrcPix[0];
      a = pSrcPix[3];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      a = (a << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_8u16s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = (Ipp16s)g;
      pU[j] = (Ipp16s)(-r);
      pV[j] = (Ipp16s)b;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_8u16s_C4P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = (Ipp16s)g;
      pU[j] = (Ipp16s)(-r);
      pV[j] = (Ipp16s)b;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_8u16s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp16s* pA = pDst[3];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];
      a = pSrcPix[3];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      a = (a << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = (Ipp16s)g;
      pU[j] = (Ipp16s)(-r);
      pV[j] = (Ipp16s)b;
      pA[j] = (Ipp16s)a;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp16s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_8u32s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_8u32s_C4P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_8u32s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pA = pDst[3];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];
      a = pSrcPix[3];

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      a = (a << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_16u32s_C3P3(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];

      r = ((r >> shift) << scaleFactor) - iBias;
      g = ((g >> shift) << scaleFactor) - iBias;
      b = ((b >> shift) << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_16u32s_C4P4(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pA = pDst[3];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];
      a = pSrcPix[3];

      r = ((r >> shift) << scaleFactor) - iBias;
      g = ((g >> shift) << scaleFactor) - iBias;
      b = ((b >> shift) << scaleFactor) - iBias;
      a = ((a >> shift) << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_16s32s_C3P3(Ipp16s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16s* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16s*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];

      r = ((r >> shift) << scaleFactor) - iBias;
      g = ((g >> shift) << scaleFactor) - iBias;
      b = ((b >> shift) << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_16s32s_C4P3(Ipp16s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16s* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16s*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];

      r = ((r >> shift) << scaleFactor) - iBias;
      g = ((g >> shift) << scaleFactor) - iBias;
      b = ((b >> shift) << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_16s32s_C4P4(Ipp16s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pA = pDst[3];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16s* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16s*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];
      a = pSrcPix[3];

      r = ((r >> shift) << scaleFactor) - iBias;
      g = ((g >> shift) << scaleFactor) - iBias;
      b = ((b >> shift) << scaleFactor) - iBias;
      a = ((a >> shift) << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_16f32s_C3P3(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      r = SCALE16F(pSrcPix[0]);
      g = SCALE16F(pSrcPix[1]);
      b = SCALE16F(pSrcPix[2]);

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_16f32s_C4P3(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = SCALE16F(pSrcPix[0]);
      g = SCALE16F(pSrcPix[1]);
      b = SCALE16F(pSrcPix[2]);

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_16f32s_C4P4(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pA = pDst[3];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = SCALE16F(pSrcPix[0]);
      g = SCALE16F(pSrcPix[1]);
      b = SCALE16F(pSrcPix[2]);
      a = SCALE16F(pSrcPix[3]);

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      a = (a << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_32s32s_C3P3(Ipp32s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp32s* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp32s*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];

      r = ((r >> shift) << scaleFactor) - iBias;
      g = ((g >> shift) << scaleFactor) - iBias;
      b = ((b >> shift) << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_32s32s_C4P3(Ipp32s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp32s* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp32s*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];

      r = ((r >> shift) << scaleFactor) - iBias;
      g = ((g >> shift) << scaleFactor) - iBias;
      b = ((b >> shift) << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_32s32s_C4P4(Ipp32s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pA = pDst[3];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp32s* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp32s*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = pSrcPix[0];
      g = pSrcPix[1];
      b = pSrcPix[2];
      a = pSrcPix[3];

      r = ((r >> shift) << scaleFactor) - iBias;
      g = ((g >> shift) << scaleFactor) - iBias;
      b = ((b >> shift) << scaleFactor) - iBias;
      a = ((a >> shift) << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_32f32s_C3P3(Ipp32f *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp32f* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp32f*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      r = scalingFloat32(pSrcPix[0], exp, mant);
      g = scalingFloat32(pSrcPix[1], exp, mant);
      b = scalingFloat32(pSrcPix[2], exp, mant);

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_32f32s_C4P3(Ipp32f *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp32f* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp32f*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = scalingFloat32(pSrcPix[0], exp, mant);
      g = scalingFloat32(pSrcPix[1], exp, mant);
      b = scalingFloat32(pSrcPix[2], exp, mant);

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBToYUV_32f32s_C4P4(Ipp32f *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pA = pDst[3];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp32f* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp32f*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = scalingFloat32(pSrcPix[0], exp, mant);
      g = scalingFloat32(pSrcPix[1], exp, mant);
      b = scalingFloat32(pSrcPix[2], exp, mant);
      a = scalingFloat32(pSrcPix[3], exp, mant);

      r = (r << scaleFactor) - iBias;
      g = (g << scaleFactor) - iBias;
      b = (b << scaleFactor) - iBias;
      a = (a << scaleFactor) - iBias;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccRGBEToYUV_8u32s_C4P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      r = unpackRGBE(pSrcPix[0], pSrcPix[3]) << scaleFactor;
      g = unpackRGBE(pSrcPix[1], pSrcPix[3]) << scaleFactor;
      b = unpackRGBE(pSrcPix[2], pSrcPix[3]) << scaleFactor;
      CONVERT_RGB(r, g, b);

      pY[j] = g;
      pU[j] = -r;
      pV[j] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKToYUVK_8u16s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp16s* pK = pDst[3];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];

      c = (c << scaleFactor) - iBias - iRound;
      m = (m << scaleFactor) - iBias - iRound;
      y = (y << scaleFactor) - iBias - iRound;
      k = (k << scaleFactor) + iBias - iRound;
      CONVERT_CMYK(c, m, y, k);

      pY[j] = (Ipp16s)(-m);
      pU[j] = (Ipp16s)c;
      pV[j] = (Ipp16s)(-y);
      pK[j] = (Ipp16s)k;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp16s*)((Ipp8u*)pK + iStepK);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKToYUVK_8u16s_C5P5(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp16s* pK = pDst[3];
  Ipp16s* pA = pDst[4];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32u iStepA = dstStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 5)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];
      a = pSrcPix[4];

      c = (c << scaleFactor) - iBias - iRound;
      m = (m << scaleFactor) - iBias - iRound;
      y = (y << scaleFactor) - iBias - iRound;
      k = (k << scaleFactor) + iBias - iRound;
      a = (a << scaleFactor) - iBias - iRound;
      CONVERT_CMYK(c, m, y, k);

      pY[j] = (Ipp16s)(-m);
      pU[j] = (Ipp16s)c;
      pV[j] = (Ipp16s)(-y);
      pK[j] = (Ipp16s)k;
      pA[j] = (Ipp16s)a;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp16s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp16s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKToYUVK_8u32s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pK = pDst[3];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];

      c = (c << scaleFactor) - iBias - iRound;
      m = (m << scaleFactor) - iBias - iRound;
      y = (y << scaleFactor) - iBias - iRound;
      k = (k << scaleFactor) + iBias - iRound;
      CONVERT_CMYK(c, m, y, k);

      pY[j] = -m;
      pU[j] = c;
      pV[j] = -y;
      pK[j] = k;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKToYUVK_8u32s_C5P5(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pK = pDst[3];
  Ipp32s* pA = pDst[4];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32u iStepA = dstStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 5)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];
      a = pSrcPix[4];

      c = (c << scaleFactor) - iBias - iRound;
      m = (m << scaleFactor) - iBias - iRound;
      y = (y << scaleFactor) - iBias - iRound;
      k = (k << scaleFactor) + iBias - iRound;
      a = (a << scaleFactor) - iBias - iRound;
      CONVERT_CMYK(c, m, y, k);

      pY[j] = -m;
      pU[j] = c;
      pV[j] = -y;
      pK[j] = k;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKToYUVK_16u32s_C4P4(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x4000 << scaleFactor) >> shift);
  Ipp32s iRound = (scaleFactor)?4:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pK = pDst[3];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];

      c = ((c >> shift) << scaleFactor) - iBias - iRound;
      m = ((m >> shift) << scaleFactor) - iBias - iRound;
      y = ((y >> shift) << scaleFactor) - iBias - iRound;
      k = ((k >> shift) << scaleFactor) + iBias - iRound;
      CONVERT_CMYK(c, m, y, k);

      pY[j] = -m;
      pU[j] = c;
      pV[j] = -y;
      pK[j] = k;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKToYUVK_16u32s_C5P5(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x4000 << scaleFactor) >> shift);
  Ipp32s iRound = (scaleFactor)?4:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pK = pDst[3];
  Ipp32s* pA = pDst[4];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32u iStepA = dstStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 5)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];
      a = pSrcPix[4];

      c = ((c >> shift) << scaleFactor) - iBias - iRound;
      m = ((m >> shift) << scaleFactor) - iBias - iRound;
      y = ((y >> shift) << scaleFactor) - iBias - iRound;
      k = ((k >> shift) << scaleFactor) + iBias - iRound;
      a = ((a >> shift) << scaleFactor) - iBias - iRound;
      CONVERT_CMYK(c, m, y, k);

      pY[j] = -m;
      pU[j] = c;
      pV[j] = -y;
      pK[j] = k;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKDToYUVK_8u16s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp16s* pK = pDst[3];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];

      c = (c << scaleFactor) - iBias - iRound;
      m = (m << scaleFactor) - iBias - iRound;
      y = (y << scaleFactor) - iBias - iRound;
      k = (k << scaleFactor) + iBias - iRound;

      pY[j] = (Ipp16s)k;
      pU[j] = (Ipp16s)c;
      pV[j] = (Ipp16s)m;
      pK[j] = (Ipp16s)y;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp16s*)((Ipp8u*)pK + iStepK);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKDToYUVK_8u16s_C5P5(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp16s* pK = pDst[3];
  Ipp16s* pA = pDst[4];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32u iStepA = dstStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 5)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];
      a = pSrcPix[4];

      c = (c << scaleFactor) - iBias - iRound;
      m = (m << scaleFactor) - iBias - iRound;
      y = (y << scaleFactor) - iBias - iRound;
      k = (k << scaleFactor) + iBias - iRound;
      a = (a << scaleFactor) - iBias - iRound;

      pY[j] = (Ipp16s)k;
      pU[j] = (Ipp16s)c;
      pV[j] = (Ipp16s)m;
      pK[j] = (Ipp16s)y;
      pA[j] = (Ipp16s)a;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp16s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp16s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKDToYUVK_8u32s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pK = pDst[3];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];

      c = (c << scaleFactor) - iBias - iRound;
      m = (m << scaleFactor) - iBias - iRound;
      y = (y << scaleFactor) - iBias - iRound;
      k = (k << scaleFactor) + iBias - iRound;

      pY[j] = k;
      pU[j] = c;
      pV[j] = m;
      pK[j] = y;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKDToYUVK_8u32s_C5P5(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pK = pDst[3];
  Ipp32s* pA = pDst[4];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32u iStepA = dstStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 5)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];
      a = pSrcPix[4];

      c = (c << scaleFactor) - iBias - iRound;
      m = (m << scaleFactor) - iBias - iRound;
      y = (y << scaleFactor) - iBias - iRound;
      k = (k << scaleFactor) + iBias - iRound;
      a = (a << scaleFactor) - iBias - iRound;

      pY[j] = k;
      pU[j] = c;
      pV[j] = m;
      pK[j] = y;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKDToYUVK_16u32s_C4P4(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x4000 << scaleFactor) >> shift);
  Ipp32s iRound = (scaleFactor)?4:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pK = pDst[3];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];

      c = ((c >> shift) << scaleFactor) - iBias - iRound;
      m = ((m >> shift) << scaleFactor) - iBias - iRound;
      y = ((y >> shift) << scaleFactor) - iBias - iRound;
      k = ((k >> shift) << scaleFactor) + iBias - iRound;

      pY[j] = k;
      pU[j] = c;
      pV[j] = m;
      pK[j] = y;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccCMYKDToYUVK_16u32s_C5P5(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x4000 << scaleFactor) >> shift);
  Ipp32s iRound = (scaleFactor)?4:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pK = pDst[3];
  Ipp32s* pA = pDst[4];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepK = dstStep[3];
  Ipp32u iStepA = dstStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 5)
    {
      c = pSrcPix[0];
      m = pSrcPix[1];
      y = pSrcPix[2];
      k = pSrcPix[3];
      a = pSrcPix[4];

      c = ((c >> shift) << scaleFactor) - iBias - iRound;
      m = ((m >> shift) << scaleFactor) - iBias - iRound;
      y = ((y >> shift) << scaleFactor) - iBias - iRound;
      k = ((k >> shift) << scaleFactor) + iBias - iRound;
      a = ((a >> shift) << scaleFactor) - iBias - iRound;

      pY[j] = k;
      pU[j] = c;
      pV[j] = m;
      pK[j] = y;
      pA[j] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccNCHToNCH_8u16s(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp16u channels)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pC;
  Ipp8u*  pSrcRow;
  Ipp8u*  pSrcPix;
  Ipp32s  i, j, k;

  for(k = 0; k < channels; k++)
  {
    pC = pDst[k];
    pSrcRow = pSrc;
    for(i = 0; i < roiSize.height; i++)
    {
      pSrcPix = pSrcRow;
      for(j = 0; j < roiSize.width; j++, pSrcPix += channels)
      {
        pC[j] = (Ipp16s)((pSrcPix[k] << scaleFactor) - iBias);
      }
      pC = (Ipp16s*)((Ipp8u*)pC + dstStep[k]);
      pSrcRow = pSrcRow + srcStep;
    }
  }
}


void ccNCHToNCH_8u32s(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp16u channels)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pC;
  Ipp8u*  pSrcRow;
  Ipp8u*  pSrcPix;
  Ipp32s  i, j, k;

  for(k = 0; k < channels; k++)
  {
    pC = pDst[k];
    pSrcRow = pSrc;
    for(i = 0; i < roiSize.height; i++)
    {
      pSrcPix = pSrcRow;
      for(j = 0; j < roiSize.width; j++, pSrcPix += channels)
      {
        pC[j] = (pSrcPix[k] << scaleFactor) - iBias;
      }
      pC = (Ipp32s*)((Ipp8u*)pC + dstStep[k]);
      pSrcRow = pSrcRow + srcStep;
    }
  }
}


void ccNCHToNCH_16u32s(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift, Ipp16u channels)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pC;
  Ipp8u*  pSrcRow;
  Ipp16u* pSrcPix;
  Ipp32s  i, j, k;

  for(k = 0; k < channels; k++)
  {
    pC = pDst[k];
    pSrcRow = (Ipp8u*)pSrc;
    for(i = 0; i < roiSize.height; i++)
    {
      pSrcPix = (Ipp16u*)pSrcRow;
      for(j = 0; j < roiSize.width; j++, pSrcPix += channels)
      {
        pC[j] = ((pSrcPix[k] >> shift) << scaleFactor) - iBias;
      }
      pC = (Ipp32s*)((Ipp8u*)pC + dstStep[k]);
      pSrcRow = pSrcRow + srcStep;
    }
  }
}


void ccYUVToYUV_8u16s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp16s y, u, v;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      y = pSrcPix[0];
      u = pSrcPix[1];
      v = pSrcPix[2];

      pY[j] = (Ipp16s)((y << scaleFactor) - iBias);
      pU[j] = (Ipp16s)((u << scaleFactor) - iBias);
      pV[j] = (Ipp16s)((v << scaleFactor) - iBias);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccYUVToYUV_8u16s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp16s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pDst[0];
  Ipp16s* pU = pDst[1];
  Ipp16s* pV = pDst[2];
  Ipp16s* pA = pDst[3];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp16s y, u, v, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      y = pSrcPix[0];
      u = pSrcPix[1];
      v = pSrcPix[2];
      a = pSrcPix[4];

      pY[j] = (Ipp16s)((y << scaleFactor) - iBias);
      pU[j] = (Ipp16s)((u << scaleFactor) - iBias);
      pV[j] = (Ipp16s)((v << scaleFactor) - iBias);
      pA[j] = (Ipp16s)((a << scaleFactor) - iBias);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp16s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccYUVToYUV_8u32s_C3P3(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s y, u, v;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      y = pSrcPix[0];
      u = pSrcPix[1];
      v = pSrcPix[2];

      pY[j] = (y << scaleFactor) - iBias;
      pU[j] = (u << scaleFactor) - iBias;
      pV[j] = (v << scaleFactor) - iBias;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccYUVToYUV_8u32s_C4P4(Ipp8u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pA = pDst[3];
  Ipp8u*  pSrcRow = pSrc;
  Ipp8u*  pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s y, u, v, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      y = pSrcPix[0];
      u = pSrcPix[1];
      v = pSrcPix[2];
      a = pSrcPix[4];

      pY[j] = (y << scaleFactor) - iBias;
      pU[j] = (u << scaleFactor) - iBias;
      pV[j] = (v << scaleFactor) - iBias;
      pA[j] = (a << scaleFactor) - iBias;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccYUVToYUV_16u32s_C3P3(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s y, u, v;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      y = pSrcPix[0];
      u = pSrcPix[1];
      v = pSrcPix[2];

      pY[j] = ((y >> shift) << scaleFactor) - iBias;
      pU[j] = ((u >> shift) << scaleFactor) - iBias;
      pV[j] = ((v >> shift) << scaleFactor) - iBias;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccYUVToYUV_16u32s_C4P4(Ipp16u *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pA = pDst[3];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16u* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s y, u, v, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16u*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      y = pSrcPix[0];
      u = pSrcPix[1];
      v = pSrcPix[2];
      a = pSrcPix[4];

      pY[j] = ((y >> shift) << scaleFactor) - iBias;
      pU[j] = ((u >> shift) << scaleFactor) - iBias;
      pV[j] = ((v >> shift) << scaleFactor) - iBias;
      pA[j] = ((a >> shift) << scaleFactor) - iBias;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccYUVToYUV_16s32s_C3P3(Ipp16s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16s* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32s y, u, v;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16s*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 3)
    {
      y = pSrcPix[0];
      u = pSrcPix[1];
      v = pSrcPix[2];

      pY[j] = ((y >> shift) << scaleFactor) - iBias;
      pU[j] = ((u >> shift) << scaleFactor) - iBias;
      pV[j] = ((v >> shift) << scaleFactor) - iBias;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pSrcRow = pSrcRow + srcStep;
  }
}


void ccYUVToYUV_16s32s_C4P4(Ipp16s *pSrc, Ipp32u srcStep, Ipp32s *pDst[], Ipp32u dstStep[], IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pDst[0];
  Ipp32s* pU = pDst[1];
  Ipp32s* pV = pDst[2];
  Ipp32s* pA = pDst[3];
  Ipp8u*  pSrcRow = (Ipp8u*)pSrc;
  Ipp16s* pSrcPix;
  Ipp32u iStepY = dstStep[0];
  Ipp32u iStepU = dstStep[1];
  Ipp32u iStepV = dstStep[2];
  Ipp32u iStepA = dstStep[3];
  Ipp32s y, u, v, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pSrcPix = (Ipp16s*)pSrcRow;
    for(j = 0; j < roiSize.width; j++, pSrcPix += 4)
    {
      y = pSrcPix[0];
      u = pSrcPix[1];
      v = pSrcPix[2];
      a = pSrcPix[4];

      pY[j] = ((y >> shift) << scaleFactor) - iBias;
      pU[j] = ((u >> shift) << scaleFactor) - iBias;
      pV[j] = ((v >> shift) << scaleFactor) - iBias;
      pA[j] = ((a >> shift) << scaleFactor) - iBias;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pSrcRow = pSrcRow + srcStep;
  }
}

