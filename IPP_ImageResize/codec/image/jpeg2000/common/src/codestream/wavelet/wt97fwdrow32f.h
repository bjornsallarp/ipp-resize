
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

#ifndef __WT97FWDROW32F_H__
#define __WT97FWDROW32F_H__

// Include common, EXTERNAL part of interface
#include "wt97const32f.h"

//
//  And defines specific WT97, forward, row-oriented transform INTERNAL routines
//  depended on data type (like border processing and so on).
//  Here is 32f version.
//

inline void WT97FwdFirstStepBig(const Ipp32f* src, Ipp32f* low, Ipp32f* high)
{
    high[0]  = src[1] + ALPHA  * (src [0] + src [2]);
    low [0]  = src[0] + BETAx2 *  high[0];
    high[1]  = src[3] + ALPHA  * (src [4] + src [2]);
    low [1]  = src[2] + BETA   * (high[0] + high[1]);
    high[0] += GAMMA   * (low [0] + low [1]);
    low [0] += DELTAx2 *  high[0];
    low [0] *= KL;
}

inline void WT97FwdFirstStepSmall(const Ipp32f* src, Ipp32f* low, Ipp32f* high)
{
    high[0]  = src[0] + ALPHAx2 *  src [1];
    high[1]  = src[2] + ALPHA   * (src [1] + src [3]);
    low [0]  = src[1] + BETA    * (high[0] + high[1]);
    high[0] += GAMMAx2 * low [0];
}

inline void WT97FwdLastStepBig(const Ipp32f* src, Ipp32f* low, Ipp32f* high)
{
    high[2]  = src[1] + ALPHAx2 * src [0];
    low [1]  = src[0] + BETA * (high[1] + high[2]);
    high[1] += GAMMA * (low [0] + low [1]);
    low [0] += DELTA * (high[0] + high[1]);
    high[2] += GAMMAx2 *  low [1];
    low [1] += DELTA   * (high[1] + high[2]);
    low [0] *= KL;
    low [1] *= KL;
    high[0] *= KH;
    high[1] *= KH;
    high[2] *= KH;
}

inline void WT97FwdLastStepSmall(const Ipp32f* src, Ipp32f* low, Ipp32f* high)
{
    low [1]  = src[0] + BETAx2  * high[1];
    high[1] += GAMMA * (low [0] + low [1]);
    low [0] += DELTA * (high[0] + high[1]);
    low [0] *= KL;
    high[0] *= KH;
    low [1] += DELTAx2 * high[1];
    low [1] *= KL;
    high[1] *= KH;
}


inline void WT97FwdInterStep(const Ipp32f* src, Ipp32f* low, Ipp32f* high)
{
    high[2]  = src[1] + ALPHA * (src [0] + src [2]);
    low [1]  = src[0] + BETA  * (high[1] + high[2]);
    high[1] += GAMMA * (low [0] + low [1]);
    low [0] += DELTA * (high[0] + high[1]);
    low [0] *= KL;
    high[0] *= KH;
}

inline void WT97FwdRowFirstL_2(const Ipp32f* src, Ipp32f* low, Ipp32f* high)
{
    high[0] = src[1] - src[0];
    low [0] = src[0] + 0.5f * high[0];
}

inline void WT97FwdRowFirstL_3(const Ipp32f* src, Ipp32f* low, Ipp32f* high)
{
    low [0] = FWDL4pL0x2 * src[0] + 0.5f * src[1] + FWDL2x2 * src[2]; // (2.0 * L0 + L4) * src[0] + 0.5 * src[1] + 2.0 * L2 * src[2];
    low [1] = 0.5f * (src[0] + src[2]);                               // -(ALPHA+GAMMA+4.0*GAMMA*ALPHA*BETA)*KH*(src[0] + src[2]);
    high[0] = src[1] - low [1];                                       // (4.0 * GAMMA * BETA + 1) * KH * src[1] - tmp;
    low [1] += (src[1] - low[0]);
}

inline void WT97FwdRowFirstL_4(const Ipp32f* src, Ipp32f* low, Ipp32f* high)
{
    high[0]  = src[1] + ALPHA   * (src [0] + src [2]);
    low [0]  = src[0] + BETAx2  *  high[0];
    high[1]  = src[3] + ALPHAx2 *  src [2];
    low [1]  = src[2] + BETA    * (high[1] + high[0]);
    high[1] += GAMMAx2 *  low[1];
    high[0] += GAMMA   * (low[1] + low[0]);
    low [1] += DELTA   * (high[1] + high[0]);
    low [0] += DELTAx2 * high[0];
    low [0] *= KL;
    low [1] *= KL;
    high[0] *= KH;
    high[1] *= KH;
}

inline void WT97FwdRowFirstL_H2MulKH(Ipp32f* high)
{
    high[2] *= KH;
}

inline void WT97FwdRowFirstL_Ending(const Ipp32f* src, unsigned int size, Ipp32f* low, Ipp32f* high)
{
    unsigned int i = (size - 5) >> 1;
    unsigned int j = 4 + 2*i;

    Ipp32f *highI = &high[i];
    Ipp32f *lowI  = &low [i];
    const Ipp32f *srcJ  = &src [j];

    Ipp32f tmpL = srcJ[-4] + BETA  * ( srcJ[-5] + srcJ[-3] + ALPHA *(srcJ[-6] + 2.0f*srcJ[-4] + srcJ[-2]) );

    Ipp32f tmpH = srcJ[-3] + ALPHA * (srcJ[-4] + srcJ[-2]);

    highI[1]  = srcJ[-1] + ALPHA * (srcJ[-2] + srcJ[0]);
    lowI [1]  = srcJ[-2] + BETA  * (tmpH + highI[1]);
    tmpH += GAMMA * (tmpL + lowI [1]);


    if(size & 1)
    {
        lowI [2]  = srcJ[0] + BETAx2  * highI[1];
        highI[1] += GAMMA * (lowI [1] + lowI [2]);
        lowI [1] += DELTA * (tmpH + highI[1]);
        lowI [1] *= KL;
        lowI [2] += DELTAx2 * highI[1];
        lowI [2] *= KL;
        highI[1] *= KH;
    }
    else
    {
        highI[2]  = srcJ[1] + ALPHAx2 * srcJ [0];
        lowI [2]  = srcJ[0] + BETA * (highI[1] + highI[2]);
        highI[1] += GAMMA * (lowI [1] + lowI [2]);
        lowI [1] += DELTA * (tmpH + highI[1]);
        highI[2] += GAMMAx2 *  lowI [2];
        lowI [2] += DELTA   * (highI[1] + highI[2]);
        lowI [1] *= KL;
        lowI [2] *= KL;
        highI[1] *= KH;
        highI[2] *= KH;
    }
}

inline void WT97FwdRowFirstH_1(const Ipp32f* src, Ipp32f* high)
{
    high[0]  = 2.0f * src[0];
}

inline void WT97FwdRowFirstH_2(const Ipp32f* src, Ipp32f* low, Ipp32f* high)
{
    high[0] = src[0] - src[1];
    low [0] = src[1] + 0.5f * high[0];
}

inline void WT97FwdRowFirstH_3(const Ipp32f* src, Ipp32f* low, Ipp32f* high)
{
    low [0]  = FWDH1x2 * src[2] + FWDH3 * src[0]; //2.0 * H1 * src[2] + H3 * src[0];
    high[0]  = low [0] - src [1];
    high[1]  = low [0] + src [1];
    low [0]  = src [0] + src [2];
    high[1]  = low [0] - high[1];
    low [0]  = 0.5f  * (src[1] + 0.5f * low [0]);
}

inline void WT97FwdRowFirstH_Ending(const Ipp32f* src, unsigned int size, Ipp32f* low, Ipp32f* high)
{
    unsigned int i = (size - 4) >> 1;
    unsigned int j = 3 + 2*i;

    Ipp32f *highI = &high[i];
    Ipp32f *lowI  = &low [i-1];
    const Ipp32f *srcJ  = &src [j];

    Ipp32f tmpL = srcJ[-4] + BETA  * ( srcJ[-5] + srcJ[-3] + ALPHA *(srcJ[-6] + 2.0f*srcJ[-4] + srcJ[-2]) );

    highI[0] = srcJ[-3] + ALPHA * (srcJ[-4] + srcJ[-2]);

    highI[1]  = srcJ[-1] + ALPHA * (srcJ[-2] + srcJ[0]);
    lowI [1]  = srcJ[-2] + BETA  * (highI[0] + highI[1]);
    highI[0] += GAMMA * (tmpL + lowI [1]);


    if(size & 1)
    {
        highI[2]  = srcJ[1] + ALPHAx2 * srcJ [0];
        lowI [2]  = srcJ[0] + BETA * (highI[1] + highI[2]);
        highI[1] += GAMMA * (lowI [1] + lowI [2]);
        lowI [1] += DELTA * (highI[0] + highI[1]);
        highI[2] += GAMMAx2 *  lowI [2];
        lowI [2] += DELTA   * (highI[1] + highI[2]);
        lowI [1] *= KL;
        lowI [2] *= KL;
        highI[0] *= KH;
        highI[1] *= KH;
        highI[2] *= KH;
    }
    else
    {
        lowI [2]  = srcJ[0] + BETAx2  * highI[1];
        highI[1] += GAMMA * (lowI [1] + lowI [2]);
        lowI [1] += DELTA * (highI[0] + highI[1]);
        lowI [1] *= KL;
        lowI [2] += DELTAx2 * highI[1];
        lowI [2] *= KL;
        highI[0] *= KH;
        highI[1] *= KH;
    }
}

#include "wt97fwdrowgen.h"

#endif // __WT97FWDROW32F_H__
