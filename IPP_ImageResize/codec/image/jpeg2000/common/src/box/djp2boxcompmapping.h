
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

#ifndef __DJP2BOXCOMPMAPPING_H__
#define __DJP2BOXCOMPMAPPING_H__

#include "fixedbuffer.h"
#include "jp2const.h"
#include "djp2boxwarning.h"
#include "djp2boxexception.h"
#include "ippdefs.h"
#include "bdiagnoutput.h"

class DJP2BoxCompMapping
{
public:
    DJP2BoxCompMapping() { Reset(); }

    void Reset()
    {
        m_isEmpty = true;
        m_nOfChannels = 0;
        m_csComponent.Free();
        m_mappingType.Free();
        m_paletteColumn.Free();
    }

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    template<class ByteInput>
        void Read(ByteInput &stream)
    {
        if(!m_isEmpty)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ExtraCompMappingBox>());

        unsigned int nOfChannels =  stream.TailSize() >> 2;

        if(nOfChannels > IPP_MAX_16U) m_nOfChannels = IPP_MAX_16U;
        else                          m_nOfChannels = (Ipp16u)nOfChannels;

        m_csComponent  .ReAlloc(m_nOfChannels);
        m_mappingType  .ReAlloc(m_nOfChannels);
        m_paletteColumn.ReAlloc(m_nOfChannels);

        unsigned int channel = 0; for(; channel < m_nOfChannels; channel++)
        {
            m_csComponent  [channel] =                 stream.Read16u();
            m_mappingType  [channel] = (JP2CompMapType)stream.Read8u();
            m_paletteColumn[channel] =                 stream.Read8u();
        }

        m_isEmpty = false;
    }

    bool           IsEmpty      ()                     const { return m_isEmpty;                }
    Ipp16u         NOfChannels  ()                     const { return m_nOfChannels;            }
    Ipp16u         CSComponent  (unsigned int channel) const { return m_csComponent  [channel]; }
    JP2CompMapType MappingType  (unsigned int channel) const { return m_mappingType  [channel]; }
    Ipp8u          PaletteColumn(unsigned int channel) const { return m_paletteColumn[channel]; }

protected:
    bool                        m_isEmpty;
    Ipp16u                      m_nOfChannels;

    FixedBuffer<Ipp16u>         m_csComponent;
    FixedBuffer<JP2CompMapType> m_mappingType;
    FixedBuffer<Ipp8u>          m_paletteColumn;

    BDiagnOutputPtr             m_diagnOutputPtr;
};

#endif // __DJP2BOXCOMPMAPPING_H__
