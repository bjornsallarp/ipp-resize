/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/
#include "stdafx.h"
#include "uic_image.h"
#include "uic_math.h"
#include "uic_general_algorithm.h"

using namespace UIC;

Ipp32u UIC::NOfColors(ImageEnumColorSpace colorspace)
{
    switch(colorspace)
    {
    case Grayscale     : return 1;
    case GrayscaleAlpha: return 2;
    case RGB           : return 3;
    case BGR           : return 3;
    case RGBA          : return 4;
    case BGRA          : return 4;
    case RGBAP         : return 4;
    case BGRAP         : return 4;
    case RGBE          : return 4;
    case YCbCr         : return 3;
    case CMYK          : return 4;
    case CMYKA         : return 5;
    case YCCK          : return 4;
    default:
        break;
    }
    return 0xFFFFFFFF;
}

Ipp32u UIC::NOfBytes(ImageDataType type)
{
    switch(type)
    {
    case T8u : return sizeof(Ipp8u );
    case T8s : return sizeof(Ipp8s );
    case T16u: return sizeof(Ipp16u);
    case T16s: return sizeof(Ipp16s);
    case T16f: return sizeof(Ipp16s);
    case T32u: return sizeof(Ipp32u);
    case T32s: return sizeof(Ipp32s);
    case T32f: return sizeof(Ipp32f);
    case T64u: return sizeof(Ipp64u);
    case T64s: return sizeof(Ipp64s);
    case T64f: return sizeof(Ipp64f);
    }
    return 0xFFFFFFFF;
}


ImageDataRange::ImageDataRange()
{
    m_min.v64u = 0;
    m_max.v64u = 1;
    m_type     = T64u;
    m_isSigned = false;
    m_bitDepth = 0;
}


void ImageDataRange::SetAsRangeInt(Ipp64s min, Ipp64s max)
{
    m_min.v64s = min;
    m_max.v64s = max;

    if(min < 0)
    {
        m_isSigned = true;
        m_bitDepth = ::BitDepth64(::Max(-(min + 1), max));
    }
    else
    {
        m_isSigned = false;
        m_bitDepth = ::BitDepth64(::Max(min, max));
    }
}


void ImageDataRange::SetAsRangeUInt(Ipp64u max)
{
    m_min.v64u = 0;
    m_max.v64u = max;
    m_isSigned = false;
    m_bitDepth = ::BitDepth64(max);
}


void ImageDataRange::SetAsRange8u (Ipp8u  max) { m_type = T8u ; SetAsRangeUInt(max); }
void ImageDataRange::SetAsRange16u(Ipp16u max) { m_type = T16u; SetAsRangeUInt(max); }
void ImageDataRange::SetAsRange16f(Ipp16u max) { m_type = T16f; SetAsRangeUInt(max); }
void ImageDataRange::SetAsRange32u(Ipp32u max) { m_type = T32u; SetAsRangeUInt(max); }
void ImageDataRange::SetAsRange64u(Ipp64u max) { m_type = T64u; SetAsRangeUInt(max); }


void ImageDataRange::SetAsRange8s (Ipp8s  min, Ipp8s  max) { m_type = T8s ; SetAsRangeInt(min, max); }
void ImageDataRange::SetAsRange16s(Ipp16s min, Ipp16s max) { m_type = T16s; SetAsRangeInt(min, max); }
void ImageDataRange::SetAsRange32s(Ipp32s min, Ipp32s max) { m_type = T32s; SetAsRangeInt(min, max); }
void ImageDataRange::SetAsRange64s(Ipp64s min, Ipp64s max) { m_type = T64s; SetAsRangeInt(min, max); }


void ImageDataRange::SetAsRange32f(Ipp32f min, Ipp32f max)
{
    m_min.v32f = min;
    m_max.v32f = max;
    m_type     = T32f;

    if(min < 0.)
    {
        m_isSigned = true;

        Ipp32f absMax = ::Max(-(min + 1.f), max);

        if(absMax > IPP_MAX_64S)
            m_bitDepth = 0xFFFFFFFF;
        else
            m_bitDepth = ::BitDepth((Ipp32u)absMax);
    }
    else
    {

        m_isSigned = false;

        Ipp32f absMax = ::Max(min, max);

         if(absMax > 2. * (Ipp32f)IPP_MAX_64S)
            m_bitDepth = 0xFFFFFFFF;
        else
            m_bitDepth = ::BitDepth((Ipp32u)absMax);
   }
}


void ImageDataRange::SetAsRange64f(Ipp64f min, Ipp64f max)
{
    m_min.v32f = (Ipp32f)min;
    m_max.v32f = (Ipp32f)max;
    m_type     = T64f;

    if(min < 0.)
    {
        m_isSigned = true;

        Ipp64f absMax = ::Max(-(min + 1.), max);

        if(absMax > IPP_MAX_64S)
            m_bitDepth = 0xFFFFFFFF;
        else
            m_bitDepth = ::BitDepth((Ipp32u)absMax);
    }
    else
    {

        m_isSigned = false;

        Ipp64f absMax = ::Max(min, max);

         if(absMax > 2. * (Ipp64f)IPP_MAX_64S)
            m_bitDepth = 0xFFFFFFFF;
        else
            m_bitDepth = ::BitDepth((Ipp32u)absMax);
   }
}


void ImageDataRange::SetAsBitDepthUInt(Ipp32u bitDepth)
{
    m_bitDepth = bitDepth;
    m_isSigned = false;

    m_min.v64u = 0;
    m_max.v64u = ((Ipp64u)1 << ((Ipp64u)bitDepth + (Ipp64u)1)) - (Ipp64u)1;
}

void ImageDataRange::SetAsBitDepthInt (Ipp32u bitDepth, bool isSigned)
{
    m_bitDepth = bitDepth;
    m_isSigned = isSigned;

    if(isSigned)
    {
        m_min.v64s = - ((Ipp64s)1 << (Ipp64s)bitDepth);
        m_max.v64s =   ((Ipp64s)1 << (Ipp64s)bitDepth) - (Ipp64s)1;
    }
    else
    {
        m_min.v64s = 0;
        m_max.v64s = ((Ipp64s)1 << ((Ipp64s)bitDepth + (Ipp64s)1)) - (Ipp64s)1;
    }
}

void ImageDataRange::SetAsBitDepth8u (Ipp32u bitDepth) { m_type = T8u ; SetAsBitDepthUInt(bitDepth); }
void ImageDataRange::SetAsBitDepth16u(Ipp32u bitDepth) { m_type = T16u; SetAsBitDepthUInt(bitDepth); }
void ImageDataRange::SetAsBitDepth16f(Ipp32u bitDepth) { m_type = T16f; SetAsBitDepthUInt(bitDepth); }
void ImageDataRange::SetAsBitDepth32u(Ipp32u bitDepth) { m_type = T32u; SetAsBitDepthUInt(bitDepth); }
void ImageDataRange::SetAsBitDepth64u(Ipp32u bitDepth) { m_type = T64u; SetAsBitDepthUInt(bitDepth); }

void ImageDataRange::SetAsBitDepth8s (Ipp32u bitDepth, bool isSigned) { m_type = T8u ; SetAsBitDepthInt(bitDepth, isSigned); }
void ImageDataRange::SetAsBitDepth16s(Ipp32u bitDepth, bool isSigned) { m_type = T16u; SetAsBitDepthInt(bitDepth, isSigned); }
void ImageDataRange::SetAsBitDepth32s(Ipp32u bitDepth, bool isSigned) { m_type = T32u; SetAsBitDepthInt(bitDepth, isSigned); }
void ImageDataRange::SetAsBitDepth64s(Ipp32u bitDepth, bool isSigned) { m_type = T64u; SetAsBitDepthInt(bitDepth, isSigned); }


ExcStatus ImagePalette::ReAlloc(Ipp32u nOfChannels, Ipp32u nOfEnumChannelIndexes, Ipp32u nOfEntries)
{
    RET_ON_EXC(m_component              .ReAlloc(nOfChannels));
    RET_ON_EXC(m_isChannelMappedDirectly.ReAlloc(nOfChannels));
    RET_ON_EXC(m_index                  .ReAlloc(nOfChannels));

    RET_ON_EXC(m_LUT                    .ReAlloc(nOfEnumChannelIndexes, nOfEntries));
    RET_ON_EXC(m_range                  .ReAlloc(nOfEnumChannelIndexes));

    m_nOfChannels = nOfChannels;
    m_nOfEntries  = nOfEntries;
    m_nOfIndexes  = nOfEnumChannelIndexes;

    return ExcStatusOk;
}

void ImagePalette::Free()
{
    m_component              .Free();
    m_isChannelMappedDirectly.Free();
    m_index                  .Free();

    m_LUT                    .Free();
    m_range                  .Free();

    m_nOfChannels = 0;
    m_nOfEntries  = 0;
    m_nOfIndexes  = 0;
}

Ipp32u ImageBufferFormat::NOfBytes(Ipp32u component)
{
    if(m_dataOrder.ComponentOrder() == Interleaved)
    {
        RectSize     period   = SamplingGeometry().Period();
        unsigned int nOfLines = Decimate(SamplingGeometry().RefGridRect(), period).Size().Height();

        return DataOrder().LineStep()[0] * nOfLines;
    }

    return DataOrder().LineStep()[component] * SamplingGeometry().ComponentRect(component).Height();
}


ExcStatus ImageBuffer::ReAlloc(
    const ImageDataOrder        &dataOrder,
    const ImageSamplingGeometry &samplingGeometry)
{
    RET_ON_EXC(m_bufferFormat.SamplingGeometry().CreateCopy(samplingGeometry));
    RET_ON_EXC(m_bufferFormat.DataOrder().CreateCopy(dataOrder, samplingGeometry.NOfComponents()));

    if(m_bufferFormat.DataOrder().ComponentOrder() == Interleaved)
    {
        RET_ON_EXC(m_buffer .ReAlloc(1));
        RET_ON_EXC(m_dataPtr.ReAlloc(1));
        RET_ON_EXC(m_buffer[0].ReAlloc(m_bufferFormat.NOfBytes(0)));
        m_dataPtr[0].p8u = m_buffer[0];
    }
    else
    {
        Ipp32u nOfComponents = m_bufferFormat.SamplingGeometry().NOfComponents();
        RET_ON_EXC(m_buffer .ReAlloc(nOfComponents));
        RET_ON_EXC(m_dataPtr.ReAlloc(nOfComponents));

        for(Ipp32u component = 0; component < nOfComponents; component++)
        {
            RET_ON_EXC(m_buffer[component].ReAlloc(m_bufferFormat.NOfBytes(component)));
            m_dataPtr[component].p8u = m_buffer[component];
        }
    }

    return ExcStatusOk;
}

ExcStatus ImageBuffer::ReAlloc(
          ImageDataType          dataType        ,
          ImageComponentOrder    componentOrder  ,
    const ImageSamplingGeometry &samplingGeometry,
          Ipp32u                 lineAlignOrder  ,
          Ipp32u                 pixelAlignOrder )
{
    RET_ON_EXC(m_bufferFormat.DataOrder().InitAligned(dataType, componentOrder, samplingGeometry, lineAlignOrder, pixelAlignOrder));
    RET_ON_EXC(ReAlloc(m_bufferFormat.DataOrder(), samplingGeometry));

    return ExcStatusOk;
}

ExcStatus ImageBuffer::Attach(
    const ImageDataPtr          *dataPtr,
    const ImageDataOrder        &dataOrder,
    const ImageSamplingGeometry &samplingGeometry)
{
    RET_ON_EXC(m_bufferFormat.SamplingGeometry().CreateCopy(samplingGeometry));
    RET_ON_EXC(m_bufferFormat.DataOrder().CreateCopy(dataOrder, samplingGeometry.NOfComponents()));

    if(m_bufferFormat.DataOrder().ComponentOrder() == Interleaved)
    {
        RET_ON_EXC(m_buffer .ReAlloc(1));
        RET_ON_EXC(m_dataPtr.ReAlloc(1));
        m_dataPtr[0].p8u = dataPtr[0].p8u;
        m_buffer [0].Attach(m_dataPtr[0].p8u);
    }
    else
    {
        Ipp32u nOfComponents = m_bufferFormat.SamplingGeometry().NOfComponents();
        RET_ON_EXC(m_buffer .ReAlloc(nOfComponents));
        RET_ON_EXC(m_dataPtr.ReAlloc(nOfComponents));

        for(Ipp32u component = 0; component < nOfComponents; component++)
        {
            m_dataPtr[component].p8u = dataPtr[component].p8u;
            m_buffer [component].Attach(m_dataPtr[component].p8u);
        }
    }

    return ExcStatusOk;
}


void ImageBuffer::FreeOrDetach()
{
    m_bufferFormat.SamplingGeometry().Free();
    m_buffer .Free();
    m_dataPtr.Free();
}

void ImageSamplingGeometry::ReduceByGCD()
{
    RectSize gcd = GCD();
    for(unsigned int i = 0; i < NOfComponents(); i++)
    {
        m_sampleSize[i].SetWidth (m_sampleSize[i].Width () / gcd.Width ());
        m_sampleSize[i].SetHeight(m_sampleSize[i].Height() / gcd.Height());
    }
    m_refGridRect = Decimate(m_refGridRect, gcd);
}

RectSize ImageSamplingGeometry::GCD() const
{
    if(!NOfComponents()) return RectSize(1,1);

    unsigned int xGCD = m_sampleSize[0].Width ();
    unsigned int yGCD = m_sampleSize[0].Height();

    for(unsigned int i = 1; i < NOfComponents(); i++)
    {
        xGCD = ::GCD(xGCD, m_sampleSize[i].Width ());
        yGCD = ::GCD(yGCD, m_sampleSize[i].Height());
    }

    return RectSize(xGCD, yGCD);
}

RectSize ImageSamplingGeometry::Period() const
{
    unsigned int nOfSamplesPerBlockX = 1;
    unsigned int nOfSamplesPerBlockY = 1;

    unsigned int minSampleW = IPP_MAX_32U;
    unsigned int minSampleH = IPP_MAX_32U;

    unsigned int nOfComponents = NOfComponents();

    for(unsigned int i = 0; i < nOfComponents; i++)
    {
        unsigned int sampleW = m_sampleSize[i].Width ();
        unsigned int sampleH = m_sampleSize[i].Height();

        nOfSamplesPerBlockX *= sampleW;
        nOfSamplesPerBlockY *= sampleH;

        minSampleW = Min(minSampleW, sampleW);
        minSampleH = Min(minSampleH, sampleH);
    }

    nOfSamplesPerBlockX /= minSampleW;
    nOfSamplesPerBlockY /= minSampleH;

    return RectSize(nOfSamplesPerBlockX, nOfSamplesPerBlockY);
}

ExcStatus ImageSamplingGeometry::SetEnumSampling(ImageEnumSampling enumSampling)
{
    static const RectSize RECT_1x1 = RectSize(1, 1);
    static const RectSize RECT_1x2 = RectSize(1, 2);
    static const RectSize RECT_2x1 = RectSize(2, 1);
    static const RectSize RECT_2x2 = RectSize(2, 2);

    RectSize* size = SampleSize();

    unsigned int nOfComponents = NOfComponents();

    switch(enumSampling)
    {
    case S444:
        {
          for(unsigned int i = 0; i < nOfComponents; i++)
            size[i] = RECT_1x1;
        }
        break;

    case S422:
        switch(nOfComponents)
        {
        case 4:
            size[3] = RECT_2x1;
        case 3:
            size[0] = RECT_2x1;
            size[1] = RECT_1x1;
            size[2] = RECT_1x1;
            break;

        default:
            return ExcStatusFail;
        }
        break;

    case S244:
        switch(nOfComponents)
        {
        case 4:
            size[3] = RECT_1x2;
        case 3:
            size[0] = RECT_1x2;
            size[1] = RECT_1x1;
            size[2] = RECT_1x1;
            break;

        default:
            return ExcStatusFail;
        }
        break;

    case S411:
        switch(nOfComponents)
        {
        case 4:
            size[3] = RECT_2x2;
        case 3:
            size[0] = RECT_2x2;
            size[1] = RECT_1x1;
            size[2] = RECT_1x1;
            break;

        default:
            return ExcStatusFail;
        }
        break;

    default:
        return ExcStatusFail;
    }

    return ExcStatusOk;
}


ImageEnumSampling ImageSamplingGeometry::EnumSampling() const
{
    RectSize* size = SampleSize();

    ImageEnumSampling enumSampling;

    switch(NOfComponents())
    {
    case 1:
        enumSampling = S444;
        break;

    case 3:
        if(size[0].Height() == size[1].Height() && size[0].Height() == size[2].Height() &&
            size[0].Width()  == size[1].Width() && size[0].Width() == size[2].Width())
        {
            enumSampling = S444;
        }
        else
        if(size[0].Height() == 2*size[1].Height() && size[0].Height() == 2*size[2].Height() &&
            size[0].Width()  == size[1].Width() && size[0].Width() == size[2].Width())
        {
            enumSampling = S244;
        }
        else
        if(size[0].Height() == size[1].Height() && size[0].Height() == size[2].Height() &&
            size[0].Width()  == 2*size[1].Width() && size[0].Width() == 2*size[2].Width())
        {
            enumSampling = S422;
        }
        else
        if(size[0].Height() == 2*size[1].Height() && size[0].Height() == 2*size[2].Height() &&
            size[0].Width()  == 2*size[1].Width() && size[0].Width() == 2*size[2].Width())
        {
            enumSampling = S411;
        }
        else
        {
            enumSampling = SOther;
        }
        break;

    case 4:
        if(size[0].Height() == size[1].Height() && size[0].Height() == size[2].Height() && size[0].Height() == size[3].Height() &&
            size[0].Width()  == size[1].Width() && size[0].Width() == size[2].Width() && size[0].Width() == size[3].Width())
        {
            enumSampling = S444;
        }
        else
        if(size[0].Height() == 2*size[1].Height() && size[0].Height() == 2*size[2].Height() && size[0].Height() == size[3].Height() &&
            size[0].Width()  == size[1].Width() && size[0].Width() == size[2].Width() && size[0].Width() == size[3].Width())
        {
            enumSampling = S244;
        }
        else
        if(size[0].Height() == size[1].Height() && size[0].Height() == size[2].Height() && size[0].Height() == size[3].Height() &&
            size[0].Width()  == 2*size[1].Width() && size[0].Width() == 2*size[2].Width() && size[0].Width() == size[3].Width())
        {
            enumSampling = S422;
        }
        else
        if(size[0].Height() == 2*size[1].Height() && size[0].Height() == 2*size[2].Height() && size[0].Height() == size[3].Height() &&
            size[0].Width()  == 2*size[1].Width() && size[0].Width() == 2*size[2].Width() && size[0].Width() == size[3].Width())
        {
            enumSampling = S411;
        }
        else
        {
            enumSampling = SOther;
        }
        break;

    default:
        enumSampling = SOther;
        break;
    } // switch()

    return enumSampling;
} // EnumSampling()

void ImageDataOrder::SetAligned(const ImageSamplingGeometry &samplingGeometry, Ipp32u lineAlignOrder, Ipp32u pixelAlignOrder)
{
    unsigned int nOfComponents = samplingGeometry.NOfComponents();
    if(!nOfComponents) return;

    if(m_componentOrder == Interleaved)
    {
        RectSize     period          = samplingGeometry.Period();
        unsigned int nOfBlocksInLine = Decimate(samplingGeometry.RefGridRect(), period).Size().Width();

        unsigned int nOfValuesInBlock           = 0;

        for(unsigned int i = 0; i < nOfComponents; i++)
        {
            unsigned int sampleW = samplingGeometry.SampleSize()[i].Width ();
            unsigned int sampleH = samplingGeometry.SampleSize()[i].Height();

            unsigned int compNOfSamplesPerBlockX = period.Width () / sampleW;
            unsigned int compNOfSamplesPerBlockY = period.Height() / sampleH;

            nOfValuesInBlock += compNOfSamplesPerBlockX * compNOfSamplesPerBlockY;
        }

        int pixelStep = AlignPow2(nOfValuesInBlock * NOfBytes(m_dataType), pixelAlignOrder);
        int lineStep  = AlignPow2(pixelStep        * nOfBlocksInLine     , lineAlignOrder );

        PixelStep()[0] = pixelStep;
        LineStep ()[0] = lineStep ;
    }
    else
    {
        int pixelStep = AlignPow2(NOfBytes(m_dataType) , pixelAlignOrder);

        for(Ipp32u i = 0; i < nOfComponents; i++)
        {
            int lineStep  = AlignPow2(pixelStep * samplingGeometry.ComponentRect(i).Width(), lineAlignOrder );

            PixelStep()[i] = pixelStep;
            LineStep ()[i] = lineStep ;
        }
    }
}
