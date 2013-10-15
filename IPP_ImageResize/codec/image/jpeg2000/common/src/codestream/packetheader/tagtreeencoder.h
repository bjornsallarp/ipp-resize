
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
//  TagTreeEncoder implements tag tree coding system defined
//      in B.10.2 article of standard. It is used for packed header writing.
//
//
//
*/

#ifndef __TAGTREEENCODER_H__
#define __TAGTREEENCODER_H__

#include "phbitout.h"
#include "tagtree.h"

class TagTreeEncoder : public TagTree
{
public:
    TagTreeEncoder();
    TagTreeEncoder(const TagTreeEncoder& tt);
    TagTreeEncoder& operator=(const TagTreeEncoder& tt);

    void SetValue(unsigned int n, int value) const;

    template<class BitOutput>
        bool Encode(unsigned int n, int threshold, BitOutput &out) const
    {
        Node *stk[32 - 1];
        Node **stkptr = stk;
        Node *node = &m_nodes[n];

        while (node->m_parent)
        {
            *stkptr++ = node;
            node = node->m_parent;
        }

        int low = 0;
        for (;;)
        {
            if (low > node->m_state) node->m_state = low;
            else                     low = node->m_state;

            while (low < threshold)
            {
                if (low >= node->m_value)
                {
                    if (!node->m_isKnown)
                    {
                        out.Write1();
                        node->m_isKnown = 1;
                    }
                    break;
                }
                out.Write0();
                low++;
            }
            node->m_state = low;

            if (stkptr == stk) break;

            node = *--stkptr;

        }
        return node->m_state < threshold;
    }

};

#endif // __TAGTREEENCODER_H__

