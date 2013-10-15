
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

#ifndef __DJP2BOXRESOLUTION_H__
#define __DJP2BOXRESOLUTION_H__

#include "djp2boxcaptureresolution.h"
#include "djp2boxdisplayresolution.h"
#include "bdiagnoutput.h"

class DJP2BoxResolution
{
public:
    DJP2BoxResolution() : m_isEmpty(true) {}

    void Reset()
    {
        m_isEmpty = true;
        m_capture.Reset();
        m_display.Reset();
    }

    void AttachDiagnOutput(BDiagnOutput &diagnOutput)
    {
        m_diagnOutputPtr = diagnOutput;
        m_capture.AttachDiagnOutput(diagnOutput);
        m_display.AttachDiagnOutput(diagnOutput);
    }

    template<class ByteInput>
        void Read(ByteInput &stream)
    {
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
            case resc: m_capture.Read(stream);  break;
            case resd: m_display.Read(stream);  break;
            default:
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, UnknownSubBoxInResolutionBox>());
            }

            if(stream.PopSize())
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ActualBoxLengthBelowSpecified>());
        }

        m_isEmpty = false;

    }

    const DJP2BoxCaptureResolution &Capture() const { return m_capture; }
    const DJP2BoxDisplayResolution &Display() const { return m_display; }

protected:
    bool m_isEmpty;

    DJP2BoxCaptureResolution m_capture;
    DJP2BoxDisplayResolution m_display;

    BDiagnOutputPtr           m_diagnOutputPtr;
};

#endif // __DJP2BOXRESOLUTION_H__
