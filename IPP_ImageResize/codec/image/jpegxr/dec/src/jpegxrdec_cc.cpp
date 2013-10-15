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
#include "jpegxrdec_cc.h"


#define CONVERT_RGB(r, g, b) (g -= (r >> 1), r -= ((b + 1) >> 1) - g, b += r)
#define CONVERT_CMYK(c, m, y, k) (k -= ((m + 1) >> 1), m -= (c >> 1) - k, c -= ((y + 1) >> 1) - m, y += c)
#define SCALE16F(x) (Ipp16u)((x < 0)?((x & 0x7FFF) | 0x8000):(x & 0x7FFF))
#define SAT5U(x) ((x > 0x1F)?0x1F:((x < 0)?0:x))
#define SAT6U(x) ((x > 0x3F)?0x3F:((x < 0)?0:x))
#define SAT8U(x) ((Ipp8u)(((x > IPP_MAX_8U)?IPP_MAX_8U:((x < IPP_MIN_8U)?IPP_MIN_8U:x))))
#define SAT10U(x) ((x > 0x3FF)?0x3FF:((x < 0)?0:x))
#define SAT16U(x) ((Ipp16u)(((x > IPP_MAX_16U)?IPP_MAX_16U:((x < IPP_MIN_16U)?IPP_MIN_16U:x))))
#define SAT16S(x) ((Ipp16s)((x > IPP_MAX_16S)?IPP_MAX_16S:((x < IPP_MIN_16S)?IPP_MIN_16S:x)))


static Ipp32f scalingFloat32(Ipp32s iValue, Ipp8s chExp, Ipp8u chMan)
{
  Ipp32f  fFloat = 0;
  Ipp32u* pFloat = (Ipp32u*)&fFloat;
  Ipp32s  iSign, iTemp, iMan, iExp, iManShift = (1 << chMan);

  iSign = iValue >> 31;
  iTemp = (iValue ^ iSign) - iSign;
  iExp  = iTemp >> chMan;
  iMan  = (iTemp & (iManShift - 1)) | iManShift;

  if(iExp == 0)
  {
    iMan ^= iManShift;
    iExp  = 1;
  }

  iExp += (127 - chExp);
  while(iMan < iManShift && iExp > 1 && iMan > 0)
  {
    iExp--;
    iMan <<= 1;
  }
  if(iMan < iManShift)
    iExp = 0;
  else
    iMan ^= iManShift;
  iMan <<= (23 - chMan);

  *pFloat = ((iSign & 0x80000000) | (iExp << 23) | iMan);
  return fFloat;
}


static void RGBToRGBE(Ipp32s iSrc, Ipp8u *pDst, Ipp8u *pExp)
{
  if(iSrc <= 0)
    *pDst = *pExp = 0;
  else if((iSrc >> 7) > 1)
  {
    *pExp = (Ipp8u)(iSrc >> 7);
    *pDst = (iSrc & 0x7f) | 0x80;
  }
  else
  {
    *pExp = 1;
    *pDst = (Ipp8u)iSrc;
  }
}


static void packRGBE(Ipp32s r, Ipp32s g, Ipp32s b, Ipp8u *pDst)
{
  Ipp8u iExpR, iExpG, iExpB;

  RGBToRGBE(r, &pDst[0], &iExpR);
  RGBToRGBE(g, &pDst[1], &iExpG);
  RGBToRGBE(b, &pDst[2], &iExpB);

  pDst[3] = IPP_MAX(IPP_MAX(iExpR, iExpG), iExpB);

  if(pDst[3] > iExpR)
    pDst[0] = (Ipp8u)(((int)pDst[0] * 2 + 1) >> (pDst[3] - iExpR + 1));
  if(pDst[3] > iExpG)
    pDst[1] = (Ipp8u)(((int)pDst[1] * 2 + 1) >> (pDst[3] - iExpG + 1));
  if(pDst[3] > iExpB)
    pDst[2] = (Ipp8u)(((int)pDst[2] * 2 + 1) >> (pDst[3] - iExpB + 1));
}


void ccYToGray_16s8u(Ipp16s* pSrc, Ipp32u srcStep, Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pSrcRow = pSrc;
  Ipp8u* pDstRow = pDst;
  Ipp16s g;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      g = (Ipp16s)((pSrcRow[j] + iBias) >> scaleFactor);
      pDstRow[j] = SAT8U(g);
    }
    pSrcRow = (Ipp16s*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp8u*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccYToGray_32s8u(Ipp32s* pSrc, Ipp32u srcStep, Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pSrcRow = pSrc;
  Ipp8u* pDstRow = pDst;
  Ipp32s g;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      g = (pSrcRow[j] + iBias) >> scaleFactor;
      pDstRow[j] = SAT8U(g);
    }
    pSrcRow = (Ipp32s*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp8u*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccYToGray_32s16u(Ipp32s* pSrc, Ipp32u srcStep, Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pSrcRow = pSrc;
  Ipp16u* pDstRow = pDst;
  Ipp32s g;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      g = ((pSrcRow[j] + iBias) >> scaleFactor) << shift;
      pDstRow[j] = SAT16U(g);
    }
    pSrcRow = (Ipp32s*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp16u*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccYToGray_32s16s(Ipp32s* pSrc, Ipp32u srcStep, Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pSrcRow = pSrc;
  Ipp16s* pDstRow = pDst;
  Ipp32s g;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      g = ((pSrcRow[j] + iBias) >> scaleFactor) << shift;
      pDstRow[j] = SAT16S(g);
    }
    pSrcRow = (Ipp32s*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp16s*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccYToGray_32s16f(Ipp32s* pSrc, Ipp32u srcStep, Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pSrcRow = pSrc;
  Ipp16u* pDstRow = pDst;
  Ipp32s g;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      g = (pSrcRow[j] + iBias) >> scaleFactor;
      pDstRow[j] = SCALE16F(g);
    }
    pSrcRow = (Ipp32s*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp16u*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccYToGray_32s32s(Ipp32s* pSrc, Ipp32u srcStep, Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pSrcRow = pSrc;
  Ipp32s* pDstRow = pDst;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      pDstRow[j] = ((pSrcRow[j] + iBias) >> scaleFactor) << shift;
    }
    pSrcRow = (Ipp32s*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp32s*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccYToGray_32s32f(Ipp32s* pSrc, Ipp32u srcStep, Ipp32f* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pSrcRow = pSrc;
  Ipp32f* pDstRow = pDst;
  Ipp32s g;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    for(j = 0; j < roiSize.width; j++)
    {
      g = (pSrcRow[j] + iBias) >> scaleFactor;
      pDstRow[j] = scalingFloat32(g, exp, mant);
    }
    pSrcRow = (Ipp32s*)((Ipp8u*)pSrcRow + srcStep);
    pDstRow = (Ipp32f*)((Ipp8u*)pDstRow + dstStep);
  }
}


void ccYUVToBGR_16s8u_P3C3(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[2] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[0] = SAT8U(b);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToBGR_16s8u_P3C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[2] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[0] = SAT8U(b);
      pDstPix[3] = 0;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToBGR_16s8u_P4C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp16s* pA = pSrc[3];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];
      a = pA[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;
      a = (a + iBias) >> scaleFactor;

      pDstPix[2] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[0] = SAT8U(b);
      pDstPix[3] = SAT8U(a);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp16s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToBGR_32s8u_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[2] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[0] = SAT8U(b);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToBGR_32s8u_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[2] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[0] = SAT8U(b);
      pDstPix[3] = 0;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToBGR_32s8u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pA = pSrc[3];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];
      a = pA[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;
      a = (a + iBias) >> scaleFactor;

      pDstPix[2] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[0] = SAT8U(b);
      pDstPix[3] = SAT8U(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_16s8u_P3C3(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[0] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[2] = SAT8U(b);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_16s8u_P3C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[0] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[2] = SAT8U(b);
      pDstPix[3] = 0;
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_16s8u_P4C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp16s* pA = pSrc[3];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];
      a = pA[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;
      a = (a + iBias) >> scaleFactor;

      pDstPix[0] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[2] = SAT8U(b);
      pDstPix[3] = SAT8U(a);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp16s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s8u_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[0] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[2] = SAT8U(b);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s8u_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[0] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[2] = SAT8U(b);
      pDstPix[3] = 0;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s8u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pA = pSrc[3];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];
      a = pA[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;
      a = (a + iBias) >> scaleFactor;

      pDstPix[0] = SAT8U(r);
      pDstPix[1] = SAT8U(g);
      pDstPix[2] = SAT8U(b);
      pDstPix[3] = SAT8U(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s16u_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = ((r + iBias) >> scaleFactor) << shift;
      g = ((g + iBias) >> scaleFactor) << shift;
      b = ((b + iBias) >> scaleFactor) << shift;

      pDstPix[0] = SAT16U(r);
      pDstPix[1] = SAT16U(g);
      pDstPix[2] = SAT16U(b);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s16u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pA = pSrc[3];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];
      a = pA[j];

      CONVERT_RGB(r, g, b);
      r = ((r + iBias) >> scaleFactor) << shift;
      g = ((g + iBias) >> scaleFactor) << shift;
      b = ((b + iBias) >> scaleFactor) << shift;
      a = ((a + iBias) >> scaleFactor) << shift;

      pDstPix[0] = SAT16U(r);
      pDstPix[1] = SAT16U(g);
      pDstPix[2] = SAT16U(b);
      pDstPix[3] = SAT16U(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s16s_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16s* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16s*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = ((r + iBias) >> scaleFactor) << shift;
      g = ((g + iBias) >> scaleFactor) << shift;
      b = ((b + iBias) >> scaleFactor) << shift;

      pDstPix[0] = SAT16S(r);
      pDstPix[1] = SAT16S(g);
      pDstPix[2] = SAT16S(b);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s16s_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16s* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16s*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = ((r + iBias) >> scaleFactor) << shift;
      g = ((g + iBias) >> scaleFactor) << shift;
      b = ((b + iBias) >> scaleFactor) << shift;

      pDstPix[0] = SAT16S(r);
      pDstPix[1] = SAT16S(g);
      pDstPix[2] = SAT16S(b);
      pDstPix[3] = 0;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s16s_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pA = pSrc[3];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16s* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16s*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];
      a = pA[j];

      CONVERT_RGB(r, g, b);
      r = ((r + iBias) >> scaleFactor) << shift;
      g = ((g + iBias) >> scaleFactor) << shift;
      b = ((b + iBias) >> scaleFactor) << shift;
      a = ((a + iBias) >> scaleFactor) << shift;

      pDstPix[0] = SAT16S(r);
      pDstPix[1] = SAT16S(g);
      pDstPix[2] = SAT16S(b);
      pDstPix[3] = SAT16S(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s16f_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[0] = SCALE16F(r);
      pDstPix[1] = SCALE16F(g);
      pDstPix[2] = SCALE16F(b);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s16f_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[0] = SCALE16F(r);
      pDstPix[1] = SCALE16F(g);
      pDstPix[2] = SCALE16F(b);
      pDstPix[3] = 0;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s16f_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pA = pSrc[3];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];
      a = pA[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;
      a = (a + iBias) >> scaleFactor;

      pDstPix[0] = SCALE16F(r);
      pDstPix[1] = SCALE16F(g);
      pDstPix[2] = SCALE16F(b);
      pDstPix[3] = SCALE16F(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s32s_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp32s* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp32s*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = ((r + iBias) >> scaleFactor) << shift;
      g = ((g + iBias) >> scaleFactor) << shift;
      b = ((b + iBias) >> scaleFactor) << shift;

      pDstPix[0] = r;
      pDstPix[1] = g;
      pDstPix[2] = b;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s32s_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp32s* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp32s*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = ((r + iBias) >> scaleFactor) << shift;
      g = ((g + iBias) >> scaleFactor) << shift;
      b = ((b + iBias) >> scaleFactor) << shift;

      pDstPix[0] = r;
      pDstPix[1] = g;
      pDstPix[2] = b;
      pDstPix[3] = 0;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s32s_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pA = pSrc[3];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp32s* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp32s*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];
      a = pA[j];

      CONVERT_RGB(r, g, b);
      r = ((r + iBias) >> scaleFactor) << shift;
      g = ((g + iBias) >> scaleFactor) << shift;
      b = ((b + iBias) >> scaleFactor) << shift;
      a = ((a + iBias) >> scaleFactor) << shift;

      pDstPix[0] = r;
      pDstPix[1] = g;
      pDstPix[2] = b;
      pDstPix[3] = a;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s32f_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32f* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp32f* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp32f*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[0] = scalingFloat32(r, exp, mant);
      pDstPix[1] = scalingFloat32(g, exp, mant);
      pDstPix[2] = scalingFloat32(b, exp, mant);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s32f_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32f* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp32f* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp32f*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;

      pDstPix[0] = scalingFloat32(r, exp, mant);
      pDstPix[1] = scalingFloat32(g, exp, mant);
      pDstPix[2] = scalingFloat32(b, exp, mant);
      pDstPix[3] = 0;
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGB_32s32f_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp32f* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8s exp, Ipp8u mant)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pA = pSrc[3];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp32f* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s r, g, b, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp32f*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];
      a = pA[j];

      CONVERT_RGB(r, g, b);
      r = (r + iBias) >> scaleFactor;
      g = (g + iBias) >> scaleFactor;
      b = (b + iBias) >> scaleFactor;
      a = (a + iBias) >> scaleFactor;

      pDstPix[0] = scalingFloat32(r, exp, mant);
      pDstPix[1] = scalingFloat32(g, exp, mant);
      pDstPix[2] = scalingFloat32(b, exp, mant);
      pDstPix[3] = scalingFloat32(a, exp, mant);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToRGBE_32s8u_P3C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s r, g, b;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      r = -pU[j];
      g = pY[j];
      b = pV[j];

      CONVERT_RGB(r, g, b);
      r >>= scaleFactor;
      g >>= scaleFactor;
      b >>= scaleFactor;

      packRGBE(r, g, b, pDstPix);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYK_16s8u_P4C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp16s* pK = pSrc[3];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      c = pU[j];
      m = -pY[j];
      y = -pV[j];
      k = pK[j];

      CONVERT_CMYK(c, m, y, k);
      c = (c + iBias + iRound) >> scaleFactor;
      m = (m + iBias + iRound) >> scaleFactor;
      y = (y + iBias + iRound) >> scaleFactor;
      k = (k - iBias + iRound) >> scaleFactor;

      pDstPix[0] = SAT8U(c);
      pDstPix[1] = SAT8U(m);
      pDstPix[2] = SAT8U(y);
      pDstPix[3] = SAT8U(k);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp16s*)((Ipp8u*)pK + iStepK);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYK_16s8u_P5C5(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias  = (0x40 << scaleFactor);
  Ipp32s iBias2 = (0x80 << scaleFactor) + ((scaleFactor)?3:0);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp16s* pK = pSrc[3];
  Ipp16s* pA = pSrc[4];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32u iStepA = srcStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 5)
    {
      c = pU[j];
      m = -pY[j];
      y = -pV[j];
      k = pK[j];
      a = pA[j];

      CONVERT_CMYK(c, m, y, k);
      c = (c + iBias + iRound) >> scaleFactor;
      m = (m + iBias + iRound) >> scaleFactor;
      y = (y + iBias + iRound) >> scaleFactor;
      k = (k - iBias + iRound) >> scaleFactor;
      a = (a + iBias2) >> scaleFactor;

      pDstPix[0] = SAT8U(c);
      pDstPix[1] = SAT8U(m);
      pDstPix[2] = SAT8U(y);
      pDstPix[3] = SAT8U(k);
      pDstPix[4] = SAT8U(a);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp16s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp16s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYK_32s8u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pK = pSrc[3];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      c = pU[j];
      m = -pY[j];
      y = -pV[j];
      k = pK[j];

      CONVERT_CMYK(c, m, y, k);
      c = (c + iBias + iRound) >> scaleFactor;
      m = (m + iBias + iRound) >> scaleFactor;
      y = (y + iBias + iRound) >> scaleFactor;
      k = (k - iBias + iRound) >> scaleFactor;

      pDstPix[0] = SAT8U(c);
      pDstPix[1] = SAT8U(m);
      pDstPix[2] = SAT8U(y);
      pDstPix[3] = SAT8U(k);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYK_32s8u_P5C5(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias  = (0x40 << scaleFactor);
  Ipp32s iBias2 = (0x80 << scaleFactor) + ((scaleFactor)?3:0);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pK = pSrc[3];
  Ipp32s* pA = pSrc[4];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32u iStepA = srcStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 5)
    {
      c = pU[j];
      m = -pY[j];
      y = -pV[j];
      k = pK[j];
      a = pA[j];

      CONVERT_CMYK(c, m, y, k);
      c = (c + iBias + iRound) >> scaleFactor;
      m = (m + iBias + iRound) >> scaleFactor;
      y = (y + iBias + iRound) >> scaleFactor;
      k = (k - iBias + iRound) >> scaleFactor;
      a = (a + iBias2) >> scaleFactor;

      pDstPix[0] = SAT8U(c);
      pDstPix[1] = SAT8U(m);
      pDstPix[2] = SAT8U(y);
      pDstPix[3] = SAT8U(k);
      pDstPix[4] = SAT8U(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYK_32s16u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x4000 << scaleFactor) >> shift);
  Ipp32s iRound = (scaleFactor)?4:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pK = pSrc[3];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      c = pU[j];
      m = -pY[j];
      y = -pV[j];
      k = pK[j];

      CONVERT_CMYK(c, m, y, k);
      c = ((c + iBias + iRound) >> scaleFactor) << shift;
      m = ((m + iBias + iRound) >> scaleFactor) << shift;
      y = ((y + iBias + iRound) >> scaleFactor) << shift;
      k = ((k - iBias + iRound) >> scaleFactor) << shift;

      pDstPix[0] = SAT16U(c);
      pDstPix[1] = SAT16U(m);
      pDstPix[2] = SAT16U(y);
      pDstPix[3] = SAT16U(k);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYK_32s16u_P5C5(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias  = ((0x4000 << scaleFactor) >> shift);
  Ipp32s iBias2 = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);
  Ipp32s iRound = (scaleFactor)?4:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pK = pSrc[3];
  Ipp32s* pA = pSrc[4];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32u iStepA = srcStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 5)
    {
      c = pU[j];
      m = -pY[j];
      y = -pV[j];
      k = pK[j];
      a = pA[j];

      CONVERT_CMYK(c, m, y, k);
      c = ((c + iBias + iRound) >> scaleFactor) << shift;
      m = ((m + iBias + iRound) >> scaleFactor) << shift;
      y = ((y + iBias + iRound) >> scaleFactor) << shift;
      k = ((k - iBias + iRound) >> scaleFactor) << shift;
      a = ((a + iBias2) >> scaleFactor) << shift;

      pDstPix[0] = SAT16U(c);
      pDstPix[1] = SAT16U(m);
      pDstPix[2] = SAT16U(y);
      pDstPix[3] = SAT16U(k);
      pDstPix[4] = SAT16U(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYKD_16s8u_P4C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp16s* pK = pSrc[3];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      c = pU[j];
      m = pV[j];
      y = pK[j];
      k = pY[j];

      c = (c + iBias + iRound) >> scaleFactor;
      m = (m + iBias + iRound) >> scaleFactor;
      y = (y + iBias + iRound) >> scaleFactor;
      k = (k - iBias + iRound) >> scaleFactor;

      pDstPix[0] = SAT8U(c);
      pDstPix[1] = SAT8U(m);
      pDstPix[2] = SAT8U(y);
      pDstPix[3] = SAT8U(k);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp16s*)((Ipp8u*)pK + iStepK);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYKD_16s8u_P5C5(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x40 << scaleFactor);
  Ipp32s iBias2 = (0x80 << scaleFactor) + ((scaleFactor)?3:0);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp16s* pK = pSrc[3];
  Ipp16s* pA = pSrc[4];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32u iStepA = srcStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 5)
    {
      c = pU[j];
      m = pV[j];
      y = pK[j];
      k = pY[j];
      a = pA[j];

      c = (c + iBias + iRound) >> scaleFactor;
      m = (m + iBias + iRound) >> scaleFactor;
      y = (y + iBias + iRound) >> scaleFactor;
      k = (k - iBias + iRound) >> scaleFactor;
      a = (a + iBias2) >> scaleFactor;

      pDstPix[0] = SAT8U(c);
      pDstPix[1] = SAT8U(m);
      pDstPix[2] = SAT8U(y);
      pDstPix[3] = SAT8U(k);
      pDstPix[4] = SAT8U(a);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp16s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp16s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYKD_32s8u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias  = (0x40 << scaleFactor);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pK = pSrc[3];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      c = pU[j];
      m = pV[j];
      y = pK[j];
      k = pY[j];

      c = (c + iBias + iRound) >> scaleFactor;
      m = (m + iBias + iRound) >> scaleFactor;
      y = (y + iBias + iRound) >> scaleFactor;
      k = (k - iBias + iRound) >> scaleFactor;

      pDstPix[0] = SAT8U(c);
      pDstPix[1] = SAT8U(m);
      pDstPix[2] = SAT8U(y);
      pDstPix[3] = SAT8U(k);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYKD_32s8u_P5C5(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias  = (0x40 << scaleFactor);
  Ipp32s iBias2 = (0x80 << scaleFactor) + ((scaleFactor)?3:0);
  Ipp32s iRound = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pK = pSrc[3];
  Ipp32s* pA = pSrc[4];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32u iStepA = srcStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 5)
    {
      c = pU[j];
      m = pV[j];
      y = pK[j];
      k = pY[j];
      a = pA[j];

      c = (c + iBias + iRound) >> scaleFactor;
      m = (m + iBias + iRound) >> scaleFactor;
      y = (y + iBias + iRound) >> scaleFactor;
      k = (k - iBias + iRound) >> scaleFactor;
      a = (a + iBias2) >> scaleFactor;

      pDstPix[0] = SAT8U(c);
      pDstPix[1] = SAT8U(m);
      pDstPix[2] = SAT8U(y);
      pDstPix[3] = SAT8U(k);
      pDstPix[4] = SAT8U(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYKD_32s16u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x4000 << scaleFactor) >> shift);
  Ipp32s iRound = (scaleFactor)?4:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pK = pSrc[3];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32s c, m, y, k;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      c = pU[j];
      m = pV[j];
      y = pK[j];
      k = pY[j];

      c = ((c + iBias + iRound) >> scaleFactor) << shift;
      m = ((m + iBias + iRound) >> scaleFactor) << shift;
      y = ((y + iBias + iRound) >> scaleFactor) << shift;
      k = ((k - iBias + iRound) >> scaleFactor) << shift;

      pDstPix[0] = SAT16U(c);
      pDstPix[1] = SAT16U(m);
      pDstPix[2] = SAT16U(y);
      pDstPix[3] = SAT16U(k);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVKToCMYKD_32s16u_P5C5(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias  = ((0x4000 << scaleFactor) >> shift);
  Ipp32s iBias2 = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);
  Ipp32s iRound = (scaleFactor)?4:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pK = pSrc[3];
  Ipp32s* pA = pSrc[4];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepK = srcStep[3];
  Ipp32u iStepA = srcStep[4];
  Ipp32s c, m, y, k, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 5)
    {
      c = pU[j];
      m = pV[j];
      y = pK[j];
      k = pY[j];
      a = pA[j];

      c = ((c + iBias + iRound) >> scaleFactor) << shift;
      m = ((m + iBias + iRound) >> scaleFactor) << shift;
      y = ((y + iBias + iRound) >> scaleFactor) << shift;
      k = ((k - iBias + iRound) >> scaleFactor) << shift;
      a = ((a + iBias2) >> scaleFactor) << shift;

      pDstPix[0] = SAT16U(c);
      pDstPix[1] = SAT16U(m);
      pDstPix[2] = SAT16U(y);
      pDstPix[3] = SAT16U(k);
      pDstPix[4] = SAT16U(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pK = (Ipp32s*)((Ipp8u*)pK + iStepK);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccNCHToNCH_16s8u(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp16u channels)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pC;
  Ipp8u*  pDstRow;
  Ipp8u*  pDstPix;
  Ipp16s  c;
  Ipp32s  i, j, k;

  for(k = 0; k < channels; k++)
  {
    pC = pSrc[k];
    pDstRow = pDst;
    for(i = 0; i < roiSize.height; i++)
    {
      pDstPix = pDstRow;
      for(j = 0; j < roiSize.width; j++, pDstPix += channels)
      {
        c = (Ipp16s)((pC[j] + iBias) >> scaleFactor);
        pDstPix[k] = SAT8U(c);
      }
      pC = (Ipp16s*)((Ipp8u*)pC + srcStep[k]);
      pDstRow = pDstRow + dstStep;
    }
  }
}


void ccNCHToNCH_32s8u(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp16u channels)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pC;
  Ipp8u*  pDstRow;
  Ipp8u*  pDstPix;
  Ipp32s  c;
  Ipp32s  i, j, k;

  for(k = 0; k < channels; k++)
  {
    pC = pSrc[k];
    pDstRow = pDst;
    for(i = 0; i < roiSize.height; i++)
    {
      pDstPix = pDstRow;
      for(j = 0; j < roiSize.width; j++, pDstPix += channels)
      {
        c = (pC[j] + iBias) >> scaleFactor;
        pDstPix[k] = SAT8U(c);
      }
      pC = (Ipp32s*)((Ipp8u*)pC + srcStep[k]);
      pDstRow = pDstRow + dstStep;
    }
  }
}


void ccNCHToNCH_32s16u(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift, Ipp16u channels)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pC;
  Ipp8u*  pDstRow;
  Ipp16u* pDstPix;
  Ipp32s  c;
  Ipp32s  i, j, k;

  for(k = 0; k < channels; k++)
  {
    pC = pSrc[k];
    pDstRow = (Ipp8u*)pDst;
    for(i = 0; i < roiSize.height; i++)
    {
      pDstPix = (Ipp16u*)pDstRow;
      for(j = 0; j < roiSize.width; j++, pDstPix += channels)
      {
        c = ((pC[j] + iBias) >> scaleFactor) << shift;
        pDstPix[k] = SAT16U(c);
      }
      pC = (Ipp32s*)((Ipp8u*)pC + srcStep[k]);
      pDstRow = pDstRow + dstStep;
    }
  }
}


void ccYUVToYUV_16s8u_P3C3(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s y, u, v;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      y = pY[j];
      u = pU[j];
      v = pV[j];

      y = (y + iBias) >> scaleFactor;
      u = (u + iBias) >> scaleFactor;
      v = (v + iBias) >> scaleFactor;

      pDstPix[0] = SAT8U(y);
      pDstPix[1] = SAT8U(u);
      pDstPix[2] = SAT8U(v);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToYUV_16s8u_P4C4(Ipp16s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp16s* pY = pSrc[0];
  Ipp16s* pU = pSrc[1];
  Ipp16s* pV = pSrc[2];
  Ipp16s* pA = pSrc[3];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s y, u, v, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      y = pY[j];
      u = pU[j];
      v = pV[j];
      a = pA[j];

      y = (y + iBias) >> scaleFactor;
      u = (u + iBias) >> scaleFactor;
      v = (v + iBias) >> scaleFactor;
      a = (a + iBias) >> scaleFactor;

      pDstPix[0] = SAT8U(y);
      pDstPix[1] = SAT8U(u);
      pDstPix[2] = SAT8U(v);
      pDstPix[3] = SAT8U(a);
    }
    pY = (Ipp16s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp16s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp16s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp16s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToYUV_32s8u_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s y, u, v;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      y = pY[j];
      u = pU[j];
      v = pV[j];

      y = (y + iBias) >> scaleFactor;
      u = (u + iBias) >> scaleFactor;
      v = (v + iBias) >> scaleFactor;

      pDstPix[0] = SAT8U(y);
      pDstPix[1] = SAT8U(u);
      pDstPix[2] = SAT8U(v);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToYUV_32s8u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp8u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor)
{
  Ipp32s iBias = (0x80 << scaleFactor) + ((scaleFactor)?3:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pA = pSrc[3];
  Ipp8u*  pDstRow = pDst;
  Ipp8u*  pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s y, u, v, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      y = pY[j];
      u = pU[j];
      v = pV[j];
      a = pA[j];

      y = (y + iBias) >> scaleFactor;
      u = (u + iBias) >> scaleFactor;
      v = (v + iBias) >> scaleFactor;
      a = (a + iBias) >> scaleFactor;

      pDstPix[0] = SAT8U(y);
      pDstPix[1] = SAT8U(u);
      pDstPix[2] = SAT8U(v);
      pDstPix[3] = SAT8U(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToYUV_32s16u_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s y, u, v;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      y = pY[j];
      u = pU[j];
      v = pV[j];

      y = ((y + iBias) >> scaleFactor) << shift;
      u = ((u + iBias) >> scaleFactor) << shift;
      v = ((v + iBias) >> scaleFactor) << shift;

      pDstPix[0] = SAT16U(y);
      pDstPix[1] = SAT16U(u);
      pDstPix[2] = SAT16U(v);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToYUV_32s16u_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16u* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = ((0x8000 << scaleFactor) >> shift) + ((scaleFactor)?4:0);

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pA = pSrc[3];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16u* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s y, u, v, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16u*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      y = pY[j];
      u = pU[j];
      v = pV[j];
      a = pA[j];

      y = ((y + iBias) >> scaleFactor) << shift;
      u = ((u + iBias) >> scaleFactor) << shift;
      v = ((v + iBias) >> scaleFactor) << shift;
      a = ((a + iBias) >> scaleFactor) << shift;

      pDstPix[0] = SAT16U(y);
      pDstPix[1] = SAT16U(u);
      pDstPix[2] = SAT16U(v);
      pDstPix[3] = SAT16U(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToYUV_32s16s_P3C3(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16s* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32s y, u, v;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16s*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 3)
    {
      y = pY[j];
      u = pU[j];
      v = pV[j];

      y = ((y + iBias) >> scaleFactor) << shift;
      u = ((u + iBias) >> scaleFactor) << shift;
      v = ((v + iBias) >> scaleFactor) << shift;

      pDstPix[0] = SAT16S(y);
      pDstPix[1] = SAT16S(u);
      pDstPix[2] = SAT16S(v);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pDstRow = pDstRow + dstStep;
  }
}


void ccYUVToYUV_32s16s_P4C4(Ipp32s* pSrc[], Ipp32u srcStep[], Ipp16s* pDst, Ipp32u dstStep, IppiSize roiSize, Ipp8u scaleFactor, Ipp8u shift)
{
  Ipp32s iBias = (scaleFactor)?3:0;

  Ipp32s* pY = pSrc[0];
  Ipp32s* pU = pSrc[1];
  Ipp32s* pV = pSrc[2];
  Ipp32s* pA = pSrc[3];
  Ipp8u*  pDstRow = (Ipp8u*)pDst;
  Ipp16s* pDstPix;
  Ipp32u iStepY = srcStep[0];
  Ipp32u iStepU = srcStep[1];
  Ipp32u iStepV = srcStep[2];
  Ipp32u iStepA = srcStep[3];
  Ipp32s y, u, v, a;
  Ipp32s i, j;

  for(i = 0; i < roiSize.height; i++)
  {
    pDstPix = (Ipp16s*)pDstRow;
    for(j = 0; j < roiSize.width; j++, pDstPix += 4)
    {
      y = pY[j];
      u = pU[j];
      v = pV[j];
      a = pA[j];

      y = ((y + iBias) >> scaleFactor) << shift;
      u = ((u + iBias) >> scaleFactor) << shift;
      v = ((v + iBias) >> scaleFactor) << shift;
      a = ((a + iBias) >> scaleFactor) << shift;

      pDstPix[0] = SAT16S(y);
      pDstPix[1] = SAT16S(u);
      pDstPix[2] = SAT16S(v);
      pDstPix[3] = SAT16S(a);
    }
    pY = (Ipp32s*)((Ipp8u*)pY + iStepY);
    pU = (Ipp32s*)((Ipp8u*)pU + iStepU);
    pV = (Ipp32s*)((Ipp8u*)pV + iStepV);
    pA = (Ipp32s*)((Ipp8u*)pA + iStepA);
    pDstRow = pDstRow + dstStep;
  }
}
