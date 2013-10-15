/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_BMP_ENC_H__
#define __UIC_BMP_ENC_H__

#ifndef __BMP_H__
#include "bmp.h"
#endif
#ifndef __UIC_IMAGE_H__
#include "uic_image.h"
#endif
#ifndef __UIC_BASE_IMAGE_encoder_H__
#include "uic_base_image_encoder.h"
#endif
#ifndef __UIC_BASE_STREAM_OUTPUT_H__
#include "uic_base_stream_output.h"
#endif


namespace UIC {

class OwnBMPEncoder;

class UICAPI BMPEncoder : public BaseImageEncoder
{
public:
    BMPEncoder(void);
    virtual ~BMPEncoder(void);

    virtual ExcStatus           Init(void);
    virtual ExcStatus           Close(void);

    virtual const CodecVersion& GetVersion(void);

    virtual Ipp32u              SetNOfThreads(Ipp32u) { return 1; }
    virtual Ipp32u              NOfThreads(void)      { return 1; }

    virtual void                AttachDiagnOut(BaseStreamDiagn& diagnOutput);
    virtual void                DetachDiagnOut(void);

    virtual ExcStatus           AttachStream(BaseStreamOutput& stream);
    virtual ExcStatus           DetachStream(void);

    virtual ExcStatus           AttachImage(const Image& image);
    virtual ExcStatus           DetachImage(void);

    virtual ExcStatus           WriteHeader(void);
    virtual ExcStatus           WriteData(void);

    // error & warning codes that can be placed to BaseStreamDiagn

protected:
    OwnBMPEncoder* m_encoder;
};

} // namespace UIC

#endif // __UIC_BMP_ENC_H__

