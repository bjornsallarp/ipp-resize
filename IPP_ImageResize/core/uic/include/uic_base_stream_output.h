/*
//
//                 INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_BASE_STREAM_OUTPUT_H__
#define __UIC_BASE_STREAM_OUTPUT_H__

#include "uic_base_stream.h"

namespace UIC {

class UICAPI BaseStreamOutput : public BaseStream
{
public:
    BaseStreamOutput(void) {}
    virtual ~BaseStreamOutput(void) {}

    virtual TStatus Write(const void* buf, TSize size, TSize& cnt) = 0;
    virtual TStatus Seek(TOffset offset, SeekOrigin origin) = 0;
};

} // namespace UIC

#endif // __UIC_BASE_STREAM_OUTPUT_H__
