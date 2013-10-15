/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2011-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __STREAM_OUTPUT_ADAPTER_H__
#define __STREAM_OUTPUT_ADAPTER_H__

#include "uic_base_stream_output.h"
#include "byteoutput.h"
#include "bdiagnoutput.h"

typedef enum {
    writeError
} ByteOutputException;

class UICAdapterOutput
{
public:
    UICAdapterOutput() : m_stream(0) {}

    void  Attach(UIC::BaseStreamOutput &stream) { m_stream = &stream; }
    void  Detach()                              { m_stream = 0; }

    void  Write8u(Ipp8u value)                  { Write(&value, 1); }

    void  Write(const Ipp8u *src, unsigned int size)
    {
        UIC::BaseStream::TSize cnt;
        m_stream->Write(src, size, cnt);
        if(cnt != size)
            throw DiagnDescrCT<ByteOutputException,writeError>();
    }

    unsigned int Size()
    {
        UIC::BaseStream::TSize size;
        m_stream->Size(size);
        return (unsigned int)size;
    }

protected:
    UIC::BaseStreamOutput *m_stream;
};


typedef ByteOutputSigned<ByteOutputBE<UICAdapterOutput> > UICAdapterOutputBE;

#endif // __STREAM_OUTPUT_ADAPTER_H__
