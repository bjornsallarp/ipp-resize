
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

#ifndef __BCODEBLOCK_H__
#define __BCODEBLOCK_H__



#include "image.h"

class BCodeBlock {
public:
    BCodeBlock()
    : m_nOfPasses(0), m_nOfZeroBits(0)
    {}

    int NOfPasses()   const { return m_nOfPasses; }
    int NOfZeroBits() const { return m_nOfZeroBits; }

protected:
    int m_nOfPasses;
    int m_nOfZeroBits;
};

#endif // __BCODEBLOCK_H__

