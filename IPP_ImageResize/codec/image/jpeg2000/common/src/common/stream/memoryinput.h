
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

#ifndef __MEMORYINPUT_H__
#define __MEMORYINPUT_H__

#include "ippdefs.h"
#include "genalg.h"

class MemoryInput
{
public:
    MemoryInput() : m_data(0) {}

    MemoryInput(const Ipp8u *data) : m_data(data) {}

    void Attach(const Ipp8u *data) { m_data = data; }
    void Detach()                  { m_data = 0;    }

    Ipp8u Read8u()                 { Ipp8u value = *m_data; m_data++; return value; }

    void Read(Ipp8u *dst, unsigned int size)
    {
        Copy(m_data, dst, size);
        m_data += size;
    }

    void Seek() { m_data++; }

    void Seek(unsigned int offset) { m_data += offset; }

protected:
    const Ipp8u *m_data;
};


#endif // __MEMORYINPUT_H__
