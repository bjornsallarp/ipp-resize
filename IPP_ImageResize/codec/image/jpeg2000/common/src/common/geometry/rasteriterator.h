
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2003-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __RASTERITERATOR_H__
#define __RASTERITERATOR_H__

#include "auxmath.h"
#include "boundediterator.h"
#include "geometry2d.h"


class RasterIterator
{
public:
    RasterIterator() : m_nOfCols(0), m_x(0, 0, 1, 0), m_y(0, 0, 1, 0) {}

    RasterIterator(const Rect &rectRange, const RectSize &step)
    {
        m_x = Iterator1D(rectRange.Origin().X(), rectRange.Width() , step.Width(),  0);
        m_y = Iterator1D(rectRange.Origin().Y(), rectRange.Height(), step.Height(), 0);

        m_nOfCols = DivCeil(rectRange.Width(), step.Width());
    }

    RasterIterator(const RasterIterator &iter)
    : m_x(iter.m_x)
    , m_y(iter.m_y)
    , m_nOfCols(iter.m_nOfCols)
    {}

    RasterIterator& operator=(const RasterIterator& iter)
    {
        if(this != &iter)
        {
            m_x       = iter.m_x;
            m_y       = iter.m_y;
            m_nOfCols = iter.m_nOfCols;
        }
        return *this;
    }

    bool IsInRange() const { return m_y.IsInRange(); }

    void operator++()
    {
        m_x++;
        if(!m_x.IsInRange())
        {
            m_y++;
            m_x.Restart();
        }
    }
    void operator++(int) { ++(*this); }

    operator Point()  const { return  Point(m_x, m_y); }
    operator unsigned int() const {
        return m_y.Index() * m_nOfCols + m_x.Index(); }

    void Restart() { m_x.Restart(); m_y.Restart(); }

    void Restart(const Rect &rectRange)
    {
        m_x.Restart(rectRange.Origin().X(), rectRange.Width ());
        m_y.Restart(rectRange.Origin().Y(), rectRange.Height());
        m_nOfCols = DivCeil(rectRange.Width(), m_x.Step());
    }

    void Restart(const RectSize &step)
    {
        m_x.Restart(step.Width());
        m_y.Restart(step.Height());
        m_nOfCols = DivCeil(m_x.Range().Size(), step.Width());
    }

protected:
    typedef BoundedFwdStepIndexIterator<unsigned int, unsigned int> Iterator1D;

    Iterator1D   m_x;
    Iterator1D   m_y;

    unsigned int m_nOfCols;
};

#endif // __RASTERITERATOR_H__
