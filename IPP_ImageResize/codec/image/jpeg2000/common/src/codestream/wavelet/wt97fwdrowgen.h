
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

#ifndef __WT97FWDROWGEN_H__
#define __WT97FWDROWGEN_H__

#include "pp.h"

template<class T> void WT97FwdRowFirstL(const T* src, unsigned int  size, T* low, T* high);
template<class T> void WT97FwdRowFirstH(const T* src, unsigned int  size, T* low, T* high);

inline void WT97FwdRowFirstH_1(const Ipp16s* src, Ipp16s* high);
inline void WT97FwdRowFirstH_1(const Ipp32s* src, Ipp32s* high);
inline void WT97FwdRowFirstH_1(const Ipp32f* src, Ipp32f* high);

inline void WT97FwdRowFirstH_2(const Ipp32f* src, Ipp32f* low, Ipp32f* high);
inline void WT97FwdRowFirstH_2(const Ipp32s* src, Ipp32s* low, Ipp32s* high);
inline void WT97FwdRowFirstH_2(const Ipp16s* src, Ipp16s* low, Ipp16s* high);

inline void WT97FwdRowFirstH_3(const Ipp32f* src, Ipp32f* low, Ipp32f* high);
inline void WT97FwdRowFirstH_3(const Ipp32s* src, Ipp32s* low, Ipp32s* high);
inline void WT97FwdRowFirstH_3(const Ipp16s* src, Ipp16s* low, Ipp16s* high);

inline void WT97FwdFirstStepSmall(const Ipp32f* src, Ipp32f* low, Ipp32f* high);
inline void WT97FwdFirstStepSmall(const Ipp32s* src, Ipp32s* low, Ipp32s* high);
inline void WT97FwdFirstStepSmall(const Ipp16s* src, Ipp16s* low, Ipp16s* high);

inline void WT97FwdLastStepSmall(const Ipp32f* src, Ipp32f* low, Ipp32f* high);
inline void WT97FwdLastStepSmall(const Ipp32s* src, Ipp32s* low, Ipp32s* high);
inline void WT97FwdLastStepSmall(const Ipp16s* src, Ipp16s* low, Ipp16s* high);

inline void WT97FwdRowFirstL_2(const Ipp32f* src, Ipp32f* low, Ipp32f* high);
inline void WT97FwdRowFirstL_2(const Ipp32s* src, Ipp32s* low, Ipp32s* high);
inline void WT97FwdRowFirstL_2(const Ipp16s* src, Ipp16s* low, Ipp16s* high);

inline void WT97FwdRowFirstL_3(const Ipp32f* src, Ipp32f* low, Ipp32f* high);
inline void WT97FwdRowFirstL_3(const Ipp32s* src, Ipp32s* low, Ipp32s* high);
inline void WT97FwdRowFirstL_3(const Ipp16s* src, Ipp16s* low, Ipp16s* high);

inline void WT97FwdRowFirstL_4(const Ipp32f* src, Ipp32f* low, Ipp32f* high);
inline void WT97FwdRowFirstL_4(const Ipp32s* src, Ipp32s* low, Ipp32s* high);
inline void WT97FwdRowFirstL_4(const Ipp16s* src, Ipp16s* low, Ipp16s* high);

inline void WT97FwdFirstStepBig(const Ipp32f* src, Ipp32f* low, Ipp32f* high);
inline void WT97FwdFirstStepBig(const Ipp32s* src, Ipp32s* low, Ipp32s* high);
inline void WT97FwdFirstStepBig(const Ipp16s* src, Ipp16s* low, Ipp16s* high);

inline void WT97FwdLastStepBig(const Ipp32f* src, Ipp32f* low, Ipp32f* high);
inline void WT97FwdLastStepBig(const Ipp32s* src, Ipp32s* low, Ipp32s* high);
inline void WT97FwdLastStepBig(const Ipp16s* src, Ipp16s* low, Ipp16s* high);

inline void WT97FwdInterStep(const Ipp32f* src, Ipp32f* low, Ipp32f* high);
inline void WT97FwdInterStep(const Ipp32s* src, Ipp32s* low, Ipp32s* high);
inline void WT97FwdInterStep(const Ipp16s* src, Ipp16s* low, Ipp16s* high);

inline void WT97FwdRowFirstL_H2MulKH(Ipp32f* high);
inline void WT97FwdRowFirstL_H2MulKH(Ipp32s* high);
inline void WT97FwdRowFirstL_H2MulKH(Ipp16s* high);

inline void WT97FwdRowFirstL_Ending(const Ipp32f* src, unsigned int size, Ipp32f* low, Ipp32f* high);
inline void WT97FwdRowFirstL_Ending(const Ipp32s* src, unsigned int size, Ipp32s* low, Ipp32s* high);
inline void WT97FwdRowFirstL_Ending(const Ipp16s* src, unsigned int size, Ipp16s* low, Ipp16s* high);

inline void WT97FwdRowFirstH_Ending(const Ipp32f* src, unsigned int size, Ipp32f* low, Ipp32f* high);
inline void WT97FwdRowFirstH_Ending(const Ipp32s* src, unsigned int size, Ipp32s* low, Ipp32s* high);
inline void WT97FwdRowFirstH_Ending(const Ipp16s* src, unsigned int size, Ipp16s* low, Ipp16s* high);

template<class T>
void WT97FwdRow(const T *src, const UIntRange &range, T *low, T *high)
{
    WTPhase phase = Phase(range.Origin());

    if(IsPhaseL(phase)) WT97FwdRowFirstL(src, range.Size(), low, high);
    else                WT97FwdRowFirstH(src, range.Size(), low, high);
}
//
// WT97, fwd, row-oriented transform common code independent from data type
// to be parametrized in template.
//

template<class T>
void WT97FwdRowFirstL(const T* src, unsigned int  size, T* low, T* high)
{
    switch(size)
    {
    case 0: return;
    case 1: low [0] = src[0]; return;
    case 2: WT97FwdRowFirstL_2(src, low, high); return;
    case 3: WT97FwdRowFirstL_3(src, low, high); return;
    case 4: WT97FwdRowFirstL_4(src, low, high); return;
    case 5:
        WT97FwdFirstStepBig(src, low, high);
        WT97FwdLastStepSmall(&src[4], &low[1], high);
        return;
    case 6:
        WT97FwdFirstStepBig(src, low, high);
        WT97FwdLastStepBig(&src[4], &low[1], high);
        return;
    case 7:
        WT97FwdFirstStepBig(src, low, high);
        WT97FwdInterStep(&src[4], &low[1], high);
        WT97FwdLastStepSmall(&src[6], &low[2], &high[1]);
        return;
    case 8:
        WT97FwdFirstStepBig(src, low, high);
        WT97FwdInterStep(&src[4], &low[1], high);
        WT97FwdLastStepBig(&src[6], &low[2], &high[1]);
        return;
    default:
        break;
    }

    WT97FwdFirstStepBig(src, low, high);
    WT97FwdInterStep(&src[4], &low[1], high);
    WT97FwdInterStep(&src[6], &low[2], &high[1]);

    WT97FwdRowFirstL_H2MulKH(high);

    if(size >= 11)
        WT97FwdRow(&src[6], &low [3], &high[3], ( (size + 1) >> 1 ) - 5, ippWTFilterFirstLow);

    WT97FwdRowFirstL_Ending(src, size, low, high);
}

template<class T>
void WT97FwdRowFirstH(const T* src, unsigned int  size, T* low, T* high)
{
    switch(size)
    {
    case 0: return;
    case 1: WT97FwdRowFirstH_1(src,      high); return;
    case 2: WT97FwdRowFirstH_2(src, low, high); return;
    case 3: WT97FwdRowFirstH_3(src, low, high); return;
    case 4:
        WT97FwdFirstStepSmall(src, low, high);
        WT97FwdLastStepSmall(&src[3], low, high);
        return;
    case 5:
        WT97FwdFirstStepSmall(src, low, high);
        WT97FwdLastStepBig(&src[3], low, high);
        return;
    case 6:
        WT97FwdFirstStepSmall(src, low, high);
        WT97FwdInterStep(&src[3], low, high);
        WT97FwdLastStepSmall(&src[5], &low[1], &high[1]);
        return;
    case 7:
        WT97FwdFirstStepSmall(src, low, high);
        WT97FwdInterStep(&src[3], low, high);
        WT97FwdLastStepBig(&src[5], &low[1], &high[1]);
        return;
    case 8:
        WT97FwdFirstStepSmall(src, low, high);
        WT97FwdInterStep(&src[3], low, high);
        WT97FwdInterStep(&src[5], &low[1], &high[1]);
        WT97FwdLastStepSmall(&src[7], &low[2], &high[2]);
        return;
    default:
        break;
    }
    WT97FwdFirstStepSmall(src, low, high);
    WT97FwdInterStep(&src[3], low, high);
    WT97FwdInterStep(&src[5], &low[1], &high[1]);
    WT97FwdInterStep(&src[7], &low[2], &high[2]);

    if(size >= 12)
        WT97FwdRow(&src[6], &low[3], &high[3], ( size >> 1 ) - 5, ippWTFilterFirstHigh);

    WT97FwdRowFirstH_Ending(src, size, low, high);
}

#endif // __WT97FWDROWGEN_H__
