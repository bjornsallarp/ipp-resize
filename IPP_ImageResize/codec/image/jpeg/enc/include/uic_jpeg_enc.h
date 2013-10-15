/*
//
//              INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_JPEG_ENC_H__
#define __UIC_JPEG_ENC_H__

#ifndef __JPEGBASE_H__
#include "jpegbase.h"
#endif
#ifndef __UIC_EXC_STATUS_H__
#include "uic_exc_status.h"
#endif
#ifndef __UIC_IMAGE_H__
#include "uic_image.h"
#endif
#ifndef __UIC_BASE_STREAM_DIAGN_H__
#include "uic_base_stream_diagn.h"
#endif
#ifndef __UIC_BASE_STREAM_OUTPUT_H__
#include "uic_base_stream_output.h"
#endif
#ifndef __UIC_BASE_IMAGE_ENCODER_H__
#include "uic_base_image_encoder.h"
#endif


class CJPEGEncoder;

namespace UIC {

class UICAPI JPEGEncoderParamsBAS
{
public:
  JPEGEncoderParamsBAS(void)
  {
    m_color        = JC_YCBCR;
    m_sampling     = JS_444;
    m_rstInterval  = 0;
    m_huffOpt      = 0;
    m_quality      = 75;

    m_app0_units    = JRU_NONE;
    m_app0_xdensity = 1;
    m_app0_ydensity = 1;

    m_threading_mode = JT_OLD;
  }

  virtual ~JPEGEncoderParamsBAS(void) {}

  void SetColor(JCOLOR color)              { m_color        = color;        return; }
  void SetSampling(JSS sampling)           { m_sampling     = sampling;     return; }
  void SetRestartInterval(int rstInterval) { m_rstInterval  = rstInterval;  return; }
  void SetHuffmanOption(int huffOpt)       { m_huffOpt      = huffOpt;      return; }
  void SetQuality(int quality)             { m_quality      = quality;      return; }
  void SetThreading(JTMODE mode)           { m_threading_mode = mode;         return; }

  void SetJFIFApp0Resolution(JRESUNITS units, int xdensity, int ydensity)
  {
    m_app0_units    = units;
    m_app0_xdensity = xdensity;
    m_app0_ydensity = ydensity;

    return;
  }

  JCOLOR GetColor(void)           { return m_color; }
  JSS    GetSampling(void)        { return m_sampling; }
  int    GetRestartInterval(void) { return m_rstInterval; }
  int    GetHuffmanOption(void)   { return m_huffOpt; }
  int    GetQuality(void)         { return m_quality; }
  JTMODE GetThreading(void)       { return m_threading_mode;}

  void  GetJFIFApp0Resolution(JRESUNITS* units, int* xdensity, int* ydensity)
  {
    *units    = m_app0_units;
    *xdensity = m_app0_xdensity;
    *ydensity = m_app0_ydensity;

    return;
  }

  JPEGEncoderParamsBAS& operator = (const JPEGEncoderParamsBAS& param)
  {
    m_color         = param.m_color;
    m_sampling      = param.m_sampling;
    m_rstInterval   = param.m_rstInterval;
    m_huffOpt       = param.m_huffOpt;
    m_quality       = param.m_quality;
    m_app0_units    = param.m_app0_units;
    m_app0_xdensity = param.m_app0_xdensity;
    m_app0_ydensity = param.m_app0_ydensity;

    return *this;
  }

private:
  JCOLOR     m_color;
  JSS        m_sampling;
  int        m_rstInterval;
  int        m_huffOpt;
  int        m_quality;
  int        m_app0_xdensity;
  int        m_app0_ydensity;
  JRESUNITS  m_app0_units;
  JTMODE     m_threading_mode;
};


class UICAPI JPEGEncoderParamsEXT
{
public:
  JPEGEncoderParamsEXT(void)
  {
    m_color         = JC_YCBCR;
    m_sampling      = JS_444;
    m_rstInterval   = 0;
    m_huffOpt       = 1;
    m_quality       = 75;
    m_app0_units    = JRU_NONE;
    m_app0_xdensity = 1;
    m_app0_ydensity = 1;
  }

  virtual ~JPEGEncoderParamsEXT(void) {}

  void SetColor(JCOLOR color)              { m_color        = color;        return; }
  void SetSampling(JSS sampling)           { m_sampling     = sampling;     return; }
  void SetRestartInterval(int rstInterval) { m_rstInterval  = rstInterval;  return; }
  void SetQuality(int quality)             { m_quality      = quality;      return; }

  void SetJFIFApp0Resolution(JRESUNITS units, int xdensity, int ydensity)
  {
    m_app0_units    = units;
    m_app0_xdensity = xdensity;
    m_app0_ydensity = ydensity;

    return;
  }

  JCOLOR GetColor(void)           { return m_color; }
  JSS    GetSampling(void)        { return m_sampling; }
  int    GetRestartInterval(void) { return m_rstInterval; }
  int    GetHuffmanOption(void)   { return m_huffOpt; }
  int    GetQuality(void)         { return m_quality; }

  void  GetJFIFApp0Resolution(JRESUNITS *units, int *xdensity, int *ydensity)
  {
    *units    = m_app0_units;
    *xdensity = m_app0_xdensity;
    *ydensity = m_app0_ydensity;

    return;
  }

  JPEGEncoderParamsEXT& operator = (const JPEGEncoderParamsEXT& param)
  {
    m_color         = param.m_color;
    m_sampling      = param.m_sampling;
    m_rstInterval   = param.m_rstInterval;
    m_huffOpt       = param.m_huffOpt;
    m_quality       = param.m_quality;
    m_app0_units    = param.m_app0_units;
    m_app0_xdensity = param.m_app0_xdensity;
    m_app0_ydensity = param.m_app0_ydensity;

    return *this;
  }

private:
  JCOLOR     m_color;
  JSS        m_sampling;
  int        m_rstInterval;
  int        m_huffOpt;
  int        m_quality;
  int        m_app0_xdensity;
  int        m_app0_ydensity;
  JRESUNITS  m_app0_units;
};


class UICAPI JPEGEncoderParamsPRG
{
public:
  JPEGEncoderParamsPRG(void)
  {
    m_color         = JC_YCBCR;
    m_sampling      = JS_444;
    m_rstInterval   = 0;
    m_huffOpt       = 0;
    m_quality       = 75;
    m_app0_units    = JRU_NONE;
    m_app0_xdensity = 1;
    m_app0_ydensity = 1;
  }

  virtual ~JPEGEncoderParamsPRG(void) {}

  void SetColor(JCOLOR color)              { m_color        = color;        return; }
  void SetSampling(JSS sampling)           { m_sampling     = sampling;     return; }
  void SetRestartInterval(int rstInterval) { m_rstInterval  = rstInterval;  return; }
  void SetHuffmanOption(int huffOpt)       { m_huffOpt      = huffOpt;      return; }
  void SetQuality(int quality)             { m_quality      = quality;      return; }

  void SetJFIFApp0Resolution(JRESUNITS units, int xdensity, int ydensity)
  {
    m_app0_units    = units;
    m_app0_xdensity = xdensity;
    m_app0_ydensity = ydensity;

    return;
  }

  JCOLOR GetColor(void)           { return m_color; }
  JSS    GetSampling(void)        { return m_sampling; }
  int    GetRestartInterval(void) { return m_rstInterval; }
  int    GetHuffmanOption(void)   { return m_huffOpt; }
  int    GetQuality(void)         { return m_quality; }

  void  GetJFIFApp0Resolution(JRESUNITS *units, int *xdensity, int *ydensity)
  {
    *units    = m_app0_units;
    *xdensity = m_app0_xdensity;
    *ydensity = m_app0_ydensity;

    return;
  }

  JPEGEncoderParamsPRG& operator = (const JPEGEncoderParamsPRG& param)
  {
    m_color         = param.m_color;
    m_sampling      = param.m_sampling;
    m_rstInterval   = param.m_rstInterval;
    m_huffOpt       = param.m_huffOpt;
    m_quality       = param.m_quality;
    m_app0_units    = param.m_app0_units;
    m_app0_xdensity = param.m_app0_xdensity;
    m_app0_ydensity = param.m_app0_ydensity;

    return *this;
  }

private:
  JCOLOR     m_color;
  JSS        m_sampling;
  int        m_rstInterval;
  int        m_huffOpt;
  int        m_quality;
  int        m_app0_xdensity;
  int        m_app0_ydensity;
  JRESUNITS  m_app0_units;
};


class UICAPI JPEGEncoderParamsLSL
{
public:
  JPEGEncoderParamsLSL(void)
  {
    m_color        = JC_YCBCR;
    m_sampling     = JS_444;
    m_rstInterval  = 0;
    m_huffOpt      = 0;
    m_pntTransform = 0;
    m_predictor    = 1;
  }

  virtual ~JPEGEncoderParamsLSL(void) {}

  void SetColor(JCOLOR color)              { m_color        = color;        return; }
  void SetSampling(JSS sampling)           { m_sampling     = sampling;     return; }
  void SetRestartInterval(int rstInterval) { m_rstInterval  = rstInterval;  return; }
  void SetHuffmanOption(int huffOpt)       { m_huffOpt      = huffOpt;      return; }
  void SetPointTransform(int pntTransform) { m_pntTransform = pntTransform; return; }
  void SetPredictor(int predictor)         { m_predictor    = predictor;    return; }

  JCOLOR GetColor(void)           { return m_color; }
  JSS    GetSampling(void)        { return m_sampling; }
  int    GetRestartInterval(void) { return m_rstInterval; }
  int    GetHuffmanOption(void)   { return m_huffOpt; }
  int    GetPointTransform(void)  { return m_pntTransform; }
  int    GetPredictor(void)       { return m_predictor; }

  JPEGEncoderParamsLSL& operator = (const JPEGEncoderParamsLSL& param)
  {
    m_color         = param.m_color;
    m_sampling      = param.m_sampling;
    m_rstInterval   = param.m_rstInterval;
    m_huffOpt       = param.m_huffOpt;
    m_pntTransform  = param.m_pntTransform;
    m_predictor     = param.m_predictor;

    return *this;
  }

private:
  JCOLOR m_color;
  JSS    m_sampling;
  int    m_rstInterval;
  int    m_huffOpt;
  int    m_pntTransform;
  int    m_predictor;
};


class UICAPI JPEGEncoder : public BaseImageEncoder
{
public:
  JPEGEncoder(void);
  virtual ~JPEGEncoder(void);

  ExcStatus Init(void);
  ExcStatus Close(void);

  void AttachDiagnOut(BaseStreamDiagn&) {}
  void DetachDiagnOut(void) {}

  ExcStatus AttachStream(BaseStreamOutput& stream);
  ExcStatus DetachStream(void);

  ExcStatus AttachImage(const Image& image);
  ExcStatus DetachImage(void);

  const CodecVersion& GetVersion(void);
  char* GetStatusString(ExcStatus) { return 0; };

  virtual Ipp32u SetNOfThreads (Ipp32u nOfThreads);
  virtual Ipp32u NOfThreads    ();

  ExcStatus SetParams(
             JMODE mode,
             JCOLOR color,
             JSS sampling,
             int restart_interval,
             int huff_opt,
             int quality);

  ExcStatus SetParams(
             JMODE mode,
             JCOLOR color,
             JSS sampling,
             int restart_interval,
             int huff_opt,
             int point_transform,
             int predictor);

  ExcStatus SetParams(JPEGEncoderParamsBAS je_params);
  ExcStatus SetParams(JPEGEncoderParamsPRG je_params);
  ExcStatus SetParams(JPEGEncoderParamsEXT je_params);
  ExcStatus SetParams(JPEGEncoderParamsLSL je_params);

  ExcStatus WriteHeader(void);
  ExcStatus WriteData(void);

  ExcStatus SetComment(int comment_size, char* comment = 0);

  ExcStatus InitHuffmanTable(Ipp8u bits[16], Ipp8u vals[256], int tbl_id, HTBL_CLASS tbl_class);
  ExcStatus InitQuantTable(Ipp8u  qnt[64], int tbl_id, int quality);
  ExcStatus InitQuantTable(Ipp16u qnt[64], int tbl_id, int quality);

  ExcStatus AttachHuffmanTable(int tbl_id, HTBL_CLASS tbl_class, int comp_no);
  ExcStatus AttachQuantTable(int tbl_id, int comp_no);

  int NumOfBytes(void);

protected:
  CJPEGEncoder* m_enc;
};

} // namespace UIC

#endif // __UIC_JPEG_ENC_H__

