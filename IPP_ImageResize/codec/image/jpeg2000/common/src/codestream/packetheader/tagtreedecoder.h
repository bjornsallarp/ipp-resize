
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

#ifndef __TAGTREEDECODER_H__
#define __TAGTREEDECODER_H__

#include "phbitin.h"
#include "tagtree.h"

class TagTreeDecoder : public TagTree
{
public:
    TagTreeDecoder() : TagTree()
    {}

    template<class ByteInput>
        bool Decode(PHBitIn<ByteInput> &in, unsigned int n, int threshold)
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

            while (low < threshold && low < node->m_value)
            {
                if (in.Read()) node->m_value = low;
                else           low++;
            }
            node->m_state = low;

            if (stkptr == stk) break;

            node = *--stkptr;
        }

        return (node->m_value < threshold);
    }
};

#endif // __TAGTREEDECODER_H__

