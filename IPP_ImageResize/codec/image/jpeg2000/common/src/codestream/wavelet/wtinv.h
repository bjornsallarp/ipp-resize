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

#ifndef __WTINV_H__
#define __WTINV_H__

//
// Here is WT templates implementing general functionality of full 2D multi-level transforms.
// (Both WT53 and WT97)
//

#include "pp.h"

#include "wt53inv.h"

#include "wt97invrow16s.h"
#include "wt97invrow32s.h"
#include "wt97invrow32f.h"
#include "wt97invcol16s.h"
#include "wt97invcol32s.h"
#include "wt97invcol32f.h"
#include "wt97invrowgen.h"
#include "wt97invcolgen.h"

#include "imagerowinput.h"

#include "sbtree.h"

//////////////////////////////////////////////////////////////////////////
//
// Auxiliary regularization to switch WT53/WT97 in argument of WTInv<AGREGATE> template
//


template<class T, class RowInput> class WT53InvRowAgregate
{
public:
    typedef T AGR_T;
    typedef RowInput AGR_RowInput;
    static void InvRow(const T *low, const T *high, T *dst, const UIntRange &range) { WT53InvRow(low, high, dst, range); }
};

template<class T, class RowInput> class WT97InvRowAgregate
{
public:
    typedef T AGR_T;
    typedef RowInput AGR_RowInput;
    static void InvRow(const T *low, const T *high, T *dst, const UIntRange &range) { WT97InvRow<T>(low, high, dst, range); }
};

template <class RowAgregate>
class WTInvRow
{
public:
    typedef typename RowAgregate::AGR_T T;
    typedef typename RowAgregate::AGR_RowInput RowInput;

    WTInvRow() {}

    void ReAlloc(unsigned int maxSize)
    {
        unsigned int maxCmpSize = (maxSize >> 1) + 2;
        m_buffL.ReAlloc(maxCmpSize);
        m_buffH.ReAlloc(maxCmpSize);
    }

    void InitAttach (const UIntRange &range, RowInput &srcL, RowInput &srcH)
    {
        m_range = range;

        m_srcL  = &srcL;
        m_srcH  = &srcH;
    }

    void Read(T *row)
    {
        m_srcL->Read(m_buffL);
        m_srcH->Read(m_buffH);
        RowAgregate::InvRow(m_buffL, m_buffH, row, m_range);
    }

protected:
    UIntRange      m_range;

    RowInput      *m_srcL;
    RowInput      *m_srcH;

    FixedBuffer<T> m_buffL;
    FixedBuffer<T> m_buffH;
};

template<class T, class RowInput> class WT53InvAgregate
{
public:
    typedef T AGR_T;
    typedef WTInvRow<WT53InvRowAgregate<T, RowInput> > AGR_WTInvRow;
    // type synchronizing for pipeline of lines processing and transferring data (row by row) to column-oriented transform
    typedef WT53InvColPipeline<T, AGR_WTInvRow> AGR_WTInvCol;
};

template<class T, class RowInput> class WT97InvAgregate
{
public:
    typedef T AGR_T;
    typedef WTInvRow<WT97InvRowAgregate<T, RowInput> > AGR_WTInvRow;
    // type synchronizing for pipeline of lines processing and transferring data (row by row) to column-oriented transform
    typedef WT97InvColPipeline<T, AGR_WTInvRow> AGR_WTInvCol;
};

//////////////////////////////////////////////////////////////////////////
//
// Here is template implementation for multi-level 2D WT
//
template <class AGREGATE>
class WTInv
{
protected:
    typedef typename AGREGATE::AGR_WTInvRow::T T;
    typedef typename AGREGATE::AGR_WTInvRow::RowInput RowInput;
public:
    WTInv() {}

    void ReAlloc(const RectSize &maxSize)
    {
        m_col  .ReAlloc(maxSize.Width());
        m_rowLy.ReAlloc(maxSize.Width());
        m_rowHy.ReAlloc(maxSize.Width());
    }

    // one-level transform
    void Transform(
        const ImageCoreC<T, 1> &srcLxLy,
        const ImageCoreC<T, 1> &srcHxLy,
        const ImageCoreC<T, 1> &srcLxHy,
        const ImageCoreC<T, 1> &srcHxHy,
        const ImageCoreC<T, 1> &dst, const Rect &rect)
    {
        unsigned int widthL = ScaleL(rect.RangeX()).Size();
        unsigned int widthH = ScaleH(rect.RangeX()).Size();
        RowInput       srcRowLxLy(srcLxLy, widthL);
        RowInput       srcRowHxLy(srcHxLy, widthH);
        RowInput       srcRowLxHy(srcLxHy, widthL);
        RowInput       srcRowHxHy(srcHxHy, widthH);
        RowIterator<T> dstRow  = dst;

        m_rowLy.InitAttach(rect.RangeX(), srcRowLxLy, srcRowHxLy);
        m_rowHy.InitAttach(rect.RangeX(), srcRowLxHy, srcRowHxHy);

        m_col  .InitAttach(rect, m_rowLy, m_rowHy);

        for(unsigned int i = 0; i < rect.Height(); i++)
        {
            m_col.NextStep();
            Copy(m_col.CurrOutputRow(), dstRow, rect.Width());
            ++dstRow;
        }

    }

    // multi-level transform
    static void Transform(const SBTree<T> &src, const ImageCoreC<T, 1> &dst, const Rect &rect)
    {
        if(src.NOfWTLevels()==0)
        {
            Copy(src.LxLy(), dst, rect.Size());
            return;
        }

        WTInv<AGREGATE> inv;

        inv.ReAlloc(rect.Size());

        int level = src.NOfWTLevels() - 1;

        if(src.NOfWTLevels()==1)
        {
            inv.Transform(
                src.LxLy(),
                src.HxLy(level),
                src.LxHy(level),
                src.HxHy(level),
                dst, ScaleLxLy(rect, level));

            return;
        }

        ImageC<T, 1> tmp(ScaleLxLy(rect).Width(), ScaleLxLy(rect).Height());


        if(src.NOfWTLevels() & 1)
        {
            inv.Transform(
                src.LxLy(),
                src.HxLy(level),
                src.LxHy(level),
                src.HxHy(level),
                dst, ScaleLxLy(rect, level));

            level--;

            for(; level >= 0; )
            {
                inv.Transform(
                    dst,
                    src.HxLy(level),
                    src.LxHy(level),
                    src.HxHy(level),
                    tmp.Core(), ScaleLxLy(rect, level));

                level--;

                inv.Transform(
                    tmp.Core(),
                    src.HxLy(level),
                    src.LxHy(level),
                    src.HxHy(level),
                    dst, ScaleLxLy(rect, level));

                level--;
            }
        }
        else
        {
            inv.Transform(
                src.LxLy(),
                src.HxLy(level),
                src.LxHy(level),
                src.HxHy(level),
                tmp.Core(), ScaleLxLy(rect, level));

            level--;

            for(; level >= 1; )
            {
                inv.Transform(
                    tmp.Core(),
                    src.HxLy(level),
                    src.LxHy(level),
                    src.HxHy(level),
                    dst, ScaleLxLy(rect, level));

                level--;

                inv.Transform(
                    dst,
                    src.HxLy(level),
                    src.LxHy(level),
                    src.HxHy(level),
                    tmp.Core(), ScaleLxLy(rect, level));

                level--;
            }

            inv.Transform(
                tmp.Core(),
                src.HxLy(level),
                src.LxHy(level),
                src.HxHy(level),
                dst, ScaleLxLy(rect, level));
        }
    }


protected:
protected:
    typename AGREGATE::AGR_WTInvCol  m_col;
    typename AGREGATE::AGR_WTInvRow  m_rowLy;
    typename AGREGATE::AGR_WTInvRow  m_rowHy;
};

//////////////////////////////////////////////////////////////////////////
//
// Auxiliary names to call multi-level 2D WT
//
template<class T>
void WT53Inv(const SBTree<T> &src, const ImageCoreC<T, 1> &dst, const Rect &rect)
{
    WTInv<WT53InvAgregate<T, ImageRowInput<T, 1> > >::Transform(src, dst, rect);
}

template<class T>
void WT97Inv(const SBTree<T> &src, const ImageCoreC<T, 1> &dst, const Rect &rect)
{
    WTInv<WT97InvAgregate<T, ImageRowInput<T, 1> > >::Transform(src, dst, rect);
}

#endif // __WTINV_H__
