
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

#ifndef __WT53INV_H__
#define __WT53INV_H__

#include "pp.h"
#include "rowiterator.h"
#include "ringstripe.h"
#include "calcvector.h"
#include "wtmetric.h"

template<class T , class RowInput>
class WT53InvColPipeline
{
public:
    WT53InvColPipeline() : m_width(0) {}

    void ReAlloc(unsigned int maxWidth)
    {
        m_stripe .ReAlloc(maxWidth, 3);
        m_stripeL.ReAlloc(maxWidth, 2);
        m_stripeH.ReAlloc(maxWidth, 3);

    }

    void InitAttach(
        const Rect     &rect,
              RowInput &srcL,
              RowInput &srcH)
    {
        m_row  = 0;

        m_srcL = &srcL;
        m_srcH = &srcH;

        m_width           = rect.Width();
        m_height          = rect.Height();
        m_heightOrgParity = rect.Y() & 1;
    }


    const T* CurrOutputRow() const { return m_stripe.CurrRow(); }

    bool NextStep()
    {
        switch(m_height)
        {
        case 1:  return StepForHeight1();
        case 2:  return StepForHeight2();
        case 3:  return StepForHeight3();
        default: return StepForHeight4nMore();
        }
    }

protected:
    bool StepForHeight1()
    {
        m_row++;

       if(m_heightOrgParity)
       {
           m_srcH->Read(m_stripeH[0]);
           RShift(1, m_stripeH[0], m_stripe.CurrRow(), m_width);
       }
       else
       {
           m_srcL->Read(m_stripeL[0]);
           Copy  (m_stripeL[0], m_stripe.CurrRow(), m_width);
       }

       return false;
    }

    bool StepForHeight2()
    {
        m_row++;
        ++m_stripe;

        if(m_row > 1) return false;

        m_srcH->Read(m_stripeH[0]);
        m_srcL->Read(m_stripeL[0]);

        if(m_heightOrgParity)
        {
            Add   (1, m_stripeH[0], m_stripe[1], m_width);
            RShift(1              , m_stripe[1], m_width);
            SubRev(   m_stripeL[0], m_stripe[1], m_width);
            Add   (   m_stripeH[0], m_stripe[1], m_stripe[0], m_width);
        }
        else
        {
            Add    (1, m_stripeH[0], m_stripe[0], m_width);
            RShift (1              , m_stripe[0], m_width);
            SubRev (   m_stripeL[0], m_stripe[0], m_width);
            Add    (   m_stripeH[0], m_stripe[0], m_stripe[1], m_width);
        }

        return true;
    }

    bool StepForHeight3()
    {
        m_row++;
        ++m_stripe;

        if(m_row > 2) return false;
        if(m_row > 1) return true;

        m_srcH->Read(m_stripeH.CurrRow());
        m_srcL->Read(m_stripeL.CurrRow());

        if(m_heightOrgParity)
        {
            Add   (2, m_stripeH[0], m_stripe[1], m_width);

            ++m_stripeH;
            m_srcH->Read(m_stripeH.CurrRow());

            Add   (m_stripeH[0], m_stripe[1], m_width);
            RShift(2,            m_stripe[1], m_width);
            SubRev(m_stripeL[0], m_stripe[1], m_width);

            Add   (m_stripeH[0], m_stripe[1], m_stripe[2], m_width);
            Add   (m_stripeH[2], m_stripe[1], m_stripe[0], m_width);
        }
        else
        {
            ++m_stripeL;
            m_srcL->Read(m_stripeL.CurrRow());

            Add   (1, m_stripeH[0], m_stripe[0], m_width);
            RShift(1, m_stripe[0], m_width);

            Sub   (m_stripe[0],  m_stripeL[0], m_stripe[2], m_width);
            SubRev(m_stripeL[1], m_stripe[0],               m_width);
            Add   (m_stripe[0],  m_stripe[2],  m_stripe[1], m_width);
            RShift(1,            m_stripe[1],               m_width);
            Add   (m_stripeH[0], m_stripe[1],               m_width);
        }

        return true;
    }

    bool StepForHeight4nMore()
    {
        m_row++;
        ++m_stripe;

        if(m_row >= m_height) return false;

        if(m_row == 2 &&  m_heightOrgParity) return true;

        if(m_row == 1)
        {
            m_i              = 1;
            m_iend           = ( ( m_height + m_heightOrgParity ) >> 1) -    m_heightOrgParity;
            m_isLastStepFull = m_heightOrgParity ? ( !(m_height & 1) ) : (m_height & 1);
            m_isFullStep     = true;

            m_srcH->Read(m_stripeH.CurrRow());
            m_srcL->Read(m_stripeL.CurrRow());
            if(m_heightOrgParity)
            {
                Add   (2, m_stripeH[0], m_stripe[1], m_width);

                ++m_stripeH;
                m_srcH->Read(m_stripeH.CurrRow());
                Add    (m_stripeH[0], m_stripe[1], m_width);
                RShift (2           , m_stripe[1], m_width);
                SubRev (m_stripeL[0], m_stripe[1], m_width);
                Add    (m_stripeH[2], m_stripe[1], m_stripe[0], m_width);
            }
            else
            {
                Add   (1, m_stripeH[0], m_stripe[0], m_width);
                RShift(1,               m_stripe[0], m_width);
                SubRev(m_stripeL[0],    m_stripe[0], m_width);
            }

            return true;
        }

        if(m_i < m_iend)
        {
            if(m_isFullStep)
            {
                ++m_stripeL;
                m_srcL->Read(m_stripeL.CurrRow());
                ++m_stripeH;
                m_srcH->Read(m_stripeH.CurrRow());

                WT53InvColLift(
                    m_stripeL[0],
                    m_stripeH[2], m_stripeH[0],
                    m_stripe[2], m_stripe[0], m_stripe[1],
                    m_width);

                m_i++;

                m_isFullStep = false;
            }
            else m_isFullStep = true;

            return true;
        }

        if(m_isLastStepFull)
        {
            if(m_row == m_height-2) return true;
            ++m_stripeL;
            m_srcL->Read(m_stripeL.CurrRow());


            WT53InvColLift(
                m_stripeL[0],
                m_stripeH[0], m_stripeH[0],
                m_stripe[2], m_stripe[0], m_stripe[1],
                m_width);
        }
        else
        {

            Add   (2, m_stripeH[2]           , m_stripe[1], m_width);
            Add   (m_stripeH[0], m_stripeH[2], m_stripe[1], m_width);
            RShift(2                         , m_stripe[1], m_width);
            SubRev(m_stripeL[0]              , m_stripe[1], m_width);
            Add   (m_stripe[0]               , m_stripe[1], m_width);
            RShift(1                         , m_stripe[1], m_width);
            Add   (m_stripeH[0]              , m_stripe[1], m_width);
        }
        return false;
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

    RingStripe<T>  m_stripeL;
    RingStripe<T>  m_stripeH;

    RowInput      *m_srcL;
    RowInput      *m_srcH;
};


template<class T>
void WT53InvRowFirstL(const T* low, const T* high, T* dst, unsigned int  size)
{
    switch(size)
    {
    case 0:
        return;
    case 1:
        dst[0] = low[0];
        return;
    case 2:
        dst[0] = low[0] - ( (high[0] + 1) >> 1 );
        dst[1] = high[0] + dst[0];
        return;
    case 3:
        {
        T tmp = (high[0] + 1) >> 1;

        dst[0] = low[0] - tmp;
        dst[2] = low[1] - tmp;
        }
        dst[1] = high[0] + ( (dst[0] + dst[2]) >> 1 );
        return;
    default:
        break;
    }

    dst[0] = low[0] - ( (high[0] + 1) >> 1 );

    unsigned int srcLen = ( (size) >> 1) - 2;

    if(srcLen) WT53InvRow(&low[1], &high[1], srcLen, &dst[2], ippWTFilterFirstLow);

    unsigned int i=srcLen+1;
    unsigned int j=2*i;
    if(size & 1)
    {
        dst[j]   = low[i] - ( (high[i-1] + high[i] + 2) >> 2 );
        dst[j+2] = low[i+1] - ( (high[i] + 1) >> 1 );
        dst[j+1] = high[i] + ( (dst[j] + dst[j+2]) >> 1 );
    }
    else
    {
        dst[j]   = low [i] - ( (high[i-1] + high[i] + 2) >> 2 );
        dst[j+1] = high[i] + dst[j];
    }
    dst[1] = high[0] + ( (dst[0] + dst[2]) >> 1 );
}


template<class T>
void WT53InvRowFirstH(const T* low, const T* high, T* dst, unsigned int  size)
{
    switch(size)
    {
    case 0:
        return;
    case 1:
        dst[0] = high[0] >> 1;
        return;
    case 2:
        dst[1] = low[0] - ( (high[0] + 1) >> 1 );
        dst[0] = high[0] + dst[1];
        return;
    case 3:
        dst[1] = low[0] - ( (high[0] + high[1] + 2) >> 2 );
        dst[2] = high[1] + dst[1];
        dst[0] = high[0] + dst[1];
        return;
    default:
        break;
    }

    dst[1] = low[0] - ( (high[0] + high[1] + 2) >> 2 );
    dst[0] = high[0] + dst[1];

    unsigned int srcLen = ( (size + 1) >> 1) - 2;

    if(srcLen) WT53InvRow(&low[1], &high[1], srcLen, &dst[2], ippWTFilterFirstHigh);

    unsigned int i=srcLen+1;
    unsigned int j=2*i;

    if(size & 1)
    {
        dst[j] = high[i] + dst[j-1];
    }
    else
    {
        dst[j+1] = low [i] - ( (high[i] + 1) >> 1 );
        dst[j]   = high[i] + ( (dst[j-1] + dst[j+1]) >> 1 );
    }
}

template<class T>
void WT53InvRow(const T *low, const T *high, T *dst, const UIntRange &range)
{
    WTPhase phase = Phase(range.Origin());

    if(phase == ippWTFilterFirstLow)
        WT53InvRowFirstL(low, high, dst, range.Size());
    else
        WT53InvRowFirstH(low, high, dst, range.Size());
}

#endif // __WT53INV_H__
