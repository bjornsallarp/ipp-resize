/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_BASE_STREAM_DIAGN_H__
#define __UIC_BASE_STREAM_DIAGN_H__

namespace UIC {

class UICAPI BaseDiagnDescriptor
{
public:
    typedef enum
    {
        jpeg_dec,
        jpeg_enc,
        jpeg2000_dec,
        jp2_dec,
        jp2_enc,
        pnm_dec,
        pnm_enc
    } TContext;

    BaseDiagnDescriptor() {}
    virtual ~BaseDiagnDescriptor() {}

    virtual TContext Context() const = 0;
    virtual int      Code   () const = 0;
    virtual const char* GetMessage() const = 0;
};


class UICAPI BaseStreamDiagn
{
public:
    BaseStreamDiagn() {}
    virtual ~BaseStreamDiagn() {}
    virtual void Write(const BaseDiagnDescriptor &) {}
};


// It uses dummy output by default
class UICAPI BaseStreamDiagnRef
{
public:
    BaseStreamDiagnRef() { m_ptr = &m_dummy; }

    BaseStreamDiagnRef(BaseStreamDiagn& diagnOutput) : m_ptr(&diagnOutput) {}

    BaseStreamDiagnRef& operator=(BaseStreamDiagn &diagnOutput) {
        m_ptr = &diagnOutput;
        return *this;
    }

    BaseStreamDiagn * operator->() { return m_ptr; }

    operator BaseStreamDiagn &() { return *m_ptr; }

    void SetDummy() { m_ptr = &m_dummy; }

protected:
    BaseStreamDiagn  m_dummy;
    BaseStreamDiagn *m_ptr;
};

} // namespace UIC

#endif // __UIC_BASE_STREAM_DIAGN_H__
