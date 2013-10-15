
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
//  Code block encoder class encapsulates interface for
//      IPP JPEG 2000 entropy coding functions.
//
//
//
*/

#ifndef __CBENCODER_H__
#define __CBENCODER_H__

#include "ecodeblock.h"
#include "pp.h"
#include "bufferstream.h"

static const Ipp8u GUARD_BITS = 2;

class CBEncoder
{
public:
    CBEncoder(
        int               guardBits     = GUARD_BITS,
        int               codingOptions = 0,
        IppiMQTermination mqTermType    = ippMQTermNearOptimal);

    ~CBEncoder();

    void Encode(
        const ImageCore32sC1 &precImg,
        const Rect           &cbRect,
              IppiWTSubband   subband,
              int             sbDynRange,
              int             sbLowestNonSfBits,
              ECodeBlock     &cb);

protected:
    int                     m_codingOptions;
    IppiMQTermination       m_termType;

private:
    IppiEncodeState_JPEG2K *m_state;
    BufferStream            m_accum;
    FixedBuffer<Ipp8u>      m_buffer;
    int                     m_guardBits;
    static const int        m_buffSize;
};


#endif // __CBENCODER_H__

