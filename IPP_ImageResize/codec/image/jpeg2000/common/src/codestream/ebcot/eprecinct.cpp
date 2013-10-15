
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

#include <cfloat>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "eprecinct.h"

void EPrecinct::Init(const Rect &rect, int cbXStepOrder, int cbYStepOrder)
{
    m_cbGrid.Init(rect, RectSize(cbXStepOrder, cbYStepOrder));

    m_cbs.ReAlloc(m_cbGrid.NOfCells());

    m_inclTagTree.Init(m_cbGrid.SizeRowCol());
    m_zbplTagTree.Init(m_cbGrid.SizeRowCol());


#ifdef _OPENMP

    unsigned int nOfCbs = NOfCBs();
    m_cbRect.ReAlloc(nOfCbs);

    GridPow2RelativeIterator ii(m_cbGrid);
    for(unsigned int cbIndx = 0; cbIndx < nOfCbs; cbIndx++)
    {
        m_cbRect[cbIndx] = ii;
        ++ii;
    }

#endif

}

void EPrecinct::ScaleDist(double factor)
{
    for(int i = 0; i < NOfCBs(); i++)
        m_cbs[i].ScaleDist(factor);
}

void EPrecinct::CalcRDSlopes()
{
    for(int i = 0; i < NOfCBs(); i++)
        m_cbs[i].CalcRDSlopes();
}

void EPrecinct::RDSlopesMinMax(double &min, double &max) const
{
    min = DBL_MAX;
    max = 0;

    for(int i = 0; i < NOfCBs(); i++)
    {
        double cbMin;
        double cbMax;

        m_cbs[i].RDSlopesMinMax(cbMin, cbMax);

        if(cbMax > max) max = cbMax;
        if(cbMin < min) min = cbMin;
    }
}

void EPrecinct::AssignLayerTillCurPass(int layer, double threshold)
{
    for(int i = 0; i < NOfCBs(); i++)
        m_cbs[i].AssignLayerTillCurPass(layer, threshold);
}

void EPrecinct::AssignLayerTillCurPass(int layer)
{
    for(int i = 0; i < NOfCBs(); i++)
        m_cbs[i].AssignLayerTillCurPass(layer);
}

void EPrecinct::StoreEncodingState()
{
    m_inclTagTreeStore = m_inclTagTree;
    m_zbplTagTreeStore = m_zbplTagTree;

    for(int cbIndx = 0; cbIndx < NOfCBs(); cbIndx++)
        m_cbs[cbIndx].StoreEncodingState();
}

void EPrecinct::RestoreEncodingState()
{
    for(int cbIndx = 0; cbIndx < NOfCBs(); cbIndx++)
        m_cbs[cbIndx].RestoreEncodingState();

    m_inclTagTree = m_inclTagTreeStore;
    m_zbplTagTree = m_zbplTagTreeStore;
}

void EPrecinct::SetTagTrees(int layer)
{
    for(int cbIndx = 0; cbIndx < NOfCBs(); cbIndx++)
    {
        ECodeBlock &cb = m_cbs[cbIndx];

        if (layer==0)
            m_zbplTagTree.SetValue(cbIndx, cb.NOfZeroBits());

        if (cb.IsIncluded(layer) && cb.IsFirstInclusion())
            m_inclTagTree.SetValue(cbIndx, layer);
    }
}

void EPrecinct::SetCurPassToFirst()
{
    for(int cbIndx = 0; cbIndx < NOfCBs(); cbIndx++)
    {
        m_cbs[cbIndx].SetCurPassToFirst();
    }
}

void EPrecinct::Encode(
          CBEncoder      *cbEncoder,
    const ImageCore32sC1 &precinct,
          IppiWTSubband   subband,
          int             sbDynRange,
          int             sbLowestBitOffset
)
{
#ifdef _OPENMP
    int nOfCbs = NOfCBs();

    int cbIndx;
/*
#pragma  omp parallel for
    for(cbIndx = 0; cbIndx < nOfCbs; cbIndx++)
    {
        int thread = omp_get_thread_num();
        cbEncoder[thread].Encode(precinct, m_cbRect[cbIndx], subband, sbDynRange, sbLowestBitOffset, m_cbs[cbIndx]);
    }
*/

    cbIndx = 0;
#pragma omp parallel shared(cbIndx)
    {
    int cbIndxThr;
loop_start:
#pragma omp critical
    {
    cbIndxThr = cbIndx;
    cbIndx++;
    }
    if(! (cbIndxThr < nOfCbs) ) goto loop_end;
    cbEncoder[omp_get_thread_num()].Encode(precinct, m_cbRect[cbIndxThr], subband, sbDynRange, sbLowestBitOffset, m_cbs[cbIndxThr]);
    goto loop_start;
loop_end:
            ;
    }

#else  // _OPENMP
    GridPow2RelativeIterator ii(m_cbGrid);
    for(int i = 0; i < NOfCBs(); i++)
    {
        cbEncoder->Encode(precinct, ii, subband, sbDynRange, sbLowestBitOffset, m_cbs[i]);
        ++ii;
    }
#endif // _OPENMP
}
