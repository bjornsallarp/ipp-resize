
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

#ifndef __GEOMETRY1D_H__
#define __GEOMETRY1D_H__

#include "geometrybase.h"

template<class T> class ROpenInterval : public SpatialSize<T, T>
{
public:
    ROpenInterval() {}

    ROpenInterval(const T &origin, const T &size)
    : SpatialSize<T, T> (origin, size)
    , m_bound (origin + size)
    {}

    void  SetOrigin (const T &origin) { SpatialSize<T, T>::SetOrigin(origin); m_bound = SpatialSize<T, T>::Origin() + SpatialSize<T, T>::Size(); }
    void  SetSize   (const T &size)   { SpatialSize<T, T>::SetSize  (size)  ; m_bound = SpatialSize<T, T>::Origin() + SpatialSize<T, T>::Size(); }

    const T& Bound() const { return m_bound; }

protected:
    T m_bound;
};

typedef ROpenInterval<unsigned int> UIntRange;

#endif // __GEOMETRY1D_H__
