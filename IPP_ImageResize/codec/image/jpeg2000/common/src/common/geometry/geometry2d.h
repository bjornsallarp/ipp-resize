
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

#ifndef __GEOMETRY2D_H__
#define __GEOMETRY2D_H__

#include "ippdefs.h"
#include "geometry1d.h"

template<class T> class Point2D
{
public:
    Point2D()         : m_x(0), m_y(0) {}
    Point2D(T x, T y) : m_x(x), m_y(y) {}

    void SetX(T x) { m_x = x; }
    void SetY(T y) { m_y = y; }

    const T &X() const { return m_x; }
    const T &Y() const { return m_y; }

protected:
    T m_x;
    T m_y;
};

template<class T>
Point2D<T> operator - (const Point2D<T> &left, const Point2D<T> &right)
{
    return Point2D<T>(left.X() - right.X(), left.Y() - right.Y());
}

template<class T>
Point2D<T> operator + (const Point2D<T> &left, const Point2D<T> &right)
{
    return Point2D<T>(left.X() + right.X(), left.Y() + right.Y());
}

template<class T>
bool operator==(const Point2D<T> &left, const Point2D<T> &right)
{
    return (left.X() == right.X() && left.Y() == right.Y());
}

template<class T>
bool operator!=(const Point2D<T> &left, const Point2D<T> &right)
{
    return (left.X() != right.X() || left.Y() != right.Y());
}

typedef Point2D<unsigned int> Point;

class RectSize
{
public:
    RectSize()                                        : m_width(0),     m_height(0)      {}
    RectSize(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}

    void SetWidth (unsigned int width)  { m_width  = width; }
    void SetHeight(unsigned int height) { m_height = height; }

    unsigned int Width()  const { return m_width;}
    unsigned int Height() const { return m_height;}

    operator IppiSize()   const { IppiSize size = {m_width, m_height}; return size; }

protected:
    unsigned int m_width;
    unsigned int m_height;
};


inline RectSize operator - (const RectSize &left, const RectSize &right)
{
    return RectSize(left.Width() - right.Width(), left.Height() - right.Height());
}

inline RectSize operator + (const RectSize &left, const RectSize &right)
{
    return RectSize(left.Width() + right.Width(), left.Height() + right.Height());
}

inline RectSize operator * (const RectSize &left, const RectSize &right)
{
    return RectSize(left.Width() * right.Width(), left.Height() * right.Height());
}

inline RectSize operator * (const RectSize &left, unsigned int &right)
{
    return RectSize(left.Width() * right, left.Height() * right);
}

inline RectSize operator * (unsigned int &left, const RectSize &right)
{
    return RectSize(left * right.Width(), left * right.Height());
}

inline RectSize operator << (unsigned int left, const RectSize &right)
{
    return RectSize(left << right.Width(), left << right.Height());
}

inline bool operator == (const RectSize &left, const RectSize &right)
{
    return left.Width() == right.Width() && left.Height() == right.Height();
}

inline bool operator != (const RectSize &left, const RectSize &right)
{
    return !(left == right);
}

inline Point operator - (const Point &left, const RectSize &right)
{
    return Point(left.X() - right.Width(), left.Y() - right.Height());
}

inline Point operator + (const Point &left, const RectSize &right)
{
    return Point(left.X() + right.Width(), left.Y() + right.Height());
}

inline Point operator * (const Point &left, const RectSize &right)
{
    return Point(left.X() * right.Width(), left.Y() * right.Height());
}

inline Point operator * (const RectSize &left, const Point &right)
{
    return Point(left.Width() * right.X(), left.Height() * right.Y());
}

inline Point operator / (const Point &left, const RectSize &right)
{
    return Point(left.X() / right.Width(), left.Y() / right.Height());
}


class Rect : public SpatialSize<Point, RectSize> // origin, size inheritance
{
public:
    Rect() {}
    Rect(const Point &origin, const RectSize &size)        : SpatialSize<Point, RectSize>(origin, size) {}
    Rect(const UIntRange &rangeX, const UIntRange &rangeY)
    : SpatialSize<Point, RectSize>(
        Point   (rangeX.Origin(), rangeY.Origin()),
        RectSize(rangeX.Size  (), rangeY.Size  ()))
    {}

    unsigned int X     () const { return m_origin.X();    }
    unsigned int Y     () const { return m_origin.Y();    }

    unsigned int Width () const { return m_size.Width();  }
    unsigned int Height() const { return m_size.Height(); }

    UIntRange    RangeX() const { return UIntRange(X(), Width ()); }
    UIntRange    RangeY() const { return UIntRange(Y(), Height()); }
};


#endif // __GEOMETRY2D_H__

