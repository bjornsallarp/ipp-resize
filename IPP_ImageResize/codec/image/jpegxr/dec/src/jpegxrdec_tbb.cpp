/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2008-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifdef USE_TBB

#include "jpegxrdec_tbb.h"
#include "jpegxrvalues.h"
#ifndef __IPPI_H__
#include "ippi.h"
#endif
#ifndef __IPPCC_H__
#include "ippcc.h"
#endif


CTBBFrame::CTBBFrame(CJPEGXRTileDecoder* pDecoder, Ipp32u iThreads, tbb::spin_mutex* mutex)
{
  m_pDecoder = pDecoder;
  m_iThreads = iThreads;
  m_Mutex = mutex;
  return;
}


CTBBHuffmanFilter::CTBBHuffmanFilter(CJPEGXRTileDecoder* pDecoder) : tbb::filter(serial_in_order)
{
  m_pDecoder = pDecoder;
  return;
}


CTBBPCTFilter::CTBBPCTFilter(CJPEGXRTileDecoder* pDecoder) : tbb::filter(serial_in_order)
{
  m_pDecoder = pDecoder;
  return;
}


CTBBCCFilter::CTBBCCFilter(CJPEGXRTileDecoder* pDecoder) : tbb::filter(serial_in_order)
{
  m_pDecoder = pDecoder;
  return;
}


void CTBBFrame::operator()(const tbb::blocked_range2d<Ipp16u> &r) const
{
  CJPEGXRTileDecoder* pDecoder;
  tbb::spin_mutex::scoped_lock lock;
  Ipp32u iProceed = 0;
  Ipp32u i;

  do
  {
    lock.acquire(*m_Mutex); // ST Block Start--------
    for(i = 0; i < m_iThreads; i++)
    {
      if(!m_pDecoder[i].isLocked())
      {
        pDecoder = &m_pDecoder[i];
        pDecoder->lock();
        iProceed = 1;
        break;
      }
    }
    lock.release(); // ST Block End--------
  } while(iProceed == 0);

  if(iProceed)
  {
    for(Ipp16u i = r.rows().begin(); i != r.rows().end(); i++)
    {
      for(Ipp16u j = r.cols().begin(); j != r.cols().end(); j++)
        pDecoder->readFrameSeparate(i, j);
    }
    lock.acquire(*m_Mutex); // ST Block Start--------
    pDecoder->release();
    lock.release(); // ST Block End--------
  }
}


void* CTBBHuffmanFilter::operator()(void*)
{
  return m_pDecoder->readTileSeparate();
}


void* CTBBPCTFilter::operator()(void*)
{
  return m_pDecoder->PCTransformSeparate();
}


void* CTBBCCFilter::operator()(void*)
{
  return m_pDecoder->colorConvertSeparate();
}


ExcStatus CJPEGXRDecoder::readFrameTBB(void)
{
  tbb::spin_mutex mutex;
  CJPEGXRTileDecoder* pDecoder = new CJPEGXRTileDecoder[m_iTileThreads];
  CTBBFrame tbbFrame(pDecoder, m_iTileThreads, &mutex);
  Ipp8u  bTBBError = 0;
  Ipp32u i;

  for(i = 0; i < m_iTileThreads; i++)
    pDecoder[i].initDecoder(this);

  parallel_for(tbb::blocked_range2d<Ipp16u>(0, m_iTilesHeight, 1, 0, m_iTilesWidth, 1), tbbFrame);

  for(i = 0; i < m_iTileThreads; i++)
  {
    if(pDecoder[i].m_bTBBError)
      bTBBError = 1;
  }

  delete[] pDecoder;

  if(bTBBError)
    return ExcStatusFail;

  return ExcStatusOk;
} // ReadFrameTBB()


ExcStatus CJPEGXRTileDecoder::readTileTBB()
{
  tbb::pipeline pipeline;
  Ipp32u iTileStartCode;
  Ipp32u iTileLocationHash;
  Ipp32u i;

  CTBBHuffmanFilter tbbHuffmanFilter((CJPEGXRTileDecoder*)this);
  CTBBPCTFilter     tbbPCTFilter((CJPEGXRTileDecoder*)this);
  CTBBCCFilter      tbbCCFilter((CJPEGXRTileDecoder*)this);

  pipeline.add_filter(tbbHuffmanFilter);
  pipeline.add_filter(tbbPCTFilter);
  pipeline.add_filter(tbbCCFilter);

  m_iCurTileMBWidth  = m_pParent->m_iTileMBWidth[m_iTileColumn];
  m_iCurTileMBHeight = m_pParent->m_iTileMBHeight[m_iTileRow];

  for(i = 0; i < m_pParent->m_iNumBands; i++)
  {
    m_Stream[i].ReadBits(24, iTileStartCode);
    if(1 != iTileStartCode) return ExcStatusFail;
    m_Stream[i].ReadBits(5, iTileLocationHash);
    m_Stream[i].ReadBits(3, m_iTileType);

    if(m_pParent->m_bTrimFlexFlag && (m_iTileType == JXR_TT_SPATIAL || m_iTileType == JXR_TT_FB))
      m_Stream[i].ReadBits(4, m_iTrimFlex);
  }

  m_bHufDone = m_bPCTDone = m_bCCDone = m_bTBBError = 0;

  m_iMBColumn = m_iMBRow = 0;
  m_iMBColumnPCT = m_iMBRowPCT = 0;
  m_iMBColumnCC = m_iMBRowCC = 0;

  pipeline.run(m_iPipeThreads);
  pipeline.clear();

  if(m_bTBBError)
    return ExcStatusFail;

  for(i = 0; i < m_pParent->m_iNumBands; i++)
    RET_(m_Stream[i].FlushToByte())

  return ExcStatusOk;
} // readTileTBB()


void CJPEGXRTileDecoder::readFrameSeparate(Ipp16u iTileRow, Ipp16u iTileColumn)
{
  ExcStatus status;

  m_iTileRow = iTileRow;
  m_iTileColumn = iTileColumn;

  status = resetDecoder();
  if(status == ExcStatusFail)
    m_bTBBError = 1;

  if(m_iPipeThreads > 1)
    status = readTileTBB();
  else
    status = readTile();

  if(status == ExcStatusFail)
    m_bTBBError = 1;

} // readFrameSeparate()


void* CJPEGXRTileDecoder::readTileSeparate(void)
{
  PredictorInf *pPred;
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u iColorFormat = m_pImagePlane->iColorFormat;
  Ipp32u m_iChannels = m_pParent->m_iChannels;
  IppiSize roi;
  Ipp32u i;

  if(!m_bHufDone && !m_bTBBError)
  {
    roi = m_pParent->m_lRoi;
    for (i = 0; i < m_iChannels; i++)
    {
      m_ImgBuf[i].pHufPtr = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pImgBuf + m_ImgBuf[i].iStep * roi.height * m_iMBRow);
      m_ImgBuf[i].pHufPtr = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pHufPtr + roi.width * m_iMBColumn * m_pParent->m_iCUSize);
      roi = ((i >= (iChannels - 1))?m_pParent->m_lRoi:m_pParent->m_cRoi);
    }
    m_iBorder = 0;

    if(m_iMBColumn == 0)
      m_iBorder |= JXR_BORDER_LEFT;
    if(m_iMBRow == 0)
      m_iBorder |= JXR_BORDER_TOP;
    if(m_iMBColumn == m_iCurTileMBWidth)
      m_iBorder |= JXR_BORDER_RIGHT;
    if(m_iMBRow == m_iCurTileMBHeight)
      m_iBorder |= JXR_BORDER_BOTTOM;

    m_bResetContext = m_bResetTotals = ((m_iMBColumn & 0xf) == 0);
    if(m_iMBColumn + 1 == m_iCurTileMBWidth)
      m_bResetContext = 1;

    // Entropy decoding
    RET_TBB_(readMacroblock());
    if(m_pParent->m_bAlphaFlag)
    {
      m_pHuffmanPlane = &m_HuffmanPlane[JXR_PLANE_ALPHA];
      m_pImagePlane = &m_pParent->m_ImagePlane[JXR_PLANE_ALPHA];
      m_bAlphaProcess = 1;

      RET_TBB_(readMacroblock());

      m_pHuffmanPlane = &m_HuffmanPlane[JXR_PLANE_PRIMARY];
      m_pImagePlane = &m_pParent->m_ImagePlane[JXR_PLANE_PRIMARY];
      m_bAlphaProcess = 0;
    }

    // Predictor swapper
    if(m_iMBColumn == m_iCurTileMBWidth)
    {
      for(i = 0; i < m_iChannels; i++)
      {
        pPred          = m_pPred[i];
        m_pPred[i]     = m_pPredPrev[i];
        m_pPredPrev[i] = pPred;
      }
    }

    if(m_iMBRow == m_iCurTileMBHeight && m_iMBColumn == m_iCurTileMBWidth)
      m_bHufDone = 1;

    if(m_iMBColumn == m_iCurTileMBWidth)
    {
      m_iMBColumn = 0;
      m_iMBRow++;
    }
    else
      m_iMBColumn++;
  }

  // Wait for other threads to complete
  if((m_bHufDone && m_bPCTDone && m_bCCDone) || m_bTBBError)
    return NULL;

  return m_pDst;
} // readTileSeparate()


void* CJPEGXRTileDecoder::PCTransformSeparate(void)
{
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u m_iChannels = m_pParent->m_iChannels;
  IppiSize roi;
  Ipp32u i;

  if(!m_bPCTDone && !m_bTBBError)
  {
    roi = m_pParent->m_lRoi;
    for (i = 0; i < m_iChannels; i++)
    {
      m_ImgBuf[i].pPCTPtr = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pImgBuf + m_ImgBuf[i].iStep * roi.height * m_iMBRowPCT);
      m_ImgBuf[i].pPCTPtr = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pPCTPtr + roi.width * m_iMBColumnPCT * m_pParent->m_iCUSize);
      roi = ((i >= (iChannels - 1))?m_pParent->m_lRoi:m_pParent->m_cRoi);
    }
    m_iBorderPCT = 0;

    if(1/*m_pParent->m_bHardTilingFlag*/)
    {
      if(m_iMBColumnPCT != 0)
        m_iBorderPCT |= ippBorderInMemLeft;
      if(m_iMBRowPCT != 0)
        m_iBorderPCT |= ippBorderInMemTop;
      if(m_iMBColumnPCT != m_iCurTileMBWidth)
        m_iBorderPCT |= ippBorderInMemRight;
      if(m_iMBRowPCT != m_iCurTileMBHeight)
        m_iBorderPCT |= ippBorderInMemBottom;
    }
    else
    {
      if((m_iCurTileMBColumn + m_iMBColumnPCT) != 0)
        m_iBorderPCT |= ippBorderInMemLeft;
      if((m_iCurTileMBRow + m_iMBRowPCT) != 0)
        m_iBorderPCT |= ippBorderInMemTop;
      if((m_iCurTileMBColumn + m_iMBColumnPCT) != m_pParent->m_iMBTotalWidth)
        m_iBorderPCT |= ippBorderInMemRight;
      if((m_iCurTileMBRow + m_iMBRowPCT) != m_pParent->m_iMBTotalHeight)
        m_iBorderPCT |= ippBorderInMemBottom;
    }

    RET_TBB_(PCTransform());
    if(m_pParent->m_bAlphaFlag)
    {
      m_pImagePlanePCT = &m_pParent->m_ImagePlane[JXR_PLANE_ALPHA];
      m_bAlphaProcessPCT = 1;

      RET_TBB_(PCTransform());

      m_pImagePlanePCT = &m_pParent->m_ImagePlane[JXR_PLANE_PRIMARY];
      m_bAlphaProcessPCT = 0;
    }

    if(m_iMBRowPCT == m_iCurTileMBHeight && m_iMBColumnPCT == m_iCurTileMBWidth)
      m_bPCTDone = 1;

    if(m_iMBColumnPCT == m_iCurTileMBWidth)
    {
      m_iMBColumnPCT = 0;
      m_iMBRowPCT++;
    }
    else
      m_iMBColumnPCT++;
  }

  return NULL;
} // PCTransformSeparate()


void* CJPEGXRTileDecoder::colorConvertSeparate(void)
{
  Ipp32u iDstStep = m_pParent->m_iStep;
  Ipp32u iChannels = m_pImagePlane->iChannels;
  Ipp32u m_iChannels = m_pParent->m_iChannels;
  Ipp8u iLE = m_pParent->m_iLeftExtra;
  Ipp8u iTE = m_pParent->m_iTopExtra;
  Ipp8u iRE = m_pParent->m_iRightExtra;
  Ipp8u iBE = m_pParent->m_iBottomExtra;
  Ipp32u iCurTileWidth;
  Ipp32u iCurTileHeight;
  Ipp32u iDstRShift = 0;
  Ipp32u iDstCShift = 0;
  Ipp32u iSrcRShift = 0;
  Ipp32u iSrcCShift = 0;
  IppiSize roi;
  Ipp32u i;

  if(!m_bCCDone && !m_bTBBError)
  {
    if(m_iMBRowPCT > 1)
    {
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

      iSrcRShift = (m_iMBRowCC * 16 < iTE)?iTE & 0xf:0;
      iSrcCShift = (m_iMBColumnCC * 16 < iLE)?iLE & 0xf:0;

      roi = m_pParent->m_lRoi;
      for (i = 0; i < m_iChannels; i++)
      {
        m_ImgBuf[i].pCCPtr = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pImgBuf + m_ImgBuf[i].iStep * (roi.height * m_iMBRowCC + iSrcRShift));
        m_ImgBuf[i].pCCPtr = (Ipp32s*)((Ipp8u*)m_ImgBuf[i].pCCPtr  + (roi.width * m_iMBColumnCC + iSrcCShift) * m_pParent->m_iCUSize);
        roi = ((i >= (iChannels - 1))?m_pParent->m_lRoi:m_pParent->m_cRoi);
      }

      if(m_iMBRowCC < m_iCurTileMBHeight && m_iMBColumnCC < m_iCurTileMBWidth)
      {
        if(((m_iMBRowCC + 1) * 16 > iTE) && (m_iMBRowCC * 16 < (iCurTileHeight + iTE)))
        {
          if(((m_iMBColumnCC + 1) * 16 > iLE) && (m_iMBColumnCC * 16 < (iCurTileWidth + iLE)))
          {
            roi.height = iCurTileHeight + iTE - m_iMBRowCC * 16;
            if(roi.height > 16) roi.height = 16;
            roi.width  = iCurTileWidth + iLE - m_iMBColumnCC * 16;
            if(roi.width > 16) roi.width = 16;

            iDstRShift = iDstStep * (16 * m_iMBRowCC - iTE);
            if(m_iMBRowCC * 16 < iTE)
            {
              iDstRShift = 0;
              roi.height -= iTE & 0xf;
            }

            iDstCShift = (16 * m_iMBColumnCC - iLE) * m_pParent->m_iUnitSize * ((m_pParent->m_bFakeAlpha)?m_iChannels+1:m_iChannels);
            if(m_iMBColumnCC * 16 < iLE)
            {
              iDstCShift = 0;
              roi.width -= iLE & 0xf;
            }

            RET_TBB_(colorConvert(m_pDst + iDstRShift + iDstCShift, iDstStep, roi));
          }
        }
      }

      if(m_iMBColumnCC == m_iCurTileMBWidth)
      {
        m_iMBColumnCC = 0;
        m_iMBRowCC++;
      }
      else
        m_iMBColumnCC++;

      if(m_iMBRowCC == m_iCurTileMBHeight && m_iMBColumnCC == m_iCurTileMBWidth)
        m_bCCDone = 1;
    }
  }

  return NULL;
} // colorConvertSeparate()

#endif // USE_TBB
