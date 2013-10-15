
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

#ifndef __DJP2BOXWARNING_H__
#define __DJP2BOXWARNING_H__

typedef enum
{
    UnknownSubBoxInHeaderBox,
    UnknownSubBoxInResolutionBox,

    ActualBoxLengthBelowSpecified,
    SpecifiedBoxLengthExceedContainerTail,

    ExtraPaletteBox,
    ExtraCompMappingBox,
    ExtraColourSpecBox,
    ExtraBitsPerCompBox,
    ExtraCaptureResolutionBox,
    ExtraDisplayResolutionBox,

    BoxImageHeaderBitDepthExceedBounds,
    BoxImageHeaderUnknownColorspace,
    BoxColourSpecUnknownMeth,
    BoxColourSpecUnknownEnumeratedColourspace,
    BoxPalleteBitDepthExceedBounds,
    BoxBitsPerCompBitDepthExceedBounds,
    BoxPalleteZeroNOfEntries,
    BoxPalleteNOfEntriesExceedBounds,
    BoxComponentMappingPaletteColumnExceedBoxPaletteNOfColumns,
    BoxComponentMappingNonPaletteMapping,

// header super box conformity
    SuperfluousBitsPerCompBox,
    NOfChannelsMismatchColourSpecAndCompMappingBoxes,
    BoxColourSpecIsNotEnumeratedColourspace,
    BoxPaletteAbsence,
    BoxCompMappingAbsence,

    BitsPerCompMismatchImageHeaderAndBitsPerCompBoxes,

    UnknownUUID

} DJP2BoxWarning;

#endif // __DJP2BOXWARNING_H__
