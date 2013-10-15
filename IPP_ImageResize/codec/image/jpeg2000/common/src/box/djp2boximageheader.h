
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

#ifndef __DJP2BOXIMAGEHEADER_H__
#define __DJP2BOXIMAGEHEADER_H__

#include "fixedbuffer.h"
#include "jp2bitspercomp.h"
#include "djp2box.h"
#include "djp2boxwarning.h"
#include "djp2boxexception.h"
#include "geometry2d.h"
#include "bdiagnoutput.h"

class DJP2BoxImageHeader
{
public:
    DJP2BoxImageHeader() { Reset(); }

    void Reset()
    {
        m_imageSize = RectSize();
        m_nOfComponents = 0;
        m_bitsPerComp   = 0;
    }

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    template<class ByteInput>
        void Read(ByteInput &stream)
    {
        JP2BoxType                       boxType;

        {
        unsigned int bodySize;
        ReadBoxHeader(stream, boxType, bodySize);
        if(stream.PushSize(bodySize) != bodySize)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, SpecifiedBoxLengthExceedContainerTail>());
        }

        if (boxType != ihdr)
            throw DiagnDescrCT<DJP2BoxException,imageHeaderBoxNotFound>();


        int height = stream.Read32u();
        int width  = stream.Read32u();

        if (height < 1 || width < 1)
            throw DiagnDescrCT<DJP2BoxException,imageHeaderBoxSize>();

        m_imageSize = RectSize(width, height);


        m_nOfComponents = stream.Read16u();

        if (m_nOfComponents < 1)
            throw DiagnDescrCT<DJP2BoxException,imageHeaderBoxComponentNumber>();


        Ipp8u bpc = stream.Read8u();
        m_bitsPerComp = bpc;

        m_isEqualBitsPerComp = (bpc != JP2V_BITS_PER_COMP_VARIOUS);

        if(m_isEqualBitsPerComp && (!m_bitsPerComp.IsConformant()))
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxImageHeaderBitDepthExceedBounds>());


        if (stream.Read8u() != 7)
            throw DiagnDescrCT<DJP2BoxException,imageHeaderBoxCompressionType>();

        if (stream.Read8u() != 0)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxImageHeaderUnknownColorspace>());

        stream.Read8u(); // we do not parse intellectual property field.

        if(stream.PopSize())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ActualBoxLengthBelowSpecified>());
    }

    bool                  IsEqualBitsPerComp() const { return m_isEqualBitsPerComp; }
    const RectSize       &ImageSize()          const { return m_imageSize; }
    Ipp16u                NOfComponents()      const { return m_nOfComponents; }

    // can be used only if IsEqualBitsPerComp() is true
    const JP2BitsPerComp  BitsPerComp()        const { return m_bitsPerComp; }

protected:
    RectSize        m_imageSize;
    Ipp16u          m_nOfComponents;
    JP2BitsPerComp  m_bitsPerComp;
    bool            m_isEqualBitsPerComp;
    BDiagnOutputPtr m_diagnOutputPtr;
};

#endif // __DJP2BOXIMAGEHEADER_H__
