
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

#ifndef __WT97INVROWGEN_H__
#define __WT97INVROWGEN_H__

#include "pp.h"

inline void WT97InvRowFirstL_2(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst);
inline void WT97InvRowFirstL_2(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst);
inline void WT97InvRowFirstL_2(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst);

inline void WT97InvRowFirstL_3(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst);
inline void WT97InvRowFirstL_3(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst);
inline void WT97InvRowFirstL_3(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst);

inline void WT97InvFirtStepSmall(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst);
inline void WT97InvFirtStepSmall(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst);
inline void WT97InvFirtStepSmall(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst);

inline void WT97InvLastStepSmall(Ipp32f high, Ipp32f* dst);
inline void WT97InvLastStepSmall(Ipp32s high, Ipp32s* dst);
inline void WT97InvLastStepSmall(Ipp16s high, Ipp16s* dst);

inline void WT97InvLastStepBig(Ipp32f low, Ipp32f high, Ipp32f* dst);
inline void WT97InvLastStepBig(Ipp32s low, Ipp32s high, Ipp32s* dst);
inline void WT97InvLastStepBig(Ipp16s low, Ipp16s high, Ipp16s* dst);

inline void WT97InvFirtStepSmall(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst);
inline void WT97InvFirtStepSmall(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst);
inline void WT97InvFirtStepSmall(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst);

inline void WT97InvInterStep(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst);
inline void WT97InvInterStep(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst);
inline void WT97InvInterStep(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst);

inline void WT97FwdInvFirstL_Ending(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst, unsigned int  size);
inline void WT97FwdInvFirstL_Ending(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst, unsigned int  size);
inline void WT97FwdInvFirstL_Ending(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst, unsigned int  size);

inline void WT97InvRowFirstH_1(Ipp32f high, Ipp32f* dst);
inline void WT97InvRowFirstH_1(Ipp32s high, Ipp32s* dst);
inline void WT97InvRowFirstH_1(Ipp16s high, Ipp16s* dst);

inline void WT97InvRowFirstH_2(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst);
inline void WT97InvRowFirstH_2(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst);
inline void WT97InvRowFirstH_2(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst);

inline void WT97InvRowFirstH_3(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst);
inline void WT97InvRowFirstH_3(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst);
inline void WT97InvRowFirstH_3(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst);

inline void WT97InvRowFirstH_4(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst);
inline void WT97InvRowFirstH_4(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst);
inline void WT97InvRowFirstH_4(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst);

inline void WT97InvFirtStepBig(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst);
inline void WT97InvFirtStepBig(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst);
inline void WT97InvFirtStepBig(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst);

inline void WT97FwdInvFirstH_Ending(const Ipp32f* low, const Ipp32f* high, Ipp32f* dst, unsigned int  size);
inline void WT97FwdInvFirstH_Ending(const Ipp32s* low, const Ipp32s* high, Ipp32s* dst, unsigned int  size);
inline void WT97FwdInvFirstH_Ending(const Ipp16s* low, const Ipp16s* high, Ipp16s* dst, unsigned int  size);
//
// WT97, fwd, row-oriented transform common code independent from data type
// to be parametrized in template.
//

template<class T>
inline void WT97InvRowFirstL(const T* low, const T* high, T* dst, unsigned int  size)
{
    switch(size)
    {
    case 0:
        return;
    case 1:
        dst[0] = low[0];
        return;
    case 2:
        WT97InvRowFirstL_2(low, high, dst);
        return;
    case 3:
        WT97InvRowFirstL_3(low, high, dst);
        return;
    case 4:
        WT97InvFirtStepSmall(low, high, dst);
        WT97InvLastStepSmall(high[1], dst);
        return;
    case 5:
        WT97InvFirtStepSmall(low, high, dst);
        WT97InvLastStepBig(low[2], high[1], dst);
        return;
    case 6:
        WT97InvFirtStepSmall(low, high, dst);
        WT97InvInterStep(&low[2], &high[1], dst);
        WT97InvLastStepSmall(high[2], &dst[2]);
        return;
    case 7:
        WT97InvFirtStepSmall(low, high, dst);
        WT97InvInterStep    (&low[2], &high[1], dst);
        WT97InvLastStepBig  (low[3], high[2], &dst[2]);
        return;
    case 8:
        WT97InvFirtStepSmall(low, high, dst);
        WT97InvInterStep    (&low[2], &high[1],  dst);
        WT97InvInterStep    (&low[3], &high[2], &dst[2]);
        WT97InvLastStepSmall(high[3], &dst[4]);
        return;
    case 9:
        WT97InvFirtStepSmall(low, high, dst);
        WT97InvInterStep    (&low[2], &high[1],  dst);
        WT97InvInterStep    (&low[3], &high[2], &dst[2]);
        WT97InvLastStepBig  (low[4], high[3], &dst[4]);
        return;
    default:
        break;
    }

    WT97InvFirtStepSmall(low, high, dst);
    WT97InvInterStep    (&low[2], &high[1],  dst);
    WT97InvInterStep    (&low[3], &high[2], &dst[2]);

    WT97InvRow(&low[2], &high[2], (size >> 1) - 4, &dst [4], ippWTFilterFirstLow);

    WT97FwdInvFirstL_Ending(low, high, dst, size);
}


template<class T>
inline void WT97InvRowFirstH(const T* low, const T* high, T* dst, unsigned int  size)
{
    switch(size)
    {
    case 0:
        return;
    case 1:
        WT97InvRowFirstH_1(high[0], dst);
        return;
    case 2:
        WT97InvRowFirstH_2(low, high, dst);
        return;
    case 3:
        WT97InvRowFirstH_3(low, high, dst);
        return;
    case 4:
        WT97InvRowFirstH_4(low, high, dst);
        return;
    case 5:
        WT97InvFirtStepBig(low, high, dst);
        WT97InvLastStepSmall(high[2], &dst[1]);
        return;
    case 6:
        WT97InvFirtStepBig(low, high, dst);
        WT97InvLastStepBig(low[2], high[2], &dst[1]);
        return;
    case 7:
        WT97InvFirtStepBig  (low, high, dst);
        WT97InvInterStep    (&low[2], &high[2], &dst[1]);
        WT97InvLastStepSmall(high[3], &dst[3]);
        return;
    case 8:
        WT97InvFirtStepBig  (low, high, dst);
        WT97InvInterStep    (&low[2], &high[2], &dst[1]);
        WT97InvLastStepBig  (low[3], high[3], &dst[3]);
        return;
    default:
        break;
    }

    WT97InvFirtStepBig  (low, high, dst);
    WT97InvInterStep    (&low[2], &high[2], &dst[1]);

    WT97InvRow(&low[2], &high[2], ((size + 1) >> 1) - 4, &dst [4], ippWTFilterFirstHigh);

    WT97FwdInvFirstH_Ending(low, high, dst, size);
}

template<class T>
inline void WT97InvRow(const T *low, const T *high, T *dst, const UIntRange &range)
{
    WTPhase phase = Phase(range.Origin());

    if(phase == ippWTFilterFirstLow) WT97InvRowFirstL(low, high, dst, range.Size());
    else                             WT97InvRowFirstH(low, high, dst, range.Size());
}

#endif // __WT97INVROWGEN_H__

