/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __IPPIMAGE_H__
#define __IPPIMAGE_H__

#include "ippdefs.h"


#define ALIGN_VALUE (sizeof(int) - 1)
#define BYTES_PAD(iWidth, iChannels, iDU) (((iWidth * iChannels * iDU + ALIGN_VALUE) & (~ALIGN_VALUE)) - iWidth * iChannels * iDU)


typedef enum _IM_ERROR
{
  IE_OK      =  0,
  IE_RHEADER = -1,  // can not read header
  IE_RDATA   = -2,  // can not read data
  IE_WHEADER = -3,  // can not write header
  IE_WDATA   = -4,  // can not write data
  IE_INIT    = -5,  // can not initialize codec
  IE_ASTREAM = -6,  // can not attach stream
  IE_AIMAGE  = -7,  // can not attach stream
  IE_RESET   = -8,  // can not reset decoder
  IE_ALLOC   = -9,  // can not allocate memory
  IE_PARAMS  = -10, // can not set params
  IE_UNSUP   = -11, // unsupported image format
  IE_FOPEN   = -12, // can not open file
  IE_FCLOSE  = -13, // can not close file
  IE_FSEEK   = -14, // can not seek in file
  IE_FREAD   = -15, // can not read file
  IE_FWRITE  = -16, // can not write file
  IE_PROCESS = -17, // image process error
  IE_UNDEF   = -255 // unknown error

} IM_ERROR;


typedef enum _IM_TYPE
{
  IT_UNKNOWN,
  IT_RAW,
  IT_BMP,
  IT_PNM,
  IT_JPEG,
  IT_JPEG2000,
  IT_DICOM,
  IT_DDS,
  IT_PNG,
  IT_JPEGXR,
  IT_TIFF,
  IT_END        // Formats count for console parser, always last

} IM_TYPE;


typedef enum _IM_COLOR
{
  IC_UNKNOWN,
  IC_GRAY,
  IC_GRAYA,
  IC_RGB,
  IC_BGR,
  IC_RGBA,
  IC_BGRA,
  IC_RGBAP,
  IC_BGRAP,
  IC_RGBE,
  IC_YCBCR,
  IC_CMYK,
  IC_CMYKA,
  IC_YCCK

} IM_COLOR;


typedef enum _IM_SAMPLING
{
  IS_444,
  IS_422,
  IS_244,
  IS_411,
  IS_OTHER

} IM_SAMPLING;


typedef enum _IM_FORMAT
{
  IF_UNSIGNED,
  IF_FIXED,
  IF_FLOAT,
  IF_SIGNED

} IM_FORMAT;


typedef enum
{
  KERNEL_3x3,
  KERNEL_5x5,
  KERNEL_7x7,
  KERNEL_9x9

} FILTER_KERNEL;


void RGBA_FPX_to_BGRA(Ipp8u* data,int width,int height);
void RGBA_FPX_to_RGBA(Ipp8u* data,int width,int height);
void BGRA_to_RGBA(Ipp8u* data,int width,int height);

class CIppImage
{
public:
  CIppImage(void);
  CIppImage(IppiSize roi, int nchannels, int precision, int align=0);
  CIppImage(int width, int height, int nchannels, int precision, int align=0);
  virtual ~CIppImage(void);

  IM_COLOR Color(void) const          { return m_color; }
  void Color(IM_COLOR color)          { m_color = color; }

  IM_SAMPLING Sampling(void) const    { return m_sampling; }
  void Sampling(IM_SAMPLING sampling) { m_sampling = sampling; }

  IM_FORMAT Format(void) const        { return m_format; }
  void Format(IM_FORMAT format)       { m_format = format; }

  int ComponentOrder(void) const { return m_order; }
  void ComponentOrder(int order) { m_order = order; } // 0 - pixel, 1 - plane

  int NChannels(void) const      { return m_nchannels; }
  void NChannels(int nchannels)  { m_nchannels = nchannels; }

  int Precision(void) const      { return m_precision; }
  void Precision(int precision)  { m_precision = precision; }

  IppiSize Size(void) const      { return m_roi; }

  int Width(void) const          { return m_roi.width; }
  int Height(void) const         { return m_roi.height; }
  void Width(int width)          { m_roi.width = width; }
  void Height(int height)        { m_roi.height = height; }

  int Step(void) const           { return m_step; }
  int PixSize() const;
  bool isAllocated(void) const   { return m_allocated; }

  Ipp8u* DataPtr(void)           { return (Ipp8u*)m_imageData; }
  const Ipp8u* DataPtr(void) const { return (const Ipp8u*)m_imageData; }
  int    Alloc(IppiSize roi, int nchannels, int precision, int align = 0);
  int    Alloc(int width, int height, int nchannels, int precision, int align=0);
  int    Attach(IppiSize roi, int nchannels, int precision, void* data, int step);
  int    Attach(int width, int height, int nchannels, int precision, void* data, int step);
  int    Free(void);
  int    Resize(Ipp8u*     pDst, int dstStep, IppiSize dstRoi) const;
  int    CopyFrom(const CIppImage& img);
  int    CopyTo(CIppImage& img) const;
  int    CopyFrom(Ipp8u*   pSrc, int srcStep, IppiSize roi);
  int    CopyFrom(Ipp16s*  pSrc, int srcStep, IppiSize roi);
  int    CopyTo  (Ipp8u*   pDst, int dstStep, IppiSize roi) const;
  int    CopyTo  (Ipp16s*  pDst, int dstStep, IppiSize roi) const;
  int    ReduceBits16(Ipp8u* pDst, int dstStep, IppiSize roi) const;
  int    ReduceBits32(Ipp8u* pDst, int dstStep, IppiSize roi) const;
  int    ToGray(CIppImage& img, const float* coeffs=0) const;
  int    FromGray(const CIppImage& img, int dst_cn=0);
  int    SplitTo(CIppImage* planes[]) const;
  int    MergeFrom(const CIppImage* planes[], int np=0);
  int    ToRGBA32(CIppImage& img, bool swap_rb=false) const;
  int    UicToIppColor(int uic_color);
  int    IppToUicColor(int ipp_color);
  void   FillAlpha_8u(Ipp8u iValue);
  void   GrayAlphaToRGBA_8u(CIppImage &image);
  void   GrayAlphaToRGBA_16u(CIppImage &image);
  void   CMYKAToRGBA_8u(CIppImage &image);
  void   CMYKAToRGBA_16u(CIppImage &image);
  void   RGBEToRGB(CIppImage &image);
  void   RGBAPToRGBA_8u(CIppImage &image);
  void   RGBAPToRGBA_16u(CIppImage &image);
  void   RGBAPToRGBA_16s(CIppImage &image);
  void   RGBAPToRGBA_32s(CIppImage &image);
  void   RGBAPToRGBA_32f(CIppImage &image);
  int    SwapChannels(int* order);
  int    Zero();

  operator Ipp8u*(void)  { return (Ipp8u*) m_imageData; }
  operator Ipp16s*(void) { return (Ipp16s*)m_imageData; }
  operator Ipp16u*(void) { return (Ipp16u*)m_imageData; }
  operator Ipp32f*(void) { return (Ipp32f*)m_imageData; }
  operator const Ipp8u*(void) const { return (const Ipp8u*) m_imageData; }
  operator const Ipp16s*(void) const { return (const Ipp16s*)m_imageData; }
  operator const Ipp16u*(void) const { return (const Ipp16u*)m_imageData; }
  operator const Ipp32f*(void) const { return (const Ipp32f*)m_imageData; }

protected:
  IppiSize    m_roi;
  IM_COLOR    m_color;
  IM_FORMAT   m_format;
  IM_SAMPLING m_sampling;
  int         m_order;
  int         m_nchannels;
  int         m_step;
  int         m_precision;
  void*       m_imageData;
  bool        m_allocated;
};

#endif // __IPPIMAGE_H__

