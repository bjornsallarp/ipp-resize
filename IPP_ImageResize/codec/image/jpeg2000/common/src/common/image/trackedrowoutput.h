
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __TRACKEDROWOUTPUT_H__
#define __TRACKEDROWOUTPUT_H__

#include "ringstripe.h"

template<class T, class RowOutput>
class TrackedRowOutput
{
public:
    TrackedRowOutput() {}

    void ReAlloc     (unsigned int maxWidth, unsigned int maxDelay)       { m_stripe.ReAlloc(maxWidth, maxDelay + 1); }
    void Attach      (RowOutput& dst)                                     { m_dst = &dst; }

    T*   DelayedRow  (unsigned int delay)                           const {  return m_stripe.DelayedRow(delay); }

    operator T*const*()                                             const { return m_stripe; }

    void operator++  ()
    {
        m_dst->Write (m_stripe.CurrRow ());
        ++m_stripe;
    }

protected:
    RowOutput*    m_dst;
    RingStripe<T> m_stripe;
};

#endif // __TRACKEDROWOUTPUT_H__
