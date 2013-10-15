
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __DUMMYOUTPUT_H__
#define __DUMMYOUTPUT_H__

class DummyOutput
{
public:
    DummyOutput() : m_position(0) {}

    void Reset()                { m_position = 0;     }

    void Write8u (Ipp8u  /* value */) { m_position += sizeof(Ipp8u);  }
    void Write16u(Ipp16u /* value */) { m_position += sizeof(Ipp16u); }
    void Write32u(Ipp32u /* value */) { m_position += sizeof(Ipp32u); }
    void Write64u(Ipp64u /* value */) { m_position += sizeof(Ipp64u); }

    void Write8s (Ipp8s  /* value */) { m_position += sizeof(Ipp8s);  }
    void Write16s(Ipp16s /* value */) { m_position += sizeof(Ipp16s); }
    void Write32s(Ipp32s /* value */) { m_position += sizeof(Ipp32s); }
    void Write64s(Ipp64s /* value */) { m_position += sizeof(Ipp64s); }

    void Write(const Ipp8u * /* src */, unsigned int size) { m_position += size; }
    void FillZero(unsigned int size)                      { m_position += size; }

    unsigned int Position() const { return m_position; }

protected:
    unsigned int m_position;
};

#endif // __DUMMYOUTPUT_H__
