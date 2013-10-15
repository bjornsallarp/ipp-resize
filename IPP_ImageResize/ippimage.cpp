/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2012 Intel Corporation. All Rights Reserved.
//
*/
#include "stdafx.h"
#include "precomp.h"
#ifndef __IPPIMAGE_H__
#include "ippimage.h"
#endif
#ifndef __IPPCORE_H__
#include "ippcore.h"
#endif
#ifndef __IPPI_H__
#include "ippi.h"
#endif
#ifndef __IPPCC_H__
#include "ippcc.h"
#endif
#ifndef __UIC_IMAGE_H__
#include "uic_image.h"
#endif
#include <math.h>


#define GAMMA_COMP18      0.55555555555555555555555555555556
#define GAMMA_COMP21      0.47619047619047619047619047619048
#define GAMMA_COMP22      0.45454545454545454545454545454545
#define GAMMA_COMP24      0.41666666666666666666666666666667
#define GAMMA_RANGE22_16S 136.32525944831522280839585221153
#define GAMMA_RANGE22_32S 8724.8166046921742597373345415681
#define VRANGE_MAX_16S    0x2000
#define VRANGE_MAX_32S    0x1000000


void RGBA_FPX_to_BGRA(Ipp8u* data,int width,int height)
{
  int    i;
  int    j;
  int    pad;
  int    line_width;
  Ipp8u  r, g, b, a;
  Ipp8u* ptr;

  ptr = data;
  pad = BYTES_PAD(width,4,1);
  line_width = width * 4 + pad;

  for(i = 0; i < height; i++)
  {
    ptr = data + line_width*i;
    for(j = 0; j < width; j++)
    {
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];
      a = ptr[3];
      ptr[2] = (Ipp8u)( (r*a+1) >> 8 );
      ptr[1] = (Ipp8u)( (g*a+1) >> 8 );
      ptr[0] = (Ipp8u)( (b*a+1) >> 8 );
      ptr[3] = 0xff;
      ptr += 4;
    }
  }

  return;
} // RGBA_FPX_to_BGRA()


void RGBA_FPX_to_RGBA(Ipp8u* data,int width,int height)
{
  int    i;
  int    j;
  int    pad;
  int    line_width;
  Ipp8u  r, g, b, a;
  Ipp8u* ptr;

  ptr = data;
  pad = BYTES_PAD(width,4,1);
  line_width = width * 4 + pad;

  for(i = 0; i < height; i++)
  {
    ptr = data + line_width*i;
    for(j = 0; j < width; j++)
    {
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];
      a = ptr[3];
      ptr[0] = (Ipp8u)( (r*a+1) >> 8 );
      ptr[1] = (Ipp8u)( (g*a+1) >> 8 );
      ptr[2] = (Ipp8u)( (b*a+1) >> 8 );
      ptr[3] = 0xff;
      ptr += 4;
    }
  }

  return;
} // RGBA_FPX_to_RGBA()


void BGRA_to_RGBA(Ipp8u* data,int width,int height)
{
  int      pad;
  int      step;
  int      order[4] = {3,2,1,0};
  Ipp8u*   ptr;
  IppiSize roi;

  ptr  = data;
  pad  = BYTES_PAD(width,4,1);
  step = width * 4 + pad;

  roi.height = height;
  roi.width  = width;

  ippiSwapChannels_8u_C4IR((Ipp8u*)ptr, step, roi, order);

  return;
} // BGRA_to_RGBA()


CIppImage::CIppImage(void)
{
  m_step         = 0;
  m_nchannels    = 0;
  m_precision    = 0;
  m_order        = 0;
  m_roi.width    = 0;
  m_roi.height   = 0;
  m_format       = IF_UNSIGNED;
  m_color        = IC_UNKNOWN;
  m_sampling     = IS_444;
  m_imageData    = 0;
  m_allocated    = false;

  return;
} // ctor


CIppImage::CIppImage(IppiSize roi, int nchannels, int precision, int align)
{
  m_roi.width  = 0;
  m_roi.height = 0;
  m_imageData  = 0;
  m_allocated  = false;

  Alloc( roi, nchannels, precision, align );

  return;
} // ctor


CIppImage::CIppImage(int width, int height, int nchannels, int precision, int align)
{
  m_roi.width  = 0;
  m_roi.height = 0;
  m_imageData  = 0;
  m_allocated  = false;

  Alloc( width, height, nchannels, precision, align );

  return;
} // ctor


CIppImage::~CIppImage(void)
{
  Free();
} // dtor


int CIppImage::PixSize(void) const
{
  return (((m_precision & 255) + 7)/8)*(m_sampling == IS_444 ? m_nchannels : 1);
} // CIppImage::PixSize()


int CIppImage::Alloc(int width, int height, int nchannels, int precision, int align)
{
  IppiSize sz = { width, height };
  return Alloc( sz, nchannels, precision, align );
} // CIppImage::Alloc()


int CIppImage::Alloc(IppiSize roi, int nchannels, int precision, int align)
{
  int size;
  int du      = (((precision & 255)+7)/8);
  int newStep = roi.width * nchannels * du;

  if(align)
    newStep += BYTES_PAD(roi.width, nchannels,du);

  if( isAllocated() && Height() == roi.height && Step() == newStep )
  {
    m_roi        = roi;
    m_nchannels  = nchannels;
    m_precision  = precision;
    m_step       = newStep;
    return 0;
  }

  Free();

  m_roi        = roi;
  m_nchannels  = nchannels;
  m_precision  = precision;
  m_step       = newStep;

  size = m_step * roi.height;
  m_imageData = (Ipp8u*)ippMalloc(size);
  m_allocated = m_imageData != 0;

  return m_imageData ? 0 : -1;
} // CIppImage::Alloc()


int CIppImage::Attach(int width, int height, int nchannels, int precision, void* data, int step)
{
  IppiSize roi = {width, height};
  return Attach(roi, nchannels, precision, data, step);
} // CIppImage::Attach()


int CIppImage::Attach(IppiSize roi, int nchannels, int precision, void* data, int step)
{
  Free();

  m_roi       = roi;
  m_nchannels = nchannels;
  m_precision = precision;
  m_imageData = (Ipp8u*)data;

  if( step == 0 )
    step = m_roi.width*PixSize();

  m_step = step;

  return 0;
} // CIppImage::Attach()


int CIppImage::Free(void)
{
  m_roi.width  = 0;
  m_roi.height = 0;
  m_step       = 0;

  if(0 != m_imageData)
  {
    if( isAllocated() )
      ippFree(m_imageData);

    m_imageData = 0;
  }

  m_allocated = false;

  return 0;
} // CIppImage::Free()


int CIppImage::UicToIppColor(int uic_color)
{
  switch(uic_color)
  {
  case UIC::Grayscale:       return IC_GRAY;
  case UIC::GrayscaleAlpha:  return IC_GRAYA;
  case UIC::RGB:             return IC_RGB;
  case UIC::BGR:             return IC_BGR;
  case UIC::RGBA:            return IC_RGBA;
  case UIC::BGRA:            return IC_BGRA;
  case UIC::RGBAP:           return IC_RGBAP;
  case UIC::BGRAP:           return IC_BGRAP;
  case UIC::RGBE:            return IC_RGBE;
  case UIC::YCbCr:           return IC_YCBCR;
  case UIC::CMYK:            return IC_CMYK;
  case UIC::CMYKA:           return IC_CMYKA;
  case UIC::YCCK:            return IC_YCCK;
  }
  return IC_UNKNOWN;
} // CIppImage::UicToIppColor()


int CIppImage::IppToUicColor(int ipp_color)
{
  switch(ipp_color)
  {
  case IC_GRAY:  return UIC::Grayscale;
  case IC_GRAYA: return UIC::GrayscaleAlpha;
  case IC_RGB:   return UIC::RGB;
  case IC_BGR:   return UIC::BGR;
  case IC_RGBA:  return UIC::RGBA;
  case IC_BGRA:  return UIC::BGRA;
  case IC_RGBAP: return UIC::RGBAP;
  case IC_BGRAP: return UIC::BGRAP;
  case IC_RGBE:  return UIC::RGBE;
  case IC_YCBCR: return UIC::YCbCr;
  case IC_CMYK:  return UIC::CMYK;
  case IC_CMYKA: return UIC::CMYKA;
  case IC_YCCK:  return UIC::YCCK;
  }
  return UIC::Unknown;
} // CIppImage::IppToUicColor()


void CIppImage::FillAlpha_8u(Ipp8u iValue)
{
  Ipp32u i, j;
  Ipp8u iPixStep;
  Ipp8u iAlphaIndex;

  switch(m_color)
  {
  case IC_GRAYA:
    iPixStep = 2;
    break;
  case IC_RGB:
  case IC_BGR:
    {
    if(m_nchannels != 4)
      return;
    }
  case IC_RGBA:
  case IC_BGRA:
    iPixStep = 4;
    break;
  case IC_CMYKA:
    iPixStep = 5;
    break;

  default:
    return;
  }
  iAlphaIndex = iPixStep - 1;

  for(i = 0; i < (Ipp32u)m_roi.height; i++)
  {
    Ipp8u* pSrc = (Ipp8u*)m_imageData + m_step*i;

    for(j = 0; j < (Ipp32u)m_roi.width; j++, pSrc += iPixStep)
      pSrc[iAlphaIndex] = iValue;
  }
}


void CIppImage::GrayAlphaToRGBA_8u(CIppImage &image)
{
  Ipp32u i, j;

  for(i = 0; i < (Ipp32u)m_roi.height; i++)
  {
    Ipp8u* pSrc = (Ipp8u*)m_imageData + m_step*i;
    Ipp8u* pDst = (Ipp8u*)image + image.Step()*i;

    for(j = 0; j < (Ipp32u)m_roi.width; j++, pSrc+=2, pDst+=4)
    {
      pDst[0] = pSrc[0];
      pDst[1] = pSrc[0];
      pDst[2] = pSrc[0];
      pDst[3] = pSrc[1];
    }
  }
}


void CIppImage::GrayAlphaToRGBA_16u(CIppImage &image)
{
  Ipp32u i, j;

  for(i = 0; i < (Ipp32u)m_roi.height; i++)
  {
    Ipp16u* pSrc = (Ipp16u*)((Ipp8u*)m_imageData + m_step*i);
    Ipp16u* pDst = (Ipp16u*)((Ipp8u*)image + image.Step()*i);

    for(j = 0; j < (Ipp32u)m_roi.width; j++, pSrc+=2, pDst+=4)
    {
      pDst[0] = pSrc[0];
      pDst[1] = pSrc[0];
      pDst[2] = pSrc[0];
      pDst[3] = pSrc[1];
    }
  }
}


void CIppImage::CMYKAToRGBA_8u(CIppImage &image)
{
  Ipp32u i, j;
  Ipp32u kFactor = 2;
  Ipp8u  bCMYK = (m_nchannels == 4) ? 1 : 0;  // CMYK/CMYKA

  if(bCMYK)
  {
    for(i = 0; i < (Ipp32u)m_roi.height; i++)
    {
      Ipp8u* pSrc = (Ipp8u*)m_imageData + m_step*i;
      Ipp8u* pDst = (Ipp8u*)image + image.Step()*i;

      for(j = 0; j < (Ipp32u)m_roi.width; j++, pSrc += 4, pDst += 3)
      {
        pDst[0] = (255 - ((pSrc[0] + (pSrc[3]/kFactor))));
        pDst[1] = (255 - ((pSrc[1] + (pSrc[3]/kFactor))));
        pDst[2] = (255 - ((pSrc[2] + (pSrc[3]/kFactor))));
      }
    }
  }
  else
  {
    for(i = 0; i < (Ipp32u)m_roi.height; i++)
    {
      Ipp8u* pSrc = (Ipp8u*)m_imageData + m_step*i;
      Ipp8u* pDst = (Ipp8u*)image + image.Step()*i;

      for(j = 0; j < (Ipp32u)m_roi.width; j++, pSrc+=5, pDst+=4)
      {
        pDst[0] = (255 - ((pSrc[0] + (pSrc[3]/kFactor))));
        pDst[1] = (255 - ((pSrc[1] + (pSrc[3]/kFactor))));
        pDst[2] = (255 - ((pSrc[2] + (pSrc[3]/kFactor))));
        pDst[3] = pSrc[4];
      }
    }
  }
}


void CIppImage::CMYKAToRGBA_16u(CIppImage &image)
{
  Ipp32u kFactor = 2;
  Ipp8u  bCMYK = (m_nchannels == 4)?1:0;  // CMYK/CMYKA
  Ipp32u i, j;

  if(bCMYK)
  {
    for(i = 0; i < (Ipp32u)m_roi.height; i++)
    {
      Ipp16u* pSrc = (Ipp16u*)((Ipp8u*)m_imageData + m_step*i);
      Ipp16u* pDst = (Ipp16u*)((Ipp8u*)image + image.Step()*i);

      for(j = 0; j < (Ipp32u)m_roi.width; j++, pSrc+=4, pDst+=3)
      {
        pDst[0] = (255 - ((pSrc[0] + (pSrc[3]/kFactor))));
        pDst[1] = (255 - ((pSrc[1] + (pSrc[3]/kFactor))));
        pDst[2] = (255 - ((pSrc[2] + (pSrc[3]/kFactor))));
      }
    }
  }
  else
  {
    for(i = 0; i < (Ipp32u)m_roi.height; i++)
    {
      Ipp16u* pSrc = (Ipp16u*)((Ipp8u*)m_imageData + m_step*i);
      Ipp16u* pDst = (Ipp16u*)((Ipp8u*)image + image.Step()*i);

      for(j = 0; j < (Ipp32u)m_roi.width; j++, pSrc+=5, pDst+=4)
      {
        pDst[0] = (255 - ((pSrc[0] + (pSrc[3]/kFactor))));
        pDst[1] = (255 - ((pSrc[1] + (pSrc[3]/kFactor))));
        pDst[2] = (255 - ((pSrc[2] + (pSrc[3]/kFactor))));
        pDst[3] = pSrc[4];
      }
    }
  }
}

// Unpack RGBE to separated RGB 32f channels
void CIppImage::RGBEToRGB(CIppImage &image)
{
  float real;
  Ipp32s i, j;

  for(i = 0; i < m_roi.height; i++)
  {
    Ipp8u*  pSrc = (Ipp8u*)((Ipp8u*)m_imageData + m_step*i);
    Ipp32f* pDst = (Ipp32f*)((Ipp8u*)image + image.Step()*i);

    for(j = 0; j < m_roi.width; j++, pSrc += 4, pDst += 3)
    {
      if(pSrc[3])
      {
        real = ldexp(1.0, pSrc[3] - (Ipp8u)136);
        pDst[0] = pSrc[0] * real;
        pDst[1] = pSrc[1] * real;
        pDst[2] = pSrc[2] * real;
      }
      else
        pDst[0] = pDst[1] = pDst[2] = 0;
    }
  }
}


void CIppImage::RGBAPToRGBA_8u(CIppImage &image)
{
  Ipp32s i, j;
  Ipp8u r = 0, g = 1, b = 2;

  if(m_color == IC_BGRAP)
    r = 2, b = 0;

  for(i = 0; i < m_roi.height; i++)
  {
    Ipp8u* pSrcPtr = (Ipp8u*)((Ipp8u*)m_imageData + m_step*i);
    Ipp8u* pDstPtr = (Ipp8u*)((Ipp8u*)image + image.Step()*i);

    for(j = 0; j < m_roi.width; j++, pSrcPtr += 4, pDstPtr+= 4)
    {
      if(pSrcPtr[3] != 0)
      {
        pDstPtr[r] = (pSrcPtr[0] > pSrcPtr[3])?IPP_MAX_8U:((pSrcPtr[0]*IPP_MAX_8U)/pSrcPtr[3]);
        pDstPtr[g] = (pSrcPtr[1] > pSrcPtr[3])?IPP_MAX_8U:((pSrcPtr[1]*IPP_MAX_8U)/pSrcPtr[3]);
        pDstPtr[b] = (pSrcPtr[2] > pSrcPtr[3])?IPP_MAX_8U:((pSrcPtr[2]*IPP_MAX_8U)/pSrcPtr[3]);
        pDstPtr[3] = pSrcPtr[3];
      }
      else
      {
        pDstPtr[0] = IPP_MAX_8U;
        pDstPtr[1] = IPP_MAX_8U;
        pDstPtr[2] = IPP_MAX_8U;
        pDstPtr[3] = 0;
      }
    }
  }
}


void CIppImage::RGBAPToRGBA_16u(CIppImage &image)
{
  Ipp32s i, j;
  Ipp8u r = 0, g = 1, b = 2;

  if(m_color == IC_BGRAP)
    r = 2, b = 0;

  for(i = 0; i < m_roi.height; i++)
  {
    Ipp16u* pSrcPtr = (Ipp16u*)((Ipp8u*)m_imageData + m_step*i);
    Ipp16u* pDstPtr = (Ipp16u*)((Ipp8u*)image + image.Step()*i);

    for(j = 0; j < m_roi.width; j++, pSrcPtr += 4, pDstPtr+= 4)
    {
      if(pSrcPtr[3] != 0)
      {
        pDstPtr[r] = (Ipp16u)(pSrcPtr[0] > pSrcPtr[3])?IPP_MAX_16U:(((float)pSrcPtr[0]/pSrcPtr[3])*IPP_MAX_16U);
        pDstPtr[g] = (Ipp16u)(pSrcPtr[1] > pSrcPtr[3])?IPP_MAX_16U:(((float)pSrcPtr[1]/pSrcPtr[3])*IPP_MAX_16U);
        pDstPtr[b] = (Ipp16u)(pSrcPtr[2] > pSrcPtr[3])?IPP_MAX_16U:(((float)pSrcPtr[2]/pSrcPtr[3])*IPP_MAX_16U);
        pDstPtr[3] = pSrcPtr[3];
      }
      else
      {
        pDstPtr[0] = IPP_MAX_16U;
        pDstPtr[1] = IPP_MAX_16U;
        pDstPtr[2] = IPP_MAX_16U;
        pDstPtr[3] = 0;
      }
    }
  }
}


void CIppImage::RGBAPToRGBA_16s(CIppImage &image)
{
  Ipp32s i, j;
  Ipp8u r = 0, g = 1, b = 2;

  if(m_color == IC_BGRAP)
    r = 2, b = 0;

  for(i = 0; i < m_roi.height; i++)
  {
    Ipp16s* pSrcPtr = (Ipp16s*)((Ipp8u*)m_imageData + m_step*i);
    Ipp16s* pDstPtr = (Ipp16s*)((Ipp8u*)image + image.Step()*i);

    for(j = 0; j < m_roi.width; j++, pSrcPtr += 4, pDstPtr+= 4)
    {
      if(pSrcPtr[3] != 0)
      {
        pDstPtr[r] = (pSrcPtr[0] > pSrcPtr[3])?VRANGE_MAX_16S:(((float)pSrcPtr[0]/pSrcPtr[3])*VRANGE_MAX_16S);
        pDstPtr[g] = (pSrcPtr[1] > pSrcPtr[3])?VRANGE_MAX_16S:(((float)pSrcPtr[1]/pSrcPtr[3])*VRANGE_MAX_16S);
        pDstPtr[b] = (pSrcPtr[2] > pSrcPtr[3])?VRANGE_MAX_16S:(((float)pSrcPtr[2]/pSrcPtr[3])*VRANGE_MAX_16S);
        pDstPtr[3] = pSrcPtr[3];
      }
      else
      {
        pDstPtr[0] = VRANGE_MAX_16S;
        pDstPtr[1] = VRANGE_MAX_16S;
        pDstPtr[2] = VRANGE_MAX_16S;
        pDstPtr[3] = 0;
      }
    }
  }
}


void CIppImage::RGBAPToRGBA_32s(CIppImage &image)
{
  Ipp32s i, j;
  Ipp8u r = 0, g = 1, b = 2;

  if(m_color == IC_BGRAP)
    r = 2, b = 0;

  for(i = 0; i < m_roi.height; i++)
  {
    Ipp32s* pSrcPtr = (Ipp32s*)((Ipp8u*)m_imageData + m_step*i);
    Ipp32s* pDstPtr = (Ipp32s*)((Ipp8u*)image + image.Step()*i);

    for(j = 0; j < m_roi.width; j++, pSrcPtr += 4, pDstPtr+= 4)
    {
      if(pSrcPtr[3] != 0)
      {
        pDstPtr[r] = (pSrcPtr[0] > pSrcPtr[3])?VRANGE_MAX_32S:(((float)pSrcPtr[0]/pSrcPtr[3])*VRANGE_MAX_32S);
        pDstPtr[g] = (pSrcPtr[1] > pSrcPtr[3])?VRANGE_MAX_32S:(((float)pSrcPtr[1]/pSrcPtr[3])*VRANGE_MAX_32S);
        pDstPtr[b] = (pSrcPtr[2] > pSrcPtr[3])?VRANGE_MAX_32S:(((float)pSrcPtr[2]/pSrcPtr[3])*VRANGE_MAX_32S);
        pDstPtr[3] = pSrcPtr[3];
      }
      else
      {
        pDstPtr[0] = VRANGE_MAX_32S;
        pDstPtr[1] = VRANGE_MAX_32S;
        pDstPtr[2] = VRANGE_MAX_32S;
        pDstPtr[3] = 0;
      }
    }
  }
}


void CIppImage::RGBAPToRGBA_32f(CIppImage &image)
{
  Ipp32s i, j;
  Ipp8u r = 0, g = 1, b = 2;

  if(m_color == IC_BGRAP)
    r = 2, b = 0;

  for(i = 0; i < m_roi.height; i++)
  {
    Ipp32f* pSrcPtr = (Ipp32f*)((Ipp8u*)m_imageData + m_step*i);
    Ipp32f* pDstPtr = (Ipp32f*)((Ipp8u*)image + image.Step()*i);

    for(j = 0; j < m_roi.width; j++, pSrcPtr += 4, pDstPtr+= 4)
    {
      if(pSrcPtr[3] != 0)
      {
        pDstPtr[r] = (pSrcPtr[0] > pSrcPtr[3])?1:(pSrcPtr[0]/pSrcPtr[3]);
        pDstPtr[g] = (pSrcPtr[1] > pSrcPtr[3])?1:(pSrcPtr[1]/pSrcPtr[3]);
        pDstPtr[b] = (pSrcPtr[2] > pSrcPtr[3])?1:(pSrcPtr[2]/pSrcPtr[3]);
        pDstPtr[3] = pSrcPtr[3];
      }
      else
      {
        pDstPtr[0] = 1;
        pDstPtr[1] = 1;
        pDstPtr[2] = 1;
        pDstPtr[3] = 0;
      }
    }
  }
}


void GammaFwd_16s(Ipp16s* pSrc, int srcStep, Ipp16s* pDst, int dstStep, int channels, IppiSize roi)
{
  Ipp32s i, j, k;
  float  fGamma = (float)GAMMA_COMP22;

  for(i = 0; i < roi.height; i++)
  {
    Ipp16s* pSrcRow = (Ipp16s*)((Ipp8u*)pSrc + srcStep * i);
    Ipp16s* pDstRow = (Ipp16s*)((Ipp8u*)pDst + dstStep * i);
    for(j = 0; j < roi.width; j++, pSrcRow += channels, pDstRow += channels)
    {
      for(k = 0; k < channels; k++)
        pDstRow[k] = (Ipp16s)(pow((float)pSrcRow[k]/VRANGE_MAX_16S, fGamma)*VRANGE_MAX_16S);
    }
  }
}


void GammaFwd_32s(Ipp32s* pSrc, int srcStep, Ipp32s* pDst, int dstStep, int channels, IppiSize roi)
{
  Ipp32s i, j, k;
  float  fGamma = (float)GAMMA_COMP22;

  for(i = 0; i < roi.height; i++)
  {
    Ipp32s* pSrcRow = (Ipp32s*)((Ipp8u*)pSrc + srcStep * i);
    Ipp32s* pDstRow = (Ipp32s*)((Ipp8u*)pDst + dstStep * i);
    for(j = 0; j < roi.width; j++, pSrcRow += channels, pDstRow += channels)
    {
      for(k = 0; k < channels; k++)
        pDstRow[k] = (Ipp32s)(pow((float)pSrcRow[k]/VRANGE_MAX_32S, fGamma)*VRANGE_MAX_32S);
    }
  }
}


void GammaFwd_32f(Ipp32f* pSrc, int srcStep, Ipp32f* pDst, int dstStep, int channels, IppiSize roi)
{
  float  fGamma = (float)GAMMA_COMP22;
  Ipp32s i, j, k;

  for(i = 0; i < roi.height; i++)
  {
    Ipp32f* pSrcRow = (Ipp32f*)((Ipp8u*)pSrc + srcStep * i);
    Ipp32f* pDstRow = (Ipp32f*)((Ipp8u*)pDst + dstStep * i);
    for(j = 0; j < roi.width; j++, pSrcRow += channels, pDstRow += channels)
    {
      for(k = 0; k < channels; k++)
        pDstRow[k] = pow(pSrcRow[k], fGamma);
    }
  }
}


int CIppImage::ReduceBits16(Ipp8u* pDst, int dstStep, IppiSize roi) const
{
  IppStatus status;

  if(m_precision <= 8 || m_precision > 16)
    return -1;

  if(m_format == IF_UNSIGNED)
  {
    Ipp16u*   p1 = (Ipp16u*)m_imageData;
    Ipp16u*   pTmpRow = 0;
    Ipp16u    maxval[4];
    Ipp32u    val[4];
    Ipp8u*    dst = pDst;
    IppiSize  sz = {roi.width, 1};
    int       nBits;
    int       i;

    switch(m_nchannels)
    {
    case 1:
      status = ippiMax_16u_C1R(p1, m_step, roi, &maxval[0]); break;
    case 3:
      status = ippiMax_16u_C3R(p1, m_step, roi, maxval); break;
    case 4:
      status = ippiMax_16u_C4R(p1, m_step, roi, maxval); break;
    default:
      return -1;
    }

    if(ippStsNoErr != status)
      return -1;

    for(i = 0; i < m_nchannels; i++)
    {
      nBits = 0;

      while(maxval[i])
      {
        maxval[i] = maxval[i] >> 1;
        nBits++;
      }
      val[i] = 16 - nBits;
    }

    pTmpRow = (Ipp16u*)ippMalloc(m_step);
    if(0 == pTmpRow)
      return -1;

    for(i = 0; i < roi.height; i++)
    {
      switch(m_nchannels)
      {
      case 1:
        status = ippiLShiftC_16u_C1R(p1,m_step,val[0],pTmpRow,m_step,sz); break;
      case 3:
        status = ippiLShiftC_16u_C3R(p1,m_step,val,pTmpRow,m_step,sz); break;
      case 4:
        status = ippiLShiftC_16u_C4R(p1,m_step,val,pTmpRow,m_step,sz); break;
      default:
        ippFree(pTmpRow);
        return -1;
      }
      if(ippStsNoErr != status)
      {
        ippFree(pTmpRow);
        return -1;
      }

      switch(m_nchannels)
      {
      case 1:
        status = ippiReduceBits_16u8u_C1R(pTmpRow,m_step,dst,dstStep,sz,0,ippDitherNone,255); break;
      case 3:
        status = ippiReduceBits_16u8u_C3R(pTmpRow,m_step,dst,dstStep,sz,0,ippDitherNone,255); break;
      case 4:
        status = ippiReduceBits_16u8u_C4R(pTmpRow,m_step,dst,dstStep,sz,0,ippDitherNone,255); break;
      default:
        ippFree(pTmpRow);
        return -1;
      }
      if(ippStsNoErr != status)
      {
        ippFree(pTmpRow);
        return -1;
      }

      p1 = (Ipp16u*)((Ipp8u*)p1 + m_step);
      dst += dstStep;
    }
  }
  else if(m_format == IF_SIGNED)
  {
    Ipp16u*   p1 = (Ipp16u*)m_imageData;
    Ipp16u*   p2;
    Ipp16u*   pTmpRow = 0;
    Ipp16u    maxval[4];
    Ipp32u    val[4];
    Ipp8u*    dst = pDst;
    IppiSize  sz = {roi.width, 1};
    int       nBits;
    int       i;
    int       j;
    int       c;

    p2 = p1;

    for(i = 0; i < roi.height; i++)
    {
      for(j = 0; j < roi.width; j++)
      {
        for(c = 0; c < 1; c++)
        {
          p2[j + c] = (p2[j + c] >= 0x8000) ? p2[j + c] - 0x8000 : p2[j + c] + 0x8000;
        }
      }

      p2 = (Ipp16u*)((Ipp8u*)p2 + m_step); 
    }

    switch(m_nchannels)
    {
    case 1:
      status = ippiMax_16u_C1R(p1, m_step, roi, &maxval[0]); break;
    case 3:
      status = ippiMax_16u_C3R(p1, m_step, roi, maxval); break;
    case 4:
      status = ippiMax_16u_C4R(p1, m_step, roi, maxval); break;
    default:
      return -1;
    }

    if(ippStsNoErr != status)
      return -1;

    for(i = 0; i < m_nchannels; i++)
    {
      nBits = 0;

      while(maxval[i])
      {
        maxval[i] = maxval[i] >> 1;
        nBits++;
      }
      val[i] = 16 - nBits;
    }

    pTmpRow = (Ipp16u*)ippMalloc(m_step);
    if(0 == pTmpRow)
      return -1;

    for(i = 0; i < roi.height; i++)
    {
      switch(m_nchannels)
      {
      case 1:
        status = ippiLShiftC_16u_C1R(p1,m_step,val[0],pTmpRow,m_step,sz); break;
      case 3:
        status = ippiLShiftC_16u_C3R(p1,m_step,val,pTmpRow,m_step,sz); break;
      case 4:
        status = ippiLShiftC_16u_C4R(p1,m_step,val,pTmpRow,m_step,sz); break;
      default:
        ippFree(pTmpRow);
        return -1;
      }
      if(ippStsNoErr != status)
      {
        ippFree(pTmpRow);
        return -1;
      }

      switch(m_nchannels)
      {
      case 1:
        status = ippiReduceBits_16u8u_C1R(pTmpRow,m_step,dst,dstStep,sz,0,ippDitherNone,255); break;
      case 3:
        status = ippiReduceBits_16u8u_C3R(pTmpRow,m_step,dst,dstStep,sz,0,ippDitherNone,255); break;
      case 4:
        status = ippiReduceBits_16u8u_C4R(pTmpRow,m_step,dst,dstStep,sz,0,ippDitherNone,255); break;
      default:
        ippFree(pTmpRow);
        return -1;
      }
      if(ippStsNoErr != status)
      {
        ippFree(pTmpRow);
        return -1;
      }

      p1 = (Ipp16u*)((Ipp8u*)p1 + m_step);
      dst += dstStep;
    }
  }
  else if(m_format == IF_FIXED)
  {
    IppiSize rowROI = {m_roi.width, 1};
    Ipp16s*  pSrcRow, *pDstRow1;
    Ipp8u*   pDstRow2;
    Ipp16s   iTemp;
    Ipp32s   i, j, k;

    pDstRow1 = (Ipp16s*)ippMalloc(m_step);
    if(0 == pDstRow1)
      return -1;

    for(i = 0; i < roi.height; i++)
    {
      Ipp16s* pDstPtr1 = pDstRow1;
      pSrcRow  = (Ipp16s*)((Ipp8u*)m_imageData + m_step*i);
      pDstRow2 = (Ipp8u*)pDst + dstStep*i;

      GammaFwd_16s(pSrcRow, m_step, pDstRow1, m_step, m_nchannels, rowROI);

      for(j = 0; j < roi.width; j++, pDstPtr1 += m_nchannels, pDstRow2 += m_nchannels)
      {
        for(k = 0; k < m_nchannels; k++)
        {
          iTemp = ((float)pDstPtr1[k]/VRANGE_MAX_16S)*IPP_MAX_8U;
          pDstRow2[k] = (Ipp8u)(iTemp > IPP_MAX_8U)?IPP_MAX_8U:((iTemp < 0)?0:iTemp);
        }
      }
    }

    ippFree(pDstRow1);
  }
  else if(m_format == IF_FLOAT)
  {
    IppiSize rowROI = {m_roi.width, 1};
    Ipp16s*  pSrcRow;
    Ipp32f*  pDstRow1;
    Ipp8u*   pDstRow2;
    Ipp32s   iDstRow1Step = m_roi.width * m_nchannels * sizeof(Ipp32f);
    Ipp32s   i, j, k;

    pDstRow1 = (Ipp32f*)ippMalloc(iDstRow1Step);
    if(0 == pDstRow1)
      return -1;

    for(i = 0; i < roi.height; i++)
    {
      Ipp32s* pDstPtr1 = (Ipp32s*)pDstRow1;
      pSrcRow  = (Ipp16s*)((Ipp8u*)m_imageData + m_step*i);
      pDstRow2 = (Ipp8u*)pDst + dstStep*i;

      // 16f -> 32f
      for(j = 0; j < roi.width; j++, pDstPtr1 += m_nchannels, pSrcRow += m_nchannels)
      {
        for(k = 0; k < m_nchannels; k++)
        {
          pDstPtr1[k] = (pSrcRow[k] >> 15) << 31;
          pDstPtr1[k] |= (Ipp8u)(127 + (Ipp8u)((Ipp8u)((pSrcRow[k] >> 10) & 0x1F) - 0xF)) << 23;
          pDstPtr1[k] |= (pSrcRow[k] & 0x3FF) << 13;
        }
      }

      GammaFwd_32f(pDstRow1, iDstRow1Step, pDstRow1, iDstRow1Step, m_nchannels, rowROI);

      switch(m_nchannels)
      {
      case 1:
        status = ippiReduceBits_32f8u_C1R(pDstRow1, iDstRow1Step, pDstRow2, dstStep, rowROI, 0, ippDitherNone, 255); break;
      case 3:
        status = ippiReduceBits_32f8u_C3R(pDstRow1, iDstRow1Step, pDstRow2, dstStep, rowROI, 0, ippDitherNone, 255); break;
      case 4:
        status = ippiReduceBits_32f8u_C4R(pDstRow1, iDstRow1Step, pDstRow2, dstStep, rowROI, 0, ippDitherNone, 255); break;
      default:
        return -1;
      }
    }

    ippFree(pDstRow1);
  }

  return 0;
} // CIppImage::ReduceBits16()


int CIppImage::ReduceBits32(Ipp8u* pDst, int dstStep, IppiSize roi) const
{
  IppStatus status;

  if(m_precision <= 16 || m_precision > 32)
    return -1;

  if(m_format == IF_UNSIGNED)
  {
    Ipp32u*  pSrcRow;
    Ipp8u*   pDstRow;
    Ipp16s   iTemp;
    Ipp32s   i, j, k;

    for(i = 0; i < roi.height; i++)
    {
      pSrcRow = (Ipp32u*)((Ipp8u*)m_imageData + m_step*i);
      pDstRow = (Ipp8u*)pDst + dstStep*i;

      for(j = 0; j < roi.width; j++, pSrcRow += m_nchannels, pDstRow += m_nchannels)
      {
        for(k = 0; k < m_nchannels; k++)
        {
          iTemp = ((float)pSrcRow[k]/IPP_MAX_32U)*IPP_MAX_8U;
          pDstRow[k] = (Ipp8u)(iTemp > IPP_MAX_8U)?IPP_MAX_8U:((iTemp < 0)?0:iTemp);
        }
      }
    }
  }
  else if(m_format == IF_SIGNED)
  {
    Ipp32s*  pSrcRow;
    Ipp8u*   pDstRow;
    Ipp16s   iTemp;
    Ipp32s   i, j, k;

    for(i = 0; i < roi.height; i++)
    {
      pSrcRow = (Ipp32s*)((Ipp8u*)m_imageData + m_step*i);
      pDstRow = (Ipp8u*)pDst + dstStep*i;

      for(j = 0; j < roi.width; j++, pSrcRow += m_nchannels, pDstRow += m_nchannels)
      {
        for(k = 0; k < m_nchannels; k++)
        {
          iTemp = ((float)(pSrcRow[k] + IPP_MAX_32S + 1)/IPP_MAX_32U)*IPP_MAX_8U;
          pDstRow[k] = (Ipp8u)(iTemp > IPP_MAX_8U)?IPP_MAX_8U:((iTemp < 0)?0:iTemp);
        }
      }
    }
  }
  else if(m_format == IF_FIXED)
  {
    IppiSize rowROI = {m_roi.width, 1};
    Ipp32s*  pSrcRow, *pDstRow1;
    Ipp8u*   pDstRow2;
    Ipp16s   iTemp;
    Ipp32s   i, j, k;

    pDstRow1 = (Ipp32s*)ippMalloc(m_step);
    if(0 == pDstRow1)
      return -1;

    for(i = 0; i < roi.height; i++)
    {
      Ipp32s* pDstPtr1 = pDstRow1;
      pSrcRow  = (Ipp32s*)((Ipp8u*)m_imageData + m_step*i);
      pDstRow2 = (Ipp8u*)pDst + dstStep*i;

      GammaFwd_32s(pSrcRow, m_step, pDstRow1, m_step, m_nchannels, rowROI);

      for(j = 0; j < roi.width; j++, pDstPtr1 += m_nchannels, pDstRow2 += m_nchannels)
      {
        for(k = 0; k < m_nchannels; k++)
        {
          iTemp = ((float)pDstPtr1[k]/VRANGE_MAX_32S)*IPP_MAX_8U;
          pDstRow2[k] = (Ipp8u)(iTemp > IPP_MAX_8U)?IPP_MAX_8U:((iTemp < 0)?0:iTemp);
        }
      }
    }

    ippFree(pDstRow1);
  }
  else if(m_format == IF_FLOAT)
  {
    IppiSize rowROI = {m_roi.width, 1};
    Ipp32f*  pSrcRow, *pDstRow1;
    Ipp8u*   pDstRow2;
    Ipp32s   i;

    pDstRow1 = (Ipp32f*)ippMalloc(m_step);
    if(0 == pDstRow1)
      return -1;

    for(i = 0; i < roi.height; i++)
    {
      pSrcRow  = (Ipp32f*)((Ipp8u*)m_imageData + m_step*i);
      pDstRow2 = (Ipp8u*)pDst + dstStep*i;

      GammaFwd_32f(pSrcRow, m_step, pDstRow1, m_step, m_nchannels, rowROI);

      switch(m_nchannels)
      {
      case 1:
        status = ippiReduceBits_32f8u_C1R(pDstRow1, m_step, pDstRow2, dstStep, rowROI, 0, ippDitherNone, 255); break;
      case 3:
        status = ippiReduceBits_32f8u_C3R(pDstRow1, m_step, pDstRow2, dstStep, rowROI, 0, ippDitherNone, 255); break;
      case 4:
        status = ippiReduceBits_32f8u_C4R(pDstRow1, m_step, pDstRow2, dstStep, rowROI, 0, ippDitherNone, 255); break;
      default:
        return -1;
      }
    }

    ippFree(pDstRow1);
  }

  return 0;
} // CIppImage::ReduceBits32()


int CIppImage::Resize(Ipp8u* pDst, int dstStep, IppiSize dstRoi) const
{
  int       bufsize;
  Ipp8u*    buf;
  IppStatus status;

  IppiRect srcRect = { 0, 0, m_roi.width, m_roi.height };
  IppiRect dstRect = { 0, 0, dstRoi.width, dstRoi.height };

  double xFactor = (double)dstRoi.width  / (double)m_roi.width;
  double yFactor = (double)dstRoi.height / (double)m_roi.height;

//  (xFactor < yFactor) ? yFactor = xFactor : xFactor = yFactor;

  if(m_precision > 8)
    return -1;

  status = ippiResizeGetBufSize(srcRect,dstRect,m_nchannels,IPPI_INTER_SUPER,&bufsize);
  if(ippStsNoErr != status)
    return -1;

  buf = (Ipp8u*)ippMalloc(bufsize);
  if(0 == buf)
    return -1;

  switch(m_nchannels)
  {
  case 1:
    {
      status = ippiResizeSqrPixel_8u_C1R((Ipp8u*)m_imageData, m_roi, m_step, srcRect,
        pDst, dstStep, dstRect, xFactor, yFactor, 0.0, 0.0, IPPI_INTER_SUPER, buf);
      break;
    }

  case 3:
    {
      status = ippiResizeSqrPixel_8u_C3R((Ipp8u*)m_imageData, m_roi, m_step, srcRect,
        pDst, dstStep, dstRect, xFactor, yFactor, 0.0, 0.0, IPPI_INTER_SUPER, buf);
      break;
    }

  case 4:
    {
      status = ippiResizeSqrPixel_8u_C4R((Ipp8u*)m_imageData, m_roi, m_step, srcRect,
        pDst, dstStep, dstRect, xFactor, yFactor, 0.0, 0.0, IPPI_INTER_SUPER, buf);
      break;
    }

  default:
    {
      status = ippStsErr;
      break;
    }
  }

  if(0 != buf)
    ippFree(buf);

  return (ippStsNoErr != status);
} // CIppImage::Resize()


int CIppImage::CopyTo(Ipp8u* pDst,int dstStep,IppiSize roi) const
{
  IppStatus status;

  if(m_precision > 8)
    return -1;

  switch(m_nchannels)
  {
  case 1:
    status = ippiCopy_8u_C1R((Ipp8u*)m_imageData,m_step,pDst,dstStep,roi);
    break;

  case 3:
    status = ippiCopy_8u_C3R((Ipp8u*)m_imageData,m_step,pDst,dstStep,roi);
    break;

  case 4:
    status = ippiCopy_8u_C4R((Ipp8u*)m_imageData,m_step,pDst,dstStep,roi);
    break;

  default:
    status = ippStsErr;
    break;
  }

  return (ippStsNoErr != status);
} // CIppImage::CopyTo()



int CIppImage::CopyTo(Ipp16s* pDst,int dstStep,IppiSize roi) const
{
  IppStatus status;

  if(m_precision <= 8)
    return -1;

  switch(m_nchannels)
  {
  case 1:
    status = ippiCopy_16s_C1R((Ipp16s*)m_imageData,m_step,pDst,dstStep,roi);
    break;

  case 3:
    status = ippiCopy_16s_C3R((Ipp16s*)m_imageData,m_step,pDst,dstStep,roi);
    break;

  case 4:
    status = ippiCopy_16s_C4R((Ipp16s*)m_imageData,m_step,pDst,dstStep,roi);
    break;

  default:
    status = ippStsErr;
    break;
  }

  return (ippStsNoErr != status);
} // CIppImage::CopyTo()


int CIppImage::CopyFrom(Ipp8u* pSrc,int srcStep,IppiSize roi)
{
  IppStatus status;

  switch(m_nchannels)
  {
  case 1:
    status = ippiCopy_8u_C1R(pSrc,srcStep,(Ipp8u*)m_imageData,m_step,roi);
    break;

  case 3:
    status = ippiCopy_8u_C3R(pSrc,srcStep,(Ipp8u*)m_imageData,m_step,roi);
    break;

  case 4:
    status = ippiCopy_8u_C4R(pSrc,srcStep,(Ipp8u*)m_imageData,m_step,roi);
    break;

  default:
    status = ippStsErr;
    break;
  }

  return (ippStsNoErr != status);
} // CIppImage::CopyBits()


int CIppImage::CopyFrom(Ipp16s* pSrc,int srcStep,IppiSize roi)
{
  IppStatus status;

  switch(m_nchannels)
  {
  case 1:
    status = ippiCopy_16s_C1R(pSrc,srcStep,(Ipp16s*)m_imageData,m_step,roi);
    break;

  case 3:
    status = ippiCopy_16s_C3R(pSrc,srcStep,(Ipp16s*)m_imageData,m_step,roi);
    break;

  case 4:
    status = ippiCopy_16s_C4R(pSrc,srcStep,(Ipp16s*)m_imageData,m_step,roi);
    break;

  default:
    status = ippStsErr;
    break;;
  }

  return (ippStsNoErr != status);
} // CIppImage::CopyBits()


int CIppImage::CopyFrom(const CIppImage& img)
{
  IppiSize sz;

  if( !img.DataPtr() )
  {
    Free();
    return 0;
  }

  if( img.Width() != Width() || img.Height() != Height() ||
    img.Precision() != Precision() ||
    img.NChannels() != NChannels() )
    Alloc( img.Size(), img.NChannels(), img.Precision() );

  sz.width = Width()*PixSize();
  sz.height = Height();

  return (int)ippiCopy_8u_C1R((const Ipp8u*)img, img.Step(), DataPtr(), Step(), sz);
} // CIppImage::CopyFrom()


int CIppImage::Zero(void)
{
  IppiSize sz;

  if( !DataPtr() )
    return 0;

  sz.width = Width()*PixSize();
  sz.height = Height();

  return (int)ippiSet_8u_C1R( 0, DataPtr(), Step(), sz );
} //CIppImage::Zero()


int CIppImage::ToRGBA32(CIppImage& dstimg, bool swap_rb) const
{
  CIppImage temp;
  const CIppImage* srcimg = this;
  const Ipp8u* src;
  Ipp8u* dst;
  int srcstep, dststep;
  int b = swap_rb ? 2 : 0, r = b ^ 2;
  IppiSize size = { Width(), Height() };

  if( NChannels() != 1 || Sampling() != IS_444 )
    return -1;

  if( dstimg.Width() != Width() || dstimg.Height() != Height() ||
    dstimg.Precision() != 8 || dstimg.NChannels() != 4 )
    dstimg.Alloc( Size(), 4, 8 );

  if( Precision() > 8 )
  {
    IppiSize size1 = { Width() * NChannels(), Height() };

    temp.Alloc(Width(), Height(), NChannels(), 8);

    if( Precision() <= 16 )
    {
      CIppImage temp2;

      if( Precision() < 16 )
      {
        temp2.Alloc(Width(), Height(), NChannels(), 16);

        ippiLShiftC_16u_C1R( (const Ipp16u*)*srcimg, srcimg->Step(),
          16 - Precision(), (Ipp16u*)temp2, temp2.Step(), size1 );

        srcimg = &temp2;
      }

      ippiScale_16u8u_C1R( (const Ipp16u*)*srcimg, srcimg->Step(),
        (Ipp8u*)temp, temp.Step(), size1, ippAlgHintFast );
    }
    else if( Precision() == 32 )
      ippiScale_32f8u_C1R( (const Ipp32f*)DataPtr(), Step(), (Ipp8u*)temp,
      temp.Step(), size1, 0.f, 255.f );
    else
      return -1;

    srcimg = &temp;
  }

  src     = srcimg->DataPtr();
  srcstep = srcimg->Step();
  dst     = dstimg.DataPtr();
  dststep = dstimg.Step();

  if( srcimg->NChannels() == 1 )
  {
    const Ipp8u* srcp[] = { src, src, src, src };
    ippiCopy_8u_P4C4R( srcp, srcstep, dst, dststep, size );
  }
  else if( srcimg->NChannels() == 3 )
  {
    int dstorder[] = {b, 1, r, 3};
    ippiSwapChannels_8u_C3C4R( src, srcstep, dst, dststep, size, dstorder, 255 );
  }
  else if( srcimg->NChannels() == 4 )
  {
    int dstorder[] = {b, 1, r, 3};
    ippiCopy_8u_C4R( src, srcstep, dst, dststep, size );

    if( !swap_rb )
      ippiSwapChannels_8u_C4IR( dst, dststep, size, dstorder );
  }

  return 0;
} // CIppImage::ToRGBA32()


int CIppImage::CopyTo(CIppImage& img) const
{
  return img.CopyFrom(*this);
} // CIppImage::CopyTo()


int CIppImage::ToGray(CIppImage& img, const float* coeffs) const
{
  float coeffs0[] = { 0.114f, 0.587f, 0.299f };

  if( NChannels() == 1 )
    CopyTo(img);

  if( img.Precision() != Precision() ||
      img.NChannels() != 1 ||
      img.Width() != Width() ||
      img.Height() != Height() )
    img.Alloc( Size(), 1, Precision() );

  if( NChannels() != 3 || NChannels() != 4 )
    return -1;;

  if( !coeffs )
  {
    coeffs = coeffs0;
    if( Color() == IC_RGB )
    {
      float t = coeffs[0];
      coeffs0[0] = coeffs0[2];
      coeffs0[2] = t;
    }
  }

  if( Precision() <= 8 )
  {
    if( NChannels() == 3 )
      ippiColorToGray_8u_C3C1R((const Ipp8u*)DataPtr(), Step(),
        (Ipp8u*)img, img.Step(), Size(), coeffs );
    else
      ippiColorToGray_8u_AC4C1R((const Ipp8u*)DataPtr(), Step(),
        (Ipp8u*)img, img.Step(), Size(), coeffs );
  }
  else
  {
    if( NChannels() == 3 )
      ippiColorToGray_16u_C3C1R((const Ipp16u*)DataPtr(), Step(),
        (Ipp16u*)img, img.Step(), Size(), coeffs );
    else
      ippiColorToGray_16u_AC4C1R((const Ipp16u*)DataPtr(), Step(),
        (Ipp16u*)img, img.Step(), Size(), coeffs );
  }

  return 0;
} // CIppImage::ToGray()


int CIppImage::FromGray(const CIppImage& img, int dst_cn)
{
  if( img.NChannels() != 1 )
    return -1;

  if( dst_cn == 0 )
    dst_cn = NChannels();

  if( dst_cn != 3 && dst_cn != 4 )
    dst_cn = 3;

  if( img.Precision() != Precision() ||
      NChannels() != dst_cn ||
      img.Width() != Width() ||
      img.Height() != Height() )
    Alloc( Size(), dst_cn, Precision() );

  if( Precision() <= 8 )
  {
    const Ipp8u* plane = (const Ipp8u*)img;
    const Ipp8u* srcp[] = { plane, plane, plane, plane };

    if( NChannels() == 3 )
      ippiCopy_8u_P3C3R(srcp, img.Step(), (Ipp8u*)DataPtr(), Step(), Size() );
    else
      ippiCopy_8u_P4C4R(srcp, img.Step(), (Ipp8u*)DataPtr(), Step(), Size() );
  }
  else
  {
    const Ipp16u* plane = (const Ipp16u*)img;
    const Ipp16u* srcp[] = { plane, plane, plane, plane };

    if( NChannels() == 3 )
      ippiCopy_16u_P3C3R(srcp, img.Step(), (Ipp16u*)DataPtr(), Step(), Size() );
    else
      ippiCopy_16u_P4C4R(srcp, img.Step(), (Ipp16u*)DataPtr(), Step(), Size() );
  }

  return 0;
} // CIppImage::FromGray()


int CIppImage::SplitTo(CIppImage* planes[]) const
{
  Ipp8u* p[4] = {0,0,0,0};
  int pstep = 0;
  int i;

  if( NChannels() != 3 && NChannels() != 4 )
    return -1;

  for( i = 0; i < m_nchannels; i++ )
  {
    if(planes[i] == 0)
    {
      return -1;
    }

    if(planes[i]->Precision() != Precision() ||
      planes[i]->Width() != Width() ||
      planes[i]->Height() != Height() ||
      planes[i]->NChannels() != 1 )
      planes[i]->Alloc(Size(), 1, Precision());

    p[i] = planes[i]->DataPtr();

    if( i == 0 )
      pstep = planes[i]->Step();
    else if(planes[i]->Step() != pstep)
    {
      return -1;
    }
  }

  if( Precision() <= 8 )
  {
    if( NChannels() == 3 )
      ippiCopy_8u_C3P3R( DataPtr(), Step(), p, pstep, Size() );
    else
      ippiCopy_8u_C4P4R( DataPtr(), Step(), p, pstep, Size() );
  }
  else if( Precision() <= 16 )
  {
    Ipp16u** p16 = (Ipp16u**)(Ipp8u**)p;
    if( NChannels() == 3 )
      ippiCopy_16u_C3P3R( (const Ipp16u*)DataPtr(), Step(), p16, pstep, Size() );
    else
      ippiCopy_16u_C4P4R( (const Ipp16u*)DataPtr(), Step(), p16, pstep, Size() );
  }
  else
  {
    Ipp32f** p32 = (Ipp32f**)(Ipp8u**)p;
    if( NChannels() == 3 )
      ippiCopy_32f_C3P3R( (const Ipp32f*)DataPtr(), Step(), p32, pstep, Size() );
    else
      ippiCopy_32f_C4P4R( (const Ipp32f*)DataPtr(), Step(), p32, pstep, Size() );
  }

  return 0;
} // CIppImage::SplitTo()


int CIppImage::MergeFrom(const CIppImage* planes[], int np)
{
  const Ipp8u* p[4] = {0,0,0,0};
  int pstep = 0;
  int i;

  if( np == 0 )
    np = NChannels();

  if( np != 3 && np != 4 )
    np = 3;

  if( !planes[0] )
    return -1;

  if( planes[0]->Precision() != Precision() ||
      planes[0]->Width() != Width() ||
      planes[0]->Height() != Height() ||
      np != NChannels() )
    Alloc(planes[0]->Size(), np, Precision());

  pstep = planes[0]->Step();

  for( i = 0; i < m_nchannels; i++ )
  {
    if(!planes[i] ||
      planes[i]->Precision() != Precision() ||
      planes[i]->Width() != Width() ||
      planes[i]->Height() != Height() ||
      planes[i]->NChannels() != 1 ||
      planes[i]->Step() != pstep )
    {
      return -1;
    }
    p[i] = planes[i]->DataPtr();
  }

  if( Precision() <= 8 )
  {
    if( NChannels() == 3 )
      ippiCopy_8u_P3C3R( p, pstep, DataPtr(), Step(), Size() );
    else
      ippiCopy_8u_P4C4R( p, pstep, DataPtr(), Step(), Size() );
  }
  else if( Precision() <= 16 )
  {
    const Ipp16u** p16 = (const Ipp16u**)(const Ipp8u**)p;
    if( NChannels() == 3 )
      ippiCopy_16u_P3C3R( p16, pstep, (Ipp16u*)DataPtr(), Step(), Size() );
    else
      ippiCopy_16u_P4C4R( p16, pstep, (Ipp16u*)DataPtr(), Step(), Size() );
  }
  else
  {
    const Ipp32f** p32 = (const Ipp32f**)(const Ipp8u**)p;
    if( NChannels() == 3 )
      ippiCopy_32f_P3C3R( p32, pstep, (Ipp32f*)DataPtr(), Step(), Size() );
    else
      ippiCopy_32f_P4C4R( p32, pstep, (Ipp32f*)DataPtr(), Step(), Size() );
  }

  return 0;
} // CIppImage::MergeFrom()


int CIppImage::SwapChannels(int* order)
{
  IppStatus status = ippStsErr;

  if(m_precision <= 8)
  {
    switch (m_nchannels)
    {
    case 3:
      status = ippiSwapChannels_8u_C3IR((Ipp8u*)m_imageData,m_step, m_roi, order);
      break;

    case 4:
      status = ippiSwapChannels_8u_C4IR((Ipp8u*)m_imageData,m_step, m_roi, order);
      break;

    default:
      break;
    }
  }

  return (ippStsNoErr != status);
} // CIppImage::SwapChannels


