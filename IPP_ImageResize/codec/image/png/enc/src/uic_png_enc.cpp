/*
//
//             INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning ( disable : 4611 4530 )
#endif
#ifndef PNG_H
#include "png.h"
#endif
#ifndef __UIC_PNG_ENC_H__
#include "uic_png_enc.h"
#endif

using namespace UIC;


static const CodecVersion version =
{
    1,                           /*  major, e.g. 1                */
    0,                           /*  minor, e.g. 2                */
    0,                           /*  micro, e.g. 3                */
    0,                           /*  build, e.g. 10               */
    "Intel (R) UIC PNG Encoder", /*  name                         */
    "v1.0",                      /*  version, e.g. "v1.2 Beta"    */
    __DATE__                     /*  buildDate, e.g. "Jul 20 99"  */
};


void PNGAPI pngWriteFunc(
  png_structp png_ptr,
  png_bytep   buffer,
  png_size_t  size)
{
  BaseStreamOutput* stream = (BaseStreamOutput*)png_ptr->io_ptr;
  BaseStream::TSize cnt    = 0;

  if(UIC::BaseStreamOutput::StatusOk != stream->Write(buffer, size, cnt))
    png_error(png_ptr, "Error reading png file");

  return;
} // pngWriteFunc()


class OwnPNGEncoder
{
public:
  OwnPNGEncoder(void) {}
  ~OwnPNGEncoder(void) {}

  ExcStatus Init(void)
  {
    m_pStream    = 0;
    return ExcStatusOk;
  }
  ExcStatus Close(void) { return ExcStatusOk; }

  ExcStatus AttachStream(BaseStreamOutput& stream)
  {
    m_pStream = &stream;
    return ExcStatusOk;
  }

  ExcStatus DetachStream(void) { m_pStream = 0; return ExcStatusOk; }

  ExcStatus SetParams(bool* filters)
  {
    m_iFilters[0] = filters[0];
    m_iFilters[1] = filters[1];
    m_iFilters[2] = filters[2];
    m_iFilters[3] = filters[3];
    m_iFilters[4] = filters[4];

    return ExcStatusOk;
  }

  ExcStatus AttachImage(const Image& image)
  {
    Rect rect              = image.Buffer().BufferFormat().SamplingGeometry().RefGridRect();
    ImageDataType dataType = image.ColorSpec().DataRange()->DataType();
    m_iWidth     = rect.Width();
    m_iHeight    = rect.Height();
    m_iPrecision = image.ColorSpec().DataRange()->BitDepth() + 1;
    m_pData      = image.Buffer().DataPtr()->p8u;
    m_iColor     = image.ColorSpec().EnumColorSpace();
    m_iStep      = *image.Buffer().BufferFormat().DataOrder().LineStep();
    m_iChannels  = image.Buffer().BufferFormat().SamplingGeometry().NOfComponents();

    // Depth support check
    switch(dataType)
    {
    case T8u:
    case T16u:
      break;
    default:
      return ExcStatusFail;
    }

    // Color support check
    switch(m_iColor)
    {
    case BGRA:
    case RGBA:
      m_iColor = PNG_COLOR_TYPE_RGB_ALPHA;
      break;
    case BGR:
    case RGB:
      m_iColor = PNG_COLOR_TYPE_RGB;
      break;
    case GrayscaleAlpha:
      m_iColor = PNG_COLOR_TYPE_GRAY_ALPHA;
      break;
    case Grayscale:
      m_iColor = PNG_COLOR_TYPE_GRAY;
      break;
    default:
      return ExcStatusFail;
    }

    // Channels support check
    switch(m_iColor)
    {
    case PNG_COLOR_TYPE_GRAY:
      if(m_iChannels != 1)
        return ExcStatusFail;
      break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      if(m_iChannels != 2)
        return ExcStatusFail;
      break;
    case PNG_COLOR_TYPE_RGB:
      if(m_iChannels != 3)
        return ExcStatusFail;
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      if(m_iChannels != 4)
        return ExcStatusFail;
      break;
    default:
      return ExcStatusFail;
    }

    return ExcStatusOk;
  }

  ExcStatus DetachImage(void) { return ExcStatusOk; }

  ExcStatus WriteHeader(void)
  {
    int iFilter = 0;

    m_pPNGStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(m_pPNGStruct == NULL)
      return ExcStatusFail;

    m_pInfo = png_create_info_struct(m_pPNGStruct);
    if(m_pInfo == NULL)
    {
      png_destroy_write_struct(&m_pPNGStruct,  png_infopp_NULL);
      return ExcStatusFail;
    }

    if(setjmp(png_jmpbuf(m_pPNGStruct)))
    {
      png_destroy_write_struct(&m_pPNGStruct, &m_pInfo);
      return ExcStatusFail;
    }

    png_set_write_fn(m_pPNGStruct, m_pStream, pngWriteFunc, NULL);

    if(m_iFilters[0])
      iFilter |= PNG_FILTER_SUB;
    if(m_iFilters[1])
      iFilter |= PNG_FILTER_UP;
    if(m_iFilters[2])
      iFilter |= PNG_FILTER_AVG;
    if(m_iFilters[3])
      iFilter |= PNG_FILTER_PAETH;
    if(m_iFilters[4])
      iFilter |= PNG_ALL_FILTERS;

    png_set_filter(m_pPNGStruct, 0, iFilter);

    png_set_IHDR(
      m_pPNGStruct, m_pInfo, m_iWidth, m_iHeight, m_iPrecision, m_iColor,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(m_pPNGStruct, m_pInfo);

    return ExcStatusOk;
  }

  ExcStatus WriteData(void)
  {
    Ipp8u** pRow;
    Ipp32u  i;

    try
    {
      pRow = new Ipp8u*[m_iHeight];
      if(pRow == 0)
      {
        png_destroy_write_struct(&m_pPNGStruct, &m_pInfo);
        return ExcStatusFail;
      }

      if(m_iHeight > IPP_MAX_32U)
      {
        png_destroy_write_struct(&m_pPNGStruct, &m_pInfo);
        return ExcStatusFail;
      }

      for(i = 0; i < m_iHeight; i++)
        pRow[i] = m_pData + i*m_iStep;

      if(!setjmp(png_jmpbuf(m_pPNGStruct)))
      {
          png_write_image(m_pPNGStruct, pRow);
          png_write_end(m_pPNGStruct, m_pInfo);
      }

      delete []pRow;
      png_destroy_write_struct(&m_pPNGStruct, &m_pInfo);
    }

    catch (...)
    {
      png_destroy_write_struct(&m_pPNGStruct, &m_pInfo);
      return ExcStatusFail;
    }

    return ExcStatusOk;
  }

protected:
  BaseStreamOutput* m_pStream;
  png_structp       m_pPNGStruct;
  png_infop         m_pInfo;
  Ipp8u* m_pData;
  Ipp32u m_iWidth;
  Ipp32u m_iHeight;
  Ipp32u m_iPrecision;
  Ipp32u m_iColor;
  Ipp32u m_iChannels;
  Ipp32u m_iStep;
  Ipp8u  m_iFilters[5];
};


PNGEncoder::PNGEncoder(void)
{
  m_enc = 0;
} // ctor


PNGEncoder::~PNGEncoder(void)
{
  Close();
} // dtor


ExcStatus PNGEncoder::Init(void)
{
  if(0 == m_enc)
  {
    m_enc = new OwnPNGEncoder;
    if(!m_enc)
      return ExcStatusFail;
  }

  return m_enc->Init();
} // PNGEncoder::Init()


ExcStatus PNGEncoder::Close(void)
{
    if(m_enc) {
        delete m_enc;
        m_enc = NULL;
    }
  return ExcStatusOk;
} // PNGEncoder::Close()


const CodecVersion& PNGEncoder::GetVersion(void)
{
  return version;
} // PNGEncoder::GetVersion()


ExcStatus PNGEncoder::AttachStream(BaseStreamOutput& stream)
{
  return m_enc->AttachStream(stream);
} // PNGEncoder::AttachStream()


ExcStatus PNGEncoder::DetachStream(void)
{
  return m_enc->DetachStream();
} // PNGEncoder::DetachStream()


void PNGEncoder::AttachDiagnOut(BaseStreamDiagn&)
{
  return;
} // PNGEncoder::AttachDiagnOut()


void PNGEncoder::DetachDiagnOut(void)
{
  return;
} // PNGEncoder::DetachDiagnOut()


ExcStatus PNGEncoder::AttachImage(const Image& image)
{
  return m_enc->AttachImage(image);
} // PNGEncoder::AttachImage()


ExcStatus PNGEncoder::DetachImage(void)
{
  return m_enc->DetachImage();
} // PNGEncoder::DetachImage()


ExcStatus PNGEncoder::SetParams(bool* filters)
{
  return m_enc->SetParams(filters);
} // PNGEncoder::SetParams()


ExcStatus PNGEncoder::WriteHeader(void)
{
  return m_enc->WriteHeader();
} // PNGEncoder::WriteHeader()


ExcStatus PNGEncoder::WriteData(void)
{
  return m_enc->WriteData();
} // PNGEncoder::WriteData()
