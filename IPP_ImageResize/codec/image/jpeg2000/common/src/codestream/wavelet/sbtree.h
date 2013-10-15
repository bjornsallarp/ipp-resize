
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
//  SBTree is "subband tree" class for storing of and access to
//      wavelet components of multilevel wavelet transform.
//
//
//
*/

#ifndef __SBTREE_H__
#define __SBTREE_H__

#include "image.h"
#include "wtmetric.h"

template <class T> class SBTriplet
{
public:
    typedef ImageC<T, 1> Image;

    SBTriplet() {};

    SBTriplet(const Rect &parentRect)
    : m_LxHy(ScaleLxHy(parentRect).Size())
    , m_HxLy(ScaleHxLy(parentRect).Size())
    , m_HxHy(ScaleHxHy(parentRect).Size())
    {}

    void ReAlloc(const Rect &parentRect)
    {
        m_LxHy.ReAlloc(ScaleLxHy(parentRect).Size());
        m_HxLy.ReAlloc(ScaleHxLy(parentRect).Size());
        m_HxHy.ReAlloc(ScaleHxHy(parentRect).Size());
    }

    void Free()
    {
        m_LxHy.Free();
        m_HxLy.Free();
        m_HxHy.Free();
    }

    const Image &HxLy() const { return m_HxLy; }
    const Image &LxHy() const { return m_LxHy; }
    const Image &HxHy() const { return m_HxHy; }

protected:
    Image m_HxLy;
    Image m_LxHy;
    Image m_HxHy;
};

template <class T> class SBTree
{
public:
    typedef FixedBuffer<SBTree<T> > Array;

    typedef ImageC<T, 1> Image;

    SBTree() : m_nOfWTLevels(0) {}

    SBTree(const Rect &rect, unsigned int nOfWTLevels)
    {
        ReAlloc(rect, nOfWTLevels);
    }

    void ReAlloc(const Rect &rect, unsigned int nOfWTLevels)
    {
        m_nOfWTLevels = nOfWTLevels;
        m_triplets.ReAlloc(m_nOfWTLevels);

        for(unsigned int i = 0; i < nOfWTLevels; i++)
            m_triplets[i].ReAlloc(ScaleLxLy(rect, i));

        m_LxLy.ReAlloc(ScaleLxLy(rect, nOfWTLevels).Size());
    }

    void Free()
    {
        m_LxLy    .Free();
        m_triplets.Free();
        m_nOfWTLevels = 0;
    }

    const Image &LxLy()            const { return m_LxLy; }
    const Image &HxLy(int sbDepth) const { return m_triplets[sbDepth].HxLy(); }
    const Image &LxHy(int sbDepth) const { return m_triplets[sbDepth].LxHy(); }
    const Image &HxHy(int sbDepth) const { return m_triplets[sbDepth].HxHy(); }

    const SBTriplet<T> &Triplet(int sbDepth) const { return m_triplets[sbDepth]; }

    unsigned int NOfWTLevels() const {return m_nOfWTLevels;}

protected:
    unsigned int                m_nOfWTLevels;
    FixedBuffer< SBTriplet<T> > m_triplets;
    Image                       m_LxLy;
};


#endif // __SBTREE_H__

