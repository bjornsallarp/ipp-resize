
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

#ifndef __PHBITIN_H__
#define __PHBITIN_H__

#include "dphwarning.h"
#include "bdiagnoutput.h"

template<class ByteInput>
class PHBitIn
{
public:
    PHBitIn(ByteInput &input) : m_input(&input), m_byte(0), m_bitPos(-1) {}

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    bool Read()
    {
        int bit;

        if (m_bitPos < 0)
        {
            if (m_byte != 0xFF) m_bitPos = 7;
            else                m_bitPos = 6;

            m_byte = m_input->Read8u();
        }
        bit = (m_byte >> m_bitPos) & 0x01;

        m_bitPos --;

        return bit!=0;
    }

    int Read(int count)
    {
        int bits = 0;
        for(int i = count-1; i >= 0; i--)
        {
            if(Read()) bits = bits | (1 << i);
        }
        return bits;
    }

    void Flush(bool isZeroLength = false)
    {
        if(isZeroLength)
        {
            int mask = 0x7f;

            int n = 0;
            int v = 0;

            if(m_bitPos==-1)
                n = (m_byte == 0xFF) ? 7 : 0;
            else
                n = m_bitPos + 1;

            if(n > 0) v = Read(n);

            if(m_byte == 0xff)
            {
                v = (v << 7) | Read(7);
                n += 7;
            }

            if (n > 7) v    >>= n - 7;
            else       mask >>= 7 - n;

            if (((~v) & mask) != mask)
            {
                m_diagnOutputPtr->Warning(DiagnDescrCT<DPHWarning, DPHBadPacketHeaderPadding>());
            }
        }
        else
        {
            if(m_bitPos==-1)
            {
                if(m_byte == 0xFF)
                    Read(7);
            }
            else
            {
                Read(m_bitPos+1);
            }
            if(m_byte == 0xFF)
                Read(7);
        }
    }

protected:
    ByteInput      *m_input;
    BDiagnOutputPtr m_diagnOutputPtr;
    unsigned char   m_byte;
    int             m_bitPos;
};



#endif // __PHBITIN_H__
