
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

/*
//  This file contains two classes RulerPow2 and GridPow2.
//          The RulerPow2 class implements simply one-dimensional case of grid
//      formed by equal segments of size represented by power 2 (it looks like a ruller).
//          The GridPow2 implements two-dimensional case of grid with
//      rectangular cells of equal size of width and height represented by power 2.
//          In this JPEG 2000 example the RectGridPow2 class used to work
//      with  codeblocks and precincts.
//
//
//
*/

#ifndef __RECTGRID_H__
#define __RECTGRID_H__

#include "auxmath.h"
#include "geometry2d.h"


// Ruler starts from ZERO point, but first segment start from origin.
class RulerPow2 {
public:
    RulerPow2() { Init(0, 0, 0); }

    RulerPow2(unsigned int origin, unsigned int size, unsigned int stepOrder)
    {
        Init(origin, size, stepOrder);
    }

    void Init(unsigned int origin, unsigned int size, unsigned int stepOrder)
    {
        m_origin        = origin;
        m_step          = 1u << stepOrder;
        m_stepOrder     = stepOrder;
        m_originReduced = RShiftFloor(origin, stepOrder);

        m_nOfSegments   = size ? RShiftCeil(size + m_origin, stepOrder) - RShiftFloor(origin, stepOrder): 0;

        if(m_nOfSegments <= 1)
        {
            m_firstSegmSize = size;
            m_lastSegmSize  = 0;
            return;
        }

        unsigned int firstMark = (m_originReduced + 1) * m_step;

        m_firstSegmSize = firstMark - m_origin;
        m_lastSegmSize  = size - (m_nOfSegments-2u) * m_step - m_firstSegmSize;
    }

    unsigned int NOfSegments() const { return m_nOfSegments; }

    unsigned int StepOrder()   const { return m_stepOrder; }

    unsigned int SegmIndex(unsigned int position) const
    {
        return RShiftFloor(position, m_stepOrder) - m_originReduced;
    }

    unsigned int SegmIndexRelative(unsigned int relPosition) const
    {
        return SegmIndex(relPosition + m_origin);
    }

    unsigned int SegmOrigin(unsigned int index) const
    {
        return m_origin + SegmOriginRelative(index);
    }

    unsigned int SegmOriginRelative(unsigned int index) const
    {
        if(index == 0) return 0;
        int origin1 = m_firstSegmSize;
        return(origin1 + (index - 1u) * m_step);
    }

    unsigned int SegmSize(unsigned int index) const
    {
        if(index == 0)                  return m_firstSegmSize;
        if(index == m_nOfSegments - 1u) return m_lastSegmSize;
        return m_step;
    }

protected:
    unsigned int m_origin;
    unsigned int m_originReduced;
    unsigned int m_step;
    unsigned int m_stepOrder;
    unsigned int m_nOfSegments;
    unsigned int m_firstSegmSize;
    unsigned int m_lastSegmSize;
};


// Grid subdivision starts from (0,0) point, but origin specifies start of
// image data in this grid.
class GridPow2 {
public:
    GridPow2() : m_nOfCells(0) {}

    GridPow2(const Rect &rect, const RectSize &stepOrder)
    {
        Init(rect, stepOrder);
    }

    void Init(const Rect &rect, const RectSize &stepOrder)
    {
        m_xRuler.Init(rect.X(), rect.Width(),  stepOrder.Width());
        m_yRuler.Init(rect.Y(), rect.Height(), stepOrder.Height());
        m_nOfCells = m_xRuler.NOfSegments() * m_yRuler.NOfSegments();
    }

    unsigned int NOfCells() const { return m_nOfCells; }

    unsigned int NOfRows() const { return m_yRuler.NOfSegments(); }
    unsigned int NOfCols() const { return m_xRuler.NOfSegments(); }

    RectSize SizeRowCol() const { return RectSize(NOfCols(), NOfRows()); }
    RectSize StepOrder()  const { return RectSize(m_xRuler.StepOrder(), m_yRuler.StepOrder()); }

    unsigned int CellRastNum(const Point &point) const { return m_xRuler.SegmIndex(point.X()) + m_yRuler.SegmIndex(point.Y()) * NOfRows(); }

    Point CellRowCol(const Point &point) const
    {
        return Point(m_xRuler.SegmIndex(point.X()),
                     m_yRuler.SegmIndex(point.Y()));
    }

    Point CellRowColRelative(const Point &relPoint) const
    {
        return Point(m_xRuler.SegmIndexRelative(relPoint.X()),
                     m_yRuler.SegmIndexRelative(relPoint.Y()));
    }

    Rect CellRect(const Point &rowCol) const
    {
        return Rect(Point   ( m_xRuler.SegmOrigin(rowCol.X()), m_yRuler.SegmOrigin(rowCol.Y())  ),
                    RectSize( m_xRuler.SegmSize  (rowCol.X()), m_yRuler.SegmSize  (rowCol.Y())) );
    }

    Rect CellRectRelative(const Point &rowCol) const
    {
        return Rect(Point   ( m_xRuler.SegmOriginRelative(rowCol.X()), m_yRuler.SegmOriginRelative(rowCol.Y())  ),
                    RectSize( m_xRuler.SegmSize          (rowCol.X()), m_yRuler.SegmSize          (rowCol.Y())) );
    }

protected:
    RulerPow2 m_xRuler;
    RulerPow2 m_yRuler;

    unsigned int m_nOfCells;
};

class GridPow2RelativeIterator {
public:
    GridPow2RelativeIterator(const GridPow2 &grid)
    : m_grid(&grid)
    , m_row(0)
    , m_col(0)
    {
        SetCurCell();
    }

    void operator++()
    {
        m_col++;
        if(m_col >= m_grid->NOfCols())
        {
            m_col = 0;
            m_row++;
        }
        SetCurCell();
    }

    operator const Rect& ()  const { return m_cell; }
    const Rect* operator->() const { return &m_cell; }

    unsigned int Row() const { return m_row; }
    unsigned int Col() const { return m_col; }

    bool IsInBounds() const
    {
        return m_row < m_grid->NOfRows() && m_col < m_grid->NOfCols();
    }

protected:
    void SetCurCell() { m_cell = m_grid->CellRectRelative(Point(m_col, m_row)); }

    const GridPow2 *m_grid;
    Rect            m_cell;

    unsigned int    m_row;
    unsigned int    m_col;
};



#endif // __RECTGRID_H__

