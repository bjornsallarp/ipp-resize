/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_FIXED_ARRAY_H__
#define __UIC_FIXED_ARRAY_H__

#include "uic_fixed_buffer.h"
#include "uic_general_algorithm.h"

namespace UIC {

template <class T> class UICAPI FixedArray : public FixedBuffer<T> {
public:
    FixedArray() : m_size(0) {}

    ExcStatus ReAlloc(unsigned int size)
    {
        m_size = size;

        BEGIN_FINALIZE_RET_ON_EXC(FixedBuffer<T>::ReAlloc(m_size)) {
            m_size = 0;
        } END_FINALIZE_RET_ON_EXC

        return ExcStatusOk;
    }

    ExcStatus CreateCopy(const FixedArray &array)
    {
        RET_ON_EXC(ReAlloc(array.Size()));
        Copy(array, *this, m_size);

        return ExcStatusOk;
    }

    void Free()
    {
        FixedBuffer<T>::Free();
        m_size = 0;
    }

    Ipp32u Size() const { return m_size; }

protected:
    Ipp32u m_size;

private:
    FixedArray(const FixedArray &) {}
    FixedArray& operator=(const FixedArray&) {}
};

} // namespace UIC

#endif // __UIC_FIXED_ARRAY_H__
