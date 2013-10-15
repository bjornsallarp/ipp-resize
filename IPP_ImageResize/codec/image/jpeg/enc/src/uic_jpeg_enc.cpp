/*
//
//            INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/
#include "stdafx.h"
#ifndef __JPEGENC_H__
#include "jpegenc.h"
#endif
#ifndef __UIC_JPEG_ENC_H__
#include "uic_jpeg_enc.h"
#endif

#include <math.h>
#include "uic_general_algorithm.h"
#include "uic_math.h"

using namespace UIC;

static const UIC::CodecVersion version =
{
    0, //int         major;      /* e.g. 1                               */
    0, //int         minor;      /* e.g. 2                               */
    1, //int         micro;      /* e.g. 3                               */
    0, //int         build;      /* e.g. 10, always >= majorBuild        */
    "Intel(R) UIC JPEG Encoder", /* e.g. "Intel(R) JPEG Decoder"         */
    "v0.0.1.0 alpha",            /* e.g. "v1.2 Beta"                     */
    __DATE__                     /* e.g. "Jul 20 99"                     */
};


JPEGEncoder::JPEGEncoder(void)
{
  m_enc = 0;
  return;
} // ctor


JPEGEncoder::~JPEGEncoder(void)
{
  if(0 != m_enc)
  {
    delete m_enc;
    m_enc = 0;
  }

  return;
} // dtor


Ipp32u JPEGEncoder::SetNOfThreads(Ipp32u nOfThreads)
{
    set_num_threads(nOfThreads);
    return NOfThreads();
}

Ipp32u JPEGEncoder::NOfThreads   ()
{
    return get_num_threads();
}



ExcStatus JPEGEncoder::Init(void)
{
  if(0 == m_enc)
  {
    m_enc = new CJPEGEncoder;
  }

  return ExcStatusOk;
} // JPEGEncoder::Init()


ExcStatus JPEGEncoder::Close(void)
{
  return ExcStatusOk;
} // JPEGEncoder::Close()


const CodecVersion& JPEGEncoder::GetVersion(void)
{
  return version;
} // JPEGEncoder::GetVersion()


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


ExcStatus JPEGEncoder::AttachImage(const Image& image)
{
  JERRCODE jerr;

  const UIC::ImageColorSpec&        imgclr = image.ColorSpec();
  const UIC::ImageBuffer&           imgbuf = image.Buffer();
  const UIC::ImageBufferFormat&     imgfmt = imgbuf.BufferFormat();
  const UIC::ImageSamplingGeometry& imgeom = imgfmt.SamplingGeometry();
  const UIC::ImageDataOrder&        imgord = imgfmt.DataOrder();
  const UIC::ImageDataPtr*          imgptr = imgbuf.DataPtr();
//  const UIC::ImagePalette&          imgpal = imgclr.Palette();
  UIC::RectSize                     imgroi = imgeom.RefGridRect().Size();

  if(imgord.DataType() != UIC::T8u && imgord.DataType() != UIC::T16u)
    return ExcStatusFail;

  int*   srcStep       = imgord.LineStep();
  int    nOfComponents = imgeom.NOfComponents();
  JSS    sampling      = u2j_smap[imgeom.EnumSampling()];
  JCOLOR color         = u2j_cmap[imgclr.EnumColorSpace()];
  int    precision     = imgclr.DataRange()[0].BitDepth() + 1;

  if(imgord.ComponentOrder() == UIC::Interleaved)
  {
  if(imgord.DataType() == UIC::T8u)
  {
    const Ipp8u* pSrc  = imgptr->p8u;

    jerr = m_enc->SetSource((Ipp8u*)pSrc,*srcStep,imgroi,nOfComponents,color,sampling,precision);
    if(JPEG_OK != jerr)
      return ExcStatusFail;
  }
  else if(imgord.DataType() == UIC::T16u)
  {
    const Ipp16u* pSrc  = imgptr->p16u;

    jerr = m_enc->SetSource((Ipp16s*)pSrc,*srcStep,imgroi,nOfComponents,color,sampling,precision);
    if(JPEG_OK != jerr)
        return ExcStatusFail;
    }
  }
  else
  {
    if(nOfComponents == 1)
      return ExcStatusFail;

    if(imgord.DataType() == UIC::T8u)
    {
      const Ipp8u* pSrc[4];
      for(int i = 0 ; i < nOfComponents; i++)
        pSrc[i] = imgbuf.DataPtr()[i].p8u;

      jerr = m_enc->SetSource((Ipp8u**)pSrc,srcStep,imgroi,nOfComponents,color,sampling,precision);
      if(JPEG_OK != jerr)
        return ExcStatusFail;
    }
    else if(imgord.DataType() == UIC::T16u)
    {
      const Ipp16u* pSrc[4];
      for(int i = 0 ; i < nOfComponents; i++)
        pSrc[i] = imgbuf.DataPtr()[i].p16u;

      jerr = m_enc->SetSource((Ipp16s**)pSrc,srcStep,imgroi,nOfComponents,color,sampling,precision);
      if(JPEG_OK != jerr)
        return ExcStatusFail;
    }
  }

  return ExcStatusOk;
} // JPEGEncoder::AttachImage()


ExcStatus JPEGEncoder::DetachImage(void)
{
  return ExcStatusOk;
} // JPEGEncoder::DetachImage()


ExcStatus JPEGEncoder::AttachStream(UIC::BaseStreamOutput& stream)
{
  JERRCODE jerr;
  jerr = m_enc->SetDestination(&stream);
  return ExcStatusOk;
} // JPEGEncoder::AttachStream()


ExcStatus JPEGEncoder::DetachStream(void)
{
  return ExcStatusOk;
} // JPEGEncoder::DetachStream()


ExcStatus JPEGEncoder::SetParams(
  JMODE  mode,
  JCOLOR color,
  JSS    sampling,
  int    restart_interval,
  int    huff_opt,
  int    quality)
{
  JERRCODE jerr;

  jerr = m_enc->SetParams(mode,color,sampling,restart_interval,huff_opt,quality);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::SetParams()


ExcStatus JPEGEncoder::SetParams(
  JMODE  mode,
  JCOLOR color,
  JSS    sampling,
  int    restart_interval,
  int    huff_opt,
  int    point_transform,
  int    predictor)
{
  JERRCODE jerr;

  jerr = m_enc->SetParams(mode,color,sampling,restart_interval,huff_opt,point_transform,predictor);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::SetParams()


ExcStatus JPEGEncoder::SetParams(JPEGEncoderParamsBAS je_params)
{
  int      xdensity;
  int        ydensity;
  JERRCODE   jerr;
  JRESUNITS  units;

  jerr = m_enc->SetParams(JPEG_BASELINE,
                          u2j_cmap[je_params.GetColor()],
                          u2j_smap[je_params.GetSampling()],
                          je_params.GetRestartInterval(),
                          je_params.GetHuffmanOption(),
                          je_params.GetQuality(),
                          je_params.GetThreading());
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  je_params.GetJFIFApp0Resolution(&units, &xdensity, &ydensity);

  jerr = m_enc->SetJFIFApp0Resolution(units, xdensity, ydensity);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::SetParams()


ExcStatus JPEGEncoder::SetParams(JPEGEncoderParamsPRG je_params)
{
  int        xdensity;
  int        ydensity;
  JERRCODE   jerr;
  JRESUNITS  units;

  jerr = m_enc->SetParams(JPEG_PROGRESSIVE,
                          u2j_cmap[je_params.GetColor()],
                          u2j_smap[je_params.GetSampling()],
                          je_params.GetRestartInterval(),
                          je_params.GetHuffmanOption(),
                          je_params.GetQuality());
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  je_params.GetJFIFApp0Resolution(&units, &xdensity, &ydensity);

  jerr = m_enc->SetJFIFApp0Resolution(units, xdensity, ydensity);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::SetParams()


ExcStatus JPEGEncoder::SetParams(JPEGEncoderParamsEXT je_params)
{
  int        xdensity;
  int        ydensity;
  JERRCODE   jerr;
  JRESUNITS  units;

  jerr = m_enc->SetParams(JPEG_EXTENDED,
                          u2j_cmap[je_params.GetColor()],
                          u2j_smap[je_params.GetSampling()],
                          je_params.GetRestartInterval(),
                          1,
                          je_params.GetQuality());
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  je_params.GetJFIFApp0Resolution(&units, &xdensity, &ydensity);

  jerr = m_enc->SetJFIFApp0Resolution(units, xdensity, ydensity);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::SetParams()


ExcStatus JPEGEncoder::SetParams(JPEGEncoderParamsLSL je_params)
{
  JERRCODE jerr;

  jerr = m_enc->SetParams(JPEG_LOSSLESS,
                          u2j_cmap[je_params.GetColor()],
                          u2j_smap[je_params.GetSampling()],
                          je_params.GetRestartInterval(),
                          je_params.GetHuffmanOption(),
                          je_params.GetPointTransform(),
                          je_params.GetPredictor());
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::SetParams()


ExcStatus JPEGEncoder::WriteHeader(void)
{
  JERRCODE jerr;

  jerr = m_enc->WriteHeader();
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::WriteHeader()


ExcStatus JPEGEncoder::WriteData(void)
{
  JERRCODE jerr;

  jerr = m_enc->WriteData();
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::WriteData()


ExcStatus JPEGEncoder::SetComment(int comment_size, char* comment)
{
  JERRCODE jerr;

  jerr = m_enc->SetComment(comment_size, comment);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::SetComment()


ExcStatus JPEGEncoder::InitHuffmanTable(Ipp8u bits[], Ipp8u vals[], int tbl_id, HTBL_CLASS tbl_class)
{
  JERRCODE jerr;

  jerr = m_enc->InitHuffmanTable(bits, vals, tbl_id, tbl_class);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::InitHuffmanTable()


ExcStatus JPEGEncoder::AttachHuffmanTable(int tbl_id, HTBL_CLASS tbl_class, int comp_no)
{
  JERRCODE jerr;

  jerr = m_enc->AttachHuffmanTable(tbl_id, tbl_class, comp_no);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::AttachHuffmanTable()


ExcStatus JPEGEncoder::InitQuantTable(Ipp8u  qnt[64], int tbl_id, int quality)
{
  JERRCODE jerr;

  jerr = m_enc->InitQuantTable(qnt, tbl_id, quality);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::InitQuantTable()


ExcStatus JPEGEncoder::InitQuantTable(Ipp16u  qnt[64], int tbl_id, int quality)
{
  JERRCODE jerr;

  jerr = m_enc->InitQuantTable(qnt, tbl_id,quality);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::InitQuantTable()


ExcStatus JPEGEncoder::AttachQuantTable(int tbl_id, int comp_no)
{
  JERRCODE jerr;

  jerr = m_enc->AttachQuantTable(tbl_id, comp_no);
  if(JPEG_OK != jerr)
    return ExcStatusFail;

  return ExcStatusOk;
} // JPEGEncoder::InitQuantTable()


int JPEGEncoder::NumOfBytes(void)
{
  return m_enc->NumOfBytes();
} // JPEGEncoder::NumOfBytes()

