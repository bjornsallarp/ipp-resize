
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

#ifndef __MEMORYOUTPUT_H__
#define __MEMORYOUTPUT_H__

class MemoryOutput
{
public:
    MemoryOutput() {}

    void Attach(Ipp8u *data)  {  m_data = data; }
    void Detach()             {  m_data = 0;    }

    void Write8u(Ipp8u value) { *m_data = value; m_data++; }

    void Write(const Ipp8u *src, unsigned int size)
    {
        for(unsigned int i = 0; i < size; i++)
            Write8u(src[i]);
    }

    void FillZero(unsigned int size)
    {
        for(unsigned int i = 0; i < size; i++)
            Write8u(0);
    }


protected:
    Ipp8u *m_data;
};

#endif // __MEMORYOUTPUT_H__
