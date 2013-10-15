/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning ( disable : 4127 )
#endif
#include "jpegxrdec.h"
#ifndef __IPPCORE_H__
#include "ippcore.h"
#endif
#ifndef __IPPS_H__
#include "ipps.h"
#endif

#ifdef USE_TBB
#include "jpegxrdec_tbb.h"
#endif

static Ipp32u typeSize[] =
{
  0, // EMPTY
  1, // JXR_TT_BYTE
  1, // JXR_TT_ASCII
  2, // JXR_TT_SHORT
  4, // JXR_TT_LONG
  8, // JXR_TT_RATIONAL
  1, // JXR_TT_SBYTE
  1, // JXR_TT_UNDEFINED
  2, // JXR_TT_SSHORT
  4, // JXR_TT_SLONG
  8, // JXR_TT_SRATIONAL
  4, // JXR_TT_FLOAT
  8  // JXR_TT_DOUBLE
};


CJPEGXRDecoder::CJPEGXRDecoder(void)
{
#ifdef USE_TBB
  m_iThreads = tbb::task_scheduler_init::default_num_threads();
  scheduler = new tbb::task_scheduler_init(tbb::task_scheduler_init::deferred);
#endif

  ippsZero_8u((Ipp8u*)&m_ImagePlane[JXR_PLANE_PRIMARY], sizeof(ImagePlane)*2);
  m_pImagePlane = &m_ImagePlane[JXR_PLANE_PRIMARY];

  m_iPipeThreads = 1;
  m_iTileThreads = 1;
  m_iPipeLength  = 3;
  m_iInstances = 0;

  m_iACHOffset = 0;
  m_bPAlphaProcess = 0;

  m_iNumTileBands = 1;
  m_iNumBands = 1;
  m_iMaxTileBitstream = 0;
  m_iMaxTileMBHeight = 0;
  m_iMaxTileMBWidth = 0;
  m_iTilesWidth = 1;
  m_iTilesHeight = 1;
  m_iTilesTotal = 0;
  m_iUnitSize = 1;
  m_bSoftTiling = 0;

  m_lRoi.height = m_cRoi.height = 16;
  m_lRoi.width = m_cRoi.width = 16;
  m_iChromaBlock = m_iLumaBlock = 16;

  m_iImageOffset = 0;
  m_iImageNumBytes = 0;
  m_iAlphaOffset = 0;
  m_iAlphaNumBytes = 0;

  m_bBGR = 0;
  m_iBandsLimit = 0;
  m_iForceColor = JXR_OCF_AUTO;
  m_bFakeAlpha = 0;
  m_iOutColorFormat = 0;
  m_bPreAlpha = 0;

  m_iLeftExtra   = 0;
  m_iTopExtra    = 0;
  m_iRightExtra  = 0;
  m_iBottomExtra = 0;

  m_bPipeArrayInit = 0;
  m_bThreadsInit = 0;
  m_bFirstLoop = 1;
  m_bReadTile = 0;
  return;
} // ctor


CJPEGXRDecoder::~CJPEGXRDecoder(void)
{
  if(!m_bFirstLoop)
    KillDecoder();

  if(m_bPipeArrayInit)
  {
    ippsFree(m_pPipeArray);
    m_bPipeArrayInit = 0;
  }

  return;
} // dtor


CJPEGXRTileDecoder::CJPEGXRTileDecoder(void)
{
  m_pHuffmanPlane = &m_HuffmanPlane[JXR_PLANE_PRIMARY];
  m_bAlphaProcess = 0;
  m_bAlphaProcessPCT = 0;

  m_iPipeThreads = 1;
  m_bTBBError = 0;
  m_iTrimFlex = 0;
  m_iCurTileMBRow = 0;
  m_iCurTileMBColumn = 0;
  m_bThreadLock = 0;

  return;
} // ctor


CJPEGXRTileDecoder::~CJPEGXRTileDecoder()
{
  m_pParent->m_iInstances--;
  for(Ipp32u i = 0; i < m_pParent->m_iNumBands; i++)
    m_Stream[i].Detach();

  return;
} // dtor


ExcStatus CJPEGXRDecoder::AttachStream(BaseStreamInput& in)
{
  RET_(m_BitStreamIn.Attach(in));
  RET_(m_BitStreamIn.Init(DEC_MIN_BUFLEN));
  m_in = &in;

  return ExcStatusOk;
} // AttachStream()


ExcStatus CJPEGXRDecoder::SetPlaneOffset(Ipp32u iImageOffset)
{
  if(iImageOffset == m_iImageOffset || iImageOffset == m_iAlphaOffset)
    m_iCurrentOffset = iImageOffset;
  else
    return ExcStatusFail;

  return ExcStatusOk;
} // SetPlaneOffset()


ExcStatus CJPEGXRDecoder::SetParams(Ipp32u iForceColor, Ipp32u iBandsLimit, Ipp32u iThreadMode)
{
  m_iForceColor = (Ipp8u)iForceColor;
  m_iBandsLimit = (Ipp8u)iBandsLimit;

  if(iForceColor != JXR_OCF_AUTO)
    m_iOutColorFormat = (Ipp8u)iForceColor;

  m_iThreadMode = (Ipp8u)iThreadMode;

  return ExcStatusOk;
} // SetParams()


Ipp32u CJPEGXRDecoder::SetThreads(Ipp32u iThreads)
{
  if(iThreads != 0)
    m_iThreads = (Ipp16u)iThreads;

  return m_iThreads;
} // SetThreads()


Ipp32u CJPEGXRDecoder::Threads()
{
  return m_iThreads;
} // Threads()


ExcStatus CJPEGXRDecoder::ReadTileInfo(Ipp32u* iTileWidth, Ipp32u* iTileHeight)
{
  *iTileWidth  = m_iTileMBWidth[0]*16;
  *iTileHeight = m_iTileMBHeight[0]*16;
  return ExcStatusOk;
} // ReadTileInfo()


ExcStatus CJPEGXRDecoder::readIFDValue(Ipp16u iType, Ipp32u iCount, Ipp32u iValue, void* pValue)
{
  Ipp32u iCurOffset;
  Ipp32u iTypeSize = typeSize[iType];
  Ipp32u* pValueLong = (Ipp32u*)pValue;
  Ipp8u*  pValueByte = (Ipp8u*)pValue;
  Ipp32u  i;

  if(iTypeSize*iCount > 4)
  {
    iCurOffset = m_BitStreamIn.GetTotalPos();
    m_BitStreamIn.Seek(iValue, BaseStreamInput::Beginning);

    for(i = 0; i < iTypeSize*iCount; i++)
      m_BitStreamIn.Read(pValueByte[i]);

    m_BitStreamIn.Seek(iCurOffset, BaseStreamInput::Beginning);
  }
  else
   *pValueLong = iValue;

  return ExcStatusOk;
} // readIFDValue()


ExcStatus CJPEGXRDecoder::readImageHeader(Ipp32u iOffset)
{
  Ipp8u  pGDISignature[8] = {0x57, 0x4d, 0x50, 0x48, 0x4f, 0x54, 0x4f, 0};
  Ipp8u  pGDISigBuffer[8];
  Ipp32u iBitsLenLo, iBitsLenHi;
  Ipp32u iMBWidth;
  Ipp32u iMBHeight;
  Ipp32u iReserved;
  Ipp32u i;

  RET_(m_BitStreamIn.Seek(iOffset, BaseStreamInput::Beginning));
  for(i = 0; i < 8; i++)
  {
    RET_(m_BitStreamIn.Read(&pGDISigBuffer[i], 1));
    if(pGDISigBuffer[i] != pGDISignature[i])
      return ExcStatusFail;
  }

  RET_(m_BitStreamIn.ReadBits(4, iReserved));
  RET_(m_BitStreamIn.ReadBits(1, m_bHardTilingFlag));
  RET_(m_BitStreamIn.ReadBits(3, iReserved));
  RET_(m_BitStreamIn.ReadBits(1, m_bTilingFlag));
  RET_(m_BitStreamIn.ReadBits(1, m_bFrequencyFlag));
  RET_(m_BitStreamIn.ReadBits(3, m_iOrientation));
  RET_(m_BitStreamIn.ReadBits(1, m_bIndexFlag));
  RET_(m_BitStreamIn.ReadBits(2, m_iOverlap));
  RET_(m_BitStreamIn.ReadBits(1, m_bShortHeaderFlag));
  RET_(m_BitStreamIn.ReadBits(1, m_bLongWordFlag));
  RET_(m_BitStreamIn.ReadBits(1, m_bWindowingFlag));
  RET_(m_BitStreamIn.ReadBits(1, m_bTrimFlexFlag));
  RET_(m_BitStreamIn.ReadBits(3, iReserved));
  RET_(m_BitStreamIn.ReadBits(1, m_bAlphaFlag));
  RET_(m_BitStreamIn.ReadBits(4, m_iSrcColorFormat));
  RET_(m_BitStreamIn.ReadBits(4, m_iBitDepth));

  iBitsLenLo = (m_bShortHeaderFlag)?8:16;
  iBitsLenHi = (m_bShortHeaderFlag)?16:32;

  RET_(m_BitStreamIn.ReadBits((Ipp8u)iBitsLenHi, m_iWidth));
  RET_(m_BitStreamIn.ReadBits((Ipp8u)iBitsLenHi, m_iHeight));
  m_iWidth++;
  m_iHeight++;

  m_iMBTotalWidth  = iMBWidth = m_iWidth/16;
  m_iMBTotalHeight = iMBHeight = m_iHeight/16;

  if(m_bTilingFlag)
  {
    RET_(m_BitStreamIn.ReadBits(12, m_iTilesWidth));
    m_iTilesWidth++;

    RET_(m_BitStreamIn.ReadBits(12, m_iTilesHeight));
    m_iTilesHeight++;
  }
  m_iTilesTotal = m_iTilesWidth * m_iTilesHeight;

  m_iTileMBWidth = ippsMalloc_16u(m_iTilesWidth + m_iTilesHeight);
  m_iTileMBHeight = m_iTileMBWidth + m_iTilesWidth;
  m_iTileMBWidth[0]  = 1;
  m_iTileMBHeight[0] = 1;

  for(i = 0; i < (Ipp16u)(m_iTilesWidth - 1); i++)
  {
    RET_(m_BitStreamIn.ReadBits((Ipp8u)iBitsLenLo, m_iTileMBWidth[i]));
    iMBWidth -= m_iTileMBWidth[i];
  }

  for(i = 0; i < (Ipp16u)(m_iTilesHeight - 1); i++)
  {
    RET_(m_BitStreamIn.ReadBits((Ipp8u)iBitsLenLo, m_iTileMBHeight[i]));
    iMBHeight -=  m_iTileMBHeight[i];
  }

  m_iTileMBWidth[m_iTilesWidth - 1]   = (Ipp16u)iMBWidth;
  m_iTileMBHeight[m_iTilesHeight - 1] = (Ipp16u)iMBHeight;

  if(m_bWindowingFlag)
  {
    RET_(m_BitStreamIn.ReadBits(6, m_iTopExtra));
    RET_(m_BitStreamIn.ReadBits(6, m_iLeftExtra));
    RET_(m_BitStreamIn.ReadBits(6, m_iBottomExtra));
    RET_(m_BitStreamIn.ReadBits(6, m_iRightExtra));

    m_iWidth  += m_iLeftExtra + m_iRightExtra;
    m_iHeight += m_iTopExtra + m_iBottomExtra;

    if(m_iWidth & 0xf || m_iHeight & 0xf)
      return ExcStatusFail;

    m_iMBTotalWidth = m_iWidth / 16;
    m_iMBTotalHeight = m_iHeight / 16;

    m_iWidth  -= m_iLeftExtra + m_iRightExtra;
    m_iHeight -= m_iTopExtra + m_iBottomExtra;

    if(m_iRightExtra)
      m_iTileMBWidth[m_iTilesWidth - 1] += ((m_iRightExtra + m_iLeftExtra) & 0xf)? (m_iRightExtra + m_iLeftExtra + 16)/16 : (m_iRightExtra + m_iLeftExtra)/16;
    if(m_iBottomExtra)
      m_iTileMBHeight[m_iTilesHeight - 1] += ((m_iBottomExtra + m_iTopExtra) & 0xf) ? (m_iBottomExtra + m_iTopExtra + 16)/16 : (m_iBottomExtra + m_iTopExtra)/16;
  }
  else
  {
    if(m_iWidth & 0xf)
    {
      m_iRightExtra = m_iWidth & 0xf;
      m_iTileMBWidth[m_iTilesWidth - 1]++;
      m_iMBTotalWidth++;
    }
    if(m_iHeight & 0xf)
    {
      m_iBottomExtra = m_iHeight & 0xf;
      m_iTileMBHeight[m_iTilesHeight - 1]++;
      m_iMBTotalHeight++;
    }
  }

  return ExcStatusOk;
} // readImageHeader()


ExcStatus CJPEGXRDecoder::readUnifQuantizer(Ipp8u* iCHMode, Ipp8u* iQuant)
{
  Ipp16u iChannels = m_pImagePlane->iChannels;
  Ipp32u i;

  if(iChannels == 1)
    *iCHMode = JXR_CM_UNIFORM;
  else
    RET_(m_BitStreamIn.ReadBits(2, *iCHMode));

  if(JXR_CM_UNIFORM == *iCHMode)
    RET_(m_BitStreamIn.ReadBits(8, iQuant[0]));

  if(JXR_CM_SEPARATE == *iCHMode)
  {
    RET_(m_BitStreamIn.ReadBits(8, iQuant[0]));
    RET_(m_BitStreamIn.ReadBits(8, iQuant[1]));
  }

  if(JXR_CM_INDEPENDENT == *iCHMode)
  {
    for(i = 0; i < iChannels; i++)
      RET_(m_BitStreamIn.ReadBits(8, iQuant[i]));
  }

  return ExcStatusOk;
} // readUnifQuantizer()


ExcStatus CJPEGXRDecoder::readImagePlaneHeader()
{
  Ipp32u iReserved = 0;
  Ipp32u iChannels;

  RET_(m_BitStreamIn.ReadBits(3, m_pImagePlane->iColorFormat));
  RET_(m_BitStreamIn.ReadBits(1, m_pImagePlane->bScalingFlag));
  RET_(m_BitStreamIn.ReadBits(4, m_pImagePlane->iBands));

  switch(m_pImagePlane->iColorFormat)
  {
  case JXR_ICF_Y:
    m_pImagePlane->iChannels = 1;
    break;

  case JXR_ICF_YUV420:
    if(m_iSrcColorFormat != JXR_OCF_YUV420)
      m_pImagePlane->bResampling = 1;
  case JXR_ICF_YUV422:
    if(m_iSrcColorFormat != JXR_OCF_YUV422 && m_iSrcColorFormat != JXR_OCF_YUV420)
      m_pImagePlane->bResampling = 1;

    m_pImagePlane->iChannels = 3;

    RET_(m_BitStreamIn.ReadBits(1, iReserved));
    RET_(m_BitStreamIn.ReadBits(3, m_pImagePlane->iCCenteringX));
    if(m_pImagePlane->iColorFormat == JXR_ICF_YUV420)
    {
      RET_(m_BitStreamIn.ReadBits(1, iReserved));
      RET_(m_BitStreamIn.ReadBits(3, m_pImagePlane->iCCenteringY));
    }
    else
      RET_(m_BitStreamIn.ReadBits(4, iReserved))
    break;

  case JXR_ICF_YUV444:
    m_pImagePlane->iChannels = 3;

    RET_(m_BitStreamIn.ReadBits(4, iReserved));
    RET_(m_BitStreamIn.ReadBits(4, iReserved));
    break;

  case JXR_ICF_YUVK:
    m_pImagePlane->iChannels = 4;
    break;

  case JXR_ICF_NCH:
    RET_(m_BitStreamIn.ReadBits(4, iChannels));
    m_pImagePlane->iChannels = (Ipp16u)(iChannels + 1);
    if(iChannels == 0xf)
    {
      RET_(m_BitStreamIn.ReadBits(12, iChannels));
      m_pImagePlane->iChannels = (Ipp16u)(iChannels + 16);
    }
    else
      RET_(m_BitStreamIn.ReadBits(4, iReserved));
    break;

  default:
    return ExcStatusFail;
  }

  switch(m_iBitDepth)
  {
  case JXR_BD_16:
  case JXR_BD_16S:
  case JXR_BD_32S:
    RET_(m_BitStreamIn.ReadBits(8, m_pImagePlane->iShift));
    break;
  case JXR_BD_32F:
    RET_(m_BitStreamIn.ReadBits(8, m_pImagePlane->iMantiss));
    RET_(m_BitStreamIn.ReadBits(8, m_pImagePlane->iExp));
    break;
  }

  RET_(m_BitStreamIn.ReadBits(1, m_pImagePlane->bDCUniformFlag));

  if(m_pImagePlane->bDCUniformFlag)
    RET_(readUnifQuantizer(&m_pImagePlane->iCHModeDC, m_pImagePlane->iDCQuant))
  else
    return ExcStatusFail; // only uniform supported

  if(m_pImagePlane->iBands < JXR_SB_DCONLY)
  {
    RET_(m_BitStreamIn.ReadBits(1, iReserved));
    RET_(m_BitStreamIn.ReadBits(1, m_pImagePlane->bLPUniformFlag));

    if(m_pImagePlane->bLPUniformFlag)
    {
      m_pImagePlane->iNumLPQ = 1;
      RET_(readUnifQuantizer(&m_pImagePlane->iCHModeLP, m_pImagePlane->iLPQuant));
    }
    else
      return ExcStatusFail; // only uniform supported

    if(m_pImagePlane->iBands < JXR_SB_NOHP)
    {
      RET_(m_BitStreamIn.ReadBits(1, iReserved));
      RET_(m_BitStreamIn.ReadBits(1, m_pImagePlane->bHPUniformFlag));

      if(m_pImagePlane->bHPUniformFlag)
      {
        m_pImagePlane->iNumHPQ = 1;
        RET_(readUnifQuantizer(&m_pImagePlane->iCHModeHP, m_pImagePlane->iHPQuant));
      }
      else
        return ExcStatusFail; // only uniform supported
    }
  }

  RET_(m_BitStreamIn.FlushToByte());

  return ExcStatusOk;
} // readImagePlaneHeader()


static Ipp64u readVLWESC(CBitStreamInput& bitStreamIn)
{
  Ipp32u iSizeCode;
  Ipp32u iWord;
  Ipp64u iValue;

  bitStreamIn.ReadBits(8, iSizeCode);

  if(iSizeCode < 0xfb) // 16bit value
  {
    bitStreamIn.ReadBits(8, iWord);
    iValue = (iSizeCode << 8) | iWord;
  }
  else if(iSizeCode == 0xfb) // 32bit value
  {
    bitStreamIn.ReadBits(16, iWord);
    iValue = iWord;
    bitStreamIn.ReadBits(16, iWord);
    iValue = (iValue << 16) | iWord;
  }
  else if(iSizeCode == 0xfc) // 64bit value
  {
    bitStreamIn.ReadBits(16, iWord);
    iValue = iWord;
    bitStreamIn.ReadBits(16, iWord);
    iValue = (iValue << 16) | iWord;
    bitStreamIn.ReadBits(16, iWord);
    iValue = (iValue << 16) | iWord;
    bitStreamIn.ReadBits(16, iWord);
    iValue = (iValue << 16) | iWord;
  }
  else // esc value
    iValue = 0;

  return iValue;
}


ExcStatus CJPEGXRDecoder::readIndexTable(void)
{
  Ipp32u iBits;
  Ipp64u iBytes;
  Ipp32u iNumBytes = 0;
  Ipp32u i;

  Ipp32u iProfileIDC;
  Ipp32u iLevelIDC;
  Ipp32u iReservedA;
  Ipp32u iReservedL;

  if(!m_bIndexFlag)
    m_iIndexSize = 0;
  else
  {
    if(m_bFrequencyFlag)
    {
      m_iNumTileBands = m_iNumBands = 4 - m_pImagePlane->iBands;
      m_iIndexSize = m_iTilesTotal * m_iNumBands;
    }
    else
      m_iIndexSize = m_iTilesTotal;

    m_BitStreamIn.ReadBits(16, iBits);
    if(iBits != 1)
      return ExcStatusFail;

    m_IndexTable = (Ipp64u*)ippsMalloc_8u(m_iIndexSize * sizeof(Ipp64u) + sizeof(Ipp64u));
    for(i = 0; i < m_iIndexSize; i++)
      m_IndexTable[i] = readVLWESC(m_BitStreamIn);
  }

  iBytes = readVLWESC(m_BitStreamIn);
  // PROFILE LEVEL INFO according to JPEGXR spec. Must be zero for HDP.
  if(iBytes > 0)
  {
    do
    {
      m_BitStreamIn.ReadBits(8,  iProfileIDC);
      m_BitStreamIn.ReadBits(8,  iLevelIDC);
      m_BitStreamIn.ReadBits(15, iReservedL);
      m_BitStreamIn.ReadBits(1,  iBits);
      iNumBytes += 4;
    } while(iBits != 1);

    for(i = 0; i < (iBytes - iNumBytes); i++)
      m_BitStreamIn.ReadBits(8, iReservedA);
  }

  // Offset to the end of last tile. For proper offset during non-direct tile reading.
  m_iFirstOffset = m_BitStreamIn.GetTotalPos();
  if(m_bIndexFlag)
    m_IndexTable[m_iIndexSize] = m_BitStreamIn.m_fileSize - m_iFirstOffset;

  return ExcStatusOk;
} // readIndexTable()


ExcStatus CJPEGXRTileDecoder::readMacroblock()
{
#ifdef USE_TBB
  tbb::spin_mutex::scoped_lock lock;
#endif

  Ipp32u iBands = m_pImagePlane->iBands;

  if(!(m_iBorder & JXR_BORDER_RIGHT) && !(m_iBorder & JXR_BORDER_BOTTOM))
  {
#ifdef USE_TBB
    lock.acquire(m_pParent->m_Mutex);
#endif
    for(Ipp32u i = 0; i < m_pParent->m_iNumBands; i++)
    {
      if(m_Stream[i].GetDataLen() < m_pParent->m_iExMBSize + 1000)
        RET_(m_Stream[i].FillBuffer())
    }
#ifdef USE_TBB
    lock.release();
#endif

    RET_(readMacroblockDC());

    if(iBands < JXR_SB_DCONLY)
      RET_(readMacroblockLP());

    RET_(predDCLP());
    RET_(dequantDCLP());

    if(iBands < JXR_SB_NOHP)
      RET_(readMacroblockHP());
  }

  return ExcStatusOk;
} // readMacroblock()


ExcStatus CJPEGXRTileDecoder::readTile()
{
  Ipp32u iDstStep = m_pParent->m_iStep;
  PredictorInf* pPred;
  IppiSize      roi;
  Ipp32u m_iChannels  = m_pParent->m_iChannels;
  Ipp32u iChannels    = m_pImagePlane->iChannels;
  Ipp32u iTileStartCode;
  Ipp32u iTileLocationHash;
  Ipp8u iLE = m_pParent->m_iLeftExtra;
  Ipp8u iTE = m_pParent->m_iTopExtra;
  Ipp8u iRE = m_pParent->m_iRightExtra;
  Ipp8u iBE = m_pParent->m_iBottomExtra;
  Ipp32u iCurTileWidth;
  Ipp32u iCurTileHeight;
  Ipp32u iDstShift = 0;
  Ipp32u iSrcShift = 0;
  Ipp32u i;

  m_iCurTileMBWidth = m_pParent->m_iTileMBWidth[m_iTileColumn];
  m_iCurTileMBHeight = m_pParent->m_iTileMBHeight[m_iTileRow];
  iCurTileWidth  = m_iCurTileMBWidth * 16;
  iCurTileHeight = m_iCurTileMBHeight * 16;

  if(m_iTileColumn == 0)
    iCurTileWidth -= iLE;
  else
    iLE = 0;

  if(m_iTileRow == 0)
    iCurTileHeight -= iTE;
  else
    iTE = 0;

  if(m_iTileColumn == (m_pParent->m_iTilesWidth - 1))
  {
    if(m_pParent->m_bWindowingFlag)
      iCurTileWidth -= iRE;
    else if(iRE)
      iCurTileWidth = (m_iCurTileMBWidth - 1) * 16 + iRE;
  }

  if(m_iTileRow == (m_pParent->m_iTilesHeight - 1))
  {
    if(m_pParent->m_bWindowingFlag)
      iCurTileHeight -= iBE;
    else if(iBE)
      iCurTileHeight = (m_iCurTileMBHeight - 1) * 16 + iBE;
  }

  for(i = 0; i < m_pParent->m_iNumBands; i++)
  {
    RET_(m_Stream[i].ReadBits(24, iTileStartCode));
    if(1 != iTileStartCode) return ExcStatusFail;
    RET_(m_Stream[i].ReadBits(5, iTileLocationHash));
    RET_(m_Stream[i].ReadBits(3, m_iTileType));

    if(m_pParent->m_bTrimFlexFlag && (m_iTileType == JXR_TT_SPATIAL || m_iTileType == JXR_TT_FB))
      RET_(m_Stream[i].ReadBits(4, m_iTrimFlex));
  }

  for(m_iMBRow = 0; m_iMBRow <= m_iCurTileMBHeight; m_iMBRow++)
  {
    iSrcShift = ((m_iMBRow - 1) * 16 < iTE)?iTE & 0xf:0;
    roi = m_pParent->m_lRoi;

    for (i = 0; i < m_iChannels; i++)
    {
      m_ImgBuf[i].pHufPtr = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pImgBuf + m_ImgBuf[i].iStep * roi.height * m_iMBRow);
      m_ImgBuf[i].pCCPtr  = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pImgBuf + m_ImgBuf[i].iStep * (roi.height * (m_iMBRow - 1) + iSrcShift) + iLE * m_pParent->m_iCUSize);
      m_ImgBuf[i].pPCTPtr = m_ImgBuf[i].pHufPtr;
      roi = ((i >= (iChannels - 1))?m_pParent->m_lRoi:m_pParent->m_cRoi);
    }

    for(m_iMBColumn = 0; m_iMBColumn <= m_iCurTileMBWidth; m_iMBColumn++)
    {
      m_iBorderPCT   = m_iBorder = 0;
      m_iMBColumnPCT = m_iMBColumn;
      m_iMBRowPCT    = m_iMBRow;

      // Huffman borders
      if(m_iMBColumn == 0)
        m_iBorder |= JXR_BORDER_LEFT;
      if(m_iMBRow == 0)
        m_iBorder |= JXR_BORDER_TOP;
      if(m_iMBColumn == m_iCurTileMBWidth)
        m_iBorder |= JXR_BORDER_RIGHT;
      if(m_iMBRow == m_iCurTileMBHeight)
        m_iBorder |= JXR_BORDER_BOTTOM;

      // PCT borders
      if(1/*m_pParent->m_bHardTilingFlag*/)
      {
        if(m_iMBColumn != 0)
          m_iBorderPCT |= ippBorderInMemLeft;
        if(m_iMBRow != 0)
          m_iBorderPCT |= ippBorderInMemTop;
        if(m_iMBColumn != m_iCurTileMBWidth)
          m_iBorderPCT |= ippBorderInMemRight;
        if(m_iMBRow != m_iCurTileMBHeight)
          m_iBorderPCT |= ippBorderInMemBottom;
      }
      else
      {
        if((m_iCurTileMBColumn + m_iMBColumn) != 0)
          m_iBorderPCT |= ippBorderInMemLeft;
        if((m_iCurTileMBRow + m_iMBRow) != 0)
          m_iBorderPCT |= ippBorderInMemTop;
        if((m_iCurTileMBColumn + m_iMBColumn) != m_pParent->m_iMBTotalWidth)
          m_iBorderPCT |= ippBorderInMemRight;
        if((m_iCurTileMBRow + m_iMBRow) != m_pParent->m_iMBTotalHeight)
          m_iBorderPCT |= ippBorderInMemBottom;
      }

      m_bResetContext = m_bResetTotals = ((m_iMBColumn & 0xf) == 0);
      if(m_iMBColumn + 1 == m_iCurTileMBWidth)
        m_bResetContext = 1;

      RET_(readMacroblock());
      RET_(PCTransform());
      if(m_pParent->m_bAlphaFlag)
      {
        m_pHuffmanPlane = &m_HuffmanPlane[JXR_PLANE_ALPHA];
        m_pImagePlane = m_pImagePlanePCT = &m_pParent->m_ImagePlane[JXR_PLANE_ALPHA];
        m_bAlphaProcess = m_bAlphaProcessPCT = 1;

        RET_(readMacroblock());
        RET_(PCTransform());

        m_pHuffmanPlane = &m_HuffmanPlane[JXR_PLANE_PRIMARY];
        m_pImagePlane = m_pImagePlanePCT = &m_pParent->m_ImagePlane[JXR_PLANE_PRIMARY];
        m_bAlphaProcess = m_bAlphaProcessPCT = 0;
      }

      roi = m_pParent->m_lRoi;
      for(i = 0; i < m_iChannels; i++)
      {
        m_ImgBuf[i].pHufPtr = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pHufPtr + roi.width * m_pParent->m_iCUSize);
        m_ImgBuf[i].pPCTPtr = m_ImgBuf[i].pHufPtr;
        roi = ((i >= (iChannels - 1))?m_pParent->m_lRoi:m_pParent->m_cRoi);
      }
    }

    if((m_iMBRow * 16 > iTE) && ((m_iMBRow - 1) * 16 < (iCurTileHeight + iTE)))
    {
      roi.height = iCurTileHeight + iTE - (m_iMBRow - 1) * 16;
      if(roi.height > 16) roi.height = 16;
      roi.width  = iCurTileWidth;

      iDstShift = iDstStep * (16 * (m_iMBRow - 1) - iTE);
      if((m_iMBRow - 1) * 16 < iTE)
      {
        iDstShift = 0;
        roi.height -= iTE & 0xf;
      }

      RET_(colorConvert(m_pDst + iDstShift, iDstStep, roi));
    }

    for(i = 0; i < m_iChannels; i++) // swap current row and previous row
    {
      pPred          = m_pPred[i];
      m_pPred[i]     = m_pPredPrev[i];
      m_pPredPrev[i] = pPred;
    }
  }

  for(i = 0; i < m_pParent->m_iNumBands; i++)
    RET_(m_Stream[i].FlushToByte())

  return ExcStatusOk;
} // readTile()


ExcStatus CJPEGXRDecoder::readFrame(void)
{
  Ipp32u iStartRow = 0;
  Ipp32u iStartColumn = 0;
  Ipp32u iEndColumn = m_iTilesWidth;
  Ipp32u iEndRow = m_iTilesHeight;

  if(m_bReadTile)
  {
    iStartRow = m_iReadTileRow;
    iStartColumn = m_iReadTileColumn;
    iEndRow = m_iReadTileRow + 1;
    iEndColumn = m_iReadTileColumn + 1;
  }

  CJPEGXRTileDecoder* pDecoder = new CJPEGXRTileDecoder;
  pDecoder->initDecoder(this);

  for(pDecoder->m_iTileRow = (Ipp16u)iStartRow; pDecoder->m_iTileRow < iEndRow; pDecoder->m_iTileRow++)
  {
    for(pDecoder->m_iTileColumn = (Ipp16u)iStartColumn; pDecoder->m_iTileColumn < iEndColumn; pDecoder->m_iTileColumn++)
    {
      RET_(pDecoder->resetDecoder());

#ifdef USE_TBB
      if(m_iPipeThreads > 1)
        RET_(pDecoder->readTileTBB())
      else
#endif
        RET_(pDecoder->readTile())
    }
  }

  delete pDecoder;

  return ExcStatusOk;
} // ReadFrame()


ExcStatus CJPEGXRDecoder::ReadData(void)
{
#ifdef USE_TBB
  if(m_bReadTile)
    m_iThreadMode = 0;

  if(!m_bThreadsInit)
  {
    switch(m_iThreadMode)
    {
    case 0:
      if(m_iThreads > m_iPipeLength)
        m_iPipeThreads = m_iThreads = m_iPipeLength;
      else
        m_iPipeThreads = m_iThreads;
      break;

    case 1:
      if(m_iThreads > m_iTilesTotal)
        m_iTileThreads = m_iThreads = m_iTilesTotal;
      else
        m_iTileThreads = m_iThreads;
      break;

    case 2:
      if(m_iThreads >= m_iTilesTotal * m_iPipeLength)
      {
        m_iThreads = m_iTilesTotal * m_iPipeLength;
        m_iPipeThreads = m_iTilesTotal * m_iPipeLength;
        m_iTileThreads = m_iTilesTotal;
      }
      else
      {
        m_iTileThreads = m_iThreads;
        if(m_iThreads > m_iTilesTotal)
        {
          Ipp16u iThreads = m_iThreads;
          m_iPipeThreads = 0;

          m_pPipeArray = ippsMalloc_16u(m_iTilesTotal);

          for(int i = m_iTilesTotal; i >= 1; i--)
          {
            m_pPipeArray[i - 1] = iThreads / i;
            if(m_pPipeArray[i - 1] > m_iPipeLength)
              m_pPipeArray[i - 1] = m_iPipeLength;
            iThreads -= m_pPipeArray[i - 1];
            m_iPipeThreads += m_pPipeArray[i - 1];
          }

          m_iTileThreads   = m_iTilesTotal;
          m_bPipeArrayInit = 1;
        }
      }
      break;

    default:
      return ExcStatusFail;
    }

    if(m_iPipeThreads > 1 && m_iTileThreads > 1)
      scheduler->initialize(m_iPipeThreads);
    else if(m_iPipeThreads > 1)
      scheduler->initialize(m_iPipeThreads);
    else if(m_iTileThreads > 1)
      scheduler->initialize(m_iTileThreads);

    m_bThreadsInit = 1;
  }
#endif

  if(m_bFirstLoop)
  {
    RET_(initDecoder());
    m_bFirstLoop = 0;
  }

  if(m_bFrequencyFlag && m_iBandsLimit != JXR_SB_ALL)
  {
    m_iNumBands = 4 - m_iBandsLimit;
    m_ImagePlane[0].iBands = m_iBandsLimit;
  }


#ifdef USE_TBB
  if(m_iTileThreads > 1)
    RET_(readFrameTBB())
  else
#endif
    RET_(readFrame())

  return ExcStatusOk;
} // ReadData()


ExcStatus CJPEGXRDecoder::ReadTile(
  const ImageDataPtr*   dataPtr,
  const ImageDataOrder* dataOrder,
        Ipp32u          iTileRow,
        Ipp32u          iTileColumn)
{
  if(iTileRow >= m_iTilesHeight || iTileColumn >= m_iTilesWidth)
    return ExcStatusFail;

  if(m_iTilesHeight != 1 || m_iTilesWidth != 1)
    m_bReadTile = 1;

  m_iReadTileRow    = (Ipp16u)iTileRow;
  m_iReadTileColumn = (Ipp16u)iTileColumn;
  m_pData     = dataPtr->p8u;
  m_iStep     = *dataOrder->LineStep();
  m_iUnitSize = (Ipp8u)NOfBytes(dataOrder->DataType());

  RET_(ReadData());
  m_bReadTile = 0;

  return ExcStatusOk;
} // ReadTile()


ExcStatus CJPEGXRDecoder::ReadFileHeader(Ipp32u& iImageOffset, Ipp32u& iAlphaOffset, Ipp8u& bPreAlpha)
{
  Ipp8u  iPixFormatCode[16] = {0x24,0xc3,0xdd,0x6f,0x03,0x4e,0xfe,0x4b,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9, 0};
  Ipp8u  iJXRID;
  Ipp8u  iJXRVersion;
  Ipp16u iByteOrder;
  Ipp32u iIFDOffset;
  Ipp16u iIFDCount;
  Ipp16u iTAG, iType;
  Ipp32u iCount, iValue;
  Ipp16u i;

  RET_(m_BitStreamIn.Read(iByteOrder));
  if(JXR_ORDER != iByteOrder)
    return ExcStatusFail;
  RET_(m_BitStreamIn.Read(iJXRID));
  if(JXR_ID != iJXRID)
    return ExcStatusFail;
  RET_(m_BitStreamIn.Read(iJXRVersion));
  if(JXR_VERSION != iJXRVersion)
    return ExcStatusFail;
  RET_(m_BitStreamIn.Read(iIFDOffset));

  RET_(m_BitStreamIn.Seek(iIFDOffset, BaseStreamInput::Beginning));
  RET_(m_BitStreamIn.Read(iIFDCount));

  for(i = 0; i < iIFDCount; i++)
  {
    RET_(m_BitStreamIn.Read(iTAG));
    RET_(m_BitStreamIn.Read(iType));
    RET_(m_BitStreamIn.Read(iCount));
    RET_(m_BitStreamIn.Read(iValue));

    switch((JXR_TAG)iTAG)
    {
    case JXRT_PIXEL_FORMAT:
      readIFDValue(iType, iCount, iValue, &iPixFormatCode[0]);
      break;
    case JXRT_IMAGE_WIDTH:           break;
    case JXRT_IMAGE_HEIGHT:          break;
    case JXRT_WIDTH_RESOLUTION:      break;
    case JXRT_HEIGHT_RESOLUTION:     break;
    case JXRT_IMAGE_OFFSET:
      m_iImageOffset = iValue;
      break;
    case JXRT_IMAGE_BYTE_COUNT:
      m_iImageNumBytes = iValue;
      break;
    case JXRT_ALPHA_OFFSET:
      m_iAlphaOffset = iValue;
      break;
    case JXRT_ALPHA_BYTE_COUNT:
      m_iAlphaNumBytes = iValue;
      break;
    default:
        break;
    }
  }

  switch(iPixFormatCode[15])
  {
  case JXR_PF_BGR_8U_C3:
  case JXR_PF_BGR_8U_C4:
  case JXR_PF_BGRA_8U_C4:
  case JXR_PF_PBGRA_8U_C4:
  case JXR_PF_BGR_5U_C3:
  case JXR_PF_BGR_565U_C3:
  case JXR_PF_BGR_10U_C3:
    m_bBGR = 1;
    break;
  }

  switch(iPixFormatCode[15])
  {
  case JXR_PF_BGR_8U_C4:
  case JXR_PF_RGB_16S_C4:
  case JXR_PF_RGB_16F_C4:
  case JXR_PF_RGB_32S_C4:
  case JXR_PF_RGB_32F_C4:
  case JXR_PF_RGBE_8U_C3:
    m_bFakeAlpha = 1;
    break;
  }

  switch(iPixFormatCode[15])
  {
  case JXR_PF_PBGRA_8U_C4:
  case JXR_PF_PRGBA_16U_C4:
  case JXR_PF_PRGBA_32F_C4:
    m_bPreAlpha = 1;
    break;
  }

  // unsupported formats
  switch(iPixFormatCode[15])
  {
  case JXR_PF_GRAY_1U:
  case JXR_PF_BGR_5U_C3:
  case JXR_PF_BGR_565U_C3:
  case JXR_PF_BGR_10U_C3:
  case JXR_PF_YCC444_10U_C3:
  case JXR_PF_YCCA444_10U_C4:
    return ExcStatusFail;
  }

  iImageOffset = m_iImageOffset;
  iAlphaOffset = m_iAlphaOffset;
  bPreAlpha    = m_bPreAlpha;

  return ExcStatusOk;
} // ReadFileHeader()


ExcStatus CJPEGXRDecoder::ReadHeader(
  int*                 width,
  int*                 height,
  Ipp32u*              nchannels,
  ImageEnumColorSpace* color,
  ImageEnumSampling*   sampling,
  Ipp32u*              precision)
{
  RET_(readImageHeader(m_iCurrentOffset));
  RET_(readImagePlaneHeader());

  m_iChannels = m_pImagePlane->iChannels;

  if(m_iCurrentOffset == m_iAlphaOffset)
    m_bPAlphaProcess = 1;
  else
    m_bPAlphaProcess = 0;

  if(m_bAlphaFlag)
  {
    m_iACHOffset = m_pImagePlane->iChannels;
    m_pImagePlane = &m_ImagePlane[JXR_PLANE_ALPHA]; // Switch image plane

    RET_(readImagePlaneHeader());

    m_iChannels += m_pImagePlane->iChannels;
    m_pImagePlane = &m_ImagePlane[JXR_PLANE_PRIMARY];
  }

  RET_(readIndexTable());

  if(m_iForceColor == JXR_OCF_AUTO)
    m_iOutColorFormat = m_iSrcColorFormat;

  // unsupported formats
  switch(m_iOutColorFormat)
  {
  case JXR_OCF_YUV420:
  case JXR_OCF_YUV422:
    return ExcStatusFail;
  }

  *sampling  = (ImageEnumSampling)m_iSrcColorFormat;
  *color     = (ImageEnumColorSpace)m_iSrcColorFormat;
  *width     = m_iWidth;
  *height    = m_iHeight;
  *nchannels = (m_bFakeAlpha)?m_iChannels + 1:m_iChannels;
  *precision = m_iBitDepth;

  return ExcStatusOk;
} // ReadHeader()

