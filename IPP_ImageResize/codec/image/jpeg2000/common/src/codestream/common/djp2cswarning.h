
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

#ifndef __DJP2CSWARNING_H__
#define __DJP2CSWARNING_H__

typedef enum
{
    UnknownQuantStyle,
    MCTIsNotApplicableForNOfComponents,
    MCTIsNotApplicableForMixedWTTypes,
    ResamplingUsed,
    MainHeaderUnknownMarker,
    MainHeaderExtraCOD,
    MainHeaderExtraQCD,
    MainHeaderExtraCOC,
    MainHeaderExtraQCC,
    MainHeaderExtraPOC,
    MainHeaderOnlyInCodestream,
    TilePartHeaderUnknownMarker,
    TilePartHeaderExtraCOD,
    TilePartHeaderExtraQCD,
    TilePartHeaderExtraCOC,
    TilePartHeaderExtraQCC,
    TilePartHeaderExtraPOC,
    ActualMarkerLengthBelowSpecified,
    SpecifiedMarkerLengthExceedContainerTail,
    TLMUsed,
    PLMUsed,
    CRGUsed,
    COMUsed,
    PLTUsed,
    SOTTilePartIndexMismatch,
    SOTNOfTilePartsMismatch,
    SOTTilePartLengthLessHeaderSize,
    SOTTilePartLengthLessActualLength,
    SOTTilePartLengthExceedActualLength,
    ActualNOfTilePartsExceedSpecifiedInSOT,
    CodeStreamBodyUnknownMarker
} DJP2CSWarning;

#endif // __DJP2CSWARNING_H__
