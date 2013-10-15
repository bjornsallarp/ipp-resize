
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

#ifndef __DJP2CSMAINHEADER_H__
#define __DJP2CSMAINHEADER_H__

#include "djp2quantcomp.h"
#include "jp2codingstylecomp.h"
#include "jp2codingstylejoint.h"
#include "jp2progrrange.h"
#include "jp2bitspercomp.h"
#include "djp2marker.h"
#include "djp2cswarning.h"
#include "djp2csexception.h"
#include "djp2csunsuppexception.h"
#include "bdiagnoutput.h"

class DJP2CSMainHeader
{
public:
    DJP2CSMainHeader()
    : m_nOfComponents(0)
    , m_CODNotFound(true)
    {}

    void Reset();

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }


    const Rect                      &ImageRectRG()       const { return m_imageRectRG;       }
    const Point                     &TileGridOriginRG()  const { return m_tileGridOriginRG;  }
    const RectSize                  &TileSizeRG()        const { return m_tileSizeRG;        }
    Ipp16u                           NOfComponents()     const { return m_nOfComponents;     }
    const JP2BitsPerComp            *BitsPerComp()       const { return m_bitsPerComp;       }
    const RectSize                  *SampleSizeRG()      const { return m_sampleSizeRG;      }

    const FixedArray<JP2ProgrRange> &ChangedProgrRange() const { return m_changedProgrRange; }

    const DJP2QuantCompMain         &Quant()             const { return m_quant;             }
    const JP2CodingStyleCompMain    &CodingStyleComp()   const { return m_codingStyleComp;   }
    const JP2CodingStyleJoint       &CodingStyleJoint()  const { return m_codingStyleJoint;  }

    bool IsMCTResampled() const
    {
        return (m_nOfComponents < 3) ? false :
            !(
                m_sampleSizeRG[0] == m_sampleSizeRG[1] &&
                m_sampleSizeRG[1] == m_sampleSizeRG[2]
            );
    }

    // This function slurps the SOC marker at the beginning
    // and slurp first occurrence of SOT marker at the end.
    // If EOC marker was found at the end instead SOT
    // function returns 'false' otherwise it returns 'true'.
    template<class ByteInput>
        bool Read(ByteInput &stream)
    {
        ReadSOC(stream);
        ReadSIZ(stream);

        m_quant          .ReAlloc(m_nOfComponents);
        m_codingStyleComp.ReAlloc(m_nOfComponents);

        JP2Marker marker = ReadMarker(stream);

        while (marker != SOT)
        {
            {
            unsigned int bodySize = ReadMarkerBodySize(stream);
            if(stream.PushSize(bodySize) != bodySize)
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, SpecifiedMarkerLengthExceedContainerTail>());
            }

            switch (marker)
            {
            case COD: ReadCODBody(stream); break;
            case COC: ReadCOCBody(stream); break;
            case QCD: ReadQCDBody(stream); break;
            case QCC: ReadQCCBody(stream); break;
            case TLM: ReadTLMBody(stream); break;
            case PLM: ReadPLMBody(stream); break;
            case CRG: ReadCRGBody(stream); break;
            case COM: ReadCOMBody(stream); break;
            case POC: ReadPOCBody(stream); break;
            case RGN: ReadRGNBody(stream); break;
            case PPM: ReadPPMBody(stream); break;
            case EOC:
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, MainHeaderOnlyInCodestream>());
                return false;
            default:
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, MainHeaderUnknownMarker>());
                stream.Seek(stream.TailSize());
            }

            if(stream.PopSize())
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, ActualMarkerLengthBelowSpecified>());

            marker = ReadMarker(stream);
        }

        if (m_CODNotFound)
            throw DiagnDescrCT<DJP2CSException,CODNotFound>();

        if (m_quant.Default().IsEmpty())
            throw DiagnDescrCT<DJP2CSException,QCDNotFound>();

        return true;
    }

protected:
    Rect                        m_imageRectRG;
    Point                       m_tileGridOriginRG;
    RectSize                    m_tileSizeRG;

    Ipp16u                      m_nOfComponents;

    FixedBuffer<JP2BitsPerComp> m_bitsPerComp;
    FixedBuffer<RectSize>       m_sampleSizeRG;

    DJP2QuantCompMain           m_quant;

    JP2CodingStyleCompMain      m_codingStyleComp;
    JP2CodingStyleJoint         m_codingStyleJoint;

    FixedArray<JP2ProgrRange>   m_changedProgrRange;

    bool                        m_CODNotFound;
    BDiagnOutputPtr             m_diagnOutputPtr;

    template<class ByteInput>
        void ReadSOC(ByteInput &stream)
    {
        if (ReadMarker(stream) != SOC)
            throw DiagnDescrCT<DJP2CSException,SOCNotFound>();
    }

    template<class ByteInput>
        void ReadSIZ(ByteInput &stream)
    {
        if (ReadMarker(stream) != SIZ)
            throw DiagnDescrCT<DJP2CSException,SIZNotFound>();

        {
        unsigned int bodySize = ReadMarkerBodySize(stream);
        if(stream.PushSize(bodySize) != bodySize)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, SpecifiedMarkerLengthExceedContainerTail>());
        }

        ReadSIZBody(
            stream,
            m_imageRectRG,
            m_tileGridOriginRG,
            m_tileSizeRG,
            m_nOfComponents,
            m_bitsPerComp,
            m_sampleSizeRG,
            m_diagnOutputPtr);

        if(stream.PopSize())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, ActualMarkerLengthBelowSpecified>());
    }

    template<class ByteInput>
        void ReadCODBody(ByteInput &body)
    {
        if(!m_CODNotFound)
        {
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, MainHeaderExtraCOD>());
        }
        else
            m_CODNotFound = false;

        ::ReadCODBody(
            body,
            m_codingStyleJoint,
            m_codingStyleComp.Default(),
            m_diagnOutputPtr);

    }

    template<class ByteInput>
        void ReadCOCBody(ByteInput &body)
    {
        unsigned int component = ReadMarkerCompIndex(body, m_nOfComponents);

        if (component > m_nOfComponents)
            throw DiagnDescrCT<DJP2CSException,COCIndexExceedNOfComponents>();

        if(m_quant.IsCustom(component))
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, MainHeaderExtraCOC>());

//        ::ReadCOCBody(body, m_codingStyleComp.CreateCustom(component), m_diagnOutputPtr);
        ::ReadCOCBody(body, m_codingStyleComp.CreateCustom(component));
    }

    template<class ByteInput>
        void ReadQCDBody(ByteInput &body)
    {
        if(!m_quant.Default().IsEmpty())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, MainHeaderExtraQCD>());

        ::ReadQCDBody(body, m_quant.Default());
    }

    template<class ByteInput>
        void ReadQCCBody(ByteInput &body)
    {
        unsigned int component = ReadMarkerCompIndex(body, m_nOfComponents);

        if (component > m_nOfComponents)
            throw DiagnDescrCT<DJP2CSException,QCCIndexExceedNOfComponents>();

        if(m_quant.IsCustom(component))
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, MainHeaderExtraQCC>());

        DJP2QuantComp &quant = m_quant.CreateCustom(component);
        quant.AttachDiagnOutput(m_diagnOutputPtr);
        ::ReadQCCBody(body, quant);
    }


    template<class ByteInput>
        void ReadPOCBody(ByteInput &body)
    {
        if(m_changedProgrRange.Size())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, MainHeaderExtraPOC>());

        ::ReadPOCBody(body, m_nOfComponents, m_changedProgrRange, m_diagnOutputPtr);
    }


    template<class ByteInput>
        void ReadRGNBody(ByteInput &/* body */) { /* body; */ throw DiagnDescrCT<DJP2CSUnsuppException,RGNUsed>(); }
    template<class ByteInput>
        void ReadPPMBody(ByteInput &/* body */) { /* body; */ throw DiagnDescrCT<DJP2CSUnsuppException,PPMUsed>(); }
    template<class ByteInput>
        void ReadTLMBody(ByteInput &body) {
            body.Seek(body.TailSize());
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, TLMUsed>()); }
    template<class ByteInput>
        void ReadPLMBody(ByteInput &body) {
            body.Seek(body.TailSize());
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, PLMUsed>()); }
    template<class ByteInput>
        void ReadCRGBody(ByteInput &body) {
            body.Seek(body.TailSize());
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, CRGUsed>()); }
    template<class ByteInput>
        void ReadCOMBody(ByteInput &body) {
            body.Seek(body.TailSize());
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, COMUsed>()); }

};

#endif // __DJP2CSMAINHEADER_H__
