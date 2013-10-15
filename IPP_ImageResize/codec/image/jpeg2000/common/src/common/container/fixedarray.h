
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2003-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __FIXEDARRAY_H__
#define __FIXEDARRAY_H__

#include "fixedbuffer.h"
#include "genalg.h"

template <class T> class FixedArray : public FixedBuffer<T> {
public:
    FixedArray(unsigned int size = 0)
    : FixedBuffer<T>(size), m_size(size)
    {}

    FixedArray(const FixedArray &array)
    : FixedBuffer<T>(array.m_size), m_size(array.m_size)
    {
        Copy(array, *this, m_size);
    }

    FixedArray& operator=(const FixedArray& array)
    {
        if(this != &array)
        {
            ReAlloc(array.m_size);
            Copy(array, *this, m_size);
        }
        return *this;
    }

    void ReAlloc(unsigned int size)
    {
        m_size = size;
        FixedBuffer<T>::ReAlloc(m_size);
    }

    void Free()
    {
        FixedBuffer<T>::Free();
        m_size = 0;
    }

    unsigned int Size() const { return m_size; }

protected:
    unsigned int m_size;
};

template <class T> class FixedArray2D : public FixedBuffer2D<T> {
public:
    FixedArray2D() : m_nOfRows(0), m_nOfCols  (0) {}

    FixedArray2D(unsigned int nOfRows, unsigned int nOfCols)
    : FixedBuffer2D<T>(nOfRows, nOfCols)
    , m_nOfRows(nOfRows)
    , m_nOfCols(nOfCols)
    {}

    FixedArray2D(const FixedArray2D &array)
    : FixedBuffer2D<T>(array.m_nOfRows, array.m_nOfCols)
    , m_nOfRows(array.m_nOfRows)
    , m_nOfCols(array.m_nOfCols)
    {
        for(unsigned int row = 0; row < m_nOfRows; row++)
            Copy(array[row], (*this)[row], m_nOfCols);
    }

    FixedArray2D& operator=(const FixedArray2D& array)
    {
        if(this != &array)
        {
            ReAlloc(array.m_nOfRows, array.m_nOfCols);
            for(unsigned int row = 0; row < m_nOfRows; row++)
                Copy(array[row], (*this)[row], m_nOfCols);
        }
        return *this;
    }

    void ReAlloc(unsigned int nOfRows, unsigned int nOfCols)
    {
        m_nOfRows = nOfRows;
        m_nOfCols = nOfCols;
        FixedBuffer2D<T>::ReAlloc(nOfRows, nOfCols);
    }

    void Free()
    {
        FixedBuffer2D<T>::Free();
        m_nOfRows = 0;
        m_nOfCols = 0;
    }

    unsigned int NOfRows() const { return m_nOfRows; }
    unsigned int NOfCols() const { return m_nOfCols; }

protected:
    unsigned int m_nOfRows;
    unsigned int m_nOfCols;
};

#endif // __FIXEDARRAY_H__

