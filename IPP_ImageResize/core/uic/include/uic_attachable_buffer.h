/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_ATTACHABLE_BUFFER_H__
#define __UIC_ATTACHABLE_BUFFER_H__

#include "uic_fixed_buffer.h"

namespace UIC {


template <class T> class AttachableBuffer {
public:
    AttachableBuffer() : m_data(0) {}

    ExcStatus ReAlloc(Ipp32u size)
    {
        BEGIN_FINALIZE_RET_ON_EXC(m_buffer.ReAlloc(size)) {
            m_data = 0;
        } END_FINALIZE_RET_ON_EXC

        m_data = m_buffer;

        return ExcStatusOk;
    }

    void Attach(T *ptr)
    {
        m_buffer.Free();
        m_data = ptr;
    }

    void FreeOrDetach()
    {
        m_buffer.Free();
        m_data = 0;
    }

    bool IsOwn() { return (((T*)m_buffer) != 0); }

    operator T*() const { return m_data; }

protected:
    FixedBuffer<T> m_buffer;
    T             *m_data;
};

} // namespace UIC

#endif // __UIC_ATTACHABLE_BUFFER_H__
