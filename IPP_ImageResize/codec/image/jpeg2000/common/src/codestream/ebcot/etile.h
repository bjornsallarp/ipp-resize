
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

#ifndef __ETILE_H__
#define __ETILE_H__

#include "etilecomp.h"

class ETile {
public:
    ETile();

    void Init(
        const Rect *cmpRect,
        const Int2D &prXStepOrder, const Int2D &prYStepOrder, /* [nOfComponents][nOfWTLevels+1] */
        const Int2D &cbXStepOrder, const Int2D &cbYStepOrder,
        const int *cmpDynRange, const QuantComponent *cmpQuant,
        const int *nOfWTLevels, int nOfComponents);

    void Encode(CBEncoder *cbEncoder, const SBTree<Ipp32s> *sbTrees);

    void WeightDistAsWT53();
    void WeightDistAsWT97();

    void WeightDistAsRCT();
    void WeightDistAsICT();

    void SetCurPassToFirst();

    int  PacketsLengthSimulate(int layer);

    void CalcRDSlopes();
    void RDSlopesMinMax(double &min, double &max) const;

    void AssignLayers(int *lengths, int nOfLayers);

    void StoreEncodingState();
    void RestoreEncodingState();

    int NOfComponents() const { return m_components.Size(); }
    int NOfResLevels(int comp) const;
    int NOfPrecincts(int comp, int resLevel) const;

    template<class ByteOutput>
        void WriteToPacket(int component, int resLevel, int precinct, int layer, ByteOutput &packet)
    {
        BufferStream         body;
        PHBitOut<ByteOutput> header(packet);

        WriteToPacket(component, resLevel, precinct, layer, header, body);
        Transfer(body, packet, body.Size());
    }


protected:
    void AssignLayerTillCurPass(int layer, double threshold);
    void AssignLayerTillCurPass(int layer);

    template<class ByteOutput, class BitOutput>
        void WriteToPacket(int component, int resLevel, int precinct, int layer, BitOutput &phBitOut, ByteOutput &packetBody)
    {
        phBitOut.Write1();
        m_components[component].WriteToPacket(resLevel, precinct, layer, phBitOut, packetBody);
        phBitOut.Flush();
    }


    FixedArray<ETileComp> m_components;

    static const double m_rctDistWeights[3];
    static const double m_ictDistWeights[3];
};

#endif // __ETILE_H__

