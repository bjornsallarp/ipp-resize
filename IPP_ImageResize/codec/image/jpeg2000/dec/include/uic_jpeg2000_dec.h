/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_JPEG2000_DEC_H__
#define __UIC_JPEG2000_DEC_H__

#include "uic_base_image_decoder.h"

namespace UIC {

class OwnJPEG2000Decoder;

class UICAPI JPEG2000Decoder : public BaseImageDecoder
{

public:
    JPEG2000Decoder();
    virtual ~JPEG2000Decoder();

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
        const ImageDataPtr   *data,
        const ImageDataOrder &dataOrder);

    // error & warning codes that can be placed to BaseStreamDiagn
    enum {
    // DJP2CSWarning
        Warn_UnknownQuantStyle,
        Warn_MCTIsNotApplicableForNOfComponents,
        Warn_MCTIsNotApplicableForMixedWTTypes,
        Warn_ResamplingUsed,
        Warn_MainHeaderUnknownMarker,
        Warn_MainHeaderExtraCOD,
        Warn_MainHeaderExtraQCD,
        Warn_MainHeaderExtraCOC,
        Warn_MainHeaderExtraQCC,
        Warn_MainHeaderExtraPOC,
        Warn_MainHeaderOnlyInCodestream,
        Warn_TilePartHeaderUnknownMarker,
        Warn_TilePartHeaderExtraCOD,
        Warn_TilePartHeaderExtraQCD,
        Warn_TilePartHeaderExtraCOC,
        Warn_TilePartHeaderExtraQCC,
        Warn_TilePartHeaderExtraPOC,
        Warn_ActualMarkerLengthBelowSpecified,
        Warn_SpecifiedMarkerLengthExceedContainerTail,
        Warn_TLMUsed,
        Warn_PLMUsed,
        Warn_CRGUsed,
        Warn_COMUsed,
        Warn_PLTUsed,
        Warn_SOTTilePartIndexMismatch,
        Warn_SOTNOfTilePartsMismatch,
        Warn_SOTTilePartLengthLessHeaderSize,
        Warn_SOTTilePartLengthLessActualLength,
        Warn_SOTTilePartLengthExceedActualLength,
        Warn_ActualNOfTilePartsExceedSpecifiedInSOT,
        Warn_CodeStreamBodyUnknownMarker,

    // DEBCOTWarning
        Warn_CodeBlockLayerDamage,
        Warn_CodeBlockLayerNOfPassesMissmatch,
        Warn_CodeBlockNOfSignificantBitsExceedImplementationLimit,
        Warn_CodeBlockNOfSignificantBitsBelowZero,

    // DJP2MarkerWarning
        Warn_CODOrCOCZeroPrecStepOrderWidth,
        Warn_CODOrCOCZeroPrecStepOrderHeight,
        Warn_CODOrCOCOrPOCZeroNOfLayers,
        Warn_UnknonwRsiz,
        Warn_SIZTileGridOriginGreaterImageOrigin,
        Warn_SIZBitDepthExceedBounds,
        Warn_UnknownMCTUsage,

    // DPHWarning
        Warn_BadPacketHeaderPadding,

    // DJP2CSUnsuppException
        Except_SOPStyleUsed,
        Except_EPHStyleUsed,
        Except_PPMUsed,
        Except_PPTUsed,
        Except_RGNUsed,
        Except_POCUsed,
        Except_badCodeblockSize,
        Except_badCodeblockStyle,
        Except_signedComponent,
        Except_isNotEnumeratedColourspace,
        Except_cmapNotSupportedNOfChannels,
        Except_cmapChannelDirectUse,
        Except_cmapBadComponentNumber,
        Except_cmapBadPaletteComponentIndex,
        Except_pclrBadNOfPaletteEntries,
        Except_pclrBadNOfPaletteChannels,
        Except_pclrBadPaletteBitDepth,
        Except_NonterminatedMultilayer,
        Except_mctIsResampled,

    // DJP2CSException
        Except_SOTTileIndexExceedNOfTiles,
        Except_SOCNotFound,
        Except_SIZNotFound,
        Except_COCIndexExceedNOfComponents,
        Except_QCCIndexExceedNOfComponents,
        Except_CODNotFound,
        Except_QCDNotFound,
        Except_OCDOrQCCLackOfQuantValues,

    // DJP2MarkerException
        Except_NegativeBodyLen,
        Except_SIZGeometryTooLage,
        Except_SIZImageNeagativeSize,
        Except_BadCODProgressionOrder,
        Except_BadCODOrCOCNumberOfDecompositionLevels,
        Except_BadCODOrCOCCodeBlockWidth,
        Except_BadCODOrCOCCodeBlockHeight,
        Except_BadCODOrCOCTransformationType,
        Except_POCResLevelIndexExceedBound,
        Except_POCCompIndexExceedNOfComponents,
        Except_POCInverseResLevelProgr,
        Except_POCInverseCompProgr,

        Except_noDataToRead,

        Except_faultMalloc

    };

protected:
    OwnJPEG2000Decoder *m_decoder;
};

}

#endif // __UIC_JPEG2000_DEC_H__
