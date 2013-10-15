/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_BASE_STREAM_INPUT_H__
#define __UIC_BASE_STREAM_INPUT_H__

#include "uic_base_stream.h"

namespace UIC {

class UICAPI BaseStreamInputFwd : public BaseStream
{
public:
    BaseStreamInputFwd() {}
    virtual ~BaseStreamInputFwd() {}

    virtual TStatus Read(void* buf, TSize size, TSize &cnt) = 0;
    virtual TStatus SeekFwd (TSize offset) = 0;
};


class UICAPI BaseStreamInput : public BaseStreamInputFwd
{
public:
    BaseStreamInput()          {}
    virtual ~BaseStreamInput() {}

    virtual TStatus Seek(TOffset offset, SeekOrigin origin) = 0;
    virtual TStatus SeekFwd(TSize   offset)  { return Seek((TOffset)offset, Current); }
};

} // namespace UIC

#endif // __UIC_BASE_STREAM_INPUT_H__


