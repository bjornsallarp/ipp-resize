/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2007-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __DIAGN_ADAPTER_H__
#define __DIAGN_ADAPTER_H__

#include "uic_defs.h"
#include "uic_base_stream_diagn.h"
#include "bdiagnoutput.h"

// JPEG 2000 native context
class DiagnCtxt
{
public:
    DiagnCtxt() : m_errNum(0) {}
    DiagnCtxt(int errNum) : m_errNum(errNum) { }

    int ErrNum() { return m_errNum; }

protected:
    int m_errNum;
};

// UIC adapters:

template<UIC::BaseDiagnDescriptor::TContext CONTEXT>
class UICAdapterDiagnDescriptor : public UIC::BaseDiagnDescriptor
{
public:
    UICAdapterDiagnDescriptor() : m_msg_txt(NULL) {}
    UICAdapterDiagnDescriptor(int code) : m_code(code), m_msg_txt(NULL) {}
    UICAdapterDiagnDescriptor(int code, const char* txt) : m_code(code), m_msg_txt(txt) {}

    virtual ~UICAdapterDiagnDescriptor() {}

    virtual TContext Context() const { return CONTEXT; }
    virtual int      Code ()  const { return m_code; }
    virtual const char* GetMessage() const
    {
        if(m_msg_txt == NULL)
            return "<no text>";
        else
            return m_msg_txt;
    }
    void SetText(const char* txt) { m_msg_txt = txt; }

protected:
    int m_code;
    const char *m_msg_txt;
};


template<UIC::BaseDiagnDescriptor::TContext CONTEXT>
class UICAdapterDiagnOutput : public BDiagnOutput
{
public:
    UICAdapterDiagnOutput() {}

    void Attach(UIC::BaseStreamDiagn &stream) { m_stream = stream; }
    void Detach()                             { m_stream.SetDummy(); }

    void Warning(const BDiagnDescr &descriptor)
    {
        UICAdapterDiagnDescriptor<CONTEXT> diagn(descriptor.Context().ErrNum(), descriptor.GetMessage());

        m_stream->Write(diagn);
    }

    void Error  (const BDiagnDescr &descriptor)
    {
        UICAdapterDiagnDescriptor<CONTEXT> diagn(descriptor.Context().ErrNum(), descriptor.GetMessage());

        m_stream->Write(diagn);
    }

    void Write  (int code)
    {
        UICAdapterDiagnDescriptor<CONTEXT> diagn(code);

        m_stream->Write(diagn);
    }

protected:
    UIC::BaseStreamDiagnRef m_stream;
};


#endif // __DIAGN_ADAPTER_H__
