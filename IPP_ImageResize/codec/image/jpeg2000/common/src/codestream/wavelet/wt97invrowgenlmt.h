
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

#ifndef __WT97INVROWGENLMT_H__
#define __WT97INVROWGENLMT_H__

#include "geometry1d.h"
#include "wt97invborderindexlmt.h"

template<class T>
void WT97InvBegStepFirstL(
    T  l_0,
    T  l_1,
    T  l_2,
    T  h_0,
    T  h_1,
    T  h_2,
    T  h_3,
    T &dst_0,
    T &dst_1,
    T &dst_2,
    T &dst_3,
    T &dst_4)
{
    LiftDELTA(l_0, h_0, h_1, dst_0);

    LiftDELTA(l_1, h_1, h_2, dst_2);
    LiftGAMMA(h_1, dst_0, dst_2, dst_1);

    LiftDELTA(l_2, h_2, h_3, dst_4);
    LiftGAMMA(h_2, dst_2, dst_4, dst_3);
    LiftBETA (dst_1, dst_3, dst_2);
}

template<class T>
void WT97InvInterStep(
    T  l_0,
    T  h_0,
    T  h_1,
    T  dst_0,
    T &dst_1,
    T &dst_2,
    T &dst_3,
    T &dst_4)
{
    LiftDELTA(l_0, h_0, h_1, dst_4);
    LiftGAMMA(h_0, dst_2, dst_4, dst_3);
    LiftBETA (dst_1, dst_3, dst_2);
    LiftALPHA(dst_0, dst_2, dst_1);
}

template<class T>
void WT97InvInterStep(const T* l, const T* h, T* dst)
{
    WT97InvInterStep(l[0], h[0], h[1], dst[0], dst[1], dst[2], dst[3], dst[4]);
}



template<class T>
void WT97InvRowFirstL_1(
    const T      *blockL     ,
    const T      *blockH     ,
          T      *dst        ,
    unsigned int  toBegBorder,
    unsigned int  toEndBorder)
{
    if(toBegBorder+toEndBorder == 0)
    {
        dst[0] = blockL[0];
        return;
    }

    toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;
    toEndBorder = toEndBorder > 3 ? 3 : toEndBorder;

    T blockL_m_1 = blockL[ WT97L1INDEX_L_m_1[toEndBorder][toBegBorder] ];
    T blockL_p_1 = blockL[ WT97L1INDEX_L_p_1[toEndBorder][toBegBorder] ];

    T blockH_m_2 = blockH[ WT97L1INDEX_H_m_2[toEndBorder][toBegBorder] ];
    T blockH_m_1 = blockH[ WT97L1INDEX_H_m_1[toEndBorder][toBegBorder] ];
    T blockH_p_0 = blockH[ WT97L1INDEX_H_p_0[toEndBorder][toBegBorder] ];
    T blockH_p_1 = blockH[ WT97L1INDEX_H_p_1[toEndBorder][toBegBorder] ];

    T tmp1, tmp2;

    WT97InvBegStepFirstL(
        blockL_m_1,
        blockL[ 0],
        blockL_p_1,
        blockH_m_2,
        blockH_m_1,
        blockH_p_0,
        blockH_p_1,
        tmp1,
        tmp1,
        dst[0],
        tmp2,
        tmp2);
}

template<class T>
void WT97InvRowFirstL_2(
    const T      *blockL     ,
    const T      *blockH     ,
          T      *dst        ,
    unsigned int  toBegBorder,
    unsigned int  toEndBorder)
{
    toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;
    toEndBorder = toEndBorder > 4 ? 4 : toEndBorder;

    T blockL_m_1 = blockL[ WT97L2INDEX_L_m_1[toEndBorder][toBegBorder] ];
    T blockL_p_1 = blockL[ WT97L2INDEX_L_p_1[toEndBorder][toBegBorder] ];
    T blockL_p_2 = blockL[ WT97L2INDEX_L_p_2[toEndBorder][toBegBorder] ];

    T blockH_m_2 = blockH[ WT97L2INDEX_H_m_2[toEndBorder][toBegBorder] ];
    T blockH_m_1 = blockH[ WT97L2INDEX_H_m_1[toEndBorder][toBegBorder] ];
    T blockH_p_1 = blockH[ WT97L2INDEX_H_p_1[toEndBorder][toBegBorder] ];
    T blockH_p_2 = blockH[ WT97L2INDEX_H_p_2[toEndBorder][toBegBorder] ];

    T tmp1, tmp2;

    WT97InvBegStepFirstL(
        blockL_m_1,
        blockL[ 0],
        blockL_p_1,
        blockH_m_2,
        blockH_m_1,
        blockH[ 0],
        blockH_p_1,
        tmp1,
        tmp1,
        dst[0],
        dst[1],
        tmp2);

    WT97InvInterStep(
        blockL_p_2,
        blockH_p_1,
        blockH_p_2,
        dst[0],
        dst[1],
        tmp2,
        tmp1,
        tmp1);
}

template<class T>
void WT97InvRowFirstL_3(
    const T      *blockL     ,
    const T      *blockH     ,
          T      *dst        ,
    unsigned int  toBegBorder,
    unsigned int  toEndBorder)
{
    toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;
    toEndBorder = toEndBorder > 3 ? 3 : toEndBorder;

    T blockL_m_1 = blockL[ WT97L3INDEX_L_m_1[toEndBorder][toBegBorder] ];
    T blockL_p_2 = blockL[ WT97L3INDEX_L_p_2[toEndBorder][toBegBorder] ];

    T blockH_m_2 = blockH[ WT97L3INDEX_H_m_2[toEndBorder][toBegBorder] ];
    T blockH_m_1 = blockH[ WT97L3INDEX_H_m_1[toEndBorder][toBegBorder] ];
    T blockH_p_1 = blockH[ WT97L3INDEX_H_p_1[toEndBorder][toBegBorder] ];
    T blockH_p_2 = blockH[ WT97L3INDEX_H_p_2[toEndBorder][toBegBorder] ];

    T tmp1, tmp2;

    WT97InvBegStepFirstL(
        blockL_m_1,
        blockL[ 0],
        blockL[ 1],
        blockH_m_2,
        blockH_m_1,
        blockH[ 0],
        blockH_p_1,
        tmp1,
        tmp2,
        dst[0],
        dst[1],
        dst[2]);

    WT97InvInterStep(
        blockL_p_2,
        blockH_p_1,
        blockH_p_2,
        dst[0],
        dst[1],
        dst[2],
        tmp1,
        tmp1);
}


template<class T>
void WT97InvRowFirstL_4(
    const T      *blockL     ,
    const T      *blockH     ,
          T      *dst        ,
    unsigned int  toBegBorder,
    unsigned int  toEndBorder)
{
    toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;
    toEndBorder = toEndBorder > 4 ? 4 : toEndBorder;

    T blockL_m_1 = blockL[ WT97L4INDEX_L_m_1[toEndBorder][toBegBorder] ];
    T blockL_p_2 = blockL[ WT97L4INDEX_L_p_2[toEndBorder][toBegBorder] ];
    T blockL_p_3 = blockL[ WT97L4INDEX_L_p_3[toEndBorder][toBegBorder] ];

    T blockH_m_2 = blockH[ WT97L4INDEX_H_m_2[toEndBorder][toBegBorder] ];
    T blockH_m_1 = blockH[ WT97L4INDEX_H_m_1[toEndBorder][toBegBorder] ];
    T blockH_p_2 = blockH[ WT97L4INDEX_H_p_2[toEndBorder][toBegBorder] ];
    T blockH_p_3 = blockH[ WT97L4INDEX_H_p_3[toEndBorder][toBegBorder] ];

    T tmp1, tmp2;

    WT97InvBegStepFirstL(
        blockL_m_1,
        blockL[ 0],
        blockL[ 1],
        blockH_m_2,
        blockH_m_1,
        blockH[ 0],
        blockH[ 1],
        tmp1,
        tmp2,
        dst[0],
        dst[1],
        dst[2]);

    WT97InvInterStep(
        blockL_p_2,
        blockH[ 1],
        blockH_p_2,
        dst[0],
        dst[1],
        dst[2],
        dst[3],
        tmp1);

    WT97InvInterStep(
        blockL_p_3,
        blockH_p_2,
        blockH_p_3,
        dst[2],
        dst[3],
        tmp1,
        tmp2,
        tmp2);
}

template<class T>
void WT97InvRowFirstL_5nMore(
    const T         *blockL     ,
    const T         *blockH     ,
          T         *dst        ,
    const UIntRange &blockRange ,
    unsigned int     toBegBorder,
    unsigned int     toEndBorder)
{

    static const int indexL_m_1[4] = { 1,  0, -1, -1};
    static const int indexH_m_1[4] = { 0, -1, -1, -1};
    static const int indexH_m_2[4] = { 1,  0, -1, -2};

    unsigned int srcSize = blockRange.Size() >> 1;

    T tmp1, tmp2;

    toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;

    T blockL_m_1 = blockL[ indexL_m_1[toBegBorder] ];
    T blockH_m_1 = blockH[ indexH_m_1[toBegBorder] ];
    T blockH_m_2 = blockH[ indexH_m_2[toBegBorder] ];

    WT97InvBegStepFirstL(
        blockL_m_1,
        blockL[ 0],
        blockL[ 1],
        blockH_m_2,
        blockH_m_1,
        blockH[ 0],
        blockH[ 1],
        tmp1,
        tmp2,
        dst[0],
        dst[1],
        dst[2]);


    for(unsigned int i = 1; i < srcSize - 1; i++)
        WT97InvInterStep(&blockL[i+1], &blockH[i], &dst[2*i-2]);

    //     WT97InvRow(&low[2], &high[2], (size >> 1) - 4, &dst [4], ippWTFilterFirstLow);
    // srcSize-1
    //2*srcSize-2 - 2 = 2*srcSize-4
    //2*(srcSize-1) - 1 - 2 + 4 = 2*srcSize - 1 - 2 - 2 + 4 = 2*srcSize - 1;

    toEndBorder += (blockRange.Size() & 1);
    toEndBorder = toEndBorder > 4 ? 4 : toEndBorder;


    const T *boundL = &blockL[srcSize - 1]; // actual bound is srcSize - 1 + blockRange.Size() & 1
    const T *boundH = &blockH[srcSize - 1];

    T boundH_p_0 = boundH[0];

    T boundL_p_1 = boundL[ WT97L5INDEX_L_p_1[toEndBorder] ];
    T boundL_p_2 = boundL[ WT97L5INDEX_L_p_2[toEndBorder] ];

    T boundH_p_1 = boundH[ WT97L5INDEX_H_p_1[toEndBorder] ];
    T boundH_p_2 = boundH[ WT97L5INDEX_H_p_2[toEndBorder] ];

    T *boundDst = &dst[srcSize * 2 - 1];
    T *pBoundDst_1;
    if(blockRange.Size() & 1)   pBoundDst_1 = &boundDst[1];
    else                        pBoundDst_1 = &tmp1;
    T &boundDst_1 = *pBoundDst_1;


    WT97InvInterStep(
        boundL_p_1,
        boundH_p_0,
        boundH_p_1,
        boundDst[-3],
        boundDst[-2],
        boundDst[-1],
        boundDst[ 0],
        boundDst_1);

    WT97InvInterStep(
        boundL_p_2,
        boundH_p_1,
        boundH_p_2,
        boundDst[-1],
        boundDst[ 0],
        boundDst_1,
        tmp2,
        tmp2);
}

template<class T>
void WT97InvRowFirstL_nMore(
    const T         *blockL     ,
    const T         *blockH     ,
          T         *dst        ,
    const UIntRange &blockRange ,
    unsigned int     toBegBorder,
    unsigned int     toEndBorder)
{

    static const int indexL_m_1[4] = { 1,  0, -1, -1};
    static const int indexH_m_1[4] = { 0, -1, -1, -1};
    static const int indexH_m_2[4] = { 1,  0, -1, -2};

    unsigned int srcSize = blockRange.Size() >> 1;

    T tmp1, tmp2;

    toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;

    T blockL_m_1 = blockL[ indexL_m_1[toBegBorder] ];
    T blockH_m_1 = blockH[ indexH_m_1[toBegBorder] ];
    T blockH_m_2 = blockH[ indexH_m_2[toBegBorder] ];

    WT97InvBegStepFirstL(
        blockL_m_1,
        blockL[ 0],
        blockL[ 1],
        blockH_m_2,
        blockH_m_1,
        blockH[ 0],
        blockH[ 1],
        tmp1,
        tmp2,
        dst[0],
        dst[1],
        dst[2]);


    WT97InvInterStep(&blockL[2], &blockH[1], &dst[0]);
    WT97InvInterStep(&blockL[3], &blockH[2], &dst[2]);


    //WT97InvInterStep(&blockL[srcSize - 3], &blockH[srcSize - 4], &dst[2*srcSize - 10]);
    LiftDELTA(blockL[srcSize - 3], blockH[srcSize - 4], blockH[srcSize - 3], dst[2*srcSize - 6]);
//    LiftGAMMA(blockH[srcSize - 4], dst[2*srcSize - 8], dst[2*srcSize - 6], dst[2*srcSize - 7]);
//    LiftBETA (dst[2*srcSize - 9], dst[2*srcSize - 7], dst[2*srcSize - 8]);
//    LiftALPHA(dst[2*srcSize - 10], dst[2*srcSize - 8], dst[2*srcSize - 9]);


    WT97InvInterStep(&blockL[srcSize - 2], &blockH[srcSize - 3], &dst[2*srcSize -  8]);
    WT97InvInterStep(&blockL[srcSize - 1], &blockH[srcSize - 2], &dst[2*srcSize -  6]);

    WT97InvRow(&blockL[2], &blockH[2], srcSize - 4, &dst [4], ippWTFilterFirstLow);




    // srcSize-1
    //2*srcSize-2 - 2 = 2*srcSize-4
    //2*(srcSize-1) - 1 - 2 + 4 = 2*srcSize - 1 - 2 - 2 + 4 = 2*srcSize - 1;

    toEndBorder += (blockRange.Size() & 1);
    toEndBorder = toEndBorder > 4 ? 4 : toEndBorder;


    const T *boundL = &blockL[srcSize - 1]; // actual bound is srcSize - 1 + blockRange.Size() & 1
    const T *boundH = &blockH[srcSize - 1];

    T boundH_p_0 = boundH[0];

    T boundL_p_1 = boundL[ WT97L5INDEX_L_p_1[toEndBorder] ];
    T boundL_p_2 = boundL[ WT97L5INDEX_L_p_2[toEndBorder] ];

    T boundH_p_1 = boundH[ WT97L5INDEX_H_p_1[toEndBorder] ];
    T boundH_p_2 = boundH[ WT97L5INDEX_H_p_2[toEndBorder] ];

    T *boundDst = &dst[srcSize * 2 - 1];
    T *pBoundDst_1;
    if(blockRange.Size() & 1)   pBoundDst_1 = &boundDst[1];
    else                        pBoundDst_1 = &tmp1;
    T &boundDst_1 = *pBoundDst_1;


    WT97InvInterStep(
        boundL_p_1,
        boundH_p_0,
        boundH_p_1,
        boundDst[-3],
        boundDst[-2],
        boundDst[-1],
        boundDst[ 0],
        boundDst_1);

    WT97InvInterStep(
        boundL_p_2,
        boundH_p_1,
        boundH_p_2,
        boundDst[-1],
        boundDst[ 0],
        boundDst_1,
        tmp2,
        tmp2);
}

template<class T>
void WT97InvRowFirstL(
    const T         *blockL     ,
    const T         *blockH     ,
          T         *dst        ,
    const UIntRange &blockRange ,
    unsigned int     toBegBorder,
    unsigned int     toEndBorder)
{
    switch(blockRange.Size())
    {
    case 0:
        break;
    case 1:
        WT97InvRowFirstL_1(
            blockL     ,
            blockH     ,
            dst        ,
            toBegBorder,
            toEndBorder);
        break;
    case 2:
        WT97InvRowFirstL_2(
            blockL     ,
            blockH     ,
            dst        ,
            toBegBorder,
            toEndBorder);
        break;
    case 3:
        WT97InvRowFirstL_3(
            blockL     ,
            blockH     ,
            dst        ,
            toBegBorder,
            toEndBorder);
        break;
    case 4:
        WT97InvRowFirstL_4(
            blockL     ,
            blockH     ,
            dst        ,
            toBegBorder,
            toEndBorder);
        break;
    default:
        if(blockRange.Size() < 14)
            WT97InvRowFirstL_5nMore(
                blockL     ,
                blockH     ,
                dst        ,
                blockRange ,
                toBegBorder,
                toEndBorder);
        else
            WT97InvRowFirstL_nMore(
                blockL     ,
                blockH     ,
                dst        ,
                blockRange ,
                toBegBorder,
                toEndBorder);

        break;
    }

    return;
}


#endif // __WT97INVROWGENLMT_H__
