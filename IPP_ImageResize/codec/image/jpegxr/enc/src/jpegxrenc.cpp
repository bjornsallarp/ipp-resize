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
#pragma warning ( disable : 4100 )
#endif
#include "jpegxrenc.h"
#ifndef __IPPCORE_H__
#include "ippcore.h"
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


CJPEGXREncoder::CJPEGXREncoder(void)
{
  m_pImagePlane = &m_ImagePlane[JXR_PLANE_PRIMARY];
  ippsSet_8u(0, (Ipp8u*)&m_ImagePlane[0], sizeof(ImagePlane));
  ippsSet_8u(0, (Ipp8u*)&m_ImagePlane[1], sizeof(ImagePlane));
  m_ImagePlane[0].bDCFrameUniform = m_ImagePlane[1].bDCFrameUniform = 1;
  m_ImagePlane[0].bLPFrameUniform = m_ImagePlane[1].bLPFrameUniform = 1;
  m_ImagePlane[0].bHPFrameUniform = m_ImagePlane[1].bHPFrameUniform = 1;
  m_ImagePlane[0].iMantiss = m_ImagePlane[1].iMantiss = 13;
  m_ImagePlane[0].iExp = m_ImagePlane[1].iExp = 4;
  m_ImagePlane[0].iShift = m_ImagePlane[1].iShift = 0xff;

  m_iInputQuant = 1;
  m_iYQuant = m_iUVQuant = 0;
  m_iTileType = JXR_TT_SPATIAL;
  m_bAlphaFlag = 0;
  m_bBGR = 0;
  m_bPAlphaProcess = 0;
  m_iImageOffset = 0;
  m_iImageNumBytes = 0;
  m_iAlphaOffset = 0;
  m_iAlphaNumBytes = 0;
  m_iTilesUniform[0] = 1;
  m_iTilesUniform[1] = 1;
  m_iTilesUniform[2] = 0;
  m_iTilesUniform[3] = 0;
  m_iTilesTotal = 0;
  m_iUnitSize = 1;
  m_iTrimFlex = 0;
  m_iACHOffset = 0;
  m_bAlphaProcess = 0;
  m_bFakeAlpha = 0;
  m_bPreAlpha = 0;
  m_bCMYKD    = 0;

  m_bHardTilingFlag = 1;
  m_bTilingFlag = 0;
  m_bFrequencyFlag = 0;
  m_iOrientation = 0;
  m_bIndexFlag = 0;
  m_iOverlap = 1;
  m_iSampling = 0;
  m_bShortHeaderFlag = 1;
  m_bLongWordFlag = 1;
  m_bWindowingFlag = 0;
  m_bTrimFlexFlag = 0;

  m_iLeftExtra = 0;
  m_iTopExtra = 0;
  m_iRightExtra = 0;
  m_iBottomExtra = 0;

  m_iIFDSize = 0;
  m_iIOEntryOffset = 0;
  m_iILEntryOffset = 0;
  m_iAOEntryOffset = 0;
  m_iALEntryOffset = 0;

  m_bFirstLoop = 1;
  return;
} // ctor


CJPEGXREncoder::~CJPEGXREncoder(void)
{
  if(!m_bFirstLoop)
    KillEncoder();
  return;
} // dtor


ExcStatus CJPEGXREncoder::AttachStream(BaseStreamOutput& out)
{
  RET_(m_BitStreamOut.Attach(out));
  RET_(m_BitStreamOut.Init());

  return ExcStatusOk;
} // AttachStream()


ExcStatus CJPEGXREncoder::SetParams(Ipp8u iQuality, Ipp8u iOverlap, Ipp8u iBands,
    Ipp8u iSampling, Ipp8u iTrim, Ipp8u iShift, Ipp8u bFrequency, Ipp8u bCMYKD, Ipp8u bAlphaPlane, Ipp16u* iTilesUniform)
{
  m_iInputQuant = iQuality;
  m_iOverlap    = iOverlap;
  m_bCMYKD      = bCMYKD;
//  m_iSampling   = iSampling;
//  m_bFrequencyFlag = iBitstream;
/*
  if(m_pImagePlane->iColorFormat == JXR_ICF_YUV444)
  {
    if(m_iSampling == 1)
      m_pImagePlane->iColorFormat = JXR_ICF_YUV422;
    else if(m_iSampling == 2)
      m_pImagePlane->iColorFormat = JXR_ICF_YUV420;
  }
*/
  if(iTrim)
    m_bTrimFlexFlag = 1;
  m_iTrimFlex = iTrim;

  m_pImagePlane->iBands = iBands;
  m_pImagePlane = &m_ImagePlane[JXR_PLANE_ALPHA];
  m_pImagePlane->iBands = iBands;
  m_pImagePlane = &m_ImagePlane[JXR_PLANE_PRIMARY];

  m_iTilesUniform[0] = iTilesUniform[0];
  m_iTilesUniform[1] = iTilesUniform[1];
  m_iTilesUniform[2] = iTilesUniform[2];
  m_iTilesUniform[3] = iTilesUniform[3];

  if(bAlphaPlane == 0)
    m_bPAlphaProcess = 0;
  else
    m_bPAlphaProcess = 1;

  if((m_iTilesUniform[0] * m_iTilesUniform[1]) != 1)
    m_bTilingFlag = 1;

  if((m_iTilesUniform[2] + m_iTilesUniform[3]) != 0)
    m_bTilingFlag = 1;

  if(m_bCMYKD && m_iSrcColorFormat == JXR_OCF_CMYK)
    m_iSrcColorFormat = JXR_OCF_CMYKD;

  m_ImagePlane[0].iShift = m_ImagePlane[1].iShift = iShift;

  return ExcStatusOk;
} // SetParams()


static Ipp8u setPixDesc(Ipp32u iBitDepth, Ipp32u iOutColorFormat, Ipp32u iChannels, Ipp32u iAlpha, Ipp32u bBGR)
{
  switch(iBitDepth)
  {
  case JXR_BD_1W:
  case JXR_BD_1B:
    return (Ipp8u)JXR_PF_GRAY_1U;
    break;
  case JXR_BD_8:
    if(iOutColorFormat == JXR_OCF_Y)
      return (Ipp8u)JXR_PF_GRAY_8U;
    else if(iOutColorFormat == JXR_OCF_RGB)
    {
      if(iAlpha == 2)
        return (Ipp8u)JXR_PF_PBGRA_8U_C4;
      else if(iAlpha)
        return (Ipp8u)JXR_PF_BGRA_8U_C4;
      else
      {
        if(bBGR)
          return iChannels == 4? (Ipp8u)JXR_PF_BGR_8U_C4 : (Ipp8u)JXR_PF_BGR_8U_C3;
        else
          return (Ipp8u)JXR_PF_RGB_8U_C3;
      }
    }
    else if(iOutColorFormat == JXR_OCF_RGBE)
      return (Ipp8u)JXR_PF_RGBE_8U_C3;
    else if(iOutColorFormat == JXR_OCF_CMYK)
    {
      if(iAlpha)
        return (Ipp8u)JXR_PF_CMYKA_8U_C5;
      else
        return (Ipp8u)JXR_PF_CMYK_8U_C4;
    }
    else if(iOutColorFormat == JXR_OCF_CMYKD)
    {
      if(iAlpha)
        return (Ipp8u)JXR_PF_CMYKDA_8U_C5;
      else
        return (Ipp8u)JXR_PF_CMYKD_8U_C4;
    }
    else if(iOutColorFormat == JXR_OCF_NCH)
    {
      if(iAlpha)
      {
        if(iChannels == 4)
          return (Ipp8u)JXR_PF_NCHA_8U_C4;
        else if(iChannels == 5)
          return (Ipp8u)JXR_PF_NCHA_8U_C5;
        else if(iChannels == 6)
          return (Ipp8u)JXR_PF_NCHA_8U_C6;
        else if(iChannels == 7)
          return (Ipp8u)JXR_PF_NCHA_8U_C7;
        else if(iChannels == 8)
          return (Ipp8u)JXR_PF_NCHA_8U_C8;
        else if(iChannels == 9)
          return (Ipp8u)JXR_PF_NCHA_8U_C9;
      }
      else
      {
        if(iChannels == 3)
          return (Ipp8u)JXR_PF_NCH_8U_C3;
        else if(iChannels == 4)
          return (Ipp8u)JXR_PF_NCH_8U_C4;
        else if(iChannels == 5)
          return (Ipp8u)JXR_PF_NCH_8U_C5;
        else if(iChannels == 6)
          return (Ipp8u)JXR_PF_NCH_8U_C6;
        else if(iChannels == 7)
          return (Ipp8u)JXR_PF_NCH_8U_C7;
        else if(iChannels == 8)
          return (Ipp8u)JXR_PF_NCH_8U_C8;
      }
    }
    break;
  case JXR_BD_16:
    if(iOutColorFormat == JXR_OCF_Y)
      return (Ipp8u)JXR_PF_GRAY_16U;
    else if(iOutColorFormat == JXR_OCF_RGB)
    {
      if(iAlpha == 2)
        return (Ipp8u)JXR_PF_PRGBA_16U_C4;
      else if(iAlpha)
        return (Ipp8u)JXR_PF_RGBA_16U_C4;
      else
        return (Ipp8u)JXR_PF_RGB_16U_C3;
    }
    else if(iOutColorFormat == JXR_OCF_CMYK)
    {
      if(iAlpha)
        return (Ipp8u)JXR_PF_CMYKA_16U_C5;
      else
        return (Ipp8u)JXR_PF_CMYK_16U_C4;
    }
    else if(iOutColorFormat == JXR_OCF_CMYKD)
    {
      if(iAlpha)
        return (Ipp8u)JXR_PF_CMYKDA_16U_C5;
      else
        return (Ipp8u)JXR_PF_CMYKD_16U_C4;
    }
    else if(iOutColorFormat == JXR_OCF_NCH)
    {
      if(iAlpha)
      {
        if(iChannels == 4)
          return (Ipp8u)JXR_PF_NCHA_16U_C4;
        else if(iChannels == 5)
          return (Ipp8u)JXR_PF_NCHA_16U_C5;
        else if(iChannels == 6)
          return (Ipp8u)JXR_PF_NCHA_16U_C6;
        else if(iChannels == 7)
          return (Ipp8u)JXR_PF_NCHA_16U_C7;
        else if(iChannels == 8)
          return (Ipp8u)JXR_PF_NCHA_16U_C8;
        else if(iChannels == 9)
          return (Ipp8u)JXR_PF_NCHA_16U_C9;
      }
      else
      {
        if(iChannels == 3)
          return (Ipp8u)JXR_PF_NCH_16U_C3;
        else if(iChannels == 4)
          return (Ipp8u)JXR_PF_NCH_16U_C4;
        else if(iChannels == 5)
          return (Ipp8u)JXR_PF_NCH_16U_C5;
        else if(iChannels == 6)
          return (Ipp8u)JXR_PF_NCH_16U_C6;
        else if(iChannels == 7)
          return (Ipp8u)JXR_PF_NCH_16U_C7;
        else if(iChannels == 8)
          return (Ipp8u)JXR_PF_NCH_16U_C8;
      }
    }
    break;
  case JXR_BD_16S:
    if(iOutColorFormat == JXR_OCF_Y)
      return (Ipp8u)JXR_PF_GRAY_16S;
    else if(iOutColorFormat == JXR_OCF_RGB)
    {
      if(iAlpha)
        return (Ipp8u)JXR_PF_RGBA_16S_C4;
      else
        return iChannels == 4? (Ipp8u)JXR_PF_RGB_16S_C4 : (Ipp8u)JXR_PF_RGB_16S_C3;
    }
    break;
  case JXR_BD_16F:
    if(iOutColorFormat == JXR_OCF_Y)
      return (Ipp8u)JXR_PF_GRAY_16F;
    else if(iOutColorFormat == JXR_OCF_RGB)
    {
      if(iAlpha)
        return (Ipp8u)JXR_PF_RGBA_16F_C4;
      else
        return iChannels == 4? (Ipp8u)JXR_PF_RGB_16F_C4 : (Ipp8u)JXR_PF_RGB_16F_C3;
    }
    break;
  case JXR_BD_32S:
    if(iOutColorFormat == JXR_OCF_Y)
      return (Ipp8u)JXR_PF_GRAY_32S;
    else if(iOutColorFormat == JXR_OCF_RGB)
    {
      if(iAlpha)
        return (Ipp8u)JXR_PF_RGBA_32S_C4;
      else
        return iChannels == 4? (Ipp8u)JXR_PF_RGB_32S_C4 : (Ipp8u)JXR_PF_RGB_32S_C3;
    }
    break;
  case JXR_BD_32F:
    if(iOutColorFormat == JXR_OCF_Y)
      return (Ipp8u)JXR_PF_GRAY_32F;
    else if(iOutColorFormat == JXR_OCF_RGB)
    {
      if(iAlpha == 2)
        return (Ipp8u)JXR_PF_PRGBA_32F_C4;
      if(iAlpha)
        return (Ipp8u)JXR_PF_RGBA_32F_C4;
      else
        return (Ipp8u)JXR_PF_RGB_32F_C4;
    }
    break;
  case JXR_BD_5:
    return JXR_PF_BGR_5U_C3;
    break;
  case JXR_BD_10:
    return JXR_PF_BGR_10U_C3;
    break;
  case JXR_BD_565:
    return JXR_PF_BGR_565U_C3;
    break;
  }
  return 0;
}


ExcStatus CJPEGXREncoder::writeIFDValue(Ipp32u iTAG, Ipp32u iType, Ipp32u iCount, void* pValue)
{
  Ipp32u iTypeSize = typeSize[iType];
  Ipp16u *pTAG   = (Ipp16u*)&m_pIFDBuffer[m_iIFDSize];
  Ipp16u *pType  = (Ipp16u*)&m_pIFDBuffer[m_iIFDSize + 2];
  Ipp32u *pCount = (Ipp32u*)&m_pIFDBuffer[m_iIFDSize + 4];
  Ipp32u *pVO    = (Ipp32u*)&m_pIFDBuffer[m_iIFDSize + 8];
  Ipp32u iValue  = *((Ipp32u*)pValue);
  Ipp32u iDenum  = 10000;
  Ipp32u iOffset = m_BitStreamOut.GetTotalPos();
  Ipp32u iCurOffset = 0;
  Ipp32u i;

  if(iTypeSize*iCount > 4)
  {
    switch(iTAG)
    {
    case JXRT_IMAGE_OFFSET:
      m_iIOEntryOffset = iOffset; break;
    case JXRT_IMAGE_BYTE_COUNT:
      m_iILEntryOffset = iOffset; break;
    case JXRT_ALPHA_OFFSET:
      m_iAOEntryOffset = iOffset; break;
    case JXRT_ALPHA_BYTE_COUNT:
      m_iALEntryOffset = iOffset; break;
    }

    if(iType == JXR_TT_RATIONAL)
    {
      Ipp32u iNum;

      for(i = 0; i < iCount; i++)
      {
        Ipp32f fValue = *((Ipp32f*)pValue + i*4);
        iNum = (Ipp32u)fValue*iDenum;

        m_BitStreamOut.Write(iNum);
        m_BitStreamOut.Write(iDenum);
        iOffset = 8;
      }
    }
    else
      m_BitStreamOut.Write((Ipp8u*)pValue, iTypeSize*iCount);

    iCurOffset = m_BitStreamOut.GetTotalPos() - iOffset;
  }

  switch(iTAG)
  {
  case JXRT_IMAGE_OFFSET:
    if(m_iIOEntryOffset == 0)
      m_iIOEntryOffset = iOffset + m_iIFDSize + 10;
    else if(m_iIOEntryOffset > iOffset)
      m_iIOEntryOffset += iCurOffset;
    break;
  case JXRT_IMAGE_BYTE_COUNT:
    if(m_iILEntryOffset == 0)
      m_iILEntryOffset = iOffset + m_iIFDSize + 10;
    else if(m_iILEntryOffset > iOffset)
      m_iILEntryOffset += iCurOffset;
    break;
  case JXRT_ALPHA_OFFSET:
    if(m_iAOEntryOffset == 0)
      m_iAOEntryOffset = iOffset + m_iIFDSize + 10;
    else if(m_iAOEntryOffset > iOffset)
      m_iAOEntryOffset += iCurOffset;
    break;
  case JXRT_ALPHA_BYTE_COUNT:
    if(m_iALEntryOffset == 0)
      m_iALEntryOffset = iOffset + m_iIFDSize + 10;
    else if(m_iALEntryOffset > iOffset)
      m_iALEntryOffset += iCurOffset;
    break;
  }

  *pTAG   = (Ipp16u)iTAG;
  *pType  = (Ipp16u)iType;
  *pCount = iCount;
  *pVO    = (iCurOffset)?iOffset:iValue;

  m_iIFDSize += JXR_TAG_SIZE;

  return ExcStatusOk;
} // writeIFDValue()


ExcStatus CJPEGXREncoder::writeIFDTable(void)
{
  Ipp16u iTAGs = (Ipp16u)(m_iIFDSize/JXR_TAG_SIZE);
  Ipp32u iOffset;

  m_BitStreamOut.Write(iTAGs);
  iOffset = m_BitStreamOut.GetTotalPos();
  m_BitStreamOut.Write(m_pIFDBuffer, m_iIFDSize);
  iOffset = m_BitStreamOut.GetTotalPos() - iOffset;
  if(iOffset != m_iIFDSize)
    return ExcStatusFail;

  return ExcStatusOk;
} // writeIFDTable()


ExcStatus CJPEGXREncoder::writeJXRImageHeader()
{
  Ipp8u  iGDISignature[8] = {0x57,0x4d,0x50,0x48,0x4f,0x54,0x4f,0};
  Ipp32u iBitsLenLo, iBitsLenHi;
  Ipp32u iMBWidth, iTileMBWidth;
  Ipp32u iMBHeight, iTileMBHeight;
  Ipp32u iReservedA = 1;
  Ipp32u iReservedB = 0;
  Ipp32u i;

  // Write image offset
  if(m_bPAlphaProcess)
  {
    m_iAlphaOffset = m_BitStreamOut.GetTotalPos();
    m_BitStreamOut.Seek(m_iAOEntryOffset);
    m_BitStreamOut.Write(m_iAlphaOffset);
    m_BitStreamOut.Seek(m_iAlphaOffset);
  }
  else
  {
    m_iImageOffset = m_BitStreamOut.GetTotalPos();
    m_BitStreamOut.Seek(m_iIOEntryOffset);
    m_BitStreamOut.Write(m_iImageOffset);
    m_BitStreamOut.Seek(m_iImageOffset);
  }

  m_iMBTotalWidth = m_iWidth/16;
  m_iMBTotalHeight = m_iHeight/16;

  if(m_bWindowingFlag)
  {
    m_iTopExtra    = 0;
    m_iLeftExtra   = 0;
    m_iBottomExtra = 0;
    m_iRightExtra  = 0;
  }
  else
  {
    if(m_iWidth & 0xf)
    {
      m_iRightExtra = m_iWidth & 0xf;
      m_iMBTotalWidth++;
    }
    if(m_iHeight & 0xf)
    {
      m_iBottomExtra = m_iHeight & 0xf;
      m_iMBTotalHeight++;
    }
  }
   iMBWidth = m_iMBTotalWidth;
   iMBHeight = m_iMBTotalHeight;

  if(m_iTilesUniform[2] != 0)
  {
    m_iTilesUniform[0] = (Ipp16u)(m_iMBTotalWidth/m_iTilesUniform[2]);
    if(m_iMBTotalWidth%m_iTilesUniform[2])
      m_iTilesUniform[0]++;
    iTileMBWidth  = m_iTilesUniform[2];
  }
  else
    iTileMBWidth = m_iMBTotalWidth/m_iTilesUniform[0];

  if(m_iTilesUniform[3] != 0)
  {
    m_iTilesUniform[1] = (Ipp16u)(m_iMBTotalHeight/m_iTilesUniform[3]);
    if(m_iMBTotalHeight%m_iTilesUniform[3])
      m_iTilesUniform[1]++;
    iTileMBHeight = m_iTilesUniform[3];
  }
  else
    iTileMBHeight = m_iMBTotalHeight/m_iTilesUniform[1];

  if(m_iWidth > 65536 || m_iHeight > 65536 || iMBWidth > 255 || iMBHeight > 255)
    m_bShortHeaderFlag = 0;

  iBitsLenLo = (m_bShortHeaderFlag)?8:16;
  iBitsLenHi = (m_bShortHeaderFlag)?16:32;

  if(m_bFrequencyFlag || m_bTilingFlag)
    m_bIndexFlag = 1;

  for(i = 0; i < 8; i++)
    RET_(m_BitStreamOut.Write(iGDISignature[i]));

  RET_(m_BitStreamOut.WriteBits(4, iReservedA));
  RET_(m_BitStreamOut.WriteBits(1, m_bHardTilingFlag));
  RET_(m_BitStreamOut.WriteBits(3, iReservedA));
  RET_(m_BitStreamOut.WriteBits(1, m_bTilingFlag));
  RET_(m_BitStreamOut.WriteBits(1, m_bFrequencyFlag));
  RET_(m_BitStreamOut.WriteBits(3, m_iOrientation));
  RET_(m_BitStreamOut.WriteBits(1, m_bIndexFlag));
  RET_(m_BitStreamOut.WriteBits(2, m_iOverlap));
  RET_(m_BitStreamOut.WriteBits(1, m_bShortHeaderFlag));
  RET_(m_BitStreamOut.WriteBits(1, m_bLongWordFlag));
  RET_(m_BitStreamOut.WriteBits(1, m_bWindowingFlag));
  RET_(m_BitStreamOut.WriteBits(1, m_bTrimFlexFlag));
  RET_(m_BitStreamOut.WriteBits(3, iReservedB));
  RET_(m_BitStreamOut.WriteBits(1, m_bAlphaFlag));
  RET_(m_BitStreamOut.WriteBits(4, m_iSrcColorFormat));
  RET_(m_BitStreamOut.WriteBits(4, m_iBitDepth));

  RET_(m_BitStreamOut.WriteBits((Ipp8u)iBitsLenHi, m_iWidth - 1));
  RET_(m_BitStreamOut.WriteBits((Ipp8u)iBitsLenHi, m_iHeight - 1));

  if(m_bTilingFlag)
  {
    RET_(m_BitStreamOut.WriteBits(12, m_iTilesUniform[0] - 1));
    RET_(m_BitStreamOut.WriteBits(12, m_iTilesUniform[1] - 1));
  }
  m_iTilesTotal = m_iTilesUniform[0] * m_iTilesUniform[1];

  m_iTileMBWidth = ippsMalloc_16u(m_iTilesUniform[0] + m_iTilesUniform[1]);
  m_iTileMBHeight = m_iTileMBWidth + m_iTilesUniform[0];
  m_iTileMBWidth[0]  = 1;
  m_iTileMBHeight[0] = 1;

  for(i = 0; i < (Ipp32u)m_iTilesUniform[0] - 1; i++)
  {
    if(m_iTilesUniform[2] == 0)
      iTileMBWidth = iMBWidth/(m_iTilesUniform[0] - i);
    iMBWidth -= iTileMBWidth;
    m_iTileMBWidth[i] = (Ipp16u)iTileMBWidth;
    RET_(m_BitStreamOut.WriteBits((Ipp8u)iBitsLenLo, m_iTileMBWidth[i]));
  }

  for(i = 0; i < (Ipp32u)m_iTilesUniform[1] - 1; i++)
  {
    if(m_iTilesUniform[3] == 0)
      iTileMBHeight = iMBHeight/(m_iTilesUniform[1] - i);
    iMBHeight -= iTileMBHeight;
    m_iTileMBHeight[i] = (Ipp16u)iTileMBHeight;
    RET_(m_BitStreamOut.WriteBits((Ipp8u)iBitsLenLo, m_iTileMBHeight[i]));
  }

  m_iTileMBWidth[m_iTilesUniform[0] - 1] = (Ipp16u)iMBWidth;
  m_iTileMBHeight[m_iTilesUniform[1] - 1] = (Ipp16u)iMBHeight;

  if(m_bWindowingFlag)
  {
    RET_(m_BitStreamOut.WriteBits(6, m_iTopExtra));
    RET_(m_BitStreamOut.WriteBits(6, m_iLeftExtra));
    RET_(m_BitStreamOut.WriteBits(6, m_iBottomExtra));
    RET_(m_BitStreamOut.WriteBits(6, m_iRightExtra));
  }

  return ExcStatusOk;
} // writeJXRImageHeader()


ExcStatus CJPEGXREncoder::writeUnifQuantizer(Ipp32u iCHMode, Ipp32u *iQuant)
{
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u i;

  if(iChannels == 1)
    iCHMode = JXR_CM_UNIFORM;
  else
    RET_(m_BitStreamOut.WriteBits(2, iCHMode));

  if(JXR_CM_UNIFORM == iCHMode)
    RET_(m_BitStreamOut.WriteBits(8, iQuant[0]));

  if(JXR_CM_SEPARATE == iCHMode)
  {
    RET_(m_BitStreamOut.WriteBits(8, iQuant[0]));
    RET_(m_BitStreamOut.WriteBits(8, iQuant[1]));
  }

  if(JXR_CM_INDEPENDENT == iCHMode)
  {
    for(i = 0; i < iChannels; i++)
      RET_(m_BitStreamOut.WriteBits(8, iQuant[i]));
  }

  return ExcStatusOk;
} // writeUnifQuantizer()


ExcStatus CJPEGXREncoder::writeJXRImagePlaneHeader()
{
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp8u  iReserved = 0;
  Ipp32u i;

  if((m_iInputQuant > 1 || iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)
    && (m_iBitDepth != JXR_BD_32S && m_iBitDepth != JXR_BD_32F))
    m_pImagePlane->bScalingFlag = 1;

  RET_(m_BitStreamOut.WriteBits(3, m_pImagePlane->iColorFormat));
  RET_(m_BitStreamOut.WriteBits(1, m_pImagePlane->bScalingFlag));
  RET_(m_BitStreamOut.WriteBits(4, m_pImagePlane->iBands));

  switch(iColorFormat)
  {
  case JXR_ICF_Y:
    m_pImagePlane->iCHModeDC = JXR_CM_UNIFORM;
    m_pImagePlane->iCHModeLP = JXR_CM_UNIFORM;
    m_pImagePlane->iCHModeHP = JXR_CM_UNIFORM;
    break;

  case JXR_ICF_YUV420:
  case JXR_ICF_YUV422:
    m_pImagePlane->iCHModeDC = JXR_CM_SEPARATE;
    m_pImagePlane->iCHModeLP = JXR_CM_SEPARATE;
    m_pImagePlane->iCHModeHP = JXR_CM_SEPARATE;
    m_pImagePlane->bResampling = 1;

    RET_(m_BitStreamOut.WriteBits(1, iReserved));
    RET_(m_BitStreamOut.WriteBits(3, m_pImagePlane->iCCenteringX));
    if(iColorFormat == JXR_ICF_YUV420)
    {
      RET_(m_BitStreamOut.WriteBits(1, iReserved));
      RET_(m_BitStreamOut.WriteBits(3, m_pImagePlane->iCCenteringY));
    }
    else
      RET_(m_BitStreamOut.WriteBits(4, iReserved))
    break;

  case JXR_ICF_YUV444:
    m_pImagePlane->iCHModeDC = JXR_CM_SEPARATE;
    m_pImagePlane->iCHModeLP = JXR_CM_SEPARATE;
    m_pImagePlane->iCHModeHP = JXR_CM_SEPARATE;

    RET_(m_BitStreamOut.WriteBits(4, iReserved));
    RET_(m_BitStreamOut.WriteBits(4, iReserved));
    break;

  case JXR_ICF_YUVK:
    m_pImagePlane->iCHModeDC = JXR_CM_SEPARATE;
    m_pImagePlane->iCHModeLP = JXR_CM_SEPARATE;
    m_pImagePlane->iCHModeHP = JXR_CM_SEPARATE;
    break;

  case JXR_ICF_NCH:
    m_pImagePlane->iCHModeDC = JXR_CM_INDEPENDENT;
    m_pImagePlane->iCHModeLP = JXR_CM_INDEPENDENT;
    m_pImagePlane->iCHModeHP = JXR_CM_INDEPENDENT;
    if(m_pImagePlane->iChannels > 16)
    {
      RET_(m_BitStreamOut.WriteBits(4,  16));
      RET_(m_BitStreamOut.WriteBits(12, m_pImagePlane->iChannels - 16));
    }
    else
    {
      RET_(m_BitStreamOut.WriteBits(4, m_pImagePlane->iChannels - 1));
      RET_(m_BitStreamOut.WriteBits(4, iReserved));
    }
    break;

  default:
    return ExcStatusFail;
  }

  switch(m_iBitDepth)
  {
  case JXR_BD_16:
  case JXR_BD_16S:
    if(m_pImagePlane->iShift == 0xff)
      m_pImagePlane->iShift = 0;
    RET_(m_BitStreamOut.WriteBits(8, m_pImagePlane->iShift));
    break;
  case JXR_BD_32S:
    if(m_pImagePlane->iShift == 0xff)
      m_pImagePlane->iShift = 10;
    RET_(m_BitStreamOut.WriteBits(8, m_pImagePlane->iShift));
    break;
  case JXR_BD_32F:
    RET_(m_BitStreamOut.WriteBits(8, m_pImagePlane->iMantiss));
    RET_(m_BitStreamOut.WriteBits(8, m_pImagePlane->iExp));
    break;
  }

  m_iYQuant = m_iInputQuant;
  if(m_iYQuant < 2)
    m_iYQuant = 0;
  else if(m_iYQuant < 16)
  {
    if(iColorFormat == JXR_ICF_YUV420)
      m_iUVQuant = m_iYQuant + ((m_iYQuant + 2) >> 2);
    else if(iColorFormat == JXR_ICF_YUV422)
      m_iUVQuant = m_iYQuant + ((m_iYQuant + 1) >> 1);
    else
      m_iUVQuant = m_iYQuant * 2;
  }
  else
  {
    if(iColorFormat == JXR_ICF_YUV420)
      m_iUVQuant = m_iYQuant + 4;
    else if(iColorFormat == JXR_ICF_YUV422)
      m_iUVQuant = m_iYQuant + 8;
    else
        m_iUVQuant = m_iYQuant + 18;
    m_iUVQuant += (m_iYQuant > 48)?2:0;
  }
  m_iYQuant &= 0xff;
  m_iUVQuant &= 0xff;

  m_pImagePlane->iDCQuant[0] = m_iYQuant;
  m_pImagePlane->iLPQuant[0] = m_iYQuant;
  m_pImagePlane->iHPQuant[0] = m_iYQuant;

  for(i = 1; i < iChannels; i++)
  {
    m_pImagePlane->iDCQuant[i] = m_iUVQuant;
    m_pImagePlane->iLPQuant[i] = m_iUVQuant;
    m_pImagePlane->iHPQuant[i] = m_iUVQuant;
  }

  RET_(m_BitStreamOut.WriteBits(1, m_pImagePlane->bDCFrameUniform));

  if(m_pImagePlane->bDCFrameUniform)
    RET_(writeUnifQuantizer(m_pImagePlane->iCHModeDC, m_pImagePlane->iDCQuant));

  if(m_pImagePlane->iBands < JXR_SB_DCONLY)
  {
    RET_(m_BitStreamOut.WriteBits(1, iReserved));
    RET_(m_BitStreamOut.WriteBits(1, m_pImagePlane->bLPFrameUniform));

    if(m_pImagePlane->bLPFrameUniform)
    {
      m_iNumLPQuantizers = 1;
      RET_(writeUnifQuantizer(m_pImagePlane->iCHModeLP, m_pImagePlane->iLPQuant));
    }
  }

  if(m_pImagePlane->iBands < JXR_SB_NOHP)
  {
    RET_(m_BitStreamOut.WriteBits(1, iReserved));
    RET_(m_BitStreamOut.WriteBits(1, m_pImagePlane->bHPFrameUniform));

    if(m_pImagePlane->bHPFrameUniform)
    {
      m_iNumHPQuantizers = 1;
      RET_(writeUnifQuantizer(m_pImagePlane->iCHModeHP, m_pImagePlane->iHPQuant));
    }
  }
  RET_(m_BitStreamOut.FlushToByte());

  return ExcStatusOk;
} // writeJXRImagePlaneHeader()


ExcStatus CJPEGXREncoder::writeJXRIndexTable(void)
{
  Ipp32u i;

  if(!m_bIndexFlag)
    m_iIndexSize = 0;
  else
  {
    if(m_bFrequencyFlag)
    {
      m_iNumBands  = 4 - m_pImagePlane->iBands;
      m_iIndexSize = m_iTilesTotal * m_iNumBands;
    }
    else
      m_iIndexSize = m_iTilesTotal;

    // Index table header
    m_BitStreamOut.WriteBits(16, (Ipp8u)1);
    m_iIndexOffset = m_BitStreamOut.GetTotalPos();

    m_pIndexTable = (Ipp64u*)ippMalloc(m_iIndexSize * sizeof(Ipp64u));
    // Write empty index table
    for(i = 0; i < m_iIndexSize; i++)
    {
      m_BitStreamOut.WriteBits(8,  0);
      m_BitStreamOut.WriteBits(16, 0);
      m_BitStreamOut.WriteBits(16, 0);
    }
  }

  // Write escape value
  m_BitStreamOut.WriteBits(8, (Ipp8u)0xff);

/*
  // PROFILE LEVEL INFO according to JPEGXR spec. Must be zero for HDP
  if(iBytes > 0)
  {
    do
    {
      m_BitStreamOut.WriteBits(8,  iProfileIDC);
      m_BitStreamOut.WriteBits(8,  iLevelIDC);
      m_BitStreamOut.WriteBits(15, iReservedL);
      m_BitStreamOut.WriteBits(1,  iBits);
      iNumBytes += 4;
    } while(iBits != 1);

    for(i = 0; i < (iBytes - iNumBytes); i++)
      m_BitStreamOut.WriteBits(8, iReservedA);
  }
*/
  return ExcStatusOk;
} // writeJXRIndexTable()

ExcStatus CJPEGXREncoder::fillJXRIndexTable(void)
{
  Ipp32u iWord1, iWord2;
  Ipp32u iCurOffset;
  Ipp32u i;

  iCurOffset = m_BitStreamOut.GetTotalPos();
  m_BitStreamOut.Seek(m_iIndexOffset);

  // Write index table values
  for(i = 0; i < m_iIndexSize; i++)
  {
    iWord1 = m_pIndexTable[i] & 0xffff;
    iWord2 = (m_pIndexTable[i] >> 16) & 0xffff;

    m_BitStreamOut.WriteBits(8,  0xfb); // 32bit fixed value
    m_BitStreamOut.WriteBits(16, iWord2);
    m_BitStreamOut.WriteBits(16, iWord1);
  }

  m_BitStreamOut.Seek(iCurOffset);

  return ExcStatusOk;
} // fillJXRIndexTable()

ExcStatus CJPEGXREncoder::writeJXRMacroblock(void)
{
  Ipp32u iBands = m_pImagePlane->iBands;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iACHOffset = (m_bAlphaProcess)?m_iACHOffset:0;
  Ipp32u lBlock = 16;
  Ipp32u cBlock = 16;
  Ipp8u bPCTLeft = !(ippBorderInMemLeft & m_iBorderPCT);
  Ipp8u bPCTTop  = !(ippBorderInMemTop  & m_iBorderPCT);
  Ipp32u i;

  if(iColorFormat == JXR_ICF_YUV422 || iColorFormat == JXR_ICF_YUV420)
    cBlock = 8;

  RET_(PCTransform())

  if(!bPCTLeft && !bPCTTop)
  {
    RET_(quantMacroblock());
    RET_(predMacroblock());
    RET_(writeMacroblockDC());

    if(iBands < JXR_SB_DCONLY)
      RET_(writeMacroblockLP());

    if(iBands < JXR_SB_NOHP)
      RET_(writeMacroblockHP());
  }

  for (i = 0; i < iChannels; i++)
  {
    Ipp32u iShift = ((i)?cBlock:lBlock) * m_iCUSize;
    m_pMBTab[i + iACHOffset].pMB4 = (Ipp32s*)((Ipp8u*)m_pMBTab[i + iACHOffset].pMB4 + iShift);
    m_pMBTab[i + iACHOffset].pCC0 = (Ipp32s*)((Ipp8u*)m_pMBTab[i + iACHOffset].pCC0 + iShift);
    m_pMBTab[i + iACHOffset].pCC1 = (Ipp32s*)((Ipp8u*)m_pMBTab[i + iACHOffset].pCC1 + iShift);
  }

  return ExcStatusOk;
}

ExcStatus CJPEGXREncoder::writeJXRTile(void)
{
  Ipp8u *pSrc = m_pData;
  Ipp32u iSrcStep = m_iStep;
  Ipp32u iSrcShift = 0;
  PredictorInf *pPred;
  Ipp32s *pTmpCC;
  Ipp32u i;

  m_iCurTileMBWidth = m_iTileMBWidth[m_iTileColumn];
  m_iCurTileMBHeight = m_iTileMBHeight[m_iTileRow];

  if(m_iTileColumn == (Ipp32u)(m_iTilesUniform[0] - 1) && m_iRightExtra)
    m_iCurTileWidth = (m_iCurTileMBWidth - 1) * 16 + m_iRightExtra;
  else
    m_iCurTileWidth = m_iCurTileMBWidth * 16;

  if(m_iTileRow == (Ipp32u)(m_iTilesUniform[1] - 1) && m_iBottomExtra)
    m_iCurTileHeight = (m_iCurTileMBHeight - 1) * 16 + m_iBottomExtra;
  else
    m_iCurTileHeight = m_iCurTileMBHeight * 16;

  for(m_iMBRow = 0; m_iMBRow <= m_iCurTileMBHeight; m_iMBRow++)
  {
    m_iMBAbsRow = m_iTileStartRow + m_iMBRow;
    iSrcShift = iSrcStep * 16 * m_iMBAbsRow + m_iTileStartColumn * 16 * ((m_bFakeAlpha)?m_iChannels + 1:m_iChannels) * m_iUnitSize;

    for (i = 0; i < m_iChannels; i++)
    {
      m_pMBTab[i].pMB4 = m_pMBTab[i].pSliceMB4;
      m_pMBTab[i].pCC0 = m_pMBTab[i].pSliceCC0;
      m_pMBTab[i].pCC1 = m_pMBTab[i].pSliceCC1;
    }

    if(m_iMBRow != m_iCurTileMBHeight)
      RET_(colorConvert(pSrc + iSrcShift, iSrcStep));

    for(m_iMBColumn = 0; m_iMBColumn <= m_iCurTileMBWidth; m_iMBColumn++)
    {
      m_iMBAbsColumn = m_iTileStartColumn + m_iMBColumn;
      m_iBorder = 0;
      m_iBorderPCT = 0;

      // Huffman borders
      if(m_iMBColumn - 1 == 0)
        m_iBorder |= JXR_BORDER_LEFT;
      if(m_iMBRow - 1 == 0)
        m_iBorder |= JXR_BORDER_TOP;
      if(m_iMBColumn == m_iCurTileMBWidth)
        m_iBorder |= JXR_BORDER_RIGHT;
      if(m_iMBRow == m_iCurTileMBHeight)
        m_iBorder |= JXR_BORDER_BOTTOM;

      // PCT borders
      if(m_iMBColumn != 0)
        m_iBorderPCT |= ippBorderInMemLeft;
      if(m_iMBRow != 0)
        m_iBorderPCT |= ippBorderInMemTop;
      if(m_iMBColumn != m_iCurTileMBWidth)
        m_iBorderPCT |= ippBorderInMemRight;
      if(m_iMBRow != m_iCurTileMBHeight)
        m_iBorderPCT |= ippBorderInMemBottom;

      m_bResetContext = m_bResetTotals = (((m_iMBColumn - 1) & 0xf) == 0);
      if(m_iMBColumn == m_iCurTileMBWidth)
        m_bResetContext = 1;

      RET_(writeJXRMacroblock());
      if(m_bAlphaFlag)
      {
        m_pImagePlane = &m_ImagePlane[JXR_PLANE_ALPHA];
        m_bAlphaProcess = 1;

        RET_(writeJXRMacroblock());

        m_pImagePlane = &m_ImagePlane[JXR_PLANE_PRIMARY];
        m_bAlphaProcess = 0;
      }
    }

    for(i = 0; i < m_iChannels; i++) // swap current row and previous row
    {
      m_pMBTab[i].pMB4 = m_pMBTab[i].pSliceMB4;
      pTmpCC = m_pMBTab[i].pSliceCC0;
      m_pMBTab[i].pSliceCC0 = m_pMBTab[i].pSliceCC1;
      m_pMBTab[i].pSliceCC1 = pTmpCC;

      pPred = m_pPred[i];
      m_pPred[i] = m_pPredPrev[i];
      m_pPredPrev[i] = pPred;
    }
  }

  return ExcStatusOk;
} // writeJXRTile()


ExcStatus CJPEGXREncoder::WriteData()
{
  Ipp32u iTileStartCode = 1;
  Ipp32u iTileLocationHash = 0;
  Ipp64u iFirstOffset;
  Ipp32u iTileIndex;
  Ipp32u iTemp;

  m_iMBAbsRow = 0;
  m_iMBAbsColumn = 0;
  m_iTileStartRow = 0;
  m_iTileStartColumn = 0;

  if(m_bFirstLoop)
  {
    initEncoder();
    m_bFirstLoop = 0;
  }

  iFirstOffset = m_BitStreamOut.GetTotalPos();

  for(m_iTileRow = 0; m_iTileRow < m_iTilesUniform[1]; m_iTileRow++)
  {
    for(m_iTileColumn = 0; m_iTileColumn < m_iTilesUniform[0]; m_iTileColumn++)
    {
      RET_(resetEncoder());
      if(m_bAlphaFlag)
      {
        m_pImagePlane = &m_ImagePlane[JXR_PLANE_ALPHA];
        RET_(resetEncoder());
        m_pImagePlane = &m_ImagePlane[JXR_PLANE_PRIMARY];
      }

      if(m_bIndexFlag)
      {
        iTileIndex = m_iTilesUniform[0] * m_iTileRow + m_iTileColumn;
        m_pIndexTable[iTileIndex] = m_BitStreamOut.GetTotalPos() - iFirstOffset;
      }

      RET_(m_BitStreamOut.WriteBits(24, iTileStartCode));
      RET_(m_BitStreamOut.WriteBits(5, iTileLocationHash));
      RET_(m_BitStreamOut.WriteBits(3, m_iTileType));

      if(m_bTrimFlexFlag)
        RET_(m_BitStreamOut.WriteBits(4, m_iTrimFlex));

      RET_(writeJXRTile());

      RET_(m_BitStreamOut.FlushToByte());
      m_iTileStartColumn += m_iTileMBWidth[m_iTileColumn];
    }
    m_iTileStartRow += m_iTileMBHeight[m_iTileRow];
    m_iTileStartColumn = 0;
  }

  if(m_bIndexFlag)
    RET_(fillJXRIndexTable());

  // Write image length
  iTemp = m_BitStreamOut.GetTotalPos();
  if(m_bPAlphaProcess)
  {
    m_iAlphaNumBytes = iTemp - m_iAlphaOffset;
    m_BitStreamOut.Seek(m_iALEntryOffset);
    m_BitStreamOut.Write(m_iAlphaNumBytes);
  }
  else
  {
    m_iImageNumBytes = iTemp - m_iImageOffset;
    m_BitStreamOut.Seek(m_iILEntryOffset);
    m_BitStreamOut.Write(m_iImageNumBytes);
  }
  m_BitStreamOut.Seek(iTemp);

  return ExcStatusOk;
} // WriteData()


ExcStatus CJPEGXREncoder::WriteFileHeader(Ipp32u bPAlphaPresent)
{
  Ipp8u   iPixFormatCode[16] = {0x24,0xc3,0xdd,0x6f,0x03,0x4e,0xfe,0x4b,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9, 0};
  Ipp32u  iChannels = (m_bFakeAlpha)?m_pImagePlane->iChannels + 1:m_pImagePlane->iChannels;
  Ipp32f  fXRes = 96, fYRes = 96;
  Ipp32u  iTemp = 0;
  Ipp32u  iAlpha = (m_bPreAlpha)?2:(m_bAlphaFlag?1:0);

  iPixFormatCode[15] = setPixDesc(m_iBitDepth, m_iSrcColorFormat, iChannels, iAlpha, m_bBGR);

  RET_(m_BitStreamOut.Write((Ipp16u)JXR_ORDER));
  RET_(m_BitStreamOut.Write((Ipp8u)JXR_ID));
  RET_(m_BitStreamOut.Write((Ipp8u)JXR_VERSION));
  RET_(m_BitStreamOut.Write(iTemp));

  writeIFDValue(JXRT_PIXEL_FORMAT,      JXR_TT_BYTE,  16, &iPixFormatCode[0]);
  writeIFDValue(JXRT_IMAGE_WIDTH,       JXR_TT_LONG,  1,  &m_iWidth);
  writeIFDValue(JXRT_IMAGE_HEIGHT,      JXR_TT_LONG,  1,  &m_iHeight);
  writeIFDValue(JXRT_WIDTH_RESOLUTION,  JXR_TT_FLOAT, 1,  &fXRes);
  writeIFDValue(JXRT_HEIGHT_RESOLUTION, JXR_TT_FLOAT, 1,  &fYRes);
  writeIFDValue(JXRT_IMAGE_OFFSET,      JXR_TT_LONG,  1,  &iTemp);
  writeIFDValue(JXRT_IMAGE_BYTE_COUNT,  JXR_TT_LONG,  1,  &iTemp);

  if(bPAlphaPresent)
  {
    writeIFDValue(JXRT_ALPHA_OFFSET,      JXR_TT_LONG,  1,  &iTemp);
    writeIFDValue(JXRT_ALPHA_BYTE_COUNT,  JXR_TT_LONG,  1,  &iTemp);
  }

  iTemp = m_BitStreamOut.GetTotalPos();
  RET_(m_BitStreamOut.Seek(4));
  RET_(m_BitStreamOut.Write(iTemp));
  RET_(m_BitStreamOut.Seek(iTemp));
  writeIFDTable();

  // Write IFD end dword
  iTemp = 0;
  RET_(m_BitStreamOut.Write(iTemp));

  return ExcStatusOk;
} // writeFileHeader()


ExcStatus CJPEGXREncoder::WriteHeader()
{
  RET_(writeJXRImageHeader())
  RET_(writeJXRImagePlaneHeader())

  if(m_bAlphaFlag)
  {
    m_iACHOffset = m_pImagePlane->iChannels;
    m_pImagePlane = &m_ImagePlane[JXR_PLANE_ALPHA];
    RET_(writeJXRImagePlaneHeader());
    m_pImagePlane = &m_ImagePlane[JXR_PLANE_PRIMARY];
  }

  RET_(writeJXRIndexTable());

  return ExcStatusOk;
} // WriteHeader()
