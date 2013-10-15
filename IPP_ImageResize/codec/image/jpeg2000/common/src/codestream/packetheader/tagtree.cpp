
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

#include "tagtree.h"
#include "genalg.h"
#include "wtmetric.h"


inline unsigned int NOfNodes(unsigned int width, unsigned int height)
{
    unsigned int count = 0;
    unsigned int level = 0;
    unsigned int n;

    do
    {
        n = RShiftCeil(width, level) * RShiftCeil(height, level);
        count += n;
        level++;
    }
    while (n > 1);

    return count;
}

inline unsigned int NOfLevels(unsigned int width, unsigned int height)
{
    if(width*height <= 1) return 0;
    return MaxNOfWTLevels(Max(width, height));
}



TagTree::TagTree()
: m_width(0)
, m_height(0)
{}

void TagTree::Init(const RectSize &size)
{
    m_width  = size.Width();
    m_height = size.Height();

    m_nodes.ReAlloc(NOfNodes(m_width, m_height));

    Node *node    = m_nodes;
    Node *parent  = &m_nodes[m_width * m_height];
    Node *parent0 = parent;

    unsigned int nOfLevels = NOfLevels(m_width, m_height);

    for (unsigned int level = 0; level < nOfLevels; level++)
    {
        unsigned int levelHeight = RShiftCeil(m_height, level);
        unsigned int levelWidth  = RShiftCeil(m_width,  level);

        for (unsigned int j = 0; j < levelHeight; j++)
        {
            for (unsigned int k = 0; k < levelWidth; k++)
            {
                node->m_parent = parent;
                ++node;

                if ((k & 1) || k + 1 == levelWidth)
                {
                    parent++;
                }
            }

            if ((j & 1) || j + 1 == levelHeight)
            {
                parent0 = parent;
            }
            else
            {
                parent = parent0;
                parent0 += levelWidth;
            }
        }
    }
}

