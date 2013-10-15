
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

#ifndef __WT97INVROWMT32F_H__
#define __WT97INVROWMT32F_H__

#include "wt97const32f.h"

inline void LiftDELTA(Ipp32f l, Ipp32f h0, Ipp32f h1, Ipp32f &dst)
{
    dst = l * KH - DELTAxKL * (h0 + h1);
}

inline void LiftGAMMA(Ipp32f h, Ipp32f src0, Ipp32f src2, Ipp32f &dst)
{
    dst = h * KL - GAMMA * (src0 + src2);
}

inline void LiftBETA(Ipp32f src0, Ipp32f src2, Ipp32f &dst)
{
    dst -= BETA  * (src0 + src2);
}

inline void LiftALPHA(Ipp32f src0, Ipp32f src2, Ipp32f &dst)
{
    dst -= ALPHA  * (src0 + src2);
}

#endif // __WT97INVROWMT32F_H__
