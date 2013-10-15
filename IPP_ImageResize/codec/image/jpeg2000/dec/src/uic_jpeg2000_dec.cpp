/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2007-2012 Intel Corporation. All Rights Reserved.
//
*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4702 )
#endif
#include "uic_jpeg2000_dec.h"
#include "djp2codestream.h"
#include "stream_input_adapter.h"
#include "diagn_adapter.h"
#include "ompthreads.h"

class UIC::OwnJPEG2000Decoder : public DJP2Codestream<UICAdapterBoundedInputBE>
{
public:
    OwnJPEG2000Decoder() {}

    void AttachDiagnOut(UIC::BaseStreamDiagn &diagnOutput)
    {
        m_diagn.Attach(diagnOutput);
        AttachDiagnOutput(m_diagn);
    }

    void DetachDiagnOut()
    {
        m_diagn.Detach();
    }

    void AttachStream(UIC::BaseStreamInput &stream)
    {
        m_stream.Attach(stream);
        Attach(m_stream);
    }

    void DetachStream()
    {
        m_stream.Detach();
    }

    void Error  (const BDiagnDescr &descriptor)
    {
        m_diagn.Error(descriptor);
    }


protected:
    UICAdapterDiagnOutput<UIC::BaseDiagnDescriptor::jpeg2000_dec> m_diagn;
    UICAdapterBoundedInputBE                                      m_stream;
};

static const UIC::CodecVersion version =
{
    1, //int         major;      /* e.g. 1                               */
    0, //int         minor;      /* e.g. 2                               */
    0, //int         micro;      /* e.g. 3                               */
    0, //int         build;      /* e.g. 10, always >= majorBuild        */
    "Intel (R) JPEG2000 Decoder", //const char* name;       /* e.g. "Intel(R) JPEG Decoder"         */
    "v1.0 beta", //const char* version;    /* e.g. "v1.2 Beta"                     */
    __DATE__  //const char* buildDate;  /* e.g. "Jul 20 99"                     */
};

UIC::JPEG2000Decoder::JPEG2000Decoder()
: m_decoder(0)
{
}

UIC::JPEG2000Decoder::~JPEG2000Decoder()
{
    Close();
}

void UIC::JPEG2000Decoder::AttachDiagnOut(UIC::BaseStreamDiagn &diagnOutput)
{
    if (!m_decoder) return;

    try
    {
        m_decoder->AttachDiagnOut(diagnOutput);
    }
    catch(BDiagnDescr &exception)
    {
        m_decoder->Error(exception);
    }
}

void UIC::JPEG2000Decoder::DetachDiagnOut()
{
    if (!m_decoder) return;

    try
    {
        m_decoder->DetachDiagnOut();
    }
    catch(BDiagnDescr &exception)
    {
        m_decoder->Error(exception);
    }
}


UIC::ExcStatus UIC::JPEG2000Decoder::Init()
{
    if(m_decoder) return ExcStatusOk;

    try
    {
        m_decoder = new OwnJPEG2000Decoder;
        if (!m_decoder)
            return ExcStatusFail;
    }
    catch(BDiagnDescr &exception)
    {
        m_decoder->Error(exception);
        return ExcStatusFail;
    }

    return ExcStatusOk;
}

UIC::ExcStatus UIC::JPEG2000Decoder::Close()
{
    if(m_decoder) {
       delete m_decoder;
       m_decoder = NULL;
    }
    return ExcStatusOk;
}


const UIC::CodecVersion& UIC::JPEG2000Decoder::GetVersion()
{
    return version;
}


UIC::ExcStatus UIC::JPEG2000Decoder::AttachStream(UIC::BaseStreamInput &stream)
{
    if (!m_decoder) return ExcStatusFail;

    try
    {
        m_decoder->AttachStream(stream);
    }
    catch(BDiagnDescr &exception)
    {
        m_decoder->Error(exception);
        return ExcStatusFail;
    }

    return ExcStatusOk;
}


UIC::ExcStatus UIC::JPEG2000Decoder::DetachStream()
{
    if (!m_decoder) return ExcStatusFail;

    try
    {
        m_decoder->DetachStream();
    }
    catch(BDiagnDescr &exception)
    {
        m_decoder->Error(exception);
        return ExcStatusFail;
    }

    return ExcStatusOk;
}


UIC::ExcStatus UIC::JPEG2000Decoder::ReadHeader(
    ImageColorSpec        &colorSpec,
    ImageSamplingGeometry &geometry)
{
    if (!m_decoder) return ExcStatusFail;

    try
    {
        m_decoder->ReadMainHeader();

        const DJP2CSMainHeader &mainHeader = m_decoder->MainHeader();

        Ipp32u nOfComponents = mainHeader.NOfComponents();

        colorSpec.ReAlloc               (nOfComponents);
        colorSpec.SetColorSpecMethod    (Enumerated);
        colorSpec.SetComponentToColorMap(Direct);
        colorSpec.SetEnumColorSpace     (Unknown);

        ::Rect rect = mainHeader.ImageRectRG();

        UIC::Point origin(rect.Origin().X(), rect.Origin().Y());
        UIC::RectSize size(rect.Size().Width(), rect.Size().Height());

        UIC::Rect uicRect(origin, size);

        geometry.ReAlloc(nOfComponents);
        geometry.SetRefGridRect(uicRect);

        const JP2BitsPerComp* jp2Depth = mainHeader.BitsPerComp();

        for(Ipp32u i = 0; i < nOfComponents; i++)
        {
            ImageDataRange range;
            range.SetAsBitDepth32s(jp2Depth[i].BitDepth(), jp2Depth[i].IsSigned());

            colorSpec.DataRange()[i] = range;

            geometry.SampleSize()[i].SetWidth (mainHeader.SampleSizeRG()[i].Width ());
            geometry.SampleSize()[i].SetHeight(mainHeader.SampleSizeRG()[i].Height());
        }
    }
    catch(BDiagnDescr &exception)
    {
        m_decoder->Error(exception);
        return ExcStatusFail;
    }

    return ExcStatusOk;
}


UIC::ExcStatus UIC::JPEG2000Decoder::ReadData(
    const ImageDataPtr   *data,
    const ImageDataOrder &dataOrder)
{
    if (!m_decoder) return ExcStatusFail;

    if (dataOrder.ComponentOrder() != UIC::Plane) return ExcStatusFail;

    try
    {
        while(m_decoder->ReadNextTilePartHeader())
        {
            while(m_decoder->ReadPacket());
        }

        unsigned int nOfComponents = m_decoder->MainHeader().NOfComponents();

        if(dataOrder.DataType() == UIC::T32s)
        {
            ::FixedBuffer<ImageCoreC<Ipp32s, 1> > interfaceImgRef(nOfComponents);

            for(unsigned int i = 0; i < nOfComponents; i++)
            {
                interfaceImgRef[i].SetData(data[i].p32s);
                interfaceImgRef[i].SetLineStep(dataOrder.LineStep()[i]);
            }

            m_decoder->UpdateImageComponents((ImageCoreC<Ipp32s, 1> *)interfaceImgRef);
        }
        else if(dataOrder.DataType() == UIC::T16s)
        {
            ::FixedBuffer<ImageCoreC<Ipp16s, 1> > interfaceImgRef(nOfComponents);

            for(unsigned int i = 0; i < nOfComponents; i++)
            {
                interfaceImgRef[i].SetData(data[i].p16s);
                interfaceImgRef[i].SetLineStep(dataOrder.LineStep()[i]);
            }

            m_decoder->UpdateImageComponents((ImageCoreC<Ipp16s, 1> *)interfaceImgRef);
        }
        else
        {
            return ExcStatusFail;
        }

    }
    catch(BDiagnDescr &exception)
    {
        m_decoder->Error(exception);
        return ExcStatusFail;
    }

    return ExcStatusOk;
}

Ipp32u UIC::JPEG2000Decoder::SetNOfThreads(Ipp32u nOfThreads)
{
    set_num_threads(nOfThreads);
    return NOfThreads();
}

Ipp32u UIC::JPEG2000Decoder::NOfThreads   ()
{
    return get_num_threads();
}

#define TRANSLATE_ERRNUM(ENUM, CODE, ERRNUM) \
template<> DiagnCtxt DiagnDescrCT<ENUM, CODE>::Context() const \
{ \
    return ERRNUM; \
} \
template<> const char* DiagnDescrCT<ENUM, CODE>::GetMessage() const \
{ \
    return #ERRNUM; \
}


TRANSLATE_ERRNUM(DJP2CSWarning, UnknownQuantStyle, UIC::JPEG2000Decoder::Warn_UnknownQuantStyle);
TRANSLATE_ERRNUM(DJP2CSWarning, MCTIsNotApplicableForNOfComponents, UIC::JPEG2000Decoder::Warn_MCTIsNotApplicableForNOfComponents);
TRANSLATE_ERRNUM(DJP2CSWarning, MCTIsNotApplicableForMixedWTTypes, UIC::JPEG2000Decoder::Warn_MCTIsNotApplicableForMixedWTTypes);
TRANSLATE_ERRNUM(DJP2CSWarning, ResamplingUsed, UIC::JPEG2000Decoder::Warn_ResamplingUsed);
TRANSLATE_ERRNUM(DJP2CSWarning, MainHeaderUnknownMarker, UIC::JPEG2000Decoder::Warn_MainHeaderUnknownMarker);
TRANSLATE_ERRNUM(DJP2CSWarning, MainHeaderExtraCOD, UIC::JPEG2000Decoder::Warn_MainHeaderExtraCOD);
TRANSLATE_ERRNUM(DJP2CSWarning, MainHeaderExtraQCD, UIC::JPEG2000Decoder::Warn_MainHeaderExtraQCD);
TRANSLATE_ERRNUM(DJP2CSWarning, MainHeaderExtraCOC, UIC::JPEG2000Decoder::Warn_MainHeaderExtraCOC);
TRANSLATE_ERRNUM(DJP2CSWarning, MainHeaderExtraQCC, UIC::JPEG2000Decoder::Warn_MainHeaderExtraQCC);
TRANSLATE_ERRNUM(DJP2CSWarning, MainHeaderExtraPOC, UIC::JPEG2000Decoder::Warn_MainHeaderExtraPOC);
TRANSLATE_ERRNUM(DJP2CSWarning, MainHeaderOnlyInCodestream, UIC::JPEG2000Decoder::Warn_MainHeaderOnlyInCodestream);
TRANSLATE_ERRNUM(DJP2CSWarning, TilePartHeaderUnknownMarker, UIC::JPEG2000Decoder::Warn_TilePartHeaderUnknownMarker);
TRANSLATE_ERRNUM(DJP2CSWarning, TilePartHeaderExtraCOD, UIC::JPEG2000Decoder::Warn_TilePartHeaderExtraCOD);
TRANSLATE_ERRNUM(DJP2CSWarning, TilePartHeaderExtraQCD, UIC::JPEG2000Decoder::Warn_TilePartHeaderExtraQCD);
TRANSLATE_ERRNUM(DJP2CSWarning, TilePartHeaderExtraCOC, UIC::JPEG2000Decoder::Warn_TilePartHeaderExtraCOC);
TRANSLATE_ERRNUM(DJP2CSWarning, TilePartHeaderExtraQCC, UIC::JPEG2000Decoder::Warn_TilePartHeaderExtraQCC);
TRANSLATE_ERRNUM(DJP2CSWarning, TilePartHeaderExtraPOC, UIC::JPEG2000Decoder::Warn_TilePartHeaderExtraPOC);
TRANSLATE_ERRNUM(DJP2CSWarning, ActualMarkerLengthBelowSpecified, UIC::JPEG2000Decoder::Warn_ActualMarkerLengthBelowSpecified);
TRANSLATE_ERRNUM(DJP2CSWarning, SpecifiedMarkerLengthExceedContainerTail, UIC::JPEG2000Decoder::Warn_SpecifiedMarkerLengthExceedContainerTail);
TRANSLATE_ERRNUM(DJP2CSWarning, TLMUsed, UIC::JPEG2000Decoder::Warn_TLMUsed);
TRANSLATE_ERRNUM(DJP2CSWarning, PLMUsed, UIC::JPEG2000Decoder::Warn_PLMUsed);
TRANSLATE_ERRNUM(DJP2CSWarning, CRGUsed, UIC::JPEG2000Decoder::Warn_CRGUsed);
TRANSLATE_ERRNUM(DJP2CSWarning, COMUsed, UIC::JPEG2000Decoder::Warn_COMUsed);
TRANSLATE_ERRNUM(DJP2CSWarning, PLTUsed, UIC::JPEG2000Decoder::Warn_PLTUsed);
TRANSLATE_ERRNUM(DJP2CSWarning, SOTTilePartIndexMismatch, UIC::JPEG2000Decoder::Warn_SOTTilePartIndexMismatch);
TRANSLATE_ERRNUM(DJP2CSWarning, SOTNOfTilePartsMismatch, UIC::JPEG2000Decoder::Warn_SOTNOfTilePartsMismatch);
TRANSLATE_ERRNUM(DJP2CSWarning, SOTTilePartLengthLessHeaderSize, UIC::JPEG2000Decoder::Warn_SOTTilePartLengthLessHeaderSize);
TRANSLATE_ERRNUM(DJP2CSWarning, SOTTilePartLengthLessActualLength, UIC::JPEG2000Decoder::Warn_SOTTilePartLengthLessActualLength);
TRANSLATE_ERRNUM(DJP2CSWarning, SOTTilePartLengthExceedActualLength, UIC::JPEG2000Decoder::Warn_SOTTilePartLengthExceedActualLength);
TRANSLATE_ERRNUM(DJP2CSWarning, ActualNOfTilePartsExceedSpecifiedInSOT, UIC::JPEG2000Decoder::Warn_ActualNOfTilePartsExceedSpecifiedInSOT);
TRANSLATE_ERRNUM(DJP2CSWarning, CodeStreamBodyUnknownMarker, UIC::JPEG2000Decoder::Warn_CodeStreamBodyUnknownMarker);

TRANSLATE_ERRNUM(DEBCOTWarning, CodeBlockLayerDamage, UIC::JPEG2000Decoder::Warn_CodeBlockLayerDamage);
TRANSLATE_ERRNUM(DEBCOTWarning, CodeBlockLayerNOfPassesMissmatch, UIC::JPEG2000Decoder::Warn_CodeBlockLayerNOfPassesMissmatch);
TRANSLATE_ERRNUM(DEBCOTWarning, CodeBlockNOfSignificantBitsExceedImplementationLimit, UIC::JPEG2000Decoder::Warn_CodeBlockNOfSignificantBitsExceedImplementationLimit);
TRANSLATE_ERRNUM(DEBCOTWarning, CodeBlockNOfSignificantBitsBelowZero, UIC::JPEG2000Decoder::Warn_CodeBlockNOfSignificantBitsBelowZero);

TRANSLATE_ERRNUM(DJP2MarkerWarning, CODOrCOCZeroPrecStepOrderWidth, UIC::JPEG2000Decoder::Warn_CODOrCOCZeroPrecStepOrderWidth);
TRANSLATE_ERRNUM(DJP2MarkerWarning, CODOrCOCZeroPrecStepOrderHeight, UIC::JPEG2000Decoder::Warn_CODOrCOCZeroPrecStepOrderHeight);
TRANSLATE_ERRNUM(DJP2MarkerWarning, CODOrCOCOrPOCZeroNOfLayers, UIC::JPEG2000Decoder::Warn_CODOrCOCOrPOCZeroNOfLayers);
TRANSLATE_ERRNUM(DJP2MarkerWarning, UnknonwRsiz, UIC::JPEG2000Decoder::Warn_UnknonwRsiz);
TRANSLATE_ERRNUM(DJP2MarkerWarning, SIZTileGridOriginGreaterImageOrigin, UIC::JPEG2000Decoder::Warn_SIZTileGridOriginGreaterImageOrigin);
TRANSLATE_ERRNUM(DJP2MarkerWarning, SIZBitDepthExceedBounds, UIC::JPEG2000Decoder::Warn_SIZBitDepthExceedBounds);
TRANSLATE_ERRNUM(DJP2MarkerWarning, UnknownMCTUsage, UIC::JPEG2000Decoder::Warn_UnknownMCTUsage);

TRANSLATE_ERRNUM(DPHWarning, DPHBadPacketHeaderPadding, UIC::JPEG2000Decoder::Warn_BadPacketHeaderPadding);

TRANSLATE_ERRNUM(DJP2CSUnsuppException, SOPStyleUsed, UIC::JPEG2000Decoder::Except_SOPStyleUsed);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, EPHStyleUsed, UIC::JPEG2000Decoder::Except_EPHStyleUsed);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, PPMUsed, UIC::JPEG2000Decoder::Except_PPMUsed);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, PPTUsed, UIC::JPEG2000Decoder::Except_PPTUsed);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, RGNUsed, UIC::JPEG2000Decoder::Except_RGNUsed);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, POCUsed, UIC::JPEG2000Decoder::Except_POCUsed);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, badCodeblockSize, UIC::JPEG2000Decoder::Except_badCodeblockSize);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, badCodeblockStyle, UIC::JPEG2000Decoder::Except_badCodeblockStyle);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, signedComponent, UIC::JPEG2000Decoder::Except_signedComponent);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, isNotEnumeratedColourspace, UIC::JPEG2000Decoder::Except_isNotEnumeratedColourspace);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, cmapNotSupportedNOfChannels, UIC::JPEG2000Decoder::Except_cmapNotSupportedNOfChannels);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, cmapChannelDirectUse, UIC::JPEG2000Decoder::Except_cmapChannelDirectUse);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, cmapBadComponentNumber, UIC::JPEG2000Decoder::Except_cmapBadComponentNumber);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, cmapBadPaletteComponentIndex, UIC::JPEG2000Decoder::Except_cmapBadPaletteComponentIndex);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, pclrBadNOfPaletteEntries, UIC::JPEG2000Decoder::Except_pclrBadNOfPaletteEntries);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, pclrBadNOfPaletteChannels, UIC::JPEG2000Decoder::Except_pclrBadNOfPaletteChannels);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, pclrBadPaletteBitDepth, UIC::JPEG2000Decoder::Except_pclrBadPaletteBitDepth);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, NonterminatedMultilayer, UIC::JPEG2000Decoder::Except_NonterminatedMultilayer);
TRANSLATE_ERRNUM(DJP2CSUnsuppException, mctIsResampled, UIC::JPEG2000Decoder::Except_mctIsResampled);

TRANSLATE_ERRNUM(DJP2CSException, SOTTileIndexExceedNOfTiles, UIC::JPEG2000Decoder::Except_SOTTileIndexExceedNOfTiles);
TRANSLATE_ERRNUM(DJP2CSException, SOCNotFound, UIC::JPEG2000Decoder::Except_SOCNotFound);
TRANSLATE_ERRNUM(DJP2CSException, SIZNotFound, UIC::JPEG2000Decoder::Except_SIZNotFound);
TRANSLATE_ERRNUM(DJP2CSException, COCIndexExceedNOfComponents, UIC::JPEG2000Decoder::Except_COCIndexExceedNOfComponents);
TRANSLATE_ERRNUM(DJP2CSException, QCCIndexExceedNOfComponents, UIC::JPEG2000Decoder::Except_QCCIndexExceedNOfComponents);
TRANSLATE_ERRNUM(DJP2CSException, CODNotFound, UIC::JPEG2000Decoder::Except_CODNotFound);
TRANSLATE_ERRNUM(DJP2CSException, QCDNotFound, UIC::JPEG2000Decoder::Except_QCDNotFound);
TRANSLATE_ERRNUM(DJP2CSException, OCDOrQCCLackOfQuantValues, UIC::JPEG2000Decoder::Except_OCDOrQCCLackOfQuantValues);


TRANSLATE_ERRNUM(DJP2MarkerException, NegativeBodyLen, UIC::JPEG2000Decoder::Except_NegativeBodyLen);
TRANSLATE_ERRNUM(DJP2MarkerException, SIZGeometryTooLage, UIC::JPEG2000Decoder::Except_SIZGeometryTooLage);
TRANSLATE_ERRNUM(DJP2MarkerException, SIZImageNeagativeSize, UIC::JPEG2000Decoder::Except_SIZImageNeagativeSize);
TRANSLATE_ERRNUM(DJP2MarkerException, BadCODProgressionOrder, UIC::JPEG2000Decoder::Except_BadCODProgressionOrder);
TRANSLATE_ERRNUM(DJP2MarkerException, BadCODOrCOCNumberOfDecompositionLevels, UIC::JPEG2000Decoder::Except_BadCODOrCOCNumberOfDecompositionLevels);
TRANSLATE_ERRNUM(DJP2MarkerException, BadCODOrCOCCodeBlockWidth, UIC::JPEG2000Decoder::Except_BadCODOrCOCCodeBlockWidth);
TRANSLATE_ERRNUM(DJP2MarkerException, BadCODOrCOCCodeBlockHeight, UIC::JPEG2000Decoder::Except_BadCODOrCOCCodeBlockHeight);
TRANSLATE_ERRNUM(DJP2MarkerException, BadCODOrCOCTransformationType, UIC::JPEG2000Decoder::Except_BadCODOrCOCTransformationType);
TRANSLATE_ERRNUM(DJP2MarkerException, POCResLevelIndexExceedBound, UIC::JPEG2000Decoder::Except_POCResLevelIndexExceedBound);
TRANSLATE_ERRNUM(DJP2MarkerException, POCCompIndexExceedNOfComponents, UIC::JPEG2000Decoder::Except_POCCompIndexExceedNOfComponents);
TRANSLATE_ERRNUM(DJP2MarkerException, POCInverseResLevelProgr, UIC::JPEG2000Decoder::Except_POCInverseResLevelProgr);
TRANSLATE_ERRNUM(DJP2MarkerException, POCInverseCompProgr, UIC::JPEG2000Decoder::Except_POCInverseCompProgr);


TRANSLATE_ERRNUM(ByteInputException, noDataToRead, UIC::JPEG2000Decoder::Except_noDataToRead);

TRANSLATE_ERRNUM(MemoryException, faultMalloc, UIC::JPEG2000Decoder::Except_faultMalloc);
