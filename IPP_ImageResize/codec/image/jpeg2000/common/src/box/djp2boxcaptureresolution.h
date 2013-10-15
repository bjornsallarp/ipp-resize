
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __DJP2BOXCAPTURERESOLUTION_H__
#define __DJP2BOXCAPTURERESOLUTION_H__

#include "jp2refgridresolution.h"
#include "bdiagnoutput.h"

class DJP2BoxCaptureResolution : public JP2RefGridResolution
{
public:
    DJP2BoxCaptureResolution() { Reset(); }

    void Reset()
    {
        m_isEmpty = true;
        (*(JP2RefGridResolution*)this) = JP2RefGridResolution();
    }

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    template<class ByteInput>
        void Read(ByteInput &stream)
    {
        if(!m_isEmpty)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ExtraCaptureResolutionBox>());

        ::Read(stream, (*(JP2RefGridResolution*)this));
    }

    bool IsEmpty() const { return m_isEmpty; }

protected:
// change domain to protected for 'Set' functions
    bool            m_isEmpty;
    BDiagnOutputPtr m_diagnOutputPtr;

    using JP2RefGridResolution::SetVerNumerator  ;
    using JP2RefGridResolution::SetVerDenominator;
    using JP2RefGridResolution::SetHorNumerator  ;
    using JP2RefGridResolution::SetHorDenominator;
    using JP2RefGridResolution::SetVerExponent   ;
    using JP2RefGridResolution::SetHorExponent   ;
};

#endif // __DJP2BOXCAPTURERESOLUTION_H__
