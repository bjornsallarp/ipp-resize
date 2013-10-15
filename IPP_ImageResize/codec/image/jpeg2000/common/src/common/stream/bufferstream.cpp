
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#include "bufferstream.h"

const unsigned int MemoryBlock::m_size = 512;

unsigned int Transfer(MemoryBlock& src, MemoryBlock& dst, unsigned int size)
{
    unsigned int actualSize = Min(Min(dst.TailSize(), size), src.FillSize());

    Copy(src.m_rPos, dst.m_wPos, actualSize);

    src.m_rPos += actualSize;
    dst.m_wPos += actualSize;

    return actualSize;
}

unsigned int Transfer(BufferStream& src, BufferStream& dst, unsigned int size)
{
    unsigned int usedSize = 0;

    while(usedSize < size)
    {
        if( ! dst.BackBlock().TailSize())
            dst.PushBackBlock();

        unsigned int blockUsedSize
            = Transfer(src.FrontBlock(), dst.BackBlock(), size - usedSize);

        if(!blockUsedSize)
        {
            if(src.IsSingleBlock()) break;
            src.PopFrontBlock();
        }
        else usedSize += blockUsedSize;
    }
    src.m_size -= usedSize;
    dst.m_size += usedSize;

    return usedSize;
}


