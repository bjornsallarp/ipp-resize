
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

#ifndef __WT97INVROW32S_H__
#define __WT97INVROW32S_H__

// Include common, EXTERNAL part of interface
#include "wt97invrowgen.h"
#include "wt97const16s32s.h"

//
//  And defines specific WT97, forward, row-oriented transform INTERNAL routines
//  depended on data type (like border processing and so on).
//  Here is 32s version.
//

inline void WT97InvInterStep(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst)
{
    dst[4]  = (Ipp32s)(((Ipp64s)FIX_KH       *  low [0]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_DELTAxKL * (high[0] + high[1]) + (1 << 15)) >> 16);
    dst[3]  = (Ipp32s)(((Ipp64s)FIX_KL       *  high[0]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_GAMMA    * (dst [2] + dst [4]) + (1 << 15)) >> 16);
    dst[2] -= (Ipp32s)(((Ipp64s)FIX_BETA     * (dst [1] + dst [3]) + (1 << 15)) >> 16);
    dst[1] -= (Ipp32s)(((Ipp64s)FIX_ALPHA    * (dst [0] + dst [2]) + (1 << 15)) >> 16);
}

inline void WT97InvFirtStepSmall(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst)
{
    dst[0]  = (Ipp32s)(((Ipp64s)FIX_KH         *  low [0]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_DELTAxKLx2 *  high[0]            + (1 << 15)) >> 16);
    dst[2]  = (Ipp32s)(((Ipp64s)FIX_KH         *  low [1]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_DELTAxKL   * (high[0] + high[1]) + (1 << 15)) >> 16);
    dst[1]  = (Ipp32s)(((Ipp64s)FIX_KL         *  high[0]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_GAMMA      * (dst [0] + dst [2]) + (1 << 15)) >> 16);
    dst[0] -= (Ipp32s)(((Ipp64s)FIX_BETAx2     *  dst [1]            + (1 << 15)) >> 16);
}

inline void WT97InvFirtStepBig(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst)
{
    dst[1]  = (Ipp32s)(((Ipp64s)FIX_KH       *  low [0]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_DELTAxKL * (high[0] + high[1]) + (1 << 15)) >> 16);
    dst[0]  = (Ipp32s)(((Ipp64s)FIX_KL       *  high[0]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_GAMMAx2  *  dst [1]            + (1 << 15)) >> 16);
    dst[3]  = (Ipp32s)(((Ipp64s)FIX_KH       *  low [1]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_DELTAxKL * (high[1] + high[2]) + (1 << 15)) >> 16);
    dst[2]  = (Ipp32s)(((Ipp64s)FIX_KL       *  high[1]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_GAMMA    * (dst [1] + dst [3]) + (1 << 15)) >> 16);
    dst[1] -= (Ipp32s)(((Ipp64s)FIX_BETA     * (dst [0] + dst [2]) + (1 << 15)) >> 16);
    dst[0] -= (Ipp32s)(((Ipp64s)FIX_ALPHAx2  *  dst [1]            + (1 << 15)) >> 16);
}

inline void WT97InvLastStepSmall(Ipp32s high, Ipp32s* dst)
{
    dst[3]  = (Ipp32s)(((Ipp64s)FIX_KL      *  high             + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_GAMMAx2 *  dst[2]           + (1 << 15)) >> 16);
    dst[2] -= (Ipp32s)(((Ipp64s)FIX_BETA    * (dst[1] + dst[3]) + (1 << 15)) >> 16);
    dst[3] -= (Ipp32s)(((Ipp64s)FIX_ALPHAx2 *  dst[2]           + (1 << 15)) >> 16);
    dst[1] -= (Ipp32s)(((Ipp64s)FIX_ALPHA   * (dst[2] + dst[0]) + (1 << 15)) >> 16);
}

inline void WT97InvLastStepBig(Ipp32s low, Ipp32s high, Ipp32s* dst)
{
    dst[4]  = (Ipp32s)(((Ipp64s)FIX_KH         *  low              + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_DELTAxKLx2 *  high             + (1 << 15)) >> 16);
    dst[3]  = (Ipp32s)(((Ipp64s)FIX_KL         *  high             + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_GAMMA      * (dst[2] + dst[4]) + (1 << 15)) >> 16);
    dst[4] -= (Ipp32s)(((Ipp64s)FIX_BETAx2     *  dst[3]           + (1 << 15)) >> 16);
    dst[2] -= (Ipp32s)(((Ipp64s)FIX_BETA       * (dst[1] + dst[3]) + (1 << 15)) >> 16);
    dst[1] -= (Ipp32s)(((Ipp64s)FIX_ALPHA      * (dst[0] + dst[2]) + (1 << 15)) >> 16);
    dst[3] -= (Ipp32s)(((Ipp64s)FIX_ALPHA      * (dst[2] + dst[4]) + (1 << 15)) >> 16);
}

inline void WT97InvRowFirstL_2(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst)
{
    dst[0]  = low[0] - (high[0] >> 1);
    dst[1]  = high[0] + dst[0];
}

inline void WT97InvRowFirstL_3(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst)
{
    dst[1]  = high[0] >> 1;
    dst[0]  = (Ipp32s)(((Ipp64s)FIX_FWDH1x2 * (low[1] - low[0]) + (1 << 15)) >> 16);
    dst[2]  = low[1] - dst[0] - dst[1];
    dst[0] += low[0] - dst[1];
    dst[1] += (low[0] + low[1]) >> 1;
}

inline void WT97FwdInvFirstL_Ending(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst, unsigned int  size)
{
    unsigned int offset = ( size >> 1) - 4;
    const Ipp32s *lowI  = &low [offset+1];
    const Ipp32s *highI = &high[offset];
    Ipp32s       *dstJ  = &dst [offset*2-2];

    Ipp32s tmp1  = (Ipp32s)(((Ipp64s)FIX_KH       *  lowI [0]             + (1 << 15)) >> 16) -
                   (Ipp32s)(((Ipp64s)FIX_DELTAxKL * (highI[0] + highI[1]) + (1 << 15)) >> 16);

    lowI++;
    highI++;
    dstJ+=2;

    dstJ[4]  = (Ipp32s)(((Ipp64s)FIX_KH        *  lowI [0]             + (1 << 15)) >> 16) -
               (Ipp32s)(((Ipp64s)FIX_DELTAxKL  * (highI[0] + highI[1]) + (1 << 15)) >> 16);
    Ipp32s tmp2  = (Ipp32s)(((Ipp64s)FIX_KL    *  highI[0]             + (1 << 15)) >> 16) -
                   (Ipp32s)(((Ipp64s)FIX_GAMMA * (tmp1  + dstJ[4])     + (1 << 15)) >> 16);

    lowI++;
    highI++;
    dstJ+=2;

    dstJ[4]  = (Ipp32s)(((Ipp64s)FIX_KH       *  lowI [0]              + (1 << 15)) >> 16) -
               (Ipp32s)(((Ipp64s)FIX_DELTAxKL * (highI[0] + highI[1])  + (1 << 15)) >> 16);
    dstJ[3]  = (Ipp32s)(((Ipp64s)FIX_KL       *  highI[0]              + (1 << 15)) >> 16) -
               (Ipp32s)(((Ipp64s)FIX_GAMMA    * (dstJ [2]  + dstJ [4]) + (1 << 15)) >> 16);
    dstJ[2] -= (Ipp32s)(((Ipp64s)FIX_BETA     * (tmp2      + dstJ [3]) + (1 << 15)) >> 16);

    highI++;
    lowI++;
    dstJ+=2;

    if(size&1)
    {
        dstJ[4]  = (Ipp32s)(((Ipp64s)FIX_KH         *  lowI [0]             + (1 << 15)) >> 16) -
                   (Ipp32s)(((Ipp64s)FIX_DELTAxKLx2 *  highI[0]             + (1 << 15)) >> 16);
        dstJ[3]  = (Ipp32s)(((Ipp64s)FIX_KL         *  highI[0]             + (1 << 15)) >> 16) -
                   (Ipp32s)(((Ipp64s)FIX_GAMMA      * (dstJ [2] + dstJ [4]) + (1 << 15)) >> 16);
        dstJ[4] -= (Ipp32s)(((Ipp64s)FIX_BETAx2     *  dstJ [3]             + (1 << 15)) >> 16);
        dstJ[2] -= (Ipp32s)(((Ipp64s)FIX_BETA       * (dstJ [1] + dstJ [3]) + (1 << 15)) >> 16);
        dstJ[1] -= (Ipp32s)(((Ipp64s)FIX_ALPHA      * (dstJ [0] + dstJ [2]) + (1 << 15)) >> 16);
        dstJ[3] -= (Ipp32s)(((Ipp64s)FIX_ALPHA      * (dstJ [2] + dstJ [4]) + (1 << 15)) >> 16);
    }
    else
    {
        dstJ[3]  = (Ipp32s)(((Ipp64s)FIX_KL      *  highI[0]             + (1 << 15)) >> 16) -
                   (Ipp32s)(((Ipp64s)FIX_GAMMAx2 *  dstJ [2]             + (1 << 15)) >> 16);
        dstJ[2] -= (Ipp32s)(((Ipp64s)FIX_BETA    * (dstJ [1] + dstJ [3]) + (1 << 15)) >> 16);
        dstJ[3] -= (Ipp32s)(((Ipp64s)FIX_ALPHAx2 *  dstJ [2]             + (1 << 15)) >> 16);
        dstJ[1] -= (Ipp32s)(((Ipp64s)FIX_ALPHA   * (dstJ [2] + dstJ [0]) + (1 << 15)) >> 16);
    }
}

inline void WT97InvRowFirstH_1(Ipp32s high, Ipp32s* dst)
{
    dst[0] = high >> 1;
}

inline void WT97InvRowFirstH_2(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst)
{
    dst[1] = low[0] - (high[0] >> 1);
    dst[0] = high[0] + dst[1];
}

inline void WT97InvRowFirstH_3(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst)
{
    dst[1] = high[1] - high[0];
    dst[0] = low[0] + (high[0] >> 1) + (Ipp32s)(((Ipp64s)FIX_FWDL2x2 * dst[1] + (1 << 15)) >> 16);
    dst[2] = (Ipp32s)(((Ipp64s)FIX_HALFmFWDL2x4 * dst[1] + (1 << 15)) >> 16) + dst[0];
    dst[1] = low[0] - ((high[1] + high[0]) >> 2);
}

inline void WT97InvRowFirstH_4(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst)
{
    dst[1]  = (Ipp32s)(((Ipp64s)FIX_KH         *  low [0]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_DELTAxKL   * (high[0] + high[1]) + (1 << 15)) >> 16);
    dst[3]  = (Ipp32s)(((Ipp64s)FIX_KH         *  low [1]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_DELTAxKLx2 *  high[1]            + (1 << 15)) >> 16);
    dst[2]  = (Ipp32s)(((Ipp64s)FIX_KL         *  high[1]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_GAMMA      * (dst [1] + dst [3]) + (1 << 15)) >> 16);
    dst[0]  = (Ipp32s)(((Ipp64s)FIX_KL         *  high[0]            + (1 << 15)) >> 16) -
              (Ipp32s)(((Ipp64s)FIX_GAMMAx2    *  dst [1]            + (1 << 15)) >> 16);
    dst[3] -= (Ipp32s)(((Ipp64s)FIX_BETAx2     *  dst [2]            + (1 << 15)) >> 16);
    dst[1] -= (Ipp32s)(((Ipp64s)FIX_BETA       * (dst [0] + dst [2]) + (1 << 15)) >> 16);
    dst[2] -= (Ipp32s)(((Ipp64s)FIX_ALPHA      * (dst [1] + dst [3]) + (1 << 15)) >> 16);
    dst[0] -= (Ipp32s)(((Ipp64s)FIX_ALPHAx2    *  dst [1]            + (1 << 15)) >> 16);
}

inline void WT97FwdInvFirstH_Ending(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst, unsigned int  size)
{
    unsigned int offset = ((size + 1) >> 1) - 4;
    const Ipp32s *lowI  = &low [offset + 1];
    const Ipp32s *highI = &high[offset + 1];
    Ipp32s       *dstJ  = &dst [offset * 2 - 1];

    Ipp32s tmp = (Ipp32s)(((Ipp64s)FIX_KH       *  lowI [0]             + (1 << 15)) >> 16) -
                 (Ipp32s)(((Ipp64s)FIX_DELTAxKL * (highI[0] + highI[1]) + (1 << 15)) >> 16);

    lowI++;
    highI++;
    dstJ+=2;

    dstJ[4]  = (Ipp32s)(((Ipp64s)FIX_KH       *  lowI [0]             + (1 << 15)) >> 16) -
               (Ipp32s)(((Ipp64s)FIX_DELTAxKL * (highI[0] + highI[1]) + (1 << 15)) >> 16);
    dstJ[3]  = (Ipp32s)(((Ipp64s)FIX_KL       *  highI[0]             + (1 << 15)) >> 16) -
               (Ipp32s)(((Ipp64s)FIX_GAMMA    * (tmp      + dstJ [4]) + (1 << 15)) >> 16);

    lowI++;
    highI++;
    dstJ+=2;

    if(size&1)
    {
        dstJ[3]  = (Ipp32s)(((Ipp64s)FIX_KL      *  highI[0]             + (1 << 15)) >> 16) -
                   (Ipp32s)(((Ipp64s)FIX_GAMMAx2 *  dstJ [2]             + (1 << 15)) >> 16);
        dstJ[2] -= (Ipp32s)(((Ipp64s)FIX_BETA    * (dstJ [1] + dstJ [3]) + (1 << 15)) >> 16);
        dstJ[3] -= (Ipp32s)(((Ipp64s)FIX_ALPHAx2 *  dstJ [2]             + (1 << 15)) >> 16);
        dstJ[1] -= (Ipp32s)(((Ipp64s)FIX_ALPHA   * (dstJ [2] + dstJ [0]) + (1 << 15)) >> 16);
    }
    else
    {
        dstJ[4]  = (Ipp32s)(((Ipp64s)FIX_KH         *  lowI [0]             + (1 << 15)) >> 16) -
                   (Ipp32s)(((Ipp64s)FIX_DELTAxKLx2 *  highI[0]             + (1 << 15)) >> 16);
        dstJ[3]  = (Ipp32s)(((Ipp64s)FIX_KL         *  highI[0]             + (1 << 15)) >> 16) -
                   (Ipp32s)(((Ipp64s)FIX_GAMMA      * (dstJ [2] + dstJ [4]) + (1 << 15)) >> 16);
        dstJ[4] -= (Ipp32s)(((Ipp64s)FIX_BETAx2     *  dstJ [3]             + (1 << 15)) >> 16);
        dstJ[2] -= (Ipp32s)(((Ipp64s)FIX_BETA       * (dstJ [1] + dstJ [3]) + (1 << 15)) >> 16);
        dstJ[1] -= (Ipp32s)(((Ipp64s)FIX_ALPHA      * (dstJ [0] + dstJ [2]) + (1 << 15)) >> 16);
        dstJ[3] -= (Ipp32s)(((Ipp64s)FIX_ALPHA      * (dstJ [2] + dstJ [4]) + (1 << 15)) >> 16);
    }
}

#endif // __WT97INVROW32S_H__
