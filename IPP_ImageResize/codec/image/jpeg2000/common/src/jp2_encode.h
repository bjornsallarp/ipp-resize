/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2007-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifdef _OPENMP
#include <omp.h>
#endif
#include "ejp2box.h"
#include "ejp2marker.h"
#include "wtfwd.h"
#include "etile.h"
#include "equant.h"
#include "epreprocess.h"

#include "genalg.h"
#include "byteinput.h"
#include "memoryoutput.h"

#include "jp2const.h"

static const int MAX_N_OF_RES_LEVELES = 33;

class DIBRGBValue
{
public:
    DIBRGBValue() : m_r(0), m_g(0), m_b(0) {}

    DIBRGBValue(unsigned char value) : m_r(value), m_g(value), m_b(value) {}
    DIBRGBValue(unsigned char r, unsigned char g, unsigned char b)
    : m_r(r), m_g(g), m_b(b) {}

    DIBRGBValue(const DIBRGBValue&  value) { operator=(value); }

    void SetR(unsigned char value) {  m_r = value; }
    void SetG(unsigned char value) {  m_g = value; }
    void SetB(unsigned char value) {  m_b = value; }

    DIBRGBValue& operator=(const DIBRGBValue& value)
    {
         if ( this != &value )
         {
            m_r = value.R();
            m_g = value.G();
            m_b = value.B();
         }
         return *this;
    }


    unsigned char R() const { return m_r; }
    unsigned char G() const { return m_g; }
    unsigned char B() const { return m_b; }


    bool operator ==(const DIBRGBValue& value) const
    {
        return (m_r == value.m_r) && (m_g == value.m_g) && (m_b == value.m_b);
    }

    bool operator !=(const DIBRGBValue& value) const
    {
        return !operator==(value);
    }

protected:
    unsigned char m_r;
    unsigned char m_g;
    unsigned char m_b;
};

inline void PreProcess(
    const ImageCoreC<Ipp32s, 1> *imgComponents,
    unsigned int                 nOfComponents,
    const RectSize              &size,
    unsigned int                 bitDepth,
    bool                         isUseMCT,
    bool                         isWT53Used)
{
    FixedBuffer<JP2BitsPerComp> bitsPerComp(nOfComponents);

    for(unsigned int i = 0; i < nOfComponents; i++)
        bitsPerComp[i] = JP2BitsPerComp((Ipp8u)bitDepth);

    PreProcess32s32f(imgComponents, size, bitsPerComp, nOfComponents, isUseMCT, isWT53Used);
}

inline void Convert32f32s(const SBTree<Ipp32f> &sbTree)
{
    Convert32f32s(sbTree.LxLy(), sbTree.LxLy().Size());
    for(unsigned int i = 0; i < sbTree.NOfWTLevels(); i++)
    {
        Convert32f32s(sbTree.LxHy(i), sbTree.LxHy(i).Size());
        Convert32f32s(sbTree.HxLy(i), sbTree.HxLy(i).Size());
        Convert32f32s(sbTree.HxHy(i), sbTree.HxHy(i).Size());
    }
}

// ========================== CodeStream ============================
template<class ByteOutput>
void writeCodeStream(
    ByteOutput                  &stream,
    Ipp32u                       nOfWTLevels,
    const ImageCoreC<Ipp32s, 1> *imgComponents,
    Ipp32u                       nOfComponents,
    const RectSize              &size,
    const Ipp32u                 depth,
    Ipp32u                       dstSize,
    bool                         isUseMCT,
    bool                         isWT53Used,
    bool                         isDerivedQuant,
    QuantValue                   derivedQuantBaseValue)
{

#ifdef IPP_JPEG2000_TIMING
    Timer timer;
    timer.PriorityBoost();
    timer.Start();
#endif

    PreProcess(imgComponents, nOfComponents, size, depth, isUseMCT, isWT53Used);

    SBTree<Ipp32s>::Array       sbTrees(nOfComponents);
    FixedBuffer<QuantComponent> qComponents(nOfComponents);

    if(isWT53Used)
    {
        for(unsigned int i = 0; i < nOfComponents; i++)
        {
            sbTrees[i].ReAlloc(Rect(Point(0, 0), size), nOfWTLevels);
            WT53Fwd(imgComponents[i], Rect(Point(0, 0), size), sbTrees[i]);
            qComponents[i].InitUnitStep(depth + 1, nOfWTLevels);
        }

    }
    else
    {
        const SBTree<Ipp32f> *sbTrees32f = (const SBTree<Ipp32f> *)((SBTree<Ipp32s> *)sbTrees);

        for(unsigned int i = 0; i < nOfComponents; i++)
        {
            sbTrees[i].ReAlloc(Rect(Point(0, 0), size), nOfWTLevels);
            WT97Fwd((const ImageCoreC<Ipp32f, 1> &)imgComponents[i], Rect(Point(0, 0), size), sbTrees32f[i]);

            if(isDerivedQuant)
            {
                qComponents[i].InitDerived(derivedQuantBaseValue, nOfWTLevels);
                QuantFwd(sbTrees32f[i], qComponents[i], depth + 1);
            }
            else qComponents[i].InitUnitStep(depth + 1, nOfWTLevels);

            Convert32f32s(sbTrees32f[i]);
        }
    }

    //
    // Fill precinct and codeblock parameters to initialize
    //   tile encoding.
    //
    // Very simple case implemented that is single precinct (size 2 ^ 15),
    // codeblock size 64x64 and all sizes are the same for every resolution
    // level and every component.
    //
    //

    Int2D prXStepOrder(nOfComponents, MAX_N_OF_RES_LEVELES);
    Int2D prYStepOrder(nOfComponents, MAX_N_OF_RES_LEVELES);

    Int2D cbXStepOrder(nOfComponents, MAX_N_OF_RES_LEVELES);
    Int2D cbYStepOrder(nOfComponents, MAX_N_OF_RES_LEVELES);

    FixedBuffer<int> nOfWTLevelsC(nOfComponents);

    for(unsigned int j = 0; j < nOfComponents; j++)
    {
        nOfWTLevelsC[j] = nOfWTLevels;
        for(unsigned int i = 0; i < (unsigned int)MAX_N_OF_RES_LEVELES; i++)
        {
            prXStepOrder[j][i] = 15;
            prYStepOrder[j][i] = 15;
            cbXStepOrder[j][i] =  6;
            cbYStepOrder[j][i] =  6;
        }
    }

    Rect imgRect(Point(0, 0), size);

    FixedBuffer<Rect> rects(nOfComponents);

    {
        for(unsigned int i = 0; i < nOfComponents; i++)
            rects[i] = imgRect;
    }

    unsigned int numOfThreads = 1;


#ifdef _OPENMP
#pragma omp parallel shared(numOfThreads)
    {
#pragma omp master
        {
            numOfThreads = omp_get_num_threads();
        }
    }

#endif


    FixedBuffer<CBEncoder> cbEncoder(numOfThreads);
    ETile     tile;

    FixedBuffer<int> cmpDynRange(nOfComponents);

    for(unsigned int component = 0; component < nOfComponents; component++)
        cmpDynRange[component] = depth + 1;

    tile.Init(rects, prXStepOrder, prYStepOrder, cbXStepOrder, cbYStepOrder,
        cmpDynRange, qComponents,
        nOfWTLevelsC, nOfComponents);

    tile.Encode(cbEncoder, sbTrees);

    if(dstSize != 0xFFFFFFFF)
    {
        if(isWT53Used)
            tile.WeightDistAsWT53();
        else
            tile.WeightDistAsWT97();

        if(isUseMCT && nOfComponents==3)
        {
            if(isWT53Used)
                tile.WeightDistAsRCT();
            else
                tile.WeightDistAsICT();
        }

        // only single layer implemented
        tile.SetCurPassToFirst();
        tile.AssignLayers((int*)&dstSize, 1);
    }

    tile.SetCurPassToFirst();

    // Layer-resolution level-component-position progression
    int nOfResLevels = NOfResLevels(nOfWTLevels);
    for(int resLevel = 0; resLevel < nOfResLevels; resLevel++)
        for(unsigned int component = 0; component < nOfComponents; component++)
            for(int precinct = 0; precinct < tile.NOfPrecincts(0, resLevel); precinct++)
                tile.WriteToPacket(component, resLevel, precinct, 0,stream);
#ifdef IPP_JPEG2000_TIMING
    timer.Stop();
    timer.PriorityRelease();
    printf("encoding time = %f msec\n", timer.GetTime(Timer::msec));
#endif

}

// ============================ Markers ============================
template<class ByteOutput>
void writeSOCMarker(ByteOutput& stream)
{
    WriteMarker(SOC, stream);
}

template<class ByteOutput>
void writeSIZMarker(ByteOutput& stream, Ipp32u height, Ipp32u width, Ipp32u nc, const Ipp8u* bpc)
{
    ByteBuffer sizStream;

    sizStream.Write16u(0);
    sizStream.Write32u(width);
    sizStream.Write32u(height);
    sizStream.Write32u(0);
    sizStream.Write32u(0);
    sizStream.Write32u(width);
    sizStream.Write32u(height);
    sizStream.Write32u(0);
    sizStream.Write32u(0);
    sizStream.Write16u((Ipp16u)nc);

    for (Ipp16u i = 0; i < nc; i++)
    {
        sizStream.Write8u(bpc[i]);
        sizStream.Write8u(1);
        sizStream.Write8u(1);
    }

    WriteMarker(SIZ, (Ipp16u)sizStream.Size(), stream);
    TransferAllSrc(sizStream, stream);
}

template<class ByteOutput>
void writeCODMarker(ByteOutput& stream, Ipp32u pOrder, Ipp32u nOfLayers, bool isMCTused,
                    bool isWT53Used, Ipp32u nOfWTLevels, Ipp32u codingStyle, Ipp8u* pPrecintSize)
{
    ByteBuffer codStream;

    codStream.Write8u ((Ipp8u)codingStyle);
    codStream.Write8u ((Ipp8u)pOrder);
    codStream.Write16u((Ipp16u)nOfLayers);

    if (isMCTused)
        codStream.Write8u(1);
    else
        codStream.Write8u(0);

    codStream.Write8u((Ipp8u)nOfWTLevels);

    codStream.Write8u(4);
    codStream.Write8u(4);
    codStream.Write8u(0);

    if (isWT53Used)
        codStream.Write8u(1);
    else
        codStream.Write8u(0);

    if (codingStyle & 0x01)
        for (unsigned int i = 0; i < nOfWTLevels; i++)
            codStream.Write8u(pPrecintSize[i]);

    WriteMarker(COD, (Ipp16u)codStream.Size(), stream);
    TransferAllSrc(codStream, stream);
}

template<class ByteOutput>
void writeQCDMarker(
    ByteOutput       &stream,
    Ipp32u            nOfWTLevels,
    int               dynRange,
    bool              isWT53Used,
    bool              isDerivedQuant,
    QuantValue        derivedQuantBaseValue)
{
    ByteBuffer qcdStream;

    if(isWT53Used)
        WriteQuantWT53(dynRange, GUARD_BITS, nOfWTLevels, qcdStream);
    else
    {
        if(isDerivedQuant)
            WriteQuantDerived(derivedQuantBaseValue, GUARD_BITS, qcdStream);
        else
            WriteQuantUnitStep(dynRange, GUARD_BITS, nOfWTLevels, qcdStream);
    }

    WriteMarker(QCD, (Ipp16u)qcdStream.Size(), stream);
    TransferAllSrc(qcdStream, stream);
}

template<class ByteOutput>
void writeSOTMarker(ByteOutput& stream, Ipp32u index, Ipp32u length, Ipp32u tpIndex, Ipp32u nOfTileParts)
{
    ByteBuffer sotStream;

    sotStream.Write16u((Ipp16u)index);
    sotStream.Write32u(length);
    sotStream.Write8u ((Ipp8u)tpIndex);
    sotStream.Write8u ((Ipp8u)nOfTileParts);

    WriteMarker(SOT, (Ipp16u)sotStream.Size(), stream);
    TransferAllSrc(sotStream, stream);
}

template<class ByteOutput>
void writeSODMarker(ByteOutput& stream)
{
    WriteMarker(SOD, stream);
}

template<class ByteOutput>
void writeEOCMarker(ByteOutput& stream)
{
    WriteMarker(EOC, stream);
}

template<class ByteOutput>
void writeCodeStreamMainHeader(
    ByteOutput     &stream,
    Ipp32u          nc,
    const RectSize &size,
    const Ipp8u    *bpc,
    Ipp32u          nOfWTLevels,
    Ipp32s          dynRange,
    bool            isUseMCT,
    bool            isWT53Used,
    bool            isDerivedQuant,
    QuantValue      derivedQuantBaseValue)
{
    writeSOCMarker(stream);
    writeSIZMarker(stream, size.Height(), size.Width(), nc, bpc);

    if(nc==3)
        writeCODMarker(stream, 0, 1, isUseMCT, isWT53Used, nOfWTLevels, 0, 0);
    else
        writeCODMarker(stream, 0, 1, false, isWT53Used, nOfWTLevels, 0, 0);

    writeQCDMarker(
        stream,
        nOfWTLevels,
        dynRange,
        isWT53Used,
        isDerivedQuant,
        derivedQuantBaseValue);
}

template<class ByteOutput>
void writeCodeStreamBody(
    ByteOutput                  &stream,
//    Ipp32u                       nc,
    const Ipp8u                 *bpc,
    Ipp32u                       nOfWTLevels,
    const ImageCoreC<Ipp32s, 1> *imgComponents,
    Ipp32u                       nOfComponents,
    const RectSize              &size,
    Ipp32u                       dstSize,
    bool                         isUseMCT,
    bool                         isWT53Used,
    bool                         isDerivedQuant,
    QuantValue                   derivedQuantBaseValue)
{
    writeSOTMarker(stream, 0, 0, 0, 1);
    writeSODMarker(stream);

    writeCodeStream(stream, nOfWTLevels, imgComponents, nOfComponents, size, bpc[0], dstSize, isUseMCT, isWT53Used, isDerivedQuant, derivedQuantBaseValue);

    writeEOCMarker(stream);
}

// ============================= Boxes ============================
template<class ByteOutput>
void writeSignatureBox(ByteOutput& stream)
{
    ByteBuffer body;

    body.Write32u(jp2id);

    WriteBoxHeader(jp__, body.Size(), stream);
    TransferAllSrc(body, stream);
}

template<class ByteOutput>
void writeFileTypeBox(ByteOutput& stream)
{
    ByteBuffer body;

    body.Write32u(jp2_);
    body.Write32u(0);
    body.Write32u(jp2_);

    WriteBoxHeader(ftyp, body.Size(), stream);
    TransferAllSrc(body, stream);
}

template<class ByteOutput>
void writeImageHeaderBox(ByteOutput& stream, Ipp32u height, Ipp32u width, Ipp16u nc, const Ipp8u bpc, Ipp8u ipr)
{
    ByteBuffer body;

    body.Write32u(height);
    body.Write32u(width);
    body.Write16u(nc);
    body.Write8u(bpc); // Bits per component. // HARD CODE //7
    body.Write8u(7); // Compression type.
    body.Write8u(0);
    body.Write8u(ipr);

    WriteBoxHeader(ihdr, body.Size(), stream);
    TransferAllSrc(body, stream);
}

template<class ByteOutput>
void writeBitPerCompBox(ByteOutput& stream, Ipp16u nc, const Ipp8u* bpc)
{
    ByteBuffer body;

    for (Ipp16u i = 0; i < nc; i++)
        body.Write8u(bpc[i]);

    WriteBoxHeader(bpcc, body.Size(), stream);
    TransferAllSrc(body, stream);
}

template<class ByteOutput>
void writeColorSpecBox(ByteOutput& stream, JP2EnumColourspace enumCS)
{
    ByteBuffer body;

    body.Write8u (1);
    body.Write8u (0);
    body.Write8u (0);
    body.Write32u(enumCS);

    WriteBoxHeader(colr, body.Size(), stream);
    TransferAllSrc(body, stream);
}

template<class ByteOutput>
void writePaletteBox(ByteOutput& stream, const DIBRGBValue *entry, unsigned int nOfEntries)
{
    static const unsigned int nOfChannels = 3;
    ByteBuffer body;

    body.Write16u((Ipp16u)nOfEntries);
    body.Write8u ((Ipp8u)nOfChannels);

    for(unsigned int i = 0; i < nOfChannels; i++)
        body.Write8u(7);

    for(unsigned int  j = 0; j < nOfEntries; j++)
    {
        DIBRGBValue element = entry[j];

        body.Write8u(element.R());
        body.Write8u(element.G());
        body.Write8u(element.B());
    }

    WriteBoxHeader(pclr, body.Size(), stream);
    TransferAllSrc(body, stream);
}

template<class ByteOutput>
void writeCompMappingBox(ByteOutput& stream)
{
    Ipp8u nOfChannels = 3;
    ByteBuffer body;

    for(Ipp8u i = 0; i < nOfChannels; i++)
    {
        body.Write16u(0);
        body.Write8u (0x01);
        body.Write8u (i);
    }

    WriteBoxHeader(cmap, body.Size(), stream);
    TransferAllSrc(body, stream);
}

template<class ByteOutput>
void writeHeaderSuperBox(ByteOutput& stream,
    Ipp32u height, Ipp32u width,
    Ipp16u nc, const Ipp8u bpc, Ipp8u ipr,
    JP2EnumColourspace enumCS,
    const DIBRGBValue *paletteEntry,
    unsigned int nOfPaletteEntries,
    bool isWritePalette)
{
    ByteBuffer body;

    writeImageHeaderBox(body, height, width, nc, bpc, ipr);
    //writeBitPerCompBox(body, nc, bpc);
    writeColorSpecBox(body, enumCS);

    if(isWritePalette)
    {
        writePaletteBox(body, paletteEntry, nOfPaletteEntries);
        writeCompMappingBox(body);
    }

    WriteBoxHeader(jp2h, body.Size(), stream);
    TransferAllSrc(body, stream);
}

// ============================== =================================


template<class ByteOutput>
void JP2WriteHeader(
    ByteOutput                  &stream,
    unsigned int                 nOfComponents,
    const RectSize              &size,
    JP2EnumColourspace           enumCS,
    unsigned int                 depth,

    const DIBRGBValue           *paletteEntry,
    unsigned int                 nOfPaletteEntries,
    bool                         isWritePalette,

    unsigned int                 maxNOfWTLevels,
    bool                         isUseMCT,
    bool                         isWT53Used,
    bool                         isDerivedQuant,
    QuantValue                   derivedQuantBaseValue)
{
    writeSignatureBox(stream);
    writeFileTypeBox (stream);

    FixedBuffer<Ipp8u> bpc(nOfComponents);

    for(unsigned int i = 0; i < nOfComponents; i++)
        bpc[i] = (Ipp8u)depth;

    writeHeaderSuperBox(stream, size.Height(), size.Width(), (Ipp16u)nOfComponents, bpc[0], 0, enumCS, paletteEntry, nOfPaletteEntries, isWritePalette);

    unsigned int nOfWTLevels = MaxNOfWTLevels(Min(size.Width(), size.Height()));
    if(nOfWTLevels > maxNOfWTLevels) nOfWTLevels = maxNOfWTLevels;

    WriteUknLenBoxHeader(jp2c, stream);

    int dynRange = depth + 1;

    writeCodeStreamMainHeader(
        stream,
        nOfComponents,
        size,
        bpc,
        nOfWTLevels,
        dynRange,
        isUseMCT,
        isWT53Used,
        isDerivedQuant,
        derivedQuantBaseValue);
}

template<class ByteOutput>
void JP2WriteBody(
    ByteOutput                  &stream,
    const ImageCoreC<Ipp32s, 1> *imgComponents,
    unsigned int                 nOfComponents,
    const RectSize              &size,
//    JP2EnumColourspace           enumCS,
    unsigned int                 depth,
    unsigned int                 maxNOfWTLevels,
    bool                         isUseMCT,
    bool                         isWT53Used,
    bool                         isDerivedQuant,
    QuantValue                   derivedQuantBaseValue,
    unsigned int                 dstSize)
{
    unsigned int nOfWTLevels = MaxNOfWTLevels(Min(size.Width(), size.Height()));
    if(nOfWTLevels > maxNOfWTLevels)
        nOfWTLevels = maxNOfWTLevels;

    FixedBuffer<Ipp8u> bpc(nOfComponents);

    for(unsigned int i = 0; i < nOfComponents; i++)
        bpc[i] = (Ipp8u)depth;

//    writeCodeStreamBody(stream, nOfComponents, bpc, nOfWTLevels, imgComponents, nOfComponents, size,
    writeCodeStreamBody(stream, bpc, nOfWTLevels, imgComponents, nOfComponents, size,
            dstSize, isUseMCT, isWT53Used, isDerivedQuant, derivedQuantBaseValue);
}
