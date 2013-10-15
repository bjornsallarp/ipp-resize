/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_JPEGXR_ENC_H__
#define __UIC_JPEGXR_ENC_H__

#ifndef __IPPDEFS_H__
#include "ippdefs.h"
#endif
#ifndef __UIC_IMAGE_H__
#include "uic_image.h"
#endif
#ifndef __UIC_BASE_IMAGE_ENCODER_H__
#include "uic_base_image_encoder.h"
#endif
#ifndef __UIC_BASE_STREAM_OUTPUT_H__
#include "uic_base_stream_output.h"
#endif

class OwnJPEGXREncoder;

namespace UIC {


typedef struct InputParams
{
  Ipp8u   iQuality;
  Ipp8u   iOverlap;
  Ipp8u   iBands;
  Ipp8u   iSampling;
  Ipp8u   iTrim;
  Ipp8u   iShift;
  Ipp8u   bFrequency;
  Ipp8u   bCMYKD;
  Ipp8u   bAlphaPlane;
  Ipp16u* pTilesUniform;

} InputParams;


class UICAPI JPEGXREncoder : public BaseImageEncoder
{
public:
  JPEGXREncoder(void);  // ctor
  virtual ~JPEGXREncoder(void); // dtor

  virtual const CodecVersion& GetVersion(void);

  virtual ExcStatus Init(void);
  virtual ExcStatus Close(void);

  virtual ExcStatus AttachStream(BaseStreamOutput& stream);
  virtual ExcStatus DetachStream(void);

  virtual Ipp32u SetNOfThreads (Ipp32u nOfThreads);
  virtual Ipp32u NOfThreads    ();

  virtual void AttachDiagnOut(BaseStreamDiagn& diagnOutput);
  virtual void DetachDiagnOut(void);

  virtual ExcStatus SetParams(InputParams &params);

  virtual ExcStatus AttachImage(const Image& image);
  virtual ExcStatus DetachImage(void);

  virtual ExcStatus WriteFileHeader(Ipp32u bPAlphaPresent);
  virtual ExcStatus WriteHeader(void);
  virtual ExcStatus WriteData(void);
  virtual ExcStatus FreeData(void);

protected:
  OwnJPEGXREncoder* m_enc;
};

} // namespace UIC

#endif // __UIC_JPEGXR_ENC_H__

