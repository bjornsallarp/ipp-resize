
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

#ifndef __WT53FWD_H__
#define __WT53FWD_H__

#include "pp.h"
#include "rowiterator.h"
#include "ringstripe.h"
#include "calcvector.h"
#include "wtmetric.h"

template<class T, class RowOutput>
class WT53FwdColPipeline
{
public:
    WT53FwdColPipeline() : m_width(0) {}

    void ReAlloc(unsigned int maxWidth)
    {
        m_stripe .ReAlloc(maxWidth, 3);
        m_stripeL.ReAlloc(maxWidth, 2);
        m_stripeH.ReAlloc(maxWidth, 2);
    }

    void InitAttach (
        const Rect      &rect,
              RowOutput &dstL,
              RowOutput &dstH)
    {
        m_row  = 0;

        m_dstL = &dstL;
        m_dstH = &dstH;

        m_width           = rect.Width();
        m_height          = rect.Height();
        m_heightOrgParity = rect.Y() & 1;
    }

    T* CurrInputRow() { return m_stripe.CurrRow(); }

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
            LShift   (1, m_stripe.CurrRow(), m_stripeH.CurrRow(), m_width);
            m_dstH->Write(m_stripeH[0]);
        }
        else
        {
            Copy <T*>(m_stripe.CurrRow(), m_stripeL.CurrRow(), m_width);
            m_dstL->Write(m_stripeL[0]);
        }

        return false;
    }

    bool StepForHeight2()
    {
        m_row++;
        ++m_stripe;

        if(m_row <= 1) return true;

        if(m_heightOrgParity)
        {
            Sub   (m_stripe[2], m_stripe[1], m_stripeH.CurrRow(), m_width);
            m_dstH->Write(m_stripeH[0]);

            Add   (1, m_stripeH[0], m_stripeL.CurrRow(), m_width);
            RShift(1,               m_stripeL.CurrRow(), m_width);
            Add   (m_stripe[2],     m_stripeL.CurrRow(), m_width);
            m_dstL->Write(m_stripeL[0]);
        }
        else
        {
            Sub   (m_stripe[1], m_stripe[2], m_stripeH.CurrRow(), m_width);
            m_dstH->Write(m_stripeH[0]);

            Add   (1, m_stripeH[0], m_stripeL.CurrRow(), m_width);
            RShift(1,               m_stripeL.CurrRow(), m_width);
            Add   (m_stripe[1],     m_stripeL.CurrRow(), m_width);
            m_dstL->Write(m_stripeL[0]);
        }

        return false;
    }

    bool StepForHeight3()
    {
        m_row++;
        ++m_stripe;

        if(m_row <= 2) return true;

        if(m_heightOrgParity)
        {
            Sub   (m_stripe[1], m_stripe[0], m_stripeH.CurrRow(),  m_width);
            m_dstH->Write(m_stripeH[0]);

            ++m_stripeH;

            Sub   (m_stripe[1], m_stripe[2], m_stripeH.CurrRow(), m_width);
            m_dstH->Write(m_stripeH[0]);

            Add   (2, m_stripeH[1], m_stripeL.CurrRow(), m_width);
            Add   (   m_stripeH[0], m_stripeL.CurrRow(), m_width);
            RShift(2,               m_stripeL.CurrRow(), m_width);
            Add   (m_stripe[1],     m_stripeL.CurrRow(), m_width);
            m_dstL->Write(m_stripeL[0]);
        }
        else
        {
            Add   (m_stripe[2], m_stripe[0], m_stripeH.CurrRow(), m_width);
            RShift(1,                        m_stripeH.CurrRow(), m_width);
            SubRev(m_stripe[1],              m_stripeH.CurrRow(), m_width);

            m_dstH->Write(m_stripeH[0]);

            Add   (1, m_stripeH[0], m_stripeL.CurrRow(), m_width);
            RShift(1,               m_stripeL.CurrRow(), m_width);

            T *prevL  = m_stripeL.CurrRow();

            ++m_stripeL;

            Add   (m_stripe[2], prevL, m_stripeL.CurrRow(), m_width);
            Add   (m_stripe[0], prevL, m_width);
            m_dstL->Write(m_stripeL[1]);
            m_dstL->Write(m_stripeL[0]);
        }

        return false;
    }

    bool StepForHeight4nMore()
    {
        m_row++;
        ++m_stripe;

        if(m_row == 1) return true;

        if(m_row == 2)
        {
            if(!m_heightOrgParity) return true;

            Sub(m_stripe[2], m_stripe[1], m_stripeH.CurrRow(), m_width);

            m_dstH->Write(m_stripeH[0]);
            ++m_stripeH;

            m_i          = 0;
            m_iend       = ( ( m_height - 1 + m_heightOrgParity ) >> 1) - 1;
            m_isFullStep = false;

            m_isLastStepFull = m_heightOrgParity ? (m_height & 1) : ( !(m_height & 1) );

            return true;
        }

        if(m_row == 3)
        {
            if(!m_heightOrgParity)
            {
                Add   (m_stripe[0], m_stripe[2], m_stripeH.CurrRow(), m_width);
                RShift(1,                        m_stripeH.CurrRow(), m_width);
                SubRev(m_stripe[1],              m_stripeH.CurrRow(), m_width);

                m_dstH->Write (m_stripeH[0]);
                ++m_stripeH;

                Add   (1, m_stripeH[1], m_stripeL.CurrRow(), m_width);
                RShift(1,               m_stripeL.CurrRow(), m_width);
                Add   (m_stripe[0],     m_stripeL.CurrRow(), m_width);

                m_dstL->Write(m_stripeL[0]);

                m_i        = 0;
                m_iend     = ( ( m_height - 1 + m_heightOrgParity ) >> 1) - 1;
                m_isFullStep = false;

                m_isLastStepFull = m_heightOrgParity ? (m_height & 1) : ( !(m_height & 1) );

                return true;
            }
        }

        if(m_i < m_iend)
        {
            if(m_isFullStep)
            {
                WT53FwdColLift(
                    m_stripe[0], m_stripe[1], m_stripe[2],
                    m_stripeL.CurrRow(),
                    m_stripeH[1], m_stripeH.CurrRow(),
                    m_width);

                m_dstH->Write(m_stripeH[0]);
                ++m_stripeH;
                m_dstL->Write(m_stripeL[0]);

                m_i++;

                if(m_i < m_iend)
                {
                    m_isFullStep = false;
                    return true;
                }
            }
            else
            {
                m_isFullStep = true;
                return true;
            }
        }

        if(m_isLastStepFull)
        {
            if(m_row != m_height) return true;

            WT53FwdColLift(
                m_stripe[1], m_stripe[2], m_stripe[1],
                m_stripeL.CurrRow(),
                m_stripeH[1], m_stripeH.CurrRow(),
                m_width);

            m_dstH->Write(m_stripeH[0]);
            m_dstL->Write(m_stripeL[0]);
        }
        else
        {
            Add   (m_stripe[0], m_stripe[2], m_stripeL.CurrRow(), m_width);
            RShift(1,                        m_stripeL.CurrRow(), m_width);
            SubRev(m_stripe[1],              m_stripeL.CurrRow(), m_width);

            Add   (m_stripeH[1], m_stripeL.CurrRow(), m_width);
            Add   (2,            m_stripeL.CurrRow(), m_width);
            RShift(2,            m_stripeL.CurrRow(), m_width);
            Add   (m_stripe[2],  m_stripeL.CurrRow(), m_width);

            m_dstL->Write(m_stripeL[0]);
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

    RowOutput     *m_dstL;
    RowOutput     *m_dstH;
};

template<class T>
void WT53FwdRowFirstL(const T* src, unsigned int  size, T* low, T* high)
{
    switch(size)
    {
    case 0:
        return;
    case 1:
        low[0]  = src[0];
        return;
    case 2:
        high[0] = src[1] - src[0];
        low [0] = src[0] + ( (high[0] + 1) >> 1 );
        return;
    case 3:
        high[0] = src[1] - ( (src[2] + src[0]) >> 1 );
        {
        T tmp = (high[0] + 1) >> 1;
        low [0] = src[0] + tmp;
        low [1] = src[2] + tmp;
        }
        return;
    default:
        break;
    }

    high[0] = src[1] - ( (src [0] + src[2]) >> 1 );
    low [0] = src[0] + ( (high[0] +     1 ) >> 1 );

    unsigned int dstLen = ( (size + 1) >> 1) - 2;

    if(dstLen) WT53FwdRow(&src[2], &low[1], &high[1], dstLen, ippWTFilterFirstLow);

    unsigned int i=dstLen+1;
    unsigned int j=2*i;

    if(size & 1)
    {
        low[i] = src[j] + ( (high[i-1] + 1) >> 1 );
    }
    else
    {
        high[i] = src[j+1] - src[j];
        low [i] = src[j] + ( (high[i-1] + high[i] + 2) >> 2 );
    }
}


template<class T>
void WT53FwdRowFirstH(const T* src, unsigned int size, T* low, T* high)
{
    switch(size)
    {
    case 0:
        return;
    case 1:
        high[0]  = src[0] << 1;
        return;
    case 2:
        high[0] = src[0] - src[1];
        low [0] = src[1] + ( (high[0] + 1) >> 1 );
        return;
    case 3:
        high[0] = src[0] - src[1];
        high[1] = src[2] - src[1];
        low [0] = src[1] + ( (high[0] + high[1] + 2) >> 2 );
        return;

    default:
        break;
    }

    high[0] = src[0] - src[1];

    unsigned int dstLen = ( (size) >> 1) - 2;

    if(dstLen) WT53FwdRow(&src[2], &low[1], &high[1], dstLen, ippWTFilterFirstHigh);

    unsigned int i=dstLen + 1;
    unsigned int j=2*i + 1;

    if(size & 1)
    {
        high[i]   = src[j-1] - ( (src[j-2] + src[j]) >> 1 );
        high[i+1] = src[j+1] - src[j];
        low[i] = src[j] + ( (high[i] + high[i+1] + 2) >> 2 );
    }
    else
    {
        high[i] = src[j-1] - ( (src[j-2] + src[j]) >> 1 );
        low [i] = src[j]   + ( (high[i] + 1) >> 1 );
    }

    low[0] = src[1] + ( (high[0] + high[1] + 2) >> 2 );
}

template<class T>
void WT53FwdRow(
    const T         *src,
    const UIntRange &range,
          T         *low,
          T         *high)
{
    WTPhase phase = Phase(range.Origin());

    if(phase == ippWTFilterFirstLow)
        WT53FwdRowFirstL(src, range.Size(), low, high);
    else
        WT53FwdRowFirstH(src, range.Size(), low, high);
}

#endif // __WT53FWD_H__
