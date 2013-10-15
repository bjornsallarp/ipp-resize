/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2012 Intel Corporation. All Rights Reserved.
//
*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4100 )
#endif
#include "stdafx.h"
#include "precomp.h"

#include <stdio.h>
#include <string.h>

#ifndef __IPPI_H__
#include "ippi.h"
#endif
#ifndef __IPPJ_H__
#include "ippj.h"
#endif

#ifndef __IPPCH_H__
#include "ippch.h"
#endif

#ifndef __JPEGBASE_H__
#include "jpegbase.h"
#endif
#ifndef __JPEGDEC_H__
#include "jpegdec.h"
#endif

#define HUFF_ROW_API // enable new huffman functions, to improve performance

#define DCT_QUANT_INV8x8To1x1LS(pMCUBuf, dst, qtbl)\
{\
  int val = (((pMCUBuf[0] * qtbl[0]) >> 3) + 128);\
  dst[0] = (Ipp8u)(val > 255 ? 255 : (val < 0 ? 0 : val));\
}


JERRCODE CJPEGDecoder::ProcessRestart(int thread_id)
{
  IppStatus status;

#ifdef _OPENMP
  status = ippiDecodeHuffmanStateInit_JPEG_8u(m_state_t[thread_id]);
#else
  status = ippiDecodeHuffmanStateInit_JPEG_8u(m_state);
#endif
  if(ippStsNoErr != status)
  {
    LOG0("Error: ippiDecodeHuffmanStateInit_JPEG_8u() failed");
    return JPEG_ERR_INTERNAL;
  }

  for(int n = 0; n < m_jpeg_ncomp; n++)
  {
    m_ccomp[n].m_lastDC = 0;
  }

#ifdef _OPENMP
  if(m_threading_mode == JT_RSTI)
  {
    for(int n = 0; n < m_jpeg_ncomp; n++)
    {
      m_lastDC[thread_id][n] = 0;
    }
  }
#endif

  return JPEG_OK;
} // CJPEGDecoder::ProcessRestart()


JERRCODE CJPEGDecoder::ParseRST(int thread_id)
{
  JERRCODE jerr = JPEG_OK;

  TRC0("-> RST");

  if(m_marker == 0xff)
  {
    jerr = m_BitStreamInT[thread_id].Seek(-1);
    if(JPEG_OK != jerr)
      return jerr;

    m_marker = JM_NONE;
  }

  if(m_marker == JM_NONE)
  {
//    jerr = NextMarker(&m_marker, thread_id);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: NextMarker() failed");
      return jerr;
    }
  }

  TRC1("restart interval ",m_next_restart_num);
  if(m_marker == ((int)JM_RST0 + m_next_restart_num))
  {
    m_marker = JM_NONE;
  }
  else
  {
    LOG1("  - got marker   - ",m_marker);
    LOG1("  - but expected - ",(int)JM_RST0 + m_next_restart_num);
    m_marker = JM_NONE;
//    return JPEG_ERR_RST_DATA;
  }

  // Update next-restart state
  m_next_restart_num = (m_next_restart_num + 1) & 7;

  return JPEG_OK;
} // CJPEGDecoder::ParseRST()


JERRCODE CJPEGDecoder::DecodeHuffmanMCURowBL_RSTI(Ipp16s* pMCUBuf, int thread_id)
{
  int       j, n, k, l;
  int       srcLen;
  int       currPos;
  int       curr_lnz;
  Ipp8u*    src;
  Ipp8u*    lnzBuf;
  IppStatus status;

#ifdef _OPENMP
  currPos = m_BitStreamInT[thread_id].GetCurrPos();
  src     = m_BitStreamInT[thread_id].GetDataPtr();
  srcLen  = m_BitStreamInT[thread_id].GetDataLen();

  Ipp8u* plnz = ((Ipp8u*)((IppiDecodeHuffmanState*)m_state_t[thread_id]) + 16*sizeof(Ipp8u));
#else
  src    = m_BitStreamIn.GetDataPtr();
  srcLen = m_BitStreamIn.GetDataLen();

  Ipp8u* plnz = ((Ipp8u*)((IppiDecodeHuffmanState*)m_state) + 16*sizeof(Ipp8u));
#endif

#ifdef _OPENMP
  JMARKER marker = JM_NONE;
#endif

  for(j = 0; j < m_numxMCU; j++)
  {
    for(n = 0; n < m_jpeg_ncomp; n++)
    {
      Ipp16s*                lastDC = &m_ccomp[n].m_lastDC;
      IppiDecodeHuffmanSpec* dctbl  = m_dctbl[m_ccomp[n].m_dc_selector];
      IppiDecodeHuffmanSpec* actbl  = m_actbl[m_ccomp[n].m_ac_selector];
      curr_lnz                      = j * m_ccomp[n].m_lnz_ds;

      lnzBuf = m_ccomp[n].GetLNZBufferPtr(thread_id);

      for(k = 0; k < m_ccomp[n].m_vsampling; k++)
      {
        for(l = 0; l < m_ccomp[n].m_hsampling; l++)
        {
#ifdef _OPENMP
          currPos = m_BitStreamInT[thread_id].GetCurrPos();

          status = ippiDecodeHuffman8x8_JPEG_1u16s_C1(
                     src,srcLen,&currPos,pMCUBuf,&m_lastDC[thread_id][n],(int*)&marker,
                     dctbl,actbl,m_state_t[thread_id]);
#else
          m_BitStreamIn.FillBuffer(SAFE_NBYTES);

          currPos = m_BitStreamIn.GetCurrPos();

          status = ippiDecodeHuffman8x8_JPEG_1u16s_C1(
                     src,srcLen,&currPos,pMCUBuf,lastDC,(int*)&m_marker,
                     dctbl,actbl,m_state);

#endif
          lnzBuf[curr_lnz] = *plnz;
          curr_lnz++;

#ifdef _OPENMP
          m_BitStreamInT[thread_id].SetCurrPos(currPos);
#else
          m_BitStreamIn.SetCurrPos(currPos);
#endif
          if(ippStsNoErr > status)
          {
            LOG0("Error: ippiDecodeHuffman8x8_JPEG_1u16s_C1() failed!");
            m_marker = JM_NONE;
            return JPEG_ERR_INTERNAL;
          }

          pMCUBuf += DCTSIZE2;
        } // for m_hsampling
      } // for m_vsampling
    } // for m_jpeg_ncomp
  } // for m_numxMCU

  return JPEG_OK;
} // CJPEGDecoder::DecodeHuffmanMCURowBL_RSTI()


JERRCODE CJPEGDecoder::ParseNextRSTI(int thread_id, int rstiNum)
{
  int      c = 0;
  JMARKER  marker = JM_NONE;

  int    len;
  int    curPos;
  int    bufPos;
  int    index  = 0;
  int    size;
  Ipp8u* src;

  IppStatus status;

  m_BitStreamIn.FillBuffer(SAFE_NBYTES);

  c = 0;

  for(;;)
  {
    src    = m_BitStreamIn.GetCurrPtr();
    len    = m_BitStreamIn.GetDataLen();
    curPos = m_BitStreamIn.GetCurrPos();
    bufPos = m_BitStreamIn.GetStreamPos();
    size   = len - curPos;

    status = ippsFindC_8u(src, size, 0xff, &index);
    if(ippStsNoErr != status)
      return JPEG_ERR_INTERNAL;

    if(-1 != index)
    {
      if((index+1) < size)
      {
        c = src[index+1];

        if(c != 0)
        {
          marker = (JMARKER)c;

          if(marker >= JM_RST0 && marker <= JM_RST7)
          {
            int rstiSize;

            m_rsti_offset[rstiNum] = bufPos + index + 2;
            rstiSize = m_rsti_offset[rstiNum] - m_rsti_offset[rstiNum - 1];

            m_BitStreamIn.FillBitStream(&m_BitStreamInT[thread_id], m_rsti_offset[rstiNum - 1], rstiSize);

            m_BitStreamIn.SetCurrPos(curPos + index + 2); // "+2" means skip RST marker

            break;
          }
          else if(marker == JM_EOI)
          {
            int rstiSize;

            m_rsti_offset[rstiNum] = bufPos + index;
            rstiSize = m_rsti_offset[rstiNum] - m_rsti_offset[rstiNum - 1];

            m_BitStreamIn.FillBitStream(&m_BitStreamInT[thread_id], m_rsti_offset[rstiNum - 1], rstiSize);

            m_BitStreamIn.SetCurrPos(curPos + index);

            break;
          }
        }
        m_BitStreamIn.SetCurrPos(curPos + index + 1);
      }
      else
      {
        m_BitStreamIn.SetCurrPos(len - 1);
      }
    }
    else
      m_BitStreamIn.SetCurrPos(len - 1);

    m_BitStreamIn.FillBuffer(SAFE_NBYTES);
  }

  return JPEG_OK;
} // CJPEGDecoder::ParseNextRSTI()


JERRCODE CJPEGDecoder::DecodeScanBaselineIN_RSTI(void)
{
  int rcount = 0;
  IppStatus status;
  JERRCODE  jerr;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif

#ifdef _OPENMP
  int j;
  for(j = 0; j < m_num_threads; j++)
    status = ippiDecodeHuffmanStateInit_JPEG_8u(m_state_t[j]);
#endif
  status = ippiDecodeHuffmanStateInit_JPEG_8u(m_state);

  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  m_marker = JM_NONE;

  // workaround for 8-bit qnt tables in 12-bit scans
  if(m_qntbl[0].m_initialized && m_qntbl[0].m_precision == 0 && m_jpeg_precision == 12)
    m_qntbl[0].ConvertToHighPrecision();

  if(m_qntbl[1].m_initialized && m_qntbl[1].m_precision == 0 && m_jpeg_precision == 12)
    m_qntbl[1].ConvertToHighPrecision();

  if(m_qntbl[2].m_initialized && m_qntbl[2].m_precision == 0 && m_jpeg_precision == 12)
    m_qntbl[2].ConvertToHighPrecision();

  if(m_qntbl[3].m_initialized && m_qntbl[3].m_precision == 0 && m_jpeg_precision == 12)
    m_qntbl[3].ConvertToHighPrecision();

  // workaround for 16-bit qnt tables in 8-bit scans
  if(m_qntbl[0].m_initialized && m_qntbl[0].m_precision == 1 && m_jpeg_precision == 8)
    m_qntbl[0].ConvertToLowPrecision();

  if(m_qntbl[1].m_initialized && m_qntbl[1].m_precision == 1 && m_jpeg_precision == 8)
    m_qntbl[1].ConvertToLowPrecision();

  if(m_qntbl[2].m_initialized && m_qntbl[2].m_precision == 1 && m_jpeg_precision == 8)
    m_qntbl[2].ConvertToLowPrecision();

  if(m_qntbl[3].m_initialized && m_qntbl[3].m_precision == 1 && m_jpeg_precision == 8)
    m_qntbl[3].ConvertToLowPrecision();

  if(m_dctbl[0].IsEmpty())
  {
    jerr = m_dctbl[0].Create();
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_dctbl[0].Init(0,0,(Ipp8u*)&DefaultLuminanceDCBits[0],(Ipp8u*)&DefaultLuminanceDCValues[0]);
    if(JPEG_OK != jerr)
      return jerr;
  }

  if(m_dctbl[1].IsEmpty())
  {
    jerr = m_dctbl[1].Create();
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_dctbl[1].Init(1,0,(Ipp8u*)&DefaultChrominanceDCBits[0],(Ipp8u*)&DefaultChrominanceDCValues[0]);
    if(JPEG_OK != jerr)
      return jerr;
  }

  if(m_actbl[0].IsEmpty())
  {
    jerr = m_actbl[0].Create();
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_actbl[0].Init(0,1,(Ipp8u*)&DefaultLuminanceACBits[0],(Ipp8u*)&DefaultLuminanceACValues[0]);
    if(JPEG_OK != jerr)
      return jerr;
  }

  if(m_actbl[1].IsEmpty())
  {
    jerr = m_actbl[1].Create();
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_actbl[1].Init(1,1,(Ipp8u*)&DefaultChrominanceACBits[0],(Ipp8u*)&DefaultChrominanceACValues[0]);
    if(JPEG_OK != jerr)
      return jerr;
  }

  m_rsti_offset[0] = m_BitStreamIn.GetStreamPos();

#ifdef _OPENMP
#pragma omp parallel shared(rcount) if(m_jpeg_sampling != JS_411)
#endif
  {
    int     i          = 0;
    int     rh         = 1;
    int     r          = 0;
    int     currMCURow = 0;
    int     idThread   = 0;
    Ipp16s* pMCUBuf    = 0;  // the pointer to Buffer for a current thread.

#ifdef _OPENMP
    idThread = omp_get_thread_num(); // the thread id of the calling thread.
#endif

    pMCUBuf = m_block_buffer + idThread * m_numxMCU * m_nblock * DCTSIZE2;

    while(i < m_num_rsti)
    {
#ifdef _OPENMP
#pragma omp critical
      {
#endif

        i = rcount;
        rcount++;

        if(i < m_num_rsti)
        {
          ParseNextRSTI(idThread,i+1);
        }

#ifdef _OPENMP
      }
#endif

      if(i < m_num_rsti)
      {
#ifdef _OPENMP

        currMCURow = m_rsti_height*i;

        m_BitStreamInT[idThread].SetDataLen(m_rsti_offset[i+1] - m_rsti_offset[i]);
        m_BitStreamInT[idThread].SetCurrPos(0);
#endif
        rh = IPP_MIN(m_rsti_height, m_numyMCU - currMCURow);

        for(r = 0; r < rh; r++)
        {
          ippsZero_16s(pMCUBuf,m_numxMCU * m_nblock * DCTSIZE2);

          jerr = DecodeHuffmanMCURowBL_RSTI(pMCUBuf, idThread);
          if(JPEG_OK != jerr)
            continue;

          jerr = ReconstructMCURowBL8x8(pMCUBuf, idThread);
          if(JPEG_OK != jerr)
            continue;

          jerr = UpSampling(currMCURow + r,idThread);
          if(JPEG_OK != jerr)
            continue;

          jerr = ColorConvert(currMCURow + r,idThread);
          if(JPEG_OK != jerr)
            continue;
        }

        if(m_jpeg_restart_interval)
        {
          jerr = ProcessRestart(idThread);
          if(JPEG_OK != jerr)
          {
            LOG0("Error: ProcessRestart() failed!");
          }
        }
      } // if
#ifndef _OPENMP
        i++;
#endif
    } // for m_numyMCU
  } // OMP

#ifdef _OPENMP
  /*for(j = 0; j < m_num_rsti; j++)
  {
    omp_destroy_lock(&locks[j]);
  }

  ippFree(locks);
  locks = 0;*/
#endif

  return JPEG_OK;
} // CJPEGDecoder::DecodeScanBaselineIN_RSTI()

