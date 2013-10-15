
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

#ifndef __DTILE_H__
#define __DTILE_H__

#include "dtilecomp.h"
#include "jp2bitspercomp.h"
#include "decimetric.h"
#include "bdiagnoutput.h"

class DTile {
public:
    DTile() {}

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    void Init(
        const SBTree<Ipp32s>         *sbTrees,
        const Rect                   &tileRectRG,
        const RectSize               *sampleSizeRG,
        const JP2BitsPerComp         *bitsPerComp,
        unsigned int                  nOfLayers,
        const JP2CodingStyleCompTile &codingStyle,
        const DJP2QuantCompTile      &quant)
    {
        m_components.ReAlloc(codingStyle.Size());
        for(unsigned int component = 0; component < NOfComponents(); component++)
        {
            m_components[component].AttachDiagnOutput(m_diagnOutputPtr);
            m_components[component].Init(sbTrees[component], DecimateRect(tileRectRG, sampleSizeRG[component]),
                bitsPerComp[component].BitDepth(), nOfLayers, codingStyle[component], quant[component]);
        }
    }

    unsigned int NOfComponents()                 const { return m_components.Size(); }
    unsigned int NOfResLevels(unsigned int comp) const { return m_components[comp].NOfResLevels(); }

    template<class ByteInput>
        void ReadPacket(ByteInput &packet, unsigned int component, unsigned int resLevel, const Point &precinct, unsigned int layer)
    {
        m_components[component].ReadPacket(packet, resLevel, precinct, layer);
    }

protected:
    FixedArray<DTileComp> m_components;
    BDiagnOutputPtr       m_diagnOutputPtr;
};

#endif // __DTILE_H__
