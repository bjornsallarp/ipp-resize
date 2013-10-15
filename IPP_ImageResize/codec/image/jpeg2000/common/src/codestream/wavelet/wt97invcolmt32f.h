
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2007-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __WT97INVCOLMT32F_H__
#define __WT97INVCOLMT32F_H__

#include "wt97const32f.h"
#include "calcvector.h"


inline void LiftDELTA(const Ipp32f *l, const Ipp32f *h0, const Ipp32f *h1, Ipp32f *dst, unsigned int width)
{
    Mul      (KH, l, dst, width);
    AddMulAdd(-DELTAxKL, h0, h1, dst, width);
}

inline void LiftGAMMA(const Ipp32f *h, const Ipp32f *src0, const Ipp32f *src2, Ipp32f *dst, unsigned int width)
{
    Mul      (KL, h, dst, width);
    AddMulAdd(-GAMMA, src0, src2, dst, width);
}

inline void LiftBETA(const Ipp32f *src0, const Ipp32f *src2, Ipp32f *dst, unsigned int width)
{
    AddMulAdd(-BETA, src0, src2, dst, width);
}

inline void LiftALPHA(const Ipp32f *src0, const Ipp32f *src2, Ipp32f *dst, unsigned int width)
{
    AddMulAdd(-ALPHA, src0, src2, dst, width);
}

#endif // __WT97INVCOLMT32F_H__
