/*
//
//              INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4100 )
#endif
#include <stdlib.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
#ifndef __UIC_BMP_DEC_H__
#include "uic_bmp_dec.h"
#endif

#ifndef __IPPI_H__
#include "ippi.h"
#endif

using namespace UIC;

namespace UIC {

class OwnBMPDecoder
{
public:
  OwnBMPDecoder(void) : m_stream(0) {}

  void AttachStream(BaseStreamInput& stream)
  {
    m_stream = &stream;
  }

  void DetachStream(void)
  {
    m_stream = 0;
  }

  void AttachDiagnOut(BaseStreamDiagn& diagnOutput);
  void DetachDiagnOut(void);

  ExcStatus ReadHeader(
    ImageColorSpec&         colorSpec,
    ImageSamplingGeometry&  geometry)
  {
    int      i;
    int      nchannels;
    Rect     refgrid;
    Point    origin;
    RectSize size;

    BaseStream::TSize   cnt;
    BaseStream::TStatus status;

    status = m_stream->Read(&m_bmfh.bfType,sizeof(m_bmfh.bfType),cnt);
    if(false != status)
      return ExcStatusFail;

    status = m_stream->Read(&m_bmfh.bfSize,sizeof(m_bmfh.bfSize),cnt);
    if(false != status)
      return ExcStatusFail;

    if(m_bmfh.bfType != 0x4d42) // 0x4d42 == 'MB'
      return ExcStatusFail;

    status = m_stream->Read(&m_bmfh.bfReserved1,sizeof(m_bmfh.bfReserved1),cnt);
    if(false != status)
      return ExcStatusFail;

    status = m_stream->Read(&m_bmfh.bfReserved2,sizeof(m_bmfh.bfReserved2),cnt);
    if(false != status)
      return ExcStatusFail;

    status = m_stream->Read(&m_bmfh.bfOffBits,sizeof(m_bmfh.bfOffBits),cnt);
    if(false != status)
      return ExcStatusFail;

    status = m_stream->Read(&m_bmih.biSize,sizeof(m_bmih.biSize),cnt);
    if(false != status)
      return ExcStatusFail;

// TODO: add check for extended BMPImageHeader
//    if(m_bmih.biSize != sizeof(BMPImageHeader))
//      return ExcStatusFail;

    status = m_stream->Read(&m_bmih.biWidth,sizeof(m_bmih.biWidth),cnt);
    if(false != status)
      return ExcStatusFail;

    status = m_stream->Read(&m_bmih.biHeight,sizeof(m_bmih.biHeight),cnt);
    if(false != status)
      return ExcStatusFail;

    status = m_stream->Read(&m_bmih.biPlanes,sizeof(m_bmih.biPlanes),cnt);
    if(false != status)
      return ExcStatusFail;

    status = m_stream->Read(&m_bmih.biBitCount,sizeof(m_bmih.biBitCount),cnt);
    if(false != status)
      return ExcStatusFail;

    if(m_bmih.biBitCount != 8 && m_bmih.biBitCount != 24 && m_bmih.biBitCount != 32)
       return ExcStatusFail;

    status = m_stream->Read(&m_bmih.biCompression,sizeof(m_bmih.biCompression),cnt);
    if(false != status)
      return ExcStatusFail;

    switch(m_bmih.biCompression)
    {
    case 0L: //0L == BI_RGB
      break;

    case 3L: //3L == BI_BITFIELDS (we support only 8uC4 images)
      {
        if(m_bmih.biBitCount != 32)
          return ExcStatusFail;
      }
      break;

    default:
      return ExcStatusFail;
    }

    status = m_stream->Read(&m_bmih.biSizeImage,sizeof(m_bmih.biSizeImage),cnt);
    if(false != status)
      return ExcStatusFail;

    status = m_stream->Read(&m_bmih.biXPelsPerMeter,sizeof(m_bmih.biXPelsPerMeter),cnt);
    if(false != status)
      return ExcStatusFail;

    status = m_stream->Read(&m_bmih.biYPelsPerMeter,sizeof(m_bmih.biYPelsPerMeter),cnt);
    if(false != status)
      return ExcStatusFail;

     status = m_stream->Read(&m_bmih.biClrUsed,sizeof(m_bmih.biClrUsed),cnt);
    if(false != status)
      return ExcStatusFail;

    status = m_stream->Read(&m_bmih.biClrImportant,sizeof(m_bmih.biClrImportant),cnt);
    if(false != status)
      return ExcStatusFail;

    if(m_bmih.biBitCount == 8)
    {
      status = m_stream->Read(m_palette,sizeof(RGBquad)*256,cnt);
      if(BaseStream::StatusOk != status)
        return ExcStatusFail;
    }

    if(m_bmih.biBitCount == 32 && m_bmih.biCompression == 3L)
    {
      status = m_stream->Read(m_palette,sizeof(RGBquad)*3,cnt);
      if(BaseStream::StatusOk != status)
        return ExcStatusFail;
    }

    m_roi.width  = m_bmih.biWidth;
    m_roi.height = abs((int)m_bmih.biHeight);

    nchannels = m_bmih.biBitCount >> 3;

    if(nchannels == 0)
      return ExcStatusFail;

    status = m_stream->Seek(m_bmfh.bfOffBits,BaseStreamInput::Beginning);
    if(BaseStream::StatusOk != status)
      return ExcStatusFail;

    origin.SetX(0);
    origin.SetY(0);

    size.SetWidth (m_roi.width);
    size.SetHeight(m_roi.height);

    refgrid.SetOrigin(origin);
    refgrid.SetSize(size);

    geometry.SetRefGridRect(refgrid);
    geometry.ReAlloc(nchannels);
    geometry.SetEnumSampling(S444);

    colorSpec.ReAlloc(nchannels);
    colorSpec.SetColorSpecMethod(Enumerated);
    colorSpec.SetComponentToColorMap(Direct);

    switch(nchannels)
    {
    case 1:  colorSpec.SetEnumColorSpace(Grayscale); break;
    case 3:  colorSpec.SetEnumColorSpace(BGR);       break;
    case 4:
      {
        if(3L == m_bmih.biCompression && m_bmih.biBitCount == 32)
          colorSpec.SetEnumColorSpace(RGBA); // it is ABGR actually
        else
          colorSpec.SetEnumColorSpace(BGRA);
      }
      break;

    default: colorSpec.SetEnumColorSpace(Unknown);   break;
    }

    for(i = 0; i < nchannels; i++)
    {
      geometry.SampleSize()[i].SetWidth (1);
      geometry.SampleSize()[i].SetHeight(1);
    }

    for(i = 0; i < nchannels; i++)
      colorSpec.DataRange()[i].SetAsRange8u(255);

    return ExcStatusOk;
  }

  ExcStatus ReadData(
    const ImageDataPtr*   data,
    const ImageDataOrder& dataOrder)
  {
    int                 i;
    int                 step;
    Ipp8u*              ptr;
    BaseStream::TSize   cnt;
    BaseStream::TStatus status;

    status = m_stream->Seek(m_bmfh.bfOffBits, BaseStreamInput::Beginning);
    if(BaseStream::StatusOk != status)
      return ExcStatusFail;

    step = dataOrder.LineStep()[0];

    if(0 < m_bmih.biHeight) // read bottom-up BMP
    {
      ptr = (Ipp8u*)data[0].p8u + step * (m_roi.height - 1);

      for(i = 0; i < m_roi.height; i++)
      {
        status = m_stream->Read(ptr - i * step, step, cnt);
        if(BaseStream::StatusOk != status)
          return ExcStatusFail;
      }
    }
    else // read up-bottom BMP
    {
      ptr = (Ipp8u*)data[0].p8u;

      for(i = 0; i < m_roi.height; i++)
      {
        status = m_stream->Read(ptr + i * step, step, cnt);
        if(BaseStream::StatusOk != status)
          return ExcStatusFail;
      }
    }

    if(3L == m_bmih.biCompression && m_bmih.biBitCount == 32)
    {
      int  order[4] = {3,2,1,0}; // convert from ABGR to RGBA

      ptr = (Ipp8u*)data[0].p8u;

      ippiSwapChannels_8u_C4IR(ptr, step, m_roi, order);
    }

    return ExcStatusOk;
  }

protected:
  BaseStreamInput*   m_stream;

  IppiSize           m_roi;
  RGBquad            m_palette[256];
  BMPImageHeader     m_bmih;
  BMPImageFileHeader m_bmfh;

};

} // namespace UIC


static const CodecVersion version =
{
    1,                           /*  major, e.g. 1                */
    0,                           /*  minor, e.g. 2                */
    0,                           /*  micro, e.g. 3                */
    0,                           /*  build, e.g. 10               */
    "Intel (R) UIC BMP Decoder", /*  name                         */
    "v1.0",                      /*  version, e.g. "v1.2 Beta"    */
    __DATE__                     /*  buildDate, e.g. "Jul 20 99"  */
};


BMPDecoder::BMPDecoder(void)
{
  m_dec = 0;
} // ctor


BMPDecoder::~BMPDecoder(void)
{
  Close();
} // dtor


const CodecVersion& BMPDecoder::GetVersion(void)
{
  return version;
} // BMPDecoder::GetVersion()


ExcStatus BMPDecoder::Init(void)
{
  if(m_dec)
    return ExcStatusOk;

  m_dec = new OwnBMPDecoder;
  if (!m_dec) return ExcStatusFail;

  return ExcStatusOk;
} // BMPDecoder::Init()


ExcStatus BMPDecoder::Close(void)
{
    if(m_dec) {
        delete m_dec;
        m_dec = NULL;
    }
 return ExcStatusOk;
} // BMPDecoder::Close()


ExcStatus BMPDecoder::AttachStream(BaseStreamInput& stream)
{
  m_dec->AttachStream(stream);
  return ExcStatusOk;
} // BMPDecoder::AttachStream()


ExcStatus BMPDecoder::DetachStream(void)
{
  m_dec->DetachStream();
  return ExcStatusOk;
} // BMPDecoder::DetachStream()


void BMPDecoder::AttachDiagnOut(BaseStreamDiagn& diagnOutput)
{
  return;
} // BMPDecoder::AttachDiagnOut()


void BMPDecoder::DetachDiagnOut(void)
{
  return;
} // BMPDecoder::DetachDiagnOut()


ExcStatus BMPDecoder::ReadHeader(
  ImageColorSpec&        colorSpec,
  ImageSamplingGeometry& geometry)
{
  if (!m_dec)
    return ExcStatusFail;

  return m_dec->ReadHeader(colorSpec, geometry);
} // BMPDecoder::ReadHeader()


ExcStatus BMPDecoder::ReadData(
  const ImageDataPtr*   data,
  const ImageDataOrder& dataOrder)
{
  return m_dec->ReadData(data, dataOrder);
} // BMPDecoder::Read()

