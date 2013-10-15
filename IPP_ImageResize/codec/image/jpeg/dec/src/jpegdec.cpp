/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2012 Intel Corporation. All Rights Reserved.
//
*/
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

CJPEGDecoder::CJPEGDecoder(void)
{
  Reset();
  return;
} // ctor


CJPEGDecoder::~CJPEGDecoder(void)
{
  Clean();
  return;
} // dtor


void CJPEGDecoder::Reset(void)
{
  m_jpeg_width             = 0;
  m_jpeg_height            = 0;
  m_jpeg_ncomp             = 0;
  m_jpeg_precision         = 8;
  m_jpeg_sampling          = JS_OTHER;
  m_jpeg_color             = JC_UNKNOWN;
  m_jpeg_quality           = 100;
  m_jpeg_restart_interval  = 0;
  m_jpeg_mode              = JPEG_UNKNOWN;
  m_jpeg_dct_scale         = JD_1_1;
  m_dd_factor              = 1;

  m_jpeg_comment_detected  = 0;
  m_jpeg_comment           = 0;
  m_jpeg_comment_size      = 0;

  m_jfif_app0_detected     = 0;
  m_jfif_app0_major        = 0;
  m_jfif_app0_minor        = 0;
  m_jfif_app0_units        = JRU_NONE;
  m_jfif_app0_xDensity     = 0;
  m_jfif_app0_yDensity     = 0;
  m_jfif_app0_thumb_width  = 0;
  m_jfif_app0_thumb_height = 0;

  m_jfxx_app0_detected     = 0;
  m_jfxx_thumbnails_type   = 0;

  m_avi1_app0_detected     = 0;
  m_avi1_app0_polarity     = 0;
  m_avi1_app0_reserved     = 0;
  m_avi1_app0_field_size   = 0;
  m_avi1_app0_field_size2  = 0;

  m_exif_app1_detected     = 0;
  m_exif_app1_data_size    = 0;
  m_exif_app1_data         = 0;

  m_adobe_app14_detected   = 0;
  m_adobe_app14_version    = 0;
  m_adobe_app14_flags0     = 0;
  m_adobe_app14_flags1     = 0;
  m_adobe_app14_transform  = 0;

  m_precision              = 0;
  m_max_hsampling          = 0;
  m_max_vsampling          = 0;
  m_numxMCU                = 0;
  m_numyMCU                = 0;
  m_mcuWidth               = 0;
  m_mcuHeight              = 0;
  m_ccWidth                = 0;
  m_ccHeight               = 0;
  m_xPadding               = 0;
  m_yPadding               = 0;
  m_rst_go                 = 0;
  m_restarts_to_go         = 0;
  m_next_restart_num       = 0;
  m_sos_len                = 0;
  m_curr_comp_no           = 0;
  m_scan_ncomps            = 0;
  m_ss                     = 0;
  m_se                     = 0;
  m_al                     = 0;
  m_ah                     = 0;
  m_dc_scan_completed      = 0;
  m_ac_scans_completed     = 0;
  m_init_done              = 0;
  m_marker                 = JM_NONE;

  m_scan_count             = 0;

  m_block_buffer           = 0;
  m_num_threads            = 0;
  m_nblock                 = 0;

  m_use_qdct               = 0;
  m_sof_find               = 0;

  m_rsti_height            = 0;
  m_rsti_offset            = 0;
  m_num_rsti               = 0;
  m_threading_mode         = JT_OLD;

#ifdef __TIMING__
  m_clk_dct                = 0;

  m_clk_dct1x1             = 0;
  m_clk_dct2x2             = 0;
  m_clk_dct4x4             = 0;
  m_clk_dct8x8             = 0;

  m_clk_ss                 = 0;
  m_clk_cc                 = 0;
  m_clk_diff               = 0;
  m_clk_huff               = 0;
#endif

  return;
} // CJPEGDecoder::Reset(void)


JERRCODE CJPEGDecoder::Clean(void)
{
  int i;

  m_jpeg_comment_detected = 0;

  if(0 != m_jpeg_comment)
  {
    ippFree(m_jpeg_comment);
    m_jpeg_comment = 0;
    m_jpeg_comment_size = 0;
  }

  m_avi1_app0_detected    = 0;
  m_avi1_app0_polarity    = 0;
  m_avi1_app0_reserved    = 0;
  m_avi1_app0_field_size  = 0;
  m_avi1_app0_field_size2 = 0;

  m_jfif_app0_detected    = 0;
  m_jfxx_app0_detected    = 0;

  m_exif_app1_detected    = 0;

  if(0 != m_exif_app1_data)
  {
    ippFree(m_exif_app1_data);
    m_exif_app1_data = 0;
  }

  m_adobe_app14_detected = 0;

  m_scan_ncomps = 0;
  m_init_done   = 0;

  for(i = 0; i < MAX_COMPS_PER_SCAN; i++)
  {
    if(0 != m_ccomp[i].m_curr_row)
    {
      ippFree(m_ccomp[i].m_curr_row);
      m_ccomp[i].m_curr_row = 0;
    }
    if(0 != m_ccomp[i].m_prev_row)
    {
      ippFree(m_ccomp[i].m_prev_row);
      m_ccomp[i].m_prev_row = 0;
    }
  }

  for(i = 0; i < MAX_HUFF_TABLES; i++)
  {
    m_dctbl[i].Destroy();
    m_actbl[i].Destroy();
  }

  if(0 != m_block_buffer)
  {
    ippFree(m_block_buffer);
    m_block_buffer = 0;
  }

#ifdef _OPENMP
  if(m_threading_mode == JT_RSTI)
  {
    if(0 != m_rsti_offset)
    {
      ippFree(m_rsti_offset);
      m_rsti_offset = 0;
    }

    delete[] m_BitStreamInT;

    for(i = 0; i < m_num_threads; i++)
    {
      m_state_t[i].Destroy();
      delete[] m_lastDC[i];
    }

    delete[] m_state_t;
    delete[] m_lastDC;
  }

#endif

  m_state.Destroy();

  return JPEG_OK;
} // CJPEGDecoder::Clean()


#define BS_BUFLEN 16384

JERRCODE CJPEGDecoder::SetSource(
  CBaseStreamInput* pInStream)
{
  JERRCODE jerr;

  jerr = m_BitStreamIn.Attach(pInStream);
  if(JPEG_OK != jerr)
    return jerr;

  return m_BitStreamIn.Init(BS_BUFLEN);
} // CJPEGDecoder::SetSource()


JERRCODE CJPEGDecoder::SetDestination(
  Ipp8u*   pDst,
  int      dstStep,
  IppiSize dstSize,
  int      dstChannels,
  JCOLOR   dstColor,
  JSS      dstSampling,
  int      dstPrecision,
  JDD      dstDctScale)
{
  if(0 == pDst)
    return JPEG_ERR_PARAMS;

  if(0 > dstStep)
    return JPEG_ERR_PARAMS;

  if(dstChannels <= 0 || dstChannels > 4)
    return JPEG_ERR_PARAMS;

  if(dstPrecision <= 0 || dstPrecision != m_jpeg_precision)
    return JPEG_ERR_PARAMS;

  if(m_jpeg_color == JC_YCBCR && dstColor == JC_GRAY && dstChannels != 1)
    return JPEG_ERR_PARAMS;

  m_dst.p.Data8u[0] = pDst;
  m_dst.lineStep[0] = dstStep;
  m_dst.width       = dstSize.width;
  m_dst.height      = dstSize.height;
  m_dst.nChannels   = dstChannels;
  m_dst.color       = dstColor;
  m_dst.sampling    = dstSampling;
  m_dst.precision   = dstPrecision;

  m_jpeg_dct_scale  = dstDctScale;

  m_dst.order = JD_PIXEL;

  return JPEG_OK;
} // CJPEGDecoder::SetDestination()


 JERRCODE CJPEGDecoder::SetDestination(
  Ipp16s*  pDst,
  int      dstStep,
  IppiSize dstSize,
  int      dstChannels,
  JCOLOR   dstColor,
  JSS      dstSampling,
  int      dstPrecision)
{
  m_dst.p.Data16s[0] = pDst;
  m_dst.lineStep[0]  = dstStep;
  m_dst.width        = dstSize.width;
  m_dst.height       = dstSize.height;
  m_dst.nChannels    = dstChannels;
  m_dst.color        = dstColor;
  m_dst.sampling     = dstSampling;
  m_dst.precision    = dstPrecision;

  m_dst.order = JD_PIXEL;

  return JPEG_OK;
} // CJPEGDecoder::SetDestination()


 JERRCODE CJPEGDecoder::SetDestination(
  Ipp8u*   pDst[],
  int      dstStep[],
  IppiSize dstSize,
  int      dstChannels,
  JCOLOR   dstColor,
  JSS      dstSampling,
  int      dstPrecision,
  JDD      dstDctScale)
{

  if(dstChannels == 4 && m_jpeg_color != dstColor)
    return JPEG_ERR_PARAMS;

  if(m_jpeg_color == JC_YCBCR && dstColor == JC_GRAY && dstChannels != 1)
    return JPEG_ERR_PARAMS;

  m_dst.p.Data8u[0] = pDst[0];
  m_dst.p.Data8u[1] = pDst[1];
  m_dst.p.Data8u[2] = pDst[2];
  m_dst.p.Data8u[3] = pDst[3];
  m_dst.lineStep[0] = dstStep[0];
  m_dst.lineStep[1] = dstStep[1];
  m_dst.lineStep[2] = dstStep[2];
  m_dst.lineStep[3] = dstStep[3];

  m_dst.order = JD_PLANE;

  m_dst.width       = dstSize.width;
  m_dst.height      = dstSize.height;
  m_dst.nChannels   = dstChannels;
  m_dst.color       = dstColor;
  m_dst.sampling    = dstSampling;
  m_dst.precision   = dstPrecision;

  m_jpeg_dct_scale  = dstDctScale;

  return JPEG_OK;
} // CJPEGDecoder::SetDestination()


  JERRCODE CJPEGDecoder::SetDestination(
  Ipp16s*  pDst[],
  int      dstStep[],
  IppiSize dstSize,
  int      dstChannels,
  JCOLOR   dstColor,
  JSS      dstSampling,
  int      dstPrecision)
{
  m_dst.p.Data16s[0] = pDst[0];
  m_dst.p.Data16s[1] = pDst[1];
  m_dst.p.Data16s[2] = pDst[2];
  m_dst.p.Data16s[3] = pDst[3];
  m_dst.lineStep[0]  = dstStep[0];
  m_dst.lineStep[1]  = dstStep[1];
  m_dst.lineStep[2]  = dstStep[2];
  m_dst.lineStep[3]  = dstStep[3];

  m_dst.order = JD_PLANE;

  m_dst.width        = dstSize.width;
  m_dst.height       = dstSize.height;
  m_dst.nChannels    = dstChannels;
  m_dst.color        = dstColor;
  m_dst.sampling     = dstSampling;
  m_dst.precision    = dstPrecision;

  return JPEG_OK;
} // CJPEGDecoder::SetDestination()


JERRCODE CJPEGDecoder::DetectSampling(void)
{
  switch(m_jpeg_ncomp)
  {
  case 1:
    if(m_ccomp[0].m_hsampling == 1 && m_ccomp[0].m_vsampling == 1)
    {
      m_jpeg_sampling = JS_444;
    }
    else
    {
      return JPEG_ERR_BAD_DATA;
    }
    break;

  case 3:
    if(m_ccomp[0].m_hsampling == 1 && m_ccomp[0].m_vsampling == 1 &&
       m_ccomp[1].m_hsampling == 1 && m_ccomp[1].m_vsampling == 1 &&
       m_ccomp[2].m_hsampling == 1 && m_ccomp[2].m_vsampling == 1)
    {
      m_jpeg_sampling = JS_444;
    }
    else if(m_ccomp[0].m_hsampling == 2 && m_ccomp[0].m_vsampling == 1 &&
            m_ccomp[1].m_hsampling == 1 && m_ccomp[1].m_vsampling == 1 &&
            m_ccomp[2].m_hsampling == 1 && m_ccomp[2].m_vsampling == 1)
    {
      m_jpeg_sampling = JS_422;
    }
    else if(m_ccomp[0].m_hsampling == 1 && m_ccomp[0].m_vsampling == 2 &&
            m_ccomp[1].m_hsampling == 1 && m_ccomp[1].m_vsampling == 1 &&
            m_ccomp[2].m_hsampling == 1 && m_ccomp[2].m_vsampling == 1)
    {
      m_jpeg_sampling = JS_244;
    }
    else if(m_ccomp[0].m_hsampling == 2 && m_ccomp[0].m_vsampling == 2 &&
            m_ccomp[1].m_hsampling == 1 && m_ccomp[1].m_vsampling == 1 &&
            m_ccomp[2].m_hsampling == 1 && m_ccomp[2].m_vsampling == 1)
    {
      m_jpeg_sampling = JS_411;
    }
    else
    {
      m_jpeg_sampling = JS_OTHER;
    }
    break;

  case 4:
    if(m_ccomp[0].m_hsampling == 1 && m_ccomp[0].m_vsampling == 1 &&
       m_ccomp[1].m_hsampling == 1 && m_ccomp[1].m_vsampling == 1 &&
       m_ccomp[2].m_hsampling == 1 && m_ccomp[2].m_vsampling == 1 &&
       m_ccomp[3].m_hsampling == 1 && m_ccomp[3].m_vsampling == 1)
    {
      m_jpeg_sampling = JS_444;
    }
    else if(m_ccomp[0].m_hsampling == 2 && m_ccomp[0].m_vsampling == 1 &&
            m_ccomp[1].m_hsampling == 1 && m_ccomp[1].m_vsampling == 1 &&
            m_ccomp[2].m_hsampling == 1 && m_ccomp[2].m_vsampling == 1 &&
            m_ccomp[3].m_hsampling == 2 && m_ccomp[3].m_vsampling == 1)
    {
      m_jpeg_sampling = JS_422;
    }
    else if(m_ccomp[0].m_hsampling == 1 && m_ccomp[0].m_vsampling == 2 &&
            m_ccomp[1].m_hsampling == 1 && m_ccomp[1].m_vsampling == 1 &&
            m_ccomp[2].m_hsampling == 1 && m_ccomp[2].m_vsampling == 1 &&
            m_ccomp[3].m_hsampling == 1 && m_ccomp[3].m_vsampling == 2)
    {
      m_jpeg_sampling = JS_244;
    }
    else if(m_ccomp[0].m_hsampling == 2 && m_ccomp[0].m_vsampling == 2 &&
            m_ccomp[1].m_hsampling == 1 && m_ccomp[1].m_vsampling == 1 &&
            m_ccomp[2].m_hsampling == 1 && m_ccomp[2].m_vsampling == 1 &&
            m_ccomp[3].m_hsampling == 2 && m_ccomp[3].m_vsampling == 2)
    {
      m_jpeg_sampling = JS_411;
    }
    else
    {
      m_jpeg_sampling = JS_OTHER;
    }
    break;
  }

  return JPEG_OK;
} // CJPEGDecoder::DetectSampling()


JERRCODE CJPEGDecoder::NextMarker(JMARKER* marker)
{
  int c;
  int n;
  JERRCODE jerr;

  n = 0;

  for(;;)
  {
    jerr = m_BitStreamIn.ReadByte(&c);
    if(JPEG_OK != jerr)
      return jerr;

    if(c != 0xff)
    {
      do
      {
        n++;

        jerr = m_BitStreamIn.ReadByte(&c);
        if(JPEG_OK != jerr)
          return jerr;

      } while(c != 0xff);
    }

    do
    {
      jerr = m_BitStreamIn.ReadByte(&c);
      if(JPEG_OK != jerr)
        return jerr;

    } while(c == 0xff);

    if(c != 0)
    {
      *marker = (JMARKER)c;
      break;
    }
  }

  if(n != 0)
  {
    TRC1("  skip enormous bytes - ",n);
  }

  return JPEG_OK;
} // CJPEGDecoder::NextMarker()


JERRCODE CJPEGDecoder::SkipMarker(void)
{
  int len;
  JERRCODE jerr;

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.Seek(len - 2);
  if(JPEG_OK != jerr)
    return jerr;

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::SkipMarker()


JERRCODE CJPEGDecoder::ProcessRestart(void)
{
  JERRCODE  jerr;
  IppStatus status;

  status = ippiDecodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    LOG0("Error: ippiDecodeHuffmanStateInit_JPEG_8u() failed");
    return JPEG_ERR_INTERNAL;
  }

  for(int n = 0; n < m_jpeg_ncomp; n++)
  {
    m_ccomp[n].m_lastDC = 0;
  }

  jerr = ParseRST();
  if(JPEG_OK != jerr)
  {
    LOG0("Error: ParseRST() failed");
//    return jerr;
  }

  m_rst_go = 1;
  m_restarts_to_go = m_jpeg_restart_interval;

  return JPEG_OK;
} // CJPEGDecoder::ProcessRestart()


JERRCODE CJPEGDecoder::ParseSOI(void)
{
  TRC0("-> SOI");
  m_marker = JM_NONE;
  return JPEG_OK;
} // CJPEGDecoder::ParseSOI()


JERRCODE CJPEGDecoder::ParseEOI(void)
{
  m_sof_find = 0;
  //or may be
  // Reset();
  TRC0("-> EOI");
  m_marker = JM_NONE;
  return JPEG_OK;
} // CJPEGDecoder::ParseEOI()


const int APP0_JFIF_LENGTH = 14;
const int APP0_JFXX_LENGTH = 6;
const int APP0_AVI1_LENGTH = 14;

JERRCODE CJPEGDecoder::ParseAPP0(void)
{
  int b0, b1, b2, b3, b4;
  int len;
  JERRCODE jerr;

  TRC0("-> APP0");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  jerr = m_BitStreamIn.CheckByte(0,&b0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(1,&b1);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(2,&b2);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(3,&b3);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(4,&b4);
  if(JPEG_OK != jerr)
    return jerr;

  if(len >= APP0_JFIF_LENGTH &&
     b0 == 0x4a && // J
     b1 == 0x46 && // F
     b2 == 0x49 && // I
     b3 == 0x46 && // F
     b4 == 0)
  {
    // we've found JFIF APP0 marker
    len -= 5;

    jerr = m_BitStreamIn.Seek(5);
    if(JPEG_OK != jerr)
      return jerr;

    m_jfif_app0_detected = 1;

    jerr = m_BitStreamIn.ReadByte(&m_jfif_app0_major);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamIn.ReadByte(&m_jfif_app0_minor);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamIn.ReadByte((int*)&m_jfif_app0_units);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamIn.ReadWord(&m_jfif_app0_xDensity);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamIn.ReadWord(&m_jfif_app0_yDensity);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamIn.ReadByte(&m_jfif_app0_thumb_width);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamIn.ReadByte(&m_jfif_app0_thumb_height);
    if(JPEG_OK != jerr)
      return jerr;

    len -= 9;
  }

  jerr = m_BitStreamIn.CheckByte(0,&b0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(1,&b1);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(2,&b2);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(3,&b3);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(4,&b4);
  if(JPEG_OK != jerr)
    return jerr;

  if(len >= APP0_JFXX_LENGTH &&
     b0 == 0x4a && // J
     b1 == 0x46 && // F
     b2 == 0x58 && // X
     b3 == 0x58 && // X
     b4 == 0)
  {
    // we've found JFXX APP0 extension marker
    len -= 5;

    jerr = m_BitStreamIn.Seek(5);
    if(JPEG_OK != jerr)
      return jerr;

    m_jfxx_app0_detected = 1;

    jerr = m_BitStreamIn.ReadByte(&m_jfxx_thumbnails_type);
    if(JPEG_OK != jerr)
      return jerr;

    switch(m_jfxx_thumbnails_type)
    {
    case 0x10: break;
    case 0x11: break;
    case 0x13: break;
    default:   break;
    }
    len -= 1;
  }

  jerr = m_BitStreamIn.CheckByte(0,&b0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(1,&b1);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(2,&b2);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(3,&b3);
  if(JPEG_OK != jerr)
    return jerr;

  if(len >= APP0_AVI1_LENGTH &&
     b0 == 0x41 && // A
     b1 == 0x56 && // V
     b2 == 0x49 && // I
     b3 == 0x31)   // 1
  {
    // we've found AVI1 APP0 marker
    len -= 4;

    jerr = m_BitStreamIn.Seek(4);
    if(JPEG_OK != jerr)
      return jerr;

    m_avi1_app0_detected = 1;

    jerr = m_BitStreamIn.ReadByte(&m_avi1_app0_polarity);
    if(JPEG_OK != jerr)
      return jerr;

    len -= 1;

    if(len == 7) // old MJPEG AVI
      len -= 7;

    if(len == 9) // ODML MJPEG AVI
    {
      jerr = m_BitStreamIn.ReadByte(&m_avi1_app0_reserved);
      if(JPEG_OK != jerr)
        return jerr;

      jerr = m_BitStreamIn.ReadDword(&m_avi1_app0_field_size);
      if(JPEG_OK != jerr)
        return jerr;

      jerr = m_BitStreamIn.ReadDword(&m_avi1_app0_field_size2);
      if(JPEG_OK != jerr)
        return jerr;

      len -= 9;
    }
  }

  jerr = m_BitStreamIn.Seek(len);
  if(JPEG_OK != jerr)
    return jerr;

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseAPP0()


JERRCODE CJPEGDecoder::ParseAPP1(void)
{
  int i;
  int b0, b1, b2, b3, b4;
  int len;
  JERRCODE jerr;

  TRC0("-> APP0");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  jerr = m_BitStreamIn.CheckByte(0,&b0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(1,&b1);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(2,&b2);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(3,&b3);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(4,&b4);
  if(JPEG_OK != jerr)
    return jerr;

  if(b0 == 0x45 && // E
     b1 == 0x78 && // x
     b2 == 0x69 && // i
     b3 == 0x66 && // f
     b4 == 0)
  {
    m_exif_app1_detected  = 1;
    m_exif_app1_data_size = len;

    jerr = m_BitStreamIn.Seek(6);
    if(JPEG_OK != jerr)
      return jerr;

    len -= 6;

    if(m_exif_app1_data != 0)
    {
      ippFree(m_exif_app1_data);
      m_exif_app1_data = 0;
    }

    m_exif_app1_data = (Ipp8u*)ippMalloc(len);
    if(0 == m_exif_app1_data)
      return JPEG_ERR_ALLOC;

    for(i = 0; i < len; i++)
    {
      jerr = m_BitStreamIn.ReadByte(&b0);
      if(JPEG_OK != jerr)
        return jerr;

      m_exif_app1_data[i] = (Ipp8u)b0;
    }
  }
  else
  {
    jerr = m_BitStreamIn.Seek(len);
    if(JPEG_OK != jerr)
      return jerr;
  }

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseAPP1()


const int APP14_ADOBE_LENGTH = 12;

JERRCODE CJPEGDecoder::ParseAPP14(void)
{
  int b0, b1, b2, b3, b4;
  int len;
  JERRCODE jerr;

  TRC0("-> APP14");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  jerr = m_BitStreamIn.CheckByte(0,&b0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(1,&b1);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(2,&b2);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(3,&b3);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.CheckByte(4,&b4);
  if(JPEG_OK != jerr)
    return jerr;

  if(len >= APP14_ADOBE_LENGTH &&
     b0 == 0x41 && // A
     b1 == 0x64 && // d
     b2 == 0x6f && // o
     b3 == 0x62 && // b
     b4 == 0x65)   // e
  {
    // we've found Adobe APP14 marker
    len -= 5;

    jerr = m_BitStreamIn.Seek(5);
    if(JPEG_OK != jerr)
      return jerr;

    m_adobe_app14_detected = 1;

    jerr = m_BitStreamIn.ReadWord(&m_adobe_app14_version);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamIn.ReadWord(&m_adobe_app14_flags0);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamIn.ReadWord(&m_adobe_app14_flags1);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamIn.ReadByte(&m_adobe_app14_transform);
    if(JPEG_OK != jerr)
      return jerr;

    TRC1("  adobe_app14_version   - ",m_adobe_app14_version);
    TRC1("  adobe_app14_flags0    - ",m_adobe_app14_flags0);
    TRC1("  adobe_app14_flags1    - ",m_adobe_app14_flags1);
    TRC1("  adobe_app14_transform - ",m_adobe_app14_transform);

    len -= 7;
  }

  jerr = m_BitStreamIn.Seek(len);
  if(JPEG_OK != jerr)
    return jerr;

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseAPP14()


JERRCODE CJPEGDecoder::ParseCOM(void)
{
  int i;
  int c;
  int len;
  JERRCODE jerr;

  TRC0("-> COM");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  TRC1("  bytes for comment - ",len);

  m_jpeg_comment_detected = 1;
  m_jpeg_comment_size     = len;

  if(m_jpeg_comment != 0)
  {
    ippFree(m_jpeg_comment);
  }

  m_jpeg_comment = (Ipp8u*)ippMalloc(len+1);
  if(0 == m_jpeg_comment)
    return JPEG_ERR_ALLOC;

  for(i = 0; i < len; i++)
  {
    jerr = m_BitStreamIn.ReadByte(&c);
    if(JPEG_OK != jerr)
      return jerr;
    m_jpeg_comment[i] = (Ipp8u)c;
  }

  m_jpeg_comment[len] = 0;

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseCOM()


JERRCODE CJPEGDecoder::ParseDQT(void)
{
  int i;
  int id;
  int len;
  JERRCODE jerr;

  TRC0("-> DQT");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  while(len > 0)
  {
    jerr = m_BitStreamIn.ReadByte(&id);
    if(JPEG_OK != jerr)
      return jerr;

    int precision = (id & 0xf0) >> 4;

    TRC1("  id        - ",(id & 0x0f));
    TRC1("  precision - ",precision);

    if((id & 0x0f) >= MAX_QUANT_TABLES)
    {
      return JPEG_ERR_DQT_DATA;
    }

    int q;
    Ipp8u qnt[DCTSIZE2*sizeof(Ipp16s)];
    Ipp8u*  pq8  = (Ipp8u*) qnt;
    Ipp16u* pq16 = (Ipp16u*)qnt;

    for(i = 0; i < DCTSIZE2; i++)
    {
      if(precision)
      {
        jerr = m_BitStreamIn.ReadWord(&q);
        pq16[i] = (Ipp16u)q;
      }
      else
      {
        jerr = m_BitStreamIn.ReadByte(&q);
        pq8[i]  = (Ipp8u)q;
      }

      if(JPEG_OK != jerr)
        return jerr;
    }

    if(precision == 1)
      jerr = m_qntbl[id & 0x0f].Init(id,pq16);
    else
      jerr = m_qntbl[id & 0x0f].Init(id,pq8);
    if(JPEG_OK != jerr)
    {
      return jerr;
    }

    len -= DCTSIZE2 + DCTSIZE2*precision + 1;
  }

  if(len != 0)
  {
    return JPEG_ERR_DQT_DATA;
  }

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseDQT()


JERRCODE CJPEGDecoder::ParseDHT(void)
{
  int i;
  int len;
  int index;
  int count;
  JERRCODE jerr;

  TRC0("-> DHT");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  int v;
  Ipp8u bits[MAX_HUFF_BITS];
  Ipp8u vals[MAX_HUFF_VALS];

  while(len > 16)
  {
    jerr = m_BitStreamIn.ReadByte(&index);
    if(JPEG_OK != jerr)
      return jerr;

    count = 0;
    for(i = 0; i < MAX_HUFF_BITS; i++)
    {
      jerr = m_BitStreamIn.ReadByte(&v);
      if(JPEG_OK != jerr)
        return jerr;

      bits[i] = (Ipp8u)v;
      count += bits[i];
    }

    len -= 16 + 1;

    if(count > MAX_HUFF_VALS || count > len)
    {
      return JPEG_ERR_DHT_DATA;
    }

    for(i = 0; i < count; i++)
    {
      jerr = m_BitStreamIn.ReadByte(&v);
      if(JPEG_OK != jerr)
        return jerr;

      vals[i] = (Ipp8u)v;
    }

    len -= count;

    if(index >> 4)
    {
      // make AC Huffman table
      if(m_actbl[index & 0x0f].IsEmpty())
      {
        jerr = m_actbl[index & 0x0f].Create();
        if(JPEG_OK != jerr)
        {
          LOG0("    Can't create AC huffman table");
          return jerr;
        }
      }

      TRC1("    AC Huffman Table - ",index & 0x0f);
      jerr = m_actbl[index & 0x0f].Init(index & 0x0f,index >> 4,bits,vals);
      if(JPEG_OK != jerr)
      {
        LOG0("    Can't build AC huffman table");
        return jerr;
      }
    }
    else
    {
      // make DC Huffman table
      if(m_dctbl[index & 0x0f].IsEmpty())
      {
        jerr = m_dctbl[index & 0x0f].Create();
        if(JPEG_OK != jerr)
        {
          LOG0("    Can't create DC huffman table");
          return jerr;
        }
      }

      TRC1("    DC Huffman Table - ",index & 0x0f);
      jerr = m_dctbl[index & 0x0f].Init(index & 0x0f,index >> 4,bits,vals);
      if(JPEG_OK != jerr)
      {
        LOG0("    Can't build DC huffman table");
        return jerr;
      }
    }
  }

  if(len != 0)
  {
    return JPEG_ERR_DHT_DATA;
  }

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseDHT()


JERRCODE CJPEGDecoder::ParseSOF0(void)
{
  int i;
  int len;
  CJPEGColorComponent* curr_comp;
  JERRCODE jerr;

  TRC0("-> SOF0");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  jerr = m_BitStreamIn.ReadByte(&m_jpeg_precision);
  if(JPEG_OK != jerr)
    return jerr;

  if(m_jpeg_precision != 8 && m_jpeg_precision != 12)
  {
    return JPEG_ERR_SOF_DATA;
  }

  jerr = m_BitStreamIn.ReadWord(&m_jpeg_height);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.ReadWord(&m_jpeg_width);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.ReadByte(&m_jpeg_ncomp);
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  height    - ",m_jpeg_height);
  TRC1("  width     - ",m_jpeg_width);
  TRC1("  nchannels - ",m_jpeg_ncomp);

  if(m_jpeg_ncomp < 0 || m_jpeg_ncomp > MAX_COMPS_PER_SCAN)
  {
    return JPEG_ERR_SOF_DATA;
  }

  len -= 6;

  if(len != m_jpeg_ncomp * 3)
  {
    return JPEG_ERR_SOF_DATA;
  }

  for(m_nblock = 0, i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    jerr = m_BitStreamIn.ReadByte(&curr_comp->m_id);
    if(JPEG_OK != jerr)
      return jerr;

    int ss;

    jerr = m_BitStreamIn.ReadByte(&ss);
    if(JPEG_OK != jerr)
      return jerr;

    curr_comp->m_hsampling  = (ss >> 4) & 0x0f;
    curr_comp->m_vsampling  = (ss     ) & 0x0f;

    if(m_jpeg_ncomp == 1)
    {
      curr_comp->m_hsampling  = 1;
      curr_comp->m_vsampling  = 1;
    }

    jerr = m_BitStreamIn.ReadByte(&curr_comp->m_q_selector);
    if(JPEG_OK != jerr)
      return jerr;

    if(curr_comp->m_hsampling <= 0 || curr_comp->m_vsampling <= 0)
    {
      return JPEG_ERR_SOF_DATA;
    }

    // num of DU block per component
    curr_comp->m_nblocks = curr_comp->m_hsampling * curr_comp->m_vsampling;

    // num of DU blocks per frame
    m_nblock += curr_comp->m_nblocks;

    TRC1("    id ",curr_comp->m_id);
    TRC1("      hsampling - ",curr_comp->m_hsampling);
    TRC1("      vsampling - ",curr_comp->m_vsampling);
    TRC1("      qselector - ",curr_comp->m_q_selector);
  }

  jerr = DetectSampling();
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  m_max_hsampling = m_ccomp[0].m_hsampling;
  m_max_vsampling = m_ccomp[0].m_vsampling;

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    if(m_max_hsampling < curr_comp->m_hsampling)
      m_max_hsampling = curr_comp->m_hsampling;

    if(m_max_vsampling < curr_comp->m_vsampling)
      m_max_vsampling = curr_comp->m_vsampling;
  }

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    curr_comp->m_h_factor = m_max_hsampling / curr_comp->m_hsampling;
    curr_comp->m_v_factor = m_max_vsampling / curr_comp->m_vsampling;
  }

  m_jpeg_mode = JPEG_BASELINE;

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseSOF0()


JERRCODE CJPEGDecoder::ParseSOF1(void)
{
  int i;
  int len;
  CJPEGColorComponent* curr_comp;
  JERRCODE jerr;

  TRC0("-> SOF0");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  jerr = m_BitStreamIn.ReadByte(&m_jpeg_precision);
  if(JPEG_OK != jerr)
    return jerr;

  if(m_jpeg_precision != 8 && m_jpeg_precision != 12)
  {
    return JPEG_ERR_SOF_DATA;
  }

  jerr = m_BitStreamIn.ReadWord(&m_jpeg_height);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.ReadWord(&m_jpeg_width);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.ReadByte(&m_jpeg_ncomp);
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  height    - ",m_jpeg_height);
  TRC1("  width     - ",m_jpeg_width);
  TRC1("  nchannels - ",m_jpeg_ncomp);

  if(m_jpeg_ncomp < 0 || m_jpeg_ncomp > MAX_COMPS_PER_SCAN)
  {
    return JPEG_ERR_SOF_DATA;
  }

  len -= 6;

  if(len != m_jpeg_ncomp * 3)
  {
    return JPEG_ERR_SOF_DATA;
  }

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    jerr = m_BitStreamIn.ReadByte(&curr_comp->m_id);
    if(JPEG_OK != jerr)
      return jerr;

    int ss;

    jerr = m_BitStreamIn.ReadByte(&ss);
    if(JPEG_OK != jerr)
      return jerr;

    curr_comp->m_hsampling  = (ss >> 4) & 0x0f;
    curr_comp->m_vsampling  = (ss     ) & 0x0f;

    if(m_jpeg_ncomp == 1)
    {
      curr_comp->m_hsampling  = 1;
      curr_comp->m_vsampling  = 1;
    }

    jerr = m_BitStreamIn.ReadByte(&curr_comp->m_q_selector);
    if(JPEG_OK != jerr)
      return jerr;

    if(curr_comp->m_hsampling <= 0 || curr_comp->m_vsampling <= 0)
    {
      return JPEG_ERR_SOF_DATA;
    }

    // num of DU block per component
    curr_comp->m_nblocks = curr_comp->m_hsampling * curr_comp->m_vsampling;

    // num of DU blocks per frame
    m_nblock += curr_comp->m_nblocks;

    TRC1("    id ",curr_comp->m_id);
    TRC1("      hsampling - ",curr_comp->m_hsampling);
    TRC1("      vsampling - ",curr_comp->m_vsampling);
    TRC1("      qselector - ",curr_comp->m_q_selector);
  }

  jerr = DetectSampling();
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  m_max_hsampling = m_ccomp[0].m_hsampling;
  m_max_vsampling = m_ccomp[0].m_vsampling;

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    if(m_max_hsampling < curr_comp->m_hsampling)
      m_max_hsampling = curr_comp->m_hsampling;

    if(m_max_vsampling < curr_comp->m_vsampling)
      m_max_vsampling = curr_comp->m_vsampling;
  }

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    curr_comp->m_h_factor = m_max_hsampling / curr_comp->m_hsampling;
    curr_comp->m_v_factor = m_max_vsampling / curr_comp->m_vsampling;
  }

  m_jpeg_mode = JPEG_EXTENDED;

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseSOF1()


JERRCODE CJPEGDecoder::ParseSOF2(void)
{
  int i;
  int len;
  CJPEGColorComponent* curr_comp;
  JERRCODE jerr;

  TRC0("-> SOF2");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  jerr = m_BitStreamIn.ReadByte(&m_jpeg_precision);
  if(JPEG_OK != jerr)
    return jerr;

  if(m_jpeg_precision != 8)
  {
    return JPEG_NOT_IMPLEMENTED;
  }

  jerr = m_BitStreamIn.ReadWord(&m_jpeg_height);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.ReadWord(&m_jpeg_width);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.ReadByte(&m_jpeg_ncomp);
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  height    - ",m_jpeg_height);
  TRC1("  width     - ",m_jpeg_width);
  TRC1("  nchannels - ",m_jpeg_ncomp);

  if(m_jpeg_ncomp < 0 || m_jpeg_ncomp > MAX_COMPS_PER_SCAN)
  {
    return JPEG_ERR_SOF_DATA;
  }

  len -= 6;

  if(len != m_jpeg_ncomp * 3)
  {
    return JPEG_ERR_SOF_DATA;
  }

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    jerr = m_BitStreamIn.ReadByte(&curr_comp->m_id);
    if(JPEG_OK != jerr)
      return jerr;

    curr_comp->m_comp_no = i;

    int ss;

    jerr = m_BitStreamIn.ReadByte(&ss);
    if(JPEG_OK != jerr)
      return jerr;

    curr_comp->m_hsampling  = (ss >> 4) & 0x0f;
    curr_comp->m_vsampling  = (ss     ) & 0x0f;

    if(m_jpeg_ncomp == 1)
    {
      curr_comp->m_hsampling  = 1;
      curr_comp->m_vsampling  = 1;
    }

    jerr = m_BitStreamIn.ReadByte(&curr_comp->m_q_selector);
    if(JPEG_OK != jerr)
      return jerr;

    if(curr_comp->m_hsampling <= 0 || curr_comp->m_vsampling <= 0)
    {
      return JPEG_ERR_SOF_DATA;
    }

    // num of DU block per component
    curr_comp->m_nblocks = curr_comp->m_hsampling * curr_comp->m_vsampling;

    // num of DU blocks per frame
    m_nblock += curr_comp->m_nblocks;

    TRC1("    id ",curr_comp->m_id);
    TRC1("      hsampling - ",curr_comp->m_hsampling);
    TRC1("      vsampling - ",curr_comp->m_vsampling);
    TRC1("      qselector - ",curr_comp->m_q_selector);
  }

  jerr = DetectSampling();
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  m_max_hsampling = m_ccomp[0].m_hsampling;
  m_max_vsampling = m_ccomp[0].m_vsampling;

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    if(m_max_hsampling < curr_comp->m_hsampling)
      m_max_hsampling = curr_comp->m_hsampling;

    if(m_max_vsampling < curr_comp->m_vsampling)
      m_max_vsampling = curr_comp->m_vsampling;
  }

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    curr_comp->m_h_factor = m_max_hsampling / curr_comp->m_hsampling;
    curr_comp->m_v_factor = m_max_vsampling / curr_comp->m_vsampling;
  }

  m_jpeg_mode = JPEG_PROGRESSIVE;

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseSOF2()


JERRCODE CJPEGDecoder::ParseSOF3(void)
{
  int i;
  int len;
  CJPEGColorComponent* curr_comp;
  JERRCODE jerr;

  TRC0("-> SOF3");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  jerr = m_BitStreamIn.ReadByte(&m_jpeg_precision);
  if(JPEG_OK != jerr)
    return jerr;

  if(m_jpeg_precision < 2 || m_jpeg_precision > 16)
  {
    return JPEG_ERR_SOF_DATA;
  }

  jerr = m_BitStreamIn.ReadWord(&m_jpeg_height);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.ReadWord(&m_jpeg_width);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.ReadByte(&m_jpeg_ncomp);
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  height    - ",m_jpeg_height);
  TRC1("  width     - ",m_jpeg_width);
  TRC1("  nchannels - ",m_jpeg_ncomp);

  len -= 6;

  if(len != m_jpeg_ncomp * 3)
  {
    // too short frame segment
    // need to have 3 bytes per component for parameters
    return JPEG_ERR_SOF_DATA;
  }

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    jerr = m_BitStreamIn.ReadByte(&curr_comp->m_id);
    if(JPEG_OK != jerr)
      return jerr;

    int ss;

    jerr = m_BitStreamIn.ReadByte(&ss);
    if(JPEG_OK != jerr)
      return jerr;

    curr_comp->m_hsampling  = (ss >> 4) & 0x0f;
    curr_comp->m_vsampling  = (ss     ) & 0x0f;

    if(m_jpeg_ncomp == 1)
    {
      curr_comp->m_hsampling  = 1;
      curr_comp->m_vsampling  = 1;
    }

    jerr = m_BitStreamIn.ReadByte(&curr_comp->m_q_selector);
    if(JPEG_OK != jerr)
      return jerr;

    if(curr_comp->m_hsampling <= 0 || curr_comp->m_vsampling <= 0)
    {
      return JPEG_ERR_SOF_DATA;
    }

    // num of DU block per component
    curr_comp->m_nblocks = curr_comp->m_hsampling * curr_comp->m_vsampling;

    // num of DU blocks per frame
    m_nblock += curr_comp->m_nblocks;

    TRC1("    id ",curr_comp->m_id);
    TRC1("      hsampling - ",curr_comp->m_hsampling);
    TRC1("      vsampling - ",curr_comp->m_vsampling);
    TRC1("      qselector - ",curr_comp->m_q_selector);
  }

  jerr = DetectSampling();
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  m_max_hsampling = m_ccomp[0].m_hsampling;
  m_max_vsampling = m_ccomp[0].m_vsampling;

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    if(m_max_hsampling < curr_comp->m_hsampling)
      m_max_hsampling = curr_comp->m_hsampling;

    if(m_max_vsampling < curr_comp->m_vsampling)
      m_max_vsampling = curr_comp->m_vsampling;
  }

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    curr_comp->m_h_factor = m_max_hsampling / curr_comp->m_hsampling;
    curr_comp->m_v_factor = m_max_vsampling / curr_comp->m_vsampling;
  }

  m_jpeg_mode = JPEG_LOSSLESS;

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseSOF3()


JERRCODE CJPEGDecoder::ParseDRI(void)
{
  int len;
  JERRCODE jerr;

  TRC0("-> DRI");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  len -= 2;

  if(len != 2)
  {
    return JPEG_ERR_RST_DATA;
  }

  jerr = m_BitStreamIn.ReadWord(&m_jpeg_restart_interval);
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  restart interval - ",m_jpeg_restart_interval);

  m_restarts_to_go = m_jpeg_restart_interval;

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseDRI()


JERRCODE CJPEGDecoder::ParseRST(void)
{
  JERRCODE jerr;

  TRC0("-> RST");

  if(m_marker == 0xff)
  {
    jerr = m_BitStreamIn.Seek(-1);
    if(JPEG_OK != jerr)
      return jerr;

    m_marker = JM_NONE;
  }

  if(m_marker == JM_NONE)
  {
    jerr = NextMarker(&m_marker);
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


JERRCODE CJPEGDecoder::ParseSOS(JOPERATION op)
{
  int i;
  int ci;
  int len;
  JERRCODE jerr;

  TRC0("-> SOS");

  jerr = m_BitStreamIn.ReadWord(&len);
  if(JPEG_OK != jerr)
    return jerr;

  // store position to return to in subsequent ReadData call
  m_sos_len = len;

  len -= 2;

  jerr = m_BitStreamIn.ReadByte(&m_scan_ncomps);
  if(JPEG_OK != jerr)
    return jerr;

  if(m_scan_ncomps < 1 || m_scan_ncomps > MAX_COMPS_PER_SCAN)
  {
    return JPEG_ERR_SOS_DATA;
  }

  if(JPEG_PROGRESSIVE != m_jpeg_mode && m_scan_ncomps < m_jpeg_ncomp && m_scan_ncomps != 1)
  {
    // TODO:
    // does not support baseline multi-scan images with more than 1 component per scan for now..
    return JPEG_NOT_IMPLEMENTED;
  }

  if(len != ((m_scan_ncomps * 2) + 4))
  {
    return JPEG_ERR_SOS_DATA;
  }

  TRC1("  ncomps - ",m_scan_ncomps);

  for(i = 0; i < m_scan_ncomps; i++)
  {
    int id;
    int huff_sel;

    jerr = m_BitStreamIn.ReadByte(&id);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamIn.ReadByte(&huff_sel);
    if(JPEG_OK != jerr)
      return jerr;

    TRC1("    id - ",id);
    TRC1("      dc_selector - ",(huff_sel >> 4) & 0x0f);
    TRC1("      ac_selector - ",(huff_sel     ) & 0x0f);

    m_ccomp[i].m_lastDC = 0;

    for(ci = 0; ci < m_jpeg_ncomp; ci++)
    {
      if(id == m_ccomp[ci].m_id)
      {
        m_curr_comp_no        = ci;
        m_ccomp[ci].m_comp_no = ci;
        goto comp_id_match;
      }
    }

    return JPEG_ERR_SOS_DATA;

comp_id_match:

    m_ccomp[ci].m_dc_selector = (huff_sel >> 4) & 0x0f;
    m_ccomp[ci].m_ac_selector = (huff_sel     ) & 0x0f;
  }

  jerr = m_BitStreamIn.ReadByte(&m_ss);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamIn.ReadByte(&m_se);
  if(JPEG_OK != jerr)
    return jerr;

  int t;

  jerr = m_BitStreamIn.ReadByte(&t);
  if(JPEG_OK != jerr)
    return jerr;

  m_ah = (t >> 4) & 0x0f;
  m_al = (t     ) & 0x0f;

  TRC1("  Ss - ",m_ss);
  TRC1("  Se - ",m_se);
  TRC1("  Ah - ",m_ah);
  TRC1("  Al - ",m_al);

  if(JO_READ_HEADER == op)
  {
    // detect JPEG color space
    if(m_jfif_app0_detected)
    {
      switch(m_jpeg_ncomp)
      {
      case 1:  m_jpeg_color = JC_GRAY;    break;
      case 3:  m_jpeg_color = JC_YCBCR;   break;
      default: m_jpeg_color = JC_UNKNOWN; break;
      }
    }

    if(m_adobe_app14_detected)
    {
      switch(m_adobe_app14_transform)
      {
      case 0:
        switch(m_jpeg_ncomp)
        {
        case 1:  m_jpeg_color = JC_GRAY;    break;
        case 3:  m_jpeg_color = JC_RGB;     break;
        case 4:  m_jpeg_color = JC_CMYK;    break;
        default: m_jpeg_color = JC_UNKNOWN; break;
        }
        break;

      case 1:  m_jpeg_color = JC_YCBCR;   break;
      case 2:  m_jpeg_color = JC_YCCK;    break;
      default: m_jpeg_color = JC_UNKNOWN; break;
      }
    }

    // try to guess what color space is used...
    if(!m_jfif_app0_detected && !m_adobe_app14_detected)
    {
      switch(m_jpeg_ncomp)
      {
      case 1:  m_jpeg_color = JC_GRAY;    break;
      case 3:  m_jpeg_color = (m_jpeg_mode != JPEG_LOSSLESS && m_jpeg_precision == 8) ? JC_YCBCR : JC_UNKNOWN; break;
      default: m_jpeg_color = JC_UNKNOWN; break;
      }
    }
  }

  m_restarts_to_go   = m_jpeg_restart_interval;
  m_next_restart_num = 0;

  m_marker = JM_NONE;

  return JPEG_OK;
} // CJPEGDecoder::ParseSOS()


JERRCODE CJPEGDecoder::ParseJPEGBitStream(JOPERATION op)
{
  int      i = 0;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif
  JERRCODE jerr = JPEG_OK;

  m_marker = JM_NONE;

  for(;;)
  {
    if(JM_NONE == m_marker)
    {
      jerr = NextMarker(&m_marker);
      if(JPEG_OK != jerr)
      {
        return jerr;
      }
    }

    switch(m_marker)
    {
    case JM_SOI:
      jerr = ParseSOI();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }
      break;

    case JM_APP0:
      jerr = ParseAPP0();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }
      break;

    case JM_APP1:
      jerr = ParseAPP1();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }
      break;

    case JM_APP14:
      jerr = ParseAPP14();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }
      break;

    case JM_COM:
      jerr = ParseCOM();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }
      break;

    case JM_DQT:
      jerr = ParseDQT();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }
      break;

    case JM_SOF0:
      if(m_sof_find)
        return JPEG_ERR_SOF_DATA;

      jerr = ParseSOF0();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }

      m_sof_find = 1;
      break;

    case JM_SOF1:
      if(m_sof_find)
        return JPEG_ERR_SOF_DATA;

      jerr = ParseSOF1();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }

      m_sof_find = 1;
      break;

    case JM_SOF2:
      if(m_sof_find)
        return JPEG_ERR_SOF_DATA;

      jerr = ParseSOF2();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }

      m_sof_find = 1;
      break;

    case JM_SOF3:
      if(m_sof_find)
        return JPEG_ERR_SOF_DATA;

      jerr = ParseSOF3();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }

      m_sof_find = 1;
      break;

    case JM_SOF5:
    case JM_SOF6:
    case JM_SOF7:
    case JM_SOF9:
    case JM_SOFA:
    case JM_SOFB:
    case JM_SOFD:
    case JM_SOFE:
    case JM_SOFF:
      return JPEG_NOT_IMPLEMENTED;

    case JM_DHT:
      jerr = ParseDHT();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }
      break;

    case JM_DRI:
      jerr = ParseDRI();
      if(JPEG_OK != jerr)
      {
        return jerr;
      }
      break;

    case JM_SOS:
      jerr = ParseSOS(op);
      if(JPEG_OK != jerr)
      {
        return jerr;
      }

      if(JO_READ_HEADER == op)
      {
        jerr = m_BitStreamIn.Seek(-(m_sos_len + 2));
        if(JPEG_OK != jerr)
          return jerr;

        // stop here, when we are reading header
        return JPEG_OK;
      }

      if(JO_READ_DATA == op)
      {
        jerr = Init();
        if(JPEG_OK != jerr)
        {
          return jerr;
        }

        switch(m_jpeg_mode)
        {
        case JPEG_BASELINE:
        case JPEG_EXTENDED:
          if(m_scan_ncomps == m_jpeg_ncomp)
          {
            if(JD_PLANE == m_dst.order)
              jerr = DecodeScanBaselineIN_P();
            else
            {
#ifdef _OPENMP
              if(JT_RSTI == m_threading_mode && JS_411 != m_jpeg_sampling)
              {
                jerr = DecodeScanBaselineIN_RSTI();
              }
              else
#endif
                jerr = DecodeScanBaselineIN();

            }
            if(JPEG_OK != jerr)
              return jerr;
          }
          else
          {
            jerr = DecodeScanBaselineNI();
            if(JPEG_OK != jerr)
              return jerr;

            if(m_ac_scans_completed == m_jpeg_ncomp)
            {
              Ipp16s* pMCUBuf;

              for(i = 0; i < m_numyMCU; i++)
              {
                pMCUBuf = m_block_buffer + (i * m_numxMCU * DCTSIZE2 * m_nblock);
#ifdef __TIMING__
                c0 = ippGetCpuClocks();
#endif
                jerr = ReconstructMCURowBL8x8(pMCUBuf);
                if(JPEG_OK != jerr)
                  return jerr;
#ifdef __TIMING__
                c1 = ippGetCpuClocks();
                m_clk_dct += c1 - c0;
#endif

#ifdef __TIMING__
                c0 = ippGetCpuClocks();
#endif
                jerr = UpSampling(i);
                if(JPEG_OK != jerr)
                  return jerr;
#ifdef __TIMING__
                c1 = ippGetCpuClocks();
                m_clk_ss += c1 - c0;
#endif

#ifdef __TIMING__
                c0 = ippGetCpuClocks();
#endif
                jerr = ColorConvert(i);
                if(JPEG_OK != jerr)
                  return jerr;
#ifdef __TIMING__
                c1 = ippGetCpuClocks();
                m_clk_cc += c1 - c0;
#endif
              }
            }
          }
          break;

        case JPEG_PROGRESSIVE:
          {
            jerr = DecodeScanProgressive();

            m_ac_scans_completed = 0;
            for(i = 0; i < m_jpeg_ncomp; i++)
            {
              m_ac_scans_completed += m_ccomp[i].m_ac_scan_completed;
            }

            if(JPEG_OK != jerr ||
              (m_dc_scan_completed != 0 && m_ac_scans_completed == m_jpeg_ncomp))
            {
              Ipp16s* pMCUBuf;
              for(i = 0; i < m_numyMCU; i++)
              {
                pMCUBuf = m_block_buffer + (i* m_numxMCU * DCTSIZE2* m_nblock);

#ifdef __TIMING__
                c0 = ippGetCpuClocks();
#endif
                switch (m_jpeg_dct_scale)
                {
                  case JD_1_1:
                  {
//                    if(m_use_qdct)
//                      jerr = ReconstructMCURowBL8x8_NxN(pMCUBuf);
//                    else
                      jerr = ReconstructMCURowBL8x8(pMCUBuf);
                  }
                  break;

                  case JD_1_2:
                  {
                    jerr = ReconstructMCURowBL8x8To4x4(pMCUBuf);
                  }
                  break;

                  case JD_1_4:
                  {
                    jerr = ReconstructMCURowBL8x8To2x2(pMCUBuf);
                  }
                  break;

                  case JD_1_8:
                  {
                    jerr = ReconstructMCURowBL8x8To1x1(pMCUBuf);
                  }
                  break;

                  default:
                    break;
                }
                if(JPEG_OK != jerr)
                  return jerr;
#ifdef __TIMING__
                c1 = ippGetCpuClocks();
                m_clk_dct += c1 - c0;
#endif
                if(JD_PIXEL == m_dst.order) // pixel by pixel order
                {
#ifdef __TIMING__
                  c0 = ippGetCpuClocks();
#endif
                  jerr = UpSampling(i);
                  if(JPEG_OK != jerr)
                    return jerr;
#ifdef __TIMING__
                  c1 = ippGetCpuClocks();
                  m_clk_ss += c1 - c0;
#endif

#ifdef __TIMING__
                  c0 = ippGetCpuClocks();
#endif
                 jerr = ColorConvert(i);
                  if(JPEG_OK != jerr)
                    return jerr;
#ifdef __TIMING__
                  c1 = ippGetCpuClocks();
                  m_clk_cc += c1 - c0;
#endif
                }
                else          // plane order
                {
                  if(m_jpeg_precision == 8)
                  {
                    jerr = ProcessBuffer(i);
                    if(JPEG_OK != jerr)
                      return jerr;
                  }
                  else
                    return JPEG_NOT_IMPLEMENTED; //not support 16-bit PLANE image
                }
              }
            }
          }
          break;

        case JPEG_LOSSLESS:
          if(m_scan_ncomps == m_jpeg_ncomp)
          {
            jerr = DecodeScanLosslessIN();
            if(JPEG_OK != jerr)
              return jerr;
          }
          else
          {
            jerr = DecodeScanLosslessNI();
            if(JPEG_OK != jerr)
              return jerr;

            if(m_ac_scans_completed == m_jpeg_ncomp)
            {
              Ipp16s* pMCUBuf = m_block_buffer;

              for(i = 0; i < m_numyMCU; i++)
              {
                if(m_jpeg_restart_interval && i*m_numxMCU % m_jpeg_restart_interval == 0)
                  m_rst_go = 1;

#ifdef __TIMING__
                c0 = ippGetCpuClocks();
#endif
                jerr = ReconstructMCURowLS(pMCUBuf, i);
                if(JPEG_OK != jerr)
                  return jerr;
#ifdef __TIMING__
                c1 = ippGetCpuClocks();
                m_clk_diff += c1 - c0;
#endif

#ifdef __TIMING__
                c0 = ippGetCpuClocks();
#endif
                jerr = ColorConvert(i);
                if(JPEG_OK != jerr)
                  return jerr;
#ifdef __TIMING__
                c1 = ippGetCpuClocks();
                m_clk_cc += c1 - c0;
#endif

                m_rst_go = 0;
              } // for m_numyMCU
            }
          }
          break;

        default:
    jerr = JPEG_ERR_INTERNAL;
    break;

        } // m_jpeg_mode

        if(JPEG_OK != jerr)
          return jerr;
      }

      break;

    case JM_RST0:
    case JM_RST1:
    case JM_RST2:
    case JM_RST3:
    case JM_RST4:
    case JM_RST5:
    case JM_RST6:
    case JM_RST7:
      {
        jerr = ParseRST();
        if(JPEG_OK != jerr)
        {
          return jerr;
        }
      }
      break;

    case JM_EOI:
      jerr = ParseEOI();
      goto Exit;

    default:
      TRC1("-> Unknown marker ",m_marker);
      TRC0("..Skipping");
      jerr = SkipMarker();
      if(JPEG_OK != jerr)
        return jerr;

      break;
    }
  }

Exit:

  return jerr;
} // CJPEGDecoder::ParseJPEGBitStream()


JERRCODE CJPEGDecoder::Init(void)
{
  int i;
  int tr_buf_size = 0;
  CJPEGColorComponent* curr_comp;
  JERRCODE  jerr;

  if(m_init_done)
    return JPEG_OK;

  m_num_threads = get_num_threads();

  // TODO:
  //   need to add support for images with more than 4 components per frame

  if(m_dst.precision <= 8)
  {
    switch (m_jpeg_dct_scale)
    {
      case JD_1_1:
      default:
        {
          m_dd_factor = 1;
        }
        break;

      case JD_1_2:
        {
          m_dd_factor = 2;
        }
        break;

      case JD_1_4:
        {
          m_dd_factor = 4;
        }
        break;

      case JD_1_8:
        {
          m_dd_factor = 8;
        }
        break;
    }
  }

  //for(i = 0; i < m_jpeg_ncomp; i++)
  for(i = 0; i < m_dst.nChannels; i++)
  {
    curr_comp = &m_ccomp[i];

    switch(m_jpeg_mode)
    {
    case JPEG_BASELINE:
    case JPEG_EXTENDED:
      {
        int ds = (m_dst.precision <= 8) ? sizeof(Ipp8u) : sizeof(Ipp16s);

        int lnz_ds               = curr_comp->m_vsampling * curr_comp->m_hsampling;
        curr_comp->m_lnz_bufsize = lnz_ds * m_numxMCU;
        curr_comp->m_lnz_ds      = lnz_ds;

        curr_comp->m_cc_height = m_mcuHeight;
        curr_comp->m_cc_step   = m_numxMCU * m_mcuWidth * ds;

        curr_comp->m_ss_height = curr_comp->m_cc_height / curr_comp->m_v_factor;
        curr_comp->m_ss_step   = curr_comp->m_cc_step   / curr_comp->m_h_factor;

        if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2)
        {
          curr_comp->m_ss_height += 2; // add border lines (top and bottom)
        }

        if(m_scan_ncomps == m_jpeg_ncomp)
          tr_buf_size = m_numxMCU * m_nblock * DCTSIZE2 * sizeof(Ipp16s) * m_num_threads;
        else
          tr_buf_size = m_numxMCU * m_numyMCU * m_nblock * DCTSIZE2 * sizeof(Ipp16s);

        break;
      } // JPEG_BASELINE

      case JPEG_PROGRESSIVE:
      {
        int lnz_ds               = curr_comp->m_vsampling * curr_comp->m_hsampling;
        curr_comp->m_lnz_bufsize = lnz_ds * m_numxMCU;
        curr_comp->m_lnz_ds      = lnz_ds;

        curr_comp->m_cc_height = m_mcuHeight;
        curr_comp->m_cc_step   = m_numxMCU * m_mcuWidth;

        curr_comp->m_ss_height = curr_comp->m_cc_height / curr_comp->m_v_factor;
        curr_comp->m_ss_step   = curr_comp->m_cc_step   / curr_comp->m_h_factor;

        if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2)
        {
          curr_comp->m_ss_height += 2; // add border lines (top and bottom)
        }

        tr_buf_size = m_numxMCU * m_numyMCU * m_nblock * DCTSIZE2 * sizeof(Ipp16s);

        break;
      } // JPEG_PROGRESSIVE

      case JPEG_LOSSLESS:
      {
        int lnz_ds               = curr_comp->m_vsampling * curr_comp->m_hsampling;
        curr_comp->m_lnz_bufsize = lnz_ds * m_numxMCU;
        curr_comp->m_lnz_ds      = lnz_ds;

        curr_comp->m_cc_height = m_mcuHeight;
        curr_comp->m_cc_step   = m_numxMCU * m_mcuWidth * sizeof(Ipp16s);

        curr_comp->m_ss_height = curr_comp->m_cc_height / curr_comp->m_v_factor;
        curr_comp->m_ss_step   = curr_comp->m_cc_step   / curr_comp->m_h_factor;

        if(m_scan_ncomps == m_jpeg_ncomp)
          tr_buf_size = m_numxMCU * m_nblock * sizeof(Ipp16s);
        else
          tr_buf_size = m_numxMCU * m_numyMCU * m_nblock * sizeof(Ipp16s);

        curr_comp->m_curr_row = (Ipp16s*)ippMalloc(curr_comp->m_cc_step * sizeof(Ipp16s));
        if(0 == curr_comp->m_curr_row)
          return JPEG_ERR_ALLOC;

        curr_comp->m_prev_row = (Ipp16s*)ippMalloc(curr_comp->m_cc_step * sizeof(Ipp16s));
        if(0 == curr_comp->m_prev_row)
          return JPEG_ERR_ALLOC;

        break;
      } // JPEG_LOSSLESS

      default:
        return JPEG_ERR_PARAMS;
    } // m_jpeg_mode

    // color convert buffer
    jerr = curr_comp->CreateBufferCC(m_num_threads);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = curr_comp->CreateBufferSS(m_num_threads);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = curr_comp->CreateBufferLNZ(m_num_threads);
    if(JPEG_OK != jerr)
      return jerr;

  } // m_jpeg_ncomp

  if(0 == m_block_buffer)
  {
    m_block_buffer = (Ipp16s*)ippMalloc(tr_buf_size);
    if(0 == m_block_buffer)
    {
      return JPEG_ERR_ALLOC;
    }

    ippsZero_8u((Ipp8u*)m_block_buffer,tr_buf_size);
  }

#ifdef _OPENMP

  if(m_threading_mode == JT_RSTI)
  {
    m_rsti_offset = (int*)ippMalloc((m_num_rsti + 1)*sizeof(int));
    if(0 == m_rsti_offset)
      return JPEG_ERR_ALLOC;

    int bitStreamSize = m_numxMCU * m_nblock * DCTSIZE2 * sizeof(Ipp16s)*m_rsti_height;

    m_state_t      = new CJPEGDecoderHuffmanState[m_num_threads];
    m_BitStreamInT = new CBitStreamInput[m_num_threads];
    m_lastDC       = new Ipp16s*[m_num_threads];

    for(i = 0; i < m_num_threads; i++)
    {
      m_state_t[i].Create();

      m_BitStreamInT[i].Attach(&m_BitStreamIn);
      m_BitStreamInT[i].Init(bitStreamSize);

      m_lastDC[i]    = new Ipp16s[4];
      m_lastDC[i][0] =0;
      m_lastDC[i][1] =0;
      m_lastDC[i][2] =0;
      m_lastDC[i][3] =0;
    }
  }
#endif

  m_state.Create();

  m_init_done = 1;

  return JPEG_OK;
} // CJPEGDecoder::Init()


JERRCODE CJPEGDecoder::ColorConvert(int nMCURow,int thread_id)
{
  int       dstStep;
  int       cc_h;
  Ipp8u*    pDst8u  = 0;
  Ipp16u*   pDst16u = 0;
  IppiSize  roi;
  IppStatus status;

  cc_h = m_ccHeight;

  if(nMCURow == m_numyMCU - 1)
  {
    cc_h = m_mcuHeight - m_yPadding;
  }

  roi.width  = m_dst.width;
  roi.height = (cc_h + m_dd_factor - 1) / m_dd_factor;

  if(roi.height == 0)
    return JPEG_OK;

  dstStep = m_dst.lineStep[0];

  if(m_dst.precision <= 8)
    pDst8u   = m_dst.p.Data8u[0] + nMCURow * m_mcuHeight * dstStep / m_dd_factor;
  else
    pDst16u  = (Ipp16u*)((Ipp8u*)m_dst.p.Data16s[0] + nMCURow * m_mcuHeight * dstStep);

  if(m_jpeg_color == JC_UNKNOWN && m_dst.color == JC_UNKNOWN)
  {
    switch(m_jpeg_ncomp)
    {
    case 1:
      {
        int     srcStep;
        Ipp8u*  pSrc8u;
        Ipp16u* pSrc16u;

        srcStep = m_ccomp[0].m_cc_step;

        if(m_dst.precision <= 8)
        {
          pSrc8u = m_ccomp[0].GetCCBufferPtr(thread_id);

          status = ippiCopy_8u_C1R(pSrc8u,srcStep,pDst8u,dstStep,roi);
        }
        else
        {
          pSrc16u = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);

          status = ippiCopy_16s_C1R((Ipp16s*)pSrc16u,srcStep,(Ipp16s*)pDst16u,dstStep,roi);
        }

        if(ippStsNoErr != status)
        {
          LOG1("IPP Error: ippiCopy_8u_C1R() failed - ",status);
          return JPEG_ERR_INTERNAL;
        }
      }
      break;

    case 3:
      {
        int     srcStep;
        Ipp8u*  pSrc8u[3];
        Ipp16u* pSrc16u[3];

        srcStep = m_ccomp[0].m_cc_step;

        if(m_dst.precision <= 8)
        {
          pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
          pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
          pSrc8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

          status = ippiCopy_8u_P3C3R(pSrc8u,srcStep,pDst8u,dstStep,roi);
        }
        else
        {
          pSrc16u[0] = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);
          pSrc16u[1] = (Ipp16u*)m_ccomp[1].GetCCBufferPtr(thread_id);
          pSrc16u[2] = (Ipp16u*)m_ccomp[2].GetCCBufferPtr(thread_id);

          status = ippiCopy_16s_P3C3R((Ipp16s**)pSrc16u,srcStep,(Ipp16s*)pDst16u,dstStep,roi);
        }

        if(ippStsNoErr != status)
        {
          LOG1("IPP Error: ippiCopy_8u_P3C3R() failed - ",status);
          return JPEG_ERR_INTERNAL;
        }
      }
      break;

    case 4:
      {
        int     srcStep;
        Ipp8u*  pSrc8u[4];
        Ipp16u* pSrc16u[4];

        srcStep = m_ccomp[0].m_cc_step;

        if(m_dst.precision <= 8)
        {
          pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
          pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
          pSrc8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);
          pSrc8u[3] = m_ccomp[3].GetCCBufferPtr(thread_id);

          status = ippiCopy_8u_P4C4R(pSrc8u,srcStep,pDst8u,dstStep,roi);
        }
        else
        {
          pSrc16u[0] = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);
          pSrc16u[1] = (Ipp16u*)m_ccomp[1].GetCCBufferPtr(thread_id);
          pSrc16u[2] = (Ipp16u*)m_ccomp[2].GetCCBufferPtr(thread_id);
          pSrc16u[3] = (Ipp16u*)m_ccomp[3].GetCCBufferPtr(thread_id);

          status = ippiCopy_16s_P4C4R((Ipp16s**)pSrc16u,srcStep,(Ipp16s*)pDst16u,dstStep,roi);
        }

        if(ippStsNoErr != status)
        {
          LOG1("IPP Error: ippiCopy_8u_P4C4R() failed - ",status);
          return JPEG_ERR_INTERNAL;
        }
      }
      break;

    default:
      return JPEG_NOT_IMPLEMENTED;
    }
  }

  // Gray to Gray
  if(m_jpeg_color == JC_GRAY && m_dst.color == JC_GRAY)
  {
    int     srcStep;
    Ipp8u*  pSrc8u;
    Ipp16u* pSrc16u;

    srcStep = m_ccomp[0].m_cc_step;

    if(m_dst.precision <= 8)
    {
      pSrc8u = m_ccomp[0].GetCCBufferPtr(thread_id);

      status = ippiCopy_8u_C1R(pSrc8u,srcStep,pDst8u,dstStep,roi);
    }
    else
    {
      pSrc16u = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);

      status = ippiCopy_16s_C1R((Ipp16s*)pSrc16u,srcStep,(Ipp16s*)pDst16u,dstStep,roi);
    }

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_C1R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // Gray to RGB
  if(m_jpeg_color == JC_GRAY && m_dst.color == JC_RGB)
  {
    int    srcStep;
    Ipp8u* pSrc8u[3];

    srcStep = m_ccomp[0].m_cc_step;

    pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[1] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[2] = m_ccomp[0].GetCCBufferPtr(thread_id);

    status = ippiCopy_8u_P3C3R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_P3C3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // Gray to BGR
  if(m_jpeg_color == JC_GRAY && m_dst.color == JC_BGR)
  {
    int    srcStep;
    Ipp8u* pSrc8u[3];

    srcStep = m_ccomp[0].m_cc_step;

    pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[1] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[2] = m_ccomp[0].GetCCBufferPtr(thread_id);

    status = ippiCopy_8u_P3C3R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_P3C3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // RGB to RGB
  if(m_jpeg_color == JC_RGB && m_dst.color == JC_RGB)
  {
    int     srcStep;
    Ipp8u*  pSrc8u[3];
    Ipp16u* pSrc16u[3];

    srcStep = m_ccomp[0].m_cc_step;

    if(m_dst.precision <= 8)
    {
      pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
      pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
      pSrc8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

      status = ippiCopy_8u_P3C3R(pSrc8u,srcStep,pDst8u,dstStep,roi);
    }
    else
    {
      pSrc16u[0] = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);
      pSrc16u[1] = (Ipp16u*)m_ccomp[1].GetCCBufferPtr(thread_id);
      pSrc16u[2] = (Ipp16u*)m_ccomp[2].GetCCBufferPtr(thread_id);

      status = ippiCopy_16s_P3C3R((Ipp16s**)pSrc16u,srcStep,(Ipp16s*)pDst16u,dstStep,roi);
    }

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_P3C3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // RGB to BGR
  if(m_jpeg_color == JC_RGB && m_dst.color == JC_BGR)
  {
    int     srcStep;
    Ipp8u*  pSrc8u[3];
    Ipp16u* pSrc16u[3];

    srcStep = m_ccomp[0].m_cc_step;

    if(m_dst.precision <= 8)
    {
      pSrc8u[2] = m_ccomp[0].GetCCBufferPtr(thread_id);
      pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
      pSrc8u[0] = m_ccomp[2].GetCCBufferPtr(thread_id);

      status = ippiCopy_8u_P3C3R(pSrc8u,srcStep,pDst8u,dstStep,roi);
    }
    else
    {
      pSrc16u[2] = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);
      pSrc16u[1] = (Ipp16u*)m_ccomp[1].GetCCBufferPtr(thread_id);
      pSrc16u[0] = (Ipp16u*)m_ccomp[2].GetCCBufferPtr(thread_id);

      status = ippiCopy_16s_P3C3R((Ipp16s**)pSrc16u,srcStep,(Ipp16s*)pDst16u,dstStep,roi);
    }

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_P3C3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // YCbCr to Gray
  if(m_jpeg_color == JC_YCBCR && m_dst.color == JC_GRAY)
  {
    int    srcStep;
    Ipp8u* pSrc8u;

    srcStep = m_ccomp[0].m_cc_step;

    pSrc8u = m_ccomp[0].GetCCBufferPtr(thread_id);

    status = ippiCopy_8u_C1R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_C1R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // YCbCr to RGB
  if(m_jpeg_color == JC_YCBCR && m_dst.color == JC_RGB)
  {
    int srcStep;
    const Ipp8u* pSrc8u[3];

    srcStep = m_ccomp[0].m_cc_step;

    pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pSrc8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

    if(JD_PIXEL == m_dst.order) { 
        if(m_jpeg_sampling == JS_444 && m_jpeg_mode == JPEG_LOSSLESS)
            status = ippiCopy_8u_P3C3R(pSrc8u, srcStep, pDst8u, dstStep, roi); // Lossless doesn't imply color conversion
        else
            status = ippiYCbCrToRGB_JPEG_8u_P3C3R(pSrc8u,srcStep,pDst8u,dstStep,roi); 
    } 
    else
    {
      Ipp8u* pDst8uP[3];

      pDst8uP[0] = m_dst.p.Data8u[0] + nMCURow * m_mcuHeight * dstStep / m_dd_factor;
      pDst8uP[1] = m_dst.p.Data8u[1] + nMCURow * m_mcuHeight * dstStep / m_dd_factor;
      pDst8uP[2] = m_dst.p.Data8u[2] + nMCURow * m_mcuHeight * dstStep / m_dd_factor;

      status = ippiYCbCrToRGB_JPEG_8u_P3R((const Ipp8u**)pSrc8u,srcStep,(Ipp8u**)pDst8uP,dstStep,roi);
    }

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiYCbCrToRGB_JPEG_8u_P3C3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // YCbCr to BGR
  if(m_jpeg_color == JC_YCBCR && m_dst.color == JC_BGR)
  {
    int srcStep;
    const Ipp8u* pSrc8u[3];

    srcStep = m_ccomp[0].m_cc_step;

    pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pSrc8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

    status = ippiYCbCrToBGR_JPEG_8u_P3C3R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiYCbCrToBGR_JPEG_8u_P3C3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // YCbCr to RGBA (with alfa channel set to 0xFF)
  if(m_jpeg_color == JC_YCBCR && m_dst.color == JC_RGBA)
  {
    int srcStep;
    const Ipp8u* pSrc8u[3];

    srcStep = m_ccomp[0].m_cc_step;

    pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pSrc8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

    status = ippiYCbCrToRGB_JPEG_8u_P3C4R(pSrc8u,srcStep,pDst8u,dstStep,roi, 0xFF);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiYCbCrToRGB_JPEG_8u_P3C4R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // YCbCr to BGRA (with alfa channel set to 0xFF)
  if(m_jpeg_color == JC_YCBCR && m_dst.color == JC_BGRA)
  {
    int srcStep;
    const Ipp8u* pSrc8u[3];

    srcStep = m_ccomp[0].m_cc_step;

    pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pSrc8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

    status = ippiYCbCrToBGR_JPEG_8u_P3C4R(pSrc8u,srcStep,pDst8u,dstStep,roi, 0xFF);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiYCbCrToBGR_JPEG_8u_P3C4R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }


  // YCbCr to YCbCr (422 sampling)
  if(m_jpeg_color == JC_YCBCR && m_dst.color == JC_YCBCR &&
     m_jpeg_sampling == JS_422 && m_dst.sampling == JS_422)
  {
    int    srcStep[3];
    const Ipp8u* pSrc8u[3];

    srcStep[0] = m_ccomp[0].m_cc_step;
    srcStep[1] = m_ccomp[1].m_cc_step;
    srcStep[2] = m_ccomp[2].m_cc_step;

    pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pSrc8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

    status = ippiYCbCr422_8u_P3C2R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiYCbCr422_8u_P3C2R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // CMYK to CMYK
  if(m_jpeg_color == JC_CMYK && m_dst.color == JC_CMYK)
  {
    int    srcStep;
    Ipp8u* pSrc8u[4];

    srcStep = m_ccomp[0].m_cc_step;

    pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pSrc8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);
    pSrc8u[3] = m_ccomp[3].GetCCBufferPtr(thread_id);

    status = ippiCopy_8u_P4C4R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_P4C4R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // YCCK to CMYK
  if(m_jpeg_color == JC_YCCK && m_dst.color == JC_CMYK)
  {
    int    srcStep;
    const Ipp8u* pSrc8u[4];

    srcStep = m_ccomp[0].m_cc_step;

    pSrc8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pSrc8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pSrc8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);
    pSrc8u[3] = m_ccomp[3].GetCCBufferPtr(thread_id);

    status = ippiYCCKToCMYK_JPEG_8u_P4C4R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiYCCKToCMYK_JPEG_8u_P4C4R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  return JPEG_OK;
} // CJPEGDecoder::ColorConvert()


JERRCODE CJPEGDecoder::UpSampling(int nMCURow,int thread_id)
{
  int i, j, k, n, c;
  CJPEGColorComponent* curr_comp;
  IppStatus status;


  int out_ncomp = m_dst.nChannels;

  for(k = 0; k < out_ncomp; k++)
  {
    curr_comp = &m_ccomp[k];
    int need_upsampling = curr_comp->m_need_upsampling;
    // sampling 444
    // nothing to do for 444

    // sampling 422
    if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 1 && need_upsampling)
    {
      int    srcWidth;
      int    srcStep;
      int    dstStep;
      Ipp8u* pSrc;
      Ipp8u* pDst;

      need_upsampling = 0;

      srcStep = curr_comp->m_ss_step;
      dstStep = curr_comp->m_cc_step;

      pSrc = curr_comp->GetSSBufferPtr(thread_id);
      pDst = curr_comp->GetCCBufferPtr(thread_id);

      (m_dd_factor == 1) ? srcWidth = srcStep : srcWidth = (m_dst.width + 1) >> 1;

      if(m_dst.sampling == JS_422)
      {
        IppiSize roi;
        roi.width  = curr_comp->m_ss_step;
        roi.height = curr_comp->m_ss_height;

        status = ippiCopy_8u_C1R(pSrc,srcStep,pDst,dstStep,roi);
        if(ippStsNoErr != status)
        {
          LOG0("Error: ippiCopy_8u_C1R() failed!");
          return JPEG_ERR_INTERNAL;
        }
      }
      else
      {
        for(i = 0; i < m_mcuHeight / m_dd_factor; i++)
        {
          status = ippiSampleUpRowH2V1_Triangle_JPEG_8u_C1(pSrc, srcWidth , pDst);
          if(ippStsNoErr != status)
          {
            LOG0("Error: ippiSampleUpRowH2V1_Triangle_JPEG_8u_C1() failed!");
            return JPEG_ERR_INTERNAL;
          }

          pSrc += srcStep;
          pDst += dstStep;
        }
      }
    }

    // sampling 244
    if(curr_comp->m_h_factor == 1 && curr_comp->m_v_factor == 2 && need_upsampling)
    {
      int    srcStep;
      Ipp8u* pSrc;
      Ipp8u* pDst;

      need_upsampling = 0;

      srcStep = curr_comp->m_ss_step;

      pSrc = curr_comp->GetSSBufferPtr(thread_id);
      pDst = curr_comp->GetCCBufferPtr(thread_id);

      for(i = 0; i < m_mcuHeight >> 1; i++)
      {
        for(n = 0; n < 2; n++)
        {
          status = ippsCopy_8u(pSrc,pDst,srcStep);
          if(ippStsNoErr != status)
          {
            LOG0("Error: ippsCopy_8u() failed!");
            return JPEG_ERR_INTERNAL;
          }
          pDst += srcStep; // dstStep is the same as srcStep
        }
        pSrc += srcStep;
      }
    }

    // sampling 411
    if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2 && need_upsampling)
    {
      int    L;
      int    dd;
      int    ddShift;
      int    srcStep;
      int    dstStep;
      Ipp8u* p1;
      Ipp8u* p2;
      Ipp8u* pSrc;
      Ipp8u* pDst;

      need_upsampling = 0;

      srcStep = curr_comp->m_ss_step;
      dstStep = curr_comp->m_cc_step;

      pSrc = curr_comp->GetSSBufferPtr(thread_id);
      pDst = curr_comp->GetCCBufferPtr(thread_id);

      p1 = pSrc + srcStep;
      p2 = pSrc;

      dd = (m_dd_factor == 1) ? curr_comp->m_ss_height - 1 : (m_dd_factor == 2) ? 5 : (m_dd_factor == 4) ? 3 : 1;
      L = (nMCURow == 0) ? 1 : dd;

      ippsCopy_8u(pSrc + L*srcStep,             pSrc,            srcStep);
      ippsCopy_8u(pSrc + 8*srcStep / m_dd_factor, pSrc + dd*srcStep,srcStep);

      ddShift = dd = (m_dd_factor == 1) ? 1 : (m_dd_factor == 2) ? 2 : (m_dd_factor == 4) ? 3 : 4;
      for(i = 0; i < m_mcuHeight >> ddShift; i++)
      {
        for(n = 0; n < 2; n++)
        {
          p2 = (n == 0) ? p1 - srcStep : (m_dd_factor == 8) ? p1 - srcStep : p1 + srcStep;

          status = ippiSampleUpRowH2V2_Triangle_JPEG_8u_C1(p1, p2, srcStep / m_dd_factor, pDst);
          if(ippStsNoErr != status)
          {
            LOG0("Error: ippiSampleUpRowH2V2_Triangle_JPEG_8u_C1() failed!");
            return JPEG_ERR_INTERNAL;
          }
          pDst += dstStep;
        }
        p1 += srcStep;
      }
    } // 411

    // arbitrary sampling
    if((curr_comp->m_h_factor != 1 || curr_comp->m_v_factor != 1) && need_upsampling)
    {
      int    srcStep;
      int    dstStep;
      int    v_step;
      int    h_step;
      int    val;
      Ipp8u* pSrc;
      Ipp8u* pDst;
      Ipp8u* p;

      srcStep = curr_comp->m_ss_step;
      dstStep = curr_comp->m_cc_step;

      h_step  = curr_comp->m_h_factor;
      v_step  = curr_comp->m_v_factor;

      pSrc = curr_comp->GetSSBufferPtr(thread_id);
      pDst = curr_comp->GetCCBufferPtr(thread_id);

      for(n = 0; n < curr_comp->m_ss_height; n++)
      {
        p = pDst;
        for(i = 0; i < srcStep; i++)
        {
          val = pSrc[i];
          for(j = 0; j < h_step; j++)
            pDst[j] = (Ipp8u)val;

          pDst += h_step;
        } //  for i

        for(c = 0; c < v_step - 1; c++)
        {
          status = ippsCopy_8u(p,pDst,dstStep);
          if(ippStsNoErr != status)
          {
            LOG0("Error: ippsCopy_8u() failed!");
            return JPEG_ERR_INTERNAL;
          }

          pDst += dstStep;
        } //for c

        pSrc += srcStep;
      } // for n
    } // if
  } // for m_jpeg_ncomp

  return JPEG_OK;
} // CJPEGDecoder::UpSampling()


JERRCODE CJPEGDecoder::ProcessBuffer(int nMCURow, int thread_id)
{
  int                  c;
  int                  yPadd = 0;
  int                  srcStep = 0;
  int                  dstStep = 0;
  int                  copyHeight = 0;
  int                  ssHeight;
  int                  out_ncomp  = 0;
  Ipp8u*               pSrc8u  = 0;
  Ipp8u*               pDst8u  = 0;
  Ipp16u*              pSrc16u = 0;
  Ipp16u*              pDst16u = 0;
  IppiSize             roi;
  IppStatus            status;
  CJPEGColorComponent* curr_comp;

  out_ncomp = m_jpeg_ncomp;

  if(m_jpeg_color == JC_YCBCR && m_dst.color == JC_GRAY)
  {
    out_ncomp = 1;
  }

  if(m_jpeg_precision <= 8)
  {
    for(c = 0; c < out_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];

      if(curr_comp->m_v_factor == 1  && curr_comp->m_h_factor == 1)
      {
        srcStep    = curr_comp->m_cc_step;
        pSrc8u     = curr_comp->GetCCBufferPtr(thread_id);
        copyHeight = curr_comp->m_ss_height ;
        yPadd      = m_yPadding;
      }

      if(curr_comp->m_v_factor == 2  && curr_comp->m_h_factor == 2)
      {
        srcStep    = curr_comp->m_ss_step;
        pSrc8u     = curr_comp->GetSSBufferPtr(thread_id);
        copyHeight = curr_comp->m_ss_height - 2;
        yPadd      = m_yPadding/2;
        pSrc8u     = pSrc8u + srcStep; //skip upper border line
      }

      if(curr_comp->m_v_factor == 1  && curr_comp->m_h_factor == 2)
      {
        srcStep    = curr_comp->m_ss_step;
        pSrc8u     = curr_comp->GetSSBufferPtr(thread_id);
        copyHeight = curr_comp->m_ss_height;
        yPadd      = m_yPadding;
      }

      ssHeight = copyHeight;

      if(nMCURow == m_numyMCU - 1)
      {
        copyHeight -= yPadd;
      }

      roi.width  = srcStep    / m_dd_factor;
      roi.height = (m_dd_factor == 1) ? copyHeight : (copyHeight + m_dd_factor - 1) / m_dd_factor;

      if(roi.height == 0)
        return JPEG_OK;

      pDst8u   = m_dst.p.Data8u[c] + nMCURow * ssHeight * m_dst.lineStep[c] / m_dd_factor;

      status = ippiCopy_8u_C1R(pSrc8u, srcStep, pDst8u, m_dst.lineStep[c], roi);
      if(ippStsNoErr != status)
      {
        LOG1("IPP Error: ippiCopy_8u_C1R() failed - ",status);
        return JPEG_ERR_INTERNAL;
      }
    } // for c
  }
  else      // 16-bit(>= 8) planar image with YCBCR color and 444 sampling
  {
    for(c = 0; c < out_ncomp; c++)
    {
      curr_comp  = &m_ccomp[c];
      srcStep    = curr_comp->m_cc_step;
      pSrc16u    = (Ipp16u*)curr_comp->GetCCBufferPtr(thread_id);

      copyHeight = m_ccHeight;

      if(nMCURow == m_numyMCU - 1)
      {
        copyHeight = m_mcuHeight - m_yPadding;
      }

      roi.width  = m_dst.width;
      roi.height = copyHeight;

      if(roi.height == 0)
        return JPEG_OK;

      dstStep  = m_dst.lineStep[c];

      pDst16u  = (Ipp16u*)((Ipp8u*)m_dst.p.Data16s[c] + nMCURow * m_mcuHeight * dstStep);

      status   = ippiCopy_16s_C1R((Ipp16s*)pSrc16u, srcStep, (Ipp16s*)pDst16u, dstStep, roi);
      if(ippStsNoErr != status)
      {
        LOG1("IPP Error: ippiCopy_16s_C1R() failed - ",status);
        return JPEG_ERR_INTERNAL;
      }
    } // for c
  }

  return JPEG_OK;
} // JERRCODE CJPEGDecoder::ProcessBuffer()


JERRCODE CJPEGDecoder::DecodeHuffmanMCURowBL(Ipp16s* pMCUBuf, int thread_id)
{
  int       j, n, k, l;
  int       srcLen;
  int       currPos;
  int       curr_lnz;
  Ipp8u*    src;
  Ipp8u*    lnzBuf;
  JERRCODE  jerr;
  IppStatus status;

  src    = m_BitStreamIn.GetDataPtr();
  srcLen = m_BitStreamIn.GetDataLen();

  Ipp8u* plnz = ((Ipp8u*)((IppiDecodeHuffmanState*)m_state) + 16*sizeof(Ipp8u));

  for(j = 0; j < m_numxMCU; j++)
  {
    if(m_jpeg_restart_interval)
    {
      if(m_restarts_to_go == 0)
      {
        jerr = ProcessRestart();
        if(JPEG_OK != jerr)
        {
          LOG0("Error: ProcessRestart() failed!");
          return jerr;
        }
      }
    }

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
          m_BitStreamIn.FillBuffer(SAFE_NBYTES);

          currPos = m_BitStreamIn.GetCurrPos();

          status = ippiDecodeHuffman8x8_JPEG_1u16s_C1(
                     src,srcLen,&currPos,pMCUBuf,lastDC,(int*)&m_marker,
                     dctbl,actbl,m_state);

          lnzBuf[curr_lnz] = *plnz;
          curr_lnz++;

          m_BitStreamIn.SetCurrPos(currPos);

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

    m_restarts_to_go--;
  } // for m_numxMCU

  return JPEG_OK;
} // CJPEGDecoder::DecodeHuffmanMCURowBL()


JERRCODE CJPEGDecoder::DecodeHuffmanMCURowLS(Ipp16s* pMCUBuf)
{
  int       c;
  Ipp8u*    src;
  Ipp16s*   dst[4];
  int       srcLen;
  int       currPos;
  const IppiDecodeHuffmanSpec* dctbl[4];
  JERRCODE  jerr;
  IppStatus status;

  for(c = 0; c < m_jpeg_ncomp; c++)
  {
    dst[c] = pMCUBuf + c * m_numxMCU;
    dctbl[c] = m_dctbl[m_ccomp[c].m_dc_selector];
  }

  src    = m_BitStreamIn.GetDataPtr();
  srcLen = m_BitStreamIn.GetDataLen();

#if defined (HUFF_ROW_API)

  if(m_jpeg_restart_interval)
  {
    if(m_restarts_to_go == 0)
    {
      jerr = ProcessRestart();
      if(JPEG_OK != jerr)
      {
        LOG0("Error: ProcessRestart() failed!");
        return jerr;
      }
    }
  }

  m_BitStreamIn.FillBuffer();

  currPos = m_BitStreamIn.GetCurrPos();

  status = ippiDecodeHuffmanRow_JPEG_1u16s_C1P4(
             src,srcLen,&currPos,dst, m_numxMCU, m_jpeg_ncomp, (int*)&m_marker,
             dctbl,m_state);

  m_BitStreamIn.SetCurrPos(currPos);

  if(ippStsNoErr > status)
  {
    LOG0("Error: ippiDecodeHuffmanRow_JPEG_1u16s_C1P4() failed!");
    return JPEG_ERR_INTERNAL;
  }

  m_restarts_to_go -= m_numxMCU;

#else

  int                    j, h, v;
  CJPEGColorComponent*   curr_comp;

  for(j = 0; j < m_numxMCU; j++)
  {
    if(m_jpeg_restart_interval)
    {
      if(m_restarts_to_go == 0)
      {
        jerr = ProcessRestart();
        if(JPEG_OK != jerr)
        {
          LOG0("Error: ProcessRestart() failed!");
          return jerr;
        }
      }
    }

    for(c = 0; c < m_jpeg_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];

      for(v = 0; v < curr_comp->m_vsampling; v++)
      {
        for(h = 0; h < curr_comp->m_hsampling; h++)
        {
          m_BitStreamIn.FillBuffer(SAFE_NBYTES);

          currPos = m_BitStreamIn.GetCurrPos();

          status = ippiDecodeHuffmanOne_JPEG_1u16s_C1(
                     src,srcLen,&currPos,dst[c],(int*)&m_marker,
                     dctbl[c],m_state);

          m_BitStreamIn.SetCurrPos(currPos);

          if(ippStsNoErr > status)
          {
            LOG0("Error: ippiDecodeHuffmanOne_JPEG_1u16s_C1() failed!");
            return JPEG_ERR_INTERNAL;
          }

          dst[c]++;
        } // for m_hsampling
      } // for m_vsampling
    } // for m_jpeg_ncomp

    m_restarts_to_go --;
  } // for m_numxMCU
#endif

  return JPEG_OK;
} // CJPEGDecoder::DecodeHuffmanMCURowLS()


JERRCODE CJPEGDecoder::ReconstructMCURowBL8x8_NxN(
  Ipp16s* pMCUBuf,
  int     thread_id)
{
  int       mcu_col, c, k, l, curr_lnz;
  Ipp8u*    lnz     = 0;
  Ipp8u*    dst     = 0;
  int       dstStep = m_ccWidth;
  Ipp16u*   qtbl;
  IppStatus status;
  CJPEGColorComponent* curr_comp;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif

  int out_ncomp = m_dst.nChannels;

  for(mcu_col = 0; mcu_col < m_numxMCU; mcu_col++)
  {
    for(c = 0; c < out_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];
      lnz       = m_ccomp[c].GetLNZBufferPtr(thread_id);
      curr_lnz  = mcu_col * curr_comp->m_lnz_ds;

      qtbl = m_qntbl[curr_comp->m_q_selector];

      for(k = 0; k < curr_comp->m_vsampling; k++)
      {
        if(curr_comp->m_hsampling == m_max_hsampling &&
           curr_comp->m_vsampling == m_max_vsampling)
        {
          dstStep = curr_comp->m_cc_step;
          dst     = curr_comp->GetCCBufferPtr(thread_id) + mcu_col*8*curr_comp->m_hsampling + k*8*dstStep;
        }
        else
        {
          dstStep = curr_comp->m_ss_step;
          dst     = curr_comp->GetSSBufferPtr(thread_id) + mcu_col*8*curr_comp->m_hsampling + k*8*dstStep;

          curr_comp->m_need_upsampling = 1;
        }

        // skip border row (when 244 or 411 sampling)
        if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2)
        {
          dst += dstStep;
        }

        for(l = 0; l < curr_comp->m_hsampling; l++)
        {
          dst += l*8;

#ifdef __TIMING__
          c0 = ippGetCpuClocks();
#endif

          if(lnz[curr_lnz] == 1)  // 1x1
          {
            status = ippiDCTQuantInv8x8LS_1x1_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);
          }
          else if(lnz[curr_lnz] < 5 && pMCUBuf[16] == 0) //2x2
          {
            status = ippiDCTQuantInv8x8LS_2x2_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);
          }
          else if(lnz[curr_lnz] <= 24 //4x4
                 && pMCUBuf[32] == 0
                 && pMCUBuf[33] == 0
                 && pMCUBuf[34] == 0
                 && pMCUBuf[4]  == 0
                 && pMCUBuf[12] == 0)
          {
            status = ippiDCTQuantInv8x8LS_4x4_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);
          }
          else      // 8x8
          {
            status = ippiDCTQuantInv8x8LS_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);
          }

          curr_lnz = curr_lnz + 1;

          if(ippStsNoErr > status)
          {
            LOG0("Error: ippiDCTQuantInv8x8_NxNLS_JPEG_16s8u_C1R() failed!");
            return JPEG_ERR_INTERNAL;
          }
#ifdef __TIMING__
          c1 = ippGetCpuClocks();
          m_clk_dct += c1 - c0;
#endif

          pMCUBuf += DCTSIZE2;
        } // for m_hsampling
      } // for m_vsampling
    } // for m_jpeg_ncomp
  } // for m_numxMCU

  return JPEG_OK;
} // CJPEGDecoder::ReconstructMCURowBL8x8_NxN()


JERRCODE CJPEGDecoder::ReconstructMCURowBL8x8(
  Ipp16s* pMCUBuf,
  int     thread_id)
{
  int       mcu_col, c, k, l;
   int      out_ncomp;
  Ipp8u*    dst     = 0;
  Ipp8u*    p       = 0;
  int       dstStep = m_ccWidth;
  Ipp16u*   qtbl;
  IppStatus status;
  CJPEGColorComponent* curr_comp;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif

  out_ncomp = m_dst.nChannels;

  for(mcu_col = 0; mcu_col < m_numxMCU; mcu_col++)
  {
    for(c = 0; c < out_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];

      qtbl = m_qntbl[curr_comp->m_q_selector];

      for(k = 0; k < curr_comp->m_vsampling; k++)
      {
        if(curr_comp->m_hsampling == m_max_hsampling &&
           curr_comp->m_vsampling == m_max_vsampling)
        {
          dstStep = curr_comp->m_cc_step;
          dst     = curr_comp->GetCCBufferPtr(thread_id) + mcu_col*8*curr_comp->m_hsampling + k*8*dstStep;
        }
        else
        {
          dstStep = curr_comp->m_ss_step;
          dst     = curr_comp->GetSSBufferPtr(thread_id) + mcu_col*8*curr_comp->m_hsampling + k*8*dstStep;

          curr_comp->m_need_upsampling = 1;
        }

        // skip border row (when 244 or 411 sampling)
        if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2)
        {
          dst += dstStep;
        }

        for(l = 0; l < curr_comp->m_hsampling; l++)
        {
          p = dst + l*8;

#ifdef __TIMING__
          c0 = ippGetCpuClocks();
#endif

          status = ippiDCTQuantInv8x8LS_JPEG_16s8u_C1R(pMCUBuf, p, dstStep, qtbl);

          if(ippStsNoErr > status)
          {
            LOG0("Error: ippiDCTQuantInv8x8LS_JPEG_16s8u_C1R() failed!");
            return JPEG_ERR_INTERNAL;
          }
#ifdef __TIMING__
          c1 = ippGetCpuClocks();
          m_clk_dct += c1 - c0;
#endif

          pMCUBuf += DCTSIZE2;
        } // for m_hsampling
      } // for m_vsampling

      if(m_jpeg_color == JC_YCBCR && m_dst.color == JC_GRAY)
      {
        pMCUBuf += DCTSIZE2 * (m_nblock - curr_comp->m_hsampling* curr_comp->m_vsampling);
      }

    } // for m_jpeg_ncomp
  } // for m_numxMCU

  return JPEG_OK;
} // CJPEGDecoder::ReconstructMCURowBL8x8()


JERRCODE CJPEGDecoder::ReconstructMCURowBL8x8To4x4(
  Ipp16s* pMCUBuf,
  int     thread_id)
{
  int       mcu_col, c, k, l;
  Ipp8u*    dst     = 0;
  int       dstStep = m_ccWidth;
  Ipp16u*   qtbl;
  IppStatus status;
  CJPEGColorComponent* curr_comp;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif

  int out_ncomp = m_dst.nChannels;

  for(mcu_col = 0; mcu_col < m_numxMCU; mcu_col++)
  {
    for(c = 0; c < out_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];
      qtbl = m_qntbl[curr_comp->m_q_selector];

      for(k = 0; k < curr_comp->m_vsampling; k++)
      {
        if(curr_comp->m_hsampling == m_max_hsampling &&
           curr_comp->m_vsampling == m_max_vsampling)
        {
          dstStep = curr_comp->m_cc_step;
          dst     = curr_comp->GetCCBufferPtr(thread_id) + mcu_col*4*curr_comp->m_hsampling + k*4*dstStep;

          for(l = 0; l < curr_comp->m_hsampling; l++)
          {
            dst += ((l == 0) ? 0 : 1)*4;

#ifdef __TIMING__
            c0 = ippGetCpuClocks();
#endif
            status = ippiDCTQuantInv8x8To4x4LS_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);

            if(ippStsNoErr > status)
            {
              LOG0("Error: ippiDCTQuantInv8x8To4x4LS_JPEG_16s8u_C1R() failed!");
              return JPEG_ERR_INTERNAL;
            }
#ifdef __TIMING__
            c1 = ippGetCpuClocks();
            m_clk_dct += c1 - c0;
#endif
            pMCUBuf += DCTSIZE2;
          } // for m_hsampling
        }
        else
        {
          if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2 && m_dst.order == JD_PIXEL)
          {
            dstStep = curr_comp->m_cc_step;
            dst     = curr_comp->GetCCBufferPtr(thread_id) + mcu_col*8*curr_comp->m_hsampling + k*8*dstStep;

            for(l = 0; l < curr_comp->m_hsampling; l++)
            {
              dst += ((l == 0) ? 0 : 1)*8;

#ifdef __TIMING__
              c0 = ippGetCpuClocks();
#endif
              status = ippiDCTQuantInv8x8LS_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);

              if(ippStsNoErr > status)
              {
                LOG0("Error: ippiDCTQuantInv8x8LS_JPEG_16s8u_C1R() failed!");
                return JPEG_ERR_INTERNAL;
              }
#ifdef __TIMING__
              c1 = ippGetCpuClocks();
              m_clk_dct += c1 - c0;
#endif
              pMCUBuf += DCTSIZE2;
            } // for m_hsampling
          }
          else
          {
            dstStep = curr_comp->m_ss_step;
            dst     = curr_comp->GetSSBufferPtr(thread_id) + mcu_col*4*curr_comp->m_hsampling + k*4*dstStep;

            curr_comp->m_need_upsampling = 1;

            // skip border row (when 244 or 411 sampling)
            if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2)
            {
              dst += dstStep;
            }

            for(l = 0; l < curr_comp->m_hsampling; l++)
            {
              dst += ((l == 0) ? 0 : 1)*4;

#ifdef __TIMING__
              c0 = ippGetCpuClocks();
#endif
              status = ippiDCTQuantInv8x8To4x4LS_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);

              if(ippStsNoErr > status)
              {
                LOG0("Error: ippiDCTQuantInv8x8To4x4LS_JPEG_16s8u_C1R() failed!");
                return JPEG_ERR_INTERNAL;
              }
#ifdef __TIMING__
              c1 = ippGetCpuClocks();
              m_clk_dct += c1 - c0;
#endif
              pMCUBuf += DCTSIZE2;
            } // for m_hsampling
          }
        }
      } // for m_vsampling
    } // for m_jpeg_ncomp
  } // for m_numxMCU

  return JPEG_OK;
} // CJPEGDecoder::ReconstructMCURowBL8x8To4x4()


JERRCODE CJPEGDecoder::ReconstructMCURowBL8x8To2x2(
  Ipp16s* pMCUBuf,
  int     thread_id)
{
  int       mcu_col, c, k, l;
  Ipp8u*    dst     = 0;
  int       dstStep = m_ccWidth;
  Ipp16u*   qtbl;
  IppStatus status;
  CJPEGColorComponent* curr_comp;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif

  int out_ncomp = m_dst.nChannels;

  for(mcu_col = 0; mcu_col < m_numxMCU; mcu_col++)
  {
    for(c = 0; c < out_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];
      qtbl = m_qntbl[curr_comp->m_q_selector];

      for(k = 0; k < curr_comp->m_vsampling; k++)
      {
        if(curr_comp->m_hsampling == m_max_hsampling &&
           curr_comp->m_vsampling == m_max_vsampling)
        {
          dstStep = curr_comp->m_cc_step;
          dst     = curr_comp->GetCCBufferPtr(thread_id) + mcu_col*2*curr_comp->m_hsampling + k*2*dstStep;

          for(l = 0; l < curr_comp->m_hsampling; l++)
          {
            dst += ((l == 0) ? 0 : 1)*2;

#ifdef __TIMING__
            c0 = ippGetCpuClocks();
#endif
            status = ippiDCTQuantInv8x8To2x2LS_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);

            if(ippStsNoErr > status)
            {
              LOG0("Error: ippiDCTQuantInv8x8To2x2LS_JPEG_16s8u_C1R() failed!");
              return JPEG_ERR_INTERNAL;
            }
#ifdef __TIMING__
            c1 = ippGetCpuClocks();
            m_clk_dct += c1 - c0;
#endif
            pMCUBuf += DCTSIZE2;
          } // for m_hsampling
        }
        else
        {
          if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2 && m_dst.order == JD_PIXEL)
          {
            dstStep = curr_comp->m_cc_step;
            dst     = curr_comp->GetCCBufferPtr(thread_id) + mcu_col*4*curr_comp->m_hsampling + k*4*dstStep;

            for(l = 0; l < curr_comp->m_hsampling; l++)
            {
              dst += ((l == 0) ? 0 : 1)*4;

#ifdef __TIMING__
              c0 = ippGetCpuClocks();
#endif
              status = ippiDCTQuantInv8x8To4x4LS_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);

              if(ippStsNoErr > status)
              {
                LOG0("Error: ippiDCTQuantInv8x8To4x4LS_JPEG_16s8u_C1R() failed!");
                return JPEG_ERR_INTERNAL;
              }
#ifdef __TIMING__
              c1 = ippGetCpuClocks();
              m_clk_dct += c1 - c0;
#endif
              pMCUBuf += DCTSIZE2;
            } // for m_hsampling
          }
          else
          {
            dstStep = curr_comp->m_ss_step;
            dst     = curr_comp->GetSSBufferPtr(thread_id) + mcu_col*2*curr_comp->m_hsampling + k*2*dstStep;

            curr_comp->m_need_upsampling = 1;

            // skip border row (when 244 or 411 sampling)
            if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2)
            {
              dst += dstStep;
            }

            for(l = 0; l < curr_comp->m_hsampling; l++)
            {
              dst += ((l == 0) ? 0 : 1)*2;

#ifdef __TIMING__
              c0 = ippGetCpuClocks();
#endif
              status = ippiDCTQuantInv8x8To2x2LS_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);

              if(ippStsNoErr > status)
              {
                LOG0("Error: ippiDCTQuantInv8x8To2x2LS_JPEG_16s8u_C1R() failed!");
                return JPEG_ERR_INTERNAL;
              }
#ifdef __TIMING__
              c1 = ippGetCpuClocks();
              m_clk_dct += c1 - c0;
#endif
              pMCUBuf += DCTSIZE2;
            } // for m_hsampling
          }
        }
      } // for m_vsampling
    } // for m_jpeg_ncomp
  } // for m_numxMCU

  return JPEG_OK;
} // CJPEGDecoder::ReconstructMCURowBL8x8To2x2()


JERRCODE CJPEGDecoder::ReconstructMCURowBL8x8To1x1(
  Ipp16s* pMCUBuf,
  int     thread_id)
{
  int       mcu_col, c, k, l;
  Ipp8u*    dst     = 0;
  int       dstStep = m_ccWidth;
  Ipp16u*   qtbl;
  IppStatus status;

  CJPEGColorComponent* curr_comp;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif

  int out_ncomp = m_dst.nChannels;

  for(mcu_col = 0; mcu_col < m_numxMCU; mcu_col++)
  {
    for(c = 0; c < out_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];
      qtbl = m_qntbl[curr_comp->m_q_selector];

      for(k = 0; k < curr_comp->m_vsampling; k++)
      {
        if(curr_comp->m_hsampling == m_max_hsampling &&
           curr_comp->m_vsampling == m_max_vsampling)
        {
          dstStep = curr_comp->m_cc_step;
          dst     = curr_comp->GetCCBufferPtr(thread_id) + mcu_col*curr_comp->m_hsampling + k*dstStep;

          for(l = 0; l < curr_comp->m_hsampling; l++)
          {
            dst += (l == 0) ? 0 : 1;

#ifdef __TIMING__
            c0 = ippGetCpuClocks();
#endif
            DCT_QUANT_INV8x8To1x1LS(pMCUBuf, dst, qtbl);

#ifdef __TIMING__
            c1 = ippGetCpuClocks();
            m_clk_dct += c1 - c0;
#endif
            pMCUBuf += DCTSIZE2;
          } // for m_hsampling
        }
        else
        {
          if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2 && m_dst.order == JD_PIXEL)
          {
            dstStep = curr_comp->m_cc_step;
            dst     = curr_comp->GetCCBufferPtr(thread_id) + mcu_col*2*curr_comp->m_hsampling + k*2*dstStep;

            for(l = 0; l < curr_comp->m_hsampling; l++)
            {
              dst += ((l == 0) ? 0 : 1)*2;

#ifdef __TIMING__
              c0 = ippGetCpuClocks();
#endif
              status = ippiDCTQuantInv8x8To2x2LS_JPEG_16s8u_C1R(pMCUBuf, dst, dstStep, qtbl);

              if(ippStsNoErr > status)
              {
                LOG0("Error: ippiDCTQuantInv8x8To2x2LS_JPEG_16s8u_C1R() failed!");
                return JPEG_ERR_INTERNAL;
              }
#ifdef __TIMING__
              c1 = ippGetCpuClocks();
              m_clk_dct += c1 - c0;
#endif
              pMCUBuf += DCTSIZE2;
            } // for m_hsampling
          }
          else
          {
            dstStep = curr_comp->m_ss_step;
            dst     = curr_comp->GetSSBufferPtr(thread_id) + mcu_col*curr_comp->m_hsampling + k*dstStep;

            curr_comp->m_need_upsampling = 1;

            // skip border row (when 244 or 411 sampling)
            if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2)
            {
              dst += dstStep;
            }

            for(l = 0; l < curr_comp->m_hsampling; l++)
            {
              dst += (l == 0) ? 0 : 1;

#ifdef __TIMING__
              c0 = ippGetCpuClocks();
#endif
              DCT_QUANT_INV8x8To1x1LS(pMCUBuf, dst, qtbl);

#ifdef __TIMING__
              c1 = ippGetCpuClocks();
              m_clk_dct += c1 - c0;
#endif
              pMCUBuf += DCTSIZE2;
            } // for m_hsampling
          } // if
        }
      } // for m_vsampling
    } // for m_jpeg_ncomp
  } // for m_numxMCU

  return JPEG_OK;
} // CJPEGDecoder::ReconstructMCURowBL8x8To1x1()


JERRCODE CJPEGDecoder::ReconstructMCURowEX(
  Ipp16s* pMCUBuf,
  int     thread_id)
{
  int       mcu_col, c, k, l;
  Ipp16u*   dst = 0;
  int       dstStep;
  Ipp32f*   qtbl;
  IppStatus status;
  CJPEGColorComponent* curr_comp;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif

  for(mcu_col = 0; mcu_col < m_numxMCU; mcu_col++)
  {
    for(c = 0; c < m_jpeg_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];

      qtbl = m_qntbl[curr_comp->m_q_selector];

      for(k = 0; k < curr_comp->m_vsampling; k++)
      {
        if(curr_comp->m_hsampling == m_max_hsampling &&
           curr_comp->m_vsampling == m_max_vsampling)
        {
          dstStep = curr_comp->m_cc_step;
          dst     = (Ipp16u*)(curr_comp->GetCCBufferPtr(thread_id) + mcu_col*8*curr_comp->m_hsampling*sizeof(Ipp16s) + k*8*dstStep*sizeof(Ipp16s));
        }
        else
        {
          dstStep = curr_comp->m_ss_step;
          dst     = (Ipp16u*)(curr_comp->GetSSBufferPtr(thread_id) + mcu_col*8*curr_comp->m_hsampling*sizeof(Ipp16s) + k*8*dstStep*sizeof(Ipp16s));

          curr_comp->m_need_upsampling = 1;
        }

        // skip border row (when 244 or 411 sampling)
        if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2)
        {
          dst += dstStep;
        }

        for(l = 0; l < curr_comp->m_hsampling; l++)
        {
          dst += l*8;

#ifdef __TIMING__
          c0 = ippGetCpuClocks();
#endif

          status = ippiDCTQuantInv8x8LS_JPEG_16s16u_C1R(
                     pMCUBuf,dst,dstStep,qtbl);
          if(ippStsNoErr > status)
          {
            LOG0("Error: ippiDCTQuantInv8x8LS_JPEG_16s16u_C1R() failed!");
            return JPEG_ERR_INTERNAL;
          }


#ifdef __TIMING__
          c1 = ippGetCpuClocks();
          m_clk_dct += c1 - c0;
#endif

          pMCUBuf += DCTSIZE2;
        } // for m_hsampling
      } // for m_vsampling
    } // for m_jpeg_ncomp
  } // for m_numxMCU

  return JPEG_OK;
} // CJPEGDecoder::ReconstructMCURowEX()


JERRCODE CJPEGDecoder::ReconstructMCURowLS(
  Ipp16s* pMCUBuf,
  int     nMCURow,
  int     thread_id)
{
  int       n;
  int       dstStep;
  Ipp16s*   ptr;
  Ipp16s*   pCurrRow;
  Ipp16s*   pPrevRow;
  Ipp8u*    pDst8u  = 0;
  Ipp16s*   pDst16s = 0;
  IppiSize  roi;
  IppStatus status;
  CJPEGColorComponent* curr_comp;

  thread_id = thread_id; // remove warning

  roi.width  = m_dst.width;
  roi.height = 1;

  for(n = 0; n < m_jpeg_ncomp; n++)
  {
    curr_comp = &m_ccomp[n];

    if(m_dst.precision <= 8)
    {
      dstStep = curr_comp->m_cc_step;
      pDst8u  = curr_comp->GetCCBufferPtr(thread_id);
    }
    else
    {
      dstStep = curr_comp->m_cc_step;
      pDst16s = (Ipp16s*)curr_comp->GetCCBufferPtr(thread_id);
    }

    if(m_jpeg_ncomp == m_scan_ncomps)
      ptr = pMCUBuf + n*m_numxMCU;
    else
      ptr = pMCUBuf + n*m_numxMCU*m_numyMCU + nMCURow*m_numxMCU;

    pCurrRow = curr_comp->m_curr_row;
    pPrevRow = curr_comp->m_prev_row;

    if(0 != nMCURow && 0 == m_rst_go)
    {
      status = ippiReconstructPredRow_JPEG_16s_C1(
        ptr,pPrevRow,pCurrRow,m_dst.width,m_ss);
    }
    else
    {
      status = ippiReconstructPredFirstRow_JPEG_16s_C1(
        ptr,pCurrRow,m_dst.width,m_jpeg_precision,m_al);
    }

    if(ippStsNoErr != status)
    {
      return JPEG_ERR_INTERNAL;
    }

    // do point-transform if any
    status = ippsLShiftC_16s(pCurrRow,m_al,pPrevRow,m_dst.width);
    if(ippStsNoErr != status)
    {
      return JPEG_ERR_INTERNAL;
    }

    if(m_dst.precision <= 8)
    {
      status = ippiAndC_16u_C1IR(0xFF, (Ipp16u*)pPrevRow, m_dst.width*sizeof(Ipp16s),roi);
      status = ippiConvert_16u8u_C1R((Ipp16u*)pPrevRow,m_dst.width*sizeof(Ipp16s),pDst8u,dstStep,roi);
    }
    else
      status = ippsCopy_16s(pPrevRow,pDst16s,m_dst.width);

    if(ippStsNoErr != status)
    {
      return JPEG_ERR_INTERNAL;
    }

    curr_comp->m_curr_row = pPrevRow;
    curr_comp->m_prev_row = pCurrRow;
  } // for m_jpeg_ncomp

  m_rst_go = 0;

  return JPEG_OK;
} // CJPEGDecoder::ReconstructMCURowLS()


JERRCODE CJPEGDecoder::DecodeScanBaselineIN(void)
{
  int scount = 0;
  IppStatus status;
  JERRCODE  jerr;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
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

#ifdef _OPENMP
#pragma omp parallel shared(scount) if(m_jpeg_sampling != JS_411)
#endif
  {
    int     i        = 0;
    int     idThread = 0;
    Ipp16s* pMCUBuf  = 0;  // the pointer to Buffer for a current thread.

#ifdef _OPENMP
    idThread = omp_get_thread_num(); // the thread id of the calling thread.
#endif

    pMCUBuf = m_block_buffer + idThread * m_numxMCU * m_nblock * DCTSIZE2;

    while(i < m_numyMCU)
    {
#ifdef _OPENMP
#pragma omp critical
#endif
      {
        i = scount;
        scount++;
        if(i < m_numyMCU)
        {
#ifdef __TIMING__
          c0 = ippGetCpuClocks();
#endif
          ippsZero_16s(pMCUBuf,m_numxMCU * m_nblock * DCTSIZE2);

          jerr = DecodeHuffmanMCURowBL(pMCUBuf, idThread);
#ifdef __TIMING__
          c1 = ippGetCpuClocks();
          m_clk_huff += (c1 - c0);
#endif
        }
      }

      if(i < m_numyMCU)
      {
#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        if(m_jpeg_precision == 12)
          jerr = ReconstructMCURowEX(pMCUBuf, idThread);
        else
        {
          switch (m_jpeg_dct_scale)
          {
            default:
            case JD_1_1:
            {
              if(m_use_qdct)
                jerr = ReconstructMCURowBL8x8_NxN(pMCUBuf, idThread);
              else
                jerr = ReconstructMCURowBL8x8(pMCUBuf, idThread);
            }
            break;

            case JD_1_2:
            {
              jerr = ReconstructMCURowBL8x8To4x4(pMCUBuf, idThread);
            }
            break;

            case JD_1_4:
            {
              jerr = ReconstructMCURowBL8x8To2x2(pMCUBuf, idThread);
            }
            break;

            case JD_1_8:
            {
              jerr = ReconstructMCURowBL8x8To1x1(pMCUBuf, idThread);
            }
            break;
          }
        }

        if(JPEG_OK != jerr)
          continue;
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_dct += c1 - c0;
#endif

#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif

        jerr = UpSampling(i,idThread);
        if(JPEG_OK != jerr)
          continue;
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_ss += c1 - c0;
#endif

#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        jerr = ColorConvert(i,idThread);
        if(JPEG_OK != jerr)
          continue;
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_cc += c1 - c0;
#endif
      }

#ifndef _OPENMP
      i++;
#endif
    } // for m_numyMCU
  } // OMP

  return JPEG_OK;
} // CJPEGDecoder::DecodeScanBaselineIN()


JERRCODE CJPEGDecoder::DecodeScanBaselineIN_P(void)
{
  int scount = 0;
  IppStatus status;
  JERRCODE  jerr = JPEG_OK;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
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

#ifdef _OPENMP
#pragma omp parallel default(shared) if(m_jpeg_sampling != JS_411)
#endif
  {
    int     i        = 0;
    int     idThread = 0;
    Ipp16s* pMCUBuf  = 0;  // the pointer to Buffer for a current thread.

#ifdef _OPENMP
    idThread = omp_get_thread_num(); // the thread id of the calling thread.
#endif

    pMCUBuf = m_block_buffer + idThread * m_numxMCU * m_nblock * DCTSIZE2;

    while(i < m_numyMCU)
    {
#ifdef _OPENMP
#pragma omp critical (IPP_JPEG_OMP)
#endif
      {
        i = scount;
        scount++;
        if(i < m_numyMCU)
        {
#ifdef __TIMING__
          c0 = ippGetCpuClocks();
#endif
          ippsZero_16s(pMCUBuf,m_numxMCU * m_nblock * DCTSIZE2);

          jerr = DecodeHuffmanMCURowBL(pMCUBuf);
//          if(JPEG_OK != jerr)
//            i = m_numyMCU;
#ifdef __TIMING__
          c1 = ippGetCpuClocks();
          m_clk_huff += (c1 - c0);
#endif
        }
      }

      if(i < m_numyMCU)
      {
#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        if(m_jpeg_precision == 12)
          jerr = ReconstructMCURowEX(pMCUBuf, idThread);
        else
        {
          switch (m_jpeg_dct_scale)
          {
            case JD_1_1:
            {
              if(m_use_qdct)
                jerr = ReconstructMCURowBL8x8_NxN(pMCUBuf, idThread);
              else
                jerr = ReconstructMCURowBL8x8(pMCUBuf, idThread);
            }
            break;

            case JD_1_2:
            {
              jerr = ReconstructMCURowBL8x8To4x4(pMCUBuf, idThread);
            }
            break;

            case JD_1_4:
            {
              jerr = ReconstructMCURowBL8x8To2x2(pMCUBuf, idThread);
            }
            break;

            case JD_1_8:
            {
              jerr = ReconstructMCURowBL8x8To1x1(pMCUBuf, idThread);
            }
            break;

            default:
              break;
          }
        }

        if(JPEG_OK != jerr)
          continue;
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_dct += c1 - c0;
#endif

#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        if(m_jpeg_color == m_dst.color)
        {
          jerr = ProcessBuffer(i,idThread);
        }
        else
        {
          jerr = UpSampling(i,idThread);

          jerr = ColorConvert(i,idThread);
        }
        if(JPEG_OK != jerr)
          continue;
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_ss += c1 - c0;
#endif
      }

      i++;
    } // for m_numyMCU
  } // OMP

  return JPEG_OK;
} // CJPEGDecoder::DecodeScanBaselineIN_P()


JERRCODE CJPEGDecoder::DecodeScanBaselineNI(void)
{
  int       i, j, k, l, c, s;
  int       srcLen;
  int       currPos;
  Ipp8u*    src;
  Ipp16s*   block;
  JERRCODE  jerr;
  IppStatus status;
#ifdef __TIMING__
  Ipp64u    c0;
  Ipp64u    c1;
#endif

  m_scan_count++;
  m_ac_scans_completed += m_scan_ncomps;

  status = ippiDecodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  m_marker = JM_NONE;

  src    = m_BitStreamIn.GetDataPtr();
  srcLen = m_BitStreamIn.GetDataLen();

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

  for(i = 0; i < m_numyMCU; i++)
  {
    for(k = 0; k < m_ccomp[m_curr_comp_no].m_vsampling; k++)
    {
      if(i*m_ccomp[m_curr_comp_no].m_vsampling*8 + k*8 >= m_jpeg_height)
        break;

      for(j = 0; j < m_numxMCU; j++)
      {
        for(c = 0; c < m_scan_ncomps; c++)
        {
          block = m_block_buffer + (DCTSIZE2*m_nblock*(j+(i*m_numxMCU)));

          // skip any relevant components
          for(s = 0; s < m_ccomp[m_curr_comp_no].m_comp_no; s++)
          {
            block += (DCTSIZE2*m_ccomp[s].m_nblocks);
          }

          // Skip over relevant 8x8 blocks from this component.
          block += (k * DCTSIZE2 * m_ccomp[m_curr_comp_no].m_hsampling);

          for(l = 0; l < m_ccomp[m_curr_comp_no].m_hsampling; l++)
          {
            // Ignore the last column(s) of the image.
            if(((j*m_ccomp[m_curr_comp_no].m_hsampling*8) + (l*8)) >= m_jpeg_width)
              break;

            if(m_jpeg_restart_interval)
            {
              if(m_restarts_to_go == 0)
              {
                jerr = ProcessRestart();
                if(JPEG_OK != jerr)
                {
                  LOG0("Error: ProcessRestart() failed!");
                  return jerr;
                }
              }
            }

            Ipp16s*                lastDC = &m_ccomp[m_curr_comp_no].m_lastDC;
            IppiDecodeHuffmanSpec* dctbl = m_dctbl[m_ccomp[m_curr_comp_no].m_dc_selector];
            IppiDecodeHuffmanSpec* actbl = m_actbl[m_ccomp[m_curr_comp_no].m_ac_selector];

            m_BitStreamIn.FillBuffer(SAFE_NBYTES);

            currPos = m_BitStreamIn.GetCurrPos();

#ifdef __TIMING__
            c0 = ippGetCpuClocks();
#endif
            status = ippiDecodeHuffman8x8_JPEG_1u16s_C1(
                       src,srcLen,&currPos,block,lastDC,(int*)&m_marker,
                       dctbl,actbl,m_state);
#ifdef __TIMING__
            c1 = ippGetCpuClocks();
            m_clk_huff += (c1 - c0);
#endif

            m_BitStreamIn.SetCurrPos(currPos);

            if(ippStsNoErr > status)
            {
              LOG0("Error: ippiDecodeHuffman8x8_JPEG_1u16s_C1() failed!");
              return JPEG_ERR_INTERNAL;
            }

            block += DCTSIZE2;

            m_restarts_to_go --;
          } // for m_hsampling
        } // for m_scan_ncomps
      } // for m_numxMCU
    } // for m_vsampling
  } // for m_numyMCU

  return JPEG_OK;
} // CJPEGDecoder::DecodeScanBaselineNI()


JERRCODE CJPEGDecoder::DecodeScanProgressive(void)
{
  int       i, j, k, n, l, c;
  int       srcLen;
  int       currPos;
  Ipp8u*    src;
  Ipp16s*   block;
  JERRCODE  jerr;
  IppStatus status;

  m_scan_count++;

  status = ippiDecodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  m_marker = JM_NONE;

  src    = m_BitStreamIn.GetDataPtr();
  srcLen = m_BitStreamIn.GetDataLen();

  if(m_ss != 0 && m_se != 0)
  {
    // AC scan
    for(i = 0; i < m_numyMCU; i++)
    {
      for(k = 0; k < m_ccomp[m_curr_comp_no].m_vsampling; k++)
      {
        if(i*m_ccomp[m_curr_comp_no].m_vsampling*8 + k*8 >= m_jpeg_height)
          break;

        for(j = 0; j < m_numxMCU; j++)
        {
          block = m_block_buffer + (DCTSIZE2*m_nblock*(j+(i*m_numxMCU)));

          // skip any relevant components
          for(c = 0; c < m_ccomp[m_curr_comp_no].m_comp_no; c++)
          {
            block += (DCTSIZE2*m_ccomp[c].m_nblocks);
          }

          // Skip over relevant 8x8 blocks from this component.
          block += (k * DCTSIZE2 * m_ccomp[m_curr_comp_no].m_hsampling);

          for(l = 0; l < m_ccomp[m_curr_comp_no].m_hsampling; l++)
          {
            // Ignore the last column(s) of the image.
            if(((j*m_ccomp[m_curr_comp_no].m_hsampling*8) + (l*8)) >= m_jpeg_width)
              break;

            if(m_jpeg_restart_interval)
            {
              if(m_restarts_to_go == 0)
              {
                jerr = ProcessRestart();
                if(JPEG_OK != jerr)
                {
                  LOG0("Error: ProcessRestart() failed!");
                  return jerr;
                }
              }
            }

            IppiDecodeHuffmanSpec* actbl = m_actbl[m_ccomp[m_curr_comp_no].m_ac_selector];

            if(m_ah == 0)
            {
              m_BitStreamIn.FillBuffer(SAFE_NBYTES);

              currPos = m_BitStreamIn.GetCurrPos();

              status = ippiDecodeHuffman8x8_ACFirst_JPEG_1u16s_C1(
                         src,srcLen,&currPos,block,(int*)&m_marker,
                         m_ss,m_se,m_al,actbl,m_state);

              m_BitStreamIn.SetCurrPos(currPos);

              if(ippStsNoErr > status)
              {
                LOG0("Error: ippiDecodeHuffman8x8_ACFirst_JPEG_1u16s_C1() failed!");
                return JPEG_ERR_INTERNAL;
              }
            }
            else
            {
              m_BitStreamIn.FillBuffer(SAFE_NBYTES);

              currPos = m_BitStreamIn.GetCurrPos();

              status = ippiDecodeHuffman8x8_ACRefine_JPEG_1u16s_C1(
                         src,srcLen,&currPos,block,(int*)&m_marker,
                         m_ss,m_se,m_al,actbl,m_state);

              m_BitStreamIn.SetCurrPos(currPos);

              if(ippStsNoErr > status)
              {
                LOG0("Error: ippiDecodeHuffman8x8_ACRefine_JPEG_1u16s_C1() failed!");
                return JPEG_ERR_INTERNAL;
              }
            }

            block += DCTSIZE2;

            m_restarts_to_go --;
          } // for m_hsampling
        } // for m_numxMCU
      } // for m_vsampling
    } // for m_numyMCU

    if(m_al == 0 && m_se == 63)
    {
      m_ccomp[m_curr_comp_no].m_ac_scan_completed = 1;
    }
  }
  else
  {
    // DC scan
    for(i = 0; i < m_numyMCU; i++)
    {
      for(j = 0; j < m_numxMCU; j++)
      {
        if(m_jpeg_restart_interval)
        {
          if(m_restarts_to_go == 0)
          {
            jerr = ProcessRestart();
            if(JPEG_OK != jerr)
            {
              LOG0("Error: ProcessRestart() failed!");
              return jerr;
            }
          }
        }

        block = m_block_buffer + (DCTSIZE2*m_nblock*(j+(i*m_numxMCU)));

        if(m_ah == 0)
        {
          // first DC scan
          for(n = 0; n < m_jpeg_ncomp; n++)
          {
            Ipp16s*                lastDC = &m_ccomp[n].m_lastDC;
            IppiDecodeHuffmanSpec* dctbl  = m_dctbl[m_ccomp[n].m_dc_selector];

            for(k = 0; k < m_ccomp[n].m_vsampling; k++)
            {
              for(l = 0; l < m_ccomp[n].m_hsampling; l++)
              {
                m_BitStreamIn.FillBuffer(SAFE_NBYTES);

                currPos = m_BitStreamIn.GetCurrPos();

                status = ippiDecodeHuffman8x8_DCFirst_JPEG_1u16s_C1(
                           src,srcLen,&currPos,block,lastDC,(int*)&m_marker,
                           m_al,dctbl,m_state);

                m_BitStreamIn.SetCurrPos(currPos);

                if(ippStsNoErr > status)
                {
                  LOG0("Error: ippiDecodeHuffman8x8_DCFirst_JPEG_1u16s_C1() failed!");
                  return JPEG_ERR_INTERNAL;
                }

                block += DCTSIZE2;
              } // for m_hsampling
            } // for m_vsampling
          } // for m_jpeg_ncomp
        }
        else
        {
          // refine DC scan
          for(n = 0; n < m_jpeg_ncomp; n++)
          {
            for(k = 0; k < m_ccomp[n].m_vsampling; k++)
            {
              for(l = 0; l < m_ccomp[n].m_hsampling; l++)
              {
                m_BitStreamIn.FillBuffer(SAFE_NBYTES);

                currPos = m_BitStreamIn.GetCurrPos();

                status = ippiDecodeHuffman8x8_DCRefine_JPEG_1u16s_C1(
                           src,srcLen,&currPos,block,(int*)&m_marker,
                           m_al,m_state);

                m_BitStreamIn.SetCurrPos(currPos);

                if(ippStsNoErr > status)
                {
                  LOG0("Error: ippiDecodeHuffman8x8_DCRefine_JPEG_1u16s_C1() failed!");
                  return JPEG_ERR_INTERNAL;
                }

                block += DCTSIZE2;
              } // for m_hsampling
            } // for m_vsampling
          } // for m_jpeg_ncomp
        }
        m_restarts_to_go --;
      } // for m_numxMCU
    } // for m_numyMCU

    if(m_al == 0)
    {
      m_dc_scan_completed = 1;
    }
  }

  return JPEG_OK;
} // CJPEGDecoder::DecodeScanProgressive()


JERRCODE CJPEGDecoder::DecodeScanLosslessIN(void)
{
  int       i;
  Ipp16s*   pMCUBuf;
  JERRCODE  jerr;
  IppStatus status;
#ifdef __TIMING__
  Ipp64u    c0;
  Ipp64u    c1;
#endif

  status = ippiDecodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  m_marker = JM_NONE;

  m_scan_count++;
  m_ac_scans_completed += m_scan_ncomps;

  pMCUBuf = m_block_buffer;

  for(i = 0; i < m_numyMCU; i++)
  {
#ifdef __TIMING__
    c0 = ippGetCpuClocks();
#endif
    jerr = DecodeHuffmanMCURowLS(pMCUBuf);
    if(JPEG_OK != jerr)
    {
      return jerr;
    }
#ifdef __TIMING__
    c1 = ippGetCpuClocks();
    m_clk_huff += c1 - c0;
#endif

#ifdef __TIMING__
    c0 = ippGetCpuClocks();
#endif
    jerr = ReconstructMCURowLS(pMCUBuf, i);
    if(JPEG_OK != jerr)
    {
      return jerr;
    }
#ifdef __TIMING__
    c1 = ippGetCpuClocks();
    m_clk_diff += c1 - c0;
#endif

    if(m_scan_ncomps == m_jpeg_ncomp)
    {
#ifdef __TIMING__
      c0 = ippGetCpuClocks();
#endif
      jerr = ColorConvert(i,0);
      if(JPEG_OK != jerr)
        return jerr;
#ifdef __TIMING__
      c1 = ippGetCpuClocks();
      m_clk_cc += c1 - c0;
#endif
    }
  } // for m_numyMCU

  return JPEG_OK;
} // CJPEGDecoder::DecodeScanLosslessIN()


JERRCODE CJPEGDecoder::DecodeScanLosslessNI(void)
{
  int       i, j, n, v, h;
  Ipp8u*    src;
  int       srcLen;
  int       currPos;
  Ipp16s*   ptr;
  Ipp16s*   pMCUBuf;
  JERRCODE  jerr;
  IppStatus status;
#ifdef __TIMING__
  Ipp64u    c0;
  Ipp64u    c1;
#endif

  status = ippiDecodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  m_marker = JM_NONE;

  m_scan_count++;
  m_ac_scans_completed += m_scan_ncomps;

  pMCUBuf = m_block_buffer + m_curr_comp_no*m_numxMCU*m_numyMCU;

  src    = m_BitStreamIn.GetDataPtr();
  srcLen = m_BitStreamIn.GetDataLen();

  for(i = 0; i < m_numyMCU; i++)
  {
    for(j = 0; j < m_numxMCU; j++)
    {
      if(m_jpeg_restart_interval)
      {
        if(m_restarts_to_go == 0)
        {
          jerr = ProcessRestart();
          if(JPEG_OK != jerr)
          {
            LOG0("Error: ProcessRestart() failed!");
            return jerr;
          }
        }
      }

      for(n = 0; n < m_scan_ncomps; n++)
      {
        CJPEGColorComponent*   curr_comp = &m_ccomp[m_curr_comp_no];
        IppiDecodeHuffmanSpec* dctbl = m_dctbl[curr_comp->m_dc_selector];

        ptr = pMCUBuf + j*m_mcuWidth;

        for(v = 0; v < curr_comp->m_vsampling; v++)
        {
          for(h = 0; h < curr_comp->m_hsampling; h++)
          {
            m_BitStreamIn.FillBuffer(SAFE_NBYTES);

            currPos = m_BitStreamIn.GetCurrPos();

#ifdef __TIMING__
            c0 = ippGetCpuClocks();
#endif
            status = ippiDecodeHuffmanOne_JPEG_1u16s_C1(
              src,srcLen,&currPos,ptr,(int*)&m_marker,
              dctbl,m_state);
#ifdef __TIMING__
            c1 = ippGetCpuClocks();
            m_clk_huff += c1 - c0;
#endif

            m_BitStreamIn.SetCurrPos(currPos);

            if(ippStsNoErr > status)
            {
              LOG0("Error: ippiDecodeHuffmanOne_JPEG_1u16s_C1() failed!");
              return JPEG_ERR_INTERNAL;
            }

            ptr++;
          } // for m_hsampling
        } // for m_vsampling
      } // for m_jpeg_ncomp

      m_restarts_to_go --;
    } // for m_numxMCU

    pMCUBuf += m_numxMCU;
  } // for m_numyMCU

  return JPEG_OK;
} // CJPEGDecoder::DecodeScanLosslessNI()


JMODE CJPEGDecoder::Mode(void)
{
  return m_jpeg_mode;
} // CJPEGDecoder::Mode()


void CJPEGDecoder::SetInColor(JCOLOR color)
{
  m_jpeg_color = color;
  return;
} // CJPEGDecoder::SetInColor()


void CJPEGDecoder::SetDCTType(int type)
{
  if(m_jpeg_mode == JPEG_BASELINE)
    m_use_qdct = type;
  else
    m_use_qdct = 0;

  return;
} // CJPEGDecoder::SetDCTType


void CJPEGDecoder::Comment(Ipp8u** buf,int* size)
{
  if(0 == buf || 0 == size)
    return;

  *buf  = m_jpeg_comment;
  *size = m_jpeg_comment_size;
  return;
} // CJPEGDecoder::Comment()


int CJPEGDecoder::IsExifAPP1Detected(void)
{
  return m_exif_app1_detected;
} // CJPEGDecoder::IsExifAPP1Detected()


Ipp8u* CJPEGDecoder::GetExifAPP1Data(void)
{
  return m_exif_app1_data;
} // CJPEGDecoder::GetExifAPP1Data()


int CJPEGDecoder::GetExifAPP1DataSize(void)
{
  return m_exif_app1_data_size;
} // CJPEGDecoder::GetExifAPP1DataSize()


JERRCODE CJPEGDecoder::ReadHeader(
  int*    width,
  int*    height,
  int*    nchannels,
  JCOLOR* color,
  JSS*    sampling,
  int*    precision)
{
  int      du_width;
  int      du_height;
  JERRCODE jerr;

  // parse bitstream up to SOS marker
  jerr = ParseJPEGBitStream(JO_READ_HEADER);

  if(JPEG_OK != jerr)
  {
    LOG0("Error: ParseJPEGBitStream() failed");
    return jerr;
  }

  if(JPEG_UNKNOWN == m_jpeg_mode)
    return JPEG_ERR_BAD_DATA;

  // DU block dimensions (8x8 for DCT based modes and 1x1 for lossless mode)
  du_width  = (JPEG_LOSSLESS == m_jpeg_mode) ? 1 : 8;
  du_height = (JPEG_LOSSLESS == m_jpeg_mode) ? 1 : 8;

  // MCU dimensions
  m_mcuWidth  = du_width  * IPP_MAX(m_max_hsampling,1);
  m_mcuHeight = du_height * IPP_MAX(m_max_vsampling,1);

  // num of MCUs in whole image
  m_numxMCU = (m_jpeg_width  + (m_mcuWidth  - 1)) / m_mcuWidth;
  m_numyMCU = (m_jpeg_height + (m_mcuHeight - 1)) / m_mcuHeight;

  // not completed MCUs should be padded
  m_xPadding = m_numxMCU * m_mcuWidth  - m_jpeg_width;
  m_yPadding = m_numyMCU * m_mcuHeight - m_jpeg_height;

  // dimensions of internal buffer for color conversion
  m_ccWidth  = m_mcuWidth * m_numxMCU;
  m_ccHeight = m_mcuHeight;

  if(m_jpeg_restart_interval != 0 && (m_jpeg_restart_interval % m_numxMCU == 0))
  {
    m_rsti_height = m_jpeg_restart_interval / m_numxMCU;
    m_num_rsti = (m_numyMCU + (m_rsti_height - 1)) / m_rsti_height;
  }

  *width     = m_jpeg_width;
  *height    = m_jpeg_height;
  *nchannels = m_jpeg_ncomp;
  *color     = m_jpeg_color;
  *sampling  = m_jpeg_sampling;
  *precision = m_jpeg_precision;

  return JPEG_OK;
} // CJPEGDecoder::ReadHeader()


JERRCODE CJPEGDecoder::ReadData(void)
{
  return ParseJPEGBitStream(JO_READ_DATA);
} // CJPEGDecoder::ReadData()


JERRCODE CJPEGDecoder::InitQuantTable(Ipp8u  qnt[64], int tbl_id)
{
  if(MAX_QUANT_TABLES < tbl_id)
    return JPEG_ERR_PARAMS;

  return m_qntbl[tbl_id].Init(tbl_id,qnt);
}  //CJPEGDecoder::InitQuantTable()


JERRCODE CJPEGDecoder::InitQuantTable(Ipp16u  qnt[64], int tbl_id)
{
  if(MAX_QUANT_TABLES < tbl_id)
    return JPEG_ERR_PARAMS;

  return m_qntbl[tbl_id].Init(tbl_id,qnt);
}  //CJPEGDecoder::InitQuantTable()


JERRCODE CJPEGDecoder::AttachQuantTable(int tbl_id, int comp_no)
{
    if(MAX_QUANT_TABLES < tbl_id)
    return JPEG_ERR_PARAMS;

  if(comp_no > m_jpeg_ncomp)
    return JPEG_ERR_PARAMS;

  m_ccomp[comp_no].m_q_selector = tbl_id;

  return JPEG_OK;
} // CJPEGDecoder::AttachQuantTable()


JERRCODE CJPEGDecoder::InitHuffmanTable(Ipp8u bits[], Ipp8u vals[], int tbl_id, HTBL_CLASS tbl_class)
{
  JERRCODE jerr;

  if(MAX_HUFF_TABLES < tbl_id)
    return JPEG_ERR_PARAMS;

  switch(tbl_class)
  {
  case DC:
    {
      if(m_dctbl[tbl_id].IsEmpty())
      {
        jerr = m_dctbl[tbl_id].Create();
        if(JPEG_OK != jerr)
          return jerr;
      }

      jerr = m_dctbl[tbl_id].Init(tbl_id,tbl_class,bits,vals);
    }
    break;

  case AC:
    {
      if(m_actbl[tbl_id].IsEmpty())
      {
        jerr = m_actbl[tbl_id].Create();
        if(JPEG_OK != jerr)
          return jerr;
      }

      jerr = m_actbl[tbl_id].Init(tbl_id,tbl_class,bits,vals);
    }
    break;

  default:
    return JPEG_ERR_PARAMS;
  }

  return jerr;

} // CJPEGDecoder::InitHuffmanTable()


JERRCODE CJPEGDecoder::AttachHuffmanTable(int tbl_id, HTBL_CLASS tbl_class, int comp_no)
{
  if(MAX_HUFF_TABLES < tbl_id)
    return JPEG_ERR_PARAMS;

  if(comp_no > m_jpeg_ncomp)
    return JPEG_ERR_PARAMS;

  switch(tbl_class)
  {
  case DC:
    m_ccomp[comp_no].m_dc_selector = tbl_id;
    break;

  case AC:
    m_ccomp[comp_no].m_ac_selector = tbl_id;
    break;

  default:
    return JPEG_ERR_PARAMS;
  }

  return JPEG_OK;
} // CJPEGDecoder::AttachHuffmanTable()


int CJPEGDecoder::IsJFIFApp0Detected(void)
{
  return m_jfif_app0_detected;
} // CJPEGDecoder::IsJFIFApp0Detected()


int CJPEGDecoder::IsJFXXApp0Detected(void)
{
  return m_jfxx_app0_detected;
} // CJPEGDecoder::IsJFXXApp0Detected()


int CJPEGDecoder::IsAVI1App0Detected(void)
{
  return m_avi1_app0_detected;
} // CJPEGDecoder::IsAVI1App0Detected()


void CJPEGDecoder::GetJFIFApp0Resolution(JRESUNITS* units, int* xdensity, int* ydensity)
{
  *units    = m_jfif_app0_units;
  *xdensity = m_jfif_app0_xDensity;
  *ydensity = m_jfif_app0_yDensity;

  return;
} // CJPEGDecoder::GetJFIFApp0Data()


JERRCODE CJPEGDecoder::SetThreadingMode(JTMODE tmode)
{
  switch(tmode)
  {
  case JT_RSTI:
    {
      if(m_jpeg_restart_interval             != 0
        && m_jpeg_restart_interval%m_numxMCU == 0
        && m_jpeg_mode                       == JPEG_BASELINE
        && m_jpeg_sampling                   != JS_411)
      {
        m_threading_mode = JT_RSTI;
      }
      else
        m_threading_mode = JT_OLD;
    }
    break;

  case JT_OLD:
  default:
    m_threading_mode = JT_OLD;
  }

  return JPEG_OK;
} // CJPEGDecoder::SetThreadingMode()
