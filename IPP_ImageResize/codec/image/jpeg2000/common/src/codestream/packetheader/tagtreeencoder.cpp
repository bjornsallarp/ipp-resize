
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

#include "tagtreeencoder.h"

TagTreeEncoder::TagTreeEncoder()
: TagTree()
{}

TagTreeEncoder::TagTreeEncoder(const TagTreeEncoder& tt)
{
    Init(RectSize(tt.m_width, tt.m_height));
    for(unsigned int i = 0; i < m_nodes.Size(); i++)
        m_nodes[i].CopyContent(tt.m_nodes[i]);
}

TagTreeEncoder& TagTreeEncoder::operator=(const TagTreeEncoder& tt)
{
    if(&tt != this)
    {
        Init(RectSize(tt.m_width, tt.m_height));

        for(unsigned int i = 0; i < m_nodes.Size(); i++)
            m_nodes[i].CopyContent(tt.m_nodes[i]);
    }
    return *this;
}

void TagTreeEncoder::SetValue(unsigned int n, int value) const
{
    Node *node = &m_nodes[n];

    while (node && node->m_value > value)
    {
        node->m_value = value;
        node = node->m_parent;
    }
}


