
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

#ifndef __WTINVMT_H__
#define __WTINVMT_H__

#include "pp.h"

#include "wt97invrowmt32f.h"
#include "wtinvrowmt.h"
#include "wt97invcolmt32f.h"
#include "wt97invcolgenmt.h"
#include "sbtree.h"

/*
template<class T, class RowInput> class WT53InvAgregate
{
public:
    typedef T AGR_T;
    typedef WTInvRowMT<WT53InvRowAgregateMT<T, RowInput> > AGR_WTInvRow;
    // type synchronizing for pipeline of lines processing and transferring data (row by row) to column-oriented transform
    typedef WT53InvColPipelineMT<T, AGR_WTInvRow> AGR_WTInvCol;
};
*/

template<class T, class RowInput> class WT97InvAgregateMT
{
public:
    typedef T AGR_T;
    typedef WTInvRowMT<WT97InvRowAgregateMT<T, RowInput> > AGR_WTInvRow;
    // type synchronizing for pipeline of lines processing and transferring data (row by row) to column-oriented transform
    typedef WT97InvColPipelineMT<T, AGR_WTInvRow> AGR_WTInvCol;

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


template <class AGREGATE>
class WTInvCellMT
{
protected:
    typedef typename AGREGATE::AGR_WTInvRow::T T;
    typedef typename AGREGATE::AGR_WTInvRow::RowInput RowInput;
public:
    WTInvCellMT() {}

    void ReAlloc(const RectSize &maxSize)
    {
        m_rowLy.ReAlloc(maxSize.Width());
        m_rowHy.ReAlloc(maxSize.Width());
    }

    void Transform(
        const ImageCoreC<T, 1> &srcLxLy,
        const ImageCoreC<T, 1> &srcHxLy,
        const ImageCoreC<T, 1> &srcLxHy,
        const ImageCoreC<T, 1> &srcHxHy,
        const ImageCoreC<T, 1> &dst,
        const Rect             &rect,
        const Rect             &cellRect)
    {
        Rect rectLxLy     = ScaleLxLy(rect);
        Rect rectHxLy     = ScaleHxLy(rect);
        Rect rectLxHy     = ScaleLxHy(rect);
        Rect rectHxHy     = ScaleHxHy(rect);

        Rect cellRectLxLy = ScaleLxLy(cellRect);
        Rect cellRectHxLy = ScaleHxLy(cellRect);
        Rect cellRectLxHy = ScaleLxHy(cellRect);
        Rect cellRectHxHy = ScaleHxHy(cellRect);

        ImageCoreC<T, 1> cell     = dst    .SubImage(cellRect    .Origin() - rect    .Origin());

        ImageCoreC<T, 1> cellLxLy = srcLxLy.SubImage(cellRectLxLy.Origin() - rectLxLy.Origin());
        ImageCoreC<T, 1> cellHxLy = srcHxLy.SubImage(cellRectHxLy.Origin() - rectHxLy.Origin());
        ImageCoreC<T, 1> cellLxHy = srcLxHy.SubImage(cellRectLxHy.Origin() - rectLxHy.Origin());
        ImageCoreC<T, 1> cellHxHy = srcHxHy.SubImage(cellRectHxHy.Origin() - rectHxHy.Origin());

        unsigned int toBegBorderX = cellRect.X() - rect.X();
        unsigned int toEndBorderX =      rect.X() +     rect.Width()
                                  - (cellRect.X() + cellRect.Width());
        unsigned int toBegBorderY = cellRect.Y() - rect.Y();
        unsigned int toEndBorderY =      rect.Y() +     rect.Height()
                                  - (cellRect.Y() + cellRect.Height());

        Transform(
            cellLxLy,
            cellHxLy,
            cellLxHy,
            cellHxHy,
            cell,
            cellRect,
            toBegBorderX,
            toEndBorderX,
            toBegBorderY,
            toEndBorderY);
    }

protected:
    void Transform(
        const ImageCoreC<T, 1> &srcLxLy,
        const ImageCoreC<T, 1> &srcHxLy,
        const ImageCoreC<T, 1> &srcLxHy,
        const ImageCoreC<T, 1> &srcHxHy,
        const ImageCoreC<T, 1> &dst,
        const Rect             &rect,
        unsigned int            toBegBorderX,
        unsigned int            toEndBorderX,
        unsigned int            toBegBorderY,
        unsigned int            toEndBorderY)
    {
        unsigned int widthL = ScaleL(rect.RangeX()).Size();
        unsigned int widthH = ScaleH(rect.RangeX()).Size();

        int          begOverstepLx = AGREGATE::BegOverstepL(rect.RangeX(), toBegBorderX, toEndBorderX);
        int          begOverstepHx = AGREGATE::BegOverstepH(rect.RangeX(), toBegBorderX, toEndBorderX);

        int          endOverstepLx = AGREGATE::EndOverstepL(rect.RangeX(), toBegBorderX, toEndBorderX);
        int          endOverstepHx = AGREGATE::EndOverstepH(rect.RangeX(), toBegBorderX, toEndBorderX);

        int          begOverstepLy = AGREGATE::BegOverstepL(rect.RangeY(), toBegBorderY, toEndBorderY);
        int          begOverstepHy = AGREGATE::BegOverstepH(rect.RangeY(), toBegBorderY, toEndBorderY);

        RowInput       srcRowLxLy(srcLxLy.SubImage(-begOverstepLx, -begOverstepLy), widthL + begOverstepLx + endOverstepLx);
        RowInput       srcRowHxLy(srcHxLy.SubImage(-begOverstepHx, -begOverstepLy), widthH + begOverstepHx + endOverstepHx);
        RowInput       srcRowLxHy(srcLxHy.SubImage(-begOverstepLx, -begOverstepHy), widthL + begOverstepLx + endOverstepLx);
        RowInput       srcRowHxHy(srcHxHy.SubImage(-begOverstepHx, -begOverstepHy), widthH + begOverstepHx + endOverstepHx);

        RowIterator<T> dstRow  = dst;

        m_rowLy.InitAttach(rect.RangeX(), toBegBorderX, toEndBorderX, srcRowLxLy, srcRowHxLy);
        m_rowHy.InitAttach(rect.RangeX(), toBegBorderX, toEndBorderX, srcRowLxHy, srcRowHxHy);

        m_col  .ReAllocInitAttach(rect, toBegBorderY, toEndBorderY, m_rowLy, m_rowHy);

        for(unsigned int i = 0; i < rect.Height(); i++)
        {
            m_col.NextStep();
            Copy(m_col.CurrOutputRow(), dstRow, rect.Width());
            ++dstRow;
        }
    }

    typename AGREGATE::AGR_WTInvCol  m_col;
    typename AGREGATE::AGR_WTInvRow  m_rowLy;
    typename AGREGATE::AGR_WTInvRow  m_rowHy;
};

template <class AGREGATE>
class WTInvMT
{
protected:
static UIntRange Intersection(const UIntRange &left, const UIntRange &right)
{
    unsigned int origin = Max(left.Origin(), right.Origin());
    unsigned int bound  = Min(left.Bound(), right.Bound());
    if(bound >= origin) return UIntRange(origin, bound - origin);
    else                return UIntRange(0xFFFFFFFF, 0);
}

static Rect Intersection(const Rect &left, const Rect &right)
{
    return Rect(Intersection(left.RangeX(), right.RangeX()), Intersection(left.RangeY(), right.RangeY()));
}

    typedef typename AGREGATE::AGR_WTInvRow::T T;
    typedef typename AGREGATE::AGR_WTInvRow::RowInput RowInput;

    FixedBuffer<WTInvCellMT<AGREGATE> > m_wtCell;
public:
    void ReAlloc(
        const RectSize &maxCellSize,
        unsigned int    maxNOfCells)
    {
        m_wtCell.ReAlloc(maxNOfCells);
        for(unsigned int i = 0; i < maxNOfCells; i++)
            m_wtCell[i].ReAlloc(maxCellSize);
    }

    void Transform(
        const ImageCoreC<T, 1> &srcLxLy,
        const ImageCoreC<T, 1> &srcHxLy,
        const ImageCoreC<T, 1> &srcLxHy,
        const ImageCoreC<T, 1> &srcHxHy,
        const ImageCoreC<T, 1> &dst,
        const Rect             &rect,
        const RectSize         &cellSize)
    {
        Point cellGridOrigin(rect.Origin().X(), rect.Origin().Y());
        unsigned int nOfCols = DivCeil(rect.Origin().X() + rect.Width () - cellGridOrigin.X(), cellSize.Width ());
        unsigned int nOfRows = DivCeil(rect.Origin().Y() + rect.Height() - cellGridOrigin.Y(), cellSize.Height());


        int nOfCells = nOfCols * nOfRows;
        FixedBuffer<Rect> cellRect(nOfCells);

        int cellIndex = 0;
        for(unsigned int row = 0; row < nOfRows; row++)
        for(unsigned int col = 0; col < nOfCols; col++)
        {
            cellRect[cellIndex] = Intersection(rect, Rect(cellGridOrigin + RectSize(col, row) * cellSize, cellSize));
            cellIndex++;
        }

#pragma  omp parallel for
        for(cellIndex = 0; cellIndex < nOfCells; cellIndex++)
        {
            m_wtCell[cellIndex].Transform(
                srcLxLy,
                srcHxLy,
                srcLxHy,
                srcHxHy,
                dst,
                rect,
                cellRect[cellIndex]);
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

        WTInvMT<AGREGATE> inv;

        unsigned int maxNOfCellsX = 4;
        unsigned int maxNOfCellsY = 4;
        unsigned int maxNOfCells = maxNOfCellsX * maxNOfCellsY;
        RectSize     cellSize(Max<int>(256, rect.Width() / (maxNOfCellsX - 2)), Max<int>(256, rect.Height() / (maxNOfCellsY - 2)));

        cellSize.SetWidth(cellSize.Width () + (cellSize.Width () & 1));
        cellSize.SetWidth(cellSize.Height() + (cellSize.Height() & 1));

        inv.ReAlloc(cellSize, maxNOfCells);

        int level = src.NOfWTLevels() - 1;

        if(src.NOfWTLevels()==1)
        {
            inv.Transform(
                src.LxLy(),
                src.HxLy(level),
                src.LxHy(level),
                src.HxHy(level),
                dst, ScaleLxLy(rect, level), cellSize);

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
                dst, ScaleLxLy(rect, level), cellSize);

            level--;

            for(; level >= 0; )
            {
                inv.Transform(
                    dst,
                    src.HxLy(level),
                    src.LxHy(level),
                    src.HxHy(level),
                    tmp.Core(), ScaleLxLy(rect, level), cellSize);

                level--;

                inv.Transform(
                    tmp.Core(),
                    src.HxLy(level),
                    src.LxHy(level),
                    src.HxHy(level),
                    dst, ScaleLxLy(rect, level), cellSize);

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
                tmp.Core(), ScaleLxLy(rect, level), cellSize);

            level--;

            for(; level >= 1; )
            {
                inv.Transform(
                    tmp.Core(),
                    src.HxLy(level),
                    src.LxHy(level),
                    src.HxHy(level),
                    dst, ScaleLxLy(rect, level), cellSize);

                level--;

                inv.Transform(
                    dst,
                    src.HxLy(level),
                    src.LxHy(level),
                    src.HxHy(level),
                    tmp.Core(), ScaleLxLy(rect, level), cellSize);

                level--;
            }

            inv.Transform(
                tmp.Core(),
                src.HxLy(level),
                src.LxHy(level),
                src.HxHy(level),
                dst, ScaleLxLy(rect, level), cellSize);
        }
    }
};

//////////////////////////////////////////////////////////////////////////
//
// Auxiliary names to call multi-level 2D WT
//
/*
template<class T>
void WT53Inv(const SBTree<T> &src, const ImageCoreC<T, 1> &dst, const Rect &rect)
{
    WTInv<WT53InvAgregateMT<T, ImageRowInput<T, 1> > >::Transform(src, dst, rect);
}
*/

template<class T>
void WT97InvMT(const SBTree<T> &src, const ImageCoreC<T, 1> &dst, const Rect &rect)
{
    WTInvMT<WT97InvAgregateMT<T, ImageRowInput<T, 1> > >::Transform(src, dst, rect);
}


#endif // __WTINVMT_H__
