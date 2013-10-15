
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __WT97FWDCOL16S_H__
#define __WT97FWDCOL16S_H__

// Include common, EXTERNAL part of interface
#include "wt97fwdcolgen.h"

#include "calcvector.h"
#include "wt97const16s32s.h"

//
//  And defines specific WT97, forward, column-oriented transform INTERNAL routines
//  depended on data type (like border processing and so on).
//  Here is 16s version.
//

template<> void BWT97FwdColPipeline<Ipp16s>::ProcessOdd_1()
{
    MulRoundSF16((1 << 17), DlyRowSrc(0), DlyRowH(2), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::ProcessOdd_2()
{
    Sub         (DlyRowSrc(0), DlyRowSrc(1), DlyRowH(2), m_width);
    MulRoundSF16((1 << 15),    DlyRowH(2),   DlyRowL(1), m_width);
    Add         (DlyRowSrc(0),               DlyRowL(1), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::ProcessEven_2()
{
    Sub         (DlyRowSrc(1), DlyRowSrc(0), DlyRowH(2), m_width);
    MulRoundSF16((1 << 15),    DlyRowH(2),   DlyRowL(1), m_width);
    Add         (DlyRowSrc(1),               DlyRowL(1), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::ProcessOdd_3()
{
    MulRoundSF16(FIX_FWDH1x2, DlyRowSrc(0),      DlyRowL(1), m_width);
    MulRoundSF16(FIX_FWDH3  , DlyRowSrc(2),      DlyRowH(2), m_width);
    Add         (DlyRowH(2),                 DlyRowL(1), m_width);

    Sub         (DlyRowSrc(1), DlyRowL(1),   DlyRowH(2),  m_width);

    NextH();

    Add         (DlyRowL(1), DlyRowSrc(1),   DlyRowH(2),  m_width);

    Add         (DlyRowSrc(2), DlyRowSrc(0), DlyRowL(1),  m_width);

    SubRev      (DlyRowL(1),                 DlyRowH(2), m_width);

    MulRoundSF16((1 << 15),                  DlyRowL(1), m_width);
    Add         (DlyRowSrc(1),               DlyRowL(1), m_width);
    MulRoundSF16((1 << 15),                  DlyRowL(1), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::ProcessEven_3()
{
    MulRoundSF16(FIX_FWDL4pL0x2, DlyRowSrc(2),   DlyRowL(1), m_width);
    MulRoundSF16((1 << 15), DlyRowSrc(1),         DlyRowH(2), m_width);
    Add         (DlyRowH(2),                 DlyRowL(1), m_width);
    MulRoundSF16(FIX_FWDL2x2, DlyRowSrc(0),      DlyRowH(2), m_width);
    Add         (DlyRowH(2),                 DlyRowL(1), m_width);

    Add         (DlyRowSrc(2), DlyRowSrc(0), DlyRowL(0), m_width);
    MulRoundSF16((1 << 15),  DlyRowL(0), m_width);
    Sub         (DlyRowL(0), DlyRowSrc(1),   DlyRowH(2), m_width);
    Add         (DlyRowSrc(1),               DlyRowL(0), m_width);
    Sub         (DlyRowL(1),                 DlyRowL(0), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::LiftALPHAx2(unsigned int srcDlyToMul, unsigned int srcDlyToAdd, unsigned int dstDlyH)
{
    MulRoundSF16(FIX_ALPHAx2, DlyRowSrc(srcDlyToMul), DlyRowH(dstDlyH), m_width);
    Add         (DlyRowSrc(srcDlyToAdd),          DlyRowH(dstDlyH), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::LiftALPHA(unsigned int srcDlyToMul1, unsigned int srcDlyToMul2, unsigned int srcDlyToAdd, unsigned int dstDlyH)
{
    Add         (DlyRowSrc(srcDlyToMul1), DlyRowSrc(srcDlyToMul2), DlyRowH(dstDlyH), m_width);
    MulRoundSF16(FIX_ALPHA,                                        DlyRowH(dstDlyH), m_width);
    Add         (DlyRowSrc(srcDlyToAdd),                           DlyRowH(dstDlyH), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::LiftBETAx2(unsigned int srcDlyHToMul, unsigned int srcDlyToAdd, unsigned int dstDlyL)
{
    MulRoundSF16(FIX_BETAx2, DlyRowH(srcDlyHToMul), DlyRowL(dstDlyL), m_width);
    Add         (DlyRowSrc(srcDlyToAdd),        DlyRowL(dstDlyL), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::LiftBETA(unsigned int srcDlyHToMul1, unsigned int srcDlyHToMul2, unsigned int srcDlyToAdd, unsigned int dstDlyL)
{
    Add         (DlyRowH(srcDlyHToMul1), DlyRowH(srcDlyHToMul2), DlyRowL(dstDlyL), m_width);
    MulRoundSF16(FIX_BETA,                                       DlyRowL(dstDlyL), m_width);
    Add         (DlyRowSrc(srcDlyToAdd),                         DlyRowL(dstDlyL), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::LiftGAMMAx2(unsigned int srcDlyLToMul, unsigned int dstDlyH)
{
    MulAddRoundSF16(FIX_GAMMAx2, DlyRowL(srcDlyLToMul), DlyRowH(dstDlyH), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::LiftGAMMA(unsigned int srcDlyLToMul1, unsigned int srcDlyLToMul2, unsigned int dstDlyH)
{
    AddMulAddRoundSF16(FIX_GAMMA, DlyRowL(srcDlyLToMul1), DlyRowL(srcDlyLToMul2), DlyRowH(dstDlyH), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::LiftDELTA(unsigned int srcDlyHToMul1, unsigned int srcDlyHToMul2, unsigned int srcDstDlyL)
{
    AddMulAddRoundSF16(FIX_DELTA, DlyRowH(srcDlyHToMul1), DlyRowH(srcDlyHToMul2), DlyRowL(srcDstDlyL), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::LiftDELTAx2(unsigned int srcDlyH, unsigned int srcDstDlyL)
{
    MulAddRoundSF16(FIX_DELTAx2, DlyRowH(srcDlyH), DlyRowL(srcDstDlyL), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::LiftKL(unsigned int srcDstDlyL)
{
    MulRoundSF16(FIX_KL, DlyRowL(srcDstDlyL), m_width);
}

template<> void BWT97FwdColPipeline<Ipp16s>::LiftKH(unsigned int srcDstDlyH)
{
    MulRoundSF16(FIX_KH, DlyRowH(srcDstDlyH), m_width);
}

#endif // __WT97FWDCOL16S_H__
