
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

#ifndef __WT97INVCOLGEN_H__
#define __WT97INVCOLGEN_H__


#include "pp.h"
#include "trackedrowinput.h"

template<class T>
class BWT97InvColPipeline
{
public:
    void ReAlloc(unsigned int maxWidth) { m_stripe.ReAlloc(maxWidth, 5); }

    void InitAttach(const Rect &rect)
    {
        m_width           = rect.Width();
        m_height          = rect.Height();
        m_heightOrgParity = rect.Y() & 1;

        m_row             = 0;
        m_i               = 0;
        m_iend            = ( (m_height - m_heightOrgParity) >> 1 ) - 2;
        m_isFullStep      = true;
        m_isLastStepFull  = m_heightOrgParity ? ( !(m_height & 1) ) : (m_height & 1);
    }


    const T* CurrOutputRow() const { return m_stripe.CurrRow(); }

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
    BWT97InvColPipeline() : m_width(0) {}
    virtual ~BWT97InvColPipeline() {}

    bool StepForHeight1()
    {
        m_row++;

        if(m_heightOrgParity)  { NextH(); ProcessOdd_1(); }
        else                   { NextL(); Copy <T*>(DlyRowL(0),       DlyRowDst(0), m_width); }

        return false;
    }

    bool StepForHeight2()
    {
        m_row++;

        if(m_row > 1) { NextDst();  return false; }

        NextL();
        NextH();

        if(m_heightOrgParity) ProcessOdd_2();
        else                  ProcessEven_2();

        NextDst();

        return true;
    }

    bool StepForHeight3()
    {
        m_row++;

        if(m_row > 2) { NextDst(); return false; }
        if(m_row > 1) { NextDst(); return true;  }

        NextL();
        NextH();

        if(m_heightOrgParity) { NextH(); ProcessOdd_3();  }
        else                  { NextL(); ProcessEven_3(); }

        NextDst();

        return true;
    }

    bool StepForHeight4()
    {
        if(!m_heightOrgParity) return StepForHeight5nMore();

        m_row++;

        if(m_row > 3) { NextDst(); return false; }
        if(m_row > 1) { NextDst(); return true;  }

        NextL();
        NextL();
        NextH();
        NextH();

        LiftDELTA(1, 1, 0, 3);
        LiftDELTAx2(0, 0, 1);
        LiftGAMMA(0, 3, 1, 2);
        LiftGAMMAx2(1, 3, 4);
        LiftBETAx2(2, 1);
        LiftBETA(4, 2, 3);
        LiftALPHA(3, 1, 2);
        LiftALPHAx2(3, 4);

        NextDst();

        return true;
    }

    bool StepForHeight5nMore()
    {
        m_row++;

        if(m_row >= m_height) { NextDst(); return false; }

        if(m_row >= (m_height-2- (m_isLastStepFull&1))) { NextDst(); return true; }

        if(m_row == 1)
        {
            if(m_heightOrgParity)
            {
                NextL();
                NextL();
                NextH();
                NextH();
                NextH();

                FirtStepBig();
                NextDst();
                return true;
            }
            else
            {
                NextL();
                NextL();
                NextH();
                NextH();

                FirtStepSmall();
            }
        }

        if(m_i < m_iend)
        {
            if(m_isFullStep)
            {
                NextL();
                NextH();

                WT97InvColLift(
                    DlyRowL(0),
                    DlyRowH(1), DlyRowH(0),
                    DlyRowDst(4), DlyRowDst(3), DlyRowDst(2), DlyRowDst(1), DlyRowDst(0),
                    m_width);

                m_isFullStep = false;
            }
            else
            {
                m_isFullStep = true;
                m_i++;
            }

            NextDst();
            return true;
        }

        if(m_isLastStepFull)
        {
            NextL();
            LastStepBig();
        }
        else
        {
            LastStepSmall();
        }

        NextDst();
        return true;
    }

    void ProcessOdd_1();
    void ProcessOdd_2();
    void ProcessEven_2();
    void ProcessOdd_3();
    void ProcessEven_3();

    void LiftDELTA(unsigned int dlyL, unsigned int dlyH1, unsigned int dlyH2, unsigned int dlyDst);
    void LiftDELTAx2(unsigned int dlyL, unsigned int dlyH1, unsigned int dlyDst);
    void LiftGAMMA(unsigned int dlyH, unsigned int dlySrc1, unsigned int dlySrc2, unsigned int dlyDst);
    void LiftGAMMAx2(unsigned int dlyH, unsigned int dlySrc1, unsigned int dlyDst);
    void LiftBETA(unsigned int dlySrc1, unsigned int dlySrc2, unsigned int dlyDst);
    void LiftBETAx2(unsigned int dlySrc1, unsigned int dlyDst);
    void LiftALPHA(unsigned int dlySrc1, unsigned int dlySrc2, unsigned int dlyDst);
    void LiftALPHAx2(unsigned int dlySrc1, unsigned int dlyDst);

    void FirtStepSmall()
    {
        LiftDELTAx2(1, 1, 4);
        LiftDELTA(0, 1, 0, 2);
        LiftGAMMA(1, 4, 2, 3);
        LiftBETAx2(3, 4);
    }

    void FirtStepBig()
    {
        LiftDELTA(1, 2, 1, 3);
        LiftGAMMAx2(2, 3, 4);
        LiftDELTA(0, 1, 0, 1);
        LiftGAMMA(1, 3, 1, 2);
        LiftBETA(4, 2, 3);
        LiftALPHAx2(3, 4);
    }

    void LastStepSmall()
    {
        LiftGAMMAx2(0, 2, 1);
        LiftBETA(3, 1, 2);
        LiftALPHAx2(2, 1);
        LiftALPHA(2, 4, 3);
    }

    void LastStepBig()
    {
        LiftDELTAx2(0, 0, 0);
        LiftGAMMA(0, 2, 0, 1);
        LiftBETAx2(1, 0);
        LiftBETA(3, 1, 2);
        LiftALPHA(4, 2, 3);
        LiftALPHA(2, 0, 1);
    }

    virtual void NextH() = 0;
    virtual void NextL() = 0;

    void NextDst() { ++m_stripe; }

    virtual T* DlyRowL(unsigned int delay) = 0;
    virtual T* DlyRowH(unsigned int delay) = 0;

    T* DlyRowDst(unsigned int delay)
    {
        switch(delay)
        {
        case 0: return m_stripe[0];
        case 1: return m_stripe[4];
        case 2: return m_stripe[3];
        case 3: return m_stripe[2];
        }
        return m_stripe[1];
    }

    RingStripe<T>  m_stripe;

    unsigned int   m_width;
    unsigned int   m_height;
    unsigned int   m_heightOrgParity;

    unsigned int   m_row;
    unsigned int   m_i;
    unsigned int   m_iend;

    bool           m_isLastStepFull;
    bool           m_isFullStep;
};

template<class T, class RowInput>
class WT97InvColPipeline : public BWT97InvColPipeline<T>
{
public:
    WT97InvColPipeline() {}

    void ReAlloc(unsigned int maxWidth)
    {
        m_rowL.ReAlloc(maxWidth, 1);
        m_rowH.ReAlloc(maxWidth, 2);

        BWT97InvColPipeline<T>::ReAlloc(maxWidth);
    }

    void InitAttach(
        const Rect      &rect,
              RowInput  &dstL,
              RowInput  &dstH)
    {
        m_rowL.Attach(dstL);
        m_rowH.Attach(dstH);

        BWT97InvColPipeline<T>::InitAttach(rect);
    }

protected:
    void NextH() { ++m_rowH; }
    void NextL() { ++m_rowL; }

    T* DlyRowL(unsigned int delay) { return m_rowL.DelayedRow(delay); }
    T* DlyRowH(unsigned int delay) { return m_rowH.DelayedRow(delay); }

    TrackedRowInput<T, RowInput>   m_rowL;
    TrackedRowInput<T, RowInput>   m_rowH;
};

#endif // __WT97INVCOLGEN_H__
