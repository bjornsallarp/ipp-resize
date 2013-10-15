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
#pragma warning ( disable : 4702 4530 )
#endif
#include "uic_jp2_dec.h"
#include "djp2file.h"
#include "stream_input_adapter.h"
#include "diagn_adapter.h"
#include "ompthreads.h"

class UIC::OwnJP2Decoder : public DJP2File<UICAdapterBoundedInputBE>
{
public:
    OwnJP2Decoder() {}

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

    void Error  (int code)
    {
        m_diagn.Write(code);
    }

protected:
    UICAdapterDiagnOutput<UIC::BaseDiagnDescriptor::jp2_dec> m_diagn;
    UICAdapterBoundedInputBE                                 m_stream;
};

static const UIC::CodecVersion version =
{
    1, //int         major;      /* e.g. 1                               */
    0, //int         minor;      /* e.g. 2                               */
    0, //int         micro;      /* e.g. 3                               */
    0, //int         build;      /* e.g. 10, always >= majorBuild        */
    "Intel (R) JP2 Decoder", //const char* name;       /* e.g. "Intel(R) JPEG Decoder"         */
    "v1.0 beta", //const char* version;    /* e.g. "v1.2 Beta"                     */
    __DATE__  //const char* buildDate;  /* e.g. "Jul 20 99"                     */
};

UIC::JP2Decoder::JP2Decoder()
: m_decoder(0)
{
}

UIC::JP2Decoder::~JP2Decoder()
{
    Close();
}

void UIC::JP2Decoder::AttachDiagnOut(UIC::BaseStreamDiagn &diagnOutput)
{
    if (!m_decoder) return;

    try
    {
        m_decoder->AttachDiagnOut(diagnOutput);
    }
    catch(BDiagnDescr &exception) { m_decoder->Error(exception); }
}

void UIC::JP2Decoder::DetachDiagnOut()
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


UIC::ExcStatus UIC::JP2Decoder::Init()
{
    try
    {
        m_decoder = new OwnJP2Decoder;
        if (!m_decoder) return ExcStatusFail;
    }
    catch(BDiagnDescr &exception)
    {
        m_decoder->Error(exception);
        return ExcStatusFail;
    }

    return ExcStatusOk;
}

UIC::ExcStatus UIC::JP2Decoder::Close()
{
    if(m_decoder) {
        delete m_decoder;
        m_decoder = NULL;
    }
    return ExcStatusOk;
}


const UIC::CodecVersion& UIC::JP2Decoder::GetVersion()
{
    return version;
}


UIC::ExcStatus UIC::JP2Decoder::AttachStream(UIC::BaseStreamInput &stream)
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


UIC::ExcStatus UIC::JP2Decoder::DetachStream()
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


UIC::ExcStatus UIC::JP2Decoder::ReadHeader(
    ImageColorSpec        &colorSpec,
    ImageSamplingGeometry &geometry)
{
    if (!m_decoder) return ExcStatusFail;

    try
    {

        m_decoder->ReadIntroBoxes();

        if(!m_decoder->ReadNextCSMainHeader())
        {
            m_decoder->Error(Except_missingJP2Codestream);
            return ExcStatusFail;
        }

        const DJP2CSMainHeader &mainHeader = m_decoder->CSMainHeader();

        Ipp32u nOfComponents = mainHeader.NOfComponents();

        colorSpec.ReAlloc               (nOfComponents);
        colorSpec.SetColorSpecMethod    (Enumerated);
        colorSpec.SetComponentToColorMap(Direct);

        if(m_decoder->HeaderBox().ColourSpec().ColourspaceMeth() == JP2V_ENUMERATED_CS)
        {
            switch(m_decoder->HeaderBox().ColourSpec().EnumColourspace())
            {
            case   JP2V_CMYK     : colorSpec.SetEnumColorSpace(CMYK     ); break;
            case   JP2V_SRGB     : colorSpec.SetEnumColorSpace(RGB      ); break;
            case   JP2V_GRAYSCALE: colorSpec.SetEnumColorSpace(Grayscale); break;
            case   JP2V_YCC      : colorSpec.SetEnumColorSpace(YCbCr    ); break;
            default:
                colorSpec.SetEnumColorSpace(Unknown);
            }
        }
        else
        {
            colorSpec.SetEnumColorSpace (Unknown);
        }

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


        ImagePalette &metaPalette = colorSpec.Palette();

        const DJP2BoxHeader &boxHeader = m_decoder->HeaderBox();

        const DJP2BoxCompMapping &boxCompMapping = boxHeader.CompMapping();
        const DJP2BoxPalette     &boxPalette     = boxHeader.Palette();

        if(boxCompMapping.IsEmpty() || boxPalette.IsEmpty())
        {
            metaPalette.Free();
        }
        else
        {
            Ipp16u  nOfChannels       = boxCompMapping.NOfChannels();
            Ipp8u   nOfChannelIndexes = boxPalette.NOfColumns();
            Ipp16u  nOfEntries        = boxPalette.NOfEntries();

            const JP2BitsPerComp* jp2Depth = boxPalette.BitsPerComp();

            RET_ON_EXC(metaPalette.ReAlloc(nOfChannels, nOfChannelIndexes, nOfEntries));

            for(Ipp8u channel = 0; channel < nOfChannels; channel++)
            {
                Ipp16u component = boxCompMapping.CSComponent  (channel);

                metaPalette.MapChannelToComponent(channel, component);

                if(boxHeader.IsChannelPaletteMappingConformant(channel))
                {
                    JP2CompMapType jp2MapType   = boxCompMapping.MappingType  (channel);
                    Ipp8u          channelIndex = boxCompMapping.PaletteColumn(channel);
                    const Ipp64s*  jp2EntryVec  = boxPalette    .Entry(channelIndex);

                    ImageDataRange range;
                    range.SetAsBitDepth32s(jp2Depth[channelIndex].BitDepth(), jp2Depth[channelIndex].IsSigned());

                    metaPalette.SetIsChannelMappedDirectly(channel,      jp2MapType == JP2V_DIRECT_USE);
                    metaPalette.MapChannelToIndex         (channel,      channelIndex);
                    metaPalette.SetDataRange              (channelIndex, range);

                    Copy(jp2EntryVec, (Ipp64s*)metaPalette.LUT(channelIndex), nOfEntries);
                }
                else
                {
                    metaPalette.SetIsChannelMappedDirectly(channel, true);
                }
            }
        }
    }
    catch(BDiagnDescr &exception) { m_decoder->Error(exception); return ExcStatusFail; }

    return ExcStatusOk;
}


UIC::ExcStatus UIC::JP2Decoder::ReadData(
    const ImageDataPtr   *data,
    const ImageDataOrder &dataOrder)
{
    if (!m_decoder) return ExcStatusFail;


    try
    {
        while(m_decoder->ReadCSNextTilePartHeader())
        {
            while(m_decoder->ReadCSPacket());
        }

        unsigned int nOfComponents = m_decoder->CSMainHeader().NOfComponents();

        if(dataOrder.DataType() == UIC::T32s)
        {
            ::FixedBuffer<ImageCoreC<Ipp32s, 1> > interfaceImgRef(nOfComponents);

            for(unsigned int i = 0; i < nOfComponents; i++)
            {
                interfaceImgRef[i].SetData(data[i].p32s);
                interfaceImgRef[i].SetLineStep(dataOrder.LineStep()[i]);
            }

            m_decoder->UpdateCSImageComponents((ImageCoreC<Ipp32s, 1> *)interfaceImgRef);
        }
        else if(dataOrder.DataType() == UIC::T16s)
        {
            ::FixedBuffer<ImageCoreC<Ipp16s, 1> > interfaceImgRef(nOfComponents);

            for(unsigned int i = 0; i < nOfComponents; i++)
            {
                interfaceImgRef[i].SetData(data[i].p16s);
                interfaceImgRef[i].SetLineStep(dataOrder.LineStep()[i]);
            }

            m_decoder->UpdateCSImageComponents((ImageCoreC<Ipp16s, 1> *)interfaceImgRef);
        }
        else
        {
            return ExcStatusFail;
        }
    }
    catch(BDiagnDescr &exception) { m_decoder->Error(exception); return ExcStatusFail; }

    return ExcStatusOk;
}

UIC::ExcStatus UIC::JP2Decoder::GetXMPEXIFDataRef  (
    Ipp8u*                &data,
    Ipp32u                &size)
{
    if (!m_decoder) return ExcStatusFail;

    try
    {
        data = (Ipp8u*)(m_decoder->UUIDBox().XMPEXIFData());
        size = m_decoder->UUIDBox().XMPEXIFData().Size();
    }
    catch(BDiagnDescr &exception) { m_decoder->Error(exception); return ExcStatusFail; }

    return ExcStatusOk;
}

Ipp32u UIC::JP2Decoder::SetNOfThreads(Ipp32u nOfThreads)
{
    set_num_threads(nOfThreads);
    return NOfThreads();
}

Ipp32u UIC::JP2Decoder::NOfThreads   ()
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

TRANSLATE_ERRNUM(DJP2BoxWarning, UnknownSubBoxInHeaderBox, UIC::JP2Decoder::Warn_UnknownSubBoxInHeaderBox);
TRANSLATE_ERRNUM(DJP2BoxWarning, UnknownSubBoxInResolutionBox, UIC::JP2Decoder::Warn_UnknownSubBoxInResolutionBox);
TRANSLATE_ERRNUM(DJP2BoxWarning, ActualBoxLengthBelowSpecified, UIC::JP2Decoder::Warn_ActualBoxLengthBelowSpecified);
TRANSLATE_ERRNUM(DJP2BoxWarning, SpecifiedBoxLengthExceedContainerTail, UIC::JP2Decoder::Warn_SpecifiedBoxLengthExceedContainerTail);
TRANSLATE_ERRNUM(DJP2BoxWarning, ExtraPaletteBox, UIC::JP2Decoder::Warn_ExtraPaletteBox);
TRANSLATE_ERRNUM(DJP2BoxWarning, ExtraCompMappingBox, UIC::JP2Decoder::Warn_ExtraCompMappingBox);
TRANSLATE_ERRNUM(DJP2BoxWarning, ExtraColourSpecBox, UIC::JP2Decoder::Warn_ExtraColourSpecBox);
TRANSLATE_ERRNUM(DJP2BoxWarning, ExtraBitsPerCompBox, UIC::JP2Decoder::Warn_ExtraBitsPerCompBox);
TRANSLATE_ERRNUM(DJP2BoxWarning, ExtraCaptureResolutionBox, UIC::JP2Decoder::Warn_ExtraCaptureResolutionBox);
TRANSLATE_ERRNUM(DJP2BoxWarning, ExtraDisplayResolutionBox, UIC::JP2Decoder::Warn_ExtraDisplayResolutionBox);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxImageHeaderBitDepthExceedBounds, UIC::JP2Decoder::Warn_BoxImageHeaderBitDepthExceedBounds);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxImageHeaderUnknownColorspace, UIC::JP2Decoder::Warn_BoxImageHeaderUnknownColorspace);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxColourSpecUnknownMeth, UIC::JP2Decoder::Warn_BoxColourSpecUnknownMeth);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxColourSpecUnknownEnumeratedColourspace, UIC::JP2Decoder::Warn_BoxColourSpecUnknownEnumeratedColourspace);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxPalleteBitDepthExceedBounds, UIC::JP2Decoder::Warn_BoxPalleteBitDepthExceedBounds);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxBitsPerCompBitDepthExceedBounds, UIC::JP2Decoder::Warn_BoxBitsPerCompBitDepthExceedBounds);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxPalleteZeroNOfEntries, UIC::JP2Decoder::Warn_BoxPalleteZeroNOfEntries);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxPalleteNOfEntriesExceedBounds, UIC::JP2Decoder::Warn_BoxPalleteNOfEntriesExceedBounds);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxComponentMappingPaletteColumnExceedBoxPaletteNOfColumns, UIC::JP2Decoder::Warn_BoxComponentMappingPaletteColumnExceedBoxPaletteNOfColumns);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxComponentMappingNonPaletteMapping, UIC::JP2Decoder::Warn_BoxComponentMappingNonPaletteMapping);
TRANSLATE_ERRNUM(DJP2BoxWarning, SuperfluousBitsPerCompBox, UIC::JP2Decoder::Warn_SuperfluousBitsPerCompBox);
TRANSLATE_ERRNUM(DJP2BoxWarning, NOfChannelsMismatchColourSpecAndCompMappingBoxes, UIC::JP2Decoder::Warn_NOfChannelsMismatchColourSpecAndCompMappingBoxes);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxColourSpecIsNotEnumeratedColourspace, UIC::JP2Decoder::Warn_BoxColourSpecIsNotEnumeratedColourspace);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxPaletteAbsence, UIC::JP2Decoder::Warn_BoxPaletteAbsence);
TRANSLATE_ERRNUM(DJP2BoxWarning, BoxCompMappingAbsence, UIC::JP2Decoder::Warn_BoxCompMappingAbsence);
TRANSLATE_ERRNUM(DJP2BoxWarning, BitsPerCompMismatchImageHeaderAndBitsPerCompBoxes, UIC::JP2Decoder::Warn_BitsPerCompMismatchImageHeaderAndBitsPerCompBoxes);
TRANSLATE_ERRNUM(DJP2BoxWarning, UnknownUUID, UIC::JP2Decoder::Warn_UnknownUUIDBox);

TRANSLATE_ERRNUM(DJP2FileWarning, UnknownBox, UIC::JP2Decoder::Warn_UnknownBox);
TRANSLATE_ERRNUM(DJP2FileWarning, UnexpectedBoxIsIgnored, UIC::JP2Decoder::Warn_UnexpectedBoxIsIgnored);
TRANSLATE_ERRNUM(DJP2FileWarning, NonZeroFileTypeBoxMinorVersion, UIC::JP2Decoder::Warn_NonZeroFileTypeBoxMinorVersion);
TRANSLATE_ERRNUM(DJP2FileWarning, NOfComponentsMismatchImageHeaderAndCodestreamBoxes, UIC::JP2Decoder::Warn_NOfComponentsMismatchImageHeaderAndCodestreamBoxes);
TRANSLATE_ERRNUM(DJP2FileWarning, ImageSizeMismatchImageHeaderAndCodestreamBoxes, UIC::JP2Decoder::Warn_ImageSizeMismatchImageHeaderAndCodestreamBoxes);
TRANSLATE_ERRNUM(DJP2FileWarning, BitsPerCompMismatchImageHeaderAndCodestreamBoxes, UIC::JP2Decoder::Warn_BitsPerCompMismatchImageHeaderAndCodestreamBoxes);
TRANSLATE_ERRNUM(DJP2FileWarning, BitsPerCompMismatchCSMainHeaderAndBitsPerCompBoxes, UIC::JP2Decoder::Warn_BitsPerCompMismatchCSMainHeaderAndBitsPerCompBoxes);

TRANSLATE_ERRNUM(DJP2BoxException, boxOfUnknownType, UIC::JP2Decoder::Except_boxOfUnknownType);
TRANSLATE_ERRNUM(DJP2BoxException, badLBoxValue, UIC::JP2Decoder::Except_badLBoxValue);
TRANSLATE_ERRNUM(DJP2BoxException, badXLBoxValue, UIC::JP2Decoder::Except_badXLBoxValue);
TRANSLATE_ERRNUM(DJP2BoxException, tooLongBoxLen, UIC::JP2Decoder::Except_tooLongBoxLen);
TRANSLATE_ERRNUM(DJP2BoxException, imageHeaderBoxNotFound, UIC::JP2Decoder::Except_imageHeaderBoxNotFound);
TRANSLATE_ERRNUM(DJP2BoxException, imageHeaderBoxSize, UIC::JP2Decoder::Except_imageHeaderBoxSize);
TRANSLATE_ERRNUM(DJP2BoxException, imageHeaderBoxComponentNumber, UIC::JP2Decoder::Except_imageHeaderBoxComponentNumber);
TRANSLATE_ERRNUM(DJP2BoxException, imageHeaderBoxCompressionType, UIC::JP2Decoder::Except_imageHeaderBoxCompressionType);
TRANSLATE_ERRNUM(DJP2BoxException, imageHeaderBoxIntellectualProperty, UIC::JP2Decoder::Except_imageHeaderBoxIntellectualProperty);

TRANSLATE_ERRNUM(DJP2FileException, signatureBoxNotFound, UIC::JP2Decoder::Except_signatureBoxNotFound);
TRANSLATE_ERRNUM(DJP2FileException, signatureBoxNotRight, UIC::JP2Decoder::Except_signatureBoxNotRight);
TRANSLATE_ERRNUM(DJP2FileException, fileTypeBoxNotFound, UIC::JP2Decoder::Except_fileTypeBoxNotFound);
TRANSLATE_ERRNUM(DJP2FileException, fileTypeBoxJP2Incompatible, UIC::JP2Decoder::Except_fileTypeBoxJP2Incompatible);
TRANSLATE_ERRNUM(DJP2FileException, headerBoxNotFound, UIC::JP2Decoder::Except_headerBoxNotFound);
