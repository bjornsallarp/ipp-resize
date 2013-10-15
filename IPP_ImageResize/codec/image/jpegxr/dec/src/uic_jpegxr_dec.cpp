/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2008-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_JPEGXR_DEC_H__
#include "uic_jpegxr_dec.h"
#endif
#ifndef __JPEGXRDEC_H__
#include "jpegxrdec.h"
#endif


using namespace UIC;


static const CodecVersion version =
{
  1,
  5,
  0,
  0,
  "Intel(R) UIC JPEGXR Decoder",
  "v1.6.0.0",
  __DATE__
};


static unsigned int bit_depth_table[] =
{
  8 + JXR_FLAG_BW,      //HDP_BD_1W  = 0
  8,                    //HDP_BD_8   = 1
  16,                   //HDP_BD_16  = 2
  16 + JXR_FLAG_SIGNED, //HDP_BD_16S = 3
  16 + JXR_FLAG_FLOAT,  //HDP_BD_16F = 4
  32,                   //HDP_BD_32  = 5
  32 + JXR_FLAG_SIGNED, //HDP_BD_32S = 6
  32 + JXR_FLAG_FLOAT,  //HDP_BD_32F = 7
  16 + JXR_FLAG_5,      //HDP_BD_5   = 8
  32 + JXR_FLAG_10,     //HDP_BD_10  = 9
  16 + JXR_FLAG_565,    //HDP_BD_565 = 10
  0,                    //UNKNOWN    = 11
  0,                    //UNKNOWN    = 12
  0,                    //UNKNOWN    = 13
  0,                    //UNKNOWN    = 14
  8 + JXR_FLAG_WB       //HDP_BD_1B  = 15
};


static ImageEnumColorSpace color_fmt_table[] =
{
  Grayscale,  //JXR_CF_Y          = 0
  YCbCr,      //JXR_CF_YUV420     = 1
  YCbCr,      //JXR_CF_YUV422     = 2
  YCbCr,      //JXR_CF_YUV444     = 3
  CMYK,       //JXR_CF_CMYK       = 4
  CMYK,       //JXR_CF_CMYKDIRECT = 5
  Unknown,    //JXR_CF_NCHANNEL   = 6
  RGB,        //JXR_CF_RGB        = 7
  RGBE,       //JXR_CF_RGBE       = 8
  YCbCr       //JXR_OCF_YCOCG     = 9
};


static ImageEnumSampling sample_fmt_table[] =
{
  S444,  //JXR_CF_Y          = 0
  S411,  //JXR_CF_YUV420     = 1
  S422,  //JXR_CF_YUV422     = 2
  S444,  //JXR_CF_YUV444     = 3
  S444,  //JXR_CF_CMYK       = 4
  S444,  //JXR_CF_CMYKDIRECT = 5
  S444,  //JXR_CF_NCHANNEL   = 6
  S444,  //JXR_CF_RGB        = 7
  S444   //JXR_CF_RGBE       = 8
};


class OwnJPEGXRDecoder : public CJPEGXRDecoder
{
public:
  OwnJPEGXRDecoder(void)
  {
    m_size.width  = 0;
    m_size.height = 0;
    m_nchannels   = 0;
    m_pData = 0;
    m_iStep = 0;
  }

  virtual ~OwnJPEGXRDecoder(void) {}

  ExcStatus ReadHeader(
    ImageColorSpec&        colorSpec,
    ImageSamplingGeometry& geometry)
  {
    ImageEnumColorSpace color;
    ImageEnumSampling   sampling;
    Point     origin;
    RectSize  size;
    Rect      refgrid;
    Ipp32u    iPrecision;
    Ipp32u    bIsSigned;
    Ipp32u    bIsFloat;
    Ipp32u    i;

    RET_(CJPEGXRDecoder::ReadHeader(&m_size.width, &m_size.height, &m_nchannels, &color, &sampling, &iPrecision));

    color = color_fmt_table[color];
    iPrecision = bit_depth_table[iPrecision];
    sampling = sample_fmt_table[sampling];

    bIsSigned  = JXR_FLAG_SIGNED & iPrecision;
    bIsFloat   = JXR_FLAG_FLOAT  & iPrecision;
    iPrecision = JXR_DEPTH_FIELD & iPrecision;

    switch(color)
    {
      case RGBE:
        m_nchannels = 4; break;
      case RGB:
        color = (m_bAlphaFlag)?((m_bPreAlpha)?((m_bBGR)?BGRAP:RGBAP):((m_bBGR)?BGRA:RGBA)):((m_bBGR)?BGR:RGB); break;
      case CMYK:
        color = (m_bAlphaFlag)?CMYKA:CMYK; break;
      case YCbCr:
        color = (m_bAlphaFlag)?YCCK:YCbCr; break;
      default:
          break;
    }

    origin.SetX(0);
    origin.SetY(0);

    size.SetWidth(m_size.width);
    size.SetHeight(m_size.height);

    refgrid.SetOrigin(origin);
    refgrid.SetSize(size);

    geometry.SetRefGridRect(refgrid);
    geometry.ReAlloc(m_nchannels);
    geometry.SetEnumSampling(sampling);

    colorSpec.ReAlloc(m_nchannels);
    colorSpec.SetColorSpecMethod(UIC::Enumerated);
    colorSpec.SetComponentToColorMap(UIC::Direct);
    colorSpec.SetEnumColorSpace(color);

    if(iPrecision <= 8)
    {
      for(i = 0; i < m_nchannels; i++)
        colorSpec.DataRange()[i].SetAsRange8u(IPP_MAX_8U);
    }
    else if(iPrecision <= 16)
    {
      for(i = 0; i < m_nchannels; i++)
      {
        if(bIsSigned)
          colorSpec.DataRange()[i].SetAsRange16s(IPP_MIN_16S, IPP_MAX_16S);
        else if(bIsFloat)
          colorSpec.DataRange()[i].SetAsRange16f(IPP_MAX_16U);
        else
          colorSpec.DataRange()[i].SetAsRange16u(IPP_MAX_16U);
      }
    }
    else
    {
      for(i = 0; i < m_nchannels; i++)
      {
        if(bIsSigned)
          colorSpec.DataRange()[i].SetAsRange32s(IPP_MIN_32S, IPP_MAX_32S);
        else if(bIsFloat)
          colorSpec.DataRange()[i].SetAsRange32f((Ipp32f)IPP_MIN_32S, (Ipp32f)IPP_MAX_32S);
        else
          colorSpec.DataRange()[i].SetAsRange32u(IPP_MAX_32U);
      }
    }

    return ExcStatusOk;
  } // OwnJPEGXRDecoder::ReadHeader()

  ExcStatus ReadData(
    const ImageDataPtr*   data,
    const ImageDataOrder& dataOrder)
  {
    m_pData     = data->p8u;
    m_iStep     = *dataOrder.LineStep();
    m_iUnitSize = (Ipp8u)NOfBytes(dataOrder.DataType());

    RET_(CJPEGXRDecoder::ReadData());

    return ExcStatusOk;
  } // OwnJPEGXRDecoder::ReadData()

protected:
  IppiSize    m_size;
  Ipp32u      m_nchannels;

};


JPEGXRDecoder::JPEGXRDecoder(void)
{
  m_dec = 0;
} // ctor


JPEGXRDecoder::~JPEGXRDecoder(void)
{
  Close();
} // dtor


ExcStatus JPEGXRDecoder::Init(void)
{
  if(0 == m_dec)
  {
    m_dec = new OwnJPEGXRDecoder;
    if(!m_dec)
      return ExcStatusFail;
  }

  return ExcStatusOk;
} // JPEGXRDecoder::Init()


ExcStatus JPEGXRDecoder::Close(void)
{
    if(m_dec) {
        delete m_dec;
        m_dec = NULL;
    }
  return ExcStatusOk;
} // JPEGXRDecoder::Close()


const CodecVersion& JPEGXRDecoder::GetVersion(void)
{
  return version;
} // JPEGXRDecoder::GetVersion()


Ipp32u JPEGXRDecoder::SetNOfThreads(Ipp32u iThreads)
{
  return m_dec->SetThreads(iThreads);
} // JPEGXRDecoder::SetNOfThreads()


Ipp32u JPEGXRDecoder::NOfThreads(void)
{
  return m_dec->Threads();
} // JPEGXRDecoder::NOfThreads()


ExcStatus JPEGXRDecoder::SetPlaneOffset(Ipp32u iImageOffset)
{
  return m_dec->SetPlaneOffset(iImageOffset);
} // JPEGXRDecoder::SetPlaneOffset()


ExcStatus JPEGXRDecoder::SetParams(Ipp32u iForceColor, Ipp32u iBandsLimit, Ipp32u iMultiMode)
{
  return m_dec->SetParams(iForceColor, iBandsLimit, iMultiMode);
} // JPEGXRDecoder::SetOptions()


ExcStatus JPEGXRDecoder::FreeData(void)
{
  return m_dec->KillDecoder();
} // JPEGXRDecoder::FreeData()


ExcStatus JPEGXRDecoder::AttachStream(BaseStreamInput& in)
{
  return m_dec->AttachStream(in);
} // JPEGXRDecoder::AttachStream()


ExcStatus JPEGXRDecoder::ReadFileHeader(
  Ipp32u& imageOffset,
  Ipp32u& alphaOffset,
  Ipp8u&  preAlpha)
{
  return m_dec->ReadFileHeader(imageOffset, alphaOffset, preAlpha);
} // JPEGXRDecoder::ReadHeader()


ExcStatus JPEGXRDecoder::ReadHeader(
  ImageColorSpec&        colorSpec,
  ImageSamplingGeometry& geometry)
{
  return m_dec->ReadHeader(colorSpec, geometry);
} // JPEGXRDecoder::ReadHeader()


ExcStatus JPEGXRDecoder::ReadData(
  const ImageDataPtr*   data,
  const ImageDataOrder& dataOrder)
{
  return m_dec->ReadData(data, dataOrder);
} // JPEGXRDecoder::ReadData()

