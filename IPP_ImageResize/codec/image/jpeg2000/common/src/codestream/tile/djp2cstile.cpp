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
#if defined(_MSC_VER)
#pragma warning ( disable : 4702 )
#endif
#include "djp2cstile.h"
#include "dquant.h"
#include "wtinv.h"

#include "wtinvmt.h"

#include "imageconvert.h"
#include "tilemetric.h"
#include "imageconvert.h"

inline void Convert32s32f(const SBTree<Ipp32f> &sbTree)
{
    Convert32s32f(sbTree.LxLy(), sbTree.LxLy().Size());
    for(unsigned int i = 0; i < sbTree.NOfWTLevels(); i++)
    {
        Convert32s32f(sbTree.LxHy(i), sbTree.LxHy(i).Size());
        Convert32s32f(sbTree.HxLy(i), sbTree.HxLy(i).Size());
        Convert32s32f(sbTree.HxHy(i), sbTree.HxHy(i).Size());
    }
}

inline void Convert32s32f(const SBTree<Ipp32s> &sbTreeSrc, const SBTree<Ipp32f> &sbTreeDst)
{
    Convert(sbTreeSrc.LxLy(), sbTreeDst.LxLy(), sbTreeSrc.LxLy().Size());
    for(unsigned int i = 0; i < sbTreeSrc.NOfWTLevels(); i++)
    {
        Convert(sbTreeSrc.LxHy(i), sbTreeDst.LxHy(i), sbTreeSrc.LxHy(i).Size());
        Convert(sbTreeSrc.HxLy(i), sbTreeDst.HxLy(i), sbTreeSrc.HxLy(i).Size());
        Convert(sbTreeSrc.HxHy(i), sbTreeDst.HxHy(i), sbTreeSrc.HxHy(i).Size());
    }
}

inline void Complement(const SBTree<Ipp32s> &sbTree)
{
    Complement(sbTree.LxLy(), sbTree.LxLy().Size());
    for(unsigned int i = 0; i < sbTree.NOfWTLevels(); i++)
    {
        Complement(sbTree.LxHy(i), sbTree.LxHy(i).Size());
        Complement(sbTree.HxLy(i), sbTree.HxLy(i).Size());
        Complement(sbTree.HxHy(i), sbTree.HxHy(i).Size());
    }
}



void DJP2CSTile::InitAttach(
    const DJP2CSMainHeader &csMainHeader,
    unsigned int            tileRasterNumber)
{
    m_csMainHeader = &csMainHeader;

    m_rect = TileRect(
        m_csMainHeader->ImageRectRG(),
        m_csMainHeader->TileGridOriginRG(),
        m_csMainHeader->TileSizeRG(),
        tileRasterNumber);

    unsigned int nOfComponents = m_csMainHeader->NOfComponents();

    m_sbTree.ReAlloc(nOfComponents);

    m_nextPartIndex     = 0;
    m_nOfTilePartsField = 0;
    m_isMCTApplicable   = false;
}

void DJP2CSTile::CheckHeader0AndCSMainHeaderConformance()
{
    m_isMCTApplicable = false;

    unsigned int nOfComponents = m_csMainHeader->NOfComponents();

    if(nOfComponents < 3)
    {
        if(m_header0.CodingStyleJoint().IsMCTUsed())
        m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, MCTIsNotApplicableForNOfComponents>());
        return;
    }

    const JP2CodingStyleCompTile &codingStyle = m_header0.CodingStyleComp();
    // check conformity of single WT type for all of components
    JP2WTSpecType wtType   = codingStyle[0].WTType();
    for(unsigned int component = 1; component < 3; component++)
    {
        if(codingStyle[component].WTType() != wtType)
        {
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, MCTIsNotApplicableForMixedWTTypes>());
            return;
        }
    }
    m_isMCTApplicable = true;
}

void DJP2CSTile::WT(unsigned int component, const ImageCoreC<Ipp32s, 1> &dst)
{
    Complement(m_sbTree[component]);

    const JP2CodingStyleCompTile &codingStyle   = m_header0.CodingStyleComp();
    const DJP2QuantCompTile      &quant         = m_header0.Quant();
    const JP2BitsPerComp         *bitsPerComp   = m_csMainHeader->BitsPerComp();

    if(codingStyle[component].WTType()==JP2V_WT97_USED)
    {
        SBTree<Ipp32f> sbTreeTmp;

        sbTreeTmp.ReAlloc(ComponentRect(component), m_sbTree[component].NOfWTLevels());

        Convert32s32f(m_sbTree[component], sbTreeTmp);
        QuantInv(sbTreeTmp, quant[component].Quant(), bitsPerComp[component].BitDepth());
        WT97Inv(sbTreeTmp, (const ImageCoreC<Ipp32f,1>&)dst, ComponentRect(component));
    }
    else WT53Inv(m_sbTree[component], dst, ComponentRect(component));

    Complement(m_sbTree[component]);
}


void DJP2CSTile::WT(unsigned int component, const ImageCoreC<Ipp16s, 1> &dst)
{
    Complement(m_sbTree[component]);

    const JP2CodingStyleCompTile &codingStyle   = m_header0.CodingStyleComp();
    const DJP2QuantCompTile      &quant         = m_header0.Quant();
    const JP2BitsPerComp         *bitsPerComp   = m_csMainHeader->BitsPerComp();

    SBTree<Ipp16s> sbTreeTmp; // only to keep original data for further progressive decoding

    sbTreeTmp.ReAlloc(ComponentRect(component), m_sbTree[component].NOfWTLevels());

    if(codingStyle[component].WTType()==JP2V_WT97_USED)
    {
        Convert(m_sbTree[component].LxLy(), sbTreeTmp.LxLy(), m_sbTree[component].LxLy().Size());
        LShift (                            sbTreeTmp.LxLy(), m_sbTree[component].LxLy().Size(), WT97_POST_PROC_SCALE_FACTOR);

        for(unsigned int j = 0; j < m_sbTree[component].NOfWTLevels(); j++)
        {
            Convert(m_sbTree[component].LxHy(j), sbTreeTmp.LxHy(j), m_sbTree[component].LxHy(j).Size());
            LShift (sbTreeTmp.LxHy(j), m_sbTree[component].LxHy(j).Size(), WT97_POST_PROC_SCALE_FACTOR);

            Convert(m_sbTree[component].HxLy(j), sbTreeTmp.HxLy(j), m_sbTree[component].HxLy(j).Size());
            LShift (sbTreeTmp.HxLy(j), m_sbTree[component].HxLy(j).Size(), WT97_POST_PROC_SCALE_FACTOR);

            Convert(m_sbTree[component].HxHy(j), sbTreeTmp.HxHy(j), m_sbTree[component].HxHy(j).Size());
            LShift (sbTreeTmp.HxHy(j), m_sbTree[component].HxHy(j).Size(), WT97_POST_PROC_SCALE_FACTOR);
        }

        QuantInv(sbTreeTmp, quant[component].Quant(), bitsPerComp[component].BitDepth());
        WT97Inv(sbTreeTmp, (const ImageCoreC<Ipp16s, 1> &)dst, ComponentRect(component));
    }
    else
    {
        Convert(m_sbTree[component].LxLy(), sbTreeTmp.LxLy(), m_sbTree[component].LxLy().Size());
        for(unsigned int j = 0; j < m_sbTree[component].NOfWTLevels(); j++)
        {
            Convert(m_sbTree[component].LxHy(j), sbTreeTmp.LxHy(j), m_sbTree[component].LxHy(j).Size());
            Convert(m_sbTree[component].HxLy(j), sbTreeTmp.HxLy(j), m_sbTree[component].HxLy(j).Size());
            Convert(m_sbTree[component].HxHy(j), sbTreeTmp.HxHy(j), m_sbTree[component].HxHy(j).Size());
        }
        WT53Inv(sbTreeTmp, dst, ComponentRect(component));
    }

    Complement(m_sbTree[component]);
}

void DJP2CSTile::MCT(
    const ImageCoreC<Ipp32s, 1> &srcDst0,
    const ImageCoreC<Ipp32s, 1> &srcDst1,
    const ImageCoreC<Ipp32s, 1> &srcDst2)
{
    if(m_csMainHeader->IsMCTResampled())
    {
        throw DiagnDescrCT<DJP2CSUnsuppException,mctIsResampled>();
    }
    else
    {
        if(m_header0.CodingStyleComp()[0].WTType() == JP2V_WT97_USED)
        {
            ICTInv((ImageCoreC<Ipp32f, 1>&)srcDst0, (ImageCoreC<Ipp32f, 1>&)srcDst1, (ImageCoreC<Ipp32f, 1>&)srcDst2, ComponentSize(0) );
        }
        else
        {
            RCTInv(srcDst0, srcDst1, srcDst2, ComponentSize(0) );
        }
    }
}

void DJP2CSTile::MCT(
                     const ImageCoreC<Ipp16s, 1> &srcDst0,
                     const ImageCoreC<Ipp16s, 1> &srcDst1,
                     const ImageCoreC<Ipp16s, 1> &srcDst2)
{
    if(m_csMainHeader->IsMCTResampled())
    {
        throw DiagnDescrCT<DJP2CSUnsuppException,mctIsResampled>();
    }
    else
    {
        if(m_header0.CodingStyleComp()[0].WTType() == JP2V_WT97_USED)
        {
            ICTInv(srcDst0, srcDst1, srcDst2, ComponentSize(0) );
        }
        else
        {
            RCTInv(srcDst0, srcDst1, srcDst2, ComponentSize(0) );
        }
    }
}

void DJP2CSTile::UpdateComponents(const ImageCoreC<Ipp32s, 1> *dst)
{
    const JP2CodingStyleCompTile &codingStyle   = m_header0.CodingStyleComp();
    const JP2BitsPerComp         *bitsPerComp   = m_csMainHeader->BitsPerComp();
    unsigned int                  nOfComponents = m_header0.CodingStyleComp().Size();

    unsigned int component;

    for(component = 0; component < nOfComponents; component++)
        WT(component, dst[component]);

    if(m_csMainHeader->IsMCTResampled())
    {
        throw DiagnDescrCT<DJP2CSUnsuppException,mctIsResampled>();
        if(m_isMCTApplicable && m_header0.CodingStyleJoint().IsMCTUsed())
            MCT(dst[0], dst[1], dst[2]);

        for(component = 0; component < nOfComponents; component++)
        {
            if(codingStyle[component].WTType() == JP2V_WT97_USED)
                Convert32f32s((const ImageCoreC<Ipp32f, 1> &)dst[component], ComponentSize(component));

            if(!bitsPerComp[component].IsSigned())
                LevShiftInv(bitsPerComp[component].BitDepth(), dst[component], ComponentSize(component));
        }
    }
    else
    {
        PostProcess32f32s(dst, m_csMainHeader->BitsPerComp(), m_header0.CodingStyleComp().Size(),
            m_isMCTApplicable && m_header0.CodingStyleJoint().IsMCTUsed(),
            (m_header0.CodingStyleComp()[0].WTType() == JP2V_WT53_USED) );
    }
}

void DJP2CSTile::UpdateComponents(const ImageCoreC<Ipp16s, 1> *dst)
{
    unsigned int                  nOfComponents = m_header0.CodingStyleComp().Size();

    unsigned int component;

    for(component = 0; component < nOfComponents; component++)
        WT(component, dst[component]);

    if(m_csMainHeader->IsMCTResampled())
    {
        throw DiagnDescrCT<DJP2CSUnsuppException,mctIsResampled>();
    }
    else
    {
        PostProcess(dst, m_csMainHeader->BitsPerComp(), m_header0.CodingStyleComp().Size(),
            m_isMCTApplicable && m_header0.CodingStyleJoint().IsMCTUsed(),
            (m_header0.CodingStyleComp()[0].WTType() == JP2V_WT53_USED) );
    }
}
