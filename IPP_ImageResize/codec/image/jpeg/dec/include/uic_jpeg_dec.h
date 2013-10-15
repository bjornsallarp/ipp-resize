/*
//
//             INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_JPEG_DEC_H__
#define __UIC_JPEG_DEC_H__

#ifndef __UIC_IMAGE_H__
#include "uic_image.h"
#endif
#ifndef __UIC_BASE_IMAGE_DECODER_H__
#include "uic_base_image_decoder.h"
#endif
#ifndef __UIC_BASE_STREAM_INPUT_H__
#include "uic_base_stream_input.h"
#endif
#ifndef __JPEG_DEC_H__
#include "jpegbase.h"
#endif


class OwnJPEGDecoder;

namespace UIC {

class UICAPI JPEGDecoderParams
{
public:
  JPEGDecoderParams(void) :
    m_dctScaleFactor(JD_1_1), m_dctType(0), m_threading_mode(JT_OLD) {}
  virtual ~JPEGDecoderParams(void) {}

  ExcStatus SetDCTScaleFactor(JDD scaleFactor) { m_dctScaleFactor = scaleFactor; return ExcStatusOk; }
  ExcStatus SetDCTType(int dctType)            { m_dctType        = dctType;     return ExcStatusOk; }
  ExcStatus SetThreadingMode(JTMODE mode)      { m_threading_mode = mode;        return ExcStatusOk; }

  JDD GetDCTScaleFactor(void)    { return m_dctScaleFactor; }
  int GetDCTType(void)           { return m_dctType; }
  JTMODE GetThreadingMode(void)  { return m_threading_mode; }

  JPEGDecoderParams& operator = (const JPEGDecoderParams& param)
  {
    m_dctScaleFactor = param.m_dctScaleFactor;
    m_dctType        = param.m_dctType;
    m_threading_mode = param.m_threading_mode;

    return *this;
  }

protected:
  JDD    m_dctScaleFactor;
  int    m_dctType;
  JTMODE m_threading_mode;
};


class UICAPI JPEGDecoder : public BaseImageDecoder
{
public:
  JPEGDecoder(void);
  virtual ~JPEGDecoder(void);

  ExcStatus Init(void);
  ExcStatus Close(void);

  ExcStatus AttachStream(BaseStreamInput& stream);
  ExcStatus DetachStream(void);

  const CodecVersion& GetVersion(void);
  char* GetStatusString(ExcStatus status);

  virtual Ipp32u SetNOfThreads (Ipp32u nOfThreads);
  virtual Ipp32u NOfThreads    ();

  void AttachDiagnOut(BaseStreamDiagn&) {}
  void DetachDiagnOut(void) {}

  ExcStatus ReadHeader(
        ImageColorSpec&        colorSpec,
        ImageSamplingGeometry& geometry);

  ExcStatus SetParams(
        const ImageColorSpec&        colorSpec,
        const ImageSamplingGeometry& geometry);

  ExcStatus SetParams(JPEGDecoderParams& params);

  ExcStatus ReadData(
        const ImageDataPtr*   data,
        const ImageDataOrder& dataOrder);

  void  Comment(Ipp8u** buf, int* size);
  JMODE Mode(void);

  ExcStatus InitHuffmanTable(Ipp8u bits[16], Ipp8u vals[256], int tbl_id, HTBL_CLASS tbl_class);
  ExcStatus InitQuantTable(Ipp8u  qnt[64], int tbl_id);
  ExcStatus InitQuantTable(Ipp16u qnt[64], int tbl_id);

  ExcStatus AttachHuffmanTable(int tbl_id, HTBL_CLASS tbl_class, int comp_no);
  ExcStatus AttachQuantTable(int tbl_id, int comp_no);

  void GetJFIFApp0Resolution(JRESUNITS* units, int* xDensity, int* yDensity);

  int IsJFIFApp0Detected(void);
  int IsJFXXApp0Detected(void);
  int IsAVI1App0Detected(void);

protected:
  OwnJPEGDecoder*  m_dec;
};

}

#endif // __UIC_JPEG_DEC_H__

