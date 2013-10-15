/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2008-2012 Intel Corporation. All Rights Reserved.
//
*/

#include "jpegxrenc.h"
#include "jpegxrenc_cc.h"
#include "jpegxrvalues.h"
#ifndef __IPPI_H__
#include "ippi.h"
#endif

static const Ipp8u pVLCCodeDCType[34] = {
  8,2,2,1,3,1,5,1,4,3,2,2,3,0,5,3,3,
  8,1,3,2,3,1,4,3,3,4,3,5,3,0,4,3,2
};

static const Ipp8u pVLCCodeCBPCh[9] = {4, 1, 1, 1, 2, 0, 3, 1, 3};

static const Ipp32u quantManTable[32] = {
  0x0,        0x0,        0x0,        0xaaaaaaab, 0x0,        0xcccccccd, 0xaaaaaaab, 0x92492493,
  0x0,        0xe38e38e4, 0xcccccccd, 0xba2e8ba3, 0xaaaaaaab, 0x9d89d89e, 0x92492493, 0x88888889,
  0x0,        0xf0f0f0f1, 0xe38e38e4, 0xd79435e6, 0xcccccccd, 0xc30c30c4, 0xba2e8ba3, 0xb21642c9,
  0xaaaaaaab, 0xa3d70a3e, 0x9d89d89e, 0x97b425ee, 0x92492493, 0x8d3dcb09, 0x88888889, 0x84210843
};

static const Ipp8u quantExpTable[32] = {
  0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};

static const Ipp8u iDCValue[]    = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};

// Lengh/code tables for encodeHPCBP()
static const Ipp8u iCBPLenghtTable1[16] = { 0, 2, 2, 2, 2, 2, 3, 2, 2, 3, 3, 2, 3, 2, 2, 0};
static const Ipp8u iCBPCodeTable1[16]   = { 0, 0, 1, 0, 2, 1, 4, 3, 3, 5, 6, 2, 7, 1, 0, 0};
static const Ipp8u iCBPValueTable[16]   = { 0, 1, 1, 2, 1, 3, 3, 4, 1, 3, 3, 4, 2, 4, 4, 5};
static const Ipp8u iCBPLenghtTable2[16] = { 0, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 0};
static const Ipp8u iCBPCodeTable2[16]   = { 0, 0, 1, 0, 2, 0, 1, 0, 3, 2, 3, 1, 1, 2, 3, 0};


static IppStatus ippiCopy_C1R(const Ipp32s* pSrc, int srcStep, Ipp32s* pDst, int dstStep, IppiSize roiSize)
{
  return ippiCopy_32s_C1R(pSrc, srcStep, pDst, dstStep, roiSize);
}

static IppStatus ippiCopy_C1R(const Ipp16s* pSrc, int srcStep, Ipp16s* pDst, int dstStep, IppiSize roiSize)
{
  return ippiCopy_16s_C1R(pSrc, srcStep, pDst, dstStep, roiSize);
}

static IppStatus ippiPCTFwd16x16DC_HDP(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale)
{
  return ippiPCTFwd16x16DC_HDP_32s_C1IR(pSrcDst, srcDstStep, downscale);
}

static IppStatus ippiPCTFwd16x16DC_HDP(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale)
{
  return ippiPCTFwd16x16DC_HDP_16s_C1IR(pSrcDst, srcDstStep, downscale);
}

static IppStatus ippiPCTFwd8x16DC_HDP(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale)
{
  return ippiPCTFwd8x16DC_HDP_32s_C1IR(pSrcDst, srcDstStep, downscale);
}

static IppStatus ippiPCTFwd8x16DC_HDP(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale)
{
  return ippiPCTFwd8x16DC_HDP_16s_C1IR(pSrcDst, srcDstStep, downscale);
}

static IppStatus ippiPCTFwd8x8DC_HDP(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale)
{
  return ippiPCTFwd8x8DC_HDP_32s_C1IR(pSrcDst, srcDstStep, downscale);
}

static IppStatus ippiPCTFwd8x8DC_HDP(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale)
{
  return ippiPCTFwd8x8DC_HDP_16s_C1IR(pSrcDst, srcDstStep, downscale);
}

static IppStatus ippiPCTFwd16x16_JPEGXR(Ipp32s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTFwd16x16_JPEGXR_32s_C1IR(pSrcDst, srcDstStep);
}

static IppStatus ippiPCTFwd16x16_JPEGXR(Ipp16s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTFwd16x16_JPEGXR_16s_C1IR(pSrcDst, srcDstStep);
}

static IppStatus ippiPCTFwd8x16_JPEGXR(Ipp32s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTFwd8x16_JPEGXR_32s_C1IR(pSrcDst, srcDstStep);
}

static IppStatus ippiPCTFwd8x16_JPEGXR(Ipp16s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTFwd8x16_JPEGXR_16s_C1IR(pSrcDst, srcDstStep);
}

static IppStatus ippiPCTFwd8x8_JPEGXR(Ipp32s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTFwd8x8_JPEGXR_32s_C1IR(pSrcDst, srcDstStep);
}

static IppStatus ippiPCTFwd8x8_JPEGXR(Ipp16s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTFwd8x8_JPEGXR_16s_C1IR(pSrcDst, srcDstStep);
}

static IppStatus ippiPCTFwd_JPEGXR(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize)
{
  return ippiPCTFwd_JPEGXR_32s_C1IR(pSrcDst, srcDstStep, roiSize);
}

static IppStatus ippiPCTFwd_JPEGXR(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize)
{
  return ippiPCTFwd_JPEGXR_16s_C1IR(pSrcDst, srcDstStep, roiSize);
}

static IppStatus ippiFilterFwd_HDP(Ipp32s* pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterFwd_HDP_32s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}

static IppStatus ippiFilterFwd_HDP(Ipp16s* pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterFwd_HDP_16s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}

static IppStatus ippiFilterFwdDCYUV444_HDP(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterFwdDCYUV444_HDP_32s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}

static IppStatus ippiFilterFwdDCYUV444_HDP(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterFwdDCYUV444_HDP_16s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}

static IppStatus ippiFilterFwdDCYUV420_HDP(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterFwdDCYUV420_HDP_32s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}

static IppStatus ippiFilterFwdDCYUV420_HDP(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterFwdDCYUV420_HDP_16s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}

/***********************************/
// Encoder initialization functions
/***********************************/
static void adaptHuffman(AdaptiveHuffman *pHuff);

static void setQuantizer(QuantInf *pQuant, Ipp32u iShift, Ipp32u bScalingFlag)
{
  Ipp8u iQPIndex = pQuant->iIndex;

  if(iQPIndex == 0)
  {
    pQuant->iQP = 1;
    pQuant->iMan = pQuant->iExp = pQuant->iOffset = 0;
  }
  else
  {
    if(bScalingFlag)
    {
      Ipp32s iMan = 0;
      Ipp32s iExp = 0;

      if(pQuant->iIndex < 16)
      {
        iMan = pQuant->iIndex;
        iExp = iShift;
      }
      else
      {
        iMan = 16 + (pQuant->iIndex & 0xf);
        iExp = ((pQuant->iIndex >> 4) - 1) + iShift;
      }

      pQuant->iQP = iMan << iExp;
      pQuant->iMan = quantManTable[iMan];
      pQuant->iExp = quantExpTable[iMan] + iExp;
      pQuant->iOffset = ((pQuant->iQP * 3 + 1) >> 3);
    }
    else
    {
      Ipp32s iMan = 0;
      Ipp32s iExp = 0;

      if(pQuant->iIndex < 32)
      {
        iMan = (pQuant->iIndex + 3) >> 2;
        iExp = 0;
      }
      else if(pQuant->iIndex < 48)
      {
        iMan = (17 + (pQuant->iIndex & 0xf)) >> 1;
        iExp = (pQuant->iIndex >> 4) - 2;
      }
      else
      {
        iMan = 16 + (pQuant->iIndex & 0xf);
        iExp = (pQuant->iIndex >> 4) - 3;
      }

      pQuant->iQP = iMan << iExp;
      pQuant->iMan = quantManTable[iMan];
      pQuant->iExp = quantExpTable[iMan] + iExp;
      pQuant->iOffset = ((pQuant->iQP * 3 + 1) >> 3);
    }
  }
} // setQuantizer()

// Quantizers initialization
static void initQuantizers(Ipp8u **ppAllocPtr, ImagePlane* pImagePlane,
                           QuantInf** pQuantDC, QuantInf** pQuantLP, QuantInf** pQuantHP)
{
  Ipp8u *pAllocPtr = *ppAllocPtr;
  Ipp32u iChannels = pImagePlane->iChannels;
  Ipp32u iCHModeDC = pImagePlane->iCHModeDC;
  Ipp32u iCHModeLP = pImagePlane->iCHModeLP;
  Ipp32u iCHModeHP = pImagePlane->iCHModeHP;
  Ipp32u bScalingFlag = pImagePlane->bScalingFlag;
  Ipp32u iShift;
  Ipp32u i;

  // Allocate Uniform Quantizers
  iShift = sizeof(QuantInf) * iChannels;
  if(pImagePlane->bDCFrameUniform)
  {
    pQuantDC[0] = (QuantInf*)pAllocPtr; pAllocPtr += iShift;
    for(i = 1; i < iChannels; i++)
      pQuantDC[i] = pQuantDC[i - 1] + 1;

    for(i = 0; i < iChannels; i++)
    {
      pQuantDC[i]->iIndex = (Ipp8u)(pImagePlane->iDCQuant[i]);
      if(i > 0 && iCHModeDC == JXR_CM_UNIFORM)
        pQuantDC[i][0] = pQuantDC[0][0];
      else if(i > 0 && iCHModeDC == JXR_CM_SEPARATE)
        pQuantDC[i][0] = pQuantDC[1][0];
      setQuantizer(pQuantDC[i], (i > 0) ? 0 : 1, bScalingFlag);
      pQuantDC[i]->iOffset = pQuantDC[i]->iQP >> 1;
    }
  }
  if(pImagePlane->iBands < JXR_SB_DCONLY)
  {
    if(pImagePlane->bLPFrameUniform)
    {
      pQuantLP[0] = (QuantInf*)pAllocPtr; pAllocPtr += iShift;
      for(i = 1; i < iChannels; i++)
        pQuantLP[i] = pQuantLP[i - 1] + 1;

      for(i = 0; i < iChannels; i++)
      {
        pQuantLP[i]->iIndex = (Ipp8u)(pImagePlane->iLPQuant[i]);
        if(i > 0 && iCHModeLP == JXR_CM_UNIFORM)
          pQuantLP[i][0] = pQuantLP[0][0];
        else if(i > 0 && iCHModeLP == JXR_CM_SEPARATE)
          pQuantLP[i][0] = pQuantLP[1][0];
        setQuantizer(pQuantLP[i], (i > 0) ? 0 : 1, bScalingFlag);
      }
    }
  }
  if(pImagePlane->iBands < JXR_SB_NOHP)
  {
    if(pImagePlane->bHPFrameUniform)
    {
      pQuantHP[0] = (QuantInf*)pAllocPtr; pAllocPtr += iShift;
      for(i = 1; i < iChannels; i++)
        pQuantHP[i] = pQuantHP[i - 1] + 1;

      for(i = 0; i < iChannels; i++)
      {
        pQuantHP[i]->iIndex = (Ipp8u)(pImagePlane->iHPQuant[i]);
        if(i > 0 && iCHModeHP == JXR_CM_UNIFORM)
          pQuantHP[i][0] = pQuantHP[0][0];
        else if(i > 0 && iCHModeHP == JXR_CM_SEPARATE)
          pQuantHP[i][0] = pQuantHP[1][0];
        setQuantizer(pQuantHP[i], 1, bScalingFlag);
      }
    }
  }
  *ppAllocPtr = pAllocPtr;
} // initQuantizers()


static void initPredictor(Ipp8u **ppAllocPtr, Ipp32u iChannels, Ipp32u iWidth, PredictorInf** pPred, PredictorInf** pPredPrev)
{
  Ipp8u *pAllocPtr = *ppAllocPtr;
  Ipp32u iShift = iWidth * sizeof(PredictorInf);
  Ipp32u i, j;

  for(i = 0; i < iChannels; i++)
  {
    pPred[i] = (PredictorInf*)pAllocPtr; pAllocPtr += iShift;
    pPredPrev[i] = (PredictorInf*)pAllocPtr; pAllocPtr += iShift;

    for(j = 0; j < iWidth; j++)
    {
      pPred[i][j].iDCLP = pPred[i][j].iLP;
      pPredPrev[i][j].iDCLP = pPredPrev[i][j].iLP;
    }
  }
  *ppAllocPtr = pAllocPtr;
} // initPredictor()

// Main encoder allocation and init
ExcStatus CJPEGXREncoder::initEncoder()
{
  Ipp8u *pAllocPtr;
  Ipp32u iAllocSize;
  Ipp32u iShift;
  Ipp32u iMBSize;
  Ipp32u iMaxMBTileWidth = 0;
  Ipp32s iHuffSize;
  Ipp32u iCCSize;
  Ipp32u iLumaSize, iChromaSize, iChromaBlock;
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u iBands = m_pImagePlane->iBands;
  Ipp32u bResampling = m_pImagePlane->bResampling;
  Ipp32u i;

  m_iCUSize = (m_iUnitSize == 1)?((m_iSrcColorFormat == JXR_OCF_RGBE)?4:2):4; // Internal calculations data range
  m_iMBSize = 256 * m_iUnitSize * iChannels;

  if(m_bLongWordFlag)
    iMBSize = 256 * 4;
  else
    iMBSize = 256 * 2;

  iChromaSize = iLumaSize = iMBSize;
  iChromaBlock = 16;
  if(iColorFormat == JXR_ICF_YUV422)
    iChromaSize = iMBSize/2, iChromaBlock = 8;
  else if(iColorFormat == JXR_ICF_YUV420)
    iChromaSize = iMBSize/4, iChromaBlock = 8;
  ippiVLCGetStateSize_JPEGXR(&iHuffSize);

  for(i = 0; i < m_iTilesUniform[0]; i++)
    iMaxMBTileWidth = (iMaxMBTileWidth < m_iTileMBWidth[i])?m_iTileMBWidth[i]:iMaxMBTileWidth;
  iCCSize = 256 * iMaxMBTileWidth * m_iUnitSize * ((bResampling)?iChannels + 2:iChannels);

  // Allocate general memory
  iAllocSize = 2 * iMBSize * m_iChannels * sizeof(PredictorInf); // Predictor info
  iAllocSize += m_iChannels * sizeof(QuantInf); // Quantizers Info DC
  if(iBands != JXR_SB_DCONLY)
  {
    iAllocSize += m_iChannels * sizeof(QuantInf); // Quantizers Info LP
    if(iBands != JXR_SB_NOHP)
      iAllocSize += m_iChannels * sizeof(QuantInf); // Quantizers Info HP
  }
  iAllocSize += (m_bAlphaFlag)?iHuffSize * 2:iHuffSize; // Adaptive huffman
  iAllocSize += (iMaxMBTileWidth * 16 + 1024) * m_iCUSize * m_iChannels; // MB4
  iAllocSize += 2 * iMaxMBTileWidth * (((m_bAlphaFlag)?iLumaSize * 2:iLumaSize) + iChromaSize * ((m_bAlphaFlag)?(m_iChannels - 2):(iChannels - 1))); // CC
  iAllocSize += iCCSize; // Color convertion buffer

  m_pAllocBuffer = pAllocPtr = (Ipp8u*)ippsMalloc_8u(iAllocSize);
  ippsZero_8u(m_pAllocBuffer, iAllocSize);

  m_pCCBuf = pAllocPtr; pAllocPtr += iCCSize;

  m_iHPQuantizerIndex = m_iLPQuantizerIndex = 0;
  initQuantizers(&pAllocPtr, m_pImagePlane, m_pQuantDC, m_pQuantLP, m_pQuantHP);

  // Init MB Tab
  iShift = (iMaxMBTileWidth * 16 + 1024) * m_iCUSize;
  for(i = 0; i < m_iChannels; i++)
  {
    m_pMBTab[i].pMB4 = m_pMBTab[i].pSliceMB4 = (Ipp32s*)pAllocPtr; pAllocPtr += iShift;
    m_pMBTab[i].iStepMB4 = m_iCUSize * 32;
  }

  iShift = iLumaSize * iMaxMBTileWidth;
  for(i = 0; i < iChannels; i++)
  {
    m_pMBTab[i].pSliceCC0 = m_pMBTab[i].pCC0 = (Ipp32s*)pAllocPtr; pAllocPtr += iShift;
    m_pMBTab[i].pSliceCC1 = m_pMBTab[i].pCC1 = (Ipp32s*)pAllocPtr; pAllocPtr += iShift;
    m_pMBTab[i].iStepCC = iMaxMBTileWidth * m_iCUSize * ((i)?iChromaBlock:16);
    iShift = iChromaSize * iMaxMBTileWidth;
  }

  if(bResampling)
  {
    iShift = iLumaSize * iMaxMBTileWidth;
    m_pResU = (Ipp32s*)pAllocPtr; pAllocPtr += iShift;
    m_pResV = (Ipp32s*)pAllocPtr; pAllocPtr += iShift;
  }

  // Init Adaptive Huffman
  m_pImagePlane->pHuffState = (IppiVLCState_JPEGXR*)pAllocPtr; pAllocPtr += (iHuffSize);
  ippsZero_8u((Ipp8u*)&m_pImagePlane->pAdaptCBP,      sizeof(AdaptiveHuffman));
  ippsZero_8u((Ipp8u*)&m_pImagePlane->pAdaptBlockCBP, sizeof(AdaptiveHuffman));
  ippsZero_8u((Ipp8u*)&m_pImagePlane->pAdaptDCY,      sizeof(AdaptiveHuffman));
  ippsZero_8u((Ipp8u*)&m_pImagePlane->pAdaptDCUV,     sizeof(AdaptiveHuffman));

  if(m_bAlphaFlag)
  {
    m_pImagePlane = &m_ImagePlane[JXR_PLANE_ALPHA];
    initQuantizers(&pAllocPtr, m_pImagePlane, m_pQuantDC + iChannels, m_pQuantLP + iChannels, m_pQuantHP + iChannels);

    iShift = iLumaSize * iMaxMBTileWidth;
    for(i = iChannels; i < m_iChannels; i++)
    {
      m_pMBTab[i].pSliceCC0 = m_pMBTab[i].pCC0 = (Ipp32s*)pAllocPtr; pAllocPtr += iShift;
      m_pMBTab[i].pSliceCC1 = m_pMBTab[i].pCC1 = (Ipp32s*)pAllocPtr; pAllocPtr += iShift;
      m_pMBTab[i].iStepCC = iMaxMBTileWidth * m_iCUSize * iChromaBlock;
      iShift = iChromaSize * iMaxMBTileWidth;
    }

    m_pImagePlane->pHuffState = (IppiVLCState_JPEGXR*)pAllocPtr; pAllocPtr += (iHuffSize);
    ippsZero_8u((Ipp8u*)&m_pImagePlane->pAdaptCBP,      sizeof(AdaptiveHuffman));
    ippsZero_8u((Ipp8u*)&m_pImagePlane->pAdaptBlockCBP, sizeof(AdaptiveHuffman));
    ippsZero_8u((Ipp8u*)&m_pImagePlane->pAdaptDCY,      sizeof(AdaptiveHuffman));
    ippsZero_8u((Ipp8u*)&m_pImagePlane->pAdaptDCUV,     sizeof(AdaptiveHuffman));
    m_pImagePlane = &m_ImagePlane[JXR_PLANE_PRIMARY];
  }

  // Allocate Predictor Info
  initPredictor(&pAllocPtr, m_iChannels, iMBSize, m_pPred, m_pPredPrev);

  return ExcStatusOk;
} // CJPEGXREncoder::initEncoder()

// Decoder reset for tile decoding
ExcStatus CJPEGXREncoder::resetEncoder()
{
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;

  // Reset Decoding Context
  m_pImagePlane->pAdaptDCY.iSymbols  = 7;
  m_pImagePlane->pAdaptDCUV.iSymbols = 7;
  m_pImagePlane->pAdaptCBP.iSymbols  = 5;
  if(iColorFormat == JXR_ICF_Y || iColorFormat == JXR_ICF_NCH || iColorFormat == JXR_ICF_YUVK)
    m_pImagePlane->pAdaptBlockCBP.iSymbols = 5;
  else
    m_pImagePlane->pAdaptBlockCBP.iSymbols = 9;
  m_pImagePlane->pAdaptDCY.bInitialize      = 0;
  m_pImagePlane->pAdaptDCUV.bInitialize     = 0;
  m_pImagePlane->pAdaptCBP.bInitialize      = 0;
  m_pImagePlane->pAdaptBlockCBP.bInitialize = 0;
  adaptHuffman(&m_pImagePlane->pAdaptDCY);
  adaptHuffman(&m_pImagePlane->pAdaptDCUV);
  adaptHuffman(&m_pImagePlane->pAdaptCBP);
  adaptHuffman(&m_pImagePlane->pAdaptBlockCBP);
  ippiVLCInit_JPEGXR(m_pImagePlane->pHuffState);

  // reset bit reduction models
  ippsZero_8u((Ipp8u*)&m_pImagePlane->pModelDC, sizeof(AdaptiveModel));
  m_pImagePlane->pModelDC.iBand = JXR_BT_DC;
  m_pImagePlane->pModelDC.iModelBits[0] = m_pImagePlane->pModelDC.iModelBits[1] = 8;

  ippsZero_8u((Ipp8u*)&m_pImagePlane->pModelLP, sizeof(AdaptiveModel));
  m_pImagePlane->pModelLP.iBand = JXR_BT_LP;
  m_pImagePlane->pModelLP.iModelBits[0] = m_pImagePlane->pModelLP.iModelBits[1] = 4;

  ippsZero_8u((Ipp8u*)&m_pImagePlane->pModelHP, sizeof(AdaptiveModel));
  m_pImagePlane->pModelHP.iBand = JXR_BT_HP;

  // reset CBP models
  m_pImagePlane->iCBPCountMax = m_pImagePlane->iCBPCountZero = 1;
  m_pImagePlane->pCBPModel.iCount0[0] = m_pImagePlane->pCBPModel.iCount0[1] = -4;
  m_pImagePlane->pCBPModel.iCount1[0] = m_pImagePlane->pCBPModel.iCount1[1] = 4;
  m_pImagePlane->pCBPModel.iState[0] = m_pImagePlane->pCBPModel.iState[1] = 0;

  return ExcStatusOk;
} // CJPEGXREncoder::resetEncoder()


ExcStatus CJPEGXREncoder::KillEncoder()
{
  ippsFree(m_iTileMBWidth);
  ippsFree(m_pAllocBuffer);
  if(m_bIndexFlag)
    ippsFree(m_pIndexTable);
  m_bFirstLoop = 1;

  return ExcStatusOk;
} // CJPEGXREncoder::killEncoder()


/***********************************/
// Huffman decode functions
/***********************************/

static void adaptHuffman(AdaptiveHuffman *pHuff)
{
  Ipp32s iSymbols = pHuff->iSymbols;
  Ipp32s iDiscr   = pHuff->iDiscr;
  Ipp32s iLevel;
  Ipp32s bChange = 0;

  if(!pHuff->bInitialize)
  {
    pHuff->bInitialize = 1;
    pHuff->iLevel = 0;
    pHuff->iDiscr = iDiscr = 0;
  }

  if(iDiscr < pHuff->iLBound)
  {
    pHuff->iLevel--;
    bChange = 1;
  }
  else if(iDiscr > pHuff->iUBound)
  {
    pHuff->iLevel++;
    bChange = 1;
  }

  if(bChange)
    pHuff->iDiscr = 0;

  if(pHuff->iDiscr < -64)
    pHuff->iDiscr = -64;
  else if(pHuff->iDiscr > 64)
    pHuff->iDiscr = 64;

  iLevel = pHuff->iLevel;
  if(iLevel < 0 || iLevel > 2)
    return;

  pHuff->iLBound = (iLevel == 0) ? (-1 << 31) : -8;
  pHuff->iUBound = (iLevel == iMaxTables[iSymbols] - 1) ? (1 << 30) : 8;

  if(iSymbols == 5)
  {
    pHuff->pTable = iHuffmanCode5 + iLevel * 11;
    pHuff->pDelta = iHuffmanDelta5;
  }
  else if(iSymbols == 7)
  {
    pHuff->pTable = iHuffmanCode7 + iLevel * 15;
    pHuff->pDelta = iHuffmanDelta7;
  }
  else if(iSymbols == 9)
  {
    pHuff->pTable = iHuffmanCode9 + iLevel * 19;
    pHuff->pDelta = iHuffmanDelta9;
  }
} // adaptHuffman()


static void updateModel(Ipp32s iLaplasMean[2], Ipp32u iColorFormat, Ipp32u iChannels, AdaptiveModel* pModel)
{
  Ipp32s iMBState;
  Ipp32s iModelBits;
  Ipp32s iDelta;
  Ipp32u i = 0;

  iLaplasMean[0] *= iWeightY[pModel->iBand];

  switch(iColorFormat)
  {
  case JXR_ICF_YUV422:
    iLaplasMean[1] *= iWeightUV422[pModel->iBand];
    break;
  case JXR_ICF_YUV420:
    iLaplasMean[1] *= iWeightUV420[pModel->iBand];
    break;
  default:
    iLaplasMean[1] *= iWeightNCH[pModel->iBand][iChannels - 1];
    if(pModel->iBand == JXR_BT_HP)
      iLaplasMean[1] >>= 4;
  }

  do
  {
    iMBState = pModel->iMBState[i];
    iModelBits = pModel->iModelBits[i];
    iDelta = (iLaplasMean[i] - 70) >> 2;

    if(iDelta <= -8)
    {
      iDelta += 4;
      if(iDelta < -16)
        iDelta = -16;
      iMBState += iDelta;
    }
    else if(iDelta >= 8)
    {
      iDelta -= 4;
      if(iDelta > 15)
        iDelta = 15;
      iMBState += iDelta;
    }

    if(iMBState < -8)
    {
      if(iModelBits == 0)
        iMBState = -8;
      else
      {
        iMBState = 0;
        iModelBits--;
      }
    }
    else if(iMBState > 8)
    {
      if(iModelBits >= 15)
      {
        iMBState = 8;
        iModelBits = 15;
      }
      else
      {
        iMBState = 0;
        iModelBits++;
      }
    }

    pModel->iMBState[i] = iMBState;
    pModel->iModelBits[i] = iModelBits;

  } while(i++, i < 2 && iColorFormat != JXR_ICF_Y);

} // updateModel()


static void updateModelCBP(Ipp32s iCount, Ipp32u bChroma, CBPModel* pModel)
{
  Ipp32s iCount0 = pModel->iCount0[bChroma];
  Ipp32s iCount1 = pModel->iCount1[bChroma];
  Ipp32s iState  = pModel->iState[bChroma];

  iCount0 += iCount - 3;
  if(iCount0 < -16)
    iCount0 = -16;
  else if(iCount0 > 15)
    iCount0 = 15;

  iCount1 += 13 - iCount;
  if(iCount1 < -16)
    iCount1 = -16;
  else if(iCount1 > 15)
    iCount1 = 15;

  if(iCount0 < 0)
  {
    if(iCount0 < iCount1)
      iState = 1;
    else
      iState = 2;
  }
  else
  {
    if(iCount1 < 0)
      iState = 2;
    else
      iState = 0;
  }

  pModel->iCount0[bChroma] = iCount0;
  pModel->iCount1[bChroma] = iCount1;
  pModel->iState[bChroma]  = iState;

} // updateModelCBP()


static void encodeDC(Ipp32s iDC, AdaptiveHuffman* pAdaptHuff, CBitStreamOutput* pStream)
{
  Ipp32s iFLen = 0, iFDC = iDC >> 5;
  Ipp32s iIndex;

  if(iDC < 16)
  {
    iIndex = iDCValue[iDC];
    pStream->WriteBits((Ipp8u)pAdaptHuff->pTable[iIndex * 2 + 2], pAdaptHuff->pTable[iIndex * 2 + 1]);
    pAdaptHuff->iDiscr += pAdaptHuff->pDelta[iIndex];
    if(iIndex > 1)
      pStream->WriteBits(iDCLength[iIndex], iDC);
  }
  else
  {
    pStream->WriteBits((Ipp8u)pAdaptHuff->pTable[6 * 2 + 2], pAdaptHuff->pTable[6 * 2 + 1]);
    pAdaptHuff->iDiscr += pAdaptHuff->pDelta[6];

    while(iFDC)
    {
      iFLen++;
      iFDC >>= 1;
    }
    if(iFLen > 14)
    {
      pStream->WriteBits(4, 15);
      if (iFLen > 17)
      {
        pStream->WriteBits(2, 3);
        pStream->WriteBits(3, iFLen - 18);
      }
      else
        pStream->WriteBits(2, iFLen - 15);
    }
    else
      pStream->WriteBits(4, iFLen);

    pStream->WriteBits((Ipp8u)(iFLen + 4), iDC);
  }
} // encodeDC()


ExcStatus CJPEGXREncoder::writeMacroblockDC()
{
  AdaptiveModel *pModelDC = &m_pImagePlane->pModelDC;
  Ipp32s iDC, iDCU, iDCV, iSign, iMDC, iMDCU, iMDCV, iDCStatus;
  Ipp32u iModelBits = pModelDC->iModelBits[0];
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32s iLaplasMean[2] = {0,0};
  Ipp32u i, k = 0;

  if(iColorFormat == JXR_ICF_Y ||
     iColorFormat == JXR_ICF_YUVK ||
     iColorFormat == JXR_ICF_NCH)
  {
    for (i = 0; i < iChannels; i++)
    {
      iDC = m_iDCLPTable[i][0];
      iSign = (iDC < 0);
      iDC = ABS(iDC);
      iMDC = iDC >> iModelBits;

      if(iMDC)
      {
        iMDC--;
        m_BitStreamOut.WriteBits(1, 1);
        encodeDC(iMDC, &m_pImagePlane->pAdaptDCY, &m_BitStreamOut);
        iLaplasMean[k] += 1;
      }
      else
        m_BitStreamOut.WriteBits(1, 0);

      m_BitStreamOut.WriteBits((Ipp8u)iModelBits, iDC);
      if(iDC)
        m_BitStreamOut.WriteBits(1, iSign);
      m_BitStreamOut.FlushBits();
      k = 1;
      iModelBits = m_pImagePlane->pModelDC.iModelBits[1];
    }
  }
  else
  {
    iDC = m_iDCLPTable[0][0], iDCU = m_iDCLPTable[1][0], iDCV = m_iDCLPTable[2][0];

    iMDC = ABS(iDC) >> iModelBits;
    iModelBits = pModelDC->iModelBits[1];
    iMDCU = ABS(iDCU) >> iModelBits;
    iMDCV = ABS(iDCV) >> iModelBits;
    iModelBits = pModelDC->iModelBits[0];

    iDCStatus = (iMDC != 0) * 4 + (iMDCU != 0) * 2 + (iMDCV != 0);
    m_BitStreamOut.WriteBits(pVLCCodeDCType[iDCStatus * 2 + 2], pVLCCodeDCType[iDCStatus * 2 + 1]);

    // Luminance DC
    if(iMDC)
    {
      iMDC--;
      encodeDC(iMDC, &m_pImagePlane->pAdaptDCY, &m_BitStreamOut);
      iLaplasMean[0] += 1;
    }
    m_BitStreamOut.WriteBits((Ipp8u)iModelBits, ABS(iDC));
    if(iDC < 0)
      m_BitStreamOut.WriteBits(1, 1);
    else if(iDC > 0)
      m_BitStreamOut.WriteBits(1, 0);

    // Chrominance DC
    iModelBits = pModelDC->iModelBits[1];

    if(iMDCU)
    {
      iMDCU--;
      encodeDC(iMDCU, &m_pImagePlane->pAdaptDCUV, &m_BitStreamOut);
      iLaplasMean[1] += 1;
    }
    m_BitStreamOut.WriteBits((Ipp8u)iModelBits, ABS(iDCU));
    if(iDCU < 0)
      m_BitStreamOut.WriteBits(1, 1);
    else if(iDCU > 0)
      m_BitStreamOut.WriteBits(1, 0);

    if(iMDCV)
    {
      iMDCV--;
      encodeDC(iMDCV, &m_pImagePlane->pAdaptDCUV, &m_BitStreamOut);
      iLaplasMean[1] += 1;
    }
    m_BitStreamOut.WriteBits((Ipp8u)iModelBits, ABS(iDCV));
    if(iDCV < 0)
      m_BitStreamOut.WriteBits(1, 1);
    else if(iDCV > 0)
      m_BitStreamOut.WriteBits(1, 0);
  }

  updateModel(iLaplasMean, iColorFormat, iChannels, &m_pImagePlane->pModelDC);

  if(m_bResetContext)
  {
    adaptHuffman(&m_pImagePlane->pAdaptDCY);
    adaptHuffman(&m_pImagePlane->pAdaptDCUV);
  }

  return ExcStatusOk;
} // CJPEGXREncoder::writeMacroblockDC()


static void VLCScanDC4XX_JPEGXR_32s(Ipp32s* pDC[], Ipp32u iColorFormat, Ipp32u iModelBits, Ipp32s* iResidual[], Ipp32s pRLCoeffs[], Ipp32u *iNumCoeffs)
{
  const Ipp8u *pLP4XXRemap = iLP4XXRemap + (iColorFormat == JXR_ICF_YUV420);
  Ipp32s iCount = (iColorFormat == JXR_ICF_YUV420) ? 6 : 14;
  Ipp32s iCoef = 0, iRun = 0;
  Ipp32s iDC, iIndex, iVal;
  Ipp32s i;

  for(i = 0; i < iCount; i++)
  {
    iIndex = pLP4XXRemap[i >> 1];
    iDC = pDC[i & 1][iIndex];
    iResidual[i & 1][iIndex] = iVal = ABS(iDC) >> iModelBits;

    if(iVal)
    {
      pRLCoeffs[iCoef * 2] = iRun;
      pRLCoeffs[iCoef * 2 + 1] = (iDC < 0) ? -iVal : iVal;
      iCoef++;
      iRun = 0;
    }
    else
      iRun++;
  }
  *iNumCoeffs = iCoef;
} // VLCScanDC4XX_JPEGXR_32s()


static void writeLPCBP(Ipp32u iChannels, Ipp32s &m_iCBPCountMax, Ipp32s &m_iCBPCountZero, Ipp32u* iNumCoeffs, CBitStreamOutput* pStream)
{
  Ipp32u iVal;
  Ipp32s iCBP = (iNumCoeffs[0] > 0) + (iNumCoeffs[1] > 0) * 2;
  Ipp32s iCountMax = m_iCBPCountMax;
  Ipp32s iCountZero = m_iCBPCountZero;
  Ipp32s iMax = (1 << iChannels) - 1;

  if(iChannels == 3)
    iCBP += (iNumCoeffs[2] > 0) * 4;

  if(iCountZero <= 0 || iCountMax < 0)
  {
    iVal = iCBP;
    if(iCountMax < iCountZero)
      iVal = iMax - iCBP;

    if(iVal == 0)
      pStream->WriteBits(1, 0);
    else if(iVal == 1)
      pStream->WriteBits((Ipp8u)iChannels, (iChannels + 1) & 0x6);
    else
      pStream->WriteBits((Ipp8u)(iChannels + 1), iVal + iMax + 1);
  }
  else
    pStream->WriteBits((Ipp8u)iChannels, iCBP);

  iCountMax += 1 - 4 * (iCBP == iMax);
  iCountZero += 1 - 4 * (iCBP == 0);

  if(iCountMax < -8)
    iCountMax = -8;
  else if(iCountMax > 7)
    iCountMax = 7;
  m_iCBPCountMax = iCountMax;

  if(iCountZero < -8)
    iCountZero = -8;
  else if(iCountZero > 7)
    iCountZero = 7;
  m_iCBPCountZero = iCountZero;

} // writeLPCBP()


ExcStatus CJPEGXREncoder::writeMacroblockLP()
{
  IppiVLCState_JPEGXR *pHuffState = m_pImagePlane->pHuffState;
  Ipp32u iModelBits = m_pImagePlane->pModelLP.iModelBits[0];
  Ipp32s iLaplasMean[2] = {0, 0};
  Ipp32s *pLaplasMean = iLaplasMean;
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u iNumCoeffs[JXR_MAX_CHANNELS];
  Ipp32s iRLCoeffs[JXR_MAX_CHANNELS][32];
  Ipp32s iResidual[JXR_MAX_CHANNELS][16];
  Ipp32u iYUV4XXBlocks = 8;
  Ipp32u i, j;

  Ipp8u *pStream;
  Ipp32u iBitBuf;
  Ipp32u iBitValid;

  if(m_bResetTotals)
    ippiVLCScanReset_JPEGXR(pHuffState, ippVLCScanRaster);

  if(iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)
  {
    Ipp32s* pDCLPTable[2] = {m_iDCLPTable[1], m_iDCLPTable[2]};
    Ipp32s* pResidual[2]  = {iResidual[1], iResidual[2]};;

    ippiVLCScan4x4DC_JPEGXR_32s(m_iDCLPTable[0], iModelBits, iResidual[0], iRLCoeffs[0], &iNumCoeffs[0], pHuffState);
    iModelBits = m_pImagePlane->pModelLP.iModelBits[1];
    VLCScanDC4XX_JPEGXR_32s(pDCLPTable, iColorFormat, iModelBits, pResidual, iRLCoeffs[1], &iNumCoeffs[1]);
    iModelBits = m_pImagePlane->pModelLP.iModelBits[0];

    writeLPCBP(2, m_pImagePlane->iCBPCountMax, m_pImagePlane->iCBPCountZero, iNumCoeffs, &m_BitStreamOut);
  }
  else
  {
    for (i = 0; i < iChannels; i++)
    {
      ippiVLCScan4x4DC_JPEGXR_32s(m_iDCLPTable[i], iModelBits, iResidual[i], iRLCoeffs[i], &iNumCoeffs[i], pHuffState);
      iModelBits = m_pImagePlane->pModelLP.iModelBits[1];
    }
    iModelBits = m_pImagePlane->pModelLP.iModelBits[0];

    if(iColorFormat == JXR_ICF_YUV444)
      writeLPCBP(iChannels, m_pImagePlane->iCBPCountMax, m_pImagePlane->iCBPCountZero, iNumCoeffs, &m_BitStreamOut);
    else
    {
      for(i = 0; i < iChannels; i++)
        m_BitStreamOut.WriteBits(1, (iNumCoeffs[i] > 0));
    }
  }

  for(i = 0; i < iChannels; i++)
  {
    Ipp32u iCoef = iNumCoeffs[i];

    if(iCoef)
    {
      *pLaplasMean += iCoef;
      m_BitStreamOut.GetStream(&pStream, &iBitBuf, &iBitValid);
      Ipp8u* savePtr = pStream;
      ippiVLCEncodeDC444_JPEGXR_32s1u(iRLCoeffs[i], iCoef, (IppBool)(i > 0), &pStream, &iBitBuf, &iBitValid, pHuffState);
      m_BitStreamOut.UpdateStream(pStream - savePtr, iBitBuf, iBitValid);
    }

    if(iModelBits)
    {
      for (j = 1; j < 16; j++)
        m_BitStreamOut.WriteBits((Ipp8u)(iModelBits + (iResidual[i][j] & 1)), iResidual[i][j] >> 1);
    }
    pLaplasMean = iLaplasMean + 1;
    iModelBits = m_pImagePlane->pModelLP.iModelBits[1];

    if(iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)
    {
      iCoef = iNumCoeffs[1];

      if(iCoef)
      {
        *pLaplasMean += iCoef;
        m_BitStreamOut.GetStream(&pStream, &iBitBuf, &iBitValid);
        Ipp8u* savePtr = pStream;
        if(iColorFormat == JXR_ICF_YUV422)
        {
          iYUV4XXBlocks = 8;
          ippiVLCEncodeDC422_JPEGXR_32s1u(iRLCoeffs[1], iCoef, &pStream, &iBitBuf, &iBitValid, pHuffState);
        }
        else
        {
          iYUV4XXBlocks = 4;
          ippiVLCEncodeDC420_JPEGXR_32s1u(iRLCoeffs[1], iCoef, &pStream, &iBitBuf, &iBitValid, pHuffState);
        }
        m_BitStreamOut.UpdateStream(pStream - savePtr, iBitBuf, iBitValid);
      }

      if (iModelBits)
      {
        for(i = 1; i < iYUV4XXBlocks; i++)
        {
          m_BitStreamOut.FlushBits();
          m_BitStreamOut.WriteBits((Ipp8u)iModelBits, ABS(m_iDCLPTable[1][i]));
          if(iResidual[1][i] == 0 && m_iDCLPTable[1][i])
            m_BitStreamOut.WriteBits(1, (m_iDCLPTable[1][i] < 0));

          m_BitStreamOut.WriteBits((Ipp8u)iModelBits, ABS(m_iDCLPTable[2][i]));
          if(iResidual[2][i] == 0 && m_iDCLPTable[2][i])
            m_BitStreamOut.WriteBits(1, (m_iDCLPTable[2][i] < 0));
        }
      }
      break;
    }
  }

  updateModel(iLaplasMean, iColorFormat, iChannels, &m_pImagePlane->pModelLP);

  if(m_bResetContext)
    ippiVLCAdapt_JPEGXR(m_pImagePlane->pHuffState, ippVLCAdaptLowpass);

  return ExcStatusOk;
} // CJPEGXREncoder::writeMacroblockLP()


static void encodeHPCBP(AdaptiveHuffman* pAdaptCBP, AdaptiveHuffman* pAdaptBlockCBP,
                      Ipp16u* pCBP, CBitStreamOutput* pStream, Ipp32u iColorFormat, Ipp32u iChannels)
{
  Ipp32s iCBPY = 0, iCBPU = 0, iCBPV = 0, iTemp;
  Ipp32s iCodeY, iCodeU = 0, iCodeV = 0;
  Ipp32s iCount, iPattern = 0, iChroma;
  Ipp32u bYUV = 0;
  Ipp32u i, j;

  if(iColorFormat == JXR_ICF_YUV444)
  {
    bYUV = 1;
    iCBPY = iTemp = pCBP[0];
    iCBPU = pCBP[1];
    iCBPV = pCBP[2];
    iTemp |= (iCBPU | iCBPV);
    iPattern = 0;

    for (j = 0; j < 4; j++)
    {
      iPattern |= ((iTemp & 0xf) != 0) * 0x10;
      iTemp >>= 4;
      iPattern >>= 1;
    }
  }
  else if(iColorFormat == JXR_ICF_YUV422)
  {
    bYUV = 1;
    iCBPY = pCBP[0];
    iCBPU = pCBP[1];
    iCBPV = pCBP[2];
    iCBPY = (iCBPY & 0xf) + ((iCBPU & 1) << 4) + ((iCBPU & 4) << 3) + ((iCBPV & 1) << 6) + ((iCBPV & 4) << 5) +
      ((iCBPY & 0x00f0) << 4) + ((iCBPU & 2) << 11) + ((iCBPU & 8) << 10) + ((iCBPV & 2) << 13) + ((iCBPV & 8) <<  12) +
      ((iCBPY & 0x0f00) << 8) + ((iCBPU & 16) << 16) + ((iCBPU & 64) << 15) + ((iCBPV & 16) << 18) + ((iCBPV & 64) <<  17) +
      ((iCBPY & 0xf000) << 12) + ((iCBPU & 32) << 23) + ((iCBPU & 128) << 22) + ((iCBPV & 32) << 25) + ((iCBPV & 128) <<  24);
    iTemp = iCBPY;
    iPattern = 0;

    for(j = 0; j < 4; j++)
    {
      iPattern |= ((iTemp & 0xff) != 0) * 0x10;
      iTemp >>= 8;
      iPattern >>= 1;
    }
  }
  else if(iColorFormat == JXR_ICF_YUV420)
  {
    bYUV = 1;
    iCBPY = pCBP[0];
    iCBPU = pCBP[1];
    iCBPV = pCBP[2];
    iCBPY = (iCBPY & 0xf) + ((iCBPU & 1) <<  4) + ((iCBPV & 1) <<  5) +
        ((iCBPY & 0x00f0) << 2) + ((iCBPU & 2) <<  9) + ((iCBPV & 2) << 10) +
        ((iCBPY & 0x0f00) << 4) + ((iCBPU & 4) << 14) + ((iCBPV & 4) << 15) +
        ((iCBPY & 0xf000) << 6) + ((iCBPU & 8) << 19) + ((iCBPV & 8) << 20);
    iTemp = iCBPY;
    iPattern = 0;

    for(j = 0; j < 4; j++)
    {
      iPattern |= ((iTemp & 0x3f) != 0) * 0x10;
      iTemp >>= 6;
      iPattern >>= 1;
    }
  }

  for(i = 0; i < iChannels; i++)
  {
    if(!bYUV)
    {
      iCBPY = iTemp = pCBP[i];
      iPattern = 0;

      for (j = 0; j < 4; j++)
      {
        iPattern |= ((iTemp & 0xf) != 0) * 0x10;
        iTemp >>= 4;
        iPattern >>= 1;
      }
    }

    iCount = iUnitCount[iPattern];
    pStream->WriteBits((Ipp8u)pAdaptCBP->pTable[iCount * 2 + 2], pAdaptCBP->pTable[iCount * 2 + 1]);
    pAdaptCBP->iDiscr += pAdaptCBP->pDelta[iCount];
    if(iCBPLenghtTable1[iPattern])
      pStream->WriteBits(iCBPLenghtTable1[iPattern], iCBPCodeTable1[iPattern]);

    for(j = 0; j < 4; j++)
    {
      if(iColorFormat == JXR_ICF_YUV444)
      {
        iCodeY = iCBPY & 0xf;
        iCodeU = iCBPU & 0xf;
        iCodeV = iCBPV & 0xf;
        iCodeY |= ((iCodeU != 0) << 4);
        iCodeY |= ((iCodeV != 0) << 5);
        iCBPY >>= 4;
        iCBPU >>= 4;
        iCBPV >>= 4;
      }
      else if(iColorFormat == JXR_ICF_YUV422)
      {
        iCodeY = iCBPY & 0xff;
        iCBPY >>= 8;
      }
      else if(iColorFormat == JXR_ICF_YUV420)
      {
        iCodeY = iCBPY & 0x3f;
        iCBPY >>= 6;
      }
      else
      {
        iCodeY = iCBPY & 0xf;
        iCBPY >>= 4;
      }

      if(iCodeY)
      {
        iChroma = (iCodeY >> 4);
        iCodeY &= 0xf;

        if(iColorFormat == JXR_ICF_YUV422)
        {
          iCodeU = iChroma & 3;
          iCodeV = (iChroma >> 2) & 3;
          iChroma = (iCodeU == 0)?0:1;
          if(iCodeV)
            iChroma += 2;
        }

        if(iChroma)
        {
          if(iCBPValueTable[iCodeY] > 2)
            iTemp = 8;
          else
            iTemp = iCBPValueTable[iCodeY] + 6 - 1;

          pStream->WriteBits((Ipp8u)pAdaptBlockCBP->pTable[iTemp * 2 + 2], pAdaptBlockCBP->pTable[iTemp * 2 + 1]);
          pAdaptBlockCBP->iDiscr += pAdaptBlockCBP->pDelta[iTemp];

          if(iChroma == 1)
            pStream->WriteBits(1, 1);
          else
            pStream->WriteBits(2, 3 - iChroma);

          if(iTemp == 8)
          {
            if(iCBPValueTable[iCodeY] == 3)
              pStream->WriteBits(1, 1);
            else
              pStream->WriteBits(2, 5 - iCBPValueTable[iCodeY]);
          }
          if(iCBPLenghtTable2[iCodeY])
            pStream->WriteBits(iCBPLenghtTable2[iCodeY], iCBPCodeTable2[iCodeY]);
        }
        else
        {
          iTemp = iCBPValueTable[iCodeY] - 1;
          pStream->WriteBits((Ipp8u)pAdaptBlockCBP->pTable[iTemp * 2 + 2], pAdaptBlockCBP->pTable[iTemp * 2 + 1]);
          pAdaptBlockCBP->iDiscr += pAdaptBlockCBP->pDelta[iTemp];

          if(iTemp == 8)
          {
            if(iCBPValueTable[iCodeY] == 3)
              pStream->WriteBits(1, 1);
            else
              pStream->WriteBits(2, 5 - iCBPValueTable[iCodeY]);
          }
          if(iCBPLenghtTable2[iCodeY])
            pStream->WriteBits(iCBPLenghtTable2[iCodeY], iCBPCodeTable2[iCodeY]);
        }

        if(iCodeU)
        {
          if(iColorFormat == JXR_ICF_YUV444)
          {
            iCount = iUnitCount[iCodeU] - 1;
            pStream->WriteBits(pVLCCodeCBPCh[iCount * 2 + 2], pVLCCodeCBPCh[iCount * 2 + 1]);
            if(iCBPLenghtTable1[iCodeU])
              pStream->WriteBits(iCBPLenghtTable1[iCodeU], iCBPCodeTable1[iCodeU]);
          }
          else
          {
            if(iCodeU == 1)
              pStream->WriteBits(1, 1);
            else
              pStream->WriteBits(2, 3 - iCodeU);
          }
        }

        if(iCodeV)
        {
          if(iColorFormat == JXR_ICF_YUV444)
          {
            iCount = iUnitCount[iCodeV] - 1;
            pStream->WriteBits(pVLCCodeCBPCh[iCount * 2 + 2], pVLCCodeCBPCh[iCount * 2 + 1]);
            if(iCBPLenghtTable1[iCodeV])
              pStream->WriteBits(iCBPLenghtTable1[iCodeV], iCBPCodeTable1[iCodeV]);
          }
          else
          {
            if(iCodeV == 1)
              pStream->WriteBits(1, 1);
            else
              pStream->WriteBits(2, 3 - iCodeV);
          }
        }
      }
    }
    if(bYUV) break;
  }
} // encodeHPCBP()


static void predHPCBP(Ipp16u* pCBP, Ipp16u* pPredCBP, Ipp32u* pCount, Ipp32u iMBColumn, Ipp32u iBorder, Ipp32u iChannels,
                      Ipp32u iColorFormat, CBPModel *pModel, PredictorInf **pPred, PredictorInf **pPredPrev)
{
  Ipp32u bLeft = (iBorder & JXR_BORDER_LEFT)?1:0;
  Ipp32u bTop = (iBorder & JXR_BORDER_TOP)?1:0;
  Ipp32u bUV42X = (iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)?1:0;
  Ipp32s iPredCBP = 0, iCount;
  Ipp32u bChroma = 0;
  Ipp32u i;

  for(i = 0; i < iChannels; i++)
  {
    iCount = pCount[i];

    if(i == 0 || !bUV42X)
    {
      if(pModel->iState[bChroma] == 0)
      {
        if(bLeft)
        {
          if(bTop)
            iPredCBP = 1;
          else
            iPredCBP = (pPredPrev[i][iMBColumn].iCBP >> 10) & 1;
        }
        else
          iPredCBP = (pPred[i][iMBColumn - 1].iCBP >> 5) & 1;

        iPredCBP |= (pCBP[i] & 0x11) << 1;
        iPredCBP |= (pCBP[i] & 0x3333) << 2;
        iPredCBP |= (pCBP[i] & 0x2) << 3;
        iPredCBP |= (pCBP[i] & 0xcc) << 6;
        iPredCBP ^= pCBP[i];
      }
      else if(pModel->iState[bChroma] == 1)
        iPredCBP = pCBP[i];
      else
        iPredCBP = pCBP[i] ^ 0xffff;
    }
    else
    {
      if(iColorFormat == JXR_ICF_YUV422)
      {
        iCount = pCount[i] * 2;

        if(pModel->iState[1] == 0)
        {
          if(bLeft)
          {
            if(bTop)
              iPredCBP = 1;
            else
              iPredCBP = (pPredPrev[i][iMBColumn].iCBP >> 6) & 1;
          }
          else
            iPredCBP = ((pPred[i][iMBColumn - 1].iCBP >> 1) & 1);

          iPredCBP |= (pCBP[i] & 0x1)  << 1;
          iPredCBP |= (pCBP[i] & 0x3f) << 2;
          iPredCBP ^= pCBP[i];
        }
        else if(pModel->iState[1] == 1)
          iPredCBP = pCBP[i];
        else
          iPredCBP = pCBP[i]^0xff;
      }
      else if(iColorFormat == JXR_ICF_YUV420)
      {
        iCount = pCount[i] * 4;

        if(pModel->iState[1] == 0)
        {
          if(bLeft)
          {
            if(bTop)
              iPredCBP = 1;
            else
              iPredCBP = (pPredPrev[i][iMBColumn].iCBP >> 2) & 1;
          }
          else
            iPredCBP = ((pPred[i][iMBColumn - 1].iCBP >> 1) & 1);

          iPredCBP |= (pCBP[i] & 0x1) << 1;
          iPredCBP |= (pCBP[i] & 0x3) << 2;
          iPredCBP ^= pCBP[i];
        }
        else if(pModel->iState[1] == 1)
          iPredCBP = pCBP[i];
        else
          iPredCBP = pCBP[i]^0xf;
      }
    }

    pPredCBP[i] = (Ipp16u)iPredCBP;
    updateModelCBP(iCount, bChroma, pModel);
    bChroma = 1;
  }
} // predHPCBP()


ExcStatus CJPEGXREncoder::writeMacroblockHP()
{
  IppiVLCScanType tScan = (m_iOrientation == 1)? ippVLCScanVert : ippVLCScanHoriz;
  const Ipp8u *pCBlocks = iBlocks444;
  const Ipp8u *pTBlocks = iBlocks444;
  Ipp32u iCBlock = 16, iTBlock = 16;
  Ipp32s iLaplasMean[2] = {0,0};
  Ipp32s *pLaplasMean = iLaplasMean;
  Ipp32s iResidual[JXR_MAX_CHANNELS];
  Ipp32s iTrim = m_iTrimFlex;
  Ipp32s iModelBits = m_pImagePlane->pModelHP.iModelBits[0];
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u iBands = m_pImagePlane->iBands;
  Ipp32u iACHOffset = (m_bAlphaProcess)?m_iACHOffset:0;
  CBPModel *pCBPModel = &m_pImagePlane->pCBPModel;
  IppiVLCState_JPEGXR *pHuffState = m_pImagePlane->pHuffState;
  Ipp32u  iNonZero;
  IppBool iChroma = ippFalse;
  Ipp16u  iPredCBP[JXR_MAX_CHANNELS];
  Ipp32u  iCount[JXR_MAX_CHANNELS];
  Ipp32u  i, j;

  Ipp8u *pStream;
  Ipp32u iBitBuf;
  Ipp32u iBitValid;

  if(m_bResetTotals)
  {
    ippiVLCScanReset_JPEGXR(pHuffState, ippVLCScanHoriz);
    ippiVLCScanReset_JPEGXR(pHuffState, ippVLCScanVert);
  }
  ippiVLCScanSet_JPEGXR(pHuffState, tScan);

  if(iColorFormat == JXR_ICF_YUV420)
  {
    iCBlock = 4;
    pCBlocks = iBlocks420;
  }
  else if(iColorFormat == JXR_ICF_YUV422)
  {
    iCBlock = 8;
    pCBlocks = iBlocks422;
  }

  for(i = 0; i < iChannels; i++)
  {
    Ipp32u iStep = m_pMBTab[i + iACHOffset].iStepMB4;
    Ipp32u iLBound = (1 << iModelBits) - 1;
    Ipp32u iHBound = iLBound * 2 + 1;
    Ipp32u iCBP = 0, iBit = 1;
    iCount[i] = 0;

    if(m_iCUSize == 2)
    {
      for(j = 0; j < iTBlock; j++, iBit <<= 1)
      {
        Ipp16s *pSrc0 = _PEL(Ipp16s, m_pMBTab[i + iACHOffset].pMB4, pTBlocks[2*j], pTBlocks[2*j + 1], iStep);
        Ipp16s *pSrc1 = _NEXTROW(Ipp16s, pSrc0, iStep);
        Ipp16s *pSrc2 = _NEXTROW(Ipp16s, pSrc1, iStep);
        Ipp16s *pSrc3 = _NEXTROW(Ipp16s, pSrc2, iStep);
        if((pSrc0[ 1] + iLBound >= iHBound) || (pSrc0[ 2] + iLBound >= iHBound) ||
          (pSrc0[ 3] + iLBound >= iHBound) || (pSrc1[ 0] + iLBound >= iHBound) ||
          (pSrc1[ 1] + iLBound >= iHBound) || (pSrc1[ 2] + iLBound >= iHBound) ||
          (pSrc1[ 3] + iLBound >= iHBound) || (pSrc2[ 0] + iLBound >= iHBound) ||
          (pSrc2[ 1] + iLBound >= iHBound) || (pSrc2[ 2] + iLBound >= iHBound) ||
          (pSrc2[ 3] + iLBound >= iHBound) || (pSrc3[ 0] + iLBound >= iHBound) ||
          (pSrc3[ 1] + iLBound >= iHBound) || (pSrc3[ 2] + iLBound >= iHBound) ||
          (pSrc3[ 3] + iLBound >= iHBound))
        {
          iCBP |= iBit, iCount[i]++;
        }
      }
    }
    else
    {
      for(j = 0; j < iTBlock; j++, iBit <<= 1)
      {
        Ipp32s *pSrc0 = _PEL(Ipp32s, m_pMBTab[i + iACHOffset].pMB4, pTBlocks[2*j], pTBlocks[2*j + 1], iStep);
        Ipp32s *pSrc1 = _NEXTROW(Ipp32s, pSrc0, iStep);
        Ipp32s *pSrc2 = _NEXTROW(Ipp32s, pSrc1, iStep);
        Ipp32s *pSrc3 = _NEXTROW(Ipp32s, pSrc2, iStep);
        if((pSrc0[ 1] + iLBound >= iHBound) || (pSrc0[ 2] + iLBound >= iHBound) ||
          (pSrc0[ 3] + iLBound >= iHBound) || (pSrc1[ 0] + iLBound >= iHBound) ||
          (pSrc1[ 1] + iLBound >= iHBound) || (pSrc1[ 2] + iLBound >= iHBound) ||
          (pSrc1[ 3] + iLBound >= iHBound) || (pSrc2[ 0] + iLBound >= iHBound) ||
          (pSrc2[ 1] + iLBound >= iHBound) || (pSrc2[ 2] + iLBound >= iHBound) ||
          (pSrc2[ 3] + iLBound >= iHBound) || (pSrc3[ 0] + iLBound >= iHBound) ||
          (pSrc3[ 1] + iLBound >= iHBound) || (pSrc3[ 2] + iLBound >= iHBound) ||
          (pSrc3[ 3] + iLBound >= iHBound))
        {
          iCBP |= iBit, iCount[i]++;
        }
      }
    }
    (m_pPred[i + iACHOffset] + m_iMBColumn - 1)->iCBP = iCBP;
    m_iCBP[i] = (Ipp16u)iCBP;
    iModelBits = m_pImagePlane->pModelHP.iModelBits[1];
    iTBlock = iCBlock;
    pTBlocks = pCBlocks;
  }
  iModelBits = m_pImagePlane->pModelHP.iModelBits[0];

  predHPCBP(m_iCBP, iPredCBP, iCount, m_iMBColumn - 1, m_iBorder, iChannels, iColorFormat,
    pCBPModel, m_pPred + iACHOffset, m_pPredPrev + iACHOffset);
  encodeHPCBP(&m_pImagePlane->pAdaptCBP, &m_pImagePlane->pAdaptBlockCBP, iPredCBP, &m_BitStreamOut, iColorFormat, iChannels);

  m_BitStreamOut.FlushBuffer();
  m_BitStreamOut.GetStream(&pStream, &iBitBuf, &iBitValid);
  Ipp8u *savePtr = pStream;
  pTBlocks = iBlocks444;
  iTBlock  = 16;

  for(i = 0; i < iChannels; i++)
  {
    Ipp32s iFlex = 0;
    Ipp32s iPattern = m_iCBP[i];
    Ipp32u iStep = m_pMBTab[i + iACHOffset].iStepMB4;
    if(iTrim < iModelBits && iBands < JXR_SB_NOFLEX)
      iFlex = iModelBits - iTrim;

    if(m_iCUSize == 2)
    {
      for (j = 0; j < iTBlock; j++, iPattern >>= 1, pTBlocks += 2)
      {
        Ipp16s *pSrc = _PEL(Ipp16s, m_pMBTab[i + iACHOffset].pMB4, pTBlocks[0], pTBlocks[1], iStep);

        if(iPattern & 1)
        {
          if(iFlex > 0)
            ippiVLCEncode4x4Flex_JPEGXR_16s1u_C1R(pSrc, iStep, iChroma, iModelBits, iTrim, iResidual, &pStream, &iBitBuf, &iBitValid, &iNonZero, pHuffState);
          else
            ippiVLCEncode4x4_JPEGXR_16s1u_C1R(pSrc, iStep, iChroma, iModelBits, &pStream, &iBitBuf, &iBitValid, &iNonZero, pHuffState);
          *pLaplasMean += iNonZero;
        }
        if(iFlex > 0)
          ippiFLCEncode4x4_JPEGXR_16s1u_C1R(pSrc, iStep, iResidual, &pStream, &iBitBuf, &iBitValid, iFlex, iTrim, (IppBool)(iPattern & 1));
      }
    }
    else
    {
      for (j = 0; j < iTBlock; j++, iPattern >>= 1, pTBlocks += 2)
      {
        Ipp32s *pSrc = _PEL(Ipp32s, m_pMBTab[i + iACHOffset].pMB4, pTBlocks[0], pTBlocks[1], iStep);

        if(iPattern & 1)
        {
          if(iFlex > 0)
            ippiVLCEncode4x4Flex_JPEGXR_32s1u_C1R(pSrc, iStep, iChroma, iModelBits, iTrim, iResidual, &pStream, &iBitBuf, &iBitValid, &iNonZero, pHuffState);
          else
            ippiVLCEncode4x4_JPEGXR_32s1u_C1R(pSrc, iStep, iChroma, iModelBits, &pStream, &iBitBuf, &iBitValid, &iNonZero, pHuffState);
          *pLaplasMean += iNonZero;
        }
        if(iFlex > 0)
          ippiFLCEncode4x4_JPEGXR_32s1u_C1R(pSrc, iStep, iResidual, &pStream, &iBitBuf, &iBitValid, iFlex, iTrim, (IppBool)(iPattern & 1));
      }
    }

    iChroma = ippTrue;
    iModelBits = m_pImagePlane->pModelHP.iModelBits[1];
    pLaplasMean = iLaplasMean + 1;
    iTBlock = iCBlock;
    pTBlocks = pCBlocks;
  }
  m_BitStreamOut.UpdateStream((pStream - savePtr), iBitBuf, iBitValid);

  updateModel(iLaplasMean, iColorFormat, iChannels, &m_pImagePlane->pModelHP);

  if(m_bResetContext)
  {
    adaptHuffman(&m_pImagePlane->pAdaptCBP);
    adaptHuffman(&m_pImagePlane->pAdaptBlockCBP);
    ippiVLCAdapt_JPEGXR(pHuffState, ippVLCAdaptHighpass);
  }

  return ExcStatusOk;
} // CJPEGXREncoder::writeMacroblockHP()


template<class TYPE>
static void mapDCLP(MBTab* pMBTab, Ipp32s *pDC, Ipp32u iMode)
{
  TYPE *pSrc = (TYPE*)pMBTab->pMB4;
  Ipp32u iSrcStep = pMBTab->iStepMB4;
  Ipp32u i;

  if(iMode == 0)
  {
    TYPE* pSrc4  = _NEARROW(TYPE, pSrc, 4, iSrcStep);
    TYPE* pSrc8  = _NEARROW(TYPE, pSrc4, 4, iSrcStep);
    TYPE* pSrc12 = _NEARROW(TYPE, pSrc8, 4, iSrcStep);
    pDC[ 0] = pSrc  [ 0];
    pDC[ 1] = pSrc  [ 8];
    pDC[ 2] = pSrc  [ 4];
    pDC[ 3] = pSrc4 [12];
    pDC[ 4] = pSrc8 [ 0];
    pDC[ 5] = pSrc12[12];
    pDC[ 6] = pSrc12[ 0];
    pDC[ 7] = pSrc8 [12];
    pDC[ 8] = pSrc4 [ 0];
    pDC[ 9] = pSrc  [12];
    pDC[10] = pSrc4 [ 4];
    pDC[11] = pSrc4 [ 8];
    pDC[12] = pSrc12[ 4];
    pDC[13] = pSrc12[ 8];
    pDC[14] = pSrc8 [ 4];
    pDC[15] = pSrc8 [ 8];
  }
  else if(iMode == 1)
  {
    for(i = 0; i < 8; i+=2)
    {
      pDC[i] = pSrc[0];
      pDC[i+1] = pSrc[4];
      pSrc = _NEARROW(TYPE, pSrc, 4, iSrcStep);
    }
  }
  else
  {
    for(i = 0; i < 4; i+=2)
    {
      pDC[i] = pSrc[0];
      pDC[i+1] = pSrc[4];
      pSrc = _NEARROW(TYPE, pSrc, 4, iSrcStep);
    }
  }
}

ExcStatus CJPEGXREncoder::quantMacroblock()
{
  Ipp32u iCMode = 0;
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u iACHOffset = (m_bAlphaProcess)?m_iACHOffset:0;
  QuantInf *pQPDC;
  QuantInf *pQPLP;
  QuantInf *pQPHP;
  IppiJPEGXRQuantizer_32u pQuant[3];
  Ipp32u i;

  if(iColorFormat == JXR_ICF_YUV422)
    iCMode = 1;
  else if(iColorFormat == JXR_ICF_YUV420)
    iCMode = 2;

  if(m_iCUSize == 2)
  {
    for(i = 0; i < iChannels; i++)
    {
      pQPDC = m_pQuantDC[i + iACHOffset];
      pQPLP = m_pQuantLP[i + iACHOffset] + m_iLPQuantizerIndex;
      pQPHP = m_pQuantHP[i + iACHOffset] + m_iHPQuantizerIndex;
      pQuant[0].mantissa = pQPDC->iMan, pQuant[0].exponent = pQPDC->iExp, pQuant[0].offset = pQPDC->iOffset;
      pQuant[1].mantissa = pQPLP->iMan, pQuant[1].exponent = pQPLP->iExp, pQuant[1].offset = pQPLP->iOffset;
      pQuant[2].mantissa = pQPHP->iMan, pQuant[2].exponent = pQPHP->iExp, pQuant[2].offset = pQPHP->iOffset;

      if(iColorFormat == JXR_ICF_YUV420)
        ippiQuant8x8_HDP_16s_C1IR((Ipp16s*)m_pMBTab[i + iACHOffset].pMB4, 64, pQuant, 2);
      else if(iColorFormat == JXR_ICF_YUV422)
        ippiQuant8x16_HDP_16s_C1IR((Ipp16s*)m_pMBTab[i + iACHOffset].pMB4, 64, pQuant, 2);
      else
        ippiQuant16x16_HDP_16s_C1IR((Ipp16s*)m_pMBTab[i + iACHOffset].pMB4, 64, pQuant, 2);

      mapDCLP<Ipp16s>(&m_pMBTab[i + iACHOffset], m_iDCLPTable[i], (i)?iCMode:0);
    }
  }
  else
  {
    for(i = 0; i < iChannels; i++)
    {
      pQPDC = m_pQuantDC[i + iACHOffset];
      pQPLP = m_pQuantLP[i + iACHOffset] + m_iLPQuantizerIndex;
      pQPHP = m_pQuantHP[i + iACHOffset] + m_iHPQuantizerIndex;
      pQuant[0].mantissa = pQPDC->iMan, pQuant[0].exponent = pQPDC->iExp, pQuant[0].offset = pQPDC->iOffset;
      pQuant[1].mantissa = pQPLP->iMan, pQuant[1].exponent = pQPLP->iExp, pQuant[1].offset = pQPLP->iOffset;
      pQuant[2].mantissa = pQPHP->iMan, pQuant[2].exponent = pQPHP->iExp, pQuant[2].offset = pQPHP->iOffset;

      if(iColorFormat == JXR_ICF_YUV420)
        ippiQuant8x8_HDP_32s_C1IR(m_pMBTab[i + iACHOffset].pMB4, 128, pQuant, 2);
      else if(iColorFormat == JXR_ICF_YUV422)
        ippiQuant8x16_HDP_32s_C1IR(m_pMBTab[i + iACHOffset].pMB4, 128, pQuant, 2);
      else
        ippiQuant16x16_HDP_32s_C1IR(m_pMBTab[i + iACHOffset].pMB4, 128, pQuant, 2);

      mapDCLP<Ipp32s>(&m_pMBTab[i + iACHOffset], m_iDCLPTable[i], iCMode);
    }
  }

  return ExcStatusOk;
} // CJPEGXREncoder::quantMacroblock()


/***********************************/
// Prediction functions
/***********************************/

template<class TYPE>
static void predHP(Ipp32u iChannels, Ipp32u iColorFormat, Ipp32u iPredMode, MBTab* pMBTab)
{
  Ipp32u iCWidth = 4, iCHeight = 4;
  Ipp32u iWidth = 4, iHeight = 4;
  TYPE *p1, *p2, *p3, *pBlock1, *pBlock2, *pBlock3;
  TYPE *pp1, *pp2, *pp3;
  Ipp32u i, j, k;

  if(iColorFormat == JXR_ICF_YUV422)
  {
    iCWidth = 2;
  }
  else if(iColorFormat == JXR_ICF_YUV420)
  {
    iCWidth = 2;
    iCHeight = 2;
  }

  for(i = 0; i < iChannels; i++)
  {
    Ipp32u iStepMB = pMBTab[i].iStepMB4;

    if(iPredMode == 1)
    {
      pBlock1 = _PEL(TYPE, pMBTab[i].pMB4, 0, 4*iHeight - 3, iStepMB);
      pBlock2 = _NEXTROW(TYPE, pBlock1, iStepMB);
      pBlock3 = _NEXTROW(TYPE, pBlock2, iStepMB);

      for(k = 0; k < iWidth; k++)
      {
        p1 = pBlock1, p2 = pBlock2, p3 = pBlock3;

        for(j = 1; j < iHeight; j++)
        {
          pp1 = _NEARROW(TYPE, p1, -4, iStepMB);
          pp2 = _NEXTROW(TYPE, pp1, iStepMB);
          pp3 = _NEXTROW(TYPE, pp2, iStepMB);
          p2[0] -= pp2[0];
          p1[0] -= pp1[0];
          p3[1] -= pp3[1];
          p1 = pp1, p2 = pp2, p3 = pp3;
        }
        pBlock1 += 4 , pBlock2 += 4, pBlock3 += 4;
      }
    }
    else if(iPredMode == 0)
    {
      pBlock1 = _PEL(TYPE, pMBTab[i].pMB4, 4*iWidth - 4, 0, iStepMB);
      pBlock2 = _NEXTROW(TYPE, pBlock1, iStepMB);

      for(k = 0; k < iHeight; k++)
      {
        p1 = pBlock1, p2 = pBlock2;
        for(j = 1; j < iWidth; j++)
        {
          pp1 = p1 - 4, pp2 = p2 - 4;
          p1[1] -= pp1[1];
          p1[2] -= pp1[2];
          p2[3] -= pp2[3];
          p1 = pp1, p2 = pp2;
        }
        pBlock1 = _NEARROW(TYPE, pBlock1, 4, iStepMB);
        pBlock2 = _NEXTROW(TYPE, pBlock1, iStepMB);
      }
    }
    iWidth = iCWidth, iHeight = iCHeight;
  }
} // predHP()


ExcStatus CJPEGXREncoder::predMacroblock()
{
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32s iDCPredMode, iLPPredMode = 2;
  Ipp32u bLeft = (m_iBorder & JXR_BORDER_LEFT)?1:0;
  Ipp32u bTop = (m_iBorder & JXR_BORDER_TOP)?1:0;
  Ipp32s iHor, iVer;
  Ipp32s iScale = 2;
  PredictorInf *pPred;
  Ipp32s *pOrg, *pRef;
  Ipp32u bUV42X = (iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)?1:0;
  Ipp32u iACHOffset = (m_bAlphaProcess)?m_iACHOffset:0;
  Ipp32u i;

  // Pred mode DCLP
  if(bLeft && bTop)
    iDCPredMode = 3;
  else if(bLeft)
    iDCPredMode = 1;
  else if(bTop)
    iDCPredMode = 0;
  else
  {
    if(iColorFormat == JXR_ICF_Y || iColorFormat == JXR_ICF_NCH)
    {
      iHor = ABS(m_pPredPrev[iACHOffset][m_iMBColumn - 2].iDC - m_pPred[iACHOffset][m_iMBColumn - 2].iDC);
      iVer = ABS(m_pPredPrev[iACHOffset][m_iMBColumn - 2].iDC - m_pPredPrev[iACHOffset][m_iMBColumn - 1].iDC);
    }
    else
    {
      if(iColorFormat == JXR_ICF_YUV422)
        iScale = 4;
      else if(iColorFormat == JXR_ICF_YUV420)
        iScale = 8;

      iHor =
        ABS(m_pPredPrev[iACHOffset][m_iMBColumn - 2].iDC - m_pPred[iACHOffset][m_iMBColumn - 2].iDC) * iScale +
        ABS(m_pPredPrev[1 + iACHOffset][m_iMBColumn - 2].iDC - m_pPred[1 + iACHOffset][m_iMBColumn - 2].iDC) +
        ABS(m_pPredPrev[2 + iACHOffset][m_iMBColumn - 2].iDC - m_pPred[2 + iACHOffset][m_iMBColumn - 2].iDC);
      iVer =
        ABS(m_pPredPrev[iACHOffset][m_iMBColumn - 2].iDC - m_pPredPrev[iACHOffset][m_iMBColumn - 1].iDC) * iScale +
        ABS(m_pPredPrev[1 + iACHOffset][m_iMBColumn - 2].iDC - m_pPredPrev[1 + iACHOffset][m_iMBColumn - 1].iDC) +
        ABS(m_pPredPrev[2 + iACHOffset][m_iMBColumn - 2].iDC - m_pPredPrev[2 + iACHOffset][m_iMBColumn - 1].iDC);
    }
    iDCPredMode = (iHor * 4 < iVer ? 1 : (iVer * 4 < iHor ? 0 : 2));
  }

  if(iDCPredMode == 1 && m_iLPQuantizerIndex == (Ipp32u)m_pPredPrev[iACHOffset][m_iMBColumn - 1].iQPIndex)
    iLPPredMode = 1;
  if(iDCPredMode == 0 && m_iLPQuantizerIndex == (Ipp32u)m_pPred[iACHOffset][m_iMBColumn - 2].iQPIndex)
    iLPPredMode = 0;

  // Pred mode HP
  iHor = ABS(m_iDCLPTable[0][1]) + ABS(m_iDCLPTable[0][2]) + ABS(m_iDCLPTable[0][3]);
  iVer = ABS(m_iDCLPTable[0][4]) + ABS(m_iDCLPTable[0][8]) + ABS(m_iDCLPTable[0][12]);

  if(iColorFormat != JXR_ICF_Y && iColorFormat != JXR_ICF_NCH)
  {
    iHor += ABS(m_iDCLPTable[1][1]) + ABS(m_iDCLPTable[2][1]);
    if(iColorFormat == JXR_ICF_YUV420)
      iVer += ABS(m_iDCLPTable[1][2]) + ABS(m_iDCLPTable[2][2]);
    else if (iColorFormat == JXR_ICF_YUV422)
    {
      iVer += ABS(m_iDCLPTable[1][2]) + ABS(m_iDCLPTable[2][2]) + ABS(m_iDCLPTable[1][6]) + ABS(m_iDCLPTable[2][6]);
      iHor += ABS(m_iDCLPTable[1][5]) + ABS(m_iDCLPTable[2][5]);
    }
    else
      iVer += ABS(m_iDCLPTable[1][4]) + ABS(m_iDCLPTable[2][4]);
  }
  m_iOrientation = 2 - (iHor * 4 < iVer ? 1 : (iVer * 4 < iHor ? 0 : 2));

  // Pred update
  for(i = 0; i < iChannels; i++)
  {
    m_pPred[i + iACHOffset][m_iMBColumn - 1].iDC = m_iDCLPTable[i][0];
    m_pPred[i + iACHOffset][m_iMBColumn - 1].iQPIndex = m_iLPQuantizerIndex;
  }

  for(i = 0; i < iChannels; i++)
  {
    pPred = &m_pPred[i + iACHOffset][m_iMBColumn - 1];
    pOrg = m_iDCLPTable[i];

    if(i == 0 || !bUV42X)
    {
      pPred->iLP[0] = pOrg[1];
      pPred->iLP[1] = pOrg[2];
      pPred->iLP[2] = pOrg[3];
      pPred->iLP[3] = pOrg[4];
      pPred->iLP[4] = pOrg[8];
      pPred->iLP[5] = pOrg[12];
    }
    else
    {
      if(iColorFormat == JXR_ICF_YUV420)
      {
        pPred->iLP[0] = pOrg[1];
        pPred->iLP[1] = pOrg[2];
      }
      else if(iColorFormat == JXR_ICF_YUV422)
      {
        pPred->iLP[0] = pOrg[1];
        pPred->iLP[1] = pOrg[2];
        pPred->iLP[2] = pOrg[5];
        pPred->iLP[3] = pOrg[6];
        pPred->iLP[4] = pOrg[4];
      }
    }
  }

  // Predict DC
  for(i = 0; i < iChannels; i ++)
  {
    pOrg = m_iDCLPTable[i];

    if(iDCPredMode == 1)
      pOrg[0] -= m_pPredPrev[i + iACHOffset][m_iMBColumn - 1].iDC;
    else if(iDCPredMode == 0)
      pOrg[0] -= m_pPred[i + iACHOffset][m_iMBColumn - 2].iDC;
    else if(iDCPredMode == 2)
    {
      if(i == 0 || !bUV42X)
        pOrg[0] -= (m_pPred[i + iACHOffset][m_iMBColumn - 2].iDC + m_pPredPrev[i + iACHOffset][m_iMBColumn - 1].iDC) >> 1;
      else
        pOrg[0] -= (m_pPred[i + iACHOffset][m_iMBColumn - 2].iDC + m_pPredPrev[i + iACHOffset][m_iMBColumn - 1].iDC + 1) >> 1;
    }
  }

  // Predict LP
  for(i = 0; i < iChannels; i ++)
  {
    pOrg = m_iDCLPTable[i];

    if(iLPPredMode == 1)
    {
      if(i == 0 || !bUV42X)
      {
        pRef = m_pPredPrev[i + iACHOffset][m_iMBColumn - 1].iDCLP;
        pOrg[4]  -= pRef[3];
        pOrg[8]  -= pRef[4];
        pOrg[12] -= pRef[5];
      }
      else
      {
        if(iColorFormat == JXR_ICF_YUV422)
        {
          pOrg[4] -= m_pPredPrev[i + iACHOffset][m_iMBColumn - 1].iDCLP[4];
          pOrg[6] -= pOrg[2];
          pOrg[2] -= m_pPredPrev[i + iACHOffset][m_iMBColumn - 1].iDCLP[3];
        }
        else
          pOrg[2] -= m_pPredPrev[i + iACHOffset][m_iMBColumn - 1].iDCLP[1];
      }
    }
    else if(iLPPredMode == 0)
    {
      if(i == 0 || !bUV42X)
      {
        pRef = m_pPred[i + iACHOffset][m_iMBColumn - 2].iDCLP;
        pOrg[1] -= pRef[0];
        pOrg[2] -= pRef[1];
        pOrg[3] -= pRef[2];
      }
      else
      {
        if(iColorFormat == JXR_ICF_YUV422)
        {
          pOrg[4] -= m_pPred[i + iACHOffset][m_iMBColumn - 2].iDCLP[4];
          pOrg[1] -= m_pPred[i + iACHOffset][m_iMBColumn - 2].iDCLP[0];
          pOrg[5] -= m_pPred[i + iACHOffset][m_iMBColumn - 2].iDCLP[2];
        }
        else
          pOrg[1] -= m_pPred[i + iACHOffset][m_iMBColumn - 2].iDCLP[0];
      }
    }
  }

  // Predict HP
  if(m_iCUSize == 2)
    predHP<Ipp16s>(iChannels, iColorFormat, 2 - m_iOrientation, &m_pMBTab[iACHOffset]);
  else
    predHP<Ipp32s>(iChannels, iColorFormat, 2 - m_iOrientation, &m_pMBTab[iACHOffset]);

  return ExcStatusOk;
} // CJPEGXREncoder::predMacroblock()


/***********************************/
// Preproccessing functions
/***********************************/

template<class TYPE>
static void sliceToMB4(MBTab* pMBTab, IppiSize roi, Ipp32u iBorderType)
{
  TYPE *pMB = (TYPE*)(pMBTab->pMB4) + roi.width;
  TYPE *pMBRow = _NEARROW(TYPE, pMB, roi.height, pMBTab->iStepMB4);
  TYPE *pCC0 = (TYPE*)pMBTab->pCC0;
  TYPE *pCC1 = (TYPE*)pMBTab->pCC1;

  if(ippBorderInMemRight & iBorderType)
  {
    if(ippBorderInMemTop & iBorderType)
      ippiCopy_C1R(pCC0, pMBTab->iStepCC, pMB, pMBTab->iStepMB4, roi);
    if(ippBorderInMemBottom & iBorderType)
      ippiCopy_C1R(pCC1, pMBTab->iStepCC, pMBRow, pMBTab->iStepMB4, roi);
  }
} // sliceToMB4()

template<class TYPE>
static void MB4ToSlice(MBTab* pMBTab, IppiSize roi, Ipp32u iBorderType)
{
  TYPE *pMB = (TYPE*)(pMBTab->pMB4);
  TYPE *pMBRow = _NEARROW(TYPE, pMB, roi.height, pMBTab->iStepMB4);
  TYPE *pCC1 = (TYPE*)pMBTab->pCC1;

  if(ippBorderInMemLeft & iBorderType)
  {
    if(ippBorderInMemBottom & iBorderType)
      ippiCopy_C1R(pMBRow, pMBTab->iStepMB4, pCC1 - roi.width, pMBTab->iStepCC, roi);
  }
} // MB4ToSlice()

template<class TYPE>
static void dirPCTransformSecond(MBTab* pTabMB4, IppiSize roi, Ipp32u iDownscale)
{
  if(roi.height == 8)
    ippiPCTFwd8x8DC_HDP((TYPE*)pTabMB4->pMB4, pTabMB4->iStepMB4, iDownscale);
  else if(roi.width == 16)
    ippiPCTFwd16x16DC_HDP((TYPE*)pTabMB4->pMB4, pTabMB4->iStepMB4, iDownscale);
  else
    ippiPCTFwd8x16DC_HDP((TYPE*)pTabMB4->pMB4, pTabMB4->iStepMB4, iDownscale);
} // dirPCTransformSecond()

template<class TYPE>
static void dirPCTransformFirst(MBTab* pTabMB4, IppiSize roi, Ipp32u iBorderType)
{
  Ipp32u bTop    = !(ippBorderInMemTop    & iBorderType);
  Ipp32u bBottom = !(ippBorderInMemBottom & iBorderType);
  Ipp32u bLeft   = !(ippBorderInMemLeft   & iBorderType);
  Ipp32u bRight  = !(ippBorderInMemRight  & iBorderType);
  IppiSize mbSize = {roi.width, roi.height};
  IppiRect iRect ={-4, -4, roi.width, roi.height};
  Ipp32s iX = (bLeft)? -iRect.x : 0;
  Ipp32s iY = (bTop)? -iRect.y : 0;
  TYPE* pSrc;

  if((roi.width == 8 || roi.height == 8) && (bLeft || bRight))
    mbSize.width = 4;
  else if(bLeft)
    mbSize.width = 12;
  else if(bRight)
    mbSize.width = 4;

  if(bTop)
    mbSize.height -= 4;
  else if(bBottom)
    mbSize.height = 4;

  pSrc = _PEL(TYPE, pTabMB4->pMB4, iX + iRect.x + iRect.width, iY + iRect.y + iRect.height, pTabMB4->iStepMB4);

  if(16 == mbSize.width && 16 == mbSize.height)
    ippiPCTFwd16x16_JPEGXR(pSrc, pTabMB4->iStepMB4);
  else if(8 == mbSize.width && 16 == mbSize.height)
    ippiPCTFwd8x16_JPEGXR(pSrc, pTabMB4->iStepMB4);
  else if(8 == mbSize.width && 8 == mbSize.height)
    ippiPCTFwd8x8_JPEGXR(pSrc, pTabMB4->iStepMB4);
  else
    ippiPCTFwd_JPEGXR(pSrc, pTabMB4->iStepMB4, mbSize);
} // dirPCTransformFirst()

template<class TYPE>
static void dirFilterFirst(MBTab* pTabMB4, IppiSize roi, Ipp32u iBorderType)
{
  Ipp32u bTop    = !(ippBorderInMemTop    & iBorderType);
  Ipp32u bBottom = !(ippBorderInMemBottom & iBorderType);
  Ipp32u bLeft   = !(ippBorderInMemLeft   & iBorderType);
  Ipp32u bRight  = !(ippBorderInMemRight  & iBorderType);
  IppiSize mbSize = roi;
  IppiRect iRect = {-2, -2, roi.width, roi.height};
  Ipp32s iX = (bLeft)? -iRect.x : 0;
  Ipp32s iY = (bTop)?  -iRect.y : 0;
  TYPE* pSrc;

  if(bLeft)
    mbSize.width = iRect.width + iRect.x;
  else if(bRight)
    mbSize.width = -iRect.x;
  if(bTop)
    mbSize.height = iRect.height + iRect.y;
  else if(bBottom)
    mbSize.height = -iRect.y;

  pSrc = _PEL(TYPE, pTabMB4->pMB4, iX + iRect.x + iRect.width, iY + iRect.y + iRect.height, pTabMB4->iStepMB4);
  ippiFilterFwd_HDP(pSrc, pTabMB4->iStepMB4, mbSize, (IppiBorderType)iBorderType);
} // dirFilterFirst()

template<class TYPE>
static void dirFilterSecond(MBTab* pTabMB4, IppiSize roi, Ipp32u iBorderType)
{
  Ipp32u bTop    = !(ippBorderInMemTop    & iBorderType);
  Ipp32u bBottom = !(ippBorderInMemBottom & iBorderType);
  Ipp32u bLeft   = !(ippBorderInMemLeft   & iBorderType);
  Ipp32u bRight  = !(ippBorderInMemRight  & iBorderType);
  IppiSize mbSize = roi;
  IppiRect iRect = {-(roi.width/2), -(roi.width/2), roi.width, roi.height};
  Ipp32s iX = (bLeft)? -iRect.x : 0;
  Ipp32s iY = (bTop)? -iRect.y : 0;
  TYPE* pSrc;

  if(bLeft)
    mbSize.width = iRect.width + iRect.x;
  else if(bRight)
    mbSize.width = -iRect.x;
  if(bTop)
    mbSize.height = iRect.height + iRect.y;
  else if(bBottom)
    mbSize.height = -iRect.y;

  pSrc = _PEL(TYPE, pTabMB4->pMB4, iX + iRect.x + iRect.width, iY + iRect.y + iRect.height, pTabMB4->iStepMB4);

  if(iRect.width == 16 && iRect.height == 16)
    ippiFilterFwdDCYUV444_HDP(pSrc, pTabMB4->iStepMB4, mbSize, (IppiBorderType)iBorderType);
  else if(mbSize.width != 4 || mbSize.height != 4)
    ippiFilterFwdDCYUV420_HDP(pSrc, pTabMB4->iStepMB4, mbSize, (IppiBorderType)iBorderType);
} // dirFilterSecond()

// Direct PC Transform
ExcStatus CJPEGXREncoder::PCTransform()
{
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u bLeft   = !(ippBorderInMemLeft & m_iBorderPCT);
  Ipp32u bTop    = !(ippBorderInMemTop  & m_iBorderPCT);
  Ipp32u iOverlap = m_iOverlap;
  Ipp32u iACHOffset = (m_bAlphaProcess)?m_iACHOffset:0;
  IppiSize lRoi = {16, 16};
  IppiSize cRoi = {16, 16};
  IppiSize tRoi;
  Ipp32u i;

  if(iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)
  {
    cRoi.width = 8;
    if(iColorFormat == JXR_ICF_YUV420)
      cRoi.height = 8;
  }

  if(m_iCUSize == 2)
  {
    for(i = 0; i < iChannels; i++)
      sliceToMB4<Ipp16s>(&m_pMBTab[i + iACHOffset], (i)?cRoi:lRoi, m_iBorderPCT);

    for(i = 0; i < iChannels; i++)
    {
      tRoi = (i)?cRoi:lRoi;

      if(0 != iOverlap)
        dirFilterFirst<Ipp16s>(&m_pMBTab[i + iACHOffset], tRoi, m_iBorderPCT);

      dirPCTransformFirst<Ipp16s>(&m_pMBTab[i + iACHOffset], tRoi, m_iBorderPCT);

      if(2 == iOverlap)
        dirFilterSecond<Ipp16s>(&m_pMBTab[i + iACHOffset], tRoi, m_iBorderPCT);

      if(!bLeft && !bTop)
        dirPCTransformSecond<Ipp16s>(&m_pMBTab[i + iACHOffset], tRoi, (i!=0) && m_pImagePlane->bScalingFlag);
    }

    for(i = 0; i < iChannels; ++i)
      MB4ToSlice<Ipp16s>(&m_pMBTab[i + iACHOffset], (i)?cRoi:lRoi, m_iBorderPCT);
  }
  else
  {
    for(i = 0; i < iChannels; i++)
      sliceToMB4<Ipp32s>(&m_pMBTab[i + iACHOffset], (i)?cRoi:lRoi, m_iBorderPCT);

    for(i = 0; i < iChannels; i++)
    {
      tRoi = (i)?cRoi:lRoi;

      if(0 != iOverlap)
        dirFilterFirst<Ipp32s>(&m_pMBTab[i + iACHOffset], tRoi, m_iBorderPCT);

      dirPCTransformFirst<Ipp32s>(&m_pMBTab[i + iACHOffset], tRoi, m_iBorderPCT);

      if(2 == iOverlap)
        dirFilterSecond<Ipp32s>(&m_pMBTab[i + iACHOffset], tRoi, m_iBorderPCT);

      if(!bLeft && !bTop)
        dirPCTransformSecond<Ipp32s>(&m_pMBTab[i + iACHOffset], tRoi, (i!=0) && m_pImagePlane->bScalingFlag);
    }

    for(i = 0; i < iChannels; i++)
      MB4ToSlice<Ipp32s>(&m_pMBTab[i + iACHOffset], (i)?cRoi:lRoi, m_iBorderPCT);
  }

  return ExcStatusOk;
} // CJPEGXREncoder::dirPCTransform()


// Colorspace transformation function
ExcStatus CJPEGXREncoder::colorConvert(Ipp8u *pSrc, Ipp32u iSrcStep)
{
  Ipp32s *pDst[JXR_MAX_CHANNELS];
  Ipp32u pDstStep[JXR_MAX_CHANNELS];
  Ipp32u iWidth = m_iCurTileMBWidth * 16;
  Ipp8u  iColorFormat = (Ipp8u)m_pImagePlane->iColorFormat;
  Ipp8u  bResampling = (Ipp8u)m_pImagePlane->bResampling;
  Ipp8u  iScale = m_pImagePlane->bScalingFlag ? 3 : 0;
  Ipp8u  iShift = (Ipp8u)m_pImagePlane->iShift;
  Ipp8u  iMan = (Ipp8u)m_pImagePlane->iMantiss;
  Ipp8u  iExp = (Ipp8u)m_pImagePlane->iExp;
  IppiSize imgRoi;
  Ipp32u i, j;

  for(i = 0; i < m_iChannels; i++)
  {
    pDst[i]     = m_pMBTab[i].pSliceCC1;
    pDstStep[i] = m_pMBTab[i].iStepCC;
  }
  if(bResampling)
  {
    pDst[1] = m_pResU, pDstStep[1] = m_pMBTab[0].iStepCC;
    pDst[2] = m_pResV, pDstStep[2] = m_pMBTab[0].iStepCC;
  }

  imgRoi.height = ((m_iCurTileHeight - (m_iMBRow) * 16) < 16)?(m_iCurTileHeight - (m_iMBRow) * 16):16;
  imgRoi.width = m_iCurTileWidth;

  switch(m_iSrcColorFormat)
  {
  case JXR_OCF_Y:
    if(m_iBitDepth == JXR_BD_1W) // BW White=1
    {
      // Unsupported
    }
    else if(m_iBitDepth == JXR_BD_1B) // BW Black=1
    {
      // Unsupported
    }
    else if(m_iBitDepth == JXR_BD_8) // GRAY 8
    {
      if(m_iCUSize == 2)
        ccGrayToY_8u16s(pSrc, iSrcStep, (Ipp16s*)pDst[0], pDstStep[0], imgRoi, iScale);
      else
        ccGrayToY_8u32s(pSrc, iSrcStep, pDst[0], pDstStep[0], imgRoi, iScale);
    }
    else if(m_iBitDepth == JXR_BD_16) // GRAY 16
      ccGrayToY_16u32s((Ipp16u*)pSrc, iSrcStep, pDst[0], pDstStep[0], imgRoi, iScale, iShift);
    else if(m_iBitDepth == JXR_BD_16S) // GRAY 16S
      ccGrayToY_16s32s((Ipp16s*)pSrc, iSrcStep, pDst[0], pDstStep[0], imgRoi, iScale, iShift);
    else if(m_iBitDepth == JXR_BD_16F) // GRAY 16F
      ccGrayToY_16f32s((Ipp16u*)pSrc, iSrcStep, pDst[0], pDstStep[0], imgRoi, iScale);
    else if(m_iBitDepth == JXR_BD_32S) // GRAY 32S
      ccGrayToY_32s32s((Ipp32s*)pSrc, iSrcStep, pDst[0], pDstStep[0], imgRoi, iScale, iShift);
    else if(m_iBitDepth == JXR_BD_32F) // GRAY 32F
      ccGrayToY_32f32s((Ipp32f*)pSrc, iSrcStep, pDst[0], pDstStep[0], imgRoi, iScale, iExp, iMan);
    break;

  case JXR_OCF_RGB:
    if(m_iBitDepth == JXR_BD_8) // RGB 8
    {
      if(m_iCUSize == 2)
      {
        if(m_bBGR)
        {
          if(m_bAlphaFlag)
            ccBGRToYUV_8u16s_C4P4(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
          else if(m_bFakeAlpha)
            ccBGRToYUV_8u16s_C4P3(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
          else
            ccBGRToYUV_8u16s_C3P3(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
        }
        else
        {
          if(m_bAlphaFlag)
            ccRGBToYUV_8u16s_C4P4(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
          else if(m_bFakeAlpha)
            ccRGBToYUV_8u16s_C4P3(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
          else
            ccRGBToYUV_8u16s_C3P3(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
        }
      }
      else
      {
        if(m_bBGR)
        {
          if(m_bAlphaFlag)
            ccBGRToYUV_8u32s_C4P4(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
          else if(m_bFakeAlpha)
            ccBGRToYUV_8u32s_C4P3(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
          else
            ccBGRToYUV_8u32s_C3P3(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
        }
        else
        {
          if(m_bAlphaFlag)
            ccRGBToYUV_8u32s_C4P4(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
          else if(m_bFakeAlpha)
            ccRGBToYUV_8u32s_C4P3(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
          else
            ccRGBToYUV_8u32s_C3P3(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
        }
      }
    }
    else if(m_iBitDepth == JXR_BD_16) // RGB 16
    {
      if(m_bAlphaFlag)
        ccRGBToYUV_16u32s_C4P4((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
      else
        ccRGBToYUV_16u32s_C3P3((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
    }
    else if(m_iBitDepth == JXR_BD_16S) // RGB 16S
    {
      if(m_bAlphaFlag)
        ccRGBToYUV_16s32s_C4P4((Ipp16s*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
      else if(m_bFakeAlpha)
        ccRGBToYUV_16s32s_C4P3((Ipp16s*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
      else
        ccRGBToYUV_16s32s_C3P3((Ipp16s*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
    }
    else if(m_iBitDepth == JXR_BD_16F) // RGB 16F
    {
      if(m_bAlphaFlag)
        ccRGBToYUV_16f32s_C4P4((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
      else if(m_bFakeAlpha)
        ccRGBToYUV_16f32s_C4P3((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
      else
        ccRGBToYUV_16f32s_C3P3((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
    }
    else if(m_iBitDepth == JXR_BD_32S) // RGB 32S
    {
      if(m_bAlphaFlag)
        ccRGBToYUV_32s32s_C4P4((Ipp32s*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
      else if(m_bFakeAlpha)
        ccRGBToYUV_32s32s_C4P3((Ipp32s*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
      else
        ccRGBToYUV_32s32s_C3P3((Ipp32s*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
    }
    else if(m_iBitDepth == JXR_BD_32F) // RGB 32F
    {
      if(m_bAlphaFlag)
        ccRGBToYUV_32f32s_C4P4((Ipp32f*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iExp, iMan);
      else if(m_bFakeAlpha)
        ccRGBToYUV_32f32s_C4P3((Ipp32f*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iExp, iMan);
      else
        ccRGBToYUV_32f32s_C3P3((Ipp32f*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iExp, iMan);
    }
    else if(m_iBitDepth == JXR_BD_5) // RGB 555
    {
      // Unsupported
    }
    else if(m_iBitDepth == JXR_BD_565) // RGB 565
    {
      // Unsupported
    }
    else if(m_iBitDepth == JXR_BD_10) // RGB 101010
    {
      // Unsupported
    }
    break;

  case JXR_OCF_RGBE:
    if(m_iBitDepth == JXR_BD_8)
      ccRGBEToYUV_8u32s_C4P3(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
    break;

  case JXR_OCF_CMYK:
    if(m_iBitDepth == JXR_BD_8) // CMYK 8
    {
      if(m_iCUSize == 2)
      {
        if(m_bAlphaFlag)
          ccCMYKToYUVK_8u16s_C5P5(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
        else
          ccCMYKToYUVK_8u16s_C4P4(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
      }
      else
      {
        if(m_bAlphaFlag)
          ccCMYKToYUVK_8u32s_C5P5(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
        else
          ccCMYKToYUVK_8u32s_C4P4(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
      }
    }
    else if(m_iBitDepth == JXR_BD_16) // CMYK 16
    {
      if(m_bAlphaFlag)
        ccCMYKToYUVK_16u32s_C5P5((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
      else
        ccCMYKToYUVK_16u32s_C4P4((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
    }
    break;

  case JXR_OCF_CMYKD:
    if(m_iBitDepth == JXR_BD_8) // CMYKD 8
    {
      if(m_iCUSize == 2)
      {
        if(m_bAlphaFlag)
          ccCMYKDToYUVK_8u16s_C5P5(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
        else
          ccCMYKDToYUVK_8u16s_C4P4(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
      }
      else
      {
        if(m_bAlphaFlag)
          ccCMYKDToYUVK_8u32s_C5P5(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
        else
          ccCMYKDToYUVK_8u32s_C4P4(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
      }
    }
    else if(m_iBitDepth == JXR_BD_16) // CMYKD 16
    {
      if(m_bAlphaFlag)
        ccCMYKDToYUVK_16u32s_C5P5((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
      else
        ccCMYKDToYUVK_16u32s_C4P4((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
    }
    break;

  case JXR_OCF_NCH:
    if(m_iBitDepth == JXR_BD_8) // NCHANNELS 8
    {
      if(m_iCUSize == 2)
        ccNCHToNCH_8u16s(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale, (Ipp16u)m_iChannels);
      else
        ccNCHToNCH_8u32s(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, (Ipp16u)m_iChannels);
    }
    else if(m_iBitDepth == JXR_BD_16) // NCHANNELS 16
      ccNCHToNCH_16u32s((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift, (Ipp16u)m_iChannels);
    break;

  case JXR_OCF_YUV444:
    if(m_iBitDepth == JXR_BD_8) // YUV444 8
    {
      if(m_iCUSize == 2)
      {
        if(m_bAlphaFlag)
          ccYUVToYUV_8u16s_C4P4(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
        else
          ccYUVToYUV_8u16s_C3P3(pSrc, iSrcStep, (Ipp16s**)pDst, pDstStep, imgRoi, iScale);
      }
      else
      {
        if(m_bAlphaFlag)
          ccYUVToYUV_8u32s_C4P4(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
        else
          ccYUVToYUV_8u32s_C3P3(pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale);
      }
    }
    else if(m_iBitDepth == JXR_BD_10) // YUV444 10
    {
      // Unsupported
    }
    else if(m_iBitDepth == JXR_BD_16) // YUV444 16
    {
      if(m_bAlphaFlag)
        ccYUVToYUV_16u32s_C4P4((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
      else
        ccYUVToYUV_16u32s_C3P3((Ipp16u*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
    }
    else if(m_iBitDepth == JXR_BD_16S) // YUV444 16S
    {
      if(m_bAlphaFlag)
        ccYUVToYUV_16s32s_C4P4((Ipp16s*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
      else
        ccYUVToYUV_16s32s_C3P3((Ipp16s*)pSrc, iSrcStep, pDst, pDstStep, imgRoi, iScale, iShift);
    }
    break;

  case JXR_OCF_YUV422:
    if(m_iBitDepth == JXR_BD_8) // YUV422 8
    {
      // Unsupported
    }
    else if(m_iBitDepth == JXR_BD_10) // YUV422 10
    {
      // Unsupported
    }
    else if(m_iBitDepth == JXR_BD_16) // YUV422 16
    {
      // Unsupported
    }
    break;

  case JXR_OCF_YUV420:
    if(m_iBitDepth == JXR_BD_8) // YUV420 8
    {
      // Unsupported
    }
    break;
  }

  // Padding
  if(imgRoi.height < 16 || imgRoi.width < (Ipp32s)iWidth)
  {
    IppiSize wROI = {1, imgRoi.height};
    IppiSize hROI = {iWidth, 1};

    if(m_iCUSize == 2)
    {
      for(i = 0; i < m_iChannels; i++)
      {
        Ipp32u  iStep = pDstStep[i]/2;
        Ipp16s* refCol = (Ipp16s*)pDst[i] + imgRoi.width - 1;
        Ipp16s* refRow = (Ipp16s*)pDst[i] + (imgRoi.height - 1) * iStep;

        for(j = imgRoi.width; j < iWidth; j++)
          ippiCopy_16s_C1R(refCol, pDstStep[i], (Ipp16s*)pDst[i] + j, pDstStep[i], wROI);
        for(j = imgRoi.height; j < 16; j++)
          ippiCopy_16s_C1R(refRow, pDstStep[i], (Ipp16s*)pDst[i] + j * iStep, pDstStep[i], hROI);
      }
    }
    else
    {
      for(i = 0; i < m_iChannels; i++)
      {
        Ipp32u  iStep = pDstStep[i]/4;
        Ipp32s* refCol = (Ipp32s*)pDst[i] + imgRoi.width - 1;
        Ipp32s* refRow = (Ipp32s*)pDst[i] + (imgRoi.height - 1) * iStep;

        for(j = imgRoi.width; j < iWidth; j++)
          ippiCopy_32s_C1R(refCol, pDstStep[i], (Ipp32s*)pDst[i] + j, pDstStep[i], wROI);
        for(j = imgRoi.height; j < 16; j++)
          ippiCopy_32s_C1R(refRow, pDstStep[i], (Ipp32s*)pDst[i] + j * iStep, pDstStep[i], hROI);
      }
    }
  }

  // Downsampling
  if(bResampling)
  {
    Ipp32s* pResDstU  = (iColorFormat == JXR_ICF_YUV420)?m_pResU:m_pMBTab[1].pSliceCC1;
    Ipp32s* pResDstV  = (iColorFormat == JXR_ICF_YUV420)?m_pResV:m_pMBTab[2].pSliceCC1;
    Ipp32u  iResStepU = (iColorFormat == JXR_ICF_YUV420)?m_pMBTab[0].iStepCC:m_pMBTab[1].iStepCC;
    Ipp32u  iResStepV = (iColorFormat == JXR_ICF_YUV420)?m_pMBTab[0].iStepCC:m_pMBTab[2].iStepCC;
    imgRoi.width = m_iCurTileMBWidth * 8;

    if(m_iCUSize == 2)
    {
      ippiYUV444To422_HDP_16s_C1R((Ipp16s*)pDst[1], pDstStep[1], (Ipp16s*)pResDstU, iResStepU, imgRoi);
      ippiYUV444To422_HDP_16s_C1R((Ipp16s*)pDst[2], pDstStep[2], (Ipp16s*)pResDstV, iResStepV, imgRoi);
    }
    else
    {
      ippiYUV444To422_HDP_32s_C1R((Ipp32s*)pDst[1], pDstStep[1], (Ipp32s*)pResDstU, iResStepU, imgRoi);
      ippiYUV444To422_HDP_32s_C1R((Ipp32s*)pDst[2], pDstStep[2], (Ipp32s*)pResDstV, iResStepV, imgRoi);
    }

    if(iColorFormat == JXR_ICF_YUV420)
    {
      Ipp32u iBorder = 0;
      imgRoi.height = 8;
      pResDstU = (Ipp32s*)_NEARROW(void, m_pMBTab[1].pSliceCC1, -1, m_pMBTab[1].iStepCC);
      pResDstV = (Ipp32s*)_NEARROW(void, m_pMBTab[2].pSliceCC1, -1, m_pMBTab[2].iStepCC);

      if(m_iMBRow != 0)
        iBorder |= JXR_BORDER_TOP;
      if(m_iMBRow + 1 != m_iCurTileMBHeight)
        iBorder |= JXR_BORDER_BOTTOM;
      else
        imgRoi.height++;

      if(m_iCUSize == 2)
      {
        ippiYUV422To420_HDP_16s_C1R((Ipp16s*)pDst[1], pDstStep[1], (Ipp16s*)pResDstU, m_pMBTab[1].iStepCC, imgRoi, (IppiBorderType)iBorder);
        ippiYUV422To420_HDP_16s_C1R((Ipp16s*)pDst[2], pDstStep[2], (Ipp16s*)pResDstV, m_pMBTab[2].iStepCC, imgRoi, (IppiBorderType)iBorder);
      }
      else
      {
        ippiYUV422To420_HDP_32s_C1R((Ipp32s*)pDst[1], pDstStep[1], (Ipp32s*)pResDstU, m_pMBTab[1].iStepCC, imgRoi, (IppiBorderType)iBorder);
        ippiYUV422To420_HDP_32s_C1R((Ipp32s*)pDst[2], pDstStep[2], (Ipp32s*)pResDstV, m_pMBTab[2].iStepCC, imgRoi, (IppiBorderType)iBorder);
      }

      if(m_iMBRow != 0)
      {
        ippsCopy_8u((Ipp8u*)pResDstU, (Ipp8u*)_NEARROW(void, m_pMBTab[1].pSliceCC0, 7, m_pMBTab[1].iStepCC), m_pMBTab[1].iStepCC);
        ippsCopy_8u((Ipp8u*)pResDstV, (Ipp8u*)_NEARROW(void, m_pMBTab[2].pSliceCC0, 7, m_pMBTab[2].iStepCC), m_pMBTab[2].iStepCC);
      }
      if(m_iMBRow + 1 != m_iCurTileMBHeight)
      {
        Ipp8u* pRowU = (Ipp8u*)_NEARROW(void, pDst[1], 12, pDstStep[1]);
        Ipp8u* pRowV = (Ipp8u*)_NEARROW(void, pDst[2], 12, pDstStep[2]);
        ippsCopy_8u(pRowU,(Ipp8u*)_NEARROW(void, pRowU, -16, pDstStep[1]), 4*pDstStep[1]);
        ippsCopy_8u(pRowV,(Ipp8u*)_NEARROW(void, pRowV, -16, pDstStep[2]), 4*pDstStep[2]);
      }
    }
  }

  return ExcStatusOk;
} // CJPEGXREncoder::colorConvert()
