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
#include "uic_jp2_enc.h"
#include "stream_output_adapter.h"
#include "diagn_adapter.h"
#include "jp2_encode.h"
#include "ompthreads.h"

class UIC::OwnJP2Encoder
{
public:
    OwnJP2Encoder()
    : m_enumCS(JP2V_SRGB)
    , m_depth(0)
    , m_isWritePalette(false)
    , m_maxNOfWTLevels(5)
    , m_isUseMCT(true)
    , m_isWT53Used(true)
    , m_isDerivedQuant(false)
    , m_dstSize(0xFFFFFFFF)
    {}

    void AttachDiagnOut(UIC::BaseStreamDiagn &diagnOutput)
    {
        m_diagn.Attach(diagnOutput);
    }

    void DetachDiagnOut()
    {
        m_diagn.Detach();
    }

    void AttachStream(UIC::BaseStreamOutput &stream)
    {
        m_stream.Attach(stream);
    }

    void DetachStream()
    {
        m_stream.Detach();
    }

    ExcStatus AttachImage(const Image &image)
    {

        const UIC::ImageColorSpec        &imgColorSpec        = image          .ColorSpec();
        const UIC::ImageBuffer           &imgBuffer           = image          .Buffer();
        const UIC::ImageBufferFormat     &imgBufferFormat     = imgBuffer      .BufferFormat();
        const UIC::ImageSamplingGeometry &imgSamplingGeometry = imgBufferFormat.SamplingGeometry();
        const UIC::ImageDataOrder        &imgDataOrder        = imgBufferFormat.DataOrder();
        const UIC::ImageDataPtr          *imgDataPtr          = imgBuffer      .DataPtr();
        const int                        *lineStep            = imgDataOrder   .LineStep();
        // const UIC::ImagePalette          &imgPalette          = imgColorSpec   .Palette();


        if(imgDataOrder.ComponentOrder() != UIC::Plane)
            return ExcStatusFail;

        if(imgDataOrder.DataType() != UIC::T32s)
            return ExcStatusFail;

        switch(imgColorSpec.EnumColorSpace())
        {
        case UIC::CMYK     : m_enumCS = ::JP2V_CMYK     ; break;
        case UIC::RGB      : m_enumCS = ::JP2V_SRGB     ; break;
        case UIC::Grayscale: m_enumCS = ::JP2V_GRAYSCALE; break;
        case UIC::YCbCr    : m_enumCS = ::JP2V_YCC      ; break;
        default:
            return ExcStatusFail;
        }


        m_size.SetWidth (imgSamplingGeometry.RefGridRect().Size().Width ());
        m_size.SetHeight(imgSamplingGeometry.RefGridRect().Size().Height());

        unsigned int nOfComponents = imgSamplingGeometry.NOfComponents();

        unsigned int component;

        unsigned int bitDepthPrev = 0;

        for(component = 0; component < nOfComponents; component++)
        {
            RectSize &sampleSize = imgSamplingGeometry.SampleSize()[component];
            if(sampleSize.Width()  != 1) return ExcStatusFail;
            if(sampleSize.Height() != 1) return ExcStatusFail;

            ImageDataRange &dataRange = imgColorSpec.DataRange()[component];
            if(dataRange.IsSigned())
                return ExcStatusFail;

            if(component)
            {
                if(dataRange.BitDepth() != bitDepthPrev)
                    return ExcStatusFail;
            }

            bitDepthPrev = dataRange.BitDepth();
        }

        m_depth = bitDepthPrev;

        m_imgComponents.ReAlloc(nOfComponents);

        for(component = 0; component < nOfComponents; component++)
        {
            m_imgComponents[component].SetData    (imgDataPtr[component].p32s);
            m_imgComponents[component].SetLineStep(lineStep  [component]);
        }

        return ExcStatusOk;
    }

    void DetachImage()
    {
        m_imgComponents.Free();
        m_paletteEntry .Free();

        m_size   = ::RectSize();
        m_enumCS = JP2V_SRGB;
        m_depth  = 0;
    }

    void SetParams(
        unsigned int maxNOfWTLevels,
        bool         isUseMCT,
        bool         isWT53Used,
        bool         isDerivedQuant,
        int          derivedQuantBaseValueMantissa,
        int          derivedQuantBaseValueExponent,
        unsigned int dstSize)
    {
        m_maxNOfWTLevels        = maxNOfWTLevels;
        m_isUseMCT              = isUseMCT      ;
        m_isWT53Used            = isWT53Used    ;
        m_isDerivedQuant        = isDerivedQuant;
        m_derivedQuantBaseValue = QuantValue(derivedQuantBaseValueMantissa, derivedQuantBaseValueExponent);

        m_dstSize = dstSize;
    }

    void WriteHeader()
    {
        JP2WriteHeader(
            m_stream,
            m_imgComponents.Size(),
            m_size,
            m_enumCS,
            m_depth,

            m_paletteEntry,
            m_paletteEntry.Size(),
            m_isWritePalette,

            m_maxNOfWTLevels,
            m_isUseMCT,
            m_isWT53Used,
            m_isDerivedQuant,
            m_derivedQuantBaseValue);
    }

    void WriteData()
    {
        JP2WriteBody(
            m_stream,
            m_imgComponents,
            m_imgComponents.Size(),
            m_size,
//            m_enumCS,
            m_depth,
            m_maxNOfWTLevels,
            m_isUseMCT,
            m_isWT53Used,
            m_isDerivedQuant,
            m_derivedQuantBaseValue,
            m_dstSize);
    }

    void Error  (const BDiagnDescr &descriptor)
    {
        m_diagn.Error(descriptor);
    }


protected:
    UICAdapterDiagnOutput<UIC::BaseDiagnDescriptor::jp2_enc> m_diagn;
    UICAdapterOutputBE                                       m_stream;

    ::RectSize                                               m_size;
    JP2EnumColourspace                                       m_enumCS;
    unsigned int                                             m_depth;

    FixedArray<ImageCoreC<Ipp32s, 1> >                       m_imgComponents;

    FixedArray<DIBRGBValue>                                  m_paletteEntry;
    bool                                                     m_isWritePalette;

    unsigned int                                             m_maxNOfWTLevels;
    bool                                                     m_isUseMCT;
    bool                                                     m_isWT53Used;
    bool                                                     m_isDerivedQuant;
    QuantValue                                               m_derivedQuantBaseValue;

    unsigned int                                             m_dstSize;
};

static const UIC::CodecVersion version =
{
    1, //int         major;      /* e.g. 1                               */
    0, //int         minor;      /* e.g. 2                               */
    0, //int         micro;      /* e.g. 3                               */
    0, //int         build;      /* e.g. 10, always >= majorBuild        */
    "Intel (R) JP2 Encoder", //const char* name;       /* e.g. "Intel(R) JPEG Encoder"         */
    "v1.0 beta", //const char* version;    /* e.g. "v1.2 Beta"                     */
    __DATE__  //const char* buildDate;  /* e.g. "Jul 20 99"                     */
};

UIC::JP2Encoder::JP2Encoder()
: m_encoder(0)
{
}

UIC::JP2Encoder::~JP2Encoder()
{
    Close();
}

void UIC::JP2Encoder::AttachDiagnOut(BaseStreamDiagn &diagnOutput)
{
    if (!m_encoder) return;

    try
    {
        m_encoder->AttachDiagnOut(diagnOutput);
    }
    catch(BDiagnDescr &exception) { m_encoder->Error(exception); }
}

void UIC::JP2Encoder::DetachDiagnOut()
{
    if (!m_encoder) return;

    try
    {
        m_encoder->DetachDiagnOut();
    }
    catch(BDiagnDescr &exception) { m_encoder->Error(exception);  }
}


UIC::ExcStatus UIC::JP2Encoder::Init()
{
    try
    {
        m_encoder = new UIC::OwnJP2Encoder;
        if (!m_encoder) return ExcStatusFail;
    }
    catch(BDiagnDescr &exception) { m_encoder->Error(exception); return ExcStatusFail; }

    return ExcStatusOk;
}

UIC::ExcStatus UIC::JP2Encoder::Close()
{
    if(m_encoder) {
        delete m_encoder;
        m_encoder = NULL;
    }
    return ExcStatusOk;
}


const UIC::CodecVersion& UIC::JP2Encoder::GetVersion()
{
    return version;
}


UIC::ExcStatus UIC::JP2Encoder::AttachStream(BaseStreamOutput &stream)
{
    if (!m_encoder) return ExcStatusFail;

    try
    {
        m_encoder->AttachStream(stream);
    }
    catch(BDiagnDescr &exception) { m_encoder->Error(exception); return ExcStatusFail; }

    return ExcStatusOk;
}


UIC::ExcStatus UIC::JP2Encoder::DetachStream()
{
    if (!m_encoder) return ExcStatusFail;

    try
    {
        m_encoder->DetachStream();
    }
    catch(BDiagnDescr &exception) { m_encoder->Error(exception); return ExcStatusFail; }

    return ExcStatusOk;
}


UIC::ExcStatus UIC::JP2Encoder::AttachImage(const Image &image)
{
    if (!m_encoder) return ExcStatusFail;

    try
    {
        return m_encoder->AttachImage(image);
    }
    catch(BDiagnDescr &exception)
    {
        m_encoder->Error(exception);
        return ExcStatusFail;
    }

    return ExcStatusOk;
}


UIC::ExcStatus UIC::JP2Encoder::DetachImage()
{
    if (!m_encoder) return ExcStatusFail;

    try
    {
        m_encoder->DetachImage();
    }
    catch(BDiagnDescr &exception) { m_encoder->Error(exception); return ExcStatusFail; }

    return ExcStatusOk;
}

UIC::ExcStatus UIC::JP2Encoder::SetParams(
        unsigned int maxNOfWTLevels,
        bool         isUseMCT,
        bool         isWT53Used,
        bool         isDerivedQuant,
        int          derivedQuantBaseValueMantissa,
        int          derivedQuantBaseValueExponent,
        unsigned int dstSize)
{
    if (!m_encoder) return ExcStatusFail;

    try
    {
        m_encoder->SetParams(
            maxNOfWTLevels,
            isUseMCT,
            isWT53Used,
            isDerivedQuant,
            derivedQuantBaseValueMantissa,
            derivedQuantBaseValueExponent,
            dstSize);
    }
    catch(BDiagnDescr &exception) { m_encoder->Error(exception); return ExcStatusFail; }

    return ExcStatusOk;
}

UIC::ExcStatus UIC::JP2Encoder::WriteHeader()
{
    if (!m_encoder) return ExcStatusFail;

    try
    {
        m_encoder->WriteHeader();
    }
    catch(BDiagnDescr &exception) { m_encoder->Error(exception); return ExcStatusFail; }

    return ExcStatusOk;
}


UIC::ExcStatus UIC::JP2Encoder::WriteData()
{
    if (!m_encoder) return ExcStatusFail;

    try
    {
        m_encoder->WriteData();
    }
    catch(BDiagnDescr &exception) { m_encoder->Error(exception); return ExcStatusFail; }

    return ExcStatusOk;
}

Ipp32u UIC::JP2Encoder::SetNOfThreads(Ipp32u nOfThreads)
{
    set_num_threads(nOfThreads);
    return NOfThreads();
}

Ipp32u UIC::JP2Encoder::NOfThreads   ()
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

TRANSLATE_ERRNUM(ByteOutputException, writeError, UIC::JP2Encoder::Except_writeError);

