
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

#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include "fixedbuffer.h"

template<class T>
class RingBuffer
{
public:
    RingBuffer() : m_size(0), m_position(0) {}

    RingBuffer(unsigned int size) : m_size(size), m_position(0), m_buffer(2 * size) {}

    void ReAlloc(unsigned int size)
    {
        m_size     = size;
        m_position =    0;
        m_buffer.ReAlloc(2 * m_size);
    }

    void Init(const T *data)
    {
        for(unsigned int i = 0; i < m_size; i++) SetEntry(data[i], i);
    }

    void SetEntry(const T &data, unsigned int position)
    {
        m_buffer[position]          = data;
        m_buffer[position + m_size] = data;
    }

    operator const T*() const { return &(m_buffer[m_position]); }

    const T& Delayed(unsigned int delay) const {  return m_buffer[m_position + m_size - 1 - delay]; }

    void operator++()
    {
        m_position++;
        if (m_position == m_size) m_position = 0;
    }

    void operator--()
    {
        if (m_position == 0) m_position = m_size - 1;
        else                 m_position--;
    }

    unsigned int Position() const { return m_position; }

    unsigned int Size    () const { return m_size;     }


protected:
    FixedBuffer<T> m_buffer;

    unsigned int   m_size;
    unsigned int   m_position;
};

#endif // __RINGBUFFER_H__
