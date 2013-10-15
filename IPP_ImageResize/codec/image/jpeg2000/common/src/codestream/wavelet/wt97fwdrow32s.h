
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

#ifndef __WT97FWDROW32S_H__
#define __WT97FWDROW32S_H__

// Include common, EXTERNAL part of interface
#include "wt97const16s32s.h"

//
//  And defines specific WT97, forward, row-oriented transform INTERNAL routines
//  depended on data type (like border processing and so on).
//  Here is 32s version.
//

inline void WT97FwdFirstStepBig(const Ipp32s* src, Ipp32s* low, Ipp32s* high)
{
    high[0]  = src[1] + (Ipp32s)(((Ipp64s)FIX_ALPHA   * (src [0] + src [2]) + (1 << 15)) >> 16);
    low [0]  = src[0] + (Ipp32s)(((Ipp64s)FIX_BETAx2  *  high[0]            + (1 << 15)) >> 16);
    high[1]  = src[3] + (Ipp32s)(((Ipp64s)FIX_ALPHA   * (src [4] + src [2]) + (1 << 15)) >> 16);
    low [1]  = src[2] + (Ipp32s)(((Ipp64s)FIX_BETA    * (high[0] + high[1]) + (1 << 15)) >> 16);
    high[0] +=          (Ipp32s)(((Ipp64s)FIX_GAMMA   * (low [0] + low [1]) + (1 << 15)) >> 16);
    low [0] +=          (Ipp32s)(((Ipp64s)FIX_DELTAx2 *  high[0]            + (1 << 15)) >> 16);
    low [0]  =          (Ipp32s)(((Ipp64s)FIX_KL      *  low [0]            + (1 << 15)) >> 16);
}

inline void WT97FwdFirstStepSmall(const Ipp32s* src, Ipp32s* low, Ipp32s* high)
{
    high[0]  = src[0] + (Ipp32s)(((Ipp64s)FIX_ALPHAx2 *  src [1]            + (1 << 15)) >> 16);
    high[1]  = src[2] + (Ipp32s)(((Ipp64s)FIX_ALPHA   * (src [1] + src [3]) + (1 << 15)) >> 16);
    low [0]  = src[1] + (Ipp32s)(((Ipp64s)FIX_BETA    * (high[0] + high[1]) + (1 << 15)) >> 16);
    high[0] +=          (Ipp32s)(((Ipp64s)FIX_GAMMAx2 *  low [0]            + (1 << 15)) >> 16);
}

inline void WT97FwdLastStepBig(const Ipp32s* src, Ipp32s* low, Ipp32s* high)
{
    high[2]  = src[1] + (Ipp32s)(((Ipp64s)FIX_ALPHAx2 *  src [0]            + (1 << 15)) >> 16);
    low [1]  = src[0] + (Ipp32s)(((Ipp64s)FIX_BETA    * (high[1] + high[2]) + (1 << 15)) >> 16);
    high[1] +=          (Ipp32s)(((Ipp64s)FIX_GAMMA   * (low [0] + low [1]) + (1 << 15)) >> 16);
    low [0] +=          (Ipp32s)(((Ipp64s)FIX_DELTA   * (high[0] + high[1]) + (1 << 15)) >> 16);
    high[2] +=          (Ipp32s)(((Ipp64s)FIX_GAMMAx2 *  low [1]            + (1 << 15)) >> 16);
    low [1] +=          (Ipp32s)(((Ipp64s)FIX_DELTA   * (high[1] + high[2]) + (1 << 15)) >> 16);
    low [0]  =          (Ipp32s)(((Ipp64s)FIX_KL      *  low [0]            + (1 << 15)) >> 16);
    low [1]  =          (Ipp32s)(((Ipp64s)FIX_KL      *  low [1]            + (1 << 15)) >> 16);
    high[0]  =          (Ipp32s)(((Ipp64s)FIX_KH      *  high[0]            + (1 << 15)) >> 16);
    high[1]  =          (Ipp32s)(((Ipp64s)FIX_KH      *  high[1]            + (1 << 15)) >> 16);
    high[2]  =          (Ipp32s)(((Ipp64s)FIX_KH      *  high[2]            + (1 << 15)) >> 16);
}

inline void WT97FwdLastStepSmall(const Ipp32s* src, Ipp32s* low, Ipp32s* high)
{
    low [1]  = src[0] + (Ipp32s)(((Ipp64s)FIX_BETAx2  *  high[1]            + (1 << 15)) >> 16);
    high[1] +=          (Ipp32s)(((Ipp64s)FIX_GAMMA   * (low [0] + low [1]) + (1 << 15)) >> 16);
    low [0] +=          (Ipp32s)(((Ipp64s)FIX_DELTA   * (high[0] + high[1]) + (1 << 15)) >> 16);
    low [0]  =          (Ipp32s)(((Ipp64s)FIX_KL      *  low [0]            + (1 << 15)) >> 16);
    high[0]  =          (Ipp32s)(((Ipp64s)FIX_KH      *  high[0]            + (1 << 15)) >> 16);
    low [1] +=          (Ipp32s)(((Ipp64s)FIX_DELTAx2 *  high[1]            + (1 << 15)) >> 16);
    low [1]  =          (Ipp32s)(((Ipp64s)FIX_KL      *  low [1]            + (1 << 15)) >> 16);
    high[1]  =          (Ipp32s)(((Ipp64s)FIX_KH      *  high[1]            + (1 << 15)) >> 16);
}


inline void WT97FwdInterStep(const Ipp32s* src, Ipp32s* low, Ipp32s* high)
{
    high[2]  = src[1] + (Ipp32s)(((Ipp64s)FIX_ALPHA * (src [0] + src [2]) + (1 << 15)) >> 16);
    low [1]  = src[0] + (Ipp32s)(((Ipp64s)FIX_BETA  * (high[1] + high[2]) + (1 << 15)) >> 16);
    high[1] +=          (Ipp32s)(((Ipp64s)FIX_GAMMA * (low [0] + low [1]) + (1 << 15)) >> 16);
    low [0] +=          (Ipp32s)(((Ipp64s)FIX_DELTA * (high[0] + high[1]) + (1 << 15)) >> 16);
    low [0]  =          (Ipp32s)(((Ipp64s)FIX_KL    *  low [0]            + (1 << 15)) >> 16);
    high[0]  =          (Ipp32s)(((Ipp64s)FIX_KH    *  high[0]            + (1 << 15)) >> 16);
}

inline void WT97FwdRowFirstL_2(const Ipp32s* src, Ipp32s* low, Ipp32s* high)
{
    high[0] = src[1] - src[0];
    low [0] = src[0] + (high[0] >> 1);
}

inline void WT97FwdRowFirstL_3(const Ipp32s* src, Ipp32s* low, Ipp32s* high)
{
    low [0]  = (Ipp32s)(((Ipp64s)FIX_FWDL4pL0x2 *  src[0] + (1 << 15)) >> 16) +
                                         (src[1] >> 1)               +
               (Ipp32s)(((Ipp64s)FIX_FWDL2x2    *  src[2] + (1 << 15)) >> 16); // (2.0 * L0 + L4) * src[0] + 0.5 * src[1] + 2.0 * L2 * src[2];
    low [1]  = (src[0] + src[2]) >> 1;                                // -(FIX_ALPHA+GAMMA+4.0*GAMMA*FIX_ALPHA*FIX_BETA)*KH*(src[0] + src[2]);

    high[0]  = src[1] - low [1];                                      // (4.0 * FIX_GAMMA * FIX_BETA + 1) * KH * src[1] - tmp;
    low [1] += (src[1] - low[0]);
}

inline void WT97FwdRowFirstL_4(const Ipp32s* src, Ipp32s* low, Ipp32s* high)
{
    high[0]  = src[1] + (Ipp32s)(((Ipp64s)FIX_ALPHA   * (src [0] + src [2]) + (1 << 15)) >> 16);
    low [0]  = src[0] + (Ipp32s)(((Ipp64s)FIX_BETAx2  *  high[0]            + (1 << 15)) >> 16);
    high[1]  = src[3] + (Ipp32s)(((Ipp64s)FIX_ALPHAx2 *  src [2]            + (1 << 15)) >> 16);
    low [1]  = src[2] + (Ipp32s)(((Ipp64s)FIX_BETA    * (high[1] + high[0]) + (1 << 15)) >> 16);
    high[1] +=          (Ipp32s)(((Ipp64s)FIX_GAMMAx2 *  low [1]            + (1 << 15)) >> 16);
    high[0] +=          (Ipp32s)(((Ipp64s)FIX_GAMMA   * (low [1] + low [0]) + (1 << 15)) >> 16);
    low [1] +=          (Ipp32s)(((Ipp64s)FIX_DELTA   * (high[1] + high[0]) + (1 << 15)) >> 16);
    low [0] +=          (Ipp32s)(((Ipp64s)FIX_DELTAx2 *  high[0]            + (1 << 15)) >> 16);
    low [0]  =          (Ipp32s)(((Ipp64s)FIX_KL      *  low [0]            + (1 << 15)) >> 16);
    low [1]  =          (Ipp32s)(((Ipp64s)FIX_KL      *  low [1]            + (1 << 15)) >> 16);
    high[0]  =          (Ipp32s)(((Ipp64s)FIX_KH      *  high[0]            + (1 << 15)) >> 16);
    high[1]  =          (Ipp32s)(((Ipp64s)FIX_KH      *  high[1]            + (1 << 15)) >> 16);
}

inline void WT97FwdRowFirstL_H2MulKH(Ipp32s* high)
{
    high[2] = (Ipp32s)(((Ipp64s)FIX_KH * high[2] + (1 << 15)) >> 16);
}

inline void WT97FwdRowFirstL_Ending(const Ipp32s* src, unsigned int size, Ipp32s* low, Ipp32s* high)
{
    unsigned int i = (size - 5) >> 1;
    unsigned int j = 4 + 2*i;

    Ipp32s *highI = &high[i];
    Ipp32s *lowI  = &low [i];
    const Ipp32s *srcJ  = &src [j];

    Ipp32s tmpL = srcJ[-4] +
                  (Ipp32s)(((Ipp64s)FIX_BETA  * ( srcJ[-5] + srcJ[-3] +
                  (Ipp32s)(((Ipp64s)FIX_ALPHA * (srcJ[-6] + 2 * srcJ[-4] + srcJ[-2]) + (1 << 15)) >> 16) )+ (1 << 15)) >> 16);

    Ipp32s tmpH = srcJ[-3] + (Ipp32s)(((Ipp64s)FIX_ALPHA * (srcJ[-4] + srcJ[-2]) + (1 << 15)) >> 16);

    highI[1]  = srcJ[-1] + (Ipp32s)(((Ipp64s)FIX_ALPHA * (srcJ[-2] + srcJ[0]) + (1 << 15)) >> 16);
    lowI [1]  = srcJ[-2] + (Ipp32s)(((Ipp64s)FIX_BETA  * (tmpH + highI[1]) + (1 << 15)) >> 16);
    tmpH     +=            (Ipp32s)(((Ipp64s)FIX_GAMMA * (tmpL + lowI [1]) + (1 << 15)) >> 16);


    if(size & 1)
    {
        lowI [2]  = srcJ[0] + (Ipp32s)(((Ipp64s)FIX_BETAx2 *  highI[1]             + (1 << 15)) >> 16);
        highI[1] +=           (Ipp32s)(((Ipp64s)FIX_GAMMA  * (lowI [1] + lowI [2]) + (1 << 15)) >> 16);
        lowI [1] +=           (Ipp32s)(((Ipp64s)FIX_DELTA  * (tmpH     + highI[1]) + (1 << 15)) >> 16);
        lowI [1]  =           (Ipp32s)(((Ipp64s)FIX_KL     *  lowI [1]             + (1 << 15)) >> 16);
        lowI [2] +=           (Ipp32s)(((Ipp64s)FIX_DELTAx2 *  highI[1]             + (1 << 15)) >> 16);
        lowI [2]  =           (Ipp32s)(((Ipp64s)FIX_KL     *  lowI [1]             + (1 << 15)) >> 16);
        highI[1]  =           (Ipp32s)(((Ipp64s)FIX_KH     *  lowI [1]             + (1 << 15)) >> 16);
    }
    else
    {
        highI[2]  = srcJ[1] + (Ipp32s)(((Ipp64s)FIX_ALPHAx2 *  srcJ [0]             + (1 << 15)) >> 16);
        lowI [2]  = srcJ[0] + (Ipp32s)(((Ipp64s)FIX_BETA    * (highI[1] + highI[2]) + (1 << 15)) >> 16);
        highI[1] +=           (Ipp32s)(((Ipp64s)FIX_GAMMA   * (lowI [1] + lowI [2]) + (1 << 15)) >> 16);
        lowI [1] +=           (Ipp32s)(((Ipp64s)FIX_DELTA   * (tmpH + highI[1])     + (1 << 15)) >> 16);
        highI[2] +=           (Ipp32s)(((Ipp64s)FIX_GAMMAx2 *  lowI [2]             + (1 << 15)) >> 16);
        lowI [2] +=           (Ipp32s)(((Ipp64s)FIX_DELTA   * (highI[1] + highI[2]) + (1 << 15)) >> 16);
        lowI [1]  =           (Ipp32s)(((Ipp64s)FIX_KL      *  lowI [1]             + (1 << 15)) >> 16);
        lowI [2]  =           (Ipp32s)(((Ipp64s)FIX_KL      *  lowI [2]             + (1 << 15)) >> 16);
        highI[1]  =           (Ipp32s)(((Ipp64s)FIX_KH      *  highI[1]             + (1 << 15)) >> 16);
        highI[2]  =           (Ipp32s)(((Ipp64s)FIX_KH      *  highI[2]             + (1 << 15)) >> 16);
    }
}

inline void WT97FwdRowFirstH_1(const Ipp32s* src, Ipp32s* high)
{
    high[0]  = src[0] << 1;
}

inline void WT97FwdRowFirstH_2(const Ipp32s* src, Ipp32s* low, Ipp32s* high)
{
    high[0] = src[0] - src[1];
    low [0] = src[1] + (high[0] >> 1);
}

inline void WT97FwdRowFirstH_3(const Ipp32s* src, Ipp32s* low, Ipp32s* high)
{
    low [0]  = (Ipp32s)(((Ipp64s)FIX_FWDH1x2 * src[2] + (1 << 15)) >> 16) +
               (Ipp32s)(((Ipp64s)FIX_FWDH3   * src[0] + (1 << 15)) >> 16); //2.0 * H1 * src[2] + H3 * src[0];
    high[0]  = low [0] - src [1];
    high[1]  = low [0] + src [1];
    low [0]  = src [0] + src [2];
    high[1]  = low [0] - high[1];
    low [0]  = (src[1] + (low [0] >> 1)) >> 1;
}

inline void WT97FwdRowFirstH_Ending(const Ipp32s* src, unsigned int size, Ipp32s* low, Ipp32s* high)
{
    unsigned int i = (size - 4) >> 1;
    unsigned int j = 3 + 2*i;

    Ipp32s *highI = &high[i];
    Ipp32s *lowI  = &low [i-1];
    const Ipp32s *srcJ  = &src [j];

    Ipp32s tmpL = srcJ[-4] + (Ipp32s)(((Ipp64s)FIX_BETA  * ( srcJ[-5] + srcJ[-3] +
                             (Ipp32s)(((Ipp64s)FIX_ALPHA * (srcJ[-6] + 2 * srcJ[-4] + srcJ[-2]) + (1 << 15)) >> 16) ) + (1 << 15)) >> 16);

    highI[0] = srcJ[-3] + (Ipp32s)(((Ipp64s)FIX_ALPHA * (srcJ[-4] + srcJ[-2]) + (1 << 15)) >> 16);

    highI[1]  = srcJ[-1] + (Ipp32s)(((Ipp64s)FIX_ALPHA * (srcJ[-2] + srcJ[0])  + (1 << 15)) >> 16);
    lowI [1]  = srcJ[-2] + (Ipp32s)(((Ipp64s)FIX_BETA  * (highI[0] + highI[1]) + (1 << 15)) >> 16);
    highI[0] +=            (Ipp32s)(((Ipp64s)FIX_GAMMA * (tmpL + lowI [1])     + (1 << 15)) >> 16);


    if(size & 1)
    {
        highI[2]  = srcJ[1] + (Ipp32s)(((Ipp64s)FIX_ALPHAx2 *  srcJ [0]             + (1 << 15)) >> 16);
        lowI [2]  = srcJ[0] + (Ipp32s)(((Ipp64s)FIX_BETA    * (highI[1] + highI[2]) + (1 << 15)) >> 16);
        highI[1] +=           (Ipp32s)(((Ipp64s)FIX_GAMMA   * (lowI [1] + lowI [2]) + (1 << 15)) >> 16);
        lowI [1] +=           (Ipp32s)(((Ipp64s)FIX_DELTA   * (highI[0] + highI[1]) + (1 << 15)) >> 16);
        highI[2] +=           (Ipp32s)(((Ipp64s)FIX_GAMMAx2 *  lowI [2]             + (1 << 15)) >> 16);
        lowI [2] +=           (Ipp32s)(((Ipp64s)FIX_DELTA   * (highI[1] + highI[2]) + (1 << 15)) >> 16);
        lowI [1]  =           (Ipp32s)(((Ipp64s)FIX_KL      *  lowI [1]             + (1 << 15)) >> 16);
        lowI [2]  =           (Ipp32s)(((Ipp64s)FIX_KL      *  lowI [2]             + (1 << 15)) >> 16);
        highI[0]  =           (Ipp32s)(((Ipp64s)FIX_KH      *  highI[0]             + (1 << 15)) >> 16);
        highI[1]  =           (Ipp32s)(((Ipp64s)FIX_KH      *  highI[1]             + (1 << 15)) >> 16);
        highI[2]  =           (Ipp32s)(((Ipp64s)FIX_KH      *  highI[2]             + (1 << 15)) >> 16);
    }
    else
    {
        lowI [2]  = srcJ[0] + (Ipp32s)(((Ipp64s)FIX_BETAx2  *  highI[1]             + (1 << 15)) >> 16);
        highI[1] +=           (Ipp32s)(((Ipp64s)FIX_GAMMA   * (lowI [1] + lowI [2]) + (1 << 15)) >> 16);
        lowI [1] +=           (Ipp32s)(((Ipp64s)FIX_DELTA   * (highI[0] + highI[1]) + (1 << 15)) >> 16);
        lowI [1]  =           (Ipp32s)(((Ipp64s)FIX_KL      *  lowI [1]             + (1 << 15)) >> 16);
        lowI [2] +=           (Ipp32s)(((Ipp64s)FIX_DELTAx2 *  highI[1]             + (1 << 15)) >> 16);
        lowI [2]  =           (Ipp32s)(((Ipp64s)FIX_KL      *  lowI [2]             + (1 << 15)) >> 16);
        highI[0]  =           (Ipp32s)(((Ipp64s)FIX_KH      *  highI[0]             + (1 << 15)) >> 16);
        highI[1]  =           (Ipp32s)(((Ipp64s)FIX_KH      *  highI[1]             + (1 << 15)) >> 16);
    }
}

#include "wt97fwdrowgen.h"

#endif // __WT97FWDROW32S_H__
