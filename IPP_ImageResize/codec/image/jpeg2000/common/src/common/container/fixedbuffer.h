
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

#ifndef __FIXEDBUFFER_H__
#define __FIXEDBUFFER_H__


template <class T> class FixedBuffer {
public:
    FixedBuffer(int size = 0) { Alloc(size); }
    ~FixedBuffer() { Free(); }

    void ReAlloc(unsigned int size)
    {
        Free();
        Alloc(size);
    }

    void Free()
    {
        if(m_data) delete[] m_data;
        m_data = 0;
    }

    operator T*() const { return m_data; }

protected:
    void Alloc(unsigned int size) { m_data = size ? new T[size] : 0; }

    T *m_data;
};

template <class T> class FixedBuffer2D {
public:
    FixedBuffer2D(unsigned int nOfRows = 0, unsigned int nOfCols = 0) { Alloc(nOfRows, nOfCols); }
    ~FixedBuffer2D() { Free(); }

    void ReAlloc(unsigned int nOfRows, unsigned int nOfCols)
    {
        Free();
        Alloc(nOfRows, nOfCols);
    }

    void Free()
    {
        if(m_rows) delete[] m_rows;
        m_rows = 0;
    }

    T* operator[](unsigned int n) const { return m_rows[n]; }

protected:
    void Alloc(unsigned int nOfRows, unsigned int nOfCols)
    {
        m_rows = nOfRows ? new FixedBuffer<T>[nOfRows] : 0;

        for(unsigned int i = 0; i < nOfRows; i++)
            m_rows[i].ReAlloc(nOfCols);
    }


    FixedBuffer<T> *m_rows;
};

typedef FixedBuffer2D<int> Int2D;

#endif // __FIXEDBUFFER_H__

