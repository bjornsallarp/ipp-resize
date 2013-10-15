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

#ifndef __DJP2MARKER_H__
#define __DJP2MARKER_H__

#include "jp2marker.h"
#include "djp2quantcomp.h"
#include "jp2codingstylejoint.h"
#include "jp2codingstylecomp.h"
#include "jp2progrrange.h"
#include "jp2bitspercomp.h"

#include "djp2markerwarning.h"
#include "djp2markerexception.h"

#include "bdiagnoutput.h"


inline JP2Marker MarkerType(Ipp16u value)
{
    switch(value)
    {
    case SOC: return SOC;
    case SOT: return SOT;
    case SOD: return SOD;
    case EOC: return EOC;
    case SIZ: return SIZ;
    case COD: return COD;
    case COC: return COC;
    case RGN: return RGN;
    case QCD: return QCD;
    case QCC: return QCC;
    case POC: return POC;
    case TLM: return TLM;
    case PLM: return PLM;
    case PLT: return PLT;
    case PPM: return PPM;
    case PPT: return PPT;
    case SOP: return SOP;
    case EPH: return EPH;
    case CRG: return CRG;
    case COM: return COM;
    default:  return UKN;
    }

}

template<class ByteInput>
void ReadCodingStyle(
    ByteInput           &body,
    bool                isPrecinctSizeSpec,
    JP2CodingStyleComp &codingStyle
//    ,BDiagnOutputPtr      &diagnOutputPtr
    )
{
    {
    unsigned int nOfWTLevels = body.Read8u();
    if (nOfWTLevels > 32)
        throw DiagnDescrCT<DJP2MarkerException,BadCODOrCOCNumberOfDecompositionLevels>();

    codingStyle.ReAlloc(nOfWTLevels);
    }

    {
    unsigned int cbXStepOrder = body.Read8u() & 0xF;
    if (cbXStepOrder > 8)
        throw DiagnDescrCT<DJP2MarkerException,BadCODOrCOCCodeBlockWidth>();
    cbXStepOrder+=2;

    unsigned int cbYStepOrder = body.Read8u() & 0xF;
    if (cbYStepOrder > 8)
        throw DiagnDescrCT<DJP2MarkerException,BadCODOrCOCCodeBlockHeight>();
    cbYStepOrder+=2;

    codingStyle.SetCBStepOrder(RectSize(cbXStepOrder, cbYStepOrder));
    }

    {
    CBCodingStyle cbCodingStyle;
    cbCodingStyle.SetFromCODOrCOCField(body.Read8u());

    codingStyle.SetCBStyle(cbCodingStyle);
    }

    {
    Ipp8u wtTypeField = body.Read8u();
    if (wtTypeField > 1)
        throw DiagnDescrCT<DJP2MarkerException,BadCODOrCOCTransformationType>();
    codingStyle.SetWTType((JP2WTSpecType)wtTypeField);
    }

    if(isPrecinctSizeSpec)
    {
        for(unsigned int resLevel = 0; resLevel < codingStyle.NOfResLevels(); resLevel++)
        {
            Ipp8u precStepOrderField = body.Read8u();

            unsigned int stepOrderWidth  =  precStepOrderField & JP2M_PRECINCT_STEP_ORDER_WIDTH;
            unsigned int stepOrderHeight = (precStepOrderField & JP2M_PRECINCT_STEP_ORDER_HEIGHT) >> JP2S_PRECINCT_STEP_ORDER_HEIGHT;

            codingStyle.SetPrecStepOrder(
                RectSize(stepOrderWidth, stepOrderHeight),
                resLevel );
        }
    }
    else
    {
        for(unsigned int resLevel = 0; resLevel < codingStyle.NOfResLevels(); resLevel++)
            codingStyle.SetPrecStepOrder(
                RectSize(DEFAULT_PREC_STEP_ORDER, DEFAULT_PREC_STEP_ORDER),
                resLevel);
    }
}

template<class ByteInput>
void ReadProgrRange(
    ByteInput       &pocBody,
    unsigned int     nOfComponents,
    JP2ProgrRange   &progrRange,
    BDiagnOutputPtr &diagnOutputPtr)
{
    unsigned int resLevelFirst;
    unsigned int resLevelBound;
    unsigned int compFirst;
    unsigned int compBound;
    unsigned int nOfLayers;

    resLevelFirst = pocBody.Read8u();
    if(resLevelFirst > 33)
        throw DiagnDescrCT<DJP2MarkerException,POCResLevelIndexExceedBound>();

    compFirst = ReadMarkerCompIndex(pocBody, nOfComponents);
    if (compFirst > nOfComponents)
        throw DiagnDescrCT<DJP2MarkerException,POCCompIndexExceedNOfComponents>();

    nOfLayers = pocBody.Read16u();

    if(!nOfLayers)
        diagnOutputPtr->Warning(DiagnDescrCT<DJP2MarkerWarning, CODOrCOCOrPOCZeroNOfLayers>());

    resLevelBound = pocBody.Read8u();
    if(resLevelBound > 33)
        throw DiagnDescrCT<DJP2MarkerException,POCResLevelIndexExceedBound>();

    if(resLevelBound < resLevelFirst)
        throw DiagnDescrCT<DJP2MarkerException,POCInverseResLevelProgr>();

    compBound = ReadMarkerCompIndex(pocBody, nOfComponents);
    if (compBound > nOfComponents)
        throw DiagnDescrCT<DJP2MarkerException,POCCompIndexExceedNOfComponents>();
    if(!compBound) compBound = 256;

    if(compBound < compFirst)
        throw DiagnDescrCT<DJP2MarkerException,POCInverseCompProgr>();

    progrRange.SetResLevelRange(UIntRange(resLevelFirst, resLevelBound - resLevelFirst));
    progrRange.SetCompRange    (UIntRange(compFirst,     compBound     - compFirst));
    progrRange.SetNOfLayers    (nOfLayers);
}


template<class ByteInput>
JP2Marker ReadMarker(ByteInput& stream)
{
    return MarkerType(stream.Read16u());
}

template<class ByteInput>
Ipp16u ReadMarkerBodySize(ByteInput& stream)
{
    int size = stream.Read16u() - 2;

    if(size < 0)
        throw DiagnDescrCT<DJP2MarkerException,NegativeBodyLen>();
    return (Ipp16u)size;
}


template<class ByteInput>
void ReadSIZBody(
    ByteInput                   &body,
    Rect                        &imageRectRG,
    Point                       &tileGridOriginRG,
    RectSize                    &tileSizeRG,
    Ipp16u                      &nOfComponents,
    FixedBuffer<JP2BitsPerComp> &bitsPerComp,
    FixedBuffer<RectSize>       &sampleSizeRG,
    BDiagnOutputPtr              &diagnOutputPtr)
{

    Ipp16u Rsiz = body.Read16u();

    if(Rsiz)
        diagnOutputPtr->Warning(DiagnDescrCT<DJP2MarkerWarning, UnknonwRsiz>());

    Ipp32u Xsiz   = body.Read32u();
    Ipp32u Ysiz   = body.Read32u();
    Ipp32u XOsiz  = body.Read32u();
    Ipp32u YOsiz  = body.Read32u();
    Ipp32u XTsiz  = body.Read32u();
    Ipp32u YTsiz  = body.Read32u();
    Ipp32u XTOsiz = body.Read32u();
    Ipp32u YTOsiz = body.Read32u();

    if(Xsiz   > IPP_MAX_32S) throw DiagnDescrCT<DJP2MarkerException,SIZGeometryTooLage>();
    if(Ysiz   > IPP_MAX_32S) throw DiagnDescrCT<DJP2MarkerException,SIZGeometryTooLage>();
    if(XOsiz  > IPP_MAX_32S) throw DiagnDescrCT<DJP2MarkerException,SIZGeometryTooLage>();
    if(YOsiz  > IPP_MAX_32S) throw DiagnDescrCT<DJP2MarkerException,SIZGeometryTooLage>();
    if(XTsiz  > IPP_MAX_32S) throw DiagnDescrCT<DJP2MarkerException,SIZGeometryTooLage>();
    if(YTsiz  > IPP_MAX_32S) throw DiagnDescrCT<DJP2MarkerException,SIZGeometryTooLage>();
    if(XTOsiz > IPP_MAX_32S) throw DiagnDescrCT<DJP2MarkerException,SIZGeometryTooLage>();
    if(YTOsiz > IPP_MAX_32S) throw DiagnDescrCT<DJP2MarkerException,SIZGeometryTooLage>();


    if(XOsiz > Xsiz) throw DiagnDescrCT<DJP2MarkerException,SIZImageNeagativeSize>();
    if(YOsiz > Ysiz) throw DiagnDescrCT<DJP2MarkerException,SIZImageNeagativeSize>();

    imageRectRG      = Rect(Point(XOsiz, YOsiz), RectSize(Xsiz - XOsiz, Ysiz - YOsiz));
    tileGridOriginRG = Point(XTOsiz, YTOsiz);
    tileSizeRG       = RectSize(XTsiz, YTsiz);


    if(
        tileGridOriginRG.X() > imageRectRG.Origin().X()
     || tileGridOriginRG.Y() > imageRectRG.Origin().Y() )
    {
        diagnOutputPtr->Warning(DiagnDescrCT<DJP2MarkerWarning, SIZTileGridOriginGreaterImageOrigin>());

        if(tileGridOriginRG.X() > imageRectRG.Origin().X())
        {
            tileGridOriginRG.SetX(
                tileGridOriginRG.X() -
                 tileSizeRG.Width() *
                  DivCeil(tileGridOriginRG.X() - imageRectRG.Origin().X(), tileSizeRG.Width())
            );
        }
        if(tileGridOriginRG.Y() > imageRectRG.Origin().Y())
        {
            tileGridOriginRG.SetY(
                tileGridOriginRG.Y() -
                 tileSizeRG.Height() *
                  DivCeil(tileGridOriginRG.Y() - imageRectRG.Origin().Y(), tileSizeRG.Height())
            );
        }
    }

    nOfComponents = body.Read16u();

    bitsPerComp .ReAlloc(nOfComponents);
    sampleSizeRG.ReAlloc(nOfComponents);

    for(unsigned int component = 0; component < nOfComponents; component++)
    {
        bitsPerComp[component] = body.Read8u();

        if(!bitsPerComp[component].IsConformant())
            diagnOutputPtr->Warning(DiagnDescrCT<DJP2MarkerWarning, SIZBitDepthExceedBounds>());

        Ipp8u XRsiz = body.Read8u();
        Ipp8u YRsiz = body.Read8u();

        sampleSizeRG[component] = RectSize(XRsiz, YRsiz);
    }
}

template<class ByteInput>
void ReadCODBody(
    ByteInput                  &body,
    JP2CodingStyleJoint        &codingStyleJoint,
    JP2CodingStyleComp         &codingStyleComp,
    BDiagnOutputPtr             &diagnOutputPtr)
{
    Ipp8u Scod = body.Read8u();

    bool isPrecinctSizeSpec =
                  ( (Scod & JP2F_PRECINCT_SIZE_SPEC) != 0);

    Ipp8u SGcodA = body.Read8u();
    if (SGcodA > 4)
        throw DiagnDescrCT<DJP2MarkerException,BadCODProgressionOrder>();

    codingStyleJoint.SetProgrType((JP2ProgrType)SGcodA);

    unsigned int nOfLayers = body.Read16u();
    if(!nOfLayers)
        diagnOutputPtr->Warning(DiagnDescrCT<DJP2MarkerWarning, CODOrCOCOrPOCZeroNOfLayers>());

    codingStyleJoint.SetNOfLayers(nOfLayers);

    Ipp8u SGcodC = body.Read8u();
    bool isMCTUsed;
    if (SGcodC > 1)
    {
        diagnOutputPtr->Warning(DiagnDescrCT<DJP2MarkerWarning, UnknownMCTUsage>());
        isMCTUsed = false;
    }
    else
        isMCTUsed = (SGcodC != 0);

    codingStyleJoint.SetIsSOPUsed( (Scod & JP2F_SOP_MAY_BE_USED) != 0);
    codingStyleJoint.SetIsEPHUsed( (Scod & JP2F_EPH_MAY_BE_USED) != 0);
    codingStyleJoint.SetIsMCTUsed(isMCTUsed);

//    ReadCodingStyle(body, isPrecinctSizeSpec, codingStyleComp, diagnOutputPtr);
    ReadCodingStyle(body, isPrecinctSizeSpec, codingStyleComp);
}

template<class ByteInput>
void ReadQCDBody(
    ByteInput                 &body,
    DJP2QuantComp             &quantComp)
{
    quantComp.Read(body);
}

template<class ByteInput>
unsigned int ReadMarkerCompIndex(
    ByteInput                 &body,
    unsigned int               nOfComponents)
{
    return ( nOfComponents < 257 ? body.Read8u() : body.Read16u() );
}

template<class ByteInput>
void ReadCOCBody(
    ByteInput                  &body,
    JP2CodingStyleComp         &codingStyleComp
//    , BDiagnOutputPtr             &diagnOutputPtr
    )
{
    Ipp8u Scoc = body.Read8u();

    bool isPrecinctSizeSpec = ( (Scoc & JP2F_PRECINCT_SIZE_SPEC) != 0);

//    ReadCodingStyle(body, isPrecinctSizeSpec, codingStyleComp, diagnOutputPtr);
    ReadCodingStyle(body, isPrecinctSizeSpec, codingStyleComp);
}

template<class ByteInput>
void ReadQCCBody(
    ByteInput                 &body,
    DJP2QuantComp             &quantComp)
{
    quantComp.Read(body);
}


template<class ByteInput>
void ReadSOTBody(
    ByteInput                 &body,
    unsigned int              &tileIndex,
    unsigned int              &tilePartLength,
    unsigned int              &tilePartIndex,
    unsigned int              &nOfTileParts)
{
    tileIndex      = body.Read16u();
    tilePartLength = body.Read32u();
    tilePartIndex  = body.Read8u();
    nOfTileParts   = body.Read8u();
}


template<class ByteInput>
void ReadPOCBody(
    ByteInput                 &body,
    int                        nOfComponents,
    FixedArray<JP2ProgrRange> &progrRange,
    BDiagnOutputPtr           &diagnOutputPtr)
{
    int nOfProgressions = (body.TailSize() - 2)
        / (nOfComponents < 257 ? 7 : 9);

    progrRange.ReAlloc(nOfProgressions);

    for(unsigned int progr = 0; progr < progrRange.Size(); progr++)
        ReadProgrRange(body, nOfComponents, progrRange[progr], diagnOutputPtr);
}


#endif // __DJP2MARKER_H__
