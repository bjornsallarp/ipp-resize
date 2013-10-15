
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

/*
//  Subband class for encoder. Only single precinct supported at
//      the moment.
//
//
//
*/

#ifndef __ESUBBAND_H__
#define __ESUBBAND_H__

#include "eprecinct.h"

class ESubband {
public:
    ESubband() : m_DynRange(0), m_lowestNonSfBits(0) {}

    void Init(
        const Rect &rect, IppiWTSubband type,
        int cmpDynRange,  int sbQuantExp,
        int prXStepOrder, int prYStepOrder,
        int cbXStepOrder, int cbYStepOrder
        );

    void Encode(CBEncoder *cbEncoder, const ImageCore32sC1 &subband);

    void SetCurPassToFirst();

    void ScaleDist(double factor);
    void CalcRDSlopes();
    void RDSlopesMinMax(double &min, double &max) const;

    void AssignLayerTillCurPass(int layer, double threshold);
    void AssignLayerTillCurPass(int layer);


    void StoreEncodingState();
    void RestoreEncodingState();

    int NOfPrecincts() const { return m_prGrid.NOfCells(); }

    template<class ByteOutput, class BitOutput>
        void WriteToPacket(int precinct, int layer, BitOutput &phBitOut, ByteOutput &packetBody)
    {
        if(precinct < NOfPrecincts())
            m_precincts[precinct].WriteToPacket(layer, phBitOut, packetBody);
    }

protected:
    IppiWTSubband          m_type;
    GridPow2               m_prGrid;
    FixedBuffer<EPrecinct> m_precincts;
    int                    m_DynRange;
    int                    m_lowestNonSfBits;
};

#endif // __ESUBBAND_H__

