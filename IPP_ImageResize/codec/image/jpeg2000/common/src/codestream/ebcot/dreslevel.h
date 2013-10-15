
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

#ifndef __DRESLEVEL_H__
#define __DRESLEVEL_H__

#include "dsubband.h"
#include "quant.h"
#include "sbtree.h"

class DResLevelLxLy : public DSubband {
public:
    DResLevelLxLy() {}

    void Init(
        const ImageCore32sC1 &LxLy,
        const Rect           &cmpRect,
        const RectSize       &precStepOrder,
        const RectSize       &cbStepOrder,
        const CBCodingStyle  &codingStyle,
        unsigned int          nOfLayers,
        unsigned int          nOfWTLevels,
        unsigned int          cmpGuardBits,
        unsigned int          bitDepth,
        int                   quantExpLxLy)
    {
        Init(
            LxLy,
            ippWTSubbandLxLy,
            ScaleR(cmpRect, 0, NOfResLevels(nOfWTLevels)),
            precStepOrder,
            cbStepOrder,
            codingStyle,
            nOfLayers,
            cmpGuardBits,
            bitDepth,
            quantExpLxLy);
    }

protected:
    // only to protect base class public initialization function
    void Init(
        const ImageCore32sC1 &subband,
        IppiWTSubband         type,
        const Rect           &rect,
        const RectSize       &precStepOrder,
        const RectSize       &cbStepOrder,
        const CBCodingStyle  &codingStyle,
        unsigned int          nOfLayers,
        unsigned int          cmpGuardBits,
        unsigned int          bitDepth,
        int                   quantExp)
    {
        DSubband::Init(subband, type, rect, precStepOrder, cbStepOrder,
            codingStyle, nOfLayers, cmpGuardBits, bitDepth, quantExp);
    }
};

class DResLevelTriplet {
public:
    DResLevelTriplet() {}

    void Init(
        const SBTriplet<Ipp32s> &sbTriplet,
        const Rect              &cmpRect,
        const RectSize          &precStepOrder,
        const RectSize          &cbStepOrder,
        const CBCodingStyle     &codingStyle,
        unsigned int             nOfLayers,
        unsigned int             resLevel,
        unsigned int             nOfWTLevels,
        unsigned int             cmpGuardBits,
        unsigned int             bitDepth,
        int                      quantExpHxLy,
        int                      quantExpLxHy,
        int                      quantExpHxHy)
    {
        Rect         rectR       = ScaleR(cmpRect, resLevel, NOfResLevels(nOfWTLevels));
        RectSize actualStepOrder = precStepOrder;

        m_HxLy.Init(sbTriplet.HxLy(), ippWTSubbandHxLy, rectR, actualStepOrder, cbStepOrder, codingStyle, nOfLayers, cmpGuardBits, bitDepth, quantExpHxLy);
        m_LxHy.Init(sbTriplet.LxHy(), ippWTSubbandLxHy, rectR, actualStepOrder, cbStepOrder, codingStyle, nOfLayers, cmpGuardBits, bitDepth, quantExpLxHy);
        m_HxHy.Init(sbTriplet.HxHy(), ippWTSubbandHxHy, rectR, actualStepOrder, cbStepOrder, codingStyle, nOfLayers, cmpGuardBits, bitDepth, quantExpHxHy);
    }


    template<class ByteInput>
        void ReadPacketHeader(const Point &precinct, unsigned int layer, PHBitIn<ByteInput> &phBitIn)
    {
        m_HxLy.ReadPacketHeader(precinct, layer, phBitIn);
        m_LxHy.ReadPacketHeader(precinct, layer, phBitIn);
        m_HxHy.ReadPacketHeader(precinct, layer, phBitIn);
    }

    template<class ByteInput>
        void ReadPacketBody(const Point &precinct, unsigned int layer, ByteInput &packetBody)
    {
        m_HxLy.ReadPacketBody(precinct, layer, packetBody);
        m_LxHy.ReadPacketBody(precinct, layer, packetBody);
        m_HxHy.ReadPacketBody(precinct, layer, packetBody);
    }

    bool IsEmpty() { return m_HxLy.IsEmpty() && m_LxHy.IsEmpty() && m_HxHy.IsEmpty(); }

protected:
    DSubband m_HxLy;
    DSubband m_LxHy;
    DSubband m_HxHy;
};

#endif // __DRESLEVEL_H__

