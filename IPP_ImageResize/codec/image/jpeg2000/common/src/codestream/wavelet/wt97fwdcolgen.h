
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

#ifndef __WT97FWDCOLGEN_H__
#define __WT97FWDCOLGEN_H__

#include "pp.h"
#include "trackedrowoutput.h"

template<class T>
class BWT97FwdColPipeline
{
public:
    virtual ~BWT97FwdColPipeline() {}

    void ReAlloc(unsigned int maxWidth) { m_stripe.ReAlloc(maxWidth, 3); }

    void InitAttach(const Rect &rect)
    {
        m_width           = rect.Width();
        m_height          = rect.Height();
        m_heightOrgParity = rect.Y() & 1;

        m_row             = 0;
        m_i               = 0;
        m_iend            = ( (m_height - 1 + m_heightOrgParity) >> 1 ) - 2;
        m_isFullStep      = true;
        m_isLastStepFull  = m_heightOrgParity ? (m_height & 1) : ( !(m_height & 1) );
    }


    T* CurrInputRow() { return m_stripe.CurrRow(); }

    bool NextStep()
    {
        switch(m_height)
        {
        case 1:  return StepForHeight1();
        case 2:  return StepForHeight2();
        case 3:  return StepForHeight3();
        case 4:  return StepForHeight4();
        default: return StepForHeight5nMore();
        }
    }

protected:
    BWT97FwdColPipeline() : m_width(0) {}

    bool StepForHeight1()
    {
        m_row++;

        if(m_heightOrgParity)  {  ProcessOdd_1();                               NextH(); }
        else                   {  Copy <T*>(DlyRowSrc(0), DlyRowL(1), m_width); NextL(); }

        return false;
    }

    bool StepForHeight2()
    {
        m_row++;

        if(m_row <= 1) { NextSrc(); return true; }

        if(m_heightOrgParity) ProcessOdd_2 ();
        else                  ProcessEven_2();

        NextSrc();

        NextL();
        NextH();

        return false;
    }

    bool StepForHeight3()
    {
        m_row++;

        if(m_row <= 2) { NextSrc(); return true; }

        if(m_heightOrgParity) ProcessOdd_3();
        else
        {
            ProcessEven_3();
            NextL();
        }

        NextL();
        NextH();

        NextSrc();
        return false;
    }

    bool StepForHeight4()
    {
        if(m_heightOrgParity) return StepForHeight5nMore();

        m_row++;

        if(m_row <= 2) { NextSrc(); return true; }

        if(m_row == 3)
        {
            if(m_heightOrgParity) LiftALPHAx2(1, 2, 2);
            else
            {
                LiftALPHA (0, 2, 1, 2);
                LiftBETAx2(2, 2, 1);
            }
            NextSrc();
            return true;
        }

        if(!m_heightOrgParity)
        {
            LiftALPHAx2(1, 0, 1);
            LiftBETA   (1, 2, 1, 0);
            LiftGAMMAx2(0, 1);
            LiftGAMMA(0, 1, 2);
            LiftDELTA(1, 2, 0);
            LiftDELTAx2(2, 1);

            LiftKL(1);
            LiftKL(0);
            LiftKH(2);
            LiftKH(1);

            NextL();
            NextL();
            NextH();
            NextH();
            NextSrc();
        }

        return false;
    }

    bool StepForHeight5nMore()
    {
        m_row++;

        if(m_row <= 2) { NextSrc(); return true; }

        if(m_row == 3)
        {
            if(m_heightOrgParity) LiftALPHAx2(1, 2, 2);
            else
            {
                LiftALPHA (0, 2, 1, 2);
                LiftBETAx2(2, 2, 1);
            }

            NextSrc();
            return true;
        }

        if(m_row == 4)
        {
            if(m_heightOrgParity)
            {
                LiftALPHA  (0, 2, 1, 1);
                LiftBETA   (1, 2, 2, 1);
                LiftGAMMAx2(1, 2);

                NextSrc();

                if(m_height > 5) return true;
            }
            else
            {
                NextSrc();
                return true;
            }
        }

        if(m_row == 5)
        {
            if(m_heightOrgParity)
            {
                if(m_height > 5) { NextSrc(); return true; }
            }
            else
            {
                LiftALPHA  (0, 2, 1, 1);
                LiftBETA   (1, 2, 2, 0);
                LiftGAMMA  (0, 1, 2);
                LiftDELTAx2(2, 1);
                LiftKL(1);

                NextL();

                NextSrc();

                if(m_height > 6) return true;
            }
        }

        if(m_row==6 && (!m_heightOrgParity) && m_height > 6) { NextSrc(); return true; }


        if(m_i < m_iend)
        {
            if(m_isFullStep)
            {
                WT97FwdColLift(
                    DlyRowSrc(2), DlyRowSrc(1), DlyRowSrc(0),
                    DlyRowL(1), DlyRowL(0),
                    DlyRowH(2), DlyRowH(1), DlyRowH(0),
                    m_width);

                NextL();
                NextH();

                m_i++;

                NextSrc();

                if(m_i < m_iend)
                {
                    m_isFullStep = false;
                    return true;
                }
            }
            else
            {
                NextSrc();

                m_isFullStep = true;
                return true;
            }
        }

        if (m_isLastStepFull)
        {
            if(m_row != m_height) return true;

            LiftALPHAx2(1, 0, 0);
            LiftBETA   (0, 1, 1, 0);
            LiftGAMMA  (0, 1, 1);
            LiftDELTA  (1, 2, 1);
            LiftGAMMAx2(0, 0);
            LiftDELTA  (0, 1, 0);
            LiftKH(0);
        }
        else
        {
            LiftBETAx2(1, 1, 0);
            LiftGAMMA  (0, 1, 1);
            LiftDELTA  (1, 2, 1);
            LiftDELTAx2(1, 0);
        }
        LiftKL(1);
        LiftKL(0);
        LiftKH(2);
        LiftKH(1);

        NextL();
        NextL();
        NextH();
        NextH();
        if (m_isLastStepFull)
            NextH();

        return false;
    }

    void ProcessOdd_1();
    void ProcessOdd_2();
    void ProcessEven_2();
    void ProcessOdd_3();
    void ProcessEven_3();

    void LiftALPHAx2(unsigned int srcDlyToMul, unsigned int srcDlyToAdd, unsigned int dstDlyH);
    void LiftALPHA(unsigned int srcDlyToMul1, unsigned int srcDlyToMul2, unsigned int srcDlyToAdd, unsigned int dstDlyH);

    void LiftBETAx2(unsigned int srcDlyHToMul, unsigned int srcDlyToAdd, unsigned int dstDlyL);
    void LiftBETA(unsigned int srcDlyHToMul1, unsigned int srcDlyHToMul2, unsigned int srcDlyToAdd, unsigned int dstDlyL);

    void LiftGAMMAx2(unsigned int srcDlyLToMul, unsigned int dstDlyH);
    void LiftGAMMA(unsigned int srcDlyLToMul1, unsigned int srcDlyLToMul2, unsigned int dstDlyH);

    void LiftDELTA(unsigned int srcDlyHToMul1, unsigned int srcDlyHToMul2, unsigned int srcDstDlyL);
    void LiftDELTAx2(unsigned int srcDlyH, unsigned int srcDstDlyL);

    void LiftKL(unsigned int srcDstDlyL);
    void LiftKH(unsigned int srcDstDlyH);

    virtual void NextH() = 0;
    virtual void NextL() = 0;

    void NextSrc() { ++m_stripe; }

    virtual T* DlyRowL(unsigned int delay) = 0;
    virtual T* DlyRowH(unsigned int delay) = 0;

    T* DlyRowSrc(unsigned int delay)
    {
        switch(delay)
        {
        case 0: return m_stripe[0];
        case 1: return m_stripe[2];
        }
        return m_stripe[1];
    }

    RingStripe<T>   m_stripe;

    unsigned int    m_width;
    unsigned int    m_height;
    unsigned int    m_heightOrgParity;

    unsigned int    m_row;
    unsigned int    m_i;
    unsigned int    m_iend;

    bool            m_isLastStepFull;
    bool            m_isFullStep;
};

template<class T, class RowOutput>
class WT97FwdColPipeline : public BWT97FwdColPipeline<T>
{
public:
    WT97FwdColPipeline() {}
    virtual ~WT97FwdColPipeline() {}

    void ReAlloc(unsigned int maxWidth)
    {
        m_rowL.ReAlloc(maxWidth, 1);
        m_rowH.ReAlloc(maxWidth, 2);

        BWT97FwdColPipeline<T>::ReAlloc(maxWidth);
    }

    void InitAttach(
        const Rect      &rect,
              RowOutput &dstL,
              RowOutput &dstH)
    {
        m_rowL.Attach(dstL);
        m_rowH.Attach(dstH);

        BWT97FwdColPipeline<T>::InitAttach(rect);
    }

protected:
    void NextH() { ++m_rowH; }
    void NextL() { ++m_rowL; }

    T* DlyRowL(unsigned int delay) { return m_rowL.DelayedRow(delay); }
    T* DlyRowH(unsigned int delay) { return m_rowH.DelayedRow(delay); }

    TrackedRowOutput<T, RowOutput>   m_rowL;
    TrackedRowOutput<T, RowOutput>   m_rowH;
};

#endif // __WT97FWDCOLGEN_H__
