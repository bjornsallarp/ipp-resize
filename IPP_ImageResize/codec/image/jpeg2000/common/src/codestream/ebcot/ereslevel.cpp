
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
#include "ereslevel.h"

void EResLevelTriplet::Init(
    const Rect &cmpRect,
    int prXStepOrder, int prYStepOrder,
    int cbXStepOrder, int cbYStepOrder,
    int sbDepth,
    int cmpDynRange,
    int quantExpHxLy,
    int quantExpLxHy,
    int quantExpHxHy
)
{
    int wtLevel = WTLevel(sbDepth);

    Rect rectLxLy = RectLxLy(cmpRect, wtLevel);

    {
    GridPow2 tmpGrid(rectLxLy, RectSize(prXStepOrder, prYStepOrder));
    m_nOfPrecincts = tmpGrid.NOfCells();
    }

    m_HxLy.Init(RectHxLy(cmpRect, wtLevel), ippWTSubbandHxLy, cmpDynRange, quantExpHxLy, prXStepOrder, prYStepOrder, cbXStepOrder, cbYStepOrder);
    m_LxHy.Init(RectLxHy(cmpRect, wtLevel), ippWTSubbandLxHy, cmpDynRange, quantExpLxHy, prXStepOrder, prYStepOrder, cbXStepOrder, cbYStepOrder);
    m_HxHy.Init(RectHxHy(cmpRect, wtLevel), ippWTSubbandHxHy, cmpDynRange, quantExpHxHy, prXStepOrder, prYStepOrder, cbXStepOrder, cbYStepOrder);
}

void EResLevelTriplet::Encode(CBEncoder *cbEncoder, const SBTriplet<Ipp32s> &sbTriplet)
{
    m_HxLy.Encode(cbEncoder, sbTriplet.HxLy());
    m_LxHy.Encode(cbEncoder, sbTriplet.LxHy());
    m_HxHy.Encode(cbEncoder, sbTriplet.HxHy());
}

void EResLevelTriplet::ScaleDist(
    double HxLy,
    double LxHy,
    double HxHy
)
{
    m_HxLy.ScaleDist(HxLy);
    m_LxHy.ScaleDist(LxHy);
    m_HxHy.ScaleDist(HxHy);
}

void EResLevelTriplet::ScaleDist(double factor)
{
    ScaleDist(factor, factor, factor);
}

void EResLevelTriplet::SetCurPassToFirst()
{
    m_HxLy.SetCurPassToFirst();
    m_LxHy.SetCurPassToFirst();
    m_HxHy.SetCurPassToFirst();
}

void EResLevelTriplet::CalcRDSlopes()
{
    m_HxLy.CalcRDSlopes();
    m_LxHy.CalcRDSlopes();
    m_HxHy.CalcRDSlopes();
}

void EResLevelTriplet::RDSlopesMinMax(double &min, double &max) const
{
    m_HxLy.RDSlopesMinMax(min, max);

    double bandMin, bandMax;

    m_LxHy.RDSlopesMinMax(bandMin, bandMax);
    if(bandMax > max) max = bandMax;
    if(bandMin < min) min = bandMin;

    m_HxHy.RDSlopesMinMax(bandMin, bandMax);
    if(bandMax > max) max = bandMax;
    if(bandMin < min) min = bandMin;
}

void EResLevelTriplet::AssignLayerTillCurPass(int layer, double threshold)
{
    m_HxLy.AssignLayerTillCurPass(layer, threshold);
    m_LxHy.AssignLayerTillCurPass(layer, threshold);
    m_HxHy.AssignLayerTillCurPass(layer, threshold);
}

void EResLevelTriplet::AssignLayerTillCurPass(int layer)
{
    m_HxLy.AssignLayerTillCurPass(layer);
    m_LxHy.AssignLayerTillCurPass(layer);
    m_HxHy.AssignLayerTillCurPass(layer);
}

void EResLevelTriplet::StoreEncodingState()
{
    m_HxLy.StoreEncodingState();
    m_LxHy.StoreEncodingState();
    m_HxHy.StoreEncodingState();
}

void EResLevelTriplet::RestoreEncodingState()
{
    m_HxLy.RestoreEncodingState();
    m_LxHy.RestoreEncodingState();
    m_HxHy.RestoreEncodingState();
}

