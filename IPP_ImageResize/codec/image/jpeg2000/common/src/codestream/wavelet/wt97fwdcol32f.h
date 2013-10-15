
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

#ifndef __WT97FWDCOL32F_H__
#define __WT97FWDCOL32F_H__

// Include common, EXTERNAL part of interface
#include "wt97fwdcolgen.h"

#include "calcvector.h"
#include "wt97const32f.h"

//
//  And defines specific WT97, forward, column-oriented transform INTERNAL routines
//  depended on data type (like border processing and so on).
//  Here is 32f version.
//

template<> void BWT97FwdColPipeline<Ipp32f>::ProcessOdd_1()
{
    Mul   (2.0f, DlyRowSrc(0), DlyRowH(2), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::ProcessOdd_2()
{
    Sub   (DlyRowSrc(0), DlyRowSrc(1), DlyRowH(2), m_width);
    Mul   (0.5f, DlyRowH(2),           DlyRowL(1), m_width);
    Add   (DlyRowSrc(0),               DlyRowL(1), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::ProcessEven_2()
{
    Sub   (DlyRowSrc(1), DlyRowSrc(0), DlyRowH(2), m_width);
    Mul   (0.5f, DlyRowH(2),           DlyRowL(1), m_width);
    Add   (DlyRowSrc(1),               DlyRowL(1), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::ProcessOdd_3()
{
    Mul   (FWDH1x2, DlyRowSrc(0),      DlyRowL(1), m_width);
    Mul   (FWDH3  , DlyRowSrc(2),      DlyRowH(2), m_width);
    Add   (DlyRowH(2),                 DlyRowL(1), m_width);

    Sub   (DlyRowSrc(1), DlyRowL(1),   DlyRowH(2),  m_width);

    NextH();

    Add   (DlyRowL(1), DlyRowSrc(1),   DlyRowH(2),  m_width);

    Add   (DlyRowSrc(2), DlyRowSrc(0), DlyRowL(1),  m_width);

    SubRev(DlyRowL(1),                 DlyRowH(2), m_width);

    Mul   (0.5f,                       DlyRowL(1), m_width);
    Add   (DlyRowSrc(1),               DlyRowL(1), m_width);
    Mul   (0.5f,                       DlyRowL(1), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::ProcessEven_3()
{
    Mul   (FWDL4pL0x2, DlyRowSrc(2),   DlyRowL(1), m_width);
    Mul   (0.5f, DlyRowSrc(1),         DlyRowH(2), m_width);
    Add   (DlyRowH(2),                 DlyRowL(1), m_width);
    Mul   (FWDL2x2, DlyRowSrc(0),      DlyRowH(2), m_width);
    Add   (DlyRowH(2),                 DlyRowL(1), m_width);

    Add   (DlyRowSrc(2), DlyRowSrc(0), DlyRowL(0), m_width);
    Mul   (0.5f,  DlyRowL(0), m_width);
    Sub   (DlyRowL(0), DlyRowSrc(1),   DlyRowH(2), m_width);
    Add   (DlyRowSrc(1),               DlyRowL(0), m_width);
    Sub   (DlyRowL(1),                 DlyRowL(0), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::LiftALPHAx2(unsigned int srcDlyToMul, unsigned int srcDlyToAdd, unsigned int dstDlyH)
{
    Mul   (ALPHAx2, DlyRowSrc(srcDlyToMul), DlyRowH(dstDlyH), m_width);
    Add   (DlyRowSrc(srcDlyToAdd),          DlyRowH(dstDlyH), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::LiftALPHA(unsigned int srcDlyToMul1, unsigned int srcDlyToMul2, unsigned int srcDlyToAdd, unsigned int dstDlyH)
{
    Add   (DlyRowSrc(srcDlyToMul1), DlyRowSrc(srcDlyToMul2), DlyRowH(dstDlyH), m_width);
    Mul   (ALPHA,                                            DlyRowH(dstDlyH), m_width);
    Add   (DlyRowSrc(srcDlyToAdd),                           DlyRowH(dstDlyH), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::LiftBETAx2(unsigned int srcDlyHToMul, unsigned int srcDlyToAdd, unsigned int dstDlyL)
{
    Mul   (BETAx2, DlyRowH(srcDlyHToMul), DlyRowL(dstDlyL), m_width);
    Add   (DlyRowSrc(srcDlyToAdd),        DlyRowL(dstDlyL), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::LiftBETA(unsigned int srcDlyHToMul1, unsigned int srcDlyHToMul2, unsigned int srcDlyToAdd, unsigned int dstDlyL)
{
    Add   (DlyRowH(srcDlyHToMul1), DlyRowH(srcDlyHToMul2), DlyRowL(dstDlyL), m_width);
    Mul   (BETA,                                           DlyRowL(dstDlyL), m_width);
    Add   (DlyRowSrc(srcDlyToAdd),                         DlyRowL(dstDlyL), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::LiftGAMMAx2(unsigned int srcDlyLToMul, unsigned int dstDlyH)
{
    MulAdd(GAMMAx2, DlyRowL(srcDlyLToMul), DlyRowH(dstDlyH), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::LiftGAMMA(unsigned int srcDlyLToMul1, unsigned int srcDlyLToMul2, unsigned int dstDlyH)
{
    AddMulAdd(GAMMA, DlyRowL(srcDlyLToMul1), DlyRowL(srcDlyLToMul2), DlyRowH(dstDlyH), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::LiftDELTA(unsigned int srcDlyHToMul1, unsigned int srcDlyHToMul2, unsigned int srcDstDlyL)
{
    AddMulAdd(DELTA, DlyRowH(srcDlyHToMul1), DlyRowH(srcDlyHToMul2), DlyRowL(srcDstDlyL), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::LiftDELTAx2(unsigned int srcDlyH, unsigned int srcDstDlyL)
{
    MulAdd(DELTAx2, DlyRowH(srcDlyH), DlyRowL(srcDstDlyL), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::LiftKL(unsigned int srcDstDlyL)
{
    Mul(KL, DlyRowL(srcDstDlyL), m_width);
}

template<> void BWT97FwdColPipeline<Ipp32f>::LiftKH(unsigned int srcDstDlyH)
{
    Mul(KH, DlyRowH(srcDstDlyH), m_width);
}

#endif // __WT97FWDCOL32F_H__
