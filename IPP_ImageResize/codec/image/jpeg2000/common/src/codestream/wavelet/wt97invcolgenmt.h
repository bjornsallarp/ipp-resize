
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

#ifndef __WT97INVCOLGENMT_H__
#define __WT97INVCOLGENMT_H__

#include "wt97invcolgenlmt.h"
#include "wt97invcolgenhmt.h"


template<class T, class RowInput>
class WT97InvColPipelineMT
{
public:
    WT97InvColPipelineMT() : m_base(0) {}
    ~WT97InvColPipelineMT() { if(m_base) delete m_base; }

    void ReAllocInitAttach(
        const Rect   &rect,
        unsigned int  toBegBorder,
        unsigned int  toEndBorder,
        RowInput     &srcL, // pointing to border
        RowInput     &srcH) // pointing to border
    {
        if(m_base) delete m_base;

        if(ippWTFilterFirstLow == Phase(rect.RangeY().Origin()))
        {
            switch(rect.RangeY().Size())
            {
            case 0:
                m_base = 0;
                break;
            case 1:
                m_base = new WT97InvColPipelineFirstL_1_MT<T, RowInput>();
                break;
            case 2:
                m_base = new WT97InvColPipelineFirstL_2_MT<T, RowInput>();
                break;
            case 3:
                m_base = new WT97InvColPipelineFirstL_3_MT<T, RowInput>();
                break;
            case 4:
                m_base = new WT97InvColPipelineFirstL_4_MT<T, RowInput>();
                break;
            default:
                m_base = new WT97InvColPipelineFirstL_5nMore_MT<T, RowInput>();
            }
        }
        else
        {
            throw 0;
        }
        if(m_base)
        {
            m_base->ReAlloc(rect.Width());
            m_base->InitAttach(
                rect,
                toBegBorder,
                toEndBorder,
                srcL,
                srcH);
        }
    }

    bool NextStep()
    {
        if(m_base) return m_base->NextStep();
        return false;
    }

    const T* CurrOutputRow() const
    {
        if(m_base) return m_base->CurrOutputRow();
        else return 0;
    }

protected:
    BWT97InvColPipelineMT<T, RowInput> *m_base;
};

#endif // __WT97INVCOLGENMT_H__
