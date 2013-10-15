
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

#ifndef __ROWITERATOR_H__
#define __ROWITERATOR_H__

#include "imagecore.h"

template <class T>
class RowIterator
{
public:
    RowIterator()                               : m_data(0),          m_lineStep(0)              {}
    RowIterator(T* data, unsigned int lineStep) : m_data(data),       m_lineStep(lineStep)       {}
    RowIterator(const ImageCore<T>& img)        : m_data(img.Data()), m_lineStep(img.LineStep()) {}

    RowIterator& operator=(const ImageCore<T>& img)
    {
        SetData    (img.Data());
        SetLineStep(img.LineStep());
        return *this;
    }

    void SetData    (T* data)               { m_data     = data; }
    void SetLineStep(unsigned int lineStep) { m_lineStep = lineStep; }

    operator T*() const { return m_data; }

    unsigned int LineStep() const { return m_lineStep; }

    void operator++() { addrInc(m_data, m_lineStep); }
    void operator--() { addrInc(m_data, -(int)m_lineStep); }

protected:
    T            *m_data;
    unsigned int  m_lineStep;
};

#endif // __ROWITERATOR_H__
