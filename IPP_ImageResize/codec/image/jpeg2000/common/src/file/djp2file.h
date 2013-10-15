
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

#ifndef __DJP2FILE_H__
#define __DJP2FILE_H__

#include "djp2boxheader.h"
#include "djp2codestream.h"
#include "djp2boxuuid.h"
#include "djp2filewarning.h"
#include "djp2fileexception.h"
#include "byteinput.h"
#include "bdiagnoutput.h"

template<class ByteInput>
class DJP2File
{
public:
    DJP2File() : m_isInCodestream(false) {}

    void Attach(ByteInput &stream) { m_stream = &stream; }

    void AttachDiagnOutput(BDiagnOutput &diagnOutput)
    {
        m_diagnOutputPtr = diagnOutput;
        m_codestreamBox.AttachDiagnOutput(diagnOutput);
    }

    // reads the signature the file type and the header boxes
    void ReadIntroBoxes()
    {
        ReadSignatureBox();
        ReadFileTypeBox();
        FindAndReadHeaderBox();
        m_isInCodestream = false;
    }

    // seeks the next codestream box (skipping all the unknown boxes)
    // and reads codestream main header

    bool ReadNextCSMainHeader()
    {
        ByteInput &stream = *m_stream;

        if(m_isInCodestream)
        {
            if(stream.PopSize())
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ActualBoxLengthBelowSpecified>());
            m_isInCodestream = false;
        }

        if(!stream.TailSize()) return false;

        JP2BoxType   boxType;
        unsigned int bodySize;

        ReadBoxHeader(stream, boxType, bodySize);

        if(stream.PushSize(bodySize) != bodySize)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, SpecifiedBoxLengthExceedContainerTail>());

        while (boxType != jp2c)
        {
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2FileWarning, UnexpectedBoxIsIgnored>());

            stream.PopSize();

            if(!stream.TailSize()) return false;

            ReadBoxHeader(stream, boxType, bodySize);

            if(stream.PushSize(bodySize) != bodySize)
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, SpecifiedBoxLengthExceedContainerTail>());

            m_isInCodestream = true;
        }

        m_codestreamBox.Attach(stream);

        m_codestreamBox.ReadMainHeader();
        CheckCSMainHeaderConformity();

        return true;
    }

    bool ReadCSNextTilePartHeader()
    {
        return m_codestreamBox.ReadNextTilePartHeader();
    }

    bool ReadCSPacket()
    {
        return m_codestreamBox.ReadPacket();
    }

    Rect CSComponentRect(unsigned int component) const { return m_codestreamBox.ComponentRect(component); }

    template<class T>
        void UpdateCSImageComponents(const ImageCoreC<T, 1> *dst)
    {
        m_codestreamBox.UpdateImageComponents(dst);
    }


    const DJP2CSMainHeader &CSMainHeader() const { return m_codestreamBox.MainHeader(); }
    const DJP2BoxHeader    &HeaderBox()    const { return m_headerBox;                  }
    const DJP2BoxUUID      &UUIDBox()      const { return m_uuidBox;                    }

protected:
    void ReadSignatureBox()
    {
        ByteInput &stream = *m_stream;

        JP2BoxType boxType;

        {
        unsigned int bodySize;
        ReadBoxHeader(stream, boxType, bodySize);
        if(stream.PushSize(bodySize) != bodySize)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, SpecifiedBoxLengthExceedContainerTail>());
        }

        if (boxType != jp__)
            throw DiagnDescrCT<DJP2FileException,signatureBoxNotFound>();

        if (stream.Read32u() != jp2id)
            throw DiagnDescrCT<DJP2FileException,signatureBoxNotRight>();

        if(stream.PopSize())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ActualBoxLengthBelowSpecified>());
    }

    void ReadFileTypeBox()
    {
        ByteInput &stream = *m_stream;

        JP2BoxType boxType;

        {
        unsigned int bodySize;
        ReadBoxHeader(stream, boxType, bodySize);
        if(stream.PushSize(bodySize) != bodySize)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, SpecifiedBoxLengthExceedContainerTail>());
        }

        if (boxType != ftyp)
            throw DiagnDescrCT<DJP2FileException,fileTypeBoxNotFound>();

        Ipp32u brand = stream.Read32u();

        if (stream.Read32u())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2FileWarning, NonZeroFileTypeBoxMinorVersion>());

        unsigned int nOfCL = stream.TailSize() >> 2;

        bool jp2compatible = false;

        for (unsigned int i = 0; i < nOfCL; i++)
        {
            if(stream.Read32u() == jp2_)
            {
                jp2compatible = true;
                break;
            }
        }

        if (brand != jp2_ && (!jp2compatible))
            throw DiagnDescrCT<DJP2FileException,fileTypeBoxJP2Incompatible>();

        if(stream.PopSize())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ActualBoxLengthBelowSpecified>());
    }

    void FindAndReadHeaderBox()
    {
        ByteInput &stream = *m_stream;

        JP2BoxType   boxType;
        unsigned int bodySize;

        ReadBoxHeader(stream, boxType, bodySize);

        if(stream.PushSize(bodySize) != bodySize)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, SpecifiedBoxLengthExceedContainerTail>());

        while (boxType != jp2h)
        {
            if(boxType == uuid)
                m_uuidBox.Read(stream);
            else
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2FileWarning, UnexpectedBoxIsIgnored>());

            stream.PopSize();

            if(!stream.TailSize())
                throw DiagnDescrCT<DJP2FileException,headerBoxNotFound>();

            ReadBoxHeader(stream, boxType, bodySize);

            if(stream.PushSize(bodySize) != bodySize)
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, SpecifiedBoxLengthExceedContainerTail>());
        }

        m_headerBox.Read(stream);

        if(stream.PopSize())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ActualBoxLengthBelowSpecified>());
    }

    void CheckCSMainHeaderConformity()
    {
        //
        // checking of conformity between codestream and image header boxes
        //
        if(CSMainHeader().NOfComponents() != m_headerBox.ImageHeader().NOfComponents())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2FileWarning, NOfComponentsMismatchImageHeaderAndCodestreamBoxes>());

        if(CSMainHeader().ImageRectRG().Size() != m_headerBox.ImageHeader().ImageSize())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2FileWarning, ImageSizeMismatchImageHeaderAndCodestreamBoxes>());

        if(m_headerBox.ImageHeader().IsEqualBitsPerComp())
        {
            for(unsigned int component = 0; component < CSMainHeader().NOfComponents(); component++)
            {
                if(CSMainHeader().BitsPerComp()[component] != m_headerBox.ImageHeader().BitsPerComp())
                {
                    m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2FileWarning, BitsPerCompMismatchImageHeaderAndCodestreamBoxes>());
                    break;
                }
            }
        }

        //
        // checking of conformity between codestream and bits per component boxes
        //
        if(!m_headerBox.BitsPerComp().IsEmpty())
        {
            unsigned int minNOfComponents = Min(
                CSMainHeader().NOfComponents(),
                m_headerBox.ImageHeader().NOfComponents());

            for(unsigned int component = 0; component < minNOfComponents; component++)
            {
                if(CSMainHeader().BitsPerComp()[component] != m_headerBox.BitsPerComp().BitsPerComp()[component])
                {
                    m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2FileWarning, BitsPerCompMismatchCSMainHeaderAndBitsPerCompBoxes>());
                    break;
                }
            }
        }
    }

    ByteInput                *m_stream;
    BDiagnOutputPtr           m_diagnOutputPtr;
    DJP2BoxHeader             m_headerBox;
    DJP2BoxUUID               m_uuidBox;

    DJP2Codestream<ByteInput> m_codestreamBox;
    bool                      m_isInCodestream;
};

#endif // __DJP2FILE_H__
