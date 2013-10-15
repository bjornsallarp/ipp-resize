
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

#ifndef __ETILECOMP_H__
#define __ETILECOMP_H__

#include "ereslevel.h"
#include "sbtree.h"
#include "quant.h"

class ETileComp {
public:
    ETileComp() {}

    void Init(
        const Rect &cmpRect,
        int cmpDynRange, const QuantComponent &quant,
        const int *prXStepOrder, const int *prYStepOrder, /* [nOfWTLevels+1] */
        const int *cbXStepOrder, const int *cbYStepOrder,
        int nOfWTLevels);

    void Encode(CBEncoder *cbEncoder, const SBTree<Ipp32s> &sbTree);

    void WeightDistAsWT53();
    void WeightDistAsWT97();

    void ScaleDist(double factor);

    void SetCurPassToFirst();

    void CalcRDSlopes();
    void RDSlopesMinMax(double &min, double &max) const;
    void AssignLayerTillCurPass(int layer, double threshold);
    void AssignLayerTillCurPass(int layer);

    void StoreEncodingState();
    void RestoreEncodingState();

    int NOfResLevels() const { return NOfWTLevels() + 1; }
    int NOfPrecincts(int resLevel) const;

    template<class ByteOutput, class BitOutput>
        void WriteToPacket(int resLevel, int precinct, int layer, BitOutput &phBitOut, ByteOutput &packetBody)
    {
        if(!resLevel) m_LxLy.WriteToPacket(precinct, layer, phBitOut, packetBody);
        else
        {
            int sbDepth = SBDepth(resLevel, NOfWTLevels());

            if(sbDepth >= 0)
                m_triplets[sbDepth].WriteToPacket(precinct, layer, phBitOut, packetBody);
        }
    }

protected:
    EResLevelLxLy                m_LxLy;
    FixedArray<EResLevelTriplet> m_triplets;

    static const double m_wt53DistWeightsLxLy    [33];
    static const double m_wt53DistWeightsLxHyHxLy[33];
    static const double m_wt53DistWeightsHxHy    [33];

    static const double m_wt97DistWeightsLxLy    [33];
    static const double m_wt97DistWeightsLxHyHxLy[33];
    static const double m_wt97DistWeightsHxHy    [33];

    int NOfWTLevels() const { return m_triplets.Size(); }
};

#endif // __ETILECOMP_H__
