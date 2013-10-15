
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

#include "dcodeblock.h"

DCodeBlock::DCodeBlock() : m_Lblock(3) {}

void DCodeBlock::Init(
    const ImageCore32sC1        &cbImage,
    const RectSize              &size,
    unsigned int                 nOfLayers,
    IppiWTSubband                subband,
    unsigned int                 cmpGuardBits,
    unsigned int                 sbDynRange,
    unsigned int                 sbLowestBitOffset,
    const CBCodingStyle         &codingStyle)
{
    int stateSize;
    ippiDecodeCBProgrGetStateSize_JPEG2K(size, &stateSize);
    m_state.ReAlloc(stateSize);

    ippiDecodeCBProgrInit_JPEG2K(State());

    m_layer.ReAlloc(nOfLayers);

    m_blockData.Clear();

    m_cbImage           = cbImage          ;
    m_size              = size             ;
    m_subband           = subband          ;
    m_cmpGuardBits      = cmpGuardBits     ;
    m_sbDynRange        = sbDynRange       ;
    m_sbLowestBitOffset = sbLowestBitOffset;
    m_codingStyle       = codingStyle      ;

    m_Lblock = 3;

    Zero(m_cbImage, m_size);
}

void DCodeBlock::SetNextSegmentSize(unsigned int layer, unsigned int size, unsigned int nOfPasses)
{
    m_layer[layer].SetNextSegmentSize(size, nOfPasses);
    m_nOfPasses += nOfPasses;
}

void DCodeBlock::SetNOfZeroBits(unsigned int nOfZeroBits)
{
    m_nOfZeroBits = nOfZeroBits;

    int sfBits = (int)m_sbDynRange - 1 + (int)m_cmpGuardBits + m_sbLowestBitOffset - nOfZeroBits;

    if(sfBits > 31)
    {
        m_diagnOutputPtr->Warning(DiagnDescrCT<DEBCOTWarning, CodeBlockNOfSignificantBitsExceedImplementationLimit>());
        sfBits = 31;
    }
    if(sfBits <  0)
    {
        m_diagnOutputPtr->Warning(DiagnDescrCT<DEBCOTWarning, CodeBlockNOfSignificantBitsBelowZero>());
        sfBits =  0;
    }

    ippiDecodeCBProgrAttach_JPEG2K_32s_C1R(
        m_cbImage.Data(),
        m_cbImage.LineStep(),
        m_size,
        State(),
        m_subband,
        sfBits,
        m_codingStyle.IPPFlags() | IPP_JPEG2K_DEC_DO_NOT_CLEAR_CB);

    m_currPass      = 0;
    m_sectNOfPasses = 0;
    m_lastPassDone = 0xFFFFFFFF;
    done = false;
}
