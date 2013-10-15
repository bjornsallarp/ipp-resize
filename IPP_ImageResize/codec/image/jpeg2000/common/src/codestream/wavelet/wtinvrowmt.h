
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

#ifndef __WTINVROWMT_H__
#define __WTINVROWMT_H__

// #include "wt53invrowgenmt.h"
#include "wt97invrowgenmt.h"
#include "fixedbuffer.h"
#include "wtmetricmt.h"

/*
template<class T, class RowInput> class WT53InvRowAgregateMT
{
public:
    typedef T AGR_T;
    typedef RowInput AGR_RowInput;
    static void InvRow(
        const T         *blockL     ,
        const T         *blockH     ,
              T         *dst        ,
        const UIntRange &blockRange ,
        unsigned int     toBegBorder,
        unsigned int     toEndBorder)
    {
        WT53InvRow(
            blockL     ,
            blockH     ,
            dst        ,
            blockRange ,
            toBegBorder,
            toEndBorder)
    }
};
*/

template<class T, class RowInput> class WT97InvRowAgregateMT
{
public:
    typedef T AGR_T;
    typedef RowInput AGR_RowInput;
    static void InvRow(
        const T         *blockL     ,
        const T         *blockH     ,
              T         *dst        ,
        const UIntRange &blockRange ,
        unsigned int     toBegBorder,
        unsigned int     toEndBorder)
    {
        WT97InvRow(
            blockL     ,
            blockH     ,
            dst        ,
            blockRange ,
            toBegBorder,
            toEndBorder);
    }
    static int BegOverstepL(
        const UIntRange &blockRange ,
        unsigned int     toBegBorder,
        unsigned int     toEndBorder)
    {
        return WT97BegOverstepL(blockRange, toBegBorder, toEndBorder);
    }

    static int BegOverstepH(
        const UIntRange &blockRange ,
        unsigned int     toBegBorder,
        unsigned int     toEndBorder)
    {
        return WT97BegOverstepH(blockRange, toBegBorder, toEndBorder);
    }

    static int EndOverstepL(
        const UIntRange &blockRange ,
        unsigned int     toBegBorder,
        unsigned int     toEndBorder)
    {
        return WT97EndOverstepL(blockRange, toBegBorder, toEndBorder);
    }

    static int EndOverstepH(
        const UIntRange &blockRange ,
        unsigned int     toBegBorder,
        unsigned int     toEndBorder)
    {
        return WT97EndOverstepH(blockRange, toBegBorder, toEndBorder);
    }
    static int MaxBegOverstepL() { return WT97MaxBegOverstepL; }
    static int MaxEndOverstepL() { return WT97MaxEndOverstepL; }
    static int MaxBegOverstepH() { return WT97MaxBegOverstepH; }
    static int MaxEndOverstepH() { return WT97MaxEndOverstepH; }
};

template <class RowAgregate>
class WTInvRowMT
{
public:
    typedef typename RowAgregate::AGR_T T;
    typedef typename RowAgregate::AGR_RowInput RowInput;

    WTInvRowMT() {}

    void ReAlloc(unsigned int maxSize)
    {
        unsigned int maxCmpSize = (maxSize >> 1) + 2;
        m_buffL.ReAlloc(maxSize + RowAgregate::MaxBegOverstepL() + RowAgregate::MaxEndOverstepL());
        m_buffH.ReAlloc(maxSize + RowAgregate::MaxBegOverstepH() + RowAgregate::MaxEndOverstepH());
    }

    void InitAttach (
        const UIntRange &range ,
        unsigned int     toBegBorder,
        unsigned int     toEndBorder,
        RowInput        &srcL,
        RowInput        &srcH)
    {
        m_range       = range;
        m_toBegBorder = toBegBorder;
        m_toEndBorder = toEndBorder;

        m_begOverstepL = RowAgregate::BegOverstepL(range, toBegBorder, toEndBorder);
        m_begOverstepH = RowAgregate::BegOverstepH(range, toBegBorder, toEndBorder);

        m_srcL = &srcL;
        m_srcH = &srcH;
   }

    void Read(T *row)
    {
        m_srcL->Read(m_buffL);
        m_srcH->Read(m_buffH);
        RowAgregate::InvRow(&(m_buffL[m_begOverstepL]), &(m_buffH[m_begOverstepH]), row, m_range, m_toBegBorder, m_toEndBorder);
    }

protected:
    UIntRange       m_range;

    RowInput       *m_srcL;
    RowInput       *m_srcH;

    FixedBuffer<T>  m_buffL;
    FixedBuffer<T>  m_buffH;

    unsigned int    m_toBegBorder;
    unsigned int    m_toEndBorder;
    unsigned int    m_begOverstepL;
    unsigned int    m_begOverstepH;
};

#endif // __WTINVROWMT_H__

