
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
//  TagTree class encapsulate building of tag tree in memory
//      space. It is used in tag tree coding/decoding systems defined
//      in B.10.2 article of standard.
//
//
//
*/

#ifndef __TAGTREE_H__
#define __TAGTREE_H__

#include "fixedarray.h"
#include "geometry2d.h"

class TagTree
{
public:
    TagTree();

    void Init(const RectSize &size);

protected:
    class Node
    {
    public:
        Node() : m_parent(0) { Reset(); }

        void Reset() {m_value = 0x7FFFFFFF; m_state = 0; m_isKnown = 0;}

        void CopyContent(const Node &node)
        {
            m_value   = node.m_value;
            m_state   = node.m_state;
            m_isKnown = node.m_isKnown;
        }

        Node *m_parent;

        int m_value;
        int m_state; // lower bound on the value associated with this node
        int m_isKnown;
    };

    unsigned int      m_width;
    unsigned int      m_height;
    FixedArray<Node>  m_nodes;
};


#endif // __TAGTREE_H__

