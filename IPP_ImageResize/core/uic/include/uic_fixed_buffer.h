/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_FIXED_BUFFER_H__
#define __UIC_FIXED_BUFFER_H__

#include "uic_exc_status.h"
#include "uic_defs.h"
#include "uic_new.h"

namespace UIC {

template <class T> class UICAPI FixedBuffer {
public:
    FixedBuffer() : m_data(0) {}
    ~FixedBuffer() { Free(); }

    ExcStatus ReAlloc(Ipp32u size)
    {
        Free();
        return Alloc(size);
    }

    void Free()
    {
        if(m_data) DeleteArr(m_data);
        m_data = 0;
    }

    operator T*() const { return m_data; }

protected:
    ExcStatus Alloc(Ipp32u size)
    {
        m_data = size ? NewArr<T> (size) : 0;

        if(!m_data) return ExcStatusFail;

        return ExcStatusOk;
    }

    T *m_data;
private:
    FixedBuffer(const FixedBuffer &) {}
    FixedBuffer& operator=(const FixedBuffer&) {}
};

template <class T> class UICAPI FixedBuffer2D {
public:
    FixedBuffer2D() {}

    ExcStatus ReAlloc(unsigned int nOfRows, unsigned int nOfCols)
    {
        Free();
        return Alloc(nOfRows, nOfCols);
    }

    void Free() { m_rows.Free(); }

    T* operator[](unsigned int n) const { return m_rows[n]; }

protected:
    ExcStatus Alloc(unsigned int nOfRows, unsigned int nOfCols)
    {
        RET_ON_EXC(m_rows.ReAlloc(nOfRows))

        for(unsigned int i = 0; i < nOfRows; i++)
        {
            RET_ON_EXC(m_rows[i].ReAlloc(nOfCols));
        }

        return ExcStatusOk;
    }

    FixedBuffer<FixedBuffer<T> > m_rows;
private:
    FixedBuffer2D(const FixedBuffer2D &) {}
    FixedBuffer2D& operator=(const FixedBuffer2D&) {}
};

} // namespace UIC

#endif // __UIC_FIXED_BUFFER_H__
