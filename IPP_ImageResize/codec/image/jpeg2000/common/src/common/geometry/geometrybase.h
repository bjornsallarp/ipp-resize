
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

#ifndef __GEOMETRYBASE_H__
#define __GEOMETRYBASE_H__

//
// Generalization of "size + origin" matter.
//
template<class TOrg, class TSiz> class SpatialSize
{
public:
    SpatialSize() {}
    SpatialSize(const TOrg &origin, const TSiz &size) : m_origin(origin), m_size(size) {}

    void  SetOrigin(const TOrg &origin) { m_origin = origin; }
    void  SetSize  (const TSiz &size)   { m_size   = size;   }

    const TOrg &Origin() const { return m_origin; }
    const TSiz &Size  () const { return m_size; }

protected:
    TOrg m_origin;
    TSiz m_size;
};

#endif // __GEOMETRYBASE_H__
