/*
//
//              INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#include <stdlib.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
#ifndef __UIC_BMP_ENC_H__
#include "uic_bmp_enc.h"
#endif


using namespace UIC;

static int DIB_PAD_BYTES(
  int width,
  int pixelStep)
{
  int pad_bytes;
  int dib_align;
  int uwidth;
  int awidth;

  dib_align = sizeof(Ipp32u) - 1;

  uwidth = width * pixelStep;
  awidth = (uwidth + dib_align) & (~dib_align);

  pad_bytes = awidth - uwidth;

  return pad_bytes;
} // DIB_PAD_BYTES()


namespace UIC {

class OwnBMPEncoder
{
public:
  OwnBMPEncoder(void) : m_stream(0), m_image(0) {}

  void AttachDiagnOut(BaseStreamDiagn& diagnOutput)
  {
    m_diagnOutput = diagnOutput;
  }

  void DetachDiagnOut(void)
  {
    m_diagnOutput.SetDummy();
  }

  void AttachStream(BaseStreamOutput& stream)
  {
    m_stream = &stream;
  }

  void DetachStream(void)
  {
    m_stream = 0;
  }

  ExcStatus AttachImage(const Image& image)
  {
    m_image = &image;

    const ImageSamplingGeometry& imgSamplingGeometry  = image.Buffer().BufferFormat().SamplingGeometry();
    const ImageColorSpec&        imgColorSpec         = image.ColorSpec();
    const ImageDataOrder&        dataOrder            = image.Buffer().BufferFormat().DataOrder();

    if(dataOrder.ComponentOrder() != UIC::Interleaved)
       return ExcStatusFail;

    if(dataOrder.DataType() != T8u)
        return ExcStatusFail;

    Ipp32u nOfComponents = imgSamplingGeometry.NOfComponents();

    Ipp32u       component;
    unsigned int bitDepth = 0;

    for(component = 0; component < nOfComponents; component++)
    {
      RectSize& sampleSize = imgSamplingGeometry.SampleSize()[component];

      if(sampleSize.Width()  != 1)
        return ExcStatusFail;

      if(sampleSize.Height() != 1)
        return ExcStatusFail;

      ImageDataRange& dataRange = imgColorSpec.DataRange()[component];
      if(dataRange.IsSigned())
        return ExcStatusFail;

      if(component)
      {
        if(dataRange.BitDepth() != bitDepth)
          return ExcStatusFail;
      }

      bitDepth = dataRange.BitDepth();
    }

    if(imgColorSpec.ColorSpecMeth() != Enumerated)
      return ExcStatusFail;

    if(imgColorSpec.ComponentToColorMap() != Direct)
      return ExcStatusFail;

    return ExcStatusOk;
  }

  ExcStatus DetachImage(void)
  {
    m_image = 0;

    return ExcStatusOk;
  }

  ExcStatus WriteHeader(void)
  {
    int i;
    int fileSize;
    int imageStep;
    int imageSize;
    int nOfComponents;

    RGBquad             palette[256];
    BMPImageHeader      bmih;
    BMPImageFileHeader  bmfh;

    BaseStream::TSize   cnt;
    BaseStream::TStatus status;

    imageStep     = m_image->Buffer().BufferFormat().DataOrder().LineStep()[0];
    imageSize     = imageStep * m_image->Buffer().BufferFormat().SamplingGeometry().RefGridRect().Height();
    fileSize      = imageSize + BMP_IFH_SIZE + BMP_IH_SIZE;
    nOfComponents = m_image->Buffer().BufferFormat().SamplingGeometry().NOfComponents();

    bmfh.bfType      = 0x4d42; // 0x4d42 == 'MB'
    bmfh.bfSize      = fileSize;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits   = BMP_IFH_SIZE + BMP_IH_SIZE;

    if(nOfComponents == 1)
    {
      bmfh.bfOffBits += sizeof(palette);
    }

    status = m_stream->Write(&bmfh.bfType, sizeof(bmfh.bfType),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmfh.bfSize, sizeof(bmfh.bfSize),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmfh.bfReserved1, sizeof(bmfh.bfReserved1),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmfh.bfReserved2, sizeof(bmfh.bfReserved2),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmfh.bfOffBits, sizeof(bmfh.bfOffBits),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    bmih.biSize          = BMP_IH_SIZE;
    bmih.biWidth         = m_image->Buffer().BufferFormat().SamplingGeometry().RefGridRect().Width();
    bmih.biHeight        = m_image->Buffer().BufferFormat().SamplingGeometry().RefGridRect().Height();
    bmih.biPlanes        = 1;
    bmih.biBitCount      = (unsigned short)(nOfComponents << 3);
    bmih.biCompression   = 0L; // BI_RGB
    bmih.biSizeImage     = imageSize;
    bmih.biXPelsPerMeter = 0;
    bmih.biYPelsPerMeter = 0;
    bmih.biClrUsed       = nOfComponents == 1 ? 256 : 0;
    bmih.biClrImportant  = nOfComponents == 1 ? 256 : 0;

    status = m_stream->Write(&bmih.biSize,sizeof(bmih.biSize),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmih.biWidth,sizeof(bmih.biWidth),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmih.biHeight,sizeof(bmih.biHeight),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmih.biPlanes,sizeof(bmih.biPlanes),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmih.biBitCount,sizeof(bmih.biBitCount),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmih.biCompression,sizeof(bmih.biCompression),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmih.biSizeImage,sizeof(bmih.biSizeImage),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmih.biXPelsPerMeter,sizeof(bmih.biXPelsPerMeter),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmih.biYPelsPerMeter,sizeof(bmih.biYPelsPerMeter),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmih.biClrUsed,sizeof(bmih.biClrUsed),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    status = m_stream->Write(&bmih.biClrImportant,sizeof(bmih.biClrImportant),cnt);
    if(BaseStream::StatusOk!= status)
      return ExcStatusFail;

    if(nOfComponents == 1)
    {
      for(i = 0; i < 256; i++)
      {
        palette[i].rgbBlue     = (Ipp8u)i;
        palette[i].rgbGreen    = (Ipp8u)i;
        palette[i].rgbRed      = (Ipp8u)i;
        palette[i].rgbReserved = (Ipp8u)0;
      }

      status = m_stream->Write(&palette,sizeof(palette),cnt);
      if(BaseStream::StatusOk!= status)
        return ExcStatusFail;
    }

    if(!m_image)
      return ExcStatusFail;

    return ExcStatusOk;
  }

  ExcStatus WriteData(void)
  {
    int                 i;
    int                 step;
    int                 bmppad;
    int                 bmpwidth;
    int                 height;
    Ipp8u*              ptr;
    BaseStream::TSize   cnt;
    BaseStream::TStatus status;

    if(!m_image) return ExcStatusFail;

    const ImageDataPtr*   data      = m_image->Buffer().DataPtr();

    height = m_image->Buffer().BufferFormat().SamplingGeometry().RefGridRect().Height();
    step   = m_image->Buffer().BufferFormat().DataOrder().LineStep()[0];
    // write bottom-up BMP
    ptr = (Ipp8u*)data[0].p8u + step * (height - 1);

    bmppad = DIB_PAD_BYTES(m_image->Buffer().BufferFormat().SamplingGeometry().RefGridRect().Width(),
                           m_image->Buffer().BufferFormat().SamplingGeometry().NOfComponents());
    bmpwidth = m_image->Buffer().BufferFormat().SamplingGeometry().RefGridRect().Width() *
               m_image->Buffer().BufferFormat().SamplingGeometry().NOfComponents() + bmppad;

    for(i = 0; i < height; i++)
    {
      status = m_stream->Write(ptr - i * step,bmpwidth,cnt);
      if(BaseStream::StatusOk!= status)
        return ExcStatusFail;
    }

    return ExcStatusOk;
  }

protected:
    BaseStreamOutput*   m_stream;
    BaseStreamDiagnRef  m_diagnOutput;
    const Image*        m_image;

};

} // namespace UIC


using namespace UIC;


static const CodecVersion version =
{
  1,
  0,
  0,
  0,
  "Intel (R) BMP Encoder",
  "1.0",
  __DATE__
};


BMPEncoder::BMPEncoder(void)
: m_encoder(0)
{
}


BMPEncoder::~BMPEncoder(void)
{
  Close();
}


ExcStatus BMPEncoder::Init(void)
{
  if(m_encoder)
    return ExcStatusOk;

  m_encoder = new OwnBMPEncoder;
  if (!m_encoder)
    return ExcStatusFail;

  return ExcStatusOk;
}


ExcStatus BMPEncoder::Close(void)
{
    if(m_encoder) {
        delete m_encoder;
        m_encoder = NULL;
    }
  return ExcStatusOk;
}


void BMPEncoder::AttachDiagnOut(BaseStreamDiagn &diagnOutput)
{
  if (!m_encoder)
    return;

  m_encoder->AttachDiagnOut(diagnOutput);
}


void BMPEncoder::DetachDiagnOut()
{
  if (!m_encoder)
    return;

  m_encoder->DetachDiagnOut();
}


const CodecVersion& BMPEncoder::GetVersion()
{
  return version;
}


ExcStatus BMPEncoder::AttachStream(BaseStreamOutput &stream)
{
  if (!m_encoder)
    return ExcStatusFail;

  m_encoder->AttachStream(stream);

  return ExcStatusOk;
}


ExcStatus BMPEncoder::DetachStream(void)
{
  if (!m_encoder)
    return ExcStatusFail;

  m_encoder->DetachStream();

  return ExcStatusOk;
}


ExcStatus BMPEncoder::AttachImage(const Image& image)
{
  if (!m_encoder)
    return ExcStatusFail;

  return m_encoder->AttachImage(image);
}


ExcStatus BMPEncoder::DetachImage(void)
{
  if (!m_encoder)
    return ExcStatusFail;

  return m_encoder->DetachImage();
}


ExcStatus BMPEncoder::WriteHeader(void)
{
  if (!m_encoder)
    return ExcStatusFail;

  return m_encoder->WriteHeader();
}


ExcStatus BMPEncoder::WriteData(void)
{
  if (!m_encoder)
    return ExcStatusFail;

  return m_encoder->WriteData();
}

