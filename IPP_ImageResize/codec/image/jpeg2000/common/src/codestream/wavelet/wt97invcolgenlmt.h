
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

#ifndef __WT97INVCOLGENLMT_H__
#define __WT97INVCOLGENLMT_H__


#include "trackedrowinput.h"

#include "wt97invcolgenbasemt.h"
#include "wt97invborderindexlmt.h"

template<class T, class RowInput>
class BWT97InvColPipelineBegStepFirstL_MT : public BWT97InvColPipelineMT<T, RowInput>
{
protected:
    BWT97InvColPipelineBegStepFirstL_MT() {}
    virtual ~BWT97InvColPipelineBegStepFirstL_MT() {}

    void StepBeg()
    {
        WT97InvBegStepFirstL<Ipp32f>(
            m_blockL_m_1,
            m_blockL_p_0,
            m_blockL_p_1,
            m_blockH_m_2,
            m_blockH_m_1,
            m_blockH_p_0,
            m_blockH_p_1,

            this->m_stripe.DelayedRow(4), // aka tmp
            this->m_stripe.DelayedRow(3), // aka tmp

            this->m_stripe.DelayedRow(2),
            this->m_stripe.DelayedRow(1),
            this->m_stripe.DelayedRow(0),
            this->m_width);
    }

    const T *m_blockL_m_1;
    const T *m_blockL_p_0;
    const T *m_blockL_p_1;

    const T *m_blockH_m_2;
    const T *m_blockH_m_1;
    const T *m_blockH_p_0;
    const T *m_blockH_p_1;
};

template<class T, class RowInput>
class BWT97InvColPipelineInterSizeFirstL_MT : public BWT97InvColPipelineBegStepFirstL_MT<T, RowInput>
{
protected:
    BWT97InvColPipelineInterSizeFirstL_MT() {}
    virtual ~BWT97InvColPipelineInterSizeFirstL_MT() {}

    void StepEnd1() { RegularStep(      m_boundL_p_1, m_boundH_p_0,       m_boundH_p_1); }
    void StepEnd2() { RegularStep(this->m_boundL_p_2, m_boundH_p_1, this->m_boundH_p_2); }

    const T *m_boundL_p_1;

    const T *m_boundH_p_0;
    const T *m_boundH_p_1;
};

template<class T, class RowInput>
class BWT97InvColPipelineFirstL_MT : public BWT97InvColPipelineInterSizeFirstL_MT<T, RowInput>
{
protected:
    BWT97InvColPipelineFirstL_MT() {}
    virtual ~BWT97InvColPipelineFirstL_MT() {}

    void StepEnd2() { RegularStep(m_boundL_p_2, this->m_boundH_p_1, m_boundH_p_2); }

    const T *m_boundL_p_2;

    const T *m_boundH_p_2;
};

template<class T, class RowInput>
class WT97InvColPipelineFirstL_1_MT : public BWT97InvColPipelineBegStepFirstL_MT<T, RowInput>
{
public:
    WT97InvColPipelineFirstL_1_MT() {}
    virtual ~WT97InvColPipelineFirstL_1_MT() {}

    void ReAlloc(unsigned int maxWidth)
    {
        BWT97InvColPipelineBegStepFirstL_MT<T, RowInput>::ReAlloc(maxWidth);
        m_buffer.ReAlloc(maxWidth);
    }

    void InitAttach(
        const Rect   &rect,
        unsigned int  toBegBorder,
        unsigned int  toEndBorder,
        RowInput     &srcL, // pointing to border
        RowInput     &srcH) // pointing to border
    {

        BWT97InvColPipelineBegStepFirstL_MT<T, RowInput>::InitAttach(
            rect,
            toBegBorder,
            toEndBorder,
            srcL,
            srcH);

        m_buffer.InitAttach(
            rect.RangeY(),
            toBegBorder,
            toEndBorder,
            srcL,
            srcH);

        if(toBegBorder+toEndBorder == 0)
        {
            m_isCopy = true;
            return;
        }
        m_isCopy = false;

        toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;
        toEndBorder = toEndBorder > 3 ? 3 : toEndBorder;

        this->m_blockL_m_1 = m_buffer.L( WT97L1INDEX_L_m_1[toEndBorder][toBegBorder] );
        this->m_blockL_p_0 = m_buffer.L( 0 );
        this->m_blockL_p_1 = m_buffer.L( WT97L1INDEX_L_p_1[toEndBorder][toBegBorder] );

        this->m_blockH_m_2 = m_buffer.H( WT97L1INDEX_H_m_2[toEndBorder][toBegBorder] );
        this->m_blockH_m_1 = m_buffer.H( WT97L1INDEX_H_m_1[toEndBorder][toBegBorder] );
        this->m_blockH_p_0 = m_buffer.H( WT97L1INDEX_H_p_0[toEndBorder][toBegBorder] );
        this->m_blockH_p_1 = m_buffer.H( WT97L1INDEX_H_p_1[toEndBorder][toBegBorder] );
    }

    bool NextStep()
    {
        if(this->m_dstCounter >= 1)
            return false;

        m_buffer.Load();

        if(m_isCopy)
            Copy(m_buffer.L(0), this->m_stripe.DelayedRow(2), this->m_width);
        else
            this->StepBeg();

        ++this->m_dstCounter;
        return true;
    }

protected:
    WT97InvColSmallHeightBuffer<T, RowInput> m_buffer;
    bool                                     m_isCopy;
};

template<class T, class RowInput>
class WT97InvColPipelineFirstL_2_MT : public BWT97InvColPipelineInterSizeFirstL_MT<T, RowInput>
{
public:
    WT97InvColPipelineFirstL_2_MT() {}
    virtual ~WT97InvColPipelineFirstL_2_MT() {}

    void ReAlloc(unsigned int maxWidth)
    {
        BWT97InvColPipelineInterSizeFirstL_MT<T, RowInput>::ReAlloc(maxWidth);
        m_buffer.ReAlloc(maxWidth);
    }

    void InitAttach(
        const Rect   &rect,
        unsigned int  toBegBorder,
        unsigned int  toEndBorder,
        RowInput     &srcL, // pointing to border
        RowInput     &srcH) // pointing to border
    {
        BWT97InvColPipelineInterSizeFirstL_MT<T, RowInput>::InitAttach(
            rect,
            toBegBorder,
            toEndBorder,
            srcL,
            srcH);

        m_buffer.InitAttach(
            rect.RangeY(),
            toBegBorder,
            toEndBorder,
            srcL,
            srcH);

        toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;
        toEndBorder = toEndBorder > 4 ? 4 : toEndBorder;

        this->m_blockL_m_1 = m_buffer.L( WT97L2INDEX_L_m_1[toEndBorder][toBegBorder] );
        this->m_blockL_p_0 = m_buffer.L( 0 );
        this->m_blockL_p_1 = m_buffer.L( WT97L2INDEX_L_p_1[toEndBorder][toBegBorder] );

        this->m_boundL_p_1 = m_buffer.L( WT97L2INDEX_L_p_2[toEndBorder][toBegBorder] ); // m_blockL_p_2

        this->m_blockH_m_2 = m_buffer.H( WT97L2INDEX_H_m_2[toEndBorder][toBegBorder] );
        this->m_blockH_m_1 = m_buffer.H( WT97L2INDEX_H_m_1[toEndBorder][toBegBorder] );
        this->m_blockH_p_0 = m_buffer.H( 0 );
        this->m_blockH_p_1 = m_buffer.H( WT97L2INDEX_H_p_1[toEndBorder][toBegBorder] );

        this->m_boundH_p_0 = this->m_blockH_p_1;
        this->m_boundH_p_1 = m_buffer.H( WT97L2INDEX_H_p_2[toEndBorder][toBegBorder] ); // m_blockH_p_2
    }

    bool NextStep()
    {
        if(this->m_dstCounter >= 2)
            return false;

        if(this->m_dstCounter == 0)
        {
            m_buffer.Load();
            this->StepBeg();
            ++this->m_dstCounter;
            return true;
        }

        this->StepEnd1();

        ++this->m_dstCounter;
        ++this->m_stripe;

        return true;
    }

protected:
    WT97InvColSmallHeightBuffer<T, RowInput> m_buffer;
};

template<class T, class RowInput>
class WT97InvColPipelineFirstL_3_MT : public BWT97InvColPipelineInterSizeFirstL_MT<T, RowInput>
{
public:
    WT97InvColPipelineFirstL_3_MT() {}
    virtual ~WT97InvColPipelineFirstL_3_MT() {}

    void ReAlloc(unsigned int maxWidth)
    {
        BWT97InvColPipelineInterSizeFirstL_MT<T, RowInput>::ReAlloc(maxWidth);
        m_buffer.ReAlloc(maxWidth);
    }

    void InitAttach(
        const Rect   &rect,
        unsigned int  toBegBorder,
        unsigned int  toEndBorder,
        RowInput     &srcL, // pointing to border
        RowInput     &srcH) // pointing to border
    {
        BWT97InvColPipelineInterSizeFirstL_MT<T, RowInput>::InitAttach(
            rect,
            toBegBorder,
            toEndBorder,
            srcL,
            srcH);

        m_buffer.InitAttach(
            rect.RangeY(),
            toBegBorder,
            toEndBorder,
            srcL,
            srcH);

        toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;
        toEndBorder = toEndBorder > 3 ? 3 : toEndBorder;

        this->m_blockL_m_1 = m_buffer.L( WT97L3INDEX_L_m_1[toEndBorder][toBegBorder] );
        this->m_blockL_p_0 = m_buffer.L( 0 );
        this->m_blockL_p_1 = m_buffer.L( 1 );

        this->m_boundL_p_1 = m_buffer.L( WT97L3INDEX_L_p_2[toEndBorder][toBegBorder] ); // m_blockL_p_2

        this->m_blockH_m_2 = m_buffer.H( WT97L3INDEX_H_m_2[toEndBorder][toBegBorder] );
        this->m_blockH_m_1 = m_buffer.H( WT97L3INDEX_H_m_1[toEndBorder][toBegBorder] );
        this->m_blockH_p_0 = m_buffer.H( 0 );
        this->m_blockH_p_1 = m_buffer.H( WT97L3INDEX_H_p_1[toEndBorder][toBegBorder] );

        this->m_boundH_p_0 = this->m_blockH_p_1;
        this->m_boundH_p_1 = m_buffer.H( WT97L3INDEX_H_p_2[toEndBorder][toBegBorder] ); // m_blockH_p_2
    }

    bool NextStep()
    {
        if(this->m_dstCounter >= 3)
            return false;

        if(this->m_dstCounter == 0)
        {
            m_buffer.Load();
            this->StepBeg();
            ++this->m_dstCounter;
            return true;
        }

        if(this->m_dstCounter == 1)
            this->StepEnd1();

        ++this->m_dstCounter;
        ++this->m_stripe;

        return true;
    }

protected:
    WT97InvColSmallHeightBuffer<T, RowInput> m_buffer;
};

template<class T, class RowInput>
class WT97InvColPipelineFirstL_4_MT : public BWT97InvColPipelineFirstL_MT<T, RowInput>
{
public:
    WT97InvColPipelineFirstL_4_MT() {}
    virtual ~WT97InvColPipelineFirstL_4_MT() {}

    void ReAlloc(unsigned int maxWidth)
    {
        BWT97InvColPipelineFirstL_MT<T, RowInput>::ReAlloc(maxWidth);
        m_buffer.ReAlloc(maxWidth);
    }

    void InitAttach(
        const Rect   &rect,
        unsigned int  toBegBorder,
        unsigned int  toEndBorder,
        RowInput     &srcL, // pointing to border
        RowInput     &srcH) // pointing to border
    {
        BWT97InvColPipelineFirstL_MT<T, RowInput>::InitAttach(
            rect,
            toBegBorder,
            toEndBorder,
            srcL,
            srcH);

        m_buffer.InitAttach(
            rect.RangeY(),
            toBegBorder,
            toEndBorder,
            srcL,
            srcH);

        toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;
        toEndBorder = toEndBorder > 4 ? 4 : toEndBorder;

        this->m_blockL_m_1 = m_buffer.L( WT97L4INDEX_L_m_1[toEndBorder][toBegBorder] );
        this->m_blockL_p_0 = m_buffer.L( 0 );
        this->m_blockL_p_1 = m_buffer.L( 1 );

        this->m_boundL_p_1 = m_buffer.L( WT97L4INDEX_L_p_2[toEndBorder][toBegBorder] ); // m_blockL_p_2
        this->m_boundL_p_2 = m_buffer.L( WT97L4INDEX_L_p_3[toEndBorder][toBegBorder] ); // m_blockL_p_3

        this->m_blockH_m_2 = m_buffer.H( WT97L4INDEX_H_m_2[toEndBorder][toBegBorder] );
        this->m_blockH_m_1 = m_buffer.H( WT97L4INDEX_H_m_1[toEndBorder][toBegBorder] );
        this->m_blockH_p_0 = m_buffer.H( 0 );
        this->m_blockH_p_1 = m_buffer.H( 1 );

        this->m_boundH_p_0 = this->m_blockH_p_1;
        this->m_boundH_p_1 = m_buffer.H( WT97L4INDEX_H_p_2[toEndBorder][toBegBorder] ); // m_blockH_p_2
        this->m_boundH_p_2 = m_buffer.H( WT97L4INDEX_H_p_3[toEndBorder][toBegBorder] ); // m_blockH_p_3
    }

    bool NextStep()
    {
        if(this->m_dstCounter >= 4)
            return false;

        if(this->m_dstCounter == 0)
        {
            m_buffer.Load();
            this->StepBeg();
            ++this->m_dstCounter;
            return true;
        }

        if(this->m_dstCounter == 1)
            this->StepEnd1();
        else if(this->m_dstCounter == 3)
            this->StepEnd2();

        ++this->m_dstCounter;
        ++this->m_stripe;

        return true;
    }

protected:
    WT97InvColSmallHeightBuffer<T, RowInput> m_buffer;
};

template<class T, class RowInput>
class WT97InvColPipelineFirstL_5nMore_MT : public BWT97InvColPipelineFirstL_MT<T, RowInput>
{
public:
    WT97InvColPipelineFirstL_5nMore_MT() {}
    virtual ~WT97InvColPipelineFirstL_5nMore_MT() {}

    void ReAlloc(unsigned int maxWidth)
    {
        BWT97InvColPipelineFirstL_MT<T, RowInput>::ReAlloc(maxWidth);

        m_rowL      .ReAlloc(maxWidth, 1); // +1 (additional point) in delay line only for easier implementation of end border processing
        m_rowH      .ReAlloc(maxWidth, 2); // +1 (additional point) in delay line only for easier implementation of end border processing

        m_begBorderL.ReAlloc(WT97MaxBegOverstepL + 2, maxWidth);
        m_begBorderH.ReAlloc(WT97MaxBegOverstepH + 2, maxWidth);

        m_endBorderL.ReAlloc(WT97MaxEndOverstepL + 3, maxWidth);
        m_endBorderH.ReAlloc(WT97MaxEndOverstepH + 3, maxWidth);
    }

    void InitAttach(
        const Rect   &rect,
        unsigned int  toBegBorder,
        unsigned int  toEndBorder,
        RowInput     &srcL, // pointing to border
        RowInput     &srcH) // pointing to border
    {
        BWT97InvColPipelineFirstL_MT<T, RowInput>::InitAttach(
            rect,
            toBegBorder,
            toEndBorder,
            srcL,
            srcH);

        UIntRange blockRange = rect.RangeY();

        m_height             = rect.Height();
        m_interStepBound     = blockRange.Size() - (blockRange.Size() & 1) - 3;

        m_rowL.Attach(srcL);
        m_rowH.Attach(srcH);

        int begOverstepL = WT97BegOverstepL(blockRange, toBegBorder, toEndBorder);
        int begOverstepH = WT97BegOverstepH(blockRange, toBegBorder, toEndBorder);

        int endOverstepL = WT97EndOverstepL(blockRange, toBegBorder, toEndBorder);
        int endOverstepH = WT97EndOverstepH(blockRange, toBegBorder, toEndBorder);

        m_begBorderSizeL = begOverstepL + 2;
        m_begBorderSizeH = begOverstepH + 2;

        m_endBorderSizeL = endOverstepL + 2 + (blockRange.Size() & 1);
        m_endBorderSizeH = endOverstepH + 3;

        toBegBorder = toBegBorder > 3 ? 3 : toBegBorder;

        this->m_blockL_m_1 = m_begBorderL[ begOverstepL + WT97L5INDEX_L_m_1[toBegBorder] ];
        this->m_blockL_p_0 = m_begBorderL[ begOverstepL + 0 ];
        this->m_blockL_p_1 = m_begBorderL[ begOverstepL + 1 ];

        this->m_blockH_m_2 = m_begBorderH[ begOverstepH + WT97L5INDEX_H_m_2[toBegBorder] ];
        this->m_blockH_m_1 = m_begBorderH[ begOverstepH + WT97L5INDEX_H_m_1[toBegBorder] ];
        this->m_blockH_p_0 = m_begBorderH[ begOverstepH + 0 ];
        this->m_blockH_p_1 = m_begBorderH[ begOverstepH + 1 ];


        toEndBorder += (blockRange.Size() & 1);
        toEndBorder = toEndBorder > 4 ? 4 : toEndBorder;

        this->m_boundL_p_1 = m_endBorderL[ 1 + WT97L5INDEX_L_p_1[toEndBorder] ];
        this->m_boundL_p_2 = m_endBorderL[ 1 + WT97L5INDEX_L_p_2[toEndBorder] ];

        this->m_boundH_p_0 = m_endBorderH[ 2 + 0];
        this->m_boundH_p_1 = m_endBorderH[ 2 + WT97L5INDEX_H_p_1[toEndBorder] ];
        this->m_boundH_p_2 = m_endBorderH[ 2 + WT97L5INDEX_H_p_2[toEndBorder] ];
    }

    bool NextStep()
    {
        if(this->m_dstCounter >= m_height)
            return false;

        if(this->m_dstCounter == 0)
        {
            LoadBegBorder();
            this->StepBeg();
            ++this->m_dstCounter;
            return true;
        }

        if(this->m_dstCounter < m_interStepBound)
        {
            if(this->m_dstCounter & 1)
            {
                ++m_rowL;
                ++m_rowH;
                StepInter();
            }
        }
        else if(this->m_dstCounter == m_interStepBound)
        {
            LoadEndBorder();
            this->StepEnd1();
        }
        else if(this->m_dstCounter == m_interStepBound + 2)
            this->StepEnd2();

        ++this->m_dstCounter;
        ++this->m_stripe;

        return true;
    }

protected:
    void LoadBegBorder()
    {
        unsigned int k;

        for(k = 0; k < m_begBorderSizeL; k++)
        {
            ++m_rowL;
            Copy(m_rowL.DelayedRow(0), m_begBorderL[k], this->m_width);

        }
        for(k = 0; k < m_begBorderSizeH; k++)
        {
            ++m_rowH;
            Copy(m_rowH.DelayedRow(0), m_begBorderH[k], this->m_width);

        }
    }

    void LoadEndBorder()
    {
        unsigned int k;

        Copy(m_rowL.DelayedRow(1), m_endBorderL[0], this->m_width);
        Copy(m_rowL.DelayedRow(0), m_endBorderL[1], this->m_width);
        for(k = 2; k < m_endBorderSizeL; k++)
        {
            ++m_rowL;
            Copy(m_rowL.DelayedRow(0), m_endBorderL[k], this->m_width);
        }

        Copy(m_rowH.DelayedRow(2), m_endBorderH[0], this->m_width);
        Copy(m_rowH.DelayedRow(1), m_endBorderH[1], this->m_width);
        Copy(m_rowH.DelayedRow(0), m_endBorderH[2], this->m_width);
        for(k = 3; k < m_endBorderSizeH; k++)
        {
            ++m_rowH;
            Copy(m_rowH.DelayedRow(0), m_endBorderH[k], this->m_width);

        }
    }

    void StepInter()
    {
        RegularStep(
            m_rowL.DelayedRow(0),

            m_rowH.DelayedRow(1),
            m_rowH.DelayedRow(0));
    }

    unsigned int                 m_height;
    unsigned int                 m_interStepBound;

    TrackedRowInput<T, RowInput> m_rowL;
    TrackedRowInput<T, RowInput> m_rowH;

    FixedBuffer2D  <T>           m_begBorderL;
    FixedBuffer2D  <T>           m_begBorderH;

    FixedBuffer2D  <T>           m_endBorderL;
    FixedBuffer2D  <T>           m_endBorderH;

    unsigned int                 m_begBorderSizeL;
    unsigned int                 m_begBorderSizeH;

    unsigned int                 m_endBorderSizeL;
    unsigned int                 m_endBorderSizeH;
};


#endif // __WT97INVCOLGENLMT_H__

