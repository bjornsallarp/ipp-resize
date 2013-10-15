
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

#ifndef __RINGSTRIPE_H__
#define __RINGSTRIPE_H__

#include "ringbuffer.h"

template<class T>
class RingStripe
{
public:
    typedef T* TPTR;

    RingStripe() {}

    RingStripe(unsigned int width, unsigned int height) { ReAlloc(width, height); }

    void ReAlloc(unsigned int width, unsigned int height)
    {
        m_stripe.ReAlloc(height, width);

        m_ring.ReAlloc(height);

        for(unsigned int i = 0; i < height; i++)
            m_ring.SetEntry(m_stripe[i], i);
    }

    T*   CurrRow   ()                   const { return *m_ring; }
    T*   DelayedRow(unsigned int delay) const { return  m_ring.Delayed(delay); }

    void operator++() { ++m_ring; }
    void operator--() { --m_ring; }

    operator const TPTR*() const { return m_ring; }

    unsigned int Size   () const { return m_ring.Size (); }

protected:
    FixedBuffer2D<T>  m_stripe;
    RingBuffer<TPTR>  m_ring;
};


#endif // __RINGSTRIPE_H__
