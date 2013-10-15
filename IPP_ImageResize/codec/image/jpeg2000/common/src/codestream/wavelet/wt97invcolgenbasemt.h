
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

#ifndef __WT97INVCOLGENBASEMT_H__
#define __WT97INVCOLGENBASEMT_H__

#include "ringstripe.h"
#include "wtmetricmt.h"

template<class T>
void WT97InvBegStepFirstL(
    const T     *l_0,
    const T     *l_1,
    const T     *l_2,
    const T     *h_0,
    const T     *h_1,
    const T     *h_2,
    const T     *h_3,
          T     *dst_0,
          T     *dst_1,
          T     *dst_2,
          T     *dst_3,
          T     *dst_4,
    unsigned int width)
{
    LiftDELTA(l_0, h_0, h_1, dst_0, width);

    LiftDELTA(l_1, h_1, h_2, dst_2, width);
    LiftGAMMA(h_1, dst_0, dst_2, dst_1, width);

    LiftDELTA(l_2, h_2, h_3, dst_4, width);
    LiftGAMMA(h_2, dst_2, dst_4, dst_3, width);
    LiftBETA (dst_1, dst_3, dst_2, width);
}

template<class T, class RowInput>
class BWT97InvColPipelineMT
{
public:
    virtual ~BWT97InvColPipelineMT() {}

    virtual void ReAlloc(unsigned int maxWidth)
    {
        m_stripe    .ReAlloc(maxWidth, 5);
    }

    virtual void InitAttach(
        const Rect   &rect,
        unsigned int  toBegBorder,
        unsigned int  toEndBorder,
        RowInput     &srcL, // pointing to border
        RowInput     &srcH  // pointing to border
        )
    {
        m_width      = rect.Width();
        m_dstCounter = 0;
    }

    virtual bool NextStep() = 0;

    const T* CurrOutputRow() const { return m_stripe.DelayedRow(2); }

protected:
    BWT97InvColPipelineMT() {}

    void RegularStep(
        const T *l_0,
        const T *h_0,
        const T *h_1)
    {
        WT97InvColLift(
            l_0,
            h_0,
            h_1,
            m_stripe.DelayedRow(2),

            m_stripe.DelayedRow(1),
            m_stripe.DelayedRow(0),

            m_stripe.DelayedRow(4),
            m_stripe.DelayedRow(3),
            m_width);
    }

    RingStripe<T> m_stripe;
    unsigned int  m_width;
    unsigned int  m_dstCounter;
};

template<class T, class RowInput>
class WT97InvColSmallHeightBuffer
{
public:
    WT97InvColSmallHeightBuffer() {}

    void ReAlloc(unsigned int maxWidth)
    {
        m_borderL.ReAlloc(WT97MaxBegOverstepL + WT97MaxEndOverstepL + 2, maxWidth);
        m_borderH.ReAlloc(WT97MaxBegOverstepH + WT97MaxEndOverstepH + 2, maxWidth);
    }

    void InitAttach(
        const UIntRange &blockRange,
        unsigned int     toBegBorder,
        unsigned int     toEndBorder,
        RowInput        &srcL, // pointing to border
        RowInput        &srcH) // pointing to border
    {
        m_rowL = &srcL;
        m_rowH = &srcH;

        m_begOverstepL = WT97BegOverstepL(blockRange, toBegBorder, toEndBorder);
        m_begOverstepH = WT97BegOverstepH(blockRange, toBegBorder, toEndBorder);

        int endOverstepL = WT97EndOverstepL(blockRange, toBegBorder, toEndBorder);
        int endOverstepH = WT97EndOverstepH(blockRange, toBegBorder, toEndBorder);

        m_borderSizeL = m_begOverstepL + endOverstepL + ScaleL(blockRange).Size();
        m_borderSizeH = m_begOverstepH + endOverstepH + ScaleH(blockRange).Size();
    }

    const T *L(unsigned int index) { return m_borderL[ m_begOverstepL + index ]; }
    const T *H(unsigned int index) { return m_borderH[ m_begOverstepH + index ]; }

    void Load()
    {
        unsigned int k;

        for(k = 0; k < m_borderSizeL; k++)
            m_rowL->Read(m_borderL[k]);

        for(k = 0; k < m_borderSizeH; k++)
            m_rowH->Read(m_borderH[k]);
    }

protected:
    RowInput         *m_rowL;
    RowInput         *m_rowH;

    FixedBuffer2D<T>  m_borderL;
    FixedBuffer2D<T>  m_borderH;

    unsigned int      m_borderSizeL;
    unsigned int      m_borderSizeH;

    unsigned int      m_begOverstepL;
    unsigned int      m_begOverstepH;
};

#endif // __WT97INVCOLGENBASEMT_H__
