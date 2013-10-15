
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

#ifndef __ERESLEVEL_H__
#define __ERESLEVEL_H__

#include "esubband.h"
#include "quant.h"
#include "sbtree.h"

class EResLevelLxLy : public ESubband {
public:
    EResLevelLxLy() {}

    void Init(
        const Rect &cmpRect,
        int prXStepOrder, int prYStepOrder,
        int cbXStepOrder, int cbYStepOrder,
        int nOfWTLevels,  int cmpDynRange,
        int quantExpLxLy
        )
    {
        Init(RectLxLy(cmpRect, nOfWTLevels),
            ippWTSubbandLxLy, cmpDynRange, quantExpLxLy,
            prXStepOrder, prYStepOrder, cbXStepOrder, cbYStepOrder);
    }

protected:
    // only to protect base class public initialization function
    void Init(
        const Rect &rect, IppiWTSubband type,
        int cmpDynRange,  int sbQuantExp,
        int prXStepOrder, int prYStepOrder,
        int cbXStepOrder, int cbYStepOrder
    )
    {
        ESubband::Init(rect, type, cmpDynRange, sbQuantExp,
            prXStepOrder, prYStepOrder, cbXStepOrder, cbYStepOrder);
    }
};

class EResLevelTriplet {
public:
    EResLevelTriplet() : m_nOfPrecincts(0) {}

    void Init(
        const Rect &cmpRect,
        int prXStepOrder, int prYStepOrder,
        int cbXStepOrder, int cbYStepOrder,
        int sbDepth,      int cmpDynRange,
        int quantExpHxLy,
        int quantExpLxHy,
        int quantExpHxHy
        );

    void Encode(CBEncoder *cbEncoder, const SBTriplet<Ipp32s> &sbTriplet);

    void ScaleDist(
        double HxLy,
        double LxHy,
        double HxHy
        );

    void ScaleDist(double factor);

    void SetCurPassToFirst();

    void CalcRDSlopes();
    void RDSlopesMinMax(double &min, double &max) const;

    void AssignLayerTillCurPass(int layer, double threshold);
    void AssignLayerTillCurPass(int layer);

    void StoreEncodingState();
    void RestoreEncodingState();

    int NOfPrecincts() const { return m_nOfPrecincts; }

    template<class ByteOutput, class BitOutput>
        void WriteToPacket(int precinct, int layer, BitOutput &phBitOut, ByteOutput &packetBody)
    {
        m_HxLy.WriteToPacket(precinct, layer, phBitOut, packetBody);
        m_LxHy.WriteToPacket(precinct, layer, phBitOut, packetBody);
        m_HxHy.WriteToPacket(precinct, layer, phBitOut, packetBody);
    }

protected:
    ESubband m_HxLy;
    ESubband m_LxHy;
    ESubband m_HxHy;
    int      m_nOfPrecincts;
};

#endif // __ERESLEVEL_H__

