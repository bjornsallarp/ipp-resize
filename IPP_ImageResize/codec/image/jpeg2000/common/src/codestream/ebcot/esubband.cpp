
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


#include <cfloat>
#include "esubband.h"
#include "quant.h"

inline int DynRangeGain(IppiWTSubband type)
{
    switch(type)
    {
        case ippWTSubbandHxLy: return DYN_RANGE_GAIN_HxLy;
        case ippWTSubbandLxHy: return DYN_RANGE_GAIN_LxHy;
        case ippWTSubbandHxHy: return DYN_RANGE_GAIN_HxHy;
        default:               return DYN_RANGE_GAIN_LxLy;
    }
}

void ESubband::Init(
    const Rect &rect, IppiWTSubband type,
    int cmpDynRange,  int sbQuantExp,
    int prXStepOrder, int prYStepOrder,
    int cbXStepOrder, int cbYStepOrder
    )
{
    m_type            = type;
    m_DynRange        = cmpDynRange + DynRangeGain(type);
    m_lowestNonSfBits = m_DynRange - sbQuantExp;

    m_prGrid.Init(rect, RectSize(prXStepOrder, prYStepOrder));
    m_precincts.ReAlloc(NOfPrecincts());

    GridPow2RelativeIterator ii(m_prGrid);

    for(int i = 0; i < NOfPrecincts(); i++)
    {
        m_precincts[i].Init(ii, cbXStepOrder, cbYStepOrder);
        ++ii;
    }
}

void ESubband::Encode(CBEncoder *cbEncoder, const ImageCore32sC1 &subband)
{
    GridPow2RelativeIterator ii(m_prGrid);

    for(int i = 0; i < NOfPrecincts(); i++)
    {
        ImageCore32sC1 prcImg = subband.SubImage(ii->Origin());
        m_precincts[i].Encode(cbEncoder, prcImg, m_type, m_DynRange, m_lowestNonSfBits);
        ++ii;
    }
}

void ESubband::ScaleDist(double factor)
{
    for(int i = 0; i < NOfPrecincts(); i++)
        m_precincts[i].ScaleDist(factor);
}

void ESubband::SetCurPassToFirst()
{
    for(int i = 0; i < NOfPrecincts(); i++)
        m_precincts[i].SetCurPassToFirst();
}

void ESubband::CalcRDSlopes()
{
    for(int i = 0; i < NOfPrecincts(); i++)
        m_precincts[i].CalcRDSlopes();
}

void ESubband::RDSlopesMinMax(double &min, double &max) const
{
    min = DBL_MAX;
    max = 0;

    for(int i = 0; i < NOfPrecincts(); i++)
    {
        double precMin;
        double precMax;

        m_precincts[i].RDSlopesMinMax(precMin, precMax);

        if(precMax > max) max = precMax;
        if(precMin < min) min = precMin;
    }
}

void ESubband::AssignLayerTillCurPass(int layer, double threshold)
{
    for(int i = 0; i < NOfPrecincts(); i++)
        m_precincts[i].AssignLayerTillCurPass(layer, threshold);
}

void ESubband::AssignLayerTillCurPass(int layer)
{
    for(int i = 0; i < NOfPrecincts(); i++)
        m_precincts[i].AssignLayerTillCurPass(layer);
}

void ESubband::StoreEncodingState()
{
    for(int i = 0; i < NOfPrecincts(); i++)
        m_precincts[i].StoreEncodingState();
}

void ESubband::RestoreEncodingState()
{
    for(int i = 0; i < NOfPrecincts(); i++)
        m_precincts[i].RestoreEncodingState();
}

