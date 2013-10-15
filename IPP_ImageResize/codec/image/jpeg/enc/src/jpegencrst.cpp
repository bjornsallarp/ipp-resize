/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2012 Intel Corporation. All Rights Reserved.
//
*/
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning ( disable : 4100 )
#endif
#include "stdafx.h"
#include "precomp.h"

#include <stdio.h>
#include <string.h>

#ifndef __IPPJ_H__
#include "ippj.h"
#endif
#ifndef __JPEGBASE_H__
#include "jpegbase.h"
#endif
#ifndef __JPEGENC_H__
#include "jpegenc.h"
#endif


JERRCODE CJPEGEncoder::WriteRST_T(
  int    next_restart_num,
  int    thread_id)
{
  JERRCODE jerr;

  TRC0("-> WriteRST");

  TRC1("  emit marker ",JM_RST0 + next_restart_num);
  TRC1("    RST ",0xfff0 | (JM_RST0 + next_restart_num));

  // emit restart interval
#ifdef _OPENMP
  jerr = m_BitStreamOutT[thread_id].WriteWord(0xff00 | (JM_RST0 + next_restart_num));
  if(JPEG_OK != jerr)
    return jerr;
#else
  jerr = m_BitStreamOut.WriteWord(0xff00 | (JM_RST0 + next_restart_num));
  if(JPEG_OK != jerr)
    return jerr;

#endif

  return JPEG_OK;
} // CJPEGEncoder::WriteRST()


JERRCODE CJPEGEncoder::ProcessRestart(
  int    id[MAX_COMPS_PER_SCAN],
  int    Ss,
  int    Se,
  int    Ah,
  int    Al,
  int    nRSTI,
  int    thread_id)
{
  int       c;
  int       dstLen;
  int       currPos;
  Ipp8u*    dst;
  JERRCODE  jerr;
  IppStatus status = ippStsNoErr;

  TRC0("-> ProcessRestart");
#ifdef _OPENMP
  dst    = m_BitStreamOutT[thread_id].GetDataPtr();
  dstLen = m_BitStreamOutT[thread_id].GetDataLen();

  currPos = m_BitStreamOutT[thread_id].GetCurrPos();
#else

  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();

  jerr = m_BitStreamOut.FlushBuffer();
  if(JPEG_OK != jerr)
    return jerr;

  currPos = m_BitStreamOut.GetCurrPos();
#endif

  // flush IppiEncodeHuffmanState
  switch(m_jpeg_mode)
  {
  case JPEG_BASELINE:
  case JPEG_EXTENDED:
    {
#ifdef _OPENMP
      status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
                 0,dst,dstLen,&currPos,0,0,0,m_state_t[thread_id],1);
#else
      status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
                 0,dst,dstLen,&currPos,0,0,0,m_state,1);

#endif
      break;
    }

  case JPEG_PROGRESSIVE:
    if(Ss == 0 && Se == 0)
    {
      // DC scan
      if(Ah == 0)
      {
        status = ippiEncodeHuffman8x8_DCFirst_JPEG_16s1u_C1(
                   0,dst,dstLen,&currPos,0,0,0,m_state_t[thread_id],1);
      }
      else
      {
        status = ippiEncodeHuffman8x8_DCRefine_JPEG_16s1u_C1(
                   0,dst,dstLen,&currPos,0,m_state_t[thread_id],1);
      }
    }
    else
    {
      // AC scan
      IppiEncodeHuffmanSpec* actbl = m_actbl[m_ccomp[id[0]].m_ac_selector];

      if(Ah == 0)
      {
        status = ippiEncodeHuffman8x8_ACFirst_JPEG_16s1u_C1(
                   0,dst,dstLen,&currPos,Ss,Se,Al,actbl,m_state_t[thread_id],1);
      }
      else
      {
        status = ippiEncodeHuffman8x8_ACRefine_JPEG_16s1u_C1(
                   0,dst,dstLen,&currPos,Ss,Se,Al,actbl,m_state_t[thread_id],1);
      }
    }
    break;

  case JPEG_LOSSLESS:
    status = ippiEncodeHuffmanOne_JPEG_16s1u_C1(
               0,dst,dstLen,&currPos,0,m_state_t[thread_id],1);
    break;
  default:
    break;
  }
#ifdef _OPENMP
  m_BitStreamOutT[thread_id].SetCurrPos(currPos);
#else
  m_BitStreamOut.SetCurrPos(currPos);
#endif
  if(ippStsNoErr > status)
  {
    LOG1("IPP Error: ippiEncodeHuffman8x8_JPEG_16s1u_C1() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }
#ifdef _OPENMP
  status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state_t[thread_id]);
#else
   status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state);
#endif
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  jerr = WriteRST_T(nRSTI & 7, thread_id);
  if(JPEG_OK != jerr)
  {
    LOG1("IPP Error: WriteRST() failed - ",jerr);
    return jerr;
  }

  for(c = 0; c < m_jpeg_ncomp; c++)
  {
    m_ccomp[c].m_lastDC = 0;
  }

#ifdef _OPENMP
    for(c = 0; c < m_jpeg_ncomp; c++)
      m_lastDC[thread_id][c] = 0;
#endif

  return JPEG_OK;
} // CJPEGEncoder::ProcessRestart()


JERRCODE CJPEGEncoder::EncodeHuffmanMCURowBL_RSTI(Ipp16s* pMCUBuf, int thread_id)
{
  int                    c;
  int                    vs;
  int                    hs;
  int                    xmcu;
  int                    dstLen;
  int                    currPos;
  Ipp8u*                 dst;
  CJPEGColorComponent*   curr_comp;
  IppiEncodeHuffmanSpec* pDCTbl = 0;
  IppiEncodeHuffmanSpec* pACTbl = 0;
  IppStatus              status;

#ifdef _OPENMP

  dst    = m_BitStreamOutT[thread_id].GetDataPtr();
  dstLen = m_BitStreamOutT[thread_id].GetDataLen();
#else
  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();
#endif

  for(xmcu = 0; xmcu < m_numxMCU; xmcu++)
  {
    for(c = 0; c < m_jpeg_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];
      pDCTbl = m_dctbl[curr_comp->m_dc_selector];
      pACTbl = m_actbl[curr_comp->m_ac_selector];

      for(vs = 0; vs < curr_comp->m_vsampling; vs++)
      {
        for(hs = 0; hs < curr_comp->m_hsampling; hs++)
        {
#ifdef _OPENMP
          currPos = m_BitStreamOutT[thread_id].GetCurrPos();
#else
          m_BitStreamOut.FlushBuffer(SAFE_NBYTES);

          currPos = m_BitStreamOut.GetCurrPos();
#endif

#ifdef _OPENMP
          status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
                     pMCUBuf,dst,dstLen,&currPos,
                     &m_lastDC[thread_id][c],pDCTbl,pACTbl,m_state_t[thread_id],0);
#else
          status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
                     pMCUBuf,dst,dstLen,&currPos,
                     &curr_comp->m_lastDC,pDCTbl,pACTbl,m_state,0);

#endif

#ifdef _OPENMP
          m_BitStreamOutT[thread_id].SetCurrPos(currPos);
#else
          m_BitStreamOut.SetCurrPos(currPos);
#endif
          if(ippStsNoErr > status)
          {
            LOG1("IPP Error: ippiEncodeHuffman8x8_JPEG_16s1u_C1() failed - ",status);
            return JPEG_ERR_INTERNAL;
          }

          pMCUBuf += DCTSIZE2;
        } // for m_hsampling
      } // for m_vsampling
    } // for m_jpeg_ncomp
  } // for numxMCU

  return JPEG_OK;
} // CJPEGEncoder::EncodeHuffmanMCURowBL_RSTI()



JERRCODE CJPEGEncoder::EncodeScanBaselineRSTI(void)
{
  int i;
  int dstLen;
  int currPos;
  Ipp8u* dst;

  JERRCODE  jerr;
  IppStatus status;

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    m_ccomp[i].m_lastDC = 0;
  }

  status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  jerr = WriteSOS();
  if(JPEG_OK != jerr)
  {
    LOG0("Error: WriteSOS() failed");
    return jerr;
  }

#ifdef _OPENMP
  jerr = m_BitStreamOut.FlushBuffer();
  if(JPEG_OK != jerr)
    return jerr;

  for(i =0; i < m_num_threads; i++)
  {
    ippiEncodeHuffmanStateInit_JPEG_8u(m_state_t[i]);
  }
#endif

  i = 0;

#ifdef _OPENMP
#pragma omp parallel
#endif
  {
    int     rh         = 0;
    int     currMCURow = 0;
    int     thread_id  = 0;
    Ipp16s* pMCUBuf    = 0;  // the pointer to Buffer for a current thread.

#ifdef _OPENMP
    thread_id = omp_get_thread_num(); // the thread id of the calling thread.
#endif

    pMCUBuf = m_block_buffer + (thread_id) * m_numxMCU * m_nblock * DCTSIZE2*m_rstiHeight;
#ifdef _OPENMP
#pragma omp for ordered schedule(static, 1)
#endif
      for(int curr_rsti = 0; curr_rsti < m_num_rsti; curr_rsti++)
      {
        currMCURow = curr_rsti*m_rstiHeight;

        rh = IPP_MIN(m_rstiHeight, m_numyMCU - currMCURow);

        for(int r = 0; r < rh; r++)
        {
          jerr = ColorConvert(currMCURow + r,thread_id);

          jerr = DownSampling(currMCURow + r,thread_id);

          jerr = TransformMCURowBL(pMCUBuf, thread_id);

          jerr = EncodeHuffmanMCURowBL_RSTI(pMCUBuf, thread_id);

        }

         // do restart procedure evry rsti
         if(m_jpeg_restart_interval && curr_rsti  < m_num_rsti - 1)
              jerr = ProcessRestart(0,0,63,0,0, curr_rsti, thread_id);

         // flush IppiEncodeHuffmanState for last RSTI
         if(curr_rsti  == m_num_rsti - 1)
         {
           dst    = m_BitStreamOutT[thread_id].GetDataPtr();
           dstLen = m_BitStreamOutT[thread_id].GetDataLen();

           currPos = m_BitStreamOutT[thread_id].GetCurrPos();

           status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
                    0,dst,dstLen,&currPos,0,0,0,m_state_t[thread_id],1);

           m_BitStreamOutT[thread_id].SetCurrPos(currPos);
         }

#ifdef _OPENMP
#pragma omp ordered
{
         m_BitStreamOut.FlushBitStream(m_BitStreamOutT[thread_id]);
}
#endif

      } // for curr_rsti
} // omp

  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  if(ippStsNoErr > status)
  {
    LOG1("IPP Error: ippiEncodeHuffman8x8_JPEG_16s1u_C1() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeScanBaselineRSTI()



JERRCODE CJPEGEncoder::EncodeScanBaselineRSTI_P(void)
{
  int i;
  int dstLen;
  int currPos;
  Ipp8u* dst;

  JERRCODE  jerr;
  IppStatus status;

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    m_ccomp[i].m_lastDC = 0;
  }

  status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  jerr = WriteSOS();
  if(JPEG_OK != jerr)
  {
    LOG0("Error: WriteSOS() failed");
    return jerr;
  }

#ifdef _OPENMP
  jerr = m_BitStreamOut.FlushBuffer();
  if(JPEG_OK != jerr)
    return jerr;

  for(i =0; i < m_num_threads; i++)
  {
    ippiEncodeHuffmanStateInit_JPEG_8u(m_state_t[i]);
  }
#endif

  i = 0;

#ifdef _OPENMP
#pragma omp parallel
#endif
  {
    int     rh         = 0;
    int     currMCURow = 0;
    int     thread_id  = 0;
    Ipp16s* pMCUBuf    = 0;  // the pointer to Buffer for a current thread.

#ifdef _OPENMP
    thread_id = omp_get_thread_num(); // the thread id of the calling thread.
#endif

    pMCUBuf = m_block_buffer + (thread_id) * m_numxMCU * m_nblock * DCTSIZE2*m_rstiHeight;
#ifdef _OPENMP
#pragma omp for ordered schedule(static, 1)
#endif
      for(int curr_rsti = 0; curr_rsti < m_num_rsti; curr_rsti++)
      {
        currMCURow = curr_rsti*m_rstiHeight;

        rh = IPP_MIN(m_rstiHeight, m_numyMCU - currMCURow);

        for(int r = 0; r < rh; r++)
        {
          if(m_src.color == m_jpeg_color)
          {
            jerr = ProcessBuffer(currMCURow + r,thread_id);
          }
          else
          {
            jerr = ColorConvert(currMCURow + r,thread_id);

            jerr = DownSampling(currMCURow + r,thread_id);
          }

          jerr = TransformMCURowBL(pMCUBuf, thread_id);

          jerr = EncodeHuffmanMCURowBL_RSTI(pMCUBuf, thread_id);

        }

         // do restart procedure evry rsti
         if(m_jpeg_restart_interval && curr_rsti  < m_num_rsti - 1)
              jerr = ProcessRestart(0,0,63,0,0, curr_rsti, thread_id);

         // flush IppiEncodeHuffmanState for last RSTI
         if(curr_rsti  == m_num_rsti - 1)
         {
           dst    = m_BitStreamOutT[thread_id].GetDataPtr();
           dstLen = m_BitStreamOutT[thread_id].GetDataLen();

           currPos = m_BitStreamOutT[thread_id].GetCurrPos();

           status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
                    0,dst,dstLen,&currPos,0,0,0,m_state_t[thread_id],1);

           m_BitStreamOutT[thread_id].SetCurrPos(currPos);
         }

#ifdef _OPENMP
#pragma omp ordered
{
         m_BitStreamOut.FlushBitStream(m_BitStreamOutT[thread_id]);
}
#endif

      } // for curr_rsti
} // omp

  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  if(ippStsNoErr > status)
  {
    LOG1("IPP Error: ippiEncodeHuffman8x8_JPEG_16s1u_C1() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeScanBaselineRSTI_P()

