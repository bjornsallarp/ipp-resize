
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

#ifndef __WT97INVROW32F_H__
#define __WT97INVROW32F_H__

// Include common, EXTERNAL part of interface
#include "wt97invrowgen.h"
#include "wt97const32f.h"

//
//  And defines specific WT97, forward, row-oriented transform INTERNAL routines
//  depended on data type (like border processing and so on).
//  Here is 32f version.
//

inline void WT97InvInterStep(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst)
{
    dst[4]  = low[0]  * KH - DELTAxKL * (high[0] + high[1]);
    dst[3]  = high[0] * KL - GAMMA    * (dst[2]  + dst[4]);
    dst[2] -= BETA  * (dst[1] + dst[3]);
    dst[1] -= ALPHA * (dst[0] + dst[2]);
}

inline void WT97InvFirtStepSmall(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst)
{
    dst[0]  = low [0] * KH - DELTAxKLx2 * high[0];
    dst[2]  = low [1] * KH - DELTAxKL   * (high[0]+high[1]);
    dst[1]  = high[0] * KL - GAMMA      * (dst[0]  + dst[2]);
    dst[0] -= BETAx2  * dst[1];
}

inline void WT97InvFirtStepBig(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst)
{
    dst[1]  = low [0] * KH - DELTAxKL   * (high[0]+high[1]);
    dst[0]  = high[0] * KL - GAMMAx2    * dst[1];
    dst[3]  = low [1] * KH - DELTAxKL   * (high[1]+high[2]);
    dst[2]  = high[1] * KL - GAMMA      * (dst[1]  + dst[3]);
    dst[1] -= BETA    * (dst[0] + dst[2]);
    dst[0] -= ALPHAx2 *  dst[1];
}

inline void WT97InvLastStepSmall(Ipp32f high, Ipp32f* dst)
{
    dst[3]  = high * KL - GAMMAx2    * dst[2];
    dst[2] -= BETA    * (dst[1] + dst[3]);
    dst[3] -= ALPHAx2 * dst[2];
    dst[1] -= ALPHA   * (dst[2] + dst[0]);
}

inline void WT97InvLastStepBig(Ipp32f low, Ipp32f high, Ipp32f* dst)
{
    dst[4]  = low  * KH - DELTAxKLx2 * high;
    dst[3]  = high * KL - GAMMA      * (dst[2]  + dst[4]);
    dst[4] -= BETAx2  * dst[3];
    dst[2] -= BETA    * (dst[1] + dst[3]);
    dst[1] -= ALPHA   * (dst[0] + dst[2]);
    dst[3] -= ALPHA   * (dst[2] + dst[4]);
}

inline void WT97InvRowFirstL_2(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst)
{
    dst[0]  = low[0] - 0.5f * high[0];
    dst[1]  = high[0] + dst[0];
}

inline void WT97InvRowFirstL_3(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst)
{
    dst[1]  = 0.5f * high[0];
    dst[0]  = FWDH1x2 * (low[1] - low[0]);
    dst[2]  = low[1] - dst[0] - dst[1];
    dst[0] += low[0] - dst[1];
    dst[1] +=  0.5f * (low[0] + low[1]);
}

inline void WT97FwdInvFirstL_Ending(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst, unsigned int  size)
{
    unsigned int offset = ( size >> 1) - 4;
    const Ipp32f *lowI  = &low [offset+1];
    const Ipp32f *highI = &high[offset];
    Ipp32f       *dstJ  = &dst [offset*2-2];

    Ipp32f tmp1  = lowI[0]  * KH - DELTAxKL * (highI[0] + highI[1]);

    lowI++;
    highI++;
    dstJ+=2;

    dstJ[4]  = lowI[0]  * KH - DELTAxKL * (highI[0] + highI[1]);
    Ipp32f tmp2  = highI[0] * KL - GAMMA    * (tmp1  + dstJ[4]);

    lowI++;
    highI++;
    dstJ+=2;

    dstJ[4]  = lowI[0]  * KH - DELTAxKL * (highI[0] + highI[1]);
    dstJ[3]  = highI[0] * KL - GAMMA    * (dstJ[2]  + dstJ[4]);
    dstJ[2] -= BETA  * (tmp2 + dstJ[3]);

    highI++;
    lowI++;
    dstJ+=2;

    if(size&1)
    {
        dstJ[4]  = lowI[0]  * KH - DELTAxKLx2 * highI[0];
        dstJ[3]  = highI[0] * KL - GAMMA      * (dstJ[2]  + dstJ[4]);
        dstJ[4] -= BETAx2  * dstJ[3];
        dstJ[2] -= BETA    * (dstJ[1] + dstJ[3]);
        dstJ[1] -= ALPHA   * (dstJ[0] + dstJ[2]);
        dstJ[3] -= ALPHA   * (dstJ[2] + dstJ[4]);
    }
    else
    {
        dstJ[3]  = highI[0] * KL - GAMMAx2 * dstJ[2];
        dstJ[2] -= BETA    * (dstJ[1] + dstJ[3]);
        dstJ[3] -= ALPHAx2 * dstJ[2];
        dstJ[1] -= ALPHA   * (dstJ[2] + dstJ[0]);
    }
}

inline void WT97InvRowFirstH_1(Ipp32f high, Ipp32f* dst)
{
    dst[0] = 0.5f * high;
}

inline void WT97InvRowFirstH_2(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst)
{
    dst[1] = low[0] - 0.5f * high[0];
    dst[0] = high[0] + dst[1];
}

inline void WT97InvRowFirstH_3(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst)
{
    dst[1] = high[1] - high[0];
    dst[0] = low[0] + 0.5f * high[0] + FWDL2x2 * dst[1];
    dst[2] = HALFmFWDL2x4 * dst[1] + dst[0];
    dst[1] = low[0] - 0.25f * (high[1] + high[0]);
}

inline void WT97InvRowFirstH_4(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst)
{
    dst[1]  = low [0] * KH - DELTAxKL   * (high[0]+high[1]);
    dst[3]  = low [1] * KH - DELTAxKLx2   * high[1];
    dst[2]  = high[1] * KL - GAMMA      * (dst[1]  + dst[3]);
    dst[0]  = high[0] * KL - GAMMAx2    * dst[1];
    dst[3] -= BETAx2  * dst[2];
    dst[1] -= BETA    * (dst[0] + dst[2]);
    dst[2] -= ALPHA   * (dst[1] + dst[3]);
    dst[0] -= ALPHAx2 *  dst[1];
}

inline void WT97FwdInvFirstH_Ending(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst, unsigned int  size)
{
    unsigned int offset = ((size + 1) >> 1) - 4;
    const Ipp32f *lowI  = &low [offset + 1];
    const Ipp32f *highI = &high[offset + 1];
    Ipp32f       *dstJ  = &dst [offset * 2 - 1];

    Ipp32f tmp = lowI[0]  * KH - DELTAxKL * (highI[0] + highI[1]);

    lowI++;
    highI++;
    dstJ+=2;

    dstJ[4]  = lowI[0]  * KH - DELTAxKL * (highI[0] + highI[1]);
    dstJ[3]  = highI[0] * KL - GAMMA    * (tmp  + dstJ[4]);

    lowI++;
    highI++;
    dstJ+=2;

    if(size&1)
    {
        dstJ[3]  = highI[0] * KL - GAMMAx2 * dstJ[2];
        dstJ[2] -= BETA    * (dstJ[1] + dstJ[3]);
        dstJ[3] -= ALPHAx2 * dstJ[2];
        dstJ[1] -= ALPHA   * (dstJ[2] + dstJ[0]);
    }
    else
    {
        dstJ[4]  = lowI[0]  * KH - DELTAxKLx2 * highI[0];
        dstJ[3]  = highI[0] * KL - GAMMA      * (dstJ[2]  + dstJ[4]);
        dstJ[4] -= BETAx2  * dstJ[3];
        dstJ[2] -= BETA    * (dstJ[1] + dstJ[3]);
        dstJ[1] -= ALPHA   * (dstJ[0] + dstJ[2]);
        dstJ[3] -= ALPHA   * (dstJ[2] + dstJ[4]);
    }
}

#endif // __WT97INVROW32F_H__
