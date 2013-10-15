/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//         Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_BASE_STREAM_H__
#define __UIC_BASE_STREAM_H__

#include "uic_defs.h"


namespace UIC {

class UICAPI BaseStream
{
public:
    typedef Ipp64u TSize;
    typedef Ipp64u TPosition;
    typedef Ipp64s TOffset;

    typedef enum
    {
        Current,
        Beginning,
        End
    } SeekOrigin;

    typedef enum
    {
        StatusOk,
        StatusFail
    } TStatus;

    static bool IsOk(TStatus status) { return status == StatusOk; }

    BaseStream(void) {}
    virtual ~BaseStream(void) {}

    virtual TStatus Size    (TSize      &size) = 0;
    virtual TStatus Position(TPosition  &pos ) = 0;
};

} // namespace UIC

#endif // __UIC_BASE_STREAM_H__

