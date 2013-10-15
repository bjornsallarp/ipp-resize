
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

#ifndef __WT97INVCOL32F_H__
#define __WT97INVCOL32F_H__

// Include common, EXTERNAL part of interface
#include "wt97invcolgen.h"

#include "calcvector.h"
#include "wt97const32f.h"

//
//  And defines specific WT97, inverse, column-oriented transform INTERNAL routines
//  depended on data type (like border processing and so on).
//  Here is 32f version.
//

template<> void BWT97InvColPipeline<Ipp32f>::ProcessOdd_1()
{
    Mul   (0.5f, DlyRowH(0), DlyRowDst(0), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::ProcessOdd_2()
{
    Mul   (0.5f, DlyRowH(0),         DlyRowDst(3), m_width);
    SubRev(DlyRowL(0), DlyRowDst(3), m_width);
    Add   (DlyRowH(0), DlyRowDst(3), DlyRowDst(4), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::ProcessEven_2()
{
    Mul   (0.5f, DlyRowH(0),         DlyRowDst(4), m_width);
    SubRev(DlyRowL(0),               DlyRowDst(4), m_width);
    Add   (DlyRowH(0), DlyRowDst(4), DlyRowDst(3), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::ProcessOdd_3()
{
    Sub      (DlyRowH(1), DlyRowH(0),     DlyRowDst(3), m_width);

    Mul      (FWDL2x2, DlyRowDst(3),      DlyRowDst(4), m_width);
    MulAdd   (0.5f, DlyRowH(1),           DlyRowDst(4), m_width);
    Add      (DlyRowL(0),                 DlyRowDst(4), m_width);

    Mul      (HALFmFWDL2x4, DlyRowDst(3), DlyRowDst(2), m_width);
    Add      (DlyRowDst(4),               DlyRowDst(2), m_width);

    Add      (DlyRowH(1), DlyRowH(0),     DlyRowDst(3), m_width);
    Mul      (0.25f,                      DlyRowDst(3), m_width);
    SubRev   (DlyRowL(0),                 DlyRowDst(3), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::ProcessEven_3()
{
    Mul      (0.5f, DlyRowH(0),            DlyRowDst(3), m_width);

    Sub      (DlyRowL(1),  DlyRowL(0),     DlyRowDst(4), m_width);
    Mul      (FWDH1x2,                     DlyRowDst(4), m_width);

    Add      (DlyRowDst(4), DlyRowDst(3),  DlyRowDst(2), m_width);
    SubRev   (DlyRowL(0),                  DlyRowDst(2), m_width);

    Add      (DlyRowL(1),                  DlyRowDst(4),  m_width);
    Sub      (DlyRowDst(3),                DlyRowDst(4), m_width);

    AddMulAdd(0.5f, DlyRowL(1), DlyRowL(0), DlyRowDst(3), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::LiftDELTA(unsigned int dlyL, unsigned int dlyH1, unsigned int dlyH2, unsigned int dlyDst)
{
    Mul      (KH, DlyRowL(dlyL), DlyRowDst(dlyDst), m_width);
    AddMulAdd(-DELTAxKL, DlyRowH(dlyH1), DlyRowH(dlyH2), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::LiftDELTAx2(unsigned int dlyL, unsigned int dlyH1, unsigned int dlyDst)
{
    Mul      (KH, DlyRowL(dlyL), DlyRowDst(dlyDst), m_width);
    MulAdd   (-DELTAxKLx2, DlyRowH(dlyH1), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::LiftGAMMA(unsigned int dlyH, unsigned int dlySrc1, unsigned int dlySrc2, unsigned int dlyDst)
{
    Mul      (KL, DlyRowH(dlyH), DlyRowDst(dlyDst), m_width);
    AddMulAdd(-GAMMA, DlyRowDst(dlySrc1), DlyRowDst(dlySrc2), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::LiftGAMMAx2(unsigned int dlyH, unsigned int dlySrc1, unsigned int dlyDst)
{
    Mul      (KL, DlyRowH(dlyH), DlyRowDst(dlyDst), m_width);
    MulAdd   (-GAMMAx2, DlyRowDst(dlySrc1), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::LiftBETA(unsigned int dlySrc1, unsigned int dlySrc2, unsigned int dlyDst)
{
    AddMulAdd(-BETA, DlyRowDst(dlySrc1), DlyRowDst(dlySrc2), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::LiftBETAx2(unsigned int dlySrc1, unsigned int dlyDst)
{
    MulAdd   (-BETAx2, DlyRowDst(dlySrc1), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::LiftALPHA(unsigned int dlySrc1, unsigned int dlySrc2, unsigned int dlyDst)
{
    AddMulAdd(-ALPHA, DlyRowDst(dlySrc1), DlyRowDst(dlySrc2), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp32f>::LiftALPHAx2(unsigned int dlySrc1, unsigned int dlyDst)
{
    MulAdd   (-ALPHAx2, DlyRowDst(dlySrc1), DlyRowDst(dlyDst), m_width);
}

#endif // __WT97INVCOL32F_H__
