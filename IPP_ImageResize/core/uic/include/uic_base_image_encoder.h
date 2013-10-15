/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_BASE_IMAGE_ENCODER_H__
#define __UIC_BASE_IMAGE_ENCODER_H__

#include "uic_base_image_codec.h"
#include "uic_base_stream_output.h"

namespace UIC {


class UICAPI BaseImageEncoder : public BaseImageCodec
{
public:
    BaseImageEncoder() {}
    virtual ~BaseImageEncoder() {}

    virtual ExcStatus AttachStream(BaseStreamOutput &stream ) = 0;

    virtual ExcStatus AttachImage (const Image      &image  ) = 0;
    virtual ExcStatus DetachImage ()                          = 0;

    virtual ExcStatus WriteHeader ()                          = 0;
    virtual ExcStatus WriteData   ()                          = 0;
};

} // namespace UIC

#endif // __UIC_BASE_IMAGE_ENCODER_H__
