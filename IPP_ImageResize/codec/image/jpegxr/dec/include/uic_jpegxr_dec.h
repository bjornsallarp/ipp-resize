/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_JPEGXR_DEC_H__
#define __UIC_JPEGXR_DEC_H__

#ifndef __IPPDEFS_H__
#include "ippdefs.h"
#endif
#ifndef __UIC_IMAGE_H__
#include "uic_image.h"
#endif
#ifndef __UIC_BASE_IMAGE_DECODER_H__
#include "uic_base_image_decoder.h"
#endif
#ifndef __UIC_BASE_STREAM_INPUT_H__
#include "uic_base_stream_input.h"
#endif

class OwnJPEGXRDecoder;

namespace UIC {

class UICAPI JPEGXRDecoder : public BaseImageDecoder
{
public:
  JPEGXRDecoder(void);  // ctor
  virtual ~JPEGXRDecoder(void); // dtor

  virtual ExcStatus Init(void);
  virtual ExcStatus Close(void);

  virtual ExcStatus AttachStream(BaseStreamInput& stream);
  virtual ExcStatus DetachStream(void) { return ExcStatusOk; }

  virtual const CodecVersion& GetVersion(void);

  virtual Ipp32u SetNOfThreads (Ipp32u nOfThreads);
  virtual Ipp32u NOfThreads    ();

  virtual void AttachDiagnOut(BaseStreamDiagn&) {}
  virtual void DetachDiagnOut(void) {}

  virtual ExcStatus SetParams(Ipp32u iForceColor, Ipp32u iBandsLimit, Ipp32u iMuliMode);

  virtual ExcStatus SetPlaneOffset(Ipp32u iImageOffset);

  virtual ExcStatus ReadFileHeader(
        Ipp32u& imageOffset,
        Ipp32u& alphaOffset,
        Ipp8u&  preAlpha);

  virtual ExcStatus ReadHeader(
        ImageColorSpec&        colorSpec,
        ImageSamplingGeometry& geometry);

  virtual ExcStatus ReadData(
        const ImageDataPtr*   data,
        const ImageDataOrder& dataOrder);

  virtual ExcStatus FreeData(void);

protected:
  OwnJPEGXRDecoder* m_dec;
};

} // namespace UIC

#endif // __UIC_JPEGXR_DEC_H__

