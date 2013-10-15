
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

#ifndef __DJP2BOXCOLOURSPEC_H__
#define __DJP2BOXCOLOURSPEC_H__

#include "fixedbuffer.h"
#include "jp2const.h"
#include "djp2boxwarning.h"
#include "bdiagnoutput.h"

class DJP2BoxColourSpec
{
public:
    DJP2BoxColourSpec() { Reset(); }

    void Reset()
    {
        m_isEmpty = true;
        m_enumColourspace = JP2V_GRAYSCALE;
    }

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    template<class ByteInput>
        void Read(ByteInput &stream)
    {
        if(!m_isEmpty)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ExtraColourSpecBox>());

        m_colourspaceMeth = (JP2ColourspaceMethod)stream.Read8u();

        if (m_colourspaceMeth != JP2V_ENUMERATED_CS
         && m_colourspaceMeth != JP2V_ICC_POROFILE)
        {
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxColourSpecUnknownMeth>());
            Reset();
            return;
        }

        stream.Read8u(); // conforming readers shall ignore the value of precedence field
        stream.Read8u(); // conforming readers shall ignore the value of colourspace approximation field

        if (m_colourspaceMeth == JP2V_ENUMERATED_CS)
        {
            Ipp32u enumCS = stream.Read32u();

            if (enumCS != JP2V_CMYK
             && enumCS != JP2V_SRGB
             && enumCS != JP2V_GRAYSCALE
             && enumCS != JP2V_YCC       )
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxColourSpecUnknownEnumeratedColourspace>());

            m_enumColourspace = (JP2EnumColourspace)enumCS;
        }

        m_isEmpty = false;
    }

    bool IsEmpty() const { return m_isEmpty; }

    JP2ColourspaceMethod ColourspaceMeth() const { return m_colourspaceMeth; }
    JP2EnumColourspace   EnumColourspace() const { return m_enumColourspace; }

protected:
    bool                 m_isEmpty;
    JP2EnumColourspace   m_enumColourspace;
    JP2ColourspaceMethod m_colourspaceMeth;
    BDiagnOutputPtr      m_diagnOutputPtr;
};

#endif // __DJP2BOXCOLOURSPEC_H__
