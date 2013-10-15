
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

#ifndef __DCBLAYER_H__
#define __DCBLAYER_H__

#include "list_.h"

class DCBSegment
{
public:
    DCBSegment() : m_size(0), m_nOfPasses(0) {}

    DCBSegment(unsigned int size, unsigned int nOfPasses)
    : m_size     (size     )
    , m_nOfPasses(nOfPasses)
    {}

    unsigned int NOfPasses() const { return m_nOfPasses; }
    unsigned int Size     () const { return m_size     ; }

protected:
    unsigned int m_size;
    unsigned int m_nOfPasses;
};

class DCBLayer
{
protected:

public:
    DCBLayer() : m_nOfPasses(0), m_size(0) {}

    void Init()
    {
        m_segment.Clear();
        m_nOfPasses = 0;
        m_size      = 0;
    }

    void SetNextSegmentSize(unsigned int size, unsigned int nOfPasses)
    {
        m_segment.PushBack();
        m_segment.Back    () = DCBSegment(size, nOfPasses);
        m_size      += size;
        m_nOfPasses += nOfPasses;
    }

    unsigned int            NOfPasses() const { return m_nOfPasses; }
    unsigned int            Size     () const { return m_size     ; }

    const List<DCBSegment> &Segment  () const { return m_segment  ; }

protected:
    List<DCBSegment> m_segment;
    unsigned int     m_nOfPasses;
    unsigned int     m_size;
};

#endif // __DCBLAYER_H__
