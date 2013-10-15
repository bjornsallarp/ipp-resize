/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_BMP_DEC_H__
#define __UIC_BMP_DEC_H__

#ifndef __BMP_H__
#include "bmp.h"
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


namespace UIC {

class OwnBMPDecoder;

class UICAPI BMPDecoder : public BaseImageDecoder
{
public:
  BMPDecoder(void);
  virtual ~BMPDecoder(void);

  virtual const CodecVersion& GetVersion(void);

  virtual Ipp32u              SetNOfThreads      (Ipp32u ) { return 1; }
  virtual Ipp32u              NOfThreads         ()        { return 1; }

  virtual ExcStatus Init(void);
  virtual ExcStatus Close(void);

  virtual ExcStatus AttachStream(BaseStreamInput& stream);
  virtual ExcStatus DetachStream(void);

  virtual void AttachDiagnOut(BaseStreamDiagn& diagnOutput);
  virtual void DetachDiagnOut(void);

  virtual ExcStatus ReadHeader(
    ImageColorSpec&        colorSpec,
    ImageSamplingGeometry& geometry);

  virtual ExcStatus ReadData(
    const ImageDataPtr*   data,
    const ImageDataOrder& dataOrder);

protected:
  OwnBMPDecoder* m_dec;
};

} // namespace UIC

#endif // __UIC_BMP_DEC_H__

