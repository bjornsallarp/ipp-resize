/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2011-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __STREAM_INPUT_ADAPTER_H__
#define __STREAM_INPUT_ADAPTER_H__

#include "uic_base_stream_input.h"
#include "byteinput.h"
#include "bdiagnoutput.h"

class UICAdapterInput
{
public:
    UICAdapterInput() : m_stream(0) {}

    void  Attach(UIC::BaseStreamInput &stream) { m_stream = &stream; }
    void  Detach()                             { m_stream = 0; }

    Ipp8u Read8u()                             { Ipp8u value; Read(&value, 1); return value; }

    void  Read(Ipp8u *dst, unsigned int size)
    {
        UIC::BaseStream::TSize cnt;
        m_stream->Read(dst, size, cnt);
        if(cnt != size)
            throw DiagnDescrCT<ByteInputException,noDataToRead>();
    }

    void  Seek()                    {  m_stream->SeekFwd(1u); }
    void  Seek(unsigned int offset) {  m_stream->SeekFwd(offset); }

    unsigned int Size()
    {
        UIC::BaseStream::TSize size;
        m_stream->Size(size);
        return (unsigned int)size;
    }

protected:
    UIC::BaseStreamInputFwd *m_stream;
};


typedef ByteInputSigned<ByteInputBE<UICAdapterInput> > UICAdapterInputBE;

class UICAdapterBoundedInputBE : public ByteInputBoundService<UICAdapterInputBE>
{
public:
    UICAdapterBoundedInputBE() {}
    virtual ~UICAdapterBoundedInputBE() {}

    void Attach(UIC::BaseStreamInput &stream)
    {
        UICAdapterInputBE::Attach(stream);
        PushSize(UICAdapterInputBE::Size());
    }

    void Detach()
    {
        UICAdapterInputBE::Detach();
        PopSize();
    }
};

#endif // __STREAM_INPUT_ADAPTER_H__
