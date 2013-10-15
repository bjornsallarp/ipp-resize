
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

#ifndef __WT97INVCOL16S_H__
#define __WT97INVCOL16S_H__

// Include common, EXTERNAL part of interface
#include "wt97invcolgen.h"

#include "calcvector.h"
#include "wt97const16s32s.h"

//
//  And defines specific WT97, inverse, column-oriented transform INTERNAL routines
//  depended on data type (like border processing and so on).
//  Here is 16s version.
//

template<> void BWT97InvColPipeline<Ipp16s>::ProcessOdd_1()
{
    MulRoundSF16((1 << 15), DlyRowH(0), DlyRowDst(0), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::ProcessOdd_2()
{
    MulRoundSF16((1 << 15), DlyRowH(0),    DlyRowDst(3), m_width);
    SubRev      (DlyRowL(0), DlyRowDst(3), m_width);
    Add         (DlyRowH(0), DlyRowDst(3), DlyRowDst(4), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::ProcessEven_2()
{
    MulRoundSF16((1 << 15), DlyRowH(0),    DlyRowDst(4), m_width);
    SubRev      (DlyRowL(0),               DlyRowDst(4), m_width);
    Add         (DlyRowH(0), DlyRowDst(4), DlyRowDst(3), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::ProcessOdd_3()
{
    Sub            (DlyRowH(1), DlyRowH(0),  DlyRowDst(3), m_width);

    MulRoundSF16   (FIX_FWDL2x2, DlyRowDst(3), DlyRowDst(4), m_width);
    MulAddRoundSF16((1 << 15),   DlyRowH(1),   DlyRowDst(4), m_width);
    Add            (DlyRowL(0), DlyRowDst(4), m_width);

    MulRoundSF16   (FIX_HALFmFWDL2x4, DlyRowDst(3), DlyRowDst(2), m_width);
    Add            (DlyRowDst(4), DlyRowDst(2), m_width);

    Add            (DlyRowH(1), DlyRowH(0), DlyRowDst(3), m_width);
    MulRoundSF16   ((1 << 14),  DlyRowDst(3), m_width);
    SubRev         (DlyRowL(0), DlyRowDst(3), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::ProcessEven_3()
{
    MulRoundSF16((1 << 15), DlyRowH(0),        DlyRowDst(3), m_width);

    Sub         (DlyRowL(1),  DlyRowL(0), DlyRowDst(4), m_width);
    MulRoundSF16(FIX_FWDH1x2, DlyRowDst(4), m_width);

    Add         (DlyRowDst(4), DlyRowDst(3),  DlyRowDst(2), m_width);
    SubRev      (DlyRowL(0),                  DlyRowDst(2), m_width);

    Add         (DlyRowL(1),    DlyRowDst(4),  m_width);
    Sub         (DlyRowDst(3),  DlyRowDst(4), m_width);

    AddMulAddRoundSF16((1 << 15), DlyRowL(1), DlyRowL(0), DlyRowDst(3), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::LiftDELTA(unsigned int dlyL, unsigned int dlyH1, unsigned int dlyH2, unsigned int dlyDst)
{
    MulRoundSF16(FIX_KH, DlyRowL(dlyL), DlyRowDst(dlyDst), m_width);
    AddMulAddRoundSF16(-FIX_DELTAxKL, DlyRowH(dlyH1), DlyRowH(dlyH2), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::LiftDELTAx2(unsigned int dlyL, unsigned int dlyH1, unsigned int dlyDst)
{
    MulRoundSF16   (FIX_KH, DlyRowL(dlyL), DlyRowDst(dlyDst), m_width);
    MulAddRoundSF16(-FIX_DELTAxKLx2, DlyRowH(dlyH1), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::LiftGAMMA(unsigned int dlyH, unsigned int dlySrc1, unsigned int dlySrc2, unsigned int dlyDst)
{
    MulRoundSF16(FIX_KL, DlyRowH(dlyH), DlyRowDst(dlyDst), m_width);
    AddMulAddRoundSF16(-FIX_GAMMA, DlyRowDst(dlySrc1), DlyRowDst(dlySrc2), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::LiftGAMMAx2(unsigned int dlyH, unsigned int dlySrc1, unsigned int dlyDst)
{
    MulRoundSF16   (FIX_KL, DlyRowH(dlyH), DlyRowDst(dlyDst), m_width);
    MulAddRoundSF16(-FIX_GAMMAx2, DlyRowDst(dlySrc1), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::LiftBETA(unsigned int dlySrc1, unsigned int dlySrc2, unsigned int dlyDst)
{
    AddMulAddRoundSF16(-FIX_BETA, DlyRowDst(dlySrc1), DlyRowDst(dlySrc2), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::LiftBETAx2(unsigned int dlySrc1, unsigned int dlyDst)
{
    MulAddRoundSF16(-FIX_BETAx2, DlyRowDst(dlySrc1), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::LiftALPHA(unsigned int dlySrc1, unsigned int dlySrc2, unsigned int dlyDst)
{
    AddMulAddRoundSF16(-FIX_ALPHA, DlyRowDst(dlySrc1), DlyRowDst(dlySrc2), DlyRowDst(dlyDst), m_width);
}

template<> void BWT97InvColPipeline<Ipp16s>::LiftALPHAx2(unsigned int dlySrc1, unsigned int dlyDst)
{
    MulAddRoundSF16(-FIX_ALPHAx2, DlyRowDst(dlySrc1), DlyRowDst(dlyDst), m_width);
}

#endif // __WT97INVCOL16S_H__
