/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_BASE_IMAGE_DECODER_H__
#define __UIC_BASE_IMAGE_DECODER_H__

#include "uic_base_image_codec.h"
#include "uic_base_stream_input.h"

namespace UIC {


class UICAPI BaseImageDecoder : public BaseImageCodec
{
public:
    BaseImageDecoder() {}
    virtual ~BaseImageDecoder() {}

    virtual ExcStatus AttachStream(BaseStreamInput &stream) = 0;

    virtual ExcStatus ReadHeader  (
        ImageColorSpec        &colorSpec,
        ImageSamplingGeometry &geometry) = 0;

    virtual ExcStatus ReadData    (
        const ImageDataPtr   *data,
        const ImageDataOrder &dataOrder) = 0;
};

} // namespace UIC

#endif // __UIC_BASE_IMAGE_DECODER_H__

