/*
//
//             INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef PNG_H
#include "png.h"
#endif
#ifndef __UIC_PNG_DEC_H__
#include "uic_png_dec.h"
#endif
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning ( disable : 4611 4530 )
#endif
using namespace UIC;

static const CodecVersion version =
{
    1,                           /*  major, e.g. 1                */
    0,                           /*  minor, e.g. 2                */
    0,                           /*  micro, e.g. 3                */
    0,                           /*  build, e.g. 10               */
    "Intel (R) UIC PNG Decoder", /*  name                         */
    "v1.0",                      /*  version, e.g. "v1.2 Beta"    */
    __DATE__                     /*  buildDate, e.g. "Jul 20 99"  */
};


static ImageEnumColorSpace uicCFormatMap[] =
{
  Grayscale,      // PNG_COLOR_TYPE_GRAY
  Unknown,
  RGB,            // PNG_COLOR_TYPE_RGB
  Unknown,        // PNG_COLOR_TYPE_PALETTE
  GrayscaleAlpha, // PNG_COLOR_MASK_ALPHA
  Unknown,
  RGBA            // PNG_COLOR_TYPE_RGB_ALPHA
};


void PNGAPI pngReadFunc(
  png_structp png_ptr,
  png_bytep   buffer,
  png_size_t  size)
{
  BaseStreamInput*  stream = (BaseStreamInput*)png_ptr->io_ptr;
  BaseStream::TSize cnt = 0;

  if(UIC::BaseStreamInput::StatusOk != stream->Read(buffer, size, cnt))
    png_error(png_ptr, "Error reading png file");

  return;
} // pngReadFunc()


class OwnPNGDecoder
{
public:
  OwnPNGDecoder(void) {}
  ~OwnPNGDecoder(void) {}

  ExcStatus Init(void)
  {
    m_pStream    = 0;
    m_iByteOrder = 0;
    return ExcStatusOk;
  }
  ExcStatus Close(void) { return ExcStatusOk; }

  ExcStatus AttachStream(BaseStreamInput& stream)
  {
    m_pStream = &stream;
    return ExcStatusOk;
  }

  ExcStatus DetachStream(void) {m_pStream = 0; return ExcStatusOk; }

  ExcStatus ReadHeader(
              ImageColorSpec&         colorSpec,
              ImageSamplingGeometry&  geometry)
  {
    Point    origin;
    RectSize size;
    Rect     refgrid;
    png_uint_32 iWidth;
    png_uint_32 iHeight;
    Ipp32u iChannels;
    Ipp32s iPrecision;
    Ipp32s iColor;
    Ipp32s iInterlace;
    Ipp32s iCompression;
    Ipp32s iFilter;
    Ipp32u i;

    m_pPNGStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(m_pPNGStruct == NULL)
      return ExcStatusFail;

    m_pInfo = png_create_info_struct(m_pPNGStruct);
    if(m_pInfo == NULL)
    {
      png_destroy_read_struct(&m_pPNGStruct, png_infopp_NULL, png_infopp_NULL);
      return ExcStatusFail;
    }

    if(setjmp(png_jmpbuf(m_pPNGStruct)))
    {
      png_destroy_read_struct(&m_pPNGStruct, &m_pInfo, png_infopp_NULL);
      return ExcStatusFail;
    }

    png_set_read_fn(m_pPNGStruct, m_pStream, pngReadFunc);

    png_read_info(m_pPNGStruct, m_pInfo);
    png_get_IHDR(m_pPNGStruct, m_pInfo, &iWidth, &iHeight, &iPrecision, &iColor, &iInterlace, &iCompression, &iFilter);
    iChannels  = m_pInfo->channels;

    // swap bytes of 16 bit files to least significant byte first
    if(!m_iByteOrder)
      png_set_swap(m_pPNGStruct);

    // Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
    if(iColor == PNG_COLOR_TYPE_GRAY && iPrecision < 8)
    {
      png_set_expand_gray_1_2_4_to_8(m_pPNGStruct);
      iPrecision = 8;
    }
    /* Expand paletted colors into true RGB triplets */
    if(iColor == PNG_COLOR_TYPE_PALETTE)
    {
      png_set_palette_to_rgb(m_pPNGStruct);
      iColor     = PNG_COLOR_TYPE_RGB;
      iChannels  = 3;
      iPrecision = 8;
    }
   /* Expand paletted or RGB images with transparency to full alpha channels */
   if(png_get_valid(m_pPNGStruct, m_pInfo, PNG_INFO_tRNS) && iColor != PNG_COLOR_TYPE_GRAY)
   {
      png_set_tRNS_to_alpha(m_pPNGStruct);
      iColor    = PNG_COLOR_TYPE_RGB_ALPHA;
      iChannels = 4;
   }

    size.SetWidth(iWidth);
    size.SetHeight(iHeight);
    origin.SetX(0);
    origin.SetY(0);
    refgrid.SetOrigin(origin);
    refgrid.SetSize(size);

    geometry.SetRefGridRect(refgrid);
    geometry.ReAlloc(iChannels);
    geometry.SetEnumSampling(UIC::S444);

    colorSpec.ReAlloc(iChannels);
    colorSpec.SetColorSpecMethod(UIC::Enumerated);
    colorSpec.SetComponentToColorMap(UIC::Direct);
    colorSpec.SetEnumColorSpace(uicCFormatMap[iColor]);

    if(iPrecision <= 8)
    {
      for(i = 0; i < iChannels; i++)
        colorSpec.DataRange()[i].SetAsRange8u(IPP_MAX_8U);
    }
    else
    {
      for(i = 0; i < iChannels; i++)
        colorSpec.DataRange()[i].SetAsRange16u(IPP_MAX_16U);
    }

    return ExcStatusOk;
  }

  ExcStatus ReadData(
              const ImageDataPtr*   data,
              const ImageDataOrder& dataOrder)
  {
    Ipp8u*  pData = data->p8u;
    Ipp8u** pRow;
    Ipp32u  iStep   = dataOrder.LineStep()[0];
    Ipp32u  iHeight = m_pInfo->height;
    Ipp32u  i;

    try
    {
      pRow = new Ipp8u*[iHeight];
      if(pRow == 0)
      {
        png_destroy_read_struct(&m_pPNGStruct, &m_pInfo, png_infopp_NULL);
        return ExcStatusFail;
      }

      for(i = 0; i < iHeight; i++)
        pRow[i] = pData + iStep*i;

      if(!setjmp(png_jmpbuf(m_pPNGStruct)))
      {
        png_read_image(m_pPNGStruct, pRow);
        png_read_end(m_pPNGStruct, m_pInfo);
      }

      delete []pRow;
      png_destroy_read_struct(&m_pPNGStruct, &m_pInfo, png_infopp_NULL);
    }

    catch (...)
    {
      png_destroy_read_struct(&m_pPNGStruct, &m_pInfo, png_infopp_NULL);
      return ExcStatusFail;
    }

    return ExcStatusOk;
  }

protected:
  BaseStreamInput* m_pStream;
  png_structp      m_pPNGStruct;
  png_infop        m_pInfo;
  Ipp32u m_iByteOrder;
};


PNGDecoder::PNGDecoder(void)
{
  m_dec = 0;
} // ctor


PNGDecoder::~PNGDecoder(void)
{
  Close();
} // dtor


ExcStatus PNGDecoder::Init(void)
{
  if(0 == m_dec)
  {
    m_dec = new OwnPNGDecoder;
    if(!m_dec)
      return ExcStatusFail;
  }

  return m_dec->Init();
} // PNGDecoder::Init()


ExcStatus PNGDecoder::Close(void)
{
    if(m_dec) {
        delete m_dec;
        m_dec = NULL;
    }
  return ExcStatusOk;
} // PNGDecoder::Close()


const CodecVersion& PNGDecoder::GetVersion(void)
{
  return version;
} // PNGDecoder::GetVersion()


ExcStatus PNGDecoder::AttachStream(BaseStreamInput& stream)
{
  return m_dec->AttachStream(stream);
} // PNGDecoder::AttachStream()


ExcStatus PNGDecoder::DetachStream(void)
{
  return m_dec->DetachStream();
} // PNGDecoder::DetachStream()


void PNGDecoder::AttachDiagnOut(BaseStreamDiagn&)
{
  return;
} // PNGDecoder::AttachDiagnOut()


void PNGDecoder::DetachDiagnOut(void)
{
  return;
} // PNGDecoder::DetachDiagnOut()


ExcStatus PNGDecoder::ReadHeader(
  ImageColorSpec&         colorSpec,
  ImageSamplingGeometry&  geometry)
{
  return m_dec->ReadHeader(colorSpec,geometry);
} // PNGDecoder::ReadHeader()


ExcStatus PNGDecoder::ReadData(
  const ImageDataPtr*   data,
  const ImageDataOrder& dataOrder)
{
  return m_dec->ReadData(data,dataOrder);
} // PNGDecoder::ReadData()

