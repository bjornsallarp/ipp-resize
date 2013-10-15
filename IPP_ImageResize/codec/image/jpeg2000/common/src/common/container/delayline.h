
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2002-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __DELAYLINE_H__
#define __DELAYLINE_H__

#include "ringbuffer.h"

template<class T>
class DelayLine : private RingBuffer<T>
{
public:
    DelayLine() {}
    DelayLine(unsigned int size) : RingBuffer<T>(size) {}

    using RingBuffer<T>::ReAlloc;
    using RingBuffer<T>::Init;

    using RingBuffer<T>::Size;

    operator const T*() const { return RingBuffer<T>::operator const T*(); }

    void Push(const T&value)
    {
        SetEntry(value, this->Position());
        this->operator++();
    }
};

#endif // __DELAYLINE_H__
