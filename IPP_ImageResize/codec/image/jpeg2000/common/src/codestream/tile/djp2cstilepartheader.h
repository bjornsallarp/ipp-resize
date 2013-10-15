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

#ifndef __DJP2CSTILEPARTHEADER_H__
#define __DJP2CSTILEPARTHEADER_H__

#include "djp2quantcomp.h"
#include "jp2codingstylecomp.h"
#include "djp2csmainheader.h"
#include "djp2marker.h"
#include "djp2cswarning.h"
#include "djp2csunsuppexception.h"
#include "bdiagnoutput.h"

class DJP2CSTilePartHeaderN
{
public:
    DJP2CSTilePartHeaderN() {}

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    const FixedArray<JP2ProgrRange> &ChangedProgrRange() const { return m_changedProgrRange; }

    // This function does not read SOT marker and its body
    // becouse it's more convenient to make it in parent
    // parsing loop which is responsible for reading markers.
    //
    // This function reads all the header till SOD will found,
    // and it slurps SOD marker from the stream.
    template<class ByteInput>
        void Read(
            ByteInput                       &stream,
            const FixedArray<JP2ProgrRange> &changedProgrRange0,
            int                              nOfComponents
        )
    {
        m_changedProgrRange.SetRef(changedProgrRange0);

        JP2Marker marker = ReadMarker(stream);


        while (marker != SOD)
        {
            {
            unsigned int bodySize = ReadMarkerBodySize(stream);
            if(stream.PushSize(bodySize) != bodySize)
                 m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, SpecifiedMarkerLengthExceedContainerTail>());
            }

            switch (marker)
            {
            case COM: ReadCOMBody(stream);                break;
            case PLT: ReadPLTBody(stream);                break;
            case POC: ReadPOCBody(stream, nOfComponents); break;
            case PPM: ReadPPMBody(stream);                break;
            default:
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, TilePartHeaderUnknownMarker>());
                stream.Seek(stream.TailSize());
            }

            if(stream.PopSize())
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, ActualMarkerLengthBelowSpecified>());

            marker = ReadMarker(stream);
        }
    }

protected:
    OptionalRefValue<FixedArray<JP2ProgrRange> > m_changedProgrRange;

    BDiagnOutputPtr m_diagnOutputPtr;

    template<class ByteInput>
        void ReadPPMBody(ByteInput &/* body */) { /* body; */ throw DiagnDescrCT<DJP2CSUnsuppException,PPMUsed>(); }
    template<class ByteInput>
        void ReadCOMBody(ByteInput &body)
    {
        body.Seek(body.TailSize());
        m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, COMUsed>());
    }
    template<class ByteInput>
        void ReadPLTBody(ByteInput &body)
    {
        body.Seek(body.TailSize());
        m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, PLTUsed>());
    }
    template<class ByteInput>
        void ReadPOCBody(ByteInput &body, int nOfComponents)
    {
        if(m_changedProgrRange.IsOwn())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, TilePartHeaderExtraPOC>());

        ::ReadPOCBody(body, nOfComponents, m_changedProgrRange.CreateOwn(), m_diagnOutputPtr);
    }

};

class DJP2CSTilePartHeader0 : public DJP2CSTilePartHeaderN
{
public:
    DJP2CSTilePartHeader0() {}

    void AttachDiagnOutput(BDiagnOutput &diagnOutput)
    {
        m_diagnOutputPtr = diagnOutput;
        m_quant.AttachDiagnOutput(diagnOutput);
    }

    const DJP2QuantCompTile      &Quant()             const { return m_quant;            }
    const JP2CodingStyleCompTile &CodingStyleComp()   const { return m_codingStyleComp;  }
    const JP2CodingStyleJoint    &CodingStyleJoint()  const { return (const JP2CodingStyleJoint&)m_codingStyleJoint; }

    // This function does not read SOT marker and its body
    // becouse it's more convenient to make it in finishing of
    // DJP2CSMainHeader::Read(ByteInput &stream) function
    // which is responsible for reading first SOT marker in the tile.
    //
    // This function reads all the header till SOD will found,
    // and it slurps SOD marker from the stream.
    template<class ByteInput>
        void Read(
            ByteInput                       &stream,
            const DJP2QuantCompMain         &quantMain,
            const JP2CodingStyleJoint       &codingStyleJointMain,
            const JP2CodingStyleCompMain    &codingStyleCompMain,
            const FixedArray<JP2ProgrRange> &changedProgrRangeMain
        )
    {
        m_changedProgrRange.SetRef(changedProgrRangeMain);
        m_codingStyleJoint .SetRef(codingStyleJointMain);

        m_quant          .ReAlloc(quantMain);
        m_codingStyleComp.ReAlloc(codingStyleCompMain);

        JP2Marker marker = ReadMarker(stream);

        while (marker != SOD)
        {
            {
            unsigned int bodySize = ReadMarkerBodySize(stream);
            if(stream.PushSize(bodySize) != bodySize)
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, SpecifiedMarkerLengthExceedContainerTail>());
            }

            switch (marker)
            {
            case COD: ReadCODBody(stream);                             break;
            case COC: ReadCOCBody(stream);                             break;
            case QCD: ReadQCDBody(stream);                             break;
            case QCC: ReadQCCBody(stream);                             break;
            case COM: ReadCOMBody(stream);                             break;
            case PLT: ReadPLTBody(stream);                             break;
            case POC: ReadPOCBody(stream, codingStyleCompMain.Size()); break;
            case PPM: ReadPPMBody(stream);                             break;
            default:
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, TilePartHeaderUnknownMarker>());
                stream.Seek(stream.TailSize());
            }

            if(stream.PopSize())
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, ActualMarkerLengthBelowSpecified>());

            marker = ReadMarker(stream);
        }

        m_quant.Conform(m_codingStyleComp);
    }

protected:
    OptionalRefValue<JP2CodingStyleJoint> m_codingStyleJoint;

    DJP2QuantCompTile                  m_quant;
    JP2CodingStyleCompTile             m_codingStyleComp;

    template<class ByteInput>
        void ReadCODBody(ByteInput &body)
    {
        if(m_codingStyleJoint.IsOwn())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, TilePartHeaderExtraCOD>());

        ::ReadCODBody(
            body,
            m_codingStyleJoint.CreateOwn(),
            m_codingStyleComp.CreateDefaultOwn(),
            m_diagnOutputPtr);
    }

    template<class ByteInput>
        void ReadCOCBody(ByteInput &body)
    {
        unsigned int component = ReadMarkerCompIndex(body, m_codingStyleComp.Size());

        if(m_quant.IsCustom(component))
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, TilePartHeaderExtraCOC>());

//        ::ReadCOCBody(body, m_codingStyleComp.CreateCustom(component), m_diagnOutputPtr);
        ::ReadCOCBody(body, m_codingStyleComp.CreateCustom(component));
    }

    template<class ByteInput>
        void ReadQCDBody(ByteInput &body)
    {
        if(m_quant.IsDefaultOwn())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, TilePartHeaderExtraCOD>());

        ::ReadQCDBody(body, m_quant.CreateDefaultOwn());
    }

    template<class ByteInput>
        void ReadQCCBody(ByteInput &body)
    {
        unsigned int component = ReadMarkerCompIndex(body, m_quant.Size());

        if(m_quant.IsCustom(component))
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, TilePartHeaderExtraQCC>());

        ::ReadQCCBody(body, m_quant.CreateCustom(component));
    }

};


#endif // __DJP2CSTILEPARTHEADER_H__
