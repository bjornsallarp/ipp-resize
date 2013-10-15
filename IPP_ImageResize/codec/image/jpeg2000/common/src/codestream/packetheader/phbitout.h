
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

/*
//  PHBitOut implements sequential data writing by single bit.
//      It uses for packet header output and it implements bit stuffing
//      routine described in B.10.1 article of JPEG 200 standard.
//      This procedure avoids writing of marker code in output stream.
//
//
//
*/

#ifndef __PHBITOUT_H__
#define __PHBITOUT_H__

#include "ippdefs.h"

template<class ByteOutput>
class PHBitOut
{
public:
    PHBitOut(ByteOutput &buffer)
    : m_buffer(&buffer)
    , m_byte(0)
    , m_bitPos(7)
    , m_isFF(0)
    {}

    void Write(bool bit)
    {
        if(bit) Write1();
        else    Write0();
    }

    void Write0()
    {
        m_bitPos--;

        if(m_bitPos >=0) return;

        m_isFF = (m_byte == 0xFF);

        if(m_isFF) m_bitPos = 6;
        else       m_bitPos = 7;

        ByteOutAndClear();
    }

    void Write1()
    {
        m_byte |= ((Ipp32u)1) << (Ipp32u)(m_bitPos);

        m_bitPos--;

        if(m_bitPos >=0) return;

        m_isFF = (m_byte == 0xFF);

        if(m_isFF) m_bitPos = 6;
        else       m_bitPos = 7;

        ByteOutAndClear();
    }

    void Write(int bits, int count)
    {
        for(int i = count-1; i >= 0; i--)
            Write( (bits & (1 << i)) != 0);
    }

    void Flush()
    {
        if (m_isFF)
        {
            if (m_bitPos == 6)
            {
                m_byte = 0;
                ByteOutAndClear();
            }
            else
                ByteOutAndClear();
        }
        else
        {
            if (m_bitPos != 7)
                ByteOutAndClear();
        }
        m_isFF   = false;
        m_bitPos = 7;
    }

protected:
    ByteOutput *m_buffer;
    unsigned int m_byte;
    int          m_bitPos;
    bool         m_isFF;

    void ByteOutAndClear()
    {
        m_buffer->Write8u((Ipp8u)m_byte);
        m_byte = 0;
    }
};


#endif // __PHBITOUT_H__

