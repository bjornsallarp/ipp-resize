/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_IMAGE_H__
#define __UIC_IMAGE_H__

#include "uic_fixed_array.h"
#include "uic_geometry.h"
#include "uic_attachable_buffer.h"

namespace UIC {


// Multi-component pixel values order
typedef enum
{
    Plane,
    Interleaved

} ImageComponentOrder;

// symbolic names for most popular sampling factors
// Note, 422 will apply for 3 and 4 channel images 4:2:2 and 4:2:2:4
//       411 will apply for 3 and 4 channel images 4:1:1 and 4:1:1:4
typedef enum
{
    S444,
    S422,
    S244,
    S411,
    SOther

} ImageEnumSampling;

// Colorspaces that can be represented by enumerated values (vs. ICC profile).
// The postfix should contain info about specific (standard reference etc.)
// No postfix for unknown specific (default conversion rules will be applied)
// The order of color channels are actual and the same colorspaces but with
// different order should be enumerated by separate value
typedef enum
{
    Unknown,
    Grayscale,
    GrayscaleAlpha,
    RGB,
    BGR,
    RGBA,
    BGRA,
    RGBAP,
    BGRAP,
    RGBE,
    YCbCr,
    CMYK,
    CMYKA,
    YCCK,

} ImageEnumColorSpace;

Ipp32u UICAPI NOfColors(ImageEnumColorSpace colorspace);

// Colorspace specification method
typedef enum
{
    Enumerated,
    ProfileICC

} ImageColorSpecMethod;

// Image components to color channels mapping
typedef enum
{
    Direct,
    Palette

} ImageComponentToColorMap;


// Image component and color channels value data type enumerator
typedef enum
{
    T8u,
    T8s,
    T16u,
    T16s,
    T16f,
    T32u,
    T32s,
    T32f,
    T64u,
    T64s,
    T64f

} ImageDataType;

Ipp32u UICAPI NOfBytes(ImageDataType type);

// Union for constant image data pointer
class UICAPI ImageDataConstPtr
{
public:
    ImageDataConstPtr() : p8u(0) {}

    union
    {
        const Ipp8u  *p8u;
        const Ipp8s  *p8s;
        const Ipp16u *p16u;
        const Ipp16s *p16s;
        const Ipp32u *p32u;
        const Ipp32s *p32s;
        const Ipp32f *p32f;
        const Ipp64u *p64u;
        const Ipp64s *p64s;
        const Ipp64f *p64f;
    };
};

// Union for image data pointer
class UICAPI ImageDataPtr
{
public:
    ImageDataPtr() : p8u(0) {}

    union
    {
        Ipp8u  *p8u;
        Ipp8s  *p8s;
        Ipp16u *p16u;
        Ipp16s *p16s;
        Ipp32u *p32u;
        Ipp32s *p32s;
        Ipp32f *p32f;
        Ipp64u *p64u;
        Ipp64s *p64s;
        Ipp64f *p64f;
    };
    operator ImageDataConstPtr() const;
};


// Union to store single image data value
typedef union
{
    Ipp8u  v8u;
    Ipp8s  v8s;
    Ipp16u v16u;
    Ipp16s v16s;
    Ipp32u v32u;
    Ipp32s v32s;
    Ipp32f v32f;
    Ipp64u v64u;
    Ipp64s v64s;
    Ipp64f v64f;

} ImageDataValue;


class UICAPI ImageSamplingGeometry
{
public:
    ImageSamplingGeometry() {}

    ExcStatus   ReAlloc(Ipp32u nOfComponents) { return m_sampleSize.ReAlloc(nOfComponents); }
    void        Free()                        { m_sampleSize.Free(); }

    ExcStatus   CreateCopy(const ImageSamplingGeometry &instance)
    {
        if(this == &instance) return ExcStatusOk;

        RET_ON_EXC(m_sampleSize.CreateCopy(instance.m_sampleSize));
        m_refGridRect = instance.RefGridRect();

        return ExcStatusOk;
    }

    void ReduceByGCD();

    // Set image area rectangle in reference grid
    void              SetRefGridRect (const Rect &rect)       { m_refGridRect = rect; }

    // Return pointer to buffer of
    RectSize*         SampleSize     ()                 const { return m_sampleSize; }

    // Rectangle in reference grid
    const Rect&       RefGridRect    ()                 const { return m_refGridRect; }

    // Number of image data components
    Ipp32u            NOfComponents  ()                 const { return m_sampleSize.Size(); }

    // Calculates image rectangle projection for particular component grid
    Rect              ComponentRect  (Ipp32u component) const { return Decimate(m_refGridRect, m_sampleSize[component]); }

    // Calculates greatest common divisor both for horizontal and vertical sample size
    RectSize          GCD            ()                 const;

    // Calculates minimum period of image sampling (ref. grid size between component sampling phases are the same)
    RectSize          Period         ()                 const;

    ExcStatus         SetEnumSampling(ImageEnumSampling enumSampling);
    ImageEnumSampling EnumSampling   ()                 const;

protected:
    Rect                 m_refGridRect;
    FixedArray<RectSize> m_sampleSize;
};


//
// ImageDataOrder
//
// Describes representation of data in memory
//
// "ComponentOrder" is representation of multi-component pixels,
//      it can be "Plane" or "Interleaved"
//
// "LineStep" is step between rows, row alignment in bytes, but it can be negative
//      to specify "flipped" images in vertical dimension
//
// "PixelStep" is a step between pixels in row
//      For example, it can return 4 (32-bit) alignment for interleaved components
//      It can be negative to specify "flipped" images in horizontal dimension
//
class UICAPI ImageDataOrder
{
public:
    ImageDataOrder()
    : m_dataType      (T8u)
    , m_componentOrder(Plane)
    {}

    ExcStatus ReAlloc(ImageComponentOrder componentOrder, Ipp32u nOfComponents)
    {
        m_componentOrder = componentOrder;

        if(componentOrder == Interleaved)
        {
            RET_ON_EXC(m_pixelStep.ReAlloc(1));
            RET_ON_EXC(m_lineStep .ReAlloc(1));
        }
        else
        {
            RET_ON_EXC(m_pixelStep.ReAlloc(nOfComponents));
            RET_ON_EXC(m_lineStep .ReAlloc(nOfComponents));
        }

        return ExcStatusOk;
    }

    ExcStatus CreateCopy(const ImageDataOrder& instance, Ipp32u nOfComponents)
    {
        if(this == &instance) return ExcStatusOk;

        RET_ON_EXC(ReAlloc(instance.ComponentOrder(), nOfComponents));
        SetDataType(instance.DataType());

        if(m_componentOrder == Interleaved)
        {
            PixelStep()[0] = instance.PixelStep()[0];
            LineStep ()[0] = instance.LineStep ()[0];
        }
        else
        {
            Copy(instance.PixelStep(), PixelStep(), nOfComponents);
            Copy(instance.LineStep (), LineStep (), nOfComponents);
        }

        return ExcStatusOk;
    }


    void SetDataType(ImageDataType dataType ) { m_dataType = dataType ; }

    void SetAligned(const ImageSamplingGeometry &samplingGeometry, Ipp32u lineAlignOrder = 5, Ipp32u pixelAlignOrder = 1);

    ExcStatus InitAligned(
              ImageDataType          dataType           ,
              ImageComponentOrder    componentOrder     ,
        const ImageSamplingGeometry &samplingGeometry   ,
              Ipp32u                 lineAlignOrder  = 5,
              Ipp32u                 pixelAlignOrder = 1
        )
    {
        RET_ON_EXC (ReAlloc(componentOrder, samplingGeometry.NOfComponents()));

        SetDataType(dataType);
        SetAligned (samplingGeometry, lineAlignOrder, pixelAlignOrder);

        return ExcStatusOk;
    }

    ImageDataType       DataType      () const { return m_dataType      ; }
    ImageComponentOrder ComponentOrder() const { return m_componentOrder; }

    int                *LineStep      () const { return m_lineStep      ; }
    int                *PixelStep     () const { return m_pixelStep     ; }

protected:
    ImageDataType       m_dataType;
    ImageComponentOrder m_componentOrder;
    FixedBuffer<int>    m_lineStep;
    FixedBuffer<int>    m_pixelStep;
};


// Image components and color channel values range
class UICAPI ImageDataRange
{
public:
    ImageDataRange();

    void SetAsRange8u (Ipp8u  max);
    void SetAsRange8s (Ipp8s  min, Ipp8s  max);
    void SetAsRange16u(Ipp16u max);
    void SetAsRange16s(Ipp16s min, Ipp16s max);
    void SetAsRange16f(Ipp16u max);
    void SetAsRange32u(Ipp32u max);
    void SetAsRange32s(Ipp32s min, Ipp32s max);
    void SetAsRange64u(Ipp64u max);
    void SetAsRange64s(Ipp64s min, Ipp64s max);
    void SetAsRange32f(Ipp32f min, Ipp32f max);
    void SetAsRange64f(Ipp64f min, Ipp64f max);

    void SetAsBitDepth8u (Ipp32u bitDepth);
    void SetAsBitDepth8s (Ipp32u bitDepth, bool isSigned);
    void SetAsBitDepth16u(Ipp32u bitDepth);
    void SetAsBitDepth16s(Ipp32u bitDepth, bool isSigned);
    void SetAsBitDepth16f(Ipp32u bitDepth);
    void SetAsBitDepth32u(Ipp32u bitDepth);
    void SetAsBitDepth32s(Ipp32u bitDepth, bool isSigned);
    void SetAsBitDepth64u(Ipp32u bitDepth);
    void SetAsBitDepth64s(Ipp32u bitDepth, bool isSigned);

    bool                  IsSigned() const { return m_isSigned; }
    Ipp32u                BitDepth() const { return m_bitDepth; }

    const ImageDataValue &Min     () const { return m_min ; }
    const ImageDataValue &Max     () const { return m_max ; }
    const ImageDataType  &DataType() const { return m_type; }

protected:
    void SetAsRangeUInt(Ipp64u max);
    void SetAsRangeInt (Ipp64s min, Ipp64s max);

    void SetAsBitDepthUInt(Ipp32u bitDepth);
    void SetAsBitDepthInt (Ipp32u bitDepth, bool isSigned);

    ImageDataValue m_min;
    ImageDataValue m_max;
    ImageDataType  m_type;
    Ipp32u         m_bitDepth;
    bool           m_isSigned;
};




class UICAPI ImagePalette
{
public:
    ImagePalette()
    : m_nOfChannels(0)
    , m_nOfEntries (0)
    , m_nOfIndexes (0)
    {}

    //////////////////////////////////////////////////////////////////////////
    //
    // Allocate and read/write data in native representation

    ExcStatus ReAlloc(Ipp32u nOfChannels, Ipp32u nOfEnumChannelIndexes, Ipp32u nOfEntries);
    void      Free();

    // Set true if some component is mapped directly to specific color channel
    void                   SetIsChannelMappedDirectly(Ipp32u channel, bool isChannelMappedDirectly) { m_isChannelMappedDirectly[channel] = isChannelMappedDirectly; }

    // Map particular color channel to particular image component
    // Different channels can be mapped to single component
    void                   MapChannelToComponent     (Ipp32u channel, Ipp32u component)             { m_component[channel] = component; }

    // Map particular color channel to enumerated index
    // Different channels can be mapped to the single index as well as to single component
    // (So it supports "same" channels mapping and keep required memory)
    void                   MapChannelToIndex         (Ipp32u channel, Ipp32u index)                 { m_index       [channel] = index; }


    // Set channel data range
    void                   SetDataRange              (Ipp32u index, const ImageDataRange &range)    { m_range       [index] = range; }

    // Returns index mapped to channel
    Ipp32u                 Index                     (Ipp32u channel)                       const   { return m_index[channel]; }

    // Access to look-up table for color channels values
    ImageDataValue        *LUT                       (Ipp32u index)                                 { return m_LUT  [index];   }
    const ImageDataValue  *LUT                       (Ipp32u index)                         const   { return m_LUT  [index];   }

    // Returns color channel data range
    const ImageDataRange  &DataRange                 (Ipp32u index)                         const   { return m_range[index];   }

    // Returns number of indexes mapped to component
    Ipp32u                 NOfIndexes                ()                                     const   { return m_nOfIndexes;     }


    //////////////////////////////////////////////////////////////////////////
    //
    // Access to read data in "easy", but not the most effective way


    // Number of final color channels
    Ipp32u                NOfChannels               ()                                      const   { return m_nOfChannels;    }

    // Returns data depth for particular color channel
    const ImageDataRange &ChannelRange              (Ipp32u channel)                        const   { return m_range[Index(channel)];}

    // Return true if some component is mapped directly to specific color channel
    bool                  IsChannelMappedDirectly   (Ipp32u channel)                        const   { return m_isChannelMappedDirectly[channel]; }

    // Returns image component number that drive particular color channel mapping
    // (it can be the same for different channels)
    Ipp32u                Component                 (Ipp32u channel)                        const   { return m_component[channel]; }

    Ipp32u                NOfEntries                ()                                      const   { return m_nOfEntries; }

    // Returns value of particular color channel for specified component value,
    // Note it can receive only INTEGER image component values
    const ImageDataValue &ChannelValue              (Ipp32u channel, Ipp32u componentValue) const
    {
        const ImageDataValue *palette = m_LUT[Index(channel)];

        // there is some logic in decision to do something like saturation
        if(componentValue > (Ipp32u)m_nOfEntries) return palette[m_nOfEntries - 1];

        return palette[componentValue];
    }

protected:
    FixedBuffer<Ipp32u>           m_component;
    FixedBuffer<bool>             m_isChannelMappedDirectly;
    FixedBuffer<Ipp32u>           m_index;
    Ipp32u                        m_nOfChannels;

    Ipp32u                        m_nOfEntries;

    FixedBuffer2D<ImageDataValue> m_LUT;
    FixedBuffer<ImageDataRange>   m_range;
    Ipp32u                        m_nOfIndexes;
};


class UICAPI ImageColorSpec
{
public:
    ImageColorSpec()
    : m_componentToColorMap(Direct)
    , m_colorSpecMeth      (Enumerated)
    , m_enumColorSpace     (Unknown)
    {}

    ExcStatus   ReAlloc(Ipp32u nOfComponents) { return m_dataRange.ReAlloc(nOfComponents); }
    void        Free   ()                     { m_dataRange.Free(); }

    ExcStatus CreateCopy(const ImageColorSpec &instance, Ipp32u nOfComponents)
    {
        if(this == &instance) return ExcStatusOk;

        m_componentToColorMap = instance.m_componentToColorMap;
        m_colorSpecMeth       = instance.m_colorSpecMeth;
        m_enumColorSpace      = instance.m_enumColorSpace;

        RET_ON_EXC(m_dataRange.ReAlloc(nOfComponents));

        Copy(instance.m_dataRange, m_dataRange, nOfComponents);

        return ExcStatusOk;
    }

    void SetComponentToColorMap (ImageComponentToColorMap componentToColorMap) { m_componentToColorMap = componentToColorMap; }
    void SetColorSpecMethod     (ImageColorSpecMethod     colorSpecMeth      ) { m_colorSpecMeth       = colorSpecMeth; }
    void SetEnumColorSpace      (ImageEnumColorSpace      enumColorSpace     ) { m_enumColorSpace      = enumColorSpace; }

    // If non-"Palette" value is returned
    // Palette information is not actual
    ImageComponentToColorMap  ComponentToColorMap() const { return m_componentToColorMap; }

    // If ProfileICC is returned, EnumColorspace is not actual and returns "Arbitrary" value
    ImageColorSpecMethod      ColorSpecMeth      () const { return m_colorSpecMeth; }

    // Returns final colorspace, if it can be represented by enumerated values.
    // If image is palettized the colorspace corresponds to
    // palette colors.
    // If colorspace can not be represented by enumerated values
    // this function returns "Arbitrary" value
    ImageEnumColorSpace       EnumColorSpace     () const { return m_enumColorSpace; }

    // Returns component data range
    ImageDataRange           *DataRange          () const { return m_dataRange; }

    // Returns actual palette, if palette is not actual, it's still correct,
    // but palette method IsChannelMappedDirectly returns true value for all color channels
    ImagePalette             &Palette            ()       { return m_palette; }
    const ImagePalette       &Palette            () const { return m_palette; }

protected:
    ImageComponentToColorMap    m_componentToColorMap;
    ImageColorSpecMethod        m_colorSpecMeth;
    ImageEnumColorSpace         m_enumColorSpace;
    FixedBuffer<ImageDataRange> m_dataRange;

    ImagePalette m_palette;
};


class UICAPI ImageBufferFormat
{
public:
    ImageBufferFormat() {}

    ExcStatus CreateCopy(const ImageBufferFormat &instance)
    {
        if(this == &instance) return ExcStatusOk;

        RET_ON_EXC(m_samplingGeometry.CreateCopy(instance.m_samplingGeometry));
        RET_ON_EXC(m_dataOrder.CreateCopy(instance.DataOrder(), m_samplingGeometry.NOfComponents()));
        return ExcStatusOk;
    }

    ImageSamplingGeometry       &SamplingGeometry()       { return m_samplingGeometry; }
    ImageDataOrder              &DataOrder       ()       { return m_dataOrder;        }

    const ImageDataOrder        &DataOrder       () const { return m_dataOrder;        }
    const ImageSamplingGeometry &SamplingGeometry() const { return m_samplingGeometry; }

    Ipp32u                       NOfBytes        (Ipp32u component);

protected:
    ImageDataOrder        m_dataOrder;
    ImageSamplingGeometry m_samplingGeometry;
};


class UICAPI ImageBuffer
{
public:
    ImageBuffer() {}

    // initialize aligned dataOrder
    ExcStatus ReAlloc(
              ImageDataType          dataType           ,
              ImageComponentOrder    componentOrder     ,
        const ImageSamplingGeometry &samplingGeometry   ,
              Ipp32u                 lineAlignOrder  = 5,
              Ipp32u                 pixelAlignOrder = 1);

    ExcStatus ReAlloc(
        const ImageDataOrder        &dataOrder,
        const ImageSamplingGeometry &samplingGeometry);

    ExcStatus Attach(
        const ImageDataPtr          *dataPtr,
        const ImageDataOrder        &dataOrder,
        const ImageSamplingGeometry &samplingGeometry);

    void      FreeOrDetach();

    const ImageDataPtr      *DataPtr     () const { return  m_dataPtr;      }
    const ImageBufferFormat &BufferFormat() const { return  m_bufferFormat; }

protected:
    FixedBuffer<AttachableBuffer<Ipp8u> > m_buffer;
    FixedBuffer<ImageDataPtr>             m_dataPtr;
    ImageBufferFormat                     m_bufferFormat;
};


class UICAPI Image
{
public:
    Image() {}

    const ImageBuffer    &Buffer   () const { return m_buffer; }
    ImageBuffer          &Buffer   ()       { return m_buffer; }

    const ImageColorSpec &ColorSpec() const { return m_colorSpec; }
    ImageColorSpec       &ColorSpec()       { return m_colorSpec; }

protected:
    ImageBuffer    m_buffer;
    ImageColorSpec m_colorSpec;
};

} // namespace UIC

#endif // __UIC_IMAGE_H__

