
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

#ifndef __DJP2BOXHEADER_H__
#define __DJP2BOXHEADER_H__

#include "fixedbuffer.h"

#include "djp2boxpalette.h"
#include "djp2boximageheader.h"
#include "djp2boxcompmapping.h"
#include "djp2boxcolourspec.h"
#include "djp2boxbitspercomp.h"
#include "djp2boxresolution.h"

#include "bdiagnoutput.h"

class DJP2BoxHeader
{
public:
    DJP2BoxHeader() : m_isEmpty(true) {}

    void AttachDiagnOutput(BDiagnOutput &diagnOutput)
    {
        m_diagnOutputPtr = diagnOutput;
        m_imageHeaderBox.AttachDiagnOutput(diagnOutput);
        m_bitsPerCompBox.AttachDiagnOutput(diagnOutput);
        m_colourSpecBox.AttachDiagnOutput(diagnOutput);
        m_paletteBox.AttachDiagnOutput(diagnOutput);
        m_compMappingBox.AttachDiagnOutput(diagnOutput);
        m_resolutionBox.AttachDiagnOutput(diagnOutput);
    }

    void Reset()
    {
        m_isEmpty = true;
        m_imageHeaderBox.Reset();
        m_bitsPerCompBox.Reset();
        m_colourSpecBox.Reset();
        m_paletteBox.Reset();
        m_compMappingBox.Reset();
        m_isChannelPaletteMappingConformant.Free();
    }

    template<class ByteInput>
        void Read(ByteInput &stream)
    {
        m_imageHeaderBox.Read(stream);

        while (stream.TailSize())
        {
            JP2BoxType boxType;

            {
            unsigned int bodySize;
            ReadBoxHeader(stream, boxType, bodySize);
            if(stream.PushSize(bodySize) != bodySize)
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, SpecifiedBoxLengthExceedContainerTail>());
            }

            switch(boxType)
            {
            case bpcc: m_bitsPerCompBox.Read(stream,
                m_imageHeaderBox.NOfComponents());    break;
            case colr: m_colourSpecBox.Read(stream);  break;
            case pclr: m_paletteBox.Read(stream);     break;
            case cmap: m_compMappingBox.Read(stream); break;
            case res_: m_resolutionBox.Read(stream);  break;
            default:
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, UnknownSubBoxInHeaderBox>());
            }

            if(stream.PopSize())
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ActualBoxLengthBelowSpecified>());
        }

        CheckPaletteMappingConformance();
        CheckBitsPerCompConformance();
        CheckColorSpecConformance();

        m_isEmpty = false;

    }

    bool IsEmpty() const { return m_isEmpty; }

    const DJP2BoxImageHeader &ImageHeader() const { return m_imageHeaderBox; }
    const DJP2BoxBitsPerComp &BitsPerComp() const { return m_bitsPerCompBox; }
    const DJP2BoxColourSpec  &ColourSpec()  const { return m_colourSpecBox;  }
    const DJP2BoxPalette     &Palette()     const { return m_paletteBox;     }
    const DJP2BoxCompMapping &CompMapping() const { return m_compMappingBox; }
    const DJP2BoxResolution  &Resolution()  const { return m_resolutionBox;  }

    // Sorry but format allows that formal data are not conformant
    // and its "direct" using are dangerous becouse of lack of data and so on.
    // Here is stored indicator of conformance analisys during reading.
    // Please remember, it contains CompMapping().NOfChannels() elements,
    // so if CompMapping is empty, then there is no values in this array.
    bool  IsChannelPaletteMappingConformant(unsigned int channel) const { return m_isChannelPaletteMappingConformant[channel]; }

protected:
    bool m_isEmpty;

    BDiagnOutputPtr    m_diagnOutputPtr;

    DJP2BoxImageHeader m_imageHeaderBox;
    DJP2BoxBitsPerComp m_bitsPerCompBox;
    DJP2BoxColourSpec  m_colourSpecBox;
    DJP2BoxPalette     m_paletteBox;
    DJP2BoxCompMapping m_compMappingBox;
    DJP2BoxResolution  m_resolutionBox;

    FixedBuffer<bool>  m_isChannelPaletteMappingConformant;

    void CheckPaletteMappingConformance()
    {
        if(m_compMappingBox.IsEmpty() && m_paletteBox.IsEmpty())
        {
            m_isChannelPaletteMappingConformant.Free();
            return;
        }

        unsigned int nOfChannels = m_compMappingBox.NOfChannels();

        m_isChannelPaletteMappingConformant.ReAlloc(nOfChannels);

        for(unsigned int channel = 0; channel < nOfChannels; channel++)
            m_isChannelPaletteMappingConformant[channel] = false;

        if((!m_compMappingBox.IsEmpty()) && m_paletteBox.IsEmpty())
        {
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxPaletteAbsence>());
        }
        else if(m_compMappingBox.IsEmpty() && (!m_paletteBox.IsEmpty()))
        {
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxCompMappingAbsence>());
        }
        else
        {
            for(unsigned int channel = 0; channel < nOfChannels; channel++)
            {
                unsigned int paletteColumn = m_compMappingBox.PaletteColumn(channel);

                if(paletteColumn >= m_paletteBox.NOfColumns())
                {
                    m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxComponentMappingPaletteColumnExceedBoxPaletteNOfColumns>());
                }
                else if(m_compMappingBox.MappingType(paletteColumn) != JP2V_PALETTE_MAPPING)
                {
                    m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxComponentMappingNonPaletteMapping>());
                }
                else m_isChannelPaletteMappingConformant[channel] = true;
            }
        }
    }

    void CheckBitsPerCompConformance()
    {
        if(m_imageHeaderBox.IsEqualBitsPerComp() && (!m_bitsPerCompBox.IsEmpty()))
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, SuperfluousBitsPerCompBox>());

        if(!BitsPerComp().IsEmpty() && ImageHeader().IsEqualBitsPerComp())
        {
            // it's warning in any case, but we should find which is appropriate
            bool isEqual = true;

            for(unsigned int component = 0; component < ImageHeader().NOfComponents(); component++)
            {
                if(BitsPerComp().BitsPerComp()[component] != ImageHeader().BitsPerComp())
                {
                    isEqual = false;
                    break;
                }
            }

            if(isEqual)
            {
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, SuperfluousBitsPerCompBox>());
            }
            else
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BitsPerCompMismatchImageHeaderAndBitsPerCompBoxes>());
        }
    }

    //
    // checking of conformance between colour specification
    // and component mapping boxes
    //
    void CheckColorSpecConformance()
    {
        if(!ColourSpec().IsEmpty())
        {
            if(ColourSpec().ColourspaceMeth() == JP2V_ENUMERATED_CS)
            {
                switch(ColourSpec().EnumColourspace())
                {
                case JP2V_GRAYSCALE:

                    if((!CompMapping().IsEmpty())
                      && CompMapping().NOfChannels() != 1
                      )
                        m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, NOfChannelsMismatchColourSpecAndCompMappingBoxes>());

                    break;
                case JP2V_YCC:
                case JP2V_SRGB:

                    if((!CompMapping().IsEmpty())
                      && CompMapping().NOfChannels() != 3
                      )
                        m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, NOfChannelsMismatchColourSpecAndCompMappingBoxes>());

                    break;
                case JP2V_CMYK:

                    if((!CompMapping().IsEmpty())
                      && CompMapping().NOfChannels() != 4
                      )
                        m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, NOfChannelsMismatchColourSpecAndCompMappingBoxes>());

                    break;
                }
            }
            else
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxColourSpecIsNotEnumeratedColourspace>());
        }

    }
};

#endif // __DJP2BOXHEADER_H__
