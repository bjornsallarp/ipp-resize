/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning ( disable : 4100 )
#endif
#ifndef __UIC_JPEGXR_ENC_H__
#include "uic_jpegxr_enc.h"
#endif
#ifndef __JPEGXRENC_H__
#include "jpegxrenc.h"
#endif


using namespace UIC;


static const CodecVersion version =
{
  1,
  0,
  0,
  0,
  "Intel(R) UIC JPEGXR Encoder",
  "v1.2.0.0",
  __DATE__
};


static JXR_BIT_DEPTH jxrBDepthMap[] =
{
  JXR_BD_8,
  JXR_BD_8,
  JXR_BD_16,
  JXR_BD_16S,
  JXR_BD_16F,
  JXR_BD_32S,
  JXR_BD_32S,
  JXR_BD_32F
};


class OwnJPEGXREncoder : public CJPEGXREncoder
{
public:
  OwnJPEGXREncoder(void) {}
  ~OwnJPEGXREncoder(void) {}

  ExcStatus AttachImage(const Image& image)
  {
    Rect rect   = image.Buffer().BufferFormat().SamplingGeometry().RefGridRect();
    m_iWidth    = rect.Width();
    m_iHeight   = rect.Height();
    m_pData     = image.Buffer().DataPtr()->p8u;
    m_iStep     = *image.Buffer().BufferFormat().DataOrder().LineStep();
    m_iChannels = image.Buffer().BufferFormat().SamplingGeometry().NOfComponents();
    m_iColor    = image.ColorSpec().EnumColorSpace();
    m_iBitDepth = jxrBDepthMap[image.ColorSpec().DataRange()->DataType()];
    m_iUnitSize = NOfBytes(image.ColorSpec().DataRange()->DataType());

    if(m_iColor == BGRAP || m_iColor == RGBAP || m_iColor == BGRA || m_iColor == RGBA || m_iColor == CMYKA || m_iColor == YCCK)
      m_bAlphaFlag = 1;
    else
      m_bAlphaFlag = 0;

    if(m_iColor == BGRAP || m_iColor == RGBAP)
      m_bPreAlpha = 1;
    else
      m_bPreAlpha = 0;

    if((m_iColor == BGR || m_iColor == RGB) && m_iChannels == 4)
    {
      m_iChannels = 3;
      m_bFakeAlpha = 1;
    }

    switch(m_iColor)
    {
    case BGR:
    case BGRA:
    case BGRAP:
      m_bBGR = 1;
    case RGB:
    case RGBA:
    case RGBAP:
      if(m_iSampling == 1)
        m_pImagePlane->iColorFormat = JXR_ICF_YUV422;
      else if(m_iSampling == 2)
        m_pImagePlane->iColorFormat = JXR_ICF_YUV420;
      else
        m_pImagePlane->iColorFormat = JXR_ICF_YUV444;
      m_pImagePlane->iChannels = 3;
      m_iSrcColorFormat = JXR_OCF_RGB;
      break;
    case RGBE:
      m_iChannels = 3;
      m_pImagePlane->iColorFormat = JXR_ICF_YUV444;
      m_pImagePlane->iChannels = 3;
      m_iSrcColorFormat = JXR_OCF_RGBE;
      break;
    case Grayscale:
      m_pImagePlane->iColorFormat = JXR_ICF_Y;
      m_pImagePlane->iChannels = 1;
      m_iSrcColorFormat = JXR_OCF_Y;
      break;
    case YCbCr:
    case YCCK:
      m_pImagePlane->iColorFormat = JXR_ICF_YUV444;
      m_pImagePlane->iChannels = 3;
      m_iSrcColorFormat = JXR_OCF_YUV444;
      break;
    case CMYK:
    case CMYKA:
      m_pImagePlane->iColorFormat = JXR_ICF_YUVK;
      m_pImagePlane->iChannels = 4;
      m_iSrcColorFormat = ((m_bCMYKD)?JXR_OCF_CMYKD:JXR_OCF_CMYK);
      break;
    case Unknown:
      m_pImagePlane->iColorFormat = JXR_ICF_NCH;
      m_pImagePlane->iChannels = m_iChannels;
      m_iSrcColorFormat = JXR_OCF_NCH;
      break;
    default:
      return ExcStatusFail;
    }

    if(m_iBitDepth == JXR_BD_8 && m_bAlphaFlag)
      m_bBGR = 1;

    if(m_bAlphaFlag)
    {
      m_pImagePlane = &m_ImagePlane[JXR_PLANE_ALPHA];
      m_pImagePlane->iColorFormat = JXR_ICF_Y;
      m_pImagePlane->iChannels = 1;
      m_pImagePlane = &m_ImagePlane[JXR_PLANE_PRIMARY];
    }

    return ExcStatusOk;
  } // OwnJPEGXREncoder::AttachImage()

  ExcStatus DetachImage(void)
  {
    return ExcStatusOk;
  } // OwnJPEGXREncoder::DetachImage()


protected:
  ImageEnumColorSpace m_iColor;
};


JPEGXREncoder::JPEGXREncoder(void)
{
  m_enc = 0;
} // ctor


JPEGXREncoder::~JPEGXREncoder(void)
{
  Close();
} // dtor


ExcStatus JPEGXREncoder::Init(void)
{
  if(0 == m_enc)
  {
    m_enc = new OwnJPEGXREncoder;
    if(!m_enc)
      return ExcStatusFail;
  }

  return ExcStatusOk;
} // JPEGXREncoder::Init()


ExcStatus JPEGXREncoder::Close(void)
{
    if(m_enc) {
        delete m_enc;
        m_enc = NULL;
    }
  return ExcStatusOk;
} // JPEGXREncoder::Clean()


const CodecVersion& JPEGXREncoder::GetVersion(void)
{
  return version;
} // JPEGXREncoder::GetVersion()


Ipp32u JPEGXREncoder::SetNOfThreads(Ipp32u iThreads)
{
  return 1;
} // JPEGXREncoder::SetNOfThreads()


Ipp32u JPEGXREncoder::NOfThreads()
{
  return 1;
} // JPEGXREncoder::NOfThreads()


ExcStatus JPEGXREncoder::AttachStream(BaseStreamOutput& out)
{
  return m_enc->AttachStream(out);
} // JPEGXREncoder::AttachStream()


ExcStatus JPEGXREncoder::DetachStream(void)
{
  return ExcStatusOk;
} // JPEGXREncoder::DetachStream()


void JPEGXREncoder::AttachDiagnOut(BaseStreamDiagn& diagnOutput)
{
  return;
} // JPEGXREncoder::AttachDiagnOut()


void JPEGXREncoder::DetachDiagnOut(void)
{
  return;
} // JPEGXREncoder::DetachDiagnOut()


ExcStatus JPEGXREncoder::SetParams(InputParams &params)
{
  return m_enc->SetParams(params.iQuality,
                          params.iOverlap,
                          params.iBands,
                          params.iSampling,
                          params.iTrim,
                          params.iShift,
                          params.bFrequency,
                          params.bCMYKD,
                          params.bAlphaPlane,
                          params.pTilesUniform);
} // JPEGXREncoder::SetOptions()


ExcStatus JPEGXREncoder::FreeData(void)
{
  return m_enc->KillEncoder();
} // JPEGXREncoder::FreeData()


ExcStatus JPEGXREncoder::AttachImage(const Image& image)
{
  return m_enc->AttachImage(image);
} // JPEGXREncoder::AttachImage()


ExcStatus JPEGXREncoder::DetachImage(void)
{
  return m_enc->DetachImage();
} // JPEGXREncoder::DetachImage()


ExcStatus JPEGXREncoder::WriteFileHeader(Ipp32u bPAlphaPresent)
{
  return m_enc->WriteFileHeader(bPAlphaPresent);
} // JPEGXREncoder::WriteFileHeader()


ExcStatus JPEGXREncoder::WriteHeader()
{
  return m_enc->WriteHeader();
} // JPEGXREncoder::WriteHeader()


ExcStatus JPEGXREncoder::WriteData()
{
  return m_enc->WriteData();
} // JPEGXREncoder::WriteData()
