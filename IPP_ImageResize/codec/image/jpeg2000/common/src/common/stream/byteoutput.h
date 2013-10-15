
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

#ifndef __BYTEOUTPUT_H__
#define __BYTEOUTPUT_H__

#include "ippdefs.h"

/*
// unidirectional abstract output stream
class ByteOutput
{
public:
    virtual void Write8u (Ipp8u  value) = 0;
    virtual void Write16u(Ipp16u value) = 0;
    virtual void Write32u(Ipp32u value) = 0;
    virtual void Write64u(Ipp64u value) = 0;

    // signed versions are covered here
    void Write8s (Ipp8s  value) { Write8u ((Ipp8s) value); }
    void Write16s(Ipp16s value) { Write16u((Ipp16s)value); }
    void Write32s(Ipp32s value) { Write32u((Ipp32s)value); }
    void Write64s(Ipp64s value) { Write64u((Ipp64s)value); }

    virtual void Write(const Ipp8u *src, unsigned int size) = 0;
    virtual void FillZero(unsigned int size) = 0;

private:
    ByteOutput() {}
};
*/

template<class ByteOutput>
class ByteOutputDriver
{
public:
    ByteOutputDriver() : m_stream(0) {}

    void Attach(ByteOutput &stream) { m_stream = &stream; }

    Ipp8u Write8u()                              const { return m_stream->Write8u(); }
    void  Write  (Ipp8u *dst, unsigned int size) const { m_stream->Write(dst, size); }

protected:
    ByteOutput *m_stream;
};

template<class ByteOutput>
class ByteOutputLE : public ByteOutput
{
public:
    void Write16u(Ipp16u value)
    {
        this->Write8u((Ipp8u) value);
        this->Write8u((Ipp8u)(value >>  8u));
    }

    void Write32u(Ipp32u value)
    {
        this->Write8u((Ipp8u) value);
        this->Write8u((Ipp8u)(value >>  8u));
        this->Write8u((Ipp8u)(value >> 16u));
        this->Write8u((Ipp8u)(value >> 24u));
    }

    void Write64u(Ipp64u value)
    {
        Write32u((Ipp32u) value);
        Write32u((Ipp32u)(value >> 32u));
    }
};

template<class ByteOutput>
class ByteOutputBE : public ByteOutput
{
public:
    void Write16u(Ipp16u value)
    {
        this->Write8u((Ipp8u)(value >> 8u));
        this->Write8u((Ipp8u) value);
    }

    void Write32u(Ipp32u value)
    {
        this->Write8u((Ipp8u)(value >> 24u));
        this->Write8u((Ipp8u)(value >> 16u));
        this->Write8u((Ipp8u)(value >> 8u));
        this->Write8u((Ipp8u) value);
    }

    void Write64u(Ipp64u value)
    {
        Write32u((Ipp32u)(value >> 32u));
        Write32u((Ipp32u) value);
    }
};

// Byte output stream template implementing signed version of functions
template<class ByteOutput>
class ByteOutputSigned : public ByteOutput
{
public:
    ByteOutputSigned() {}

    void Write8s (Ipp8s  value) { this->Write8u ((Ipp8u) value); }
    void Write16s(Ipp16s value) { this->Write16u((Ipp16u)value); }
    void Write32s(Ipp32s value) { this->Write32u((Ipp32u)value); }
    void Write64s(Ipp64s value) { this->Write64u((Ipp64u)value); }
};

#endif // __BYTEOUTPUT_H__
