
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2003-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __DJP2CODESTREAM_H__
#define __DJP2CODESTREAM_H__

#include "image.h"
#include "djp2cstile.h"
#include "djp2marker.h"
#include "djp2cswarning.h"
#include "djp2csexception.h"
#include "bdiagnoutput.h"

template<class ByteInput>
class DJP2Codestream
{
public:

    DJP2Codestream()
    : m_stream                     (0)
    , m_isNextTilePartExist        (false)
    , m_isFirstTilePartAfterMainHdr(true)
    , m_tileIndex                  (0xFFFFFFFF)
    , m_curTilePartTail            (0)
    , m_isCurTilePartLengthSpec    (false)
    {}

    const DJP2CSMainHeader &MainHeader() const { return m_mainHeader; }
    unsigned int            NOfTiles()   const { return m_tile.Size(); }

    void Attach(ByteInput &stream)
    {
        m_stream = &stream;

        m_mainHeader.Reset();
        m_isNextTilePartExist         = false;
        m_isFirstTilePartAfterMainHdr = true;
        m_tile.Free();

        m_tileIndex                   = 0xFFFFFFFF;

        m_curTilePartTail             = 0;
        m_isCurTilePartLengthSpec     = false;
    }

    void AttachDiagnOutput(BDiagnOutput &diagnOutput)
    {
        m_diagnOutputPtr = diagnOutput;
        m_mainHeader.AttachDiagnOutput(diagnOutput);
    }

    void ReadMainHeader()
    {
        m_isNextTilePartExist = m_mainHeader.Read(*m_stream);
        m_isFirstTilePartAfterMainHdr = true;

        unsigned int nOfCols  = DivCeil(m_mainHeader.ImageRectRG().Origin().X() + m_mainHeader.ImageRectRG().Width()  - m_mainHeader.TileGridOriginRG().X(), m_mainHeader.TileSizeRG().Width());
        unsigned int nOfRows  = DivCeil(m_mainHeader.ImageRectRG().Origin().Y() + m_mainHeader.ImageRectRG().Height() - m_mainHeader.TileGridOriginRG().Y(), m_mainHeader.TileSizeRG().Height());

        unsigned int nOfTiles = nOfCols * nOfRows;

        m_tile.ReAlloc(nOfTiles);

        for(unsigned int tile = 0; tile < nOfTiles; tile++)
        {
            m_tile[tile].AttachDiagnOutput(m_diagnOutputPtr);
            m_tile[tile].InitAttach(m_mainHeader, tile);
        }
    }

    // returns false if there are no tile-parts
    bool ReadNextTilePartHeader()
    {
        if(!m_isNextTilePartExist) return false;

        ByteInput &stream = *m_stream;

        if(!m_isFirstTilePartAfterMainHdr)
        {
            // search next tile-part header (mostly theoretical protection
            // from damage because tile-parts should follow inseparable
            // in the current version of JPEG 2000 standard)
            JP2Marker marker = ReadMarker(stream);

            while (marker != SOT)
            {
                if(marker == EOC)
                {
                    m_isNextTilePartExist = false;
                    return false;
                }

                unsigned int markerSize = ReadMarkerBodySize(stream);

                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, CodeStreamBodyUnknownMarker>());
                stream.Seek(markerSize);

                marker = ReadMarker(stream);
            }
        }
        else
            m_isFirstTilePartAfterMainHdr = false;


        // process SOT _BODY_ and tile-part header
        unsigned int SOTSize = ReadMarkerBodySize(stream);

        unsigned int tilePartLength;
        unsigned int tilePartIndex;
        unsigned int nOfTileParts;

        if(stream.PushSize(SOTSize) != SOTSize)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, SpecifiedMarkerLengthExceedContainerTail>());

        ReadSOTBody(stream, m_tileIndex, tilePartLength, tilePartIndex, nOfTileParts);

        if(stream.PopSize())
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, ActualMarkerLengthBelowSpecified>());

        if(m_tileIndex >= m_tile.Size())
            throw DiagnDescrCT<DJP2CSException,SOTTileIndexExceedNOfTiles>();

        unsigned int headerStart = stream.Position();

        CurrTile().ReadNextPartHeader(stream, tilePartIndex, nOfTileParts);

        unsigned int headerSize = SOTSize
            + 2 // SOT marker itself
            + 2 // SOT marker length field (Lsot)
            + stream.Position() - headerStart; // plus header body (SOD and so on)

        if(tilePartLength)
        {
            if(tilePartLength < headerSize)
            {
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, SOTTilePartLengthLessHeaderSize>());
                m_curTilePartTail         = 0;
                m_isCurTilePartLengthSpec = false;
            }
            else
            {
                m_curTilePartTail = tilePartLength - headerSize;
                m_isCurTilePartLengthSpec = true;
            }
        }


        return true;
    }

    bool ReadPacket()
    {
        ByteInput &stream = *m_stream;

        // it could be easier without checking of conformity
        // of actual tile-part length
        // (defined in some cases by progression range)
        //  and
        // tile-part length value specified in SOT
        // to detect possible damages and its reasons

        if(!m_isCurTilePartLengthSpec)
            return CurrTile().ReadPacket(stream);

        if(!m_curTilePartTail)
            return false;

        unsigned int packetStart = stream.Position();

        bool isProgrContinue = CurrTile().ReadPacket(stream);

        unsigned int packetSize = stream.Position() - packetStart;

        if(m_curTilePartTail < packetSize)
        {
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, SOTTilePartLengthLessActualLength>());
            m_curTilePartTail         = 0;
            m_isCurTilePartLengthSpec = false;
        }
        else
        {
            if(isProgrContinue)
            {
                m_curTilePartTail  -= packetSize;
            }
            else
            {
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, SOTTilePartLengthExceedActualLength>());
                stream.Seek(m_curTilePartTail);
                m_curTilePartTail = 0;
            }
        }

        return true;
    }

    Rect ComponentRect(unsigned int component) const { return DecimateRect(m_mainHeader.ImageRectRG(), m_mainHeader.SampleSizeRG()[component]); }

    // components may be resampled
    template<class T>
        void UpdateImageComponents(const ImageCoreC<T, 1> *dst)
    {
        Ipp16u nOfComponents = m_mainHeader.NOfComponents();

        FixedBuffer<ImageCoreC<T, 1> > dstTileComponents(nOfComponents);

        for(unsigned int tile = 0; tile < NOfTiles(); tile++)
        {
            for(unsigned int component = 0; component < nOfComponents; component++)
                dstTileComponents[component] = dst[component].SubImage(m_tile[tile].ComponentRect(component).Origin() - m_mainHeader.ImageRectRG().Origin());

            m_tile[tile].UpdateComponents(dstTileComponents);
        }
    }

protected:
    DJP2CSTile &CurrTile() { return m_tile[m_tileIndex]; }

    ByteInput             *m_stream;
    BDiagnOutputPtr        m_diagnOutputPtr;

    bool                   m_isNextTilePartExist;
    bool                   m_isFirstTilePartAfterMainHdr;

    DJP2CSMainHeader       m_mainHeader;
    FixedArray<DJP2CSTile> m_tile;

    unsigned int           m_tileIndex;

    unsigned int           m_curTilePartTail;
    bool                   m_isCurTilePartLengthSpec;
};

#endif // __DJP2CODESTREAM_H__
