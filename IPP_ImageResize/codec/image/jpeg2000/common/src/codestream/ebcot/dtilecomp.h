
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


#include "dreslevel.h"
#include "sbtree.h"
#include "quant.h"
#include "jp2codingstylecomp.h"
#include "djp2quantcomp.h"
#include "bdiagnoutput.h"

class DTileComp {
public:
    DTileComp() : m_firstNonEmptyResLevel (0) {}

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    void Init(
        const SBTree<Ipp32s>     &sbTree,
        const Rect               &cmpRect,
        unsigned int              bitDepth,
        unsigned int              nOfLayers,
        const JP2CodingStyleComp &codingStyle,
        const DJP2QuantComp      &quant
        )
    {
        Init(
            sbTree,
            cmpRect,
            bitDepth,
            nOfLayers,
            codingStyle.PrecStepOrder(),
            codingStyle.CBStepOrder(),
            codingStyle.CBStyle(),
            codingStyle.NOfWTLevels(),
            quant.GuardBits(),
            quant.Quant());
    }

    void Init(
        const SBTree<Ipp32s>    &sbTree,
        const Rect              &cmpRect,
        unsigned int             bitDepth,
        unsigned int             nOfLayers,
        const RectSize          *precStepOrder, // [nOfResLevels]
        const RectSize          &cbStepOrder,
        const CBCodingStyle     &cbCodingStyle,
        unsigned int             nOfWTLevels,
        unsigned int             cmpGuardBits,
        const QuantComponent    &quant)
    {
        m_triplets.ReAlloc(nOfWTLevels);

        m_firstNonEmptyResLevel = 0xFFFFFFFF;

        m_LxLy.Init(
            sbTree.LxLy(),
            cmpRect,
            precStepOrder[0],
            cbStepOrder,
            cbCodingStyle,
            nOfLayers,
            nOfWTLevels,
            cmpGuardBits,
            bitDepth,
            quant.LxLy().Expn());

        if(!m_LxLy.IsEmpty()) m_firstNonEmptyResLevel = 0;

        for(unsigned int sbDepth = 0; sbDepth < nOfWTLevels; sbDepth++)
        {
            unsigned int resLevel = ResLevel(sbDepth, nOfWTLevels);
            m_triplets[sbDepth].Init(
                sbTree.Triplet(sbDepth),
                cmpRect,
                precStepOrder[resLevel],
                cbStepOrder,
                cbCodingStyle,
                nOfLayers,
                resLevel,
                nOfWTLevels,
                cmpGuardBits,
                bitDepth,
                quant.HxLy(sbDepth).Expn(),
                quant.LxHy(sbDepth).Expn(),
                quant.HxHy(sbDepth).Expn());

            if(resLevel < m_firstNonEmptyResLevel && (!m_triplets[sbDepth].IsEmpty()) )
                m_firstNonEmptyResLevel = resLevel;
        }
    }

    unsigned int NOfResLevels() const { return ::NOfResLevels(NOfWTLevels()); }

    template<class ByteInput>
        void ReadPacket(ByteInput &packet, unsigned int resLevel, const Point &precinct, unsigned int layer)
    {
        if(resLevel < m_firstNonEmptyResLevel) return;

        PHBitIn<ByteInput> phBitIn(packet);
        phBitIn.AttachDiagnOutput(m_diagnOutputPtr);

        bool isNotZeroLength = phBitIn.Read();

        if (isNotZeroLength)
            ReadPacketHeader(resLevel, precinct, layer, phBitIn);

        phBitIn.Flush(!isNotZeroLength);

        // if (isNotZeroLength)
        // excluded to support decoding in layer-accumalating modes
        // that lead to emit final decoding results for code-blocks
        ReadPacketBody(resLevel, precinct, layer, packet);
    }


    template<class ByteInput>
        void ReadPacketHeader(unsigned int resLevel, const Point &precinct, unsigned int layer, PHBitIn<ByteInput> &phBitIn)
    {
        if(!resLevel)
            m_LxLy.ReadPacketHeader(precinct, layer, phBitIn);
        else
        {
            unsigned int sbDepth = SBDepth(resLevel, NOfWTLevels());

            m_triplets[sbDepth].ReadPacketHeader(precinct, layer, phBitIn);
        }
    }

    template<class ByteInput>
        void ReadPacketBody(unsigned int resLevel, const Point &precinct, unsigned int layer, ByteInput &packetBody)
    {
        if(!resLevel) m_LxLy.ReadPacketBody(precinct, layer, packetBody);
        else
        {
            int sbDepth = SBDepth(resLevel, NOfWTLevels());

            if(sbDepth >= 0)
                m_triplets[sbDepth].ReadPacketBody(precinct, layer, packetBody);
        }
    }

protected:
    DResLevelLxLy                m_LxLy;
    FixedArray<DResLevelTriplet> m_triplets;
    BDiagnOutputPtr              m_diagnOutputPtr;

    unsigned int                 m_firstNonEmptyResLevel;

    unsigned int NOfWTLevels() const { return m_triplets.Size(); }
};
