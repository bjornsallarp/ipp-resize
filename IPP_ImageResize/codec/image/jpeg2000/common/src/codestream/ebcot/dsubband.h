
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

#ifndef __DSUBBAND_H__
#define __DSUBBAND_H__

#include "dprecinct.h"

class DSubband {
public:
    DSubband() {}

    void Init(
        const ImageCore32sC1 &subband,
        IppiWTSubband         type,
        const Rect           &rectR,
        const RectSize       &precStepOrder,
        const RectSize       &cbStepOrder,
        const CBCodingStyle  &codingStyle,
        unsigned int          nOfLayers,
        unsigned int          cmpGuardBits,
        unsigned int          bitDepth,
        int                   quantExp);

    //
    // In this model precinct's coors are used instead of precinct raster number.
    //

    template<class ByteInput>
        void ReadPacketHeader(const Point &precinct, unsigned int layer, PHBitIn<ByteInput> &phBitIn)
    {
        if(!IsEmpty(precinct)) m_precincts[precinct.Y()][precinct.X()].ReadPacketHeader(layer, phBitIn);
    }

    template<class ByteInput>
        void ReadPacketBody  (const Point &precinct, unsigned int layer, ByteInput &packetBody)
    {
        if(!IsEmpty(precinct)) m_precincts[precinct.Y()][precinct.X()].ReadPacketBody(layer, packetBody);
    }

    bool IsEmpty() const { return m_isEmpty; }
protected:
    bool IsEmpty(const Point &precinct) const { return precinct.Y() >= NOfActualPrecRows() || precinct.X() >= NOfActualPrecCols(); }
    unsigned int NOfActualPrecRows() const { return m_precincts.NOfRows(); }
    unsigned int NOfActualPrecCols() const { return m_precincts.NOfCols(); }

    FixedArray2D<DPrecinct>  m_precincts;
    bool                     m_isEmpty;
};

#endif // __DSUBBAND_H__

