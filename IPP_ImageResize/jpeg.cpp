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

#ifndef __JPEG_H__
#include "jpeg.h"
#endif
#ifndef __METADATA_H__
#include "metadata.h"
#endif
#ifndef __JPEGENC_H__
#include "jpegenc.h"
#endif
#ifndef __JPEGDEC_H__
#include "jpegdec.h"
#endif
#ifndef __UIC_JPEG_DEC_H__
#include "uic_jpeg_dec.h"
#endif
#ifndef __UIC_JPEG_ENC_H__
#include "uic_jpeg_enc.h"
#endif

using namespace UIC;


IM_ERROR ReadImageJPEG(
  BaseStreamInput& in,
  PARAMS_JPEG&     param,
  CIppImage&       image)
{
  int                   i;
  int                   du = 0;
  int                   dd_factor;
  int                   comment_size;
  Ipp8u*                comment;
  Image                 imageCn;
  JPEGDecoder           jpegdec;
  ImageDataPtr          dataPtr;
  ImageDataOrder        dataOrder;
  ImageColorSpec        colorSpec;
  ImageSamplingGeometry geometry;
  IM_COLOR              jcolor;
  IM_SAMPLING           jsampling;

  jpegdec.SetNOfThreads(param.nthreads);

  if(ExcStatusOk != jpegdec.Init())
    return IE_INIT;

  if(ExcStatusOk != jpegdec.AttachStream(in))
    return IE_ASTREAM;

  if(ExcStatusOk != jpegdec.ReadHeader(colorSpec,geometry))
    return IE_RHEADER;

  jcolor = (IM_COLOR)image.UicToIppColor(colorSpec.EnumColorSpace());

  switch(geometry.EnumSampling())
  {
  case S444:      jsampling = IS_444; break;
  case S422:      jsampling = IS_422; break;
  case S244:      jsampling = IS_244; break;
  case S411:      jsampling = IS_411; break;
  default:
  case SOther:    jsampling = IS_OTHER; break;
  }

  param.mode     = jpegdec.Mode();
  param.color    = jcolor;
  param.sampling = jsampling;

  comment_size = 0;
  comment      = 0;

  jpegdec.Comment(&comment, &comment_size);
  if(comment_size != 0)
  {
    param.comment_size = IPP_MIN(comment_size,MAX_JPEG_COMMENT_BUF);
    ippsCopy_8u(comment,param.comment,param.comment_size);
  }
  else
  {
    param.comment[0]   = '\0';
    param.comment_size = 0;
  }

  int nOfComponents = geometry.NOfComponents();

  if(JPEG_BASELINE == param.mode || JPEG_PROGRESSIVE == param.mode)
  {
    switch(param.dct_scale)
    {
    case JD_1_1:  dd_factor = 1;  break;
    case JD_1_2:  dd_factor = 2;  break;
    case JD_1_4:  dd_factor = 4;  break;
    case JD_1_8:  dd_factor = 8;  break;
    default:
      {
        param.dct_scale = JD_1_1;
        dd_factor = 1;
      }
    }
  }
  else
  {
    param.dct_scale = JD_1_1;
    dd_factor = 1;
  }

  JPEGDecoderParams jparam;

  jparam.SetDCTScaleFactor((JDD)param.dct_scale);
  jparam.SetDCTType(param.use_qdct);
  jparam.SetThreadingMode((JTMODE)param.tmode);

  jpegdec.SetParams(jparam);

  if(param.dct_scale != JD_1_1)
  {
    Rect       refgrid;
    Point      origin;
    RectSize   size;

    origin.SetX(0);
    origin.SetY(0);

    switch (param.dct_scale)
    {
    case JD_1_2:
      {
        size.SetWidth ((geometry.RefGridRect().Width()  + 1) >> 1);
        size.SetHeight((geometry.RefGridRect().Height() + 1) >> 1);
      }
      break;

    case JD_1_4:
      {
        size.SetWidth ((geometry.RefGridRect().Width()  + 3) >> 2);
        size.SetHeight((geometry.RefGridRect().Height() + 3) >> 2);
      }
       break;

    case JD_1_8:
       {
        size.SetWidth ((geometry.RefGridRect().Width()  + 7) >> 3);
        size.SetHeight((geometry.RefGridRect().Height() + 7) >> 3);
      }
       break;
    }

    refgrid.SetOrigin(origin);
    refgrid.SetSize(size);

    geometry.SetRefGridRect(refgrid);
    geometry.ReAlloc(nOfComponents);
  }

  geometry.SetEnumSampling(S444);

  if(colorSpec.DataRange()->BitDepth() + 1 <= 8)
  {
    dataOrder.SetDataType(T8u);
    du = sizeof(Ipp8u);
  }
  else
  {
    dataOrder.SetDataType(T16u);
    du = sizeof(Ipp16u);
  }

  dataOrder.ReAlloc(Interleaved, nOfComponents);
  dataOrder.PixelStep()[0] = nOfComponents;
  dataOrder.LineStep() [0] = geometry.RefGridRect().Width() * nOfComponents * du + BYTES_PAD(geometry.RefGridRect().Width(), nOfComponents, du);

  imageCn.ColorSpec().ReAlloc(nOfComponents);
  imageCn.ColorSpec().SetColorSpecMethod(Enumerated);
  imageCn.ColorSpec().SetComponentToColorMap(Direct);

  for(i = 0; i < nOfComponents; i++)
  {
    if(colorSpec.DataRange()->BitDepth() + 1 <= 8)
    {
      imageCn.ColorSpec().DataRange()[i].SetAsRange8u(255);
    }
    else
    {
      imageCn.ColorSpec().DataRange()[i].SetAsRange16u(1 << colorSpec.DataRange()->BitDepth());
    }
  }

  ImageEnumColorSpace in_color;
  ImageEnumColorSpace out_color;
  in_color = colorSpec.EnumColorSpace();

  switch(nOfComponents)
  {
  case 1:  out_color = (in_color == Unknown) ? Unknown : Grayscale; break;
  case 3:  out_color = (in_color == Unknown) ? Unknown : RGB;       break;
  case 4:  out_color = (in_color == Unknown) ? Unknown : CMYK;      break;
  default: out_color = Unknown;                                     break;
  }

  imageCn.ColorSpec().SetEnumColorSpace(out_color);

  IppiSize size;

  size.width  = geometry.RefGridRect().Width();
  size.height = geometry.RefGridRect().Height();

  if(0 != image.Alloc(size, nOfComponents, colorSpec.DataRange()->BitDepth() + 1, 1))
    return IE_ALLOC;

  image.Color((IM_COLOR)image.UicToIppColor(out_color));

  if(image.Precision() <= 8)
  {
    dataPtr.p8u = image;
  }
  else
  {
    dataPtr.p16s = image;
  }

  imageCn.Buffer().Attach(&dataPtr,dataOrder,geometry);

  if(ExcStatusOk != jpegdec.SetParams(imageCn.ColorSpec(),imageCn.Buffer().BufferFormat().SamplingGeometry()))
    return IE_PARAMS;

  if(ExcStatusOk != jpegdec.ReadData(imageCn.Buffer().DataPtr(),dataOrder))
    return IE_RDATA;

  return IE_OK;
} // ReadImageJPEG()


IM_ERROR SaveImageJPEG(
  CIppImage&        image,
  PARAMS_JPEG&      param,
  BaseStreamOutput& out)
{
  int                   i;
  Image                 imageCn;
  int                   du;
  int                   nOfComponents;
  Rect                  refgrid;
  Point                 origin;
  RectSize              size;
  JPEGEncoder           jpegenc;
  ImageDataPtr          dataPtr;
  ImageColorSpec        colorSpec;
  ImageDataOrder        dataOrder;
  ImageSamplingGeometry geometry;

  IM_COLOR color;

  jpegenc.SetNOfThreads(param.nthreads);

  if(ExcStatusOk != jpegenc.Init())
    return IE_INIT;

  if(ExcStatusOk != jpegenc.AttachStream(out))
    return IE_ASTREAM;

  nOfComponents = image.NChannels();

  if(ExcStatusOk != jpegenc.SetComment(param.comment_size, (char*)param.comment))
    return IE_PARAMS;

  if(image.Precision() <= 8)
  {
    dataOrder.SetDataType(T8u);
    du = sizeof(Ipp8u);
  }
  else
  {
    dataOrder.SetDataType(T16u);
    du = sizeof(Ipp16u);
  }

  size.SetWidth(image.Width());
  size.SetHeight(image.Height());

  origin.SetX(0);
  origin.SetY(0);

  refgrid.SetOrigin(origin);
  refgrid.SetSize(size);

  geometry.SetRefGridRect(refgrid);
  geometry.ReAlloc(nOfComponents);
  geometry.SetEnumSampling((ImageEnumSampling)image.Sampling());

  dataOrder.ReAlloc(Interleaved, nOfComponents);
  dataOrder.PixelStep()[0] = nOfComponents;
  dataOrder.LineStep() [0] = image.Step();

  imageCn.ColorSpec().ReAlloc(nOfComponents);
  imageCn.ColorSpec().SetColorSpecMethod(Enumerated);
  imageCn.ColorSpec().SetComponentToColorMap(Direct);

  for(i = 0; i < nOfComponents; i++)
  {
    if(image.Precision() <= 8)
    {
      imageCn.ColorSpec().DataRange()[i].SetAsRange8u(255);
    }
    else
    {
      imageCn.ColorSpec().DataRange()[i].SetAsRange16u((1 << (image.Precision()-1)));
    }
  }


  if(image.Color() == IC_BGRA)
  {
    int order[4] = {2, 1, 0, 3};
    image.SwapChannels(order);
    image.Color(IC_RGBA);
  }

  if(image.Precision() == 12)
    color = image.NChannels() == 1 ? IC_GRAY : IC_UNKNOWN;
  else
    color = (image.NChannels() == 4) ? ((image.Color() == IC_RGBA) ? IC_CMYK : image.Color()) : image.Color();


  imageCn.ColorSpec().SetEnumColorSpace((ImageEnumColorSpace)color);

  dataPtr.p8u = image;
  imageCn.Buffer().Attach(&dataPtr,dataOrder,geometry);

  if(ExcStatusOk != jpegenc.AttachImage(imageCn))
    return IE_AIMAGE;

  switch(param.mode)
  {
  case JPEG_BASELINE:
    {
      JPEGEncoderParamsBAS je_params;

      je_params.SetColor((JCOLOR)param.color);
      je_params.SetSampling((JSS)param.sampling);
      je_params.SetRestartInterval(param.restart_interval);
      je_params.SetHuffmanOption(param.huffman_opt);
      je_params.SetQuality(param.quality);
      je_params.SetThreading((JTMODE)param.tmode);

      if(ExcStatusOk != jpegenc.SetParams(je_params))
        return IE_PARAMS;
    }
    break;

  case JPEG_EXTENDED:
    {
      JPEGEncoderParamsEXT je_params;

      je_params.SetColor((JCOLOR)param.color);
      je_params.SetSampling((JSS)param.sampling);
      je_params.SetRestartInterval(param.restart_interval);
      je_params.SetQuality(param.quality);

      if(ExcStatusOk != jpegenc.SetParams(je_params))
        return IE_PARAMS;
    }
    break;

  case JPEG_PROGRESSIVE:
    {
      JPEGEncoderParamsPRG je_params;

      je_params.SetColor((JCOLOR)param.color);
      je_params.SetSampling((JSS)param.sampling);
      je_params.SetRestartInterval(param.restart_interval);
      je_params.SetHuffmanOption(param.huffman_opt);
      je_params.SetQuality(param.quality);

      if(ExcStatusOk != jpegenc.SetParams(je_params))
        return IE_PARAMS;
    }
    break;

  case JPEG_LOSSLESS:
    {
      JPEGEncoderParamsLSL je_params;

      je_params.SetColor((JCOLOR)param.color);
      je_params.SetSampling((JSS)param.sampling);
      je_params.SetRestartInterval(param.restart_interval);
      je_params.SetHuffmanOption(param.huffman_opt);
      je_params.SetPointTransform(param.point_transform);
      je_params.SetPredictor(param.predictor);

      if(ExcStatusOk != jpegenc.SetParams(je_params))
        return IE_PARAMS;
    }
    break;

  default:
    return IE_PARAMS;
  }

  if(ExcStatusOk != jpegenc.WriteHeader())
    return IE_WHEADER;

  if(ExcStatusOk != jpegenc.WriteData())
    return IE_WDATA;

  return IE_OK;
} // SaveImageJPEG()

