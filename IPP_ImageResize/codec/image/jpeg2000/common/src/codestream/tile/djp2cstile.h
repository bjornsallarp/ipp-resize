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

#ifndef __DJP2CSTILE_H__
#define __DJP2CSTILE_H__

#include "image.h"
#include "djp2csmainheader.h"
#include "djp2cstilepartheader.h"
#include "jp2tileprogriterator.h"
#include "dtile.h"
#include "djp2cswarning.h"
#include "djp2csunsuppexception.h"
#include "bdiagnoutput.h"
#include "lookahead.h"

#include "dpostprocess.h"


class DJP2CSTile
{
public:
    DJP2CSTile()
    : m_csMainHeader(0)
    , m_nextPartIndex(0)
    , m_nOfTilePartsField(0)
    , m_isMCTApplicable(false)
    {}

    void InitAttach(const DJP2CSMainHeader &csMainHeader,
                    unsigned int            tileRasterNumber);

    void AttachDiagnOutput(BDiagnOutput &diagnOutput)
    {
        m_diagnOutputPtr = diagnOutput;
        m_header0.AttachDiagnOutput(diagnOutput);
        m_headerN.AttachDiagnOutput(diagnOutput);
        m_ebcot  .AttachDiagnOutput(diagnOutput);
    }

    // ReadNextPartHeader function does not read SOT marker and its body
    // becouse it's more convenient to make it in parent
    // parsing loop which is responsible for reading SOT markers
    // and redirecting parsing to responsible tile.
    //
    // NOTE: tilePartIndex and nOfTileParts is used only for conformity check.
    // The actual value of TNsot (nOfTileParts) can be zero.
    //
    // NOTE: Instead of tilePartIndex and nOfTileParts
    // it's a parent (caller) obligation to care
    // about current tile-part ending and calling of ReadNextPartHeader.
    // It could be done based on SOT marker field Psot (tilePartLength)
    // and it's more convenient to make it in parent
    // parsing loop which is responsible for reading SOT markers
    // and which is able to manage tile-part stream in explicit manner.
    //
    template<class ByteInput>
        void ReadNextPartHeader(ByteInput &stream, unsigned int tilePartIndex, unsigned int nOfTileParts)
    {
        if(m_nextPartIndex != tilePartIndex)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, SOTTilePartIndexMismatch>());

        if(nOfTileParts)
        {
            if(m_nOfTilePartsField && m_nOfTilePartsField != nOfTileParts)
            {
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, SOTNOfTilePartsMismatch>());
                m_nOfTilePartsField = Max(m_nOfTilePartsField, nOfTileParts);
            }
            else
                m_nOfTilePartsField = nOfTileParts;
        }

        if(m_nOfTilePartsField && m_nextPartIndex >= m_nOfTilePartsField)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, ActualNOfTilePartsExceedSpecifiedInSOT>());

        if(!m_nextPartIndex) ReadPartHeader0(stream);
        else                 ReadPartHeaderN(stream);
    }

    // Return false in the end of progression.
    // NOTE: So it's a parent (caller) obligation to care
    // about current tile-part ending and calling of ReadNextPartHeader.
    // It could be done based on SOT marker field Psot
    // and it's more convenient to make it in parent
    // parsing loop which is responsible for reading SOT markers
    // and which is able to manage tile-part stream in explicit manner.
    template<class ByteInput>
        bool ReadPacket(ByteInput &stream)
    {
        if(!m_progrIter.IsInRange())
            return false;

        if(m_header0.CodingStyleJoint().IsSOPUsed() ||
           m_header0.CodingStyleJoint().IsEPHUsed())
        {
          ByteInputLookAhead<ByteInput> laStream;
          laStream.Attach(stream);

          m_ebcot.ReadPacket(
            laStream,
            m_progrIter.Component(),
            m_progrIter.ResLevel(),
            m_progrIter.Precinct(),
            m_progrIter.Layer());
        }
        else
          m_ebcot.ReadPacket(
                  stream,
                  m_progrIter.Component(),
                  m_progrIter.ResLevel(),
                  m_progrIter.Precinct(),
                  m_progrIter.Layer());

        m_progrIter++;

        return true;
    }

    void UpdateComponents(const ImageCoreC<Ipp32s, 1> *dst);
    void UpdateComponents(const ImageCoreC<Ipp16s, 1> *dst);

    const Rect& RectRG() const { return m_rect; }

    RectSize ComponentSize(unsigned int component) const { return DecimateSize(m_rect.Size(), m_csMainHeader->SampleSizeRG()[component]); }
    Rect     ComponentRect(unsigned int component) const { return DecimateRect(m_rect,        m_csMainHeader->SampleSizeRG()[component]); }

protected:
    // note it does not convert 32s to 32f in final calculations if irreversible transform is used
    // (to provide effective MCT support
    void WT(unsigned int component, const ImageCoreC<Ipp32s, 1> &dst);

    void WT(unsigned int component, const ImageCoreC<Ipp16s, 1> &dst);

    void MCT(
        const ImageCoreC<Ipp32s, 1> &srcDst0,
        const ImageCoreC<Ipp32s, 1> &srcDst1,
        const ImageCoreC<Ipp32s, 1> &srcDst2);

    void MCT(
        const ImageCoreC<Ipp16s, 1> &srcDst0,
        const ImageCoreC<Ipp16s, 1> &srcDst1,
        const ImageCoreC<Ipp16s, 1> &srcDst2);

    template <class T>
    void PostProcessWT53(
        const ImageCoreC<T, 1> *srcDst,
        const JP2BitsPerComp   *bitsPerComp,
        unsigned int            nOfComponents,
        bool                    isMCTNeeded
        )
    {
        if(!isMCTNeeded || nOfComponents < 3)
        {
            for(unsigned int component = 0; component < nOfComponents; component++)
            {
                RectSize       size            = ComponentSize(component);
                JP2BitsPerComp bitsPerCompCurr = bitsPerComp  [component];

                if(!bitsPerCompCurr.IsSigned()) LevShiftInvClip(bitsPerCompCurr.BitDepth(), srcDst[component], size);
                else                            SignedClip     (bitsPerCompCurr.BitDepth(), srcDst[component], size);
            }
        }
        else LevShiftRCTInv(srcDst, ComponentSize(0), bitsPerComp);
    }

    template <class T>
    void PostProcessWT97(
        const ImageCoreC<T, 1> *srcDst,
        const JP2BitsPerComp   *bitsPerComp,
        unsigned int            nOfComponents,
        bool                    isMCTNeeded,
        int                     scaleFactor
        )
    {
        if(!isMCTNeeded || nOfComponents != 3)
        {
            for(unsigned int component = 0; component < nOfComponents; component++)
            {
                RectSize       size            = ComponentSize(component);
                JP2BitsPerComp bitsPerCompCurr = bitsPerComp  [component];

                if(!bitsPerCompCurr.IsSigned()) LevShiftInv(bitsPerCompCurr.BitDepth(), srcDst[component], size, scaleFactor);
                else                            RShift     (                            srcDst[component], size, scaleFactor);
            }
        }
        else LevShiftICTInv(srcDst, ComponentSize(0), bitsPerComp, scaleFactor);
    }

    inline void PostProcessWT97_32f32s(
        const ImageCoreC<Ipp32s, 1> *srcDst,
        const JP2BitsPerComp        *bitsPerComp,
        unsigned int                 nOfComponents,
        bool                         isMCTNeeded
        )
    {
        if(!isMCTNeeded || nOfComponents != 3)
        {
            for(unsigned int component = 0; component < nOfComponents; component++)
            {
                RectSize       size            = ComponentSize(component);
                JP2BitsPerComp bitsPerCompCurr = bitsPerComp  [component];

                if(!bitsPerCompCurr.IsSigned()) LevShiftInvClip32f32s  (bitsPerCompCurr.BitDepth(), srcDst[component], size);
                else                            SignedConvertClip32f32s(bitsPerCompCurr.BitDepth(), srcDst[component], size);
            }
        }
        else 
            LevShiftICTInv32f32s(srcDst, ComponentSize(0), bitsPerComp);
    }

    static const unsigned int WT97_POST_PROC_SCALE_FACTOR = 4;

    inline void PostProcess(
                            const ImageCoreC<Ipp16s, 1> *srcDst,
                            const JP2BitsPerComp        *bitsPerComp,
                            unsigned int                 nOfComponents,
                            bool                         isMCTNeeded,
                            bool                         isWT53Used
                            )
    {
        if(isWT53Used) 
            PostProcessWT53(srcDst, bitsPerComp, nOfComponents, isMCTNeeded);
        else
            PostProcessWT97(srcDst, bitsPerComp, nOfComponents, isMCTNeeded, WT97_POST_PROC_SCALE_FACTOR);
    }

    inline void PostProcess32f32s(
        const ImageCoreC<Ipp32s, 1> *srcDst,
        const JP2BitsPerComp        *bitsPerComp,
        unsigned int                 nOfComponents,
        bool                         isMCTNeeded,
        bool                         isWT53Used
        )
    {
        if(isWT53Used) PostProcessWT53       (srcDst, bitsPerComp, nOfComponents, isMCTNeeded);
        else           PostProcessWT97_32f32s(srcDst, bitsPerComp, nOfComponents, isMCTNeeded);
    }

    // This functions does not read SOT marker and its body
    // becouse it's more convenient to make it in parent
    // parsing loop which is responsible for reading SOT markers
    // and redirecting parsing to responsible tile.
    template<class ByteInput>
        void ReadPartHeader0(ByteInput &stream)
    {
        m_header0.Read(
            stream,
            m_csMainHeader->Quant(),
            m_csMainHeader->CodingStyleJoint(),
            m_csMainHeader->CodingStyleComp(),
            m_csMainHeader->ChangedProgrRange());


        CheckHeader0AndCSMainHeaderConformance();

        const JP2CodingStyleCompTile &codingStyle = m_header0.CodingStyleComp();

        for(unsigned int component = 0; component < codingStyle.Size(); component++)
            m_sbTree [component].ReAlloc(ComponentRect(component), codingStyle[component].NOfWTLevels());

        m_progrIter.InitAttachStartProgrRangeTilePart0(
            m_rect,
            m_csMainHeader->SampleSizeRG(),
            m_header0.CodingStyleJoint(),
            m_header0.CodingStyleComp(),
            m_header0.ChangedProgrRange());

        m_ebcot.Init(
            m_sbTree,
            m_rect,
            m_csMainHeader->SampleSizeRG(),
            m_csMainHeader->BitsPerComp(),
            m_header0.CodingStyleJoint().NOfLayers(),
            m_header0.CodingStyleComp(),
            m_header0.Quant());

        m_nextPartIndex++;
    }

    template<class ByteInput>
        void ReadPartHeaderN(ByteInput &stream)
    {
        m_headerN.Read(stream, m_header0.ChangedProgrRange(), m_csMainHeader->NOfComponents());

        m_progrIter.AttachChangeProgrRangeTilePartN(m_headerN.ChangedProgrRange());

        m_nextPartIndex++;
    }

    void CheckHeader0AndCSMainHeaderConformance();

    FixedBuffer<RectSize >          m_compSize;
    BDiagnOutputPtr                 m_diagnOutputPtr;

    Rect                            m_rect;
    const DJP2CSMainHeader         *m_csMainHeader;
    DJP2CSTilePartHeader0           m_header0;
    DJP2CSTilePartHeaderN           m_headerN;

    JP2TileProgrIterator            m_progrIter;
    DTile                           m_ebcot;

    FixedBuffer<SBTree<Ipp32s> >    m_sbTree;

    unsigned int                    m_nextPartIndex;
    unsigned int                    m_nOfTilePartsField;

    bool                            m_isMCTApplicable;
};

#endif // __DJP2CSTILE_H__
