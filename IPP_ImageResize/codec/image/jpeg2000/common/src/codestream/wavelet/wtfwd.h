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

#ifndef __WTFWD_H__
#define __WTFWD_H__

//
// Here is WT templates implementing general functionality of full 2D multi-level transforms.
// (Both WT53 and WT97)
//

#include "pp.h"


#include "wt53fwd.h"


//////////////////////////////////////////////////////////////////////////
#include "wt97fwdrow16s.h"
#include "wt97fwdrow32s.h"

#include "wt97fwdrow32f.h"

#include "wt97fwdrowgen.h"


//////////////////////////////////////////////////////////////////////////
#include "wt97fwdcol16s.h"
#include "wt97fwdcol32s.h"

#include "wt97fwdcol32f.h"

#include "wt97fwdcolgen.h"


//////////////////////////////////////////////////////////////////////////
#include "imagerowoutput.h"

#include "sbtree.h"

//////////////////////////////////////////////////////////////////////////
//
// Auxiliary regularization to switch WT53/WT97 in argument of WTFwd<AGREGATE> template
//

template<class T, class RowOutput> class WT53FwdRowAgregate
{
public:
    typedef T AGR_T;
    typedef RowOutput AGR_RowOutput;
    static void FwdRow(const T *src, const UIntRange &range, T *low, T *high) { WT53FwdRow(src, range, low, high); }
};

template<class T, class RowOutput> class WT97FwdRowAgregate
{
public:
    typedef T AGR_T;
    typedef RowOutput AGR_RowOutput;
    static void FwdRow(const T *src, const UIntRange &range, T *low, T *high) { WT97FwdRow<T>(src, range, low, high); }
};

template <class RowAgregate>
class WTFwdRow
{
public:
    typedef typename RowAgregate::AGR_T T;
    typedef typename RowAgregate::AGR_RowOutput RowOutput;

    WTFwdRow() {}

    void ReAlloc(unsigned int maxSize)
    {
        unsigned int maxCmpSize = (maxSize >> 1) + 2;
        m_buffL.ReAlloc(maxCmpSize);
        m_buffH.ReAlloc(maxCmpSize);
    }

    void InitAttach (const UIntRange &range, RowOutput &dstL, RowOutput &dstH)
    {
        m_range = range;

        m_dstL  = &dstL;
        m_dstH  = &dstH;
    }

    void Write(T *row)
    {
        RowAgregate::FwdRow(row, m_range, m_buffL, m_buffH);
        m_dstL->Write(m_buffL);
        m_dstH->Write(m_buffH);
    }

protected:
    UIntRange      m_range;

    RowOutput     *m_dstL;
    RowOutput     *m_dstH;

    FixedBuffer<T> m_buffL;
    FixedBuffer<T> m_buffH;
};

template<class T, class RowOutput> class WT53FwdAgregate
{
public:
    typedef T AGR_T;
    typedef WTFwdRow<WT53FwdRowAgregate<T, RowOutput> > AGR_WTFwdRow;
    // type synchronizing for pipeline of lines processing and transferring data (row by row) to column-oriented transform
    typedef WT53FwdColPipeline<T, AGR_WTFwdRow> AGR_WTFwdCol;
};

template<class T, class RowOutput> class WT97FwdAgregate
{
public:
    typedef T AGR_T;
    typedef WTFwdRow<WT97FwdRowAgregate<T, RowOutput> > AGR_WTFwdRow;
    // type synchronizing for pipeline of lines processing and transferring data (row by row) to column-oriented transform
    typedef WT97FwdColPipeline<T, AGR_WTFwdRow> AGR_WTFwdCol;
};

//////////////////////////////////////////////////////////////////////////
//
// Here is template implementation for multi-level 2D WT
//

template <class AGREGATE>
class WTFwd
{
protected:
    typedef typename AGREGATE::AGR_WTFwdRow::T T;
    typedef typename AGREGATE::AGR_WTFwdRow::RowOutput RowOutput;
public:
    WTFwd() {}

    void ReAlloc(const RectSize &maxSize)
    {
        m_col  .ReAlloc(maxSize.Width());
        m_rowLy.ReAlloc(maxSize.Width());
        m_rowHy.ReAlloc(maxSize.Width());
    }

    void Transform(
        const ImageCoreC<T, 1> &src,
        const Rect             &rect,
        const ImageCoreC<T, 1> &dstLxLy,
        const ImageCoreC<T, 1> &dstHxLy,
        const ImageCoreC<T, 1> &dstLxHy,
        const ImageCoreC<T, 1> &dstHxHy)
    {
        RowIterator<T> srcRow = src;
        unsigned int widthL = ScaleL(rect.RangeX()).Size();
        unsigned int widthH = ScaleH(rect.RangeX()).Size();
        RowOutput      dstRowLxLy(dstLxLy, widthL);
        RowOutput      dstRowHxLy(dstHxLy, widthH);
        RowOutput      dstRowLxHy(dstLxHy, widthL);
        RowOutput      dstRowHxHy(dstHxHy, widthH);

        m_rowLy.InitAttach(rect.RangeX(), dstRowLxLy, dstRowHxLy);
        m_rowHy.InitAttach(rect.RangeX(), dstRowLxHy, dstRowHxHy);

        m_col  .InitAttach(rect, m_rowLy, m_rowHy);

        for(unsigned int i = 0; i < rect.Height(); i++)
        {
            Copy(srcRow, m_col.CurrInputRow(), rect.Width());
            m_col.NextStep();
            ++srcRow;
        }
    }

    // multi-level transform
    static void Transform(const ImageCoreC<T, 1> &src, const Rect &rect, const SBTree<T> &dst)
    {
        if(dst.NOfWTLevels()==0)
        {
            Copy(src, dst.LxLy(), rect.Size());
            return;
        }

        WTFwd<AGREGATE> fwd;

        fwd.ReAlloc(rect.Size());
        ImageC<T, 1> LxLy(ScaleLxLy(rect).Size());

        const ImageCoreC<T, 1> *parent = &src;

        int level = 0;
        for(; level < (int)dst.NOfWTLevels() - 1; level++)
        {
            fwd.Transform(*parent, ScaleLxLy(rect, level),
                LxLy,
                dst.HxLy(level),
                dst.LxHy(level),
                dst.HxHy(level));

            parent = &LxLy.Core();
        }

        fwd.Transform(*parent, ScaleLxLy(rect, level),
            dst.LxLy(),
            dst.HxLy(level),
            dst.LxHy(level),
            dst.HxHy(level));
    }


protected:
    typename AGREGATE::AGR_WTFwdCol  m_col;
    typename AGREGATE::AGR_WTFwdRow  m_rowLy;
    typename AGREGATE::AGR_WTFwdRow  m_rowHy;
};

//////////////////////////////////////////////////////////////////////////
//
// Auxiliary names to call multi-level 2D WT
//
template<class T>
void WT53Fwd(const ImageCoreC<T, 1> &src, const Rect &rect, const SBTree<T> &dst)
{
    WTFwd<WT53FwdAgregate<T, ImageRowOutput<T, 1> > >::Transform(src, rect, dst);
}

template<class T>
void WT97Fwd(const ImageCoreC<T, 1> &src, const Rect &rect, const SBTree<T> &dst)
{
    WTFwd<WT97FwdAgregate<T, ImageRowOutput<T, 1> > >::Transform(src, rect, dst);
}

/*
template<class WTFWD, class T>
void Fwd(const ImageCoreC<T, 1> &src, const Rect &rect, const SBTree<T> &dst)
{
    if(dst.NOfWTLevels()==0)
    {
        Copy(src, dst.LxLy(), rect.Size());
        return;
    }

    WTFWD fwd;
    fwd.ReAlloc(rect.Size());
    ImageC<T, 1> LxLy(ScaleLxLy(rect).Size());

    const ImageCoreC<T, 1> *parent = &src;

    int level = 0;
    for(; level < (int)dst.NOfWTLevels() - 1; level++)
    {
        fwd.Transform(*parent, ScaleLxLy(rect, level),
            LxLy,
            dst.HxLy(level),
            dst.LxHy(level),
            dst.HxHy(level));

        parent = &LxLy.Core();
    }

    fwd.Transform(*parent, ScaleLxLy(rect, level),
        dst.LxLy(),
        dst.HxLy(level),
        dst.LxHy(level),
        dst.HxHy(level));
}
*/

#endif // __WTFWD_H__
