/*
//
//             INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/
#include "stdafx.h"
#if defined(_MSC_VER)
#pragma warning ( disable : 4100 )
#endif
#ifndef __JPEGDEC_H__
#include "jpegdec.h"
#endif
#ifndef __UIC_JPEG_DEC_H__
#include "uic_jpeg_dec.h"
#endif


using namespace UIC;

static const ImageEnumColorSpace j2u_cmap[] =
{
  Unknown,
  Grayscale,
  RGB,
  BGR,
  YCbCr,
  CMYK,
  YCCK
};


static const ImageEnumSampling j2u_smap[] =
{
  S444,
  S422,
  S244,
  S411,
  SOther
};


static const JCOLOR u2j_cmap[] =
{
  JC_UNKNOWN,
  JC_GRAY,
  JC_UNKNOWN,
  JC_RGB,
  JC_BGR,
  JC_RGBA,
  JC_BGRA,
  JC_UNKNOWN,
  JC_UNKNOWN,
  JC_UNKNOWN,
  JC_YCBCR,
  JC_CMYK,
  JC_UNKNOWN,
  JC_YCCK
};


static const JSS u2j_smap[] =
{
  JS_444,
  JS_422,
  JS_244,
  JS_411,
  JS_OTHER
};


static const CodecVersion version =
{
    1, //int         major;      /* e.g. 1                               */
    0, //int         minor;      /* e.g. 2                               */
    0, //int         micro;      /* e.g. 3                               */
    0, //int         build;      /* e.g. 10, always >= majorBuild        */
    "Intel(R) UIC JPEG Decoder", /* e.g. "Intel(R) JPEG Decoder"         */
    "v1.0.0.0 beta",             /* e.g. "v1.2 Beta"                     */
    __DATE__                     /* e.g. "Jul 20 99"                     */
};


class OwnJPEGDecoder : public CJPEGDecoder
{
public:
  OwnJPEGDecoder(void)
  {
    m_size.width  = 0;
    m_size.height = 0;
    m_nchannels   = 0;
    m_color       = JC_UNKNOWN;
    m_sampling    = JS_444;
    m_precision   = 0;
  }

  virtual ~OwnJPEGDecoder(void) {}


  ExcStatus ReadHeader(
    ImageColorSpec&        colorSpec,
    ImageSamplingGeometry& geometry)
  {
    int        i;
    Point      origin;
    RectSize   size;
    Rect       refgrid;
    JERRCODE   jerr;

    jerr = CJPEGDecoder::ReadHeader(&m_size.width, &m_size.height, &m_nchannels, &m_color, &m_sampling, &m_precision);
    if(JPEG_OK != jerr)
      return ExcStatusFail;

    origin.SetX(0);
    origin.SetY(0);

    size.SetWidth(m_size.width);
    size.SetHeight(m_size.height);

    refgrid.SetOrigin(origin);
    refgrid.SetSize(size);

    geometry.SetRefGridRect(refgrid);

    geometry.ReAlloc(m_nchannels);
    geometry.SetEnumSampling(j2u_smap[m_sampling]);

    colorSpec.ReAlloc(m_nchannels);
    colorSpec.SetColorSpecMethod(UIC::Enumerated);
    colorSpec.SetComponentToColorMap(UIC::Direct);
    colorSpec.SetEnumColorSpace(j2u_cmap[m_color]);

    if(m_precision <= 8)
    {
      for(i = 0; i < m_nchannels; i++)
        colorSpec.DataRange()[i].SetAsRange8u((1 << m_precision) - 1);
    }
    else
    {
      for(i = 0; i < m_nchannels; i++)
        colorSpec.DataRange()[i].SetAsRange16u((1 << m_precision) - 1);
    }

    return ExcStatusOk;
  }

  ExcStatus SetParams(
    const ImageColorSpec&        colorSpec,
    const ImageSamplingGeometry& geometry)
  {
    ImageEnumSampling m_enumSampling;

    m_enumSampling = geometry.EnumSampling();

    if(JC_YCBCR == m_color && JC_GRAY == u2j_cmap[colorSpec.EnumColorSpace()])
      m_nchannels = 1;

    m_color     = u2j_cmap[colorSpec.EnumColorSpace()];
    m_sampling  = u2j_smap[m_enumSampling];
    m_precision = colorSpec.DataRange()[0].BitDepth() + 1;

    m_size.width  = geometry.RefGridRect().Width();
    m_size.height = geometry.RefGridRect().Height();

    return ExcStatusOk;
  } // JPEGDecoder::SetParams()

  ExcStatus ReadData(
    const ImageDataPtr*   data,
    const ImageDataOrder& dataOrder)
  {
    JERRCODE jerr;

    if(dataOrder.ComponentOrder() == UIC::Interleaved)
    {
      if(m_precision <= 8)
      {
        jerr = CJPEGDecoder::SetDestination(data[0].p8u , dataOrder.LineStep()[0], m_size, m_nchannels, m_color, m_sampling, m_precision, m_params.GetDCTScaleFactor());
        if(JPEG_OK != jerr)
          return ExcStatusFail;

          jerr = CJPEGDecoder::SetThreadingMode(m_params.GetThreadingMode());
          if(JPEG_OK != jerr)
            return ExcStatusFail;
      }
      else
      {
        jerr = CJPEGDecoder::SetDestination(data[0].p16s, dataOrder.LineStep()[0], m_size, m_nchannels, m_color, m_sampling, m_precision);
        if(JPEG_OK != jerr)
          return ExcStatusFail;
      }
    }
    else  //   UIC::Plane
    {
      if(m_precision <= 8)
      {
        Ipp8u* p[4];

        for(int i = 0; i < m_nchannels; i++)
          p[i] = data[i].p8u;

        jerr = CJPEGDecoder::SetDestination((Ipp8u**)p , dataOrder.LineStep(), m_size, m_nchannels, m_color, m_sampling, m_precision, m_params.GetDCTScaleFactor());
        if(JPEG_OK != jerr)
          return ExcStatusFail;
      }
      else
      {
        Ipp16u* p[4];

        for(int i = 0; i < m_nchannels; i++)
          p[i] = data[i].p16u;

        jerr = CJPEGDecoder::SetDestination((Ipp16s**)p, dataOrder.LineStep(), m_size, m_nchannels, m_color, m_sampling, m_precision);
        if(JPEG_OK != jerr)
          return ExcStatusFail;
      }
    }

    jerr = CJPEGDecoder::ReadData();
    if(JPEG_OK != jerr)
      return ExcStatusFail;

    return ExcStatusOk;
  } // JPEGDecoder::ReadData()

  ExcStatus SetParams(JPEGDecoderParams& params)
  {
    m_params = params;
    return ExcStatusOk;
  }

protected:
  IppiSize m_size;
  int      m_nchannels;
  JCOLOR   m_color;
  JSS      m_sampling;
  int      m_precision;

  JPEGDecoderParams m_params;
};


JPEGDecoder::JPEGDecoder(void)
{
  m_dec = 0;
} // ctor


JPEGDecoder::~JPEGDecoder(void)
{
  Close();
} // dtor


Ipp32u JPEGDecoder::SetNOfThreads(Ipp32u nOfThreads)
{
    set_num_threads(nOfThreads);
    return NOfThreads();
}

Ipp32u JPEGDecoder::NOfThreads   ()
{
    return get_num_threads();
}



ExcStatus JPEGDecoder::Init(void)
{
  if(0 == m_dec)
  {
    m_dec = new OwnJPEGDecoder;
    if(!m_dec)
      return ExcStatusFail;
  }

  return ExcStatusOk;
} // JPEGDecoder::Init()


ExcStatus JPEGDecoder::Close(void)
{
    if(m_dec) {
        delete m_dec;
        m_dec = NULL;
    }
  return ExcStatusOk;
} // JPEGDecoder::Close()


const CodecVersion& JPEGDecoder::GetVersion(void)
{
  return version;
} // JPEGDecoder::GetVersion()


char* JPEGDecoder::GetStatusString(ExcStatus status)
{
  return 0;
} // JPEGDecoder::GetStatusString()


ExcStatus JPEGDecoder::AttachStream(UIC::BaseStreamInput& stream)
{
  JERRCODE jerr;

  jerr = m_dec->SetSource((CBaseStreamInput*)&stream);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGDecoder::AttachStream()


ExcStatus JPEGDecoder::DetachStream(void)
{
  return ExcStatusOk;
} // JPEGDecoder::DetachStream()


ExcStatus JPEGDecoder::ReadHeader(
  ImageColorSpec&        colorSpec,
  ImageSamplingGeometry& geometry)
{
  return m_dec->ReadHeader(colorSpec, geometry);
} // JPEGDecoder::ReadHeader()


ExcStatus JPEGDecoder::SetParams(
  const ImageColorSpec&        colorSpec,
  const ImageSamplingGeometry& geometry)
{
  return m_dec->SetParams(colorSpec,geometry);
} // JPEGDecoder::SetParams()


ExcStatus JPEGDecoder::ReadData(
  const ImageDataPtr*  data,
  const ImageDataOrder &dataOrder)
{
  return m_dec->ReadData(data, dataOrder);
} // JPEGDecoder::ReadData()


void JPEGDecoder::Comment(Ipp8u** buf, int* size)
{
  m_dec->Comment(buf, size);
  return;
} // JPEGDecoder::Comment


JMODE JPEGDecoder::Mode()
{
  return m_dec->Mode();
} // JPEGDecoder::Mode


ExcStatus JPEGDecoder::InitHuffmanTable(Ipp8u bits[], Ipp8u vals[], int tbl_id, HTBL_CLASS tbl_class)
{
  JERRCODE jerr;

  jerr = m_dec->InitHuffmanTable(bits, vals, tbl_id, tbl_class);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGDecoder::InitHuffmanTable()


ExcStatus JPEGDecoder::AttachHuffmanTable(int tbl_id, HTBL_CLASS tbl_class, int comp_no)
{
  JERRCODE jerr;

  jerr = m_dec->AttachHuffmanTable(tbl_id, tbl_class, comp_no);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGDecoder::AttachHuffmanTable()


ExcStatus JPEGDecoder::InitQuantTable(Ipp8u  qnt[64], int tbl_id)
{
  JERRCODE jerr;

  jerr = m_dec->InitQuantTable(qnt, tbl_id);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGDecoder::InitQuantTable()


ExcStatus JPEGDecoder::InitQuantTable(Ipp16u  qnt[64], int tbl_id)
{
  JERRCODE jerr;

  jerr = m_dec->InitQuantTable(qnt, tbl_id);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGDecoder::InitQuantTable()


ExcStatus JPEGDecoder::AttachQuantTable(int tbl_id, int comp_no)
{
  JERRCODE jerr;

  jerr = m_dec->AttachQuantTable(tbl_id, comp_no);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGDecoder::InitQuantTable()


ExcStatus JPEGDecoder::SetParams(JPEGDecoderParams& params)
{
  m_dec->SetParams(params);

  return ExcStatusOk;
} // JPEGDecoder::SetParams()


void JPEGDecoder::GetJFIFApp0Resolution(JRESUNITS* units, int* xDensity, int* yDensity)
{
  m_dec->GetJFIFApp0Resolution(units, xDensity, yDensity);

  return;
} // JPEGDecoder::GetResolution()


int JPEGDecoder::IsAVI1App0Detected(void)
{
  return m_dec->IsAVI1App0Detected();
} // JPEGDecoder::IsAVI1App0Detected()


int JPEGDecoder::IsJFIFApp0Detected(void)
{
  return m_dec->IsJFIFApp0Detected();
} // JPEGDecoder::IsJFIFApp0Detected()


int JPEGDecoder::IsJFXXApp0Detected(void)
{
  return m_dec->IsJFXXApp0Detected();
} // JPEGDecoder::IsJFXXApp0Detected()


