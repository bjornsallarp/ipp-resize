/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_JP2_DEC_H__
#define __UIC_JP2_DEC_H__

#include "uic_base_image_decoder.h"

namespace UIC {

class OwnJP2Decoder;

class UICAPI JP2Decoder : public BaseImageDecoder
{

public:
    JP2Decoder();
    virtual ~JP2Decoder();

    virtual ExcStatus           Init               ();
    virtual ExcStatus           Close              ();

    virtual const CodecVersion& GetVersion         ();

    virtual Ipp32u              SetNOfThreads      (Ipp32u nOfThreads);
    virtual Ipp32u              NOfThreads         ();

    virtual void                AttachDiagnOut     (BaseStreamDiagn &diagnOutput);
    virtual void                DetachDiagnOut     ();

    virtual ExcStatus           AttachStream       (BaseStreamInput &stream);

    virtual ExcStatus           DetachStream       ();

    virtual ExcStatus           ReadHeader         (
        ImageColorSpec        &colorSpec,
        ImageSamplingGeometry &geometry);

    virtual ExcStatus           ReadData           (
        const ImageDataPtr    *data,
        const ImageDataOrder  &dataOrder);

    ExcStatus                   GetXMPEXIFDataRef(
        Ipp8u* &data,
        Ipp32u &size);


    // error & warning codes that can be placed to BaseStreamDiagn
    enum {

    // DJP2BoxWarning
    Warn_UnknownSubBoxInHeaderBox,
    Warn_UnknownSubBoxInResolutionBox,

    Warn_ActualBoxLengthBelowSpecified,
    Warn_SpecifiedBoxLengthExceedContainerTail,

    Warn_ExtraPaletteBox,
    Warn_ExtraCompMappingBox,
    Warn_ExtraColourSpecBox,
    Warn_ExtraBitsPerCompBox,
    Warn_ExtraCaptureResolutionBox,
    Warn_ExtraDisplayResolutionBox,

    Warn_BoxImageHeaderBitDepthExceedBounds,
    Warn_BoxImageHeaderUnknownColorspace,
    Warn_BoxColourSpecUnknownMeth,
    Warn_BoxColourSpecUnknownEnumeratedColourspace,
    Warn_BoxPalleteBitDepthExceedBounds,
    Warn_BoxBitsPerCompBitDepthExceedBounds,
    Warn_BoxPalleteZeroNOfEntries,
    Warn_BoxPalleteNOfEntriesExceedBounds,
    Warn_BoxComponentMappingPaletteColumnExceedBoxPaletteNOfColumns,
    Warn_BoxComponentMappingNonPaletteMapping,

// header super box conformity
    Warn_SuperfluousBitsPerCompBox,
    Warn_NOfChannelsMismatchColourSpecAndCompMappingBoxes,
    Warn_BoxColourSpecIsNotEnumeratedColourspace,
    Warn_BoxPaletteAbsence,
    Warn_BoxCompMappingAbsence,

    Warn_BitsPerCompMismatchImageHeaderAndBitsPerCompBoxes,

    Warn_UnknownUUIDBox,

    // DJP2FileWarning
    Warn_UnknownBox,
    Warn_UnexpectedBoxIsIgnored,
    Warn_NonZeroFileTypeBoxMinorVersion,
    Warn_NOfComponentsMismatchImageHeaderAndCodestreamBoxes,
    Warn_ImageSizeMismatchImageHeaderAndCodestreamBoxes,
    Warn_BitsPerCompMismatchImageHeaderAndCodestreamBoxes,
    Warn_BitsPerCompMismatchCSMainHeaderAndBitsPerCompBoxes,

    // DJP2BoxException
    Except_boxOfUnknownType,
    Except_badLBoxValue,
    Except_badXLBoxValue,
    Except_tooLongBoxLen,
    Except_imageHeaderBoxNotFound,
    Except_imageHeaderBoxSize,
    Except_imageHeaderBoxComponentNumber,
    Except_imageHeaderBoxCompressionType,
    Except_imageHeaderBoxIntellectualProperty,

    // DJP2FileException
    Except_signatureBoxNotFound,
    Except_signatureBoxNotRight,
    Except_fileTypeBoxNotFound,
    Except_fileTypeBoxJP2Incompatible,
    Except_headerBoxNotFound,


    //
    Except_missingJP2Codestream

    };

protected:
    OwnJP2Decoder *m_decoder;
};

}

#endif // __UIC_JP2_DEC_H__
