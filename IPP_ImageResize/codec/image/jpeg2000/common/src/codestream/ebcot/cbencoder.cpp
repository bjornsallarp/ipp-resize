
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

#include "cbencoder.h"
#include "diagndescr.h"
#include "memexception.h"

const int CBEncoder::m_buffSize = 1024;

CBEncoder::CBEncoder(int guardBits, int codingOptions, IppiMQTermination mqTermType)
: m_codingOptions(codingOptions)
, m_termType(mqTermType)
, m_buffer(m_buffSize)
, m_guardBits(guardBits)
{
    IppiSize size = {64, 64};
    ippiEncodeInitAlloc_JPEG2K(&m_state, size);

    if(!m_state) throw DiagnDescrCT<MemoryException,faultMalloc>();
}

CBEncoder::~CBEncoder() { ippiEncodeFree_JPEG2K(m_state); }

void CBEncoder::Encode(
        const ImageCore32sC1 &precImg,
        const Rect           &cbRect,
              IppiWTSubband   subband,
              int             sbDynRange,
              int             sbLowestNonSfBits,
              ECodeBlock     &cb
    )
{
    int sfBits;
    int nOfTermPasses; // stub, there is no need in this implementation
    int nOfPasses;

    ImageCore32sC1 cbImg = precImg.SubImage(cbRect.Origin());

    int magnBits = 31 - sbLowestNonSfBits;
    if(magnBits > 31) magnBits = 31;
    if(magnBits <  0) magnBits =  0;

    Shift(cbImg, cbRect.Size(), sbLowestNonSfBits);
    Complement(cbImg, cbRect.Size());

    ippiEncodeLoadCodeBlock_JPEG2K_32s_C1R(
        cbImg.Data(),
        cbImg.LineStep(),
        cbRect.Size(),
        m_state,
        subband,
        magnBits,
        m_termType,
        ippMQRateApprGood,
        m_codingOptions,
       &sfBits,
       &nOfPasses,
       &nOfTermPasses);

    cb.SetNOfZeroBits(sbDynRange + m_guardBits - sfBits - 1);

    int isNotFinish = 1;

    while(isNotFinish)
    {
        int len = m_buffSize;
        ippiEncodeStoreBits_JPEG2K_1u(m_buffer, &len, m_state, &isNotFinish);
        m_accum.Write(m_buffer, len);
    }

    cb.ReceiveComprData(m_accum);

    for(int i = 0; i < nOfPasses; i++)
    {
        int    rate;
        double dist;

        ippiEncodeGetRate_JPEG2K(m_state, i, &rate);
        ippiEncodeGetDist_JPEG2K(m_state, i, &dist);

        cb.PushPass(rate, dist);
    }
}


