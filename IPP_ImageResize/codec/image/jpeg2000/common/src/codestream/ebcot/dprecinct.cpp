
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

#include "dprecinct.h"
#include "genalg.h"

unsigned int DPrecinct::SegmentMaxNOfPasses(unsigned int startPass, bool isSelectiveMQBypass, bool isTermOnEveryPass)
{
    if (isTermOnEveryPass) return 1;

    if (isSelectiveMQBypass)
    {
        if (startPass < 10) return 10 - startPass;

        // if pass is significance propagation pass return 2, else return 1
        if(startPass % 3 == 1) return 2;
        return 1;
    }

    return 32 * 3 - 2; // 32 - bit precision
}

void DPrecinct::Init(
    const ImageCore32sC1 &precinct,
    const Rect           &rect,
    const RectSize       &cbStepOrder,
    unsigned int          nOfLayers,
    IppiWTSubband         subband,
    unsigned int          cmpGuardBits,
    unsigned int          sbDynRange,
    unsigned int          sbLowestBitOffset,
    const CBCodingStyle  &codingStyle)
{
    GridPow2 cbGrid;
    cbGrid.Init(rect, cbStepOrder);

    unsigned int nOfCbs = cbGrid.NOfCells();
    m_cbs.ReAlloc(nOfCbs);

    GridPow2RelativeIterator ii(cbGrid);
    for(unsigned int cb = 0; cb < nOfCbs; cb++)
    {
        ImageCore32sC1 cbImg = precinct.SubImage(ii->Origin());

        m_cbs[cb].Init(
            cbImg,
            ii->Size(),
            nOfLayers,
            subband,
            cmpGuardBits,
            sbDynRange,
            sbLowestBitOffset,
            codingStyle);
        ++ii;
    }

    m_inclTagTree.Init(cbGrid.SizeRowCol());
    m_zbplTagTree.Init(cbGrid.SizeRowCol());

    m_isSelectiveMQBypass = codingStyle.IsSelectiveMQBypass();
    m_isTermOnEveryPass   = codingStyle.IsTerminateOnEveryPass();

#ifdef _OPENMP
    m_cbBody.ReAlloc(m_cbs.Size());
#endif
}

