/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2008-2012 Intel Corporation. All Rights Reserved.
//
*/

#include "jpegxrdec.h"
#include "jpegxrdec_cc.h"
#include "jpegxrvalues.h"
#ifndef __IPPI_H__
#include "ippi.h"
#endif
#ifndef __IPPCC_H__
#include "ippcc.h"
#endif


static const Ipp16s pVLCLookupDCType[48] = {
  53,21,28,28,11,11,11,11,43,43,43,43,59,59,59,59,
  2,2,2,2,2,2,2,2,34,34,34,34,34,34,34,34,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static const Ipp16s pVLCLookupCBPCh[40] = {
  19,19,19,19,27,27,27,27,10,10,10,10,10,10,10,10,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  0,0,0,0,0,0,0,0
};

static const Ipp8u iDCRemap[] = { 2, 3, 4, 6, 10, 14};

// Lengh tables for decodeHPCBP()
static const Ipp8u iLengthTable1[5] = {0, 2, 0, 2, 0};
static const Ipp8u iLengthTable2[8] = {2, 2, 2, 2, 3, 3, 3, 3};
static const Ipp8u iLengthTable3[4] = {2, 2, 1, 1};
static const Ipp8u iCountCBP[40] = {
   0,  0,  0,  0,  0,  0,  0,  0,
   1,  1,  2,  2,  4,  4,  8,  8,
   3,  3,  5,  5,  6,  9, 10, 12,
  14, 14, 13, 13, 11, 11,  7,  7,
  15, 15, 15, 15, 15, 15, 15, 15
};
static const Ipp8u iValueCBP[4]   = {48, 32, 16, 16};
static const Ipp8u iEValueCBP[16] = { 0, 15,  3, 12,  1,  2,  4,  8,  5,  6,  9, 10,  7, 11, 13, 14};
static const Ipp8u iValueUpdate[4]    = {11, 10,  9,  9};
static const Ipp8u iExtraCBPOffset[6] = { 0,  4,  2,  8, 12, 1};
static const Ipp8u iExtraBits[6]      = { 0,  2,  1,  2,  2, 0};
static const Ipp8u iCBP422Offset[4]   = { 0,  1,  4,  5};
static const Ipp8u iCBP422Value[4]    = { 5,  4,  1,  1};

static IppStatus ippiPCTInv16x16DC_HDP(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale)
{
  return ippiPCTInv16x16DC_HDP_32s_C1IR(pSrcDst, srcDstStep, upscale);
}


static IppStatus ippiPCTInv16x16DC_HDP(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale)
{
  return ippiPCTInv16x16DC_HDP_16s_C1IR(pSrcDst, srcDstStep, upscale);
}


static IppStatus ippiPCTInv8x16DC_HDP(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale)
{
  return ippiPCTInv8x16DC_HDP_32s_C1IR(pSrcDst, srcDstStep, upscale);
}


static IppStatus ippiPCTInv8x16DC_HDP(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale)
{
  return ippiPCTInv8x16DC_HDP_16s_C1IR(pSrcDst, srcDstStep, upscale);
}


static IppStatus ippiPCTInv8x8DC_HDP(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale)
{
  return ippiPCTInv8x8DC_HDP_32s_C1IR(pSrcDst, srcDstStep, upscale);
}


static IppStatus ippiPCTInv8x8DC_HDP(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale)
{
  return ippiPCTInv8x8DC_HDP_16s_C1IR(pSrcDst, srcDstStep, upscale);
}


static IppStatus ippiPCTInv16x16_JPEGXR(Ipp32s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTInv16x16_JPEGXR_32s_C1IR(pSrcDst, srcDstStep);
}


static IppStatus ippiPCTInv16x16_JPEGXR(Ipp16s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTInv16x16_JPEGXR_16s_C1IR(pSrcDst, srcDstStep);
}


static IppStatus ippiPCTInv8x16_JPEGXR(Ipp32s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTInv8x16_JPEGXR_32s_C1IR(pSrcDst, srcDstStep);
}


static IppStatus ippiPCTInv8x16_JPEGXR(Ipp16s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTInv8x16_JPEGXR_16s_C1IR(pSrcDst, srcDstStep);
}


static IppStatus ippiPCTInv8x8_JPEGXR(Ipp32s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTInv8x8_JPEGXR_32s_C1IR(pSrcDst, srcDstStep);
}


static IppStatus ippiPCTInv8x8_JPEGXR(Ipp16s *pSrcDst, Ipp32u srcDstStep)
{
  return ippiPCTInv8x8_JPEGXR_16s_C1IR(pSrcDst, srcDstStep);
}


static IppStatus ippiPCTInv_JPEGXR(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize)
{
  return ippiPCTInv_JPEGXR_32s_C1IR(pSrcDst, srcDstStep, roiSize);
}


static IppStatus ippiPCTInv_JPEGXR(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize)
{
  return ippiPCTInv_JPEGXR_16s_C1IR(pSrcDst, srcDstStep, roiSize);
}


static IppStatus ippiFilterInv_HDP(Ipp32s* pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterInv_HDP_32s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}


static IppStatus ippiFilterInv_HDP(Ipp16s* pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterInv_HDP_16s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}


static IppStatus ippiFilterInvDCYUV444_HDP(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterInvDCYUV444_HDP_32s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}


static IppStatus ippiFilterInvDCYUV444_HDP(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterInvDCYUV444_HDP_16s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}


static IppStatus ippiFilterInvDCYUV420_HDP(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterInvDCYUV420_HDP_32s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}


static IppStatus ippiFilterInvDCYUV420_HDP(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType)
{
  return ippiFilterInvDCYUV420_HDP_16s_C1IR(pSrcDst, srcDstStep, roiSize, borderType);
}


/***********************************/
// Decoder initialization functions
/***********************************/
static void adaptHuffman(AdaptiveHuffman *pHuff);

static void setQuantizer(QuantInf* pQuant, Ipp32u iShift, Ipp32u bScalingFlag)
{
  Ipp8u iQPIndex = pQuant->iIndex;

  if(iQPIndex == 0)
    pQuant->iQP = 1;
  else
  {
    if(bScalingFlag)
    {
      Ipp32s iMantis = 0;
      Ipp32s iExp = 0;

      if(pQuant->iIndex < 16)
      {
        iMantis = pQuant->iIndex;
        iExp = iShift;
      }
      else
      {
        iMantis = 16 + (pQuant->iIndex & 0xf);
        iExp = ((pQuant->iIndex >> 4) - 1) + iShift;
      }

      pQuant->iQP = iMantis << iExp;
    }
    else
    {
      Ipp32s iMantis = 0;
      Ipp32s iExp = 0;

      if(pQuant->iIndex < 32)
      {
        iMantis = (pQuant->iIndex + 3) >> 2;
        iExp = 0;
      }
      else if(pQuant->iIndex < 48)
      {
        iMantis = (17 + (pQuant->iIndex & 0xf)) >> 1;
        iExp = (pQuant->iIndex >> 4) - 2;
      }
      else
      {
        iMantis = 16 + (pQuant->iIndex & 0xf);
        iExp = (pQuant->iIndex >> 4) - 3;
      }

      pQuant->iQP = iMantis << iExp;
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
  if(pImagePlane->bDCUniformFlag)
  {
    pQuantDC[0] = (QuantInf*)pAllocPtr; pAllocPtr += iShift;
    for(i = 1; i < iChannels; i++)
      pQuantDC[i] = pQuantDC[i - 1] + 1;

    for(i = 0; i < iChannels; i++)
    {
      pQuantDC[i]->iIndex = pImagePlane->iDCQuant[i];
      if(i > 0 && iCHModeDC == JXR_CM_UNIFORM)
        pQuantDC[i][0] = pQuantDC[0][0];
      else if(i > 0 && iCHModeDC == JXR_CM_SEPARATE)
        pQuantDC[i][0] = pQuantDC[1][0];
      setQuantizer(pQuantDC[i], (i > 0) ? 0 : 1, bScalingFlag);
    }
  }
  if(pImagePlane->iBands < JXR_SB_DCONLY)
  {
    if(pImagePlane->bLPUniformFlag)
    {
      pQuantLP[0] = (QuantInf*)pAllocPtr; pAllocPtr += iShift;
      for(i = 1; i < iChannels; i++)
        pQuantLP[i] = pQuantLP[i - 1] + 1;

      for(i = 0; i < iChannels; i++)
      {
        pQuantLP[i]->iIndex = pImagePlane->iLPQuant[i];
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
    if(pImagePlane->bHPUniformFlag)
    {
      pQuantHP[0] = (QuantInf*)pAllocPtr; pAllocPtr += iShift;
      for(i = 1; i < iChannels; i++)
        pQuantHP[i] = pQuantHP[i - 1] + 1;

      for(i = 0; i < iChannels; i++)
      {
        pQuantHP[i]->iIndex = pImagePlane->iHPQuant[i];
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

// Main decoder allocation and init
ExcStatus CJPEGXRDecoder::initDecoder(void)
{
  Ipp8u *pAllocPtr;
  Ipp32u iAllocSize = 0;
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u iBands = m_pImagePlane->iBands;
  Ipp32s iHuffSize = 0;
  Ipp32u iImageSize = 0;
  Ipp32u iResSize = 0;
  Ipp32u iTemp;
  Ipp32u i;

  ippiVLCGetStateSize_JPEGXR(&iHuffSize);

  m_iCUSize = (m_iUnitSize == 1)?((m_iSrcColorFormat == JXR_OCF_RGBE)?4:2):4; // Internal calculations data range
  m_iExMBSize = 256 * m_iUnitSize * m_iChannels;
  m_iInMBSize = 256 * m_iCUSize;
  m_iChromaSize = m_iLumaSize = m_iInMBSize;

  if(iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)
  {
    m_cRoi.width = 8;
    m_iChromaBlock = 8;
    m_iChromaSize  = m_iInMBSize/2;
    if(iColorFormat == JXR_ICF_YUV420)
    {
      m_cRoi.height = 8;
      m_iChromaBlock  = 4;
      m_iChromaSize   = m_iInMBSize/4;
    }
  }

  if(m_bTilingFlag && !m_bHardTilingFlag)
    m_bSoftTiling = 1;

  // Maximum tiled allocation range
  for(i = 0; i < m_iTilesHeight; i++)
    m_iMaxTileMBHeight = (m_iMaxTileMBHeight < m_iTileMBHeight[i])?m_iTileMBHeight[i]:m_iMaxTileMBHeight;

  for(i = 0; i < m_iTilesWidth; i++)
    m_iMaxTileMBWidth = (m_iMaxTileMBWidth < m_iTileMBWidth[i])?m_iTileMBWidth[i]:m_iMaxTileMBWidth;

  for(i = 1; i < m_iTilesTotal; i++)
  {
    iTemp = (Ipp32u)(m_IndexTable[i] - m_IndexTable[i-1]);
    m_iMaxTileBitstream = (Ipp32u)((iTemp > m_iMaxTileBitstream)?iTemp:m_iMaxTileBitstream);
  }

  // Allocate general memory
  iAllocSize += sizeof(PredictorInf) * m_iMaxTileMBWidth * m_iChannels * m_iTileThreads * 2; // Predictor info
  iAllocSize += sizeof(QuantInf) * m_iChannels; // Quantizers Info DC
  if(iBands != JXR_SB_DCONLY)
  {
    iAllocSize += sizeof(QuantInf) * m_iChannels; // Quantizers Info LP
    if(iBands != JXR_SB_NOHP)
      iAllocSize += sizeof(QuantInf) * m_iChannels; // Quantizers Info HP
  }
  iAllocSize += ((m_bAlphaFlag)?iHuffSize * 2:iHuffSize) * m_iTileThreads; // Adaptive huffman
  iAllocSize += m_iExMBSize * m_iMBTotalWidth; // Color convertion buffer
/*
  // Image allocation
  Ipp32u iMaxAlloc = 2 * m_iInMBSize * (m_iMBTotalWidth + 1) * m_iChannels;
  if(iMaxAlloc < JXR_MAX_MEMORY) iMaxAlloc = JXR_MAX_MEMORY;
*/
  iTemp = (m_iMBTotalWidth + 1) * (m_iMBTotalHeight + 1);
  iImageSize = m_iLumaSize * iTemp * ((m_bAlphaFlag)?2:1) + m_iChromaSize * iTemp * (iChannels - 1);
  iAllocSize += iImageSize; // Full-frame buffer

  if(m_pImagePlane->bResampling)
  {
    iResSize = m_iLumaSize * m_iMaxTileMBWidth * 16 * 2 * m_iTileThreads;
    iAllocSize += iResSize; // Resampling buffer
  }

  m_pAllocBuffer = pAllocPtr = (Ipp8u*)ippsMalloc_8u(iAllocSize);
  if(m_pAllocBuffer == 0)
    return ExcStatusFail;
  ippsZero_8u(m_pAllocBuffer, iAllocSize);

  m_pCCBuf = pAllocPtr; pAllocPtr += m_iExMBSize * m_iMBTotalWidth;

  m_iHPQuantizerIndex = m_iLPQuantizerIndex = 0;
  initQuantizers(&pAllocPtr, m_pImagePlane, m_pQuantDC, m_pQuantLP, m_pQuantHP);
  if(m_bAlphaFlag)
  {
    m_pImagePlane = &m_ImagePlane[JXR_PLANE_ALPHA];
    initQuantizers(&pAllocPtr, m_pImagePlane, m_pQuantDC + iChannels, m_pQuantLP + iChannels, m_pQuantHP + iChannels);
    m_pImagePlane = &m_ImagePlane[JXR_PLANE_PRIMARY];
  }

  m_pImgBuffer  = pAllocPtr; pAllocPtr += iImageSize;
  m_pResBuffer  = pAllocPtr; pAllocPtr += iResSize;
  m_pHuffBuffer = pAllocPtr; pAllocPtr += (((m_bAlphaFlag)?iHuffSize * 2:iHuffSize) * m_iTileThreads);
  m_pPredBuffer = pAllocPtr; pAllocPtr += (sizeof(PredictorInf) * m_iMaxTileMBWidth * m_iChannels * m_iTileThreads * 2);

  return ExcStatusOk;
} // CJPEGXRDecoder::initDecoder()


// Tile decoder allocation and init
ExcStatus CJPEGXRTileDecoder::initDecoder(CJPEGXRDecoder* pParent)
{
  Ipp8u* pAllocPtr;
  Ipp32u m_iChannels = pParent->m_iChannels;
  Ipp32u m_iCUSize = pParent->m_iCUSize;
  Ipp32u iChannels;
  Ipp32s iHuffSize;
  Ipp32u iShift;
  Ipp32u iImgShift;
  Ipp32u iStep;
  Ipp32u iTemp;
  Ipp32u i;

  ippiVLCGetStateSize_JPEGXR(&iHuffSize);

  m_pParent = pParent;
  m_pImagePlane    = &pParent->m_ImagePlane[JXR_PLANE_PRIMARY];
  m_pImagePlanePCT = &pParent->m_ImagePlane[JXR_PLANE_PRIMARY];
  iChannels = m_pImagePlane->iChannels;

  if(pParent->m_bPipeArrayInit)
    m_iPipeThreads = pParent->m_pPipeArray[m_pParent->m_iInstances];
  else if(pParent->m_iThreadMode == 2)
    m_iPipeThreads = pParent->m_iPipeLength;
  else
    m_iPipeThreads = pParent->m_iPipeThreads;

  if(m_pParent->m_bIndexFlag)
    iTemp = m_pParent->m_iMaxTileBitstream;
  else
    iTemp = DEC_MIN_BUFLEN;

  for(i = 0; i < pParent->m_iNumBands; i++)
  {
    m_Stream[i].Attach(*pParent->m_in);
    m_Stream[i].Init(iTemp);
    m_Stream[i].Seek(m_pParent->m_iFirstOffset, UIC::BaseStreamInput::Beginning);
  }

  m_pStream = &m_Stream[0];
  m_pDst = m_pParent->m_pData;

  pAllocPtr = m_pParent->m_pImgBuffer;
  iTemp  = (m_pParent->m_iMBTotalWidth + 1) * (m_pParent->m_iMBTotalHeight + 1);
  iShift = m_pParent->m_iLumaSize * iTemp;
  iStep  = (m_pParent->m_iMBTotalWidth + 1) * m_iCUSize * m_pParent->m_lRoi.width;
  iImgShift = m_pParent->m_lRoi.width/2 * m_iCUSize + iStep * m_pParent->m_lRoi.height/2;
  for(i = 0; i < iChannels; i++)
  {
    m_ImgBuf[i].pBuf    = (Ipp32s*)pAllocPtr; pAllocPtr += iShift;
    m_ImgBuf[i].pImgBuf = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pBuf + iImgShift);
    m_ImgBuf[i].pHufPtr = m_ImgBuf[i].pPCTPtr = m_ImgBuf[i].pCCPtr = m_ImgBuf[i].pImgBuf;
    m_ImgBuf[i].iStep   = iStep;
    iShift = m_pParent->m_iChromaSize * iTemp;
    iStep  = (m_pParent->m_iMBTotalWidth + 1) * m_iCUSize * m_pParent->m_cRoi.width;
    iImgShift = m_pParent->m_cRoi.width/2 * m_iCUSize + iStep * m_pParent->m_cRoi.height/2;
  }
  if(m_pParent->m_bAlphaFlag)
  {
    m_ImgBuf[iChannels].pBuf    = (Ipp32s*)pAllocPtr; pAllocPtr += m_pParent->m_iLumaSize * iTemp;
    m_ImgBuf[iChannels].pImgBuf = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pBuf + 8 * m_iCUSize + ((m_pParent->m_iMBTotalWidth + 1) * 16) * 8 * m_iCUSize);
    m_ImgBuf[iChannels].pHufPtr = m_ImgBuf[i].pPCTPtr = m_ImgBuf[i].pCCPtr = m_ImgBuf[i].pImgBuf;
    m_ImgBuf[iChannels].iStep   = (m_pParent->m_iMBTotalWidth + 1) * m_iCUSize * 16;
  }

  if(m_pImagePlane->bResampling)
  {
    iShift = m_pParent->m_iLumaSize * m_pParent->m_iMaxTileMBWidth * 16;
    pAllocPtr = m_pParent->m_pResBuffer;
    pAllocPtr += iShift * 2 * m_pParent->m_iInstances;

    m_pResU = (Ipp32s*)pAllocPtr; pAllocPtr += iShift;
    m_pResV = (Ipp32s*)pAllocPtr; pAllocPtr += iShift;
  }

  // Init Adaptive Huffman
  pAllocPtr = m_pParent->m_pHuffBuffer;
  pAllocPtr += (((m_pParent->m_bAlphaFlag)?iHuffSize * 2:iHuffSize) * m_pParent->m_iInstances);
  m_pHuffmanPlane->pHuffState = (IppiVLCState_JPEGXR*)pAllocPtr; pAllocPtr += (iHuffSize);
  ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pAdaptCBP,      sizeof(AdaptiveHuffman));
  ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pAdaptBlockCBP, sizeof(AdaptiveHuffman));
  ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pAdaptDCY,      sizeof(AdaptiveHuffman));
  ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pAdaptDCUV,     sizeof(AdaptiveHuffman));

  if(m_pParent->m_bAlphaFlag)
  {
    m_pHuffmanPlane = &m_HuffmanPlane[JXR_PLANE_ALPHA];
    m_pHuffmanPlane->pHuffState = (IppiVLCState_JPEGXR*)pAllocPtr; pAllocPtr += (iHuffSize);
    ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pAdaptCBP,      sizeof(AdaptiveHuffman));
    ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pAdaptBlockCBP, sizeof(AdaptiveHuffman));
    ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pAdaptDCY,      sizeof(AdaptiveHuffman));
    ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pAdaptDCUV,     sizeof(AdaptiveHuffman));
    m_pHuffmanPlane = &m_HuffmanPlane[JXR_PLANE_PRIMARY];
  }

  // Allocate Predictor Info
  pAllocPtr = m_pParent->m_pPredBuffer;
  pAllocPtr += (sizeof(PredictorInf) * m_pParent->m_iMaxTileMBWidth * m_iChannels * 2 * m_pParent->m_iInstances);
  initPredictor(&pAllocPtr, m_iChannels, m_pParent->m_iMaxTileMBWidth, m_pPred, m_pPredPrev);

  // Increase initialized decoders counter
  m_pParent->m_iInstances++;

  return ExcStatusOk;
} // CJPEGXRTileDecoder::initDecoder()


// Decoder reset for tile decoding
ExcStatus CJPEGXRTileDecoder::resetDecoder(void)
{
#ifdef USE_TBB
  tbb::spin_mutex::scoped_lock lock;
#endif
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u m_iChannels  = m_pParent->m_iChannels;
  Ipp32u m_iCUSize    = m_pParent->m_iCUSize;
  Ipp32u iShift;
  Ipp64u iOffset;
  Ipp32u iTileIndex;
  Ipp8u  iLE;
  Ipp8u  iTE;
  Ipp32u i;

  m_iCurTileMBRow = m_iCurTileMBColumn = 0;

  if(m_pParent->m_bIndexFlag)
  {
    for(i = 1; i <= m_iTileRow; i++)
      m_iCurTileMBRow += m_pParent->m_iTileMBHeight[i - 1];

    for(i = 1; i <= m_iTileColumn; i++)
      m_iCurTileMBColumn += m_pParent->m_iTileMBWidth[i - 1];

    // Move dst pointer to tile position
    iLE = (m_iCurTileMBColumn)?m_pParent->m_iLeftExtra:0;
    iTE = (m_iCurTileMBRow)?m_pParent->m_iTopExtra:0;

    m_pDst = m_pParent->m_pData + ((16 * m_iCurTileMBRow - iTE) * m_pParent->m_iStep +
      (16 * m_iCurTileMBColumn - iLE) * ((m_pParent->m_bFakeAlpha)?m_iChannels+1:m_iChannels) * m_pParent->m_iUnitSize);

    // Set bitstream to tile position
    iTileIndex = (m_pParent->m_iTilesWidth * m_iTileRow + m_iTileColumn) * m_pParent->m_iNumTileBands;
#ifdef USE_TBB
    lock.acquire(m_pParent->m_Mutex);
#endif
    for(i = 0; i < m_pParent->m_iNumBands; i++)
    {
      iOffset = m_pParent->m_iFirstOffset + m_pParent->m_IndexTable[iTileIndex + i];
      RET_(m_Stream[i].Seek(iOffset, BaseStreamInput::Beginning));
    }
#ifdef USE_TBB
    lock.release();
#endif

    // Move buffer pointer to tile position
    for(i = 0; i < m_iChannels; i++)
    {
      IppiSize roi = (i)?m_pParent->m_cRoi:m_pParent->m_lRoi;
      iShift = roi.height/2 * m_ImgBuf[i].iStep + roi.width/2 * m_iCUSize;
      iShift += roi.height * m_iCurTileMBRow * m_ImgBuf[i].iStep + roi.width * m_iCurTileMBColumn * m_iCUSize;
      m_ImgBuf[i].pImgBuf = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pBuf + iShift);
    }
  }

  // Reset Decoding Context
  if(iColorFormat == JXR_ICF_Y || iColorFormat == JXR_ICF_NCH || iColorFormat == JXR_ICF_YUVK)
    m_pHuffmanPlane->pAdaptBlockCBP.iSymbols = 5;
  else
    m_pHuffmanPlane->pAdaptBlockCBP.iSymbols = 9;
  m_pHuffmanPlane->pAdaptCBP.iSymbols  = 5;
  m_pHuffmanPlane->pAdaptDCY.iSymbols  = 7;
  m_pHuffmanPlane->pAdaptDCUV.iSymbols = 7;
  m_pHuffmanPlane->pAdaptCBP.bInitialize      = 0;
  m_pHuffmanPlane->pAdaptBlockCBP.bInitialize = 0;
  m_pHuffmanPlane->pAdaptDCY.bInitialize      = 0;
  m_pHuffmanPlane->pAdaptDCUV.bInitialize     = 0;
  adaptHuffman(&m_pHuffmanPlane->pAdaptCBP);
  adaptHuffman(&m_pHuffmanPlane->pAdaptBlockCBP);
  adaptHuffman(&m_pHuffmanPlane->pAdaptDCY);
  adaptHuffman(&m_pHuffmanPlane->pAdaptDCUV);
  ippiVLCInit_JPEGXR(m_pHuffmanPlane->pHuffState);

  // reset bit reduction models
  ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pModelDC, sizeof(AdaptiveModel));
  m_pHuffmanPlane->pModelDC.iBand = JXR_BT_DC;
  m_pHuffmanPlane->pModelDC.iModelBits[0] = m_pHuffmanPlane->pModelDC.iModelBits[1] = 8;

  ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pModelLP, sizeof(AdaptiveModel));
  m_pHuffmanPlane->pModelLP.iBand = JXR_BT_LP;
  m_pHuffmanPlane->pModelLP.iModelBits[0] = m_pHuffmanPlane->pModelLP.iModelBits[1] = 4;

  ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pModelHP, sizeof(AdaptiveModel));
  m_pHuffmanPlane->pModelHP.iBand = JXR_BT_HP;

  // reset CBP models
  m_pHuffmanPlane->iCBPCountMax = m_pHuffmanPlane->iCBPCountZero = 1;
  m_pHuffmanPlane->pCBPModel.iCount0[0] = m_pHuffmanPlane->pCBPModel.iCount0[1] = -4;
  m_pHuffmanPlane->pCBPModel.iCount1[0] = m_pHuffmanPlane->pCBPModel.iCount1[1] = 4;
  m_pHuffmanPlane->pCBPModel.iState[0] = m_pHuffmanPlane->pCBPModel.iState[1] = 0;

  if(m_pParent->m_bAlphaFlag)
  {
    m_pImagePlane = &m_pParent->m_ImagePlane[JXR_PLANE_ALPHA];
    m_pHuffmanPlane = &m_HuffmanPlane[JXR_PLANE_ALPHA];
    iColorFormat = m_pImagePlane->iColorFormat;

    // Reset Decoding Context
    if(iColorFormat == JXR_ICF_Y || iColorFormat == JXR_ICF_NCH || iColorFormat == JXR_ICF_YUVK)
      m_pHuffmanPlane->pAdaptBlockCBP.iSymbols = 5;
    else
      m_pHuffmanPlane->pAdaptBlockCBP.iSymbols = 9;
    m_pHuffmanPlane->pAdaptCBP.iSymbols  = 5;
    m_pHuffmanPlane->pAdaptDCY.iSymbols  = 7;
    m_pHuffmanPlane->pAdaptDCUV.iSymbols = 7;
    m_pHuffmanPlane->pAdaptCBP.bInitialize      = 0;
    m_pHuffmanPlane->pAdaptBlockCBP.bInitialize = 0;
    m_pHuffmanPlane->pAdaptDCY.bInitialize      = 0;
    m_pHuffmanPlane->pAdaptDCUV.bInitialize     = 0;
    adaptHuffman(&m_pHuffmanPlane->pAdaptCBP);
    adaptHuffman(&m_pHuffmanPlane->pAdaptBlockCBP);
    adaptHuffman(&m_pHuffmanPlane->pAdaptDCY);
    adaptHuffman(&m_pHuffmanPlane->pAdaptDCUV);
    ippiVLCInit_JPEGXR(m_pHuffmanPlane->pHuffState);

    // reset bit reduction models
    ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pModelDC, sizeof(AdaptiveModel));
    m_pHuffmanPlane->pModelDC.iBand = JXR_BT_DC;
    m_pHuffmanPlane->pModelDC.iModelBits[0] = m_pHuffmanPlane->pModelDC.iModelBits[1] = 8;

    ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pModelLP, sizeof(AdaptiveModel));
    m_pHuffmanPlane->pModelLP.iBand = JXR_BT_LP;
    m_pHuffmanPlane->pModelLP.iModelBits[0] = m_pHuffmanPlane->pModelLP.iModelBits[1] = 4;

    ippsZero_8u((Ipp8u*)&m_pHuffmanPlane->pModelHP, sizeof(AdaptiveModel));
    m_pHuffmanPlane->pModelHP.iBand = JXR_BT_HP;

    // reset CBP models
    m_pHuffmanPlane->iCBPCountMax = m_pHuffmanPlane->iCBPCountZero = 1;
    m_pHuffmanPlane->pCBPModel.iCount0[0] = m_pHuffmanPlane->pCBPModel.iCount0[1] = -4;
    m_pHuffmanPlane->pCBPModel.iCount1[0] = m_pHuffmanPlane->pCBPModel.iCount1[1] = 4;
    m_pHuffmanPlane->pCBPModel.iState[0] = m_pHuffmanPlane->pCBPModel.iState[1] = 0;

    m_pImagePlane = &m_pParent->m_ImagePlane[JXR_PLANE_PRIMARY];
    m_pHuffmanPlane = &m_HuffmanPlane[JXR_PLANE_PRIMARY];
  }

  return ExcStatusOk;
} // CJPEGXRTileDecoder::resetDecoder()


ExcStatus CJPEGXRDecoder::KillDecoder(void)
{
  ippsFree(m_iTileMBWidth);
  ippsFree(m_pAllocBuffer);
  if(m_bIndexFlag)
    ippsFree(m_IndexTable);

  m_bFirstLoop     = 1;

  return ExcStatusOk;
} // CJPEGXRDecoder::KillDecoder()


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
    pHuff->pTable = iHuffmanLookup5[iLevel];
    pHuff->pDelta = iHuffmanDelta5;
  }
  else if(iSymbols == 7)
  {
    pHuff->pTable = iHuffmanLookup7[iLevel];
    pHuff->pDelta = iHuffmanDelta7;
  }
  else if(iSymbols == 9)
  {
    pHuff->pTable = iHuffmanLookup9[iLevel];
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


static Ipp32s decodeDC(AdaptiveHuffman* pHuffman, CBitStreamInput* pStream)
{
  Ipp32s iDC;
  Ipp32s iFixLen;
  Ipp8u  iByte;
  Ipp32u iDWord;

  RET_(pStream->GetHuffman(pHuffman->pTable, iDC));
  pHuffman->iDiscr += pHuffman->pDelta[iDC];

  if(iDC < 2)
    iDC = iDC + 2;
  else if(iDC < 6)
  {
    iFixLen = iDCLength[iDC];
    RET_(pStream->ReadBits((Ipp8u)iFixLen, iByte));
    iDC = iDCRemap[iDC] + iByte;
  }
  else
  {
    RET_(pStream->ReadBits(4, iByte));
    iFixLen = 4 + iByte;
    if(iFixLen == 19)
    {
      RET_(pStream->ReadBits(2, iByte));
      iFixLen += iByte;
      if(iFixLen == 22)
      {
        RET_(pStream->ReadBits(3, iByte));
        iFixLen += iByte;
      }
    }
    RET_(pStream->ReadBits((Ipp8u)iFixLen, iDWord));
    iDC = 2 + (1 << iFixLen);
    iDC += iDWord;
  }

  return (iDC - 1);
} // decodeDC()


ExcStatus CJPEGXRTileDecoder::readMacroblockDC(void)
{
  Ipp32s iDC, iDCStatus;
  Ipp32s iLaplasMean[2] = {0,0};
  Ipp32u iModelBits = m_pHuffmanPlane->pModelDC.iModelBits[0];
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp8u  iByte;
  Ipp32u iDWord;
  Ipp32u i, k = 0;

  ippsZero_8u((Ipp8u*)&m_iDCLPTable[0][0], sizeof(Ipp32u) * 16 * iChannels);

  if(m_pParent->m_bFrequencyFlag)
    m_pStream = &m_Stream[0];

  if(iColorFormat == JXR_ICF_Y ||
     iColorFormat == JXR_ICF_YUVK ||
     iColorFormat == JXR_ICF_NCH)
  {
    for (i = 0; i < iChannels; i++)
    {
      iDC = 0;
      RET_(m_pStream->ReadBits(1, iByte));
      if(iByte)
      {
        iDC = decodeDC(&m_pHuffmanPlane->pAdaptDCY, m_pStream);
        iLaplasMean[k]++;
      }
      if(iModelBits)
      {
        RET_(m_pStream->ReadBits((Ipp8u)iModelBits, iDWord));
        iDC = (iDC << iModelBits) | iDWord;
      }

      if(iDC)
      {
        RET_(m_pStream->ReadBits(1, iByte));
        if(iByte) iDC = -iDC;
      }

      m_iDCLPTable[i][0] = iDC;

      k = 1;
      iModelBits = m_pHuffmanPlane->pModelDC.iModelBits[1];
    }
  }
  else
  {
    iDC = 0;
    RET_(m_pStream->GetHuffman(pVLCLookupDCType, iDCStatus));

    // Luma DC
    if(iDCStatus & 4)
    {
      iDC = decodeDC(&m_pHuffmanPlane->pAdaptDCY, m_pStream);
      iLaplasMean[0]++;
    }
    if(iModelBits)
    {
      RET_(m_pStream->ReadBits((Ipp8u)iModelBits, iDWord));
      iDC = (iDC << iModelBits) | iDWord;
    }

    if(iDC)
    {
      RET_(m_pStream->ReadBits(1, iByte));
      if(iByte) iDC = -iDC;
    }

    m_iDCLPTable[0][0] = iDC;
    iDC = 0;

    // Chroma DC
    iModelBits = m_pHuffmanPlane->pModelDC.iModelBits[1];

    if(iDCStatus & 2)
    {
      iDC = decodeDC(&m_pHuffmanPlane->pAdaptDCUV, m_pStream);
      iLaplasMean[1]++;
    }
    if(iModelBits)
    {
      RET_(m_pStream->ReadBits((Ipp8u)iModelBits, iDWord));
      iDC = (iDC << iModelBits) | iDWord;
    }

    if(iDC)
    {
      RET_(m_pStream->ReadBits(1, iByte));
      if(iByte) iDC = -iDC;
    }

    m_iDCLPTable[1][0] = iDC;
    iDC = 0;

    if(iDCStatus & 1)
    {
      iDC = decodeDC(&m_pHuffmanPlane->pAdaptDCUV, m_pStream);
      iLaplasMean[1]++;
    }
    if(iModelBits)
    {
      RET_(m_pStream->ReadBits((Ipp8u)iModelBits, iDWord));
      iDC = (iDC << iModelBits) | iDWord;
    }

    if(iDC)
    {
      RET_(m_pStream->ReadBits(1, iByte));
      if(iByte) iDC = -iDC;
    }

    m_iDCLPTable[2][0] = iDC;
  }

  updateModel(iLaplasMean, iColorFormat, iChannels, &m_pHuffmanPlane->pModelDC);

  if(m_pParent->m_bFrequencyFlag && m_bResetContext)
  {
    adaptHuffman(&m_pHuffmanPlane->pAdaptDCY);
    adaptHuffman(&m_pHuffmanPlane->pAdaptDCUV);
  }

  return ExcStatusOk;
} // CJPEGXRDecoder::readMacroblockDC()


static Ipp32s readLPCBP(Ipp32u iChannels, Ipp32s& m_iCBPCountMax, Ipp32s& m_iCBPCountZero, CBitStreamInput* pStream)
{
  Ipp32s iCBP = 0;
  Ipp32s iCountMax = m_iCBPCountMax;
  Ipp32s iCountZero = m_iCBPCountZero;
  Ipp32s iMax = iChannels * 4 - 5;
  Ipp16u iBuffer;

  if(iCountZero <= 0 || iCountMax < 0)
  {
    RET_(pStream->ReadBits(1, iBuffer));
    if(iBuffer)
    {
      iCBP = 1;
      RET_(pStream->ReadBits((Ipp8u)(iChannels - 1), iBuffer));
      if(iBuffer)
      {
        iCBP = iBuffer * 2;
        RET_(pStream->ReadBits(1, iBuffer));
        iCBP += iBuffer;
      }
    }
    if(iCountMax < iCountZero)
      iCBP = iMax - iCBP;
  }
  else
  {
    RET_(pStream->ReadBits((Ipp8u)iChannels, iBuffer));
    iCBP = iBuffer;
  }

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

  return iCBP;
} // readLPCBP()


ExcStatus CJPEGXRTileDecoder::readMacroblockLP(void)
{
  Ipp32u iModelBits = m_pHuffmanPlane->pModelLP.iModelBits[0];
  Ipp32s iLaplasMean[2] = {0, 0};
  Ipp32s iSig[2] = {1, -1};
  Ipp32s *pLaplasMean = iLaplasMean;
  Ipp32s iCBP = 0;
  Ipp32u iNumNonZero;
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u bUV42X = (iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)?1:0;
  Ipp32s i42XCoeffs[14];
  Ipp8u  iByte;
  Ipp32u i, j;

  if(m_pParent->m_bFrequencyFlag)
    m_pStream = &m_Stream[1];

  Ipp8u *pStream;
  Ipp32u iBitBuf;
  Ipp32u iBitValid;

  if(m_bResetTotals)
    ippiVLCScanReset_JPEGXR(m_pHuffmanPlane->pHuffState, ippVLCScanRaster);

  if(iColorFormat == JXR_ICF_YUV444)
      iCBP = readLPCBP(iChannels, m_pHuffmanPlane->iCBPCountMax, m_pHuffmanPlane->iCBPCountZero, m_pStream);
  else if(iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)
      iCBP = readLPCBP(2, m_pHuffmanPlane->iCBPCountMax, m_pHuffmanPlane->iCBPCountZero, m_pStream);
  else
  {
    for(i = 0; i < iChannels; i++)
    {
      RET_(m_pStream->ReadBits(1, iByte));
      iCBP |= iByte << i;
    }
  }

  for(i = 0; i < iChannels; i++, iCBP >>= 1)
  {
    Ipp32s *pBlockDC = m_iDCLPTable[i];

    if(iCBP & 1)
    {
      m_pStream->GetStream(&pStream, &iBitBuf, &iBitValid);
      ippiVLCDecodeDC444_JPEGXR_1u32s((const Ipp8u**)&pStream, &iBitBuf, &iBitValid,
        pBlockDC, IppBool(i > 0), &iNumNonZero, m_pHuffmanPlane->pHuffState);
      RET_(m_pStream->UpdateStream(pStream, iBitBuf, iBitValid))
      *pLaplasMean += iNumNonZero;
    }

    if(iModelBits)
    {
      Ipp32u iMBits;
      Ipp32u iFlag;
      Ipp32s pBDC;

      for(j = 1; j < 16; j++)
      {
        RET_(m_pStream->PeekBits((Ipp8u)(iModelBits + 1), iBitBuf));
        iMBits = iModelBits;
        iFlag = iBitBuf & 1;
        pBDC = pBlockDC[j];
        pBDC <<= iModelBits;
        iBitBuf >>= 1;
        if(iBitBuf)
        {
          iMBits += !pBDC;
          pBDC += iBitBuf * iSig[(((Ipp32u)pBDC) >> 31 ) | (!pBDC & iFlag)];
        }
        pBlockDC[j] = pBDC;
        RET_(m_pStream->Flush((Ipp8u)iMBits));
      }
    }
    pLaplasMean = iLaplasMean + 1;
    iModelBits = m_pHuffmanPlane->pModelLP.iModelBits[1];

    if(bUV42X)
    {
      Ipp8u* pRemap;
      Ipp32u iCount;
      Ipp32u iMBCount;
      iCBP >>= 1;

      if(iColorFormat == JXR_ICF_YUV422)
      {
        pRemap = (Ipp8u*)&iLP4XXRemap[0];
        iCount = 14;
        iMBCount = 8;
      }
      else
      {
        pRemap = (Ipp8u*)&iLP4XXRemap[1];
        iCount = 6;
        iMBCount = 4;
      }

      if(iCBP & 1)
      {
        m_pStream->GetStream(&pStream, &iBitBuf, &iBitValid);
        if(iColorFormat == JXR_ICF_YUV422)
          ippiVLCDecodeDC422_JPEGXR_1u32s((const Ipp8u**)&pStream, &iBitBuf, &iBitValid,
            i42XCoeffs, &iNumNonZero, m_pHuffmanPlane->pHuffState);
        else
          ippiVLCDecodeDC420_JPEGXR_1u32s((const Ipp8u**)&pStream, &iBitBuf, &iBitValid,
            i42XCoeffs, &iNumNonZero, m_pHuffmanPlane->pHuffState);
        RET_(m_pStream->UpdateStream(pStream, iBitBuf, iBitValid))
        *pLaplasMean += iNumNonZero;

        for(j = 0; j < iCount; j++)
          m_iDCLPTable[(j & 1) + 1][pRemap[j >> 1]] = i42XCoeffs[j];
      }

      if(iModelBits)
      {
        Ipp32u iMBits;
        Ipp32u iFlag;
        Ipp32s pBDC;

        for(j = 1; j < iMBCount; j++)
        {
          RET_(m_pStream->PeekBits((Ipp8u)(iModelBits + 1), iBitBuf));
          iMBits = iModelBits;
          iFlag = iBitBuf & 1;
          pBDC = m_iDCLPTable[1][j] <<= iModelBits;
          iBitBuf >>= 1;
          if(iBitBuf)
          {
            iMBits += !pBDC;
            pBDC += iBitBuf * iSig[(((Ipp32u)pBDC) >> 31 ) | (!pBDC & iFlag)];
          }
          m_iDCLPTable[1][j] = pBDC;
          RET_(m_pStream->Flush((Ipp8u)iMBits));

          RET_(m_pStream->PeekBits((Ipp8u)(iModelBits + 1), iBitBuf));
          iMBits = iModelBits;
          iFlag = iBitBuf & 1;
          pBDC = m_iDCLPTable[2][j] <<= iModelBits;
          iBitBuf >>= 1;
          if(iBitBuf)
          {
            iMBits += !pBDC;
            pBDC += iBitBuf * iSig[(((Ipp32u)pBDC) >> 31 ) | (!pBDC & iFlag)];
          }
          m_iDCLPTable[2][j] = pBDC;
          RET_(m_pStream->Flush((Ipp8u)iMBits));
        }
      }

      break;
    }
  }

  updateModel(iLaplasMean, iColorFormat, iChannels, &m_pHuffmanPlane->pModelLP);

  if(m_bResetContext)
  {
    adaptHuffman(&m_pHuffmanPlane->pAdaptDCY);
    adaptHuffman(&m_pHuffmanPlane->pAdaptDCUV);
    ippiVLCAdapt_JPEGXR(m_pHuffmanPlane->pHuffState, ippVLCAdaptLowpass);
  }

  return ExcStatusOk;
} // CJPEGXRDecoder::readMacroblockLP()


static ExcStatus decodeHPCBP(AdaptiveHuffman* pAdaptCBP, AdaptiveHuffman* pAdaptBlockCBP,
                      Ipp16u* pCBP, CBitStreamInput* pStream, Ipp32u iColorFormat, Ipp32u iChannels)
{
  Ipp32s iNumCBP, iSymbol;
  Ipp32u iBitBuf;
  Ipp32u iCode, iBlockCBP, iTemp;
  Ipp32u i, j, k;

  iChannels = (iColorFormat == JXR_ICF_YUVK || iColorFormat == JXR_ICF_NCH)?iChannels:1;
  pCBP[1] = pCBP[2] = 0;

  for (i = 0; i < iChannels; i++)
  {
    pCBP[i] = 0;

    RET_(pStream->GetHuffman(pAdaptCBP->pTable, iSymbol));
    pAdaptCBP->iDiscr += pAdaptCBP->pDelta[iSymbol];

    RET_(pStream->PeekBits(3, iBitBuf));
    if(iSymbol == 2)
      iTemp = iLengthTable2[iBitBuf];
    else
      iTemp = iLengthTable1[iSymbol];

    iNumCBP = iCountCBP[iSymbol*8 + iBitBuf];
    if(iTemp)
    {
      RET_(pStream->Flush((Ipp8u)iTemp));
    }

    for(j = 0; j < 4; j++)
    {
      if(iNumCBP & (1 << j))
      {
        iBlockCBP = 0;
        RET_(pStream->GetHuffman(pAdaptBlockCBP->pTable, iSymbol));
        pAdaptBlockCBP->iDiscr += pAdaptBlockCBP->pDelta[iSymbol];
        iTemp = iSymbol + 1;

        if(iTemp >= 6)
        {
          RET_(pStream->PeekBits(2, iBitBuf));
          RET_(pStream->Flush(iLengthTable3[iBitBuf]));
          iBlockCBP = iValueCBP[iBitBuf];

          RET_(pStream->PeekBits(2, iBitBuf));
          if(iTemp >= 9)
          {
            iTemp = iValueUpdate[iBitBuf];
            RET_(pStream->Flush(iLengthTable3[iBitBuf]));
          }
          iTemp -= 6;
        }

        iCode = iExtraCBPOffset[iTemp];
        if(iExtraBits[iTemp])
        {
          RET_(pStream->ReadBits(iExtraBits[iTemp], iBitBuf));
          iCode += iBitBuf;
        }
        iBlockCBP += iEValueCBP[iCode];

        if(iColorFormat == JXR_ICF_YUV444)
        {
          pCBP[0] |= ((iBlockCBP & 0xf) << (j * 4));

          for(k = 0; k < 2; k++)
          {
            if((iBlockCBP >> (k + 4)) & 0x01)
            {
              RET_(pStream->GetHuffman(pVLCLookupCBPCh, iSymbol));
              RET_(pStream->PeekBits(3, iBitBuf));
              if(iSymbol == 1)
                iTemp = iLengthTable2[iBitBuf];
              else
                iTemp = iLengthTable1[iSymbol + 1];

              iCode = iCountCBP[(iSymbol + 1)*8 + iBitBuf];
              if(iTemp)
              {
                RET_(pStream->Flush((Ipp8u)iTemp));
              }

              pCBP[k + 1] |= (iCode << (j * 4));
            }
          }
        }
        else if(iColorFormat == JXR_ICF_YUV422)
        {
          pCBP[0] |= ((iBlockCBP & 0xf) << (j * 4));

          for(k = 0; k < 2; k++)
          {
            if((iBlockCBP >> (k + 4)) & 0x01)
            {
              RET_(pStream->PeekBits(2, iBitBuf));
              iCode   = iCBP422Value[iBitBuf] << iCBP422Offset[j];
              RET_(pStream->Flush(iLengthTable3[iBitBuf]));

              pCBP[k + 1] |= iCode;
            }
          }
        }
        else if(iColorFormat == JXR_ICF_YUV420)
        {
          pCBP[0] |= ((iBlockCBP & 0xf) << (j * 4));
          pCBP[1] |= ((iBlockCBP >> 4) & 0x1) << j;
          pCBP[2] |= ((iBlockCBP >> 5) & 0x1) << j;
        }
        else
          pCBP[i] |= (iBlockCBP << (j * 4));
      }
    }
  }

  return ExcStatusOk;
} // decodeHPCBP()


static void predHPCBP(Ipp16u *pCBP, Ipp32u iMBColumn, Ipp32u iBorder, Ipp32u iChannels, Ipp32u iColorFormat,
                      CBPModel* pModel, PredictorInf** pPred, PredictorInf** pPredPrev)
{
  Ipp32u bLeft = (iBorder & JXR_BORDER_LEFT) ? 1 : 0;
  Ipp32u bTop  = (iBorder & JXR_BORDER_TOP) ? 1 : 0;
  Ipp32u bUV42X = (iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)?1:0;
  Ipp16u iCBP;
  Ipp32s iCount = 0;
  Ipp32u bChroma = 0;
  Ipp32u i;

  for(i = 0; i < iChannels; i++)
  {
    iCBP = pCBP[i];

    if(i == 0 || !bUV42X)
    {
      if(pModel->iState[bChroma] == 0)
      {
        if(bLeft)
        {
          if(bTop)
            iCBP ^= 1;
          else
            iCBP ^= (pPredPrev[i][iMBColumn].iCBP >> 10) & 1;
        }
        else
          iCBP ^= ((pPred[i][iMBColumn - 1].iCBP >> 5) & 1);

        iCBP ^= (0x02 & (iCBP << 1));
        iCBP ^= (0x10 & (iCBP << 3));
        iCBP ^= (0x20 & (iCBP << 1));

        iCBP ^= ((iCBP & 0x33)   << 2);
        iCBP ^= ((iCBP & 0xcc)   << 6);
        iCBP ^= ((iCBP & 0x3300) << 2);
      }
      else if (pModel->iState[bChroma] == 2)
        iCBP ^= 0xffff;
      iCount = 1;
    }
    else
    {
      if(iColorFormat == JXR_ICF_YUV422)
      {
        if(pModel->iState[1] == 0)
        {
          if(bLeft)
          {
            if(bTop)
              iCBP ^= 1;
            else
              iCBP ^= (pPredPrev[i][iMBColumn].iCBP >> 6) & 1;
          }
          else
            iCBP ^= ((pPred[i][iMBColumn - 1].iCBP >> 1) & 1);

          iCBP ^= ((iCBP & 0x1)  << 1);
          iCBP ^= ((iCBP & 0x3)  << 2);
          iCBP ^= ((iCBP & 0xc)  << 2);
          iCBP ^= ((iCBP & 0x30) << 2);
        }
        else if (pModel->iState[1] == 2)
          iCBP ^= 0xff;
        iCount = 2;
      }
      else if(iColorFormat == JXR_ICF_YUV420)
      {
        if(pModel->iState[1] == 0)
        {
          if(bLeft)
          {
            if(bTop)
              iCBP ^= 1;
            else
              iCBP ^= (pPredPrev[i][iMBColumn].iCBP >> 2) & 1;
          }
          else
            iCBP ^= ((pPred[i][iMBColumn - 1].iCBP >> 1) & 1);

          iCBP ^= (0x02 & (iCBP << 1));
          iCBP ^= ((iCBP & 0x3) << 2);
        }
        else if (pModel->iState[1] == 2)
          iCBP ^= 0xf;
        iCount = 4;
      }
    }

    iCount *= iUnitCount[iCBP & 0xf] + iUnitCount[(iCBP >> 4) & 0xf] + iUnitCount[(iCBP >> 8) & 0xf] + iUnitCount[iCBP >> 12];
    pPred[i][iMBColumn].iCBP = pCBP[i] = iCBP;
    updateModelCBP(iCount, bChroma, pModel);
    bChroma = 1;
  }
} // predHPCBP()


template<class TYPE>
static void predHP(Ipp32u iChannels, Ipp32u iColorFormat, Ipp32u iPredMode, ImgBuf* pImgBuf);


ExcStatus CJPEGXRTileDecoder::readMacroblockHP(void)
{
  IppiVLCScanType tScan = (m_iOrientation == 1) ? ippVLCScanVert : ippVLCScanHoriz;
  const Ipp8u *pCBlocks = iBlocks444;
  const Ipp8u *pTBlocks = iBlocks444;
  Ipp32u iTBlock = 16;
  Ipp32s iLaplasMean[2] = {0,0};
  Ipp32s *pLaplasMean = iLaplasMean;
  Ipp32s iModelBits = m_pHuffmanPlane->pModelHP.iModelBits[0];
  Ipp32s iTrim = m_iTrimFlex;
  Ipp32s iQP = 1, iQPHP, iQPFB;
  Ipp16u iCBP[JXR_MAX_CHANNELS];
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u iBands = m_pImagePlane->iBands;
  Ipp32u iACHOffset = (m_bAlphaProcess)?m_pParent->m_iACHOffset:0;
  IppiVLCState_JPEGXR *pHuffState = m_pHuffmanPlane->pHuffState;
  Ipp32u  iNonZero;
  IppBool iChroma = ippFalse;
  Ipp32u i, j;

  if(m_pParent->m_bFrequencyFlag)
    m_pStream = &m_Stream[2];

  Ipp8u *pStream,  *pStreamFB = NULL;
  Ipp32u iBitBuf,   iBitBufFB = 0;
  Ipp32u iBitValid, iBitValidFB = 0;

  if(m_bResetTotals)
  {
    ippiVLCScanReset_JPEGXR(pHuffState, ippVLCScanHoriz);
    ippiVLCScanReset_JPEGXR(pHuffState, ippVLCScanVert);
  }
  ippiVLCScanSet_JPEGXR(pHuffState, tScan);

  if(iColorFormat == JXR_ICF_YUV420)
    pCBlocks = iBlocks420;
  else if(iColorFormat == JXR_ICF_YUV422)
    pCBlocks = iBlocks422;

  RET_(decodeHPCBP(&m_pHuffmanPlane->pAdaptCBP, &m_pHuffmanPlane->pAdaptBlockCBP, iCBP, m_pStream, iColorFormat, iChannels));
  predHPCBP(iCBP, m_iMBColumn, m_iBorder, iChannels, iColorFormat,
    &m_pHuffmanPlane->pCBPModel, m_pPred + iACHOffset, m_pPredPrev + iACHOffset);

  m_pStream->GetStream(&pStream, &iBitBuf, &iBitValid);
  if(m_pParent->m_bFrequencyFlag && iBands < JXR_SB_NOFLEX)
    m_Stream[3].GetStream(&pStreamFB, &iBitBufFB, &iBitValidFB);

  for(i = 0; i < iChannels; i++)
  {
    Ipp32s iFlex = 0;
    Ipp16u iPattern = iCBP[i];
    Ipp32u iStep = m_ImgBuf[i + iACHOffset].iStep;
    if(iTrim < iModelBits && iBands < JXR_SB_NOFLEX)
      iFlex = iModelBits - iTrim;

    iQP = m_pParent->m_pQuantHP[i + iACHOffset][m_pParent->m_iHPQuantizerIndex].iQP;
    iQPHP = iQP << iModelBits;
    iQPFB = iQP << iTrim;

    if(m_pParent->m_iCUSize == 2)
    {
      Ipp16s *pDst;
      Ipp16s *pMB = (Ipp16s*)m_ImgBuf[i + iACHOffset].pHufPtr;

      for(j = 0; j < iTBlock; j++, iPattern >>= 1, pTBlocks += 2)
      {
        pDst = _PEL(Ipp16s, pMB, pTBlocks[0], pTBlocks[1], iStep);

        if(iPattern & 1)
        {
          ippiVLCDecode4x4_JPEGXR_1u16s_C1R((const Ipp8u**)&pStream, &iBitBuf, &iBitValid,
            (Ipp16s)iQPHP, pDst, iStep, iChroma, &iNonZero, pHuffState);
          *pLaplasMean += iNonZero;
        }
        else
        {
          Ipp16s* pDst1 = _NEXTROW(Ipp16s, pDst, iStep);
          Ipp16s* pDst2 = _NEXTROW(Ipp16s, pDst1, iStep);
          Ipp16s* pDst3 = _NEXTROW(Ipp16s, pDst2, iStep);
          pDst[1]  = pDst[2]  = pDst[3]  = 0;
          pDst1[0] = pDst1[1] = pDst1[2] = pDst1[3] = 0;
          pDst2[0] = pDst2[1] = pDst2[2] = pDst2[3] = 0;
          pDst3[0] = pDst3[1] = pDst3[2] = pDst3[3] = 0;
        }
        if(iFlex > 0)
        {
          if(!m_pParent->m_bFrequencyFlag)
            pStreamFB = pStream, iBitBufFB = iBitBuf, iBitValidFB = iBitValid;
          ippiFLCDecode4x4_JPEGXR_1u16s_C1IR((const Ipp8u**)&pStreamFB, &iBitBufFB, &iBitValidFB,
            pDst, iStep, iFlex, (Ipp16s)iQPFB, (IppBool)(iPattern & 1));
          if(!m_pParent->m_bFrequencyFlag)
            pStream = pStreamFB, iBitBuf = iBitBufFB, iBitValid = iBitValidFB;
        }
      }
    }
    else
    {
      Ipp32s *pDst;
      Ipp16s *pMB = (Ipp16s*)m_ImgBuf[i + iACHOffset].pHufPtr;
      for(j = 0; j < iTBlock; j++, iPattern >>= 1, pTBlocks += 2)
      {
        pDst = _PEL(Ipp32s, pMB, pTBlocks[0], pTBlocks[1], iStep);

        if(iPattern & 1)
        {
          ippiVLCDecode4x4_JPEGXR_1u32s_C1R((const Ipp8u**)&pStream, &iBitBuf, &iBitValid,
            iQPHP, pDst, iStep, iChroma, &iNonZero, pHuffState);
          *pLaplasMean += iNonZero;
        }
        else
        {
          Ipp32s* pDst1 = _NEXTROW(Ipp32s, pDst, iStep);
          Ipp32s* pDst2 = _NEXTROW(Ipp32s, pDst1, iStep);
          Ipp32s* pDst3 = _NEXTROW(Ipp32s, pDst2, iStep);
          pDst[1] =  pDst[2] =  pDst[3] = 0;
          pDst1[0] = pDst1[1] = pDst1[2] = pDst1[3] = 0;
          pDst2[0] = pDst2[1] = pDst2[2] = pDst2[3] = 0;
          pDst3[0] = pDst3[1] = pDst3[2] = pDst3[3] = 0;
        }
        if(iFlex > 0)
        {
          if(!m_pParent->m_bFrequencyFlag)
            pStreamFB = pStream, iBitBufFB = iBitBuf, iBitValidFB = iBitValid;
          ippiFLCDecode4x4_JPEGXR_1u32s_C1IR((const Ipp8u**)&pStreamFB, &iBitBufFB, &iBitValidFB,
            pDst, iStep, iFlex, iQPFB, (IppBool)(iPattern & 1));
          if(!m_pParent->m_bFrequencyFlag)
            pStream = pStreamFB, iBitBuf = iBitBufFB, iBitValid = iBitValidFB;
        }
      }
    }

    iChroma = ippTrue;
    iModelBits = m_pHuffmanPlane->pModelHP.iModelBits[1];
    pLaplasMean = iLaplasMean + 1;
    iTBlock = m_pParent->m_iChromaBlock;
    pTBlocks = pCBlocks;
  }
  RET_(m_pStream->UpdateStream(pStream, iBitBuf, iBitValid))
  if(m_pParent->m_bFrequencyFlag && iBands < JXR_SB_NOFLEX)
    RET_(m_Stream[3].UpdateStream(pStreamFB, iBitBufFB, iBitValidFB))

  updateModel(iLaplasMean, iColorFormat, iChannels, &m_pHuffmanPlane->pModelHP);

  if(m_pParent->m_iCUSize == 2)
    predHP<Ipp16s>(iChannels, iColorFormat, 2 - m_iOrientation, &m_ImgBuf[iACHOffset]);
  else
    predHP<Ipp32s>(iChannels, iColorFormat, 2 - m_iOrientation, &m_ImgBuf[iACHOffset]);

  if(m_bResetContext)
  {
    adaptHuffman(&m_pHuffmanPlane->pAdaptCBP);
    adaptHuffman(&m_pHuffmanPlane->pAdaptBlockCBP);
    ippiVLCAdapt_JPEGXR(pHuffState, ippVLCAdaptHighpass);
  }

  return ExcStatusOk;
} // CJPEGXRDecoder::readMacroblockHP()


template<class TYPE>
static void dequantLP(TYPE* Ptr0, Ipp32s* pDC, Ipp32u iMode, Ipp32u iStepMB, Ipp32s iQP)
{
  if(iMode == 0)
  {
    TYPE *Ptr4  = _NEARROW(TYPE, Ptr0, 4, iStepMB);
    TYPE *Ptr8  = _NEARROW(TYPE, Ptr0, 8, iStepMB);
    TYPE *Ptr12 = _NEARROW(TYPE, Ptr0,12, iStepMB);

    Ptr0 [ 4] = (TYPE)(pDC[ 2] * iQP);
    Ptr0 [ 8] = (TYPE)(pDC[ 1] * iQP);
    Ptr0 [12] = (TYPE)(pDC[ 9] * iQP);
    Ptr4 [ 0] = (TYPE)(pDC[ 8] * iQP);
    Ptr4 [ 4] = (TYPE)(pDC[10] * iQP);
    Ptr4 [ 8] = (TYPE)(pDC[11] * iQP);
    Ptr4 [12] = (TYPE)(pDC[ 3] * iQP);
    Ptr8 [ 0] = (TYPE)(pDC[ 4] * iQP);
    Ptr8 [ 4] = (TYPE)(pDC[14] * iQP);
    Ptr8 [ 8] = (TYPE)(pDC[15] * iQP);
    Ptr8 [12] = (TYPE)(pDC[ 7] * iQP);
    Ptr12[ 0] = (TYPE)(pDC[ 6] * iQP);
    Ptr12[ 4] = (TYPE)(pDC[12] * iQP);
    Ptr12[ 8] = (TYPE)(pDC[13] * iQP);
    Ptr12[12] = (TYPE)(pDC[ 5] * iQP);
  }
  else if(iMode == 1)
  {
    TYPE *Ptr4  = _NEARROW(TYPE, Ptr0, 4, iStepMB);
    TYPE *Ptr8  = _NEARROW(TYPE, Ptr0, 8, iStepMB);
    TYPE *Ptr12 = _NEARROW(TYPE, Ptr0,12, iStepMB);

    Ptr0 [4] = (TYPE)(pDC[1] * iQP);
    Ptr4 [0] = (TYPE)(pDC[2] * iQP);
    Ptr4 [4] = (TYPE)(pDC[3] * iQP);
    Ptr8 [0] = (TYPE)(pDC[4] * iQP);
    Ptr8 [4] = (TYPE)(pDC[5] * iQP);
    Ptr12[0] = (TYPE)(pDC[6] * iQP);
    Ptr12[4] = (TYPE)(pDC[7] * iQP);
  }
  else
  {
    TYPE *Ptr4  = _NEARROW(TYPE, Ptr0, 4, iStepMB);
    Ptr0[4] = (TYPE)(pDC[1] * iQP);
    Ptr4[0] = (TYPE)(pDC[2] * iQP);
    Ptr4[4] = (TYPE)(pDC[3] * iQP);
  }
}


ExcStatus CJPEGXRTileDecoder::dequantDCLP(void)
{
  Ipp32u iStepMB;
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u iBands = m_pImagePlane->iBands;
  Ipp32u iACHOffset = (m_bAlphaProcess)?m_pParent->m_iACHOffset:0;
  Ipp32u iUV42X = 0;
  Ipp32u i;

  if(iColorFormat == JXR_ICF_YUV422)
    iUV42X = 1;
  else if(iColorFormat == JXR_ICF_YUV420)
    iUV42X = 2;

  // Dequantize DC
  if(m_pParent->m_iCUSize == 2)
  {
    for(i = 0; i < iChannels; i++)
    {
      Ipp16s *Ptr0 = (Ipp16s*)m_ImgBuf[i + iACHOffset].pHufPtr;
      iStepMB = m_ImgBuf[i + iACHOffset].iStep;
      Ptr0[0] = (Ipp16s)(m_iDCLPTable[i][0] * m_pParent->m_pQuantDC[i + iACHOffset]->iQP);

      // Dequantize LP
      if(iBands < JXR_SB_DCONLY)
        dequantLP<Ipp16s>(Ptr0, m_iDCLPTable[i], (i)?iUV42X:0, iStepMB, m_pParent->m_pQuantLP[i + iACHOffset][m_pParent->m_iLPQuantizerIndex].iQP);
    }
  }
  else
  {
    for(i = 0; i < iChannels; i++)
    {
      Ipp32s *Ptr0 = (Ipp32s*)m_ImgBuf[i + iACHOffset].pHufPtr;
      iStepMB = m_ImgBuf[i + iACHOffset].iStep;
      Ptr0[0] = m_iDCLPTable[i][0] * m_pParent->m_pQuantDC[i + iACHOffset]->iQP;

      // Dequantize LP
      if(iBands < JXR_SB_DCONLY)
        dequantLP<Ipp32s>(Ptr0, m_iDCLPTable[i], (i)?iUV42X:0, iStepMB, m_pParent->m_pQuantLP[i + iACHOffset][m_pParent->m_iLPQuantizerIndex].iQP);
    }
  }

  return ExcStatusOk;
} // CJPEGXRDecoder::dequantDCLP()


/***********************************/
// Prediction functions
/***********************************/

ExcStatus CJPEGXRTileDecoder::predDCLP(void)
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
  Ipp32u iACHOffset = (m_bAlphaProcess)?m_pParent->m_iACHOffset:0;
  Ipp32u bUV42X = (iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)?1:0;
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
      iHor = ABS(m_pPredPrev[iACHOffset][m_iMBColumn - 1].iDC - m_pPred[iACHOffset][m_iMBColumn - 1].iDC);
      iVer = ABS(m_pPredPrev[iACHOffset][m_iMBColumn - 1].iDC - m_pPredPrev[iACHOffset][m_iMBColumn].iDC);
    }
    else
    {
      if(iColorFormat == JXR_ICF_YUV422)
        iScale = 4;
      else if(iColorFormat == JXR_ICF_YUV420)
        iScale = 8;

      iHor =
        ABS(m_pPredPrev[iACHOffset][m_iMBColumn - 1].iDC - m_pPred[iACHOffset][m_iMBColumn - 1].iDC) * iScale +
        ABS(m_pPredPrev[1 + iACHOffset][m_iMBColumn - 1].iDC - m_pPred[1 + iACHOffset][m_iMBColumn - 1].iDC) +
        ABS(m_pPredPrev[2 + iACHOffset][m_iMBColumn - 1].iDC - m_pPred[2 + iACHOffset][m_iMBColumn - 1].iDC);
      iVer =
        ABS(m_pPredPrev[iACHOffset][m_iMBColumn - 1].iDC - m_pPredPrev[iACHOffset][m_iMBColumn].iDC) * iScale +
        ABS(m_pPredPrev[1 + iACHOffset][m_iMBColumn - 1].iDC - m_pPredPrev[1 + iACHOffset][m_iMBColumn].iDC) +
        ABS(m_pPredPrev[2 + iACHOffset][m_iMBColumn - 1].iDC - m_pPredPrev[2 + iACHOffset][m_iMBColumn].iDC);
    }
    iDCPredMode = (iHor * 4 < iVer ? 1 : (iVer * 4 < iHor ? 0 : 2));
  }

  if(iDCPredMode == 1 && m_pParent->m_iLPQuantizerIndex == m_pPredPrev[iACHOffset][m_iMBColumn].iQPIndex)
    iLPPredMode = 1;
  if(iDCPredMode == 0 && m_pParent->m_iLPQuantizerIndex == m_pPred[iACHOffset][m_iMBColumn - 1].iQPIndex)
    iLPPredMode = 0;


  for(i = 0; i < iChannels; i ++)
  {
    pOrg = m_iDCLPTable[i];

    if(iDCPredMode == 1)
      pOrg[0] += m_pPredPrev[i + iACHOffset][m_iMBColumn].iDC;
    else if(iDCPredMode == 0)
      pOrg[0] += m_pPred[i + iACHOffset][m_iMBColumn - 1].iDC;
    else if(iDCPredMode == 2)
    {
      if(i == 0 || !bUV42X)
        pOrg[0] += (m_pPred[i + iACHOffset][m_iMBColumn - 1].iDC + m_pPredPrev[i + iACHOffset][m_iMBColumn].iDC) >> 1;
      else
        pOrg[0] += (m_pPred[i + iACHOffset][m_iMBColumn - 1].iDC + m_pPredPrev[i + iACHOffset][m_iMBColumn].iDC + 1) >> 1;
    }
  }

  for(i = 0; i < iChannels; i ++)
  {
    pOrg = m_iDCLPTable[i];

    if(iLPPredMode == 1)
    {
      if(i == 0 || !bUV42X)
      {
        pRef = m_pPredPrev[i + iACHOffset][m_iMBColumn].iDCLP;
        pOrg[4]  += pRef[3];
        pOrg[8]  += pRef[4];
        pOrg[12] += pRef[5];
      }
      else
      {
        if(iColorFormat == JXR_ICF_YUV422)
        {
          pOrg[4] += m_pPredPrev[i + iACHOffset][m_iMBColumn].iDCLP[4];
          pOrg[2] += m_pPredPrev[i + iACHOffset][m_iMBColumn].iDCLP[3];
          pOrg[6] += pOrg[2];
        }
        else
          pOrg[2] += m_pPredPrev[i + iACHOffset][m_iMBColumn].iDCLP[1];
      }
    }
    else if(iLPPredMode == 0)
    {
      if(i == 0 || !bUV42X)
      {
        pRef = m_pPred[i + iACHOffset][m_iMBColumn - 1].iDCLP;
        pOrg[1] += pRef[0];
        pOrg[2] += pRef[1];
        pOrg[3] += pRef[2];
      }
      else
      {
        if(iColorFormat == JXR_ICF_YUV422)
        {
          pOrg[4] += m_pPred[i + iACHOffset][m_iMBColumn - 1].iDCLP[4];
          pOrg[1] += m_pPred[i + iACHOffset][m_iMBColumn - 1].iDCLP[0];
          pOrg[5] += m_pPred[i + iACHOffset][m_iMBColumn - 1].iDCLP[2];
        }
        else
          pOrg[1] += m_pPred[i + iACHOffset][m_iMBColumn - 1].iDCLP[0];
      }
    }
  }

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
  m_iOrientation = 2 - ((iHor * 4 < iVer)?1:((iVer * 4 < iHor)?0:2));

  // Pred update
  for(i = 0; i < iChannels; i++)
  {
    m_pPred[i + iACHOffset][m_iMBColumn].iDC = m_iDCLPTable[i][0];
    m_pPred[i + iACHOffset][m_iMBColumn].iQPIndex = m_pParent->m_iLPQuantizerIndex;
  }

  for(i = 0; i < iChannels; i++)
  {
    pPred = &m_pPred[i + iACHOffset][m_iMBColumn];
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

  return ExcStatusOk;
} // CJPEGXRDecoder::predDCLP()


template<class TYPE>
static void predHP(Ipp32u iChannels, Ipp32u iColorFormat, Ipp32u iPredMode, ImgBuf* pImgBuf)
{
  Ipp32u iTWidth = 4, iTHeight = 4;
  Ipp32u iWidth = 4, iHeight = 4;
  TYPE *pRow10, *pRow11, *pRow12, *pRow20, *pRow21, *pRow22;
  Ipp32u i, j, k;

  if(iColorFormat == JXR_ICF_YUV422)
    iTWidth = 2;
  else if(iColorFormat == JXR_ICF_YUV420)
  {
    iTWidth = 2;
    iTHeight = 2;
  }

  for(i = 0; i < iChannels; i++)
  {
    Ipp32u iStepMB = pImgBuf[i].iStep;

    if(iPredMode == 1)
    {
      pRow10 = _PEL(TYPE, pImgBuf[i].pHufPtr, 0, 1, iStepMB);

      for(k = 1; k < iHeight; k++)
      {
        pRow11 = _NEXTROW(TYPE, pRow10, iStepMB);
        pRow12 = _NEXTROW(TYPE, pRow11,  iStepMB);
        pRow20 = _NEARROW(TYPE, pRow10, 4, iStepMB);
        pRow21 = _NEXTROW(TYPE, pRow20, iStepMB);
        pRow22 = _NEXTROW(TYPE, pRow21, iStepMB);

        for(j = 0; j < iWidth; j++)
        {
          pRow20[4*j]   += pRow10[4*j];
          pRow21[4*j]   += pRow11[4*j];
          pRow22[4*j+1] += pRow12[4*j+1];
        }
        pRow10 = _NEARROW(TYPE, pRow10, 4, iStepMB);
      }
    }
    else if(iPredMode == 0)
    {
      for(k = 0; k < iWidth - 1; k++)
      {
        pRow10 = _PEL(TYPE, pImgBuf[i].pHufPtr, 4*k, 0, iStepMB);

        for(j = 0; j < iHeight; j++)
        {
          pRow11 = _NEARROW(TYPE, pRow10, 4*j, iStepMB);
          pRow12 = _NEXTROW(TYPE, pRow11, iStepMB);
          pRow11[5] += pRow11[1];
          pRow11[6] += pRow11[2];
          pRow12[7] += pRow12[3];
        }
      }
    }
    iWidth = iTWidth, iHeight = iTHeight;
  }
} // predHP()


/***********************************/
// Postproccessing functions
/***********************************/
template<class TYPE>
static void invPCTransformFirst(ImgBuf* pImgBuf, IppiSize roi, Ipp32u iNoHP, Ipp32u iBorder, Ipp32s iShift)
{
  Ipp32u bTop    = !(ippBorderInMemTop    & iBorder);
  Ipp32u bBottom = !(ippBorderInMemBottom & iBorder);
  Ipp32u bLeft   = !(ippBorderInMemLeft   & iBorder);
  Ipp32u bRight  = !(ippBorderInMemRight  & iBorder);
  IppiSize mbSize = roi;
  IppiRect iRect ={-(roi.width/2), -(roi.width/2), roi.width, roi.height};
  Ipp32s iX = (bLeft)? -iRect.x : 0;
  Ipp32s iY = (bTop)? -iRect.y : 0;
  Ipp32s i, j;
  TYPE* pSrc;
  Ipp32u iStep;

  if(bLeft)
    mbSize.width += iRect.x;
  else if(bRight)
    mbSize.width = -iRect.x;

  if(bTop)
    mbSize.height += iRect.y;
  else if(bBottom)
    mbSize.height = -iRect.y;

  iStep = pImgBuf->iStep;
  pSrc = _PEL(TYPE, pImgBuf->pPCTPtr, iX + iRect.x, iY + iRect.y, iStep);
  if(iShift)
    pSrc += iShift * roi.width/2;

  if(iNoHP)
  {
    for(i = 4; i <= mbSize.height; i += 4)
    {
      TYPE *p0 = _PEL(TYPE, pSrc, -4, i-4, iStep);
      TYPE *p1 = _PEL(TYPE, pSrc, -4, i-3, iStep);
      TYPE *p2 = _PEL(TYPE, pSrc, -4, i-2, iStep);
      TYPE *p3 = _PEL(TYPE, pSrc, -4, i-1, iStep);

      for(j = 4; j <= mbSize.width; j += 4)
      {
        p0 += 4, p1 += 4, p2 += 4, p3 += 4;
        TYPE pel = (p0[0] + 1) >> 1;
        TYPE pel1;

        p0[1] = p1[0] = p1[1] = pel;
        p0[0] -= pel;

        pel1 = p0[0] >> 1;
        p0[3] = p3[0] = p3[3] = pel1;

        pel >>= 1;
        p0[2] = p3[1] = p3[2] = p1[3] = p2[0] = p2[3] = p1[2] = p2[1] = p2[2] = pel;

        p0[0] -= pel1;
        p0[1] -= pel;
        p1[0] -= pel;
        p1[1] -= pel;
      }
    }
  }
  else if(16 == mbSize.width && 16 == mbSize.height)
    ippiPCTInv16x16_JPEGXR(pSrc, iStep);
  else if(8 == mbSize.width && 16 == mbSize.height)
    ippiPCTInv8x16_JPEGXR(pSrc, iStep);
  else if(8 == mbSize.width && 8 == mbSize.height)
    ippiPCTInv8x8_JPEGXR(pSrc, iStep);
  else
    ippiPCTInv_JPEGXR(pSrc, iStep, mbSize);
} // invPCTransformFirst()


template<class TYPE>
static void invPCTransformSecond(ImgBuf* pImgBuf, IppiSize roi, Ipp32u iNoLP, Ipp32u iUpscale)
{
  if(iNoLP)
  {
    TYPE*  pSrc  = (TYPE*)pImgBuf->pPCTPtr;
    Ipp32u iStep = pImgBuf->iStep;

    if(roi.height == 8)
    {
      TYPE* p0 = pSrc;
      TYPE* p4 = _NEARROW(TYPE, p0, 4, iStep);

      p4[0]  = p0[4] = p4[4] = (p0[0] >> 1);
      p0[0] -= p4[0];

      if(iUpscale)
        p0[0] <<= iUpscale, p0[4] <<= iUpscale, p4[0] <<= iUpscale, p4[4] <<= iUpscale;
    }
    else if(roi.width == 16)
    {
      TYPE* p0  = pSrc;
      TYPE* p4  = _NEARROW(TYPE, p0, 4, iStep);
      TYPE* p8  = _NEARROW(TYPE, p4, 4, iStep);
      TYPE* p12 = _NEARROW(TYPE, p8, 4, iStep);

      p0[4]  = p4[0]  = p4[4]   = ((p0[0] + 1) >> 1);
      p0[0] -= p0[4];
      p0[12] = p12[0] = p12[12] = (p0[0] >> 1);
      p0[0] -= p0[12];
      p0[8]  = p12[4] = p12[8]  = (p0[4] >> 1);
      p0[4] -= p0[8];
      p4[12] = p8[0]  = p8[12]  = (p4[0] >> 1);
      p4[0] -= p4[12];
      p4[8]  = p8[4]  = p8[ 8]  = (p4[4] >> 1);
      p4[4] -= p4[8];

      if(iUpscale)
      {
        p0[0] <<= iUpscale, p0[4]  <<= iUpscale, p4[0]  <<= iUpscale, p4[4]   <<= iUpscale;
        p0[8] <<= iUpscale, p0[12] <<= iUpscale, p4[8]  <<= iUpscale, p4[12]  <<= iUpscale;
        p8[0] <<= iUpscale, p8[4]  <<= iUpscale, p12[0] <<= iUpscale, p12[4]  <<= iUpscale;
        p8[8] <<= iUpscale, p8[12] <<= iUpscale, p12[8] <<= iUpscale, p12[12] <<= iUpscale;
      }
    }
    else
    {
      TYPE* p0  = pSrc;
      TYPE* p4  = _NEARROW(TYPE, p0, 4, iStep);
      TYPE* p8  = _NEARROW(TYPE, p4, 4, iStep);
      TYPE* p12 = _NEARROW(TYPE, p8, 4, iStep);

      p8[0]  = p0[0];
      p4[0]  = p0[4]  = p4[4]  = (p0[0] >> 1);
      p0[0] -= p4[0];
      p8[4]  = p12[0] = p12[4] = (p8[0] >> 1);
      p8[0] -= p4[0];

      if(iUpscale)
      {
        p0[0] <<= iUpscale, p0[4] <<= iUpscale, p4[0]  <<= iUpscale, p4[4]  <<= iUpscale;
        p8[0] <<= iUpscale, p8[4] <<= iUpscale, p12[0] <<= iUpscale, p12[4] <<= iUpscale;
      }
    }
  }
  else
  {
    if(roi.height == 8)
      ippiPCTInv8x8DC_HDP((TYPE*)pImgBuf->pPCTPtr, pImgBuf->iStep, iUpscale);
    else if(roi.width == 16)
      ippiPCTInv16x16DC_HDP((TYPE*)pImgBuf->pPCTPtr, pImgBuf->iStep, iUpscale);
    else
      ippiPCTInv8x16DC_HDP((TYPE*)pImgBuf->pPCTPtr, pImgBuf->iStep, iUpscale);
  }
} // invPCTransformSecond()


template<class TYPE>
static void invFilterFirst(ImgBuf* pImgBuf, IppiSize roi, Ipp32u iBorder, Ipp32s iShift)
{
  Ipp32u bTop    = !(ippBorderInMemTop    & iBorder);
  Ipp32u bBottom = !(ippBorderInMemBottom & iBorder);
  Ipp32u bLeft   = !(ippBorderInMemLeft   & iBorder);
  Ipp32u bRight  = !(ippBorderInMemRight  & iBorder);
  IppiSize mbSize = roi;
  IppiRect iRect = {-2-roi.width/2, -2-roi.width/2, roi.width, roi.height};
  Ipp32s iX = (bLeft)? -iRect.x : 0;
  Ipp32s iY = (bTop)?  -iRect.y : 0;
  TYPE* pSrc;
  Ipp32u iStep;

  if(bLeft)
    mbSize.width = iRect.width + iRect.x;
  else if(bRight)
    mbSize.width = -iRect.x;
  if(bTop)
    mbSize.height = iRect.height + iRect.y;
  else if(bBottom)
    mbSize.height = -iRect.y;

  iStep = pImgBuf->iStep;
  pSrc = _PEL(TYPE, pImgBuf->pPCTPtr, iX + iRect.x, iY + iRect.y, iStep);
  if(iShift)
    pSrc += iShift * roi.width/2;

  ippiFilterInv_HDP(pSrc, iStep, mbSize, (IppiBorderType)iBorder);
} // invFilterFirst()


template<class TYPE>
static void invFilterSecond(ImgBuf* pImgBuf, IppiSize roi, Ipp32u iBorder, Ipp32u iShift)
{
  Ipp32u bTop    = !(ippBorderInMemTop    & iBorder);
  Ipp32u bBottom = !(ippBorderInMemBottom & iBorder);
  Ipp32u bLeft   = !(ippBorderInMemLeft   & iBorder);
  Ipp32u bRight  = !(ippBorderInMemRight  & iBorder);
  IppiSize mbSize = roi;
  IppiRect iRect = {-(roi.width/2), -(roi.width/2), roi.width, roi.height};
  Ipp32s iX = (bLeft)? -iRect.x : 0;
  Ipp32s iY = (bTop)? -iRect.y : 0;
  TYPE* pSrc;
  Ipp32u iStep;

  if(bLeft)
    mbSize.width = iRect.width + iRect.x;
  else if(bRight)
    mbSize.width = -iRect.x;
  if(bTop)
    mbSize.height = iRect.height + iRect.y;
  else if(bBottom)
    mbSize.height = -iRect.y;

  iStep = pImgBuf->iStep;
  pSrc = _PEL(TYPE, pImgBuf->pPCTPtr, iX + iRect.x, iY + iRect.y, iStep);

  if(roi.width > 8 || roi.height > 8)
    ippiFilterInvDCYUV444_HDP(pSrc, iStep, mbSize, (IppiBorderType)iBorder);
  else
  {
    Ipp32u bFLeft  = iShift & 2; 
    Ipp32u bFRight = iShift & 1;

    if(bFLeft)
    {
      if(bTop)
        pSrc[-4] -= pSrc[0];
      if(bBottom)
      {
        TYPE* p = _PEL(TYPE, pSrc, 0, roi.height - 4, iStep);
        p[-4] -= p[0];
      }
    }
    if(bFRight)
    {
      if(bTop)
        pSrc[roi.width] -= pSrc[roi.width - 4];
      if(bBottom)
      {
        TYPE* p = _PEL(TYPE, pSrc, 0, roi.height - 4, iStep);
        p[roi.width] -= p[roi.width - 4];
      }
    }

    ippiFilterInvDCYUV420_HDP(pSrc, iStep, mbSize, (IppiBorderType)iBorder);

    if(!bLeft && bFLeft)
    {
      if(bTop)
          pSrc[-4] += pSrc[0];
      if(bBottom)
      {
        TYPE* p = _PEL(TYPE, pSrc, 0, roi.height - 4, iStep);
        p[-4] += p[0];
      }
    }
    if(!bRight && bFRight)
    {
      if(bTop)
        pSrc[roi.width] += pSrc[roi.width - 4];
      if(bBottom)
      {
        TYPE* p = _PEL(TYPE, pSrc, 0, roi.height - 4, iStep);
        p[roi.width] += p[roi.width - 4];
      }
    }
  }
} // invFilterSecond()


// Inverse PC Transform
ExcStatus CJPEGXRTileDecoder::PCTransform(void)
{
  Ipp32u iChannels = m_pImagePlanePCT->iChannels;
  Ipp32u iColorFormat = m_pImagePlanePCT->iColorFormat;
  Ipp32u iBands  = m_pImagePlanePCT->iBands;
  Ipp32u bLeft   = !(ippBorderInMemLeft & m_iBorderPCT);
  Ipp32u bBottom = !(ippBorderInMemBottom & m_iBorderPCT);
  Ipp32u bRight  = !(ippBorderInMemRight  & m_iBorderPCT);
  Ipp32u bUV42X = (iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)?1:0;
  Ipp32u iNoHP = (iBands >= JXR_SB_NOHP)?1:0;
  Ipp32u iNoLP = (iBands >= JXR_SB_DCONLY)?1:0;
  Ipp32u iShift = ((m_iMBColumnPCT == 1)? 2 : 0) | ((m_iMBColumnPCT == m_iCurTileMBWidth - 1)? 1 : 0);
  Ipp32u iACHOffset = (m_bAlphaProcessPCT)?m_pParent->m_iACHOffset:0;
  Ipp32u i;

  if(m_pParent->m_iCUSize == 2)
  {
    for(i = 0; i < iChannels; i++)
    {
      IppiSize roi = (i)?m_pParent->m_cRoi:m_pParent->m_lRoi;
      Ipp32u   iScale = ((i != 0)?m_pImagePlanePCT->bScalingFlag:0);

      if(!bBottom && !bRight)
        invPCTransformSecond<Ipp16s>(&m_ImgBuf[i + iACHOffset], roi, iNoLP, iScale);

      if(2 == m_pParent->m_iOverlap)
        invFilterSecond<Ipp16s>(&m_ImgBuf[i + iACHOffset], roi, m_iBorderPCT, iShift);

      if(i == 0 || !bUV42X)
      {
        invPCTransformFirst<Ipp16s>(&m_ImgBuf[i + iACHOffset], roi, iNoHP, m_iBorderPCT, 0);

        if(0 != m_pParent->m_iOverlap)
          invFilterFirst<Ipp16s>(&m_ImgBuf[i + iACHOffset], roi, m_iBorderPCT, 0);
      }
      else if(!bLeft)
      {
        invPCTransformFirst<Ipp16s>(&m_ImgBuf[i + iACHOffset], roi, iBands, m_iBorderPCT, -1);

        if(0 != m_pParent->m_iOverlap)
          invFilterFirst<Ipp16s>(&m_ImgBuf[i + iACHOffset], roi, m_iBorderPCT, -1);
      }
    }

    if(m_pImagePlane->bResampling && bRight && !m_bAlphaProcess)
    {
      IppiSize roi = m_pParent->m_cRoi;
      invPCTransformFirst<Ipp16s>(&m_ImgBuf[1], roi, iBands, m_iBorderPCT, 0);
      invPCTransformFirst<Ipp16s>(&m_ImgBuf[2], roi, iBands, m_iBorderPCT, 0);

      if(0 != m_pParent->m_iOverlap)
      {
        invFilterFirst<Ipp16s>(&m_ImgBuf[1], roi, m_iBorderPCT, 0);
        invFilterFirst<Ipp16s>(&m_ImgBuf[2], roi, m_iBorderPCT, 0);
      }
    }
  }
  else
  {
    for(i = 0; i < iChannels; i++)
    {
      IppiSize roi = (i)?m_pParent->m_cRoi:m_pParent->m_lRoi;
      Ipp32u   iScale = ((i != 0)?m_pImagePlanePCT->bScalingFlag:0);

      if(!bBottom && !bRight)
        invPCTransformSecond<Ipp32s>(&m_ImgBuf[i + iACHOffset], roi, iBands, iScale);

      if(2 == m_pParent->m_iOverlap)
        invFilterSecond<Ipp32s>(&m_ImgBuf[i + iACHOffset], roi, m_iBorderPCT, iShift);

      if(i == 0 || !bUV42X)
      {
        invPCTransformFirst<Ipp32s>(&m_ImgBuf[i + iACHOffset], roi, iBands, m_iBorderPCT, 0);

        if(0 != m_pParent->m_iOverlap)
          invFilterFirst<Ipp32s>(&m_ImgBuf[i + iACHOffset], roi, m_iBorderPCT, 0);
      }
      else if(!bLeft)
      {
        invPCTransformFirst<Ipp32s>(&m_ImgBuf[i + iACHOffset], roi, iBands, m_iBorderPCT, -1);

        if(0 != m_pParent->m_iOverlap)
          invFilterFirst<Ipp32s>(&m_ImgBuf[i + iACHOffset], roi, m_iBorderPCT, -1);
      }
    }

    if(m_pImagePlane->bResampling && bRight && !m_bAlphaProcess)
    {
      IppiSize roi = m_pParent->m_cRoi;
      invPCTransformFirst<Ipp32s>(&m_ImgBuf[1], roi, iBands, m_iBorderPCT, 0);
      invPCTransformFirst<Ipp32s>(&m_ImgBuf[2], roi, iBands, m_iBorderPCT, 0);

      if(0 != m_pParent->m_iOverlap)
      {
        invFilterFirst<Ipp32s>(&m_ImgBuf[1], roi, m_iBorderPCT, 0);
        invFilterFirst<Ipp32s>(&m_ImgBuf[2], roi, m_iBorderPCT, 0);
      }
    }
  }

  return ExcStatusOk;
} // CJPEGXRDecoder::invPCTransform()


// Colorspace transformation function
ExcStatus CJPEGXRTileDecoder::colorConvert(Ipp8u* pDst, Ipp32u iDstStep, IppiSize imgRoi)
{
  Ipp32s* pSrc[JXR_MAX_CHANNELS];
  Ipp32u  pSrcStep[JXR_MAX_CHANNELS];
  Ipp16u iChannels = m_pParent->m_iChannels;
  Ipp8u  iBitDepth = m_pParent->m_iBitDepth;
  Ipp8u  bAlphaFlag = m_pParent->m_bAlphaFlag;
  Ipp8u  bFakeAlpha = m_pParent->m_bFakeAlpha;
  Ipp8u  iColorFormat = m_pParent->m_ImagePlane[0].iColorFormat;
  Ipp8u  iSrcColorFormat = m_pParent->m_iSrcColorFormat;
  Ipp8u  iCCenteringX = m_pImagePlane->iCCenteringX;
  Ipp8u  iCCenteringY = m_pImagePlane->iCCenteringY;
  Ipp8u  iCUSize = m_pParent->m_iCUSize;
  Ipp8u  iScale = m_pImagePlane->bScalingFlag ? 3 : 0;
  Ipp8u  iShift = m_pImagePlane->iShift;
  Ipp8u  iMan = m_pImagePlane->iMantiss;
  Ipp8u  iExp = m_pImagePlane->iExp;
  Ipp32u i;

  for(i = 0; i < iChannels; i++)
  {
    pSrc[i] = m_ImgBuf[i].pCCPtr;
    pSrcStep[i] = m_ImgBuf[i].iStep;
  }

  if(iColorFormat == JXR_ICF_Y && iSrcColorFormat == JXR_OCF_RGB)
  {
    pSrc[1] = pSrc[2] = m_ImgBuf[0].pCCPtr;
    pSrcStep[1] = pSrcStep[2] = m_ImgBuf[0].iStep;
  }

  // Upsampling
  if(m_pImagePlane->bResampling)
  {
    IppiBorderType borderType = (IppiBorderType)(ippBorderInMemTop | ippBorderInMemBottom);
    Ipp32u iResStep = m_iCurTileMBWidth * 16 * m_pParent->m_iCUSize;
    IppiSize imgRoi422 = {m_iCurTileMBWidth * 8, 16};

    if(iColorFormat == JXR_ICF_YUV420)
    {
      IppiSize imgRoi420 = {m_iCurTileMBWidth * 8, 8};

      if(iCUSize == 2)
      {
        ippiYUV420To422_HDP_16s_C1R((Ipp16s*)pSrc[1], pSrcStep[1], (Ipp16s*)m_pResU, iResStep, imgRoi420, borderType, iCCenteringY);
        ippiYUV420To422_HDP_16s_C1R((Ipp16s*)pSrc[2], pSrcStep[2], (Ipp16s*)m_pResV, iResStep, imgRoi420, borderType, iCCenteringY);
        ippiYUV422To444_HDP_16s_C1IR((Ipp16s*)m_pResU, iResStep, imgRoi422, borderType, iCCenteringX);
        ippiYUV422To444_HDP_16s_C1IR((Ipp16s*)m_pResV, iResStep, imgRoi422, borderType, iCCenteringX);
      }
      else
      {
        ippiYUV420To422_HDP_32s_C1R(pSrc[1], pSrcStep[1], m_pResU, iResStep, imgRoi420, borderType, iCCenteringY);
        ippiYUV420To422_HDP_32s_C1R(pSrc[2], pSrcStep[2], m_pResV, iResStep, imgRoi420, borderType, iCCenteringY);
        ippiYUV422To444_HDP_32s_C1IR(m_pResU, iResStep, imgRoi422, borderType, iCCenteringX);
        ippiYUV422To444_HDP_32s_C1IR(m_pResV, iResStep, imgRoi422, borderType, iCCenteringX);
      }
    }
    else
    {
      if(iCUSize == 2)
      {
        ippiYUV422To444_HDP_16s_C1R((Ipp16s*)pSrc[1], pSrcStep[1], (Ipp16s*)m_pResU, iResStep, imgRoi422, borderType, iCCenteringX);
        ippiYUV422To444_HDP_16s_C1R((Ipp16s*)pSrc[2], pSrcStep[2], (Ipp16s*)m_pResV, iResStep, imgRoi422, borderType, iCCenteringX);
      }
      else
      {
        ippiYUV422To444_HDP_32s_C1R(pSrc[1], pSrcStep[1], m_pResU, iResStep, imgRoi422, borderType, iCCenteringX);
        ippiYUV422To444_HDP_32s_C1R(pSrc[2], pSrcStep[2], m_pResV, iResStep, imgRoi422, borderType, iCCenteringX);
      }
    }

    pSrc[1] = m_pResU;
    pSrc[2] = m_pResV;
    pSrcStep[1] = pSrcStep[2] = iResStep;
  }

  switch(iSrcColorFormat)
  {
  case JXR_OCF_Y:
    if(iBitDepth == JXR_BD_1W) // BW White=1
    {
      // Unsupported
    }
    else if(iBitDepth == JXR_BD_1B) // BW Black=1
    {
      // Unsupported
    }
    else if(iBitDepth == JXR_BD_8) // GRAY 8
    {
      if(iCUSize == 2)
        ccYToGray_16s8u((Ipp16s*)pSrc[0], pSrcStep[0], pDst, iDstStep, imgRoi, iScale);
      else
        ccYToGray_32s8u(pSrc[0], pSrcStep[0], pDst, iDstStep, imgRoi, iScale);
    }
    else if(iBitDepth == JXR_BD_16) // GRAY 16
      ccYToGray_32s16u(pSrc[0], pSrcStep[0], (Ipp16u*)pDst, iDstStep, imgRoi, iScale, iShift);
    else if(iBitDepth == JXR_BD_16S) // GRAY 16S
      ccYToGray_32s16s(pSrc[0], pSrcStep[0], (Ipp16s*)pDst, iDstStep, imgRoi, iScale, iShift);
    else if(iBitDepth == JXR_BD_16F) // GRAY 16F
      ccYToGray_32s16f(pSrc[0], pSrcStep[0], (Ipp16u*)pDst, iDstStep, imgRoi, iScale);
    else if(iBitDepth == JXR_BD_32S) // GRAY 32S
      ccYToGray_32s32s(pSrc[0], pSrcStep[0], (Ipp32s*)pDst, iDstStep, imgRoi, iScale, iShift);
    else if(iBitDepth == JXR_BD_32F) // GRAY 32F
      ccYToGray_32s32f(pSrc[0], pSrcStep[0], (Ipp32f*)pDst, iDstStep, imgRoi, iScale, iExp, iMan);
    break;

  case JXR_OCF_RGB:
    if(iBitDepth == JXR_BD_8) // RGB 8
    {
      if(iCUSize == 2)
      {
        if(m_pParent->m_bBGR)
        {
          if(bAlphaFlag)
            ccYUVToBGR_16s8u_P4C4((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
          else if(bFakeAlpha)
            ccYUVToBGR_16s8u_P3C4((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
          else
            ccYUVToBGR_16s8u_P3C3((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
        }
        else
        {
          if(bAlphaFlag)
            ccYUVToRGB_16s8u_P4C4((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
          else if(bFakeAlpha)
            ccYUVToRGB_16s8u_P3C4((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
          else
            ccYUVToRGB_16s8u_P3C3((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
        }
      }
      else
      {
        if(m_pParent->m_bBGR)
        {
          if(bAlphaFlag)
            ccYUVToBGR_32s8u_P4C4(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
          else if(bFakeAlpha)
            ccYUVToBGR_32s8u_P3C4(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
          else
            ccYUVToBGR_32s8u_P3C3(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
        }
        else
        {
          if(bAlphaFlag)
            ccYUVToRGB_32s8u_P4C4(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
          else if(bFakeAlpha)
            ccYUVToRGB_32s8u_P3C4(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
          else
            ccYUVToRGB_32s8u_P3C3(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
        }
      }
    }
    else if(iBitDepth == JXR_BD_16) // RGB 16
    {
      if(bAlphaFlag)
        ccYUVToRGB_32s16u_P4C4(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale, iShift);
      else
        ccYUVToRGB_32s16u_P3C3(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale, iShift);
    }
    else if(iBitDepth == JXR_BD_16S) // RGB 16S
    {
      if(bAlphaFlag)
        ccYUVToRGB_32s16s_P4C4(pSrc, pSrcStep, (Ipp16s*)pDst, iDstStep, imgRoi, iScale, iShift);
      else if(bFakeAlpha)
        ccYUVToRGB_32s16s_P3C4(pSrc, pSrcStep, (Ipp16s*)pDst, iDstStep, imgRoi, iScale, iShift);
      else
        ccYUVToRGB_32s16s_P3C3(pSrc, pSrcStep, (Ipp16s*)pDst, iDstStep, imgRoi, iScale, iShift);
    }
    else if(iBitDepth == JXR_BD_16F) // RGB 16F
    {
      if(bAlphaFlag)
        ccYUVToRGB_32s16f_P4C4(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale);
      else if(bFakeAlpha)
        ccYUVToRGB_32s16f_P3C4(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale);
      else
        ccYUVToRGB_32s16f_P3C3(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale);
    }
    else if(iBitDepth == JXR_BD_32S) // RGB 32S
    {
      if(bAlphaFlag)
        ccYUVToRGB_32s32s_P4C4(pSrc, pSrcStep, (Ipp32s*)pDst, iDstStep, imgRoi, iScale, iShift);
      else if(bFakeAlpha)
        ccYUVToRGB_32s32s_P3C4(pSrc, pSrcStep, (Ipp32s*)pDst, iDstStep, imgRoi, iScale, iShift);
      else
        ccYUVToRGB_32s32s_P3C3(pSrc, pSrcStep, (Ipp32s*)pDst, iDstStep, imgRoi, iScale, iShift);
    }
    else if(iBitDepth == JXR_BD_32F) // RGB 32F
    {
      if(bAlphaFlag)
        ccYUVToRGB_32s32f_P4C4(pSrc, pSrcStep, (Ipp32f*)pDst, iDstStep, imgRoi, iScale, iExp, iMan);
      else if(bFakeAlpha)
        ccYUVToRGB_32s32f_P3C4(pSrc, pSrcStep, (Ipp32f*)pDst, iDstStep, imgRoi, iScale, iExp, iMan);
      else
        ccYUVToRGB_32s32f_P3C3(pSrc, pSrcStep, (Ipp32f*)pDst, iDstStep, imgRoi, iScale, iExp, iMan);
    }
    else if(iBitDepth == JXR_BD_5) // RGB 555
    {
      // Unsupported
    }
    else if(iBitDepth == JXR_BD_565) // RGB 565
    {
      // Unsupported
    }
    else if(iBitDepth == JXR_BD_10) // RGB 101010
    {
      // Unsupported
    }
    break;

  case JXR_OCF_RGBE:
    if(iBitDepth == JXR_BD_8)
      ccYUVToRGBE_32s8u_P3C4(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
    break;

  case JXR_OCF_CMYK:
    if(iBitDepth == JXR_BD_8) // CMYK 8
    {
      if(iCUSize == 2)
      {
        if(bAlphaFlag)
          ccYUVKToCMYK_16s8u_P5C5((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
        else
          ccYUVKToCMYK_16s8u_P4C4((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
      }
      else
      {
        if(bAlphaFlag)
          ccYUVKToCMYK_32s8u_P5C5(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
        else
          ccYUVKToCMYK_32s8u_P4C4(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
      }
    }
    else if(iBitDepth == JXR_BD_16) // CMYK 16
    {
      if(bAlphaFlag)
        ccYUVKToCMYK_32s16u_P5C5(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale, iShift);
      else
        ccYUVKToCMYK_32s16u_P4C4(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale, iShift);
    }
    break;

  case JXR_OCF_CMYKD:
    if(iBitDepth == JXR_BD_8) // CMYKD 8
    {
      if(iCUSize == 2)
      {
        if(bAlphaFlag)
          ccYUVKToCMYKD_16s8u_P5C5((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
        else
          ccYUVKToCMYKD_16s8u_P4C4((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
      }
      else
      {
        if(bAlphaFlag)
          ccYUVKToCMYKD_32s8u_P5C5(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
        else
          ccYUVKToCMYKD_32s8u_P4C4(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
      }
    }
    else if(iBitDepth == JXR_BD_16) // CMYKD 16
    {
      if(bAlphaFlag)
        ccYUVKToCMYKD_32s16u_P5C5(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale, iShift);
      else
        ccYUVKToCMYKD_32s16u_P4C4(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale, iShift);
    }
    break;

  case JXR_OCF_NCH:
    if(iBitDepth == JXR_BD_8) // NCHANNELS 8
    {
      if(iCUSize == 2)
        ccNCHToNCH_16s8u((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale, iChannels);
      else
        ccNCHToNCH_32s8u(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale, iChannels);
    }
    else if(iBitDepth == JXR_BD_16) // NCHANNELS 16
      ccNCHToNCH_32s16u(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale, iShift, iChannels);
    break;

  case JXR_OCF_YUV444:
    if(iBitDepth == JXR_BD_8) // YUV444 8
    {
      if(iCUSize == 2)
      {
        if(bAlphaFlag)
          ccYUVToYUV_16s8u_P4C4((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
        else
          ccYUVToYUV_16s8u_P3C3((Ipp16s**)pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
      }
      else
      {
        if(bAlphaFlag)
          ccYUVToYUV_32s8u_P4C4(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
        else
          ccYUVToYUV_32s8u_P3C3(pSrc, pSrcStep, pDst, iDstStep, imgRoi, iScale);
      }
    }
    else if(iBitDepth == JXR_BD_10) // YUV444 10
    {
      // Unsupported
    }
    else if(iBitDepth == JXR_BD_16) // YUV444 16
    {
      if(bAlphaFlag)
        ccYUVToYUV_32s16u_P4C4(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale, iShift);
      else
        ccYUVToYUV_32s16u_P3C3(pSrc, pSrcStep, (Ipp16u*)pDst, iDstStep, imgRoi, iScale, iShift);
    }
    else if(iBitDepth == JXR_BD_16S) // YUV444 16S
    {
      if(bAlphaFlag)
        ccYUVToYUV_32s16s_P4C4(pSrc, pSrcStep, (Ipp16s*)pDst, iDstStep, imgRoi, iScale, iShift);
      else
        ccYUVToYUV_32s16s_P3C3(pSrc, pSrcStep, (Ipp16s*)pDst, iDstStep, imgRoi, iScale, iShift);
    }
    break;

  case JXR_OCF_YUV422:
    if(iBitDepth == JXR_BD_8) // YUV422 8
    {
      // Unsupported
    }
    else if(iBitDepth == JXR_BD_10) // YUV422 10
    {
      // Unsupported
    }
    else if(iBitDepth == JXR_BD_16) // YUV422 16
    {
      // Unsupported
    }
    break;

  case JXR_OCF_YUV420:
    if(iBitDepth == JXR_BD_8) // YUV420 8
    {
      // Unsupported
    }
    break;
  }

  return ExcStatusOk;
} // CJPEGXRDecoder::colorConvert()

