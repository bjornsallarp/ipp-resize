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
#pragma warning ( disable : 4996 )
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


CJPEGEncoder::CJPEGEncoder(void)
{
  m_src.p.Data8u[0] = 0;
  m_src.p.Data8u[1] = 0;
  m_src.p.Data8u[2] = 0;
  m_src.p.Data8u[3] = 0;
  m_src.width       = 0;
  m_src.height      = 0;
  m_src.lineStep[0] = 0;
  m_src.lineStep[1] = 0;
  m_src.lineStep[2] = 0;
  m_src.lineStep[3] = 0;
  m_src.precision   = 0;
  m_src.nChannels   = 0;
  m_src.color       = JC_UNKNOWN;
  m_src.sampling    = JS_OTHER;

  m_jpeg_ncomp            = 0;
  m_jpeg_precision        = 8;
  m_jpeg_sampling         = JS_444;
  m_jpeg_color            = JC_UNKNOWN;
  m_jpeg_quality          = 100;
  m_jpeg_restart_interval = 0;
  m_jpeg_mode             = JPEG_BASELINE;
  m_jpeg_comment          = 0;

  m_numxMCU   = 0;
  m_numyMCU   = 0;
  m_mcuWidth  = 0;
  m_mcuHeight = 0;

  m_ccWidth  = 0;
  m_ccHeight = 0;
  m_xPadding = 0;
  m_yPadding = 0;

  m_rst_go           = 0;
  m_restarts_to_go   = 0;
  m_next_restart_num = 0;

  m_scan_count   = 0;
  m_scan_script  = 0;
  m_block_buffer = 0;

  m_ss = 0;
  m_se = 63;
  m_ah = 0;
  m_al = 0;

  m_predictor = 1;
  m_pt = 0;

  // 1 - to set generated table, 0 for default or external tables
  m_optimal_htbl = 0;

  m_jfif_app0_units    = JRU_NONE;
  m_jfif_app0_xDensity = 1;
  m_jfif_app0_yDensity = 1;

  m_num_rsti     = 0;
  m_rstiHeight   = 0;

  m_threading_mode = JT_OLD;

#ifdef __TIMING__
  m_clk_dct  = 0;
  m_clk_ss   = 0;
  m_clk_cc   = 0;
  m_clk_diff = 0;
  m_clk_huff = 0;
#endif

  return;
} // ctor


CJPEGEncoder::~CJPEGEncoder(void)
{
  Clean();
  return;
} // dtor


JERRCODE CJPEGEncoder::Clean(void)
{
  int i;

  for(i = 0; i < MAX_HUFF_TABLES; i++)
  {
    m_dctbl[i].Destroy();
    m_actbl[i].Destroy();
  }

  if(0 != m_scan_script)
  {
    delete[] m_scan_script;
    m_scan_script = 0;
  }

  if(0 != m_block_buffer)
  {
    ippFree(m_block_buffer);
    m_block_buffer = 0;
  }

  for(i =0; i < m_jpeg_ncomp; i++)
  {
    m_ccomp[i].DeleteBufferCC();
    m_ccomp[i].DeleteBufferSS();
    m_ccomp[i].DeleteBufferLNZ();
  }

  m_src.p.Data8u[0] = 0;
  m_src.p.Data8u[1] = 0;
  m_src.p.Data8u[2] = 0;
  m_src.p.Data8u[3] = 0;

  m_src.width       = 0;
  m_src.height      = 0;
  m_src.lineStep[0] = 0;
  m_src.lineStep[1] = 0;
  m_src.lineStep[2] = 0;
  m_src.lineStep[3] = 0;
  m_src.precision   = 0;
  m_src.nChannels   = 0;
  m_src.color       = JC_UNKNOWN;
  m_src.sampling    = JS_OTHER;

  m_jpeg_ncomp            = 0;
  m_jpeg_sampling         = JS_444;
  m_jpeg_color            = JC_UNKNOWN;
  m_jpeg_quality          = 100;
  m_jpeg_mode             = JPEG_BASELINE;

  m_numxMCU = 0;
  m_numyMCU = 0;
  m_mcuWidth  = 0;
  m_mcuHeight = 0;

  m_ccWidth  = 0;
  m_ccHeight = 0;
  m_xPadding = 0;
  m_yPadding = 0;

  m_restarts_to_go   = 0;
  m_next_restart_num = 0;

  m_scan_count = 0;

  m_optimal_htbl = 0;

  m_state.Destroy();

#ifdef _OPENMP
  if(m_jpeg_restart_interval != 0 && JT_RSTI == m_threading_mode)
  {
    for(i = 0; i < m_num_threads; i++)
    {
      m_state_t[i].Destroy();
      m_BitStreamOutT[i].Detach();

      if(m_lastDC[i])
        delete m_lastDC[i];
    }

    delete[] m_lastDC;
    delete[] m_state_t;
    delete[] m_BitStreamOutT;
  }
#endif

  m_jpeg_restart_interval = 0;

  return JPEG_OK;
} // CJPEGEncoder::Clean()


JERRCODE CJPEGEncoder::SetSource(
  Ipp8u*   pSrc,
  int      srcStep,
  IppiSize srcSize,
  int      srcChannels,
  JCOLOR   srcColor,
  JSS      srcSampling,
  int      srcPrecision)
{
  m_src.p.Data8u[0] = pSrc;
  m_src.lineStep[0] = srcStep;
  m_src.width       = srcSize.width;
  m_src.height      = srcSize.height;
  m_src.nChannels   = srcChannels;
  m_src.color       = srcColor;
  m_src.sampling    = srcSampling;
  m_src.precision   = srcPrecision;
  m_src.order        = JD_PIXEL;

  return JPEG_OK;
} // CJPEGEncoder::SetSource()


JERRCODE CJPEGEncoder::SetSource(
  Ipp16s*  pSrc,
  int      srcStep,
  IppiSize srcSize,
  int      srcChannels,
  JCOLOR   srcColor,
  JSS      srcSampling,
  int      srcPrecision)
{
  m_src.p.Data16s[0] = pSrc;
  m_src.lineStep[0]  = srcStep;
  m_src.width        = srcSize.width;
  m_src.height       = srcSize.height;
  m_src.nChannels    = srcChannels;
  m_src.color        = srcColor;
  m_src.sampling     = srcSampling;
  m_src.precision    = srcPrecision;
  m_src.order        = JD_PIXEL;

  if(m_src.precision > 10)
    m_optimal_htbl = 1;

  return JPEG_OK;
} // CJPEGEncoder::SetSource()


JERRCODE CJPEGEncoder::SetSource(
  Ipp8u*   pSrc[],
  int      srcStep[],
  IppiSize srcSize,
  int      srcChannels,
  JCOLOR   srcColor,
  JSS      srcSampling ,
  int      srcPrecision)
{

  if(srcChannels == 4 && m_jpeg_color != srcColor)
    return JPEG_ERR_PARAMS;

  m_src.p.Data8u[0] = pSrc[0];
  m_src.p.Data8u[1] = pSrc[1];
  m_src.p.Data8u[2] = pSrc[2];
  m_src.p.Data8u[3] = pSrc[3];

  m_src.lineStep[0] = srcStep[0];
  m_src.lineStep[1] = srcStep[1];
  m_src.lineStep[2] = srcStep[2];
  m_src.lineStep[3] = srcStep[3];

  m_src.order     = JD_PLANE;
  m_src.width     = srcSize.width;
  m_src.height    = srcSize.height;
  m_src.nChannels = srcChannels;
  m_src.color     = srcColor;
  m_src.sampling  = srcSampling;
  m_src.precision = srcPrecision;

  return JPEG_OK;
} // CJPEGEncoder::SetSource()


JERRCODE CJPEGEncoder::SetSource(
  Ipp16s*  pSrc[],
  int      srcStep[],
  IppiSize srcSize,
  int      srcChannels,
  JCOLOR   srcColor,
  JSS      srcSampling,
  int      srcPrecision)
{
  m_src.p.Data16s[0] = pSrc[0];
  m_src.p.Data16s[1] = pSrc[1];
  m_src.p.Data16s[2] = pSrc[2];
  m_src.p.Data16s[3] = pSrc[3];

  m_src.lineStep[0] = srcStep[0];
  m_src.lineStep[1] = srcStep[1];
  m_src.lineStep[2] = srcStep[2];
  m_src.lineStep[3] = srcStep[3];

  m_src.order     = JD_PLANE;
  m_src.width     = srcSize.width;
  m_src.height    = srcSize.height;
  m_src.nChannels = srcChannels;
  m_src.color     = srcColor;
  m_src.sampling  = srcSampling;
  m_src.precision = srcPrecision;

  if(m_src.precision > 10)
    m_optimal_htbl = 1;

  return JPEG_OK;
} // CJPEGEncoder::SetSource()


JERRCODE CJPEGEncoder::SetDestination(
  UIC::BaseStreamOutput* pStreamOut)
{
  return m_BitStreamOut.Attach(pStreamOut);
} // CJPEGEncoder::SetDestination()


JERRCODE CJPEGEncoder::SetParams(
  JMODE  mode,
  JCOLOR color,
  JSS    sampling,
  int    restart,
  int    huff_opt,
  int    quality,
  JTMODE threading_mode)
{
  int      i;
  JERRCODE jerr;

  if(JD_PLANE == m_src.order)
  {
    if(m_src.precision <= 8)
    {
      if(m_src.nChannels == 3 && color != m_src.color)
        return JPEG_ERR_PARAMS;

      if(m_src.nChannels == 4 && color != m_src.color)
        return JPEG_ERR_PARAMS;
    }
    else
    {
      if(m_src.nChannels == 3 && color != m_src.color && m_src.sampling != 444)
        return JPEG_ERR_PARAMS;

      if(m_src.nChannels == 4 && color != m_src.color && m_src.sampling != 444)
        return JPEG_ERR_PARAMS;
    }
  }

  // common params
  m_jpeg_mode             = mode;
  m_jpeg_color            = color;
  m_jpeg_sampling         = sampling;
  m_jpeg_restart_interval = restart;

  // baseline/progressive specific params
  m_jpeg_quality          = quality;
  m_optimal_htbl          = huff_opt;
  m_restarts_to_go        = m_jpeg_restart_interval;
  m_jpeg_precision        = m_src.precision;

  m_mcuWidth  = (m_jpeg_sampling == JS_444) ?  8 : 16;
  m_mcuHeight = (m_jpeg_sampling == JS_411) ? 16 :  8;

  m_numxMCU = (m_src.width  + (m_mcuWidth  - 1)) / m_mcuWidth;
  m_numyMCU = (m_src.height + (m_mcuHeight - 1)) / m_mcuHeight;

  m_threading_mode = threading_mode;
#ifdef _OPENMP
  if(m_jpeg_restart_interval != 0 && JT_RSTI == threading_mode && mode == JPEG_BASELINE)
  {
    m_rstiHeight = m_jpeg_restart_interval;
    m_num_rsti   = (m_numyMCU + (m_rstiHeight - 1)) / m_rstiHeight;
  }
  else
#endif
  {
    m_num_rsti   = 0;
    m_rstiHeight = 1;

    m_threading_mode = JT_OLD;
  }

  switch(m_jpeg_color)
  {
  case JC_GRAY:  m_jpeg_ncomp = 1; break;
  case JC_RGB:   m_jpeg_ncomp = 3; break;
  case JC_YCBCR: m_jpeg_ncomp = 3; break;
  case JC_CMYK:  m_jpeg_ncomp = 4; break;
  case JC_YCCK:  m_jpeg_ncomp = 4; break;
  default:       m_jpeg_ncomp = IPP_MIN(MAX_COMPS_PER_SCAN,m_src.nChannels); break;
  }

  switch(mode)
  {
  case JPEG_BASELINE:
  case JPEG_PROGRESSIVE:
    {
      jerr = InitQuantTable((Ipp8u*)DefaultLuminanceQuant, 0, m_jpeg_quality);
      if(JPEG_OK != jerr)
      {
        LOG0("Error: can't attach quant table");
        return jerr;
      }

      if(m_jpeg_ncomp > 1 && (m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK))
      {
        jerr = InitQuantTable((Ipp8u*)DefaultChrominanceQuant, 1, m_jpeg_quality);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: can't attach quant table");
          return jerr;
        }
      }

      jerr = m_dctbl[0].Create();
      if(JPEG_OK != jerr)
        return jerr;

      jerr = m_actbl[0].Create();
      if(JPEG_OK != jerr)
        return jerr;

      if(JPEG_BASELINE == mode)
      {
        if(!m_optimal_htbl)
        {
          jerr = InitHuffmanTable((Ipp8u*)DefaultLuminanceDCBits, (Ipp8u*)DefaultLuminanceDCValues, 0, DC);
          if(JPEG_OK != jerr)
            return jerr;

          jerr = InitHuffmanTable((Ipp8u*)DefaultLuminanceACBits, (Ipp8u*)DefaultLuminanceACValues, 0, AC);
          if(JPEG_OK != jerr)
            return jerr;
        }
      }

      if(m_jpeg_ncomp > 1 && (m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK))
      {
        jerr = m_dctbl[1].Create();
        if(JPEG_OK != jerr)
          return jerr;

        jerr = m_actbl[1].Create();
        if(JPEG_OK != jerr)
          return jerr;

        if(JPEG_BASELINE == mode)
        {
          if(!m_optimal_htbl)
          {
            jerr = InitHuffmanTable((Ipp8u*)DefaultChrominanceDCBits, (Ipp8u*)DefaultChrominanceDCValues, 1, DC);
            if(JPEG_OK != jerr)
              return jerr;

            jerr = InitHuffmanTable((Ipp8u*)DefaultChrominanceACBits, (Ipp8u*)DefaultChrominanceACValues, 1, AC);
            if(JPEG_OK != jerr)
              return jerr;
          }
        }
      }
    }
    break;

  case JPEG_EXTENDED:
    {
      if(m_jpeg_precision > 8)
      {
        Ipp16u buf[64];

        for(i = 0; i < DCTSIZE2; i++)
          buf[i] = (Ipp16u)DefaultLuminanceQuant[i];

        jerr = InitQuantTable(buf, 0, quality);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: can't attach quant table");
          return jerr;
        }

        if(m_jpeg_ncomp > 1 && (m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK))
        {
          for(i = 0; i < DCTSIZE2; i++)
            buf[i] = (Ipp16u)DefaultChrominanceQuant[i];

          jerr = InitQuantTable(buf, 1, quality);
          if(JPEG_OK != jerr)
          {
            LOG0("Error: can't attach quant table");
            return jerr;
          }
        }
      }
      else // m_jpeg_precision == 8
      {
        jerr = InitQuantTable((Ipp8u*)DefaultLuminanceQuant, 0, m_jpeg_quality);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: can't attach quant table");
          return jerr;
        }

        if(m_jpeg_ncomp > 1 && (m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK))
        {
          jerr = InitQuantTable((Ipp8u*)DefaultChrominanceQuant, 1, m_jpeg_quality);
          if(JPEG_OK != jerr)
          {
            LOG0("Error: can't attach quant table");
            return jerr;
          }
        }
      }

      for(i = 0; i < m_jpeg_ncomp; i++)
      {
        jerr = m_dctbl[i].Create();
        if(JPEG_OK != jerr)
          return jerr;

        jerr = m_actbl[i].Create();
        if(JPEG_OK != jerr)
          return jerr;
      }
    }
    break;

  default:
    return JPEG_ERR_PARAMS;
  } // case mode

  int id[4] = { 0, 1, 1, 0 };

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    if(m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK)
      jerr = AttachQuantTable(id[i], i);
    else
      jerr = AttachQuantTable(id[0], i);

    if(JPEG_OK != jerr)
      return jerr;

    if(m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK)
    {
      jerr = AttachHuffmanTable(id[i], DC, i);
      if(JPEG_OK != jerr)
        return jerr;

      jerr = AttachHuffmanTable(id[i], AC, i);
      if(JPEG_OK != jerr)
        return jerr;
    }
    else
    {
      jerr = AttachHuffmanTable(id[0], DC, i);
      if(JPEG_OK != jerr)
        return jerr;

      jerr = AttachHuffmanTable(id[0], AC, i);
      if(JPEG_OK != jerr)
        return jerr;
    }
  }

  return JPEG_OK;
} // CJPEGEncoder::SetParams()


JERRCODE CJPEGEncoder::SetParams(
  JMODE  mode,
  JCOLOR color,
  JSS    sampling,
  int    restart,
  int    huff_opt,
  int    point_transform,
  int    predictor)
{
  int      c, i;
  JERRCODE jerr;

  if(JPEG_LOSSLESS != mode)
    return JPEG_ERR_PARAMS;

  if(JS_444 != sampling)
    return JPEG_NOT_IMPLEMENTED;

  m_jpeg_mode             = mode;
  m_jpeg_color            = color;
  m_jpeg_sampling         = sampling;
  m_jpeg_restart_interval = restart;
  // lossless specific params
  m_pt                    = point_transform;
  m_predictor             = predictor;

  m_optimal_htbl          = m_src.precision > 11 ? 1 : huff_opt;

  m_jpeg_precision = m_src.precision;

  m_restarts_to_go = m_jpeg_restart_interval;

  m_mcuWidth  = 1;
  m_mcuHeight = 1;

  m_numxMCU = (m_src.width  + (m_mcuWidth  - 1)) / m_mcuWidth;
  m_numyMCU = (m_src.height + (m_mcuHeight - 1)) / m_mcuHeight;

  switch(m_jpeg_color)
  {
  case JC_GRAY:  m_jpeg_ncomp = 1; break;
  case JC_RGB:   m_jpeg_ncomp = 3; break;
  case JC_YCBCR: m_jpeg_ncomp = 3; break;
  case JC_CMYK:  m_jpeg_ncomp = 4; break;
  case JC_YCCK:  m_jpeg_ncomp = 4; break;
  default:       m_jpeg_ncomp = IPP_MIN(MAX_COMPS_PER_SCAN,m_src.nChannels); break;
  }

  if(!m_optimal_htbl)
  {
    jerr = m_dctbl[0].Create();
    if(JPEG_OK != jerr)
      return jerr;

    jerr = InitHuffmanTable((Ipp8u*)DefaultLuminanceDCBits, (Ipp8u*)DefaultLuminanceDCValues, 0, DC);
    if(JPEG_OK != jerr)
      return jerr;

    // lossless mode defaults is one huffman table for all JPEG color components
    for(i = 0; i < m_jpeg_ncomp; i++)
    {
      jerr = AttachHuffmanTable(0, DC, i);
      if(JPEG_OK != jerr)
        return jerr;
    }
  }
  else
  {
    for(c = 0; c < m_jpeg_ncomp; c++)
    {
      jerr = m_dctbl[c].Create();
      if(JPEG_OK != jerr)
        return jerr;

      // optimal huff table for each JPEG color component
      jerr = AttachHuffmanTable(c, DC, c);
      if(JPEG_OK != jerr)
        return jerr;
    }
  }
  return JPEG_OK;
} // CJPEGEncoder::SetParams()


JERRCODE CJPEGEncoder::InitHuffmanTable(
  Ipp8u      bits[16],
  Ipp8u      vals[256],
  int        tbl_id,
  HTBL_CLASS tbl_class)
{
  JERRCODE jerr;

  if(MAX_HUFF_TABLES < tbl_id)
    return JPEG_ERR_PARAMS;

  switch(tbl_class)
  {
  case DC:
    jerr = m_dctbl[tbl_id].Init(tbl_id,tbl_class,bits,vals);
    break;

  case AC:
    jerr = m_actbl[tbl_id].Init(tbl_id,tbl_class,bits,vals);
    break;

  default:
    return JPEG_ERR_PARAMS;
  }

  return jerr;
} // CJPEGEncoder::InitHuffmanTable()


JERRCODE CJPEGEncoder::InitQuantTable(
  Ipp8u qnt[64],
  int   tbl_id,
  int   quality)
{
  if(MAX_QUANT_TABLES < tbl_id)
    return JPEG_ERR_PARAMS;

  return m_qntbl[tbl_id].Init(tbl_id,qnt,quality);
} // CJPEGEncoder::InitQuantTable()


JERRCODE CJPEGEncoder::InitQuantTable(
  Ipp16u qnt[64],
  int    tbl_id,
  int    quality)
{
  if(MAX_QUANT_TABLES < tbl_id)
    return JPEG_ERR_PARAMS;

  return m_qntbl[tbl_id].Init(tbl_id,qnt,quality);
} // CJPEGEncoder::InitQuantTable()


JERRCODE CJPEGEncoder::AttachHuffmanTable(int tbl_id, HTBL_CLASS tbl_class, int comp_no)
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
} // CJPEGEncoder::AttachHuffmanTable()


JERRCODE CJPEGEncoder::AttachQuantTable(int tbl_id, int comp_no)
{
  if(MAX_QUANT_TABLES < tbl_id)
    return JPEG_ERR_PARAMS;

  if(comp_no > m_jpeg_ncomp)
    return JPEG_ERR_PARAMS;

  m_ccomp[comp_no].m_q_selector = tbl_id;

  return JPEG_OK;
} // CJPEGEncoder::AttachQuantTable()


JERRCODE CJPEGEncoder::WriteSOI(void)
{
  JERRCODE jerr;

  TRC0("-> WriteSOI");

  TRC1("  emit marker ",JM_SOI);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_SOI);
  if(JPEG_OK != jerr)
    return jerr;

  return JPEG_OK;
} // CJPEGEncoder::WriteSOI()


JERRCODE CJPEGEncoder::WriteEOI(void)
{
  JERRCODE jerr;

  TRC0("-> WriteEOI");

  TRC1("emit marker ",JM_EOI);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_EOI);
  if(JPEG_OK != jerr)
    return jerr;

  return JPEG_OK;
} // CJPEGEncoder::WriteEOI()


JERRCODE CJPEGEncoder::WriteAPP0(void)
{
  int len;
  JERRCODE jerr;

  TRC0("-> WriteAPP0");

  len = 2 + 5 + 2 + 1 + 2 + 2 + 1 + 1;

  TRC1("  emit marker ",JM_APP0);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_APP0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  // identificator JFIF
  jerr = m_BitStreamOut.WriteByte('J');
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte('F');
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte('I');
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte('F');
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(0);
  if(JPEG_OK != jerr)
    return jerr;

  // version
  jerr = m_BitStreamOut.WriteWord(0x0102);
  if(JPEG_OK != jerr)
    return jerr;

  // units: 0 - none, 1 - dot per inch, 2 - dot per cm
  jerr = m_BitStreamOut.WriteByte(m_jfif_app0_units);
  if(JPEG_OK != jerr)
    return jerr;

  // xDensity
  jerr = m_BitStreamOut.WriteWord(m_jfif_app0_xDensity);
  if(JPEG_OK != jerr)
    return jerr;

  // yDensity
  jerr = m_BitStreamOut.WriteWord(m_jfif_app0_yDensity);
  if(JPEG_OK != jerr)
    return jerr;

  // xThumbnails, yThumbnails
  jerr = m_BitStreamOut.WriteByte(0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(0);
  if(JPEG_OK != jerr)
    return jerr;

  return JPEG_OK;
} // CJPEGEncoder::WriteAPP0()


JERRCODE CJPEGEncoder::WriteAPP14(void)
{
  int len;
  JERRCODE jerr;

  TRC0("-> WriteAPP14");

  len = 2 + 5 + 2 + 2 + 2 + 1;

  TRC1("  emit marker ",JM_APP14);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_APP14);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  // identificator Adobe
  jerr = m_BitStreamOut.WriteByte('A');
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte('d');
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte('o');
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte('b');
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte('e');
  if(JPEG_OK != jerr)
    return jerr;

  // version
  jerr = m_BitStreamOut.WriteWord(100);
  if(JPEG_OK != jerr)
    return jerr;

  // Flags 0
  jerr = m_BitStreamOut.WriteWord(0);
  if(JPEG_OK != jerr)
    return jerr;

  // Flags 1
  jerr = m_BitStreamOut.WriteWord(0);
  if(JPEG_OK != jerr)
    return jerr;

  switch(m_jpeg_color)
  {
  case JC_YCBCR:
    jerr = m_BitStreamOut.WriteByte(1);
    break;

  case JC_YCCK:
    jerr = m_BitStreamOut.WriteByte(2);
    break;

  default:
    jerr = m_BitStreamOut.WriteByte(0);
    break;
  }

  if(JPEG_OK != jerr)
    return jerr;

  return JPEG_OK;
} // CJPEGEncoder::WriteAPP14()


JERRCODE CJPEGEncoder::WriteCOM(
  char*  comment)
{
  int   i;
  int   len;
  char* ptr;
  char  buf[256];
  JERRCODE jerr;

  TRC0("-> WriteCOM");

  {
    buf[0] = 0;
    ptr = &buf[0];
    const IppLibraryVersion* jv = ippjGetLibVersion();

    sprintf(ptr,"Intel(R) IPP JPEG encoder [%d.%d.%d] - %s",
      jv->major,jv->minor,jv->build,jv->BuildDate);

    len = (int)strlen(ptr) + 1;
  }

  if(comment != 0)
  {
    int sz;
    ptr[len-1] = ';';
    ptr[len  ] = ' ';

    sz = (int)IPP_MIN(strlen(comment),127);

    for(int i = 0; i < sz; i++)
      ptr[len + i] = comment[i];

    len += sz + 1;
    ptr[len-1] = 0;
  }

  len += 2;

  TRC1("  emit marker ",JM_COM);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_COM);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  for(i = 0; i < len - 2; i++)
  {
    jerr = m_BitStreamOut.WriteByte(ptr[i]);
    if(JPEG_OK != jerr)
      return jerr;
  }

  return JPEG_OK;
} // CJPEGEncoder::WriteCOM()


JERRCODE CJPEGEncoder::WriteDQT(
  CJPEGEncoderQuantTable* qtbl)
{
  int i;
  int len;
  JERRCODE jerr;

  TRC0("-> WriteDQT");

  len = DCTSIZE2 + 2 + 1;

  if(qtbl->m_precision)
    len += DCTSIZE2;

  TRC1("  emit marker ",JM_DQT);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_DQT);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  // precision/id
  jerr = m_BitStreamOut.WriteByte((qtbl->m_precision << 4) | qtbl->m_id);
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  id ",qtbl->m_id);
  TRC1("  precision ",qtbl->m_precision);

  TRC(endl);
  for(i = 0; i < DCTSIZE2; i++)
  {
    TRC(" ");
    TRC((int)qtbl->m_raw[i]);
    if(i % 8 == 7)
    {
      TRC(endl);
    }
  }
  TRC(endl);

  for(i = 0; i < DCTSIZE2; i++)
  {
    if(qtbl->m_precision == 0)
    {
      jerr = m_BitStreamOut.WriteByte(qtbl->m_raw8u[i]);
    }
    else
    {
      jerr = m_BitStreamOut.WriteWord(qtbl->m_raw16u[i]);
    }
    if(JPEG_OK != jerr)
      return jerr;
  }

  return JPEG_OK;
} // CJPEGEncoder::WriteDQT()


JERRCODE CJPEGEncoder::WriteDHT(
  CJPEGEncoderHuffmanTable* htbl)
{
  int i;
  int len;
  int htbl_len;
  JERRCODE jerr;

  TRC0("-> WriteDHT");

  for(htbl_len = 0, i = 0; i < 16; i++)
  {
    htbl_len += htbl->m_bits[i];
  }

  len = 16 + htbl_len + 2 + 1;

  TRC1("  emit marker ",JM_DHT);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_DHT);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte((htbl->m_hclass << 4) | htbl->m_id);
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  id ",htbl->m_id);
  TRC1("  class ",htbl->m_hclass);

  for(i = 0; i < 16; i++)
  {
    jerr = m_BitStreamOut.WriteByte(htbl->m_bits[i]);
    if(JPEG_OK != jerr)
      return jerr;
  }

  for(i = 0; i < htbl_len; i++)
  {
    jerr = m_BitStreamOut.WriteByte(htbl->m_vals[i]);
    if(JPEG_OK != jerr)
      return jerr;
  }

  return JPEG_OK;
} // CJPEGEncoder::WriteDHT()


JERRCODE CJPEGEncoder::WriteSOF0(void)
{
  int len;
  JERRCODE jerr;

  TRC0("-> WriteSOF0");

  len = 8 + m_jpeg_ncomp * 3;

  TRC1("  emit marker ",JM_SOF0);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_SOF0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  // sample precision
  jerr = m_BitStreamOut.WriteByte(8);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(m_src.height);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(m_src.width);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(m_jpeg_ncomp);
  if(JPEG_OK != jerr)
    return jerr;

  for(int i = 0; i < m_jpeg_ncomp; i++)
  {
    jerr = m_BitStreamOut.WriteByte(i);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamOut.WriteByte((m_ccomp[i].m_hsampling << 4) | m_ccomp[i].m_vsampling);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamOut.WriteByte(m_ccomp[i].m_q_selector);
    if(JPEG_OK != jerr)
      return jerr;
  }

  return JPEG_OK;
} // CJPEGEncoder::WriteSOF0()


JERRCODE CJPEGEncoder::WriteSOF1(void)
{
  int len;
  JERRCODE jerr;

  TRC0("-> WriteSOF1");

  len = 8 + m_jpeg_ncomp * 3;

  TRC1("  emit marker ",JM_SOF1);
  TRC1("    length ",sof1_len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_SOF1);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  // sample precision
  jerr = m_BitStreamOut.WriteByte(m_jpeg_precision);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(m_src.height);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(m_src.width);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(m_jpeg_ncomp);
  if(JPEG_OK != jerr)
    return jerr;

  for(int i = 0; i < m_jpeg_ncomp; i++)
  {
    jerr = m_BitStreamOut.WriteByte(i);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamOut.WriteByte((m_ccomp[i].m_hsampling << 4) | m_ccomp[i].m_vsampling);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamOut.WriteByte(m_ccomp[i].m_q_selector);
    if(JPEG_OK != jerr)
      return jerr;
  }

  return JPEG_OK;
} // CJPEGEncoder::WriteSOF1()


JERRCODE CJPEGEncoder::WriteSOF2(void)
{
  int len;
  JERRCODE jerr;

  TRC0("-> WriteSOF2");

  len = 8 + m_jpeg_ncomp * 3;

  TRC1("  emit marker ",JM_SOF2);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_SOF2);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  // sample precision
  jerr = m_BitStreamOut.WriteByte(8);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(m_src.height);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(m_src.width);
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  height ",m_src.height);
  TRC1("  width  ",m_src.width);

  jerr = m_BitStreamOut.WriteByte(m_jpeg_ncomp);
  if(JPEG_OK != jerr)
    return jerr;

  for(int i = 0; i < m_jpeg_ncomp; i++)
  {
    jerr = m_BitStreamOut.WriteByte(i);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamOut.WriteByte((m_ccomp[i].m_hsampling << 4) | m_ccomp[i].m_vsampling);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamOut.WriteByte(m_ccomp[i].m_q_selector);
    if(JPEG_OK != jerr)
      return jerr;

    TRC1("    id ",i);
    TRC1("      h_sampling ",m_ccomp[i]->m_hsampling);
    TRC1("      v_sampling ",m_ccomp[i]->m_vsampling);
    TRC1("      q_selector ",m_ccomp[i]->m_q_selector);
  }

  return JPEG_OK;
} // CJPEGEncoder::WriteSOF2()


JERRCODE CJPEGEncoder::WriteSOF3(void)
{
  int len;
  JERRCODE jerr;

  TRC0("-> WriteSOF3");

  len = 8 + m_jpeg_ncomp * 3;

  TRC1("  emit marker ",JM_SOF3);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_SOF3);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  // sample precision
  jerr = m_BitStreamOut.WriteByte(m_jpeg_precision);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(m_src.height);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(m_src.width);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(m_jpeg_ncomp);
  if(JPEG_OK != jerr)
    return jerr;

  for(int i = 0; i < m_jpeg_ncomp; i++)
  {
    jerr = m_BitStreamOut.WriteByte(i);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamOut.WriteByte((m_ccomp[i].m_hsampling << 4) | m_ccomp[i].m_vsampling);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamOut.WriteByte(m_ccomp[i].m_q_selector);
    if(JPEG_OK != jerr)
      return jerr;
  }

  return JPEG_OK;
} // CJPEGEncoder::WriteSOF3()


JERRCODE CJPEGEncoder::WriteDRI(
  int    restart_interval)
{
  int len;
  JERRCODE jerr;

  TRC0("-> WriteDRI");

  len = 2 + 2;

  TRC1("  emit marker ",JM_DRI);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_DRI);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  // emit restart interval
  if(JT_OLD == m_threading_mode)
  {
    jerr = m_BitStreamOut.WriteWord(restart_interval);
  }
#ifdef _OPENMP
  else
  {
    jerr = m_BitStreamOut.WriteWord(restart_interval*m_numxMCU);
  }
#endif
  if(JPEG_OK != jerr)
    return jerr;

  m_restarts_to_go = m_jpeg_restart_interval;
  m_next_restart_num = 0;

  TRC1("  restart ",restart_interval);

  return JPEG_OK;
} // CJPEGEncoder::WriteDRI()


JERRCODE CJPEGEncoder::WriteRST(
  int    next_restart_num)
{
  JERRCODE jerr;

  TRC0("-> WriteRST");

  TRC1("  emit marker ",JM_RST0 + next_restart_num);
  TRC1("    RST ",0xfff0 | (JM_RST0 + next_restart_num));

  // emit restart interval
  jerr = m_BitStreamOut.WriteWord(0xff00 | (JM_RST0 + next_restart_num));
  if(JPEG_OK != jerr)
    return jerr;

  // Update next-restart state
  m_next_restart_num = (next_restart_num + 1) & 7;

  return JPEG_OK;
} // CJPEGEncoder::WriteRST()


JERRCODE CJPEGEncoder::ProcessRestart(
  int    id[MAX_COMPS_PER_SCAN],
  int    Ss,
  int    Se,
  int    Ah,
  int    Al)
{
  int       dstLen;
  int       currPos;
  Ipp8u*    dst;
  JERRCODE  jerr;
  IppStatus status = ippStsNoErr;

  TRC0("-> ProcessRestart");

  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();

  jerr = m_BitStreamOut.FlushBuffer();
  if(JPEG_OK != jerr)
    return jerr;

  currPos = m_BitStreamOut.GetCurrPos();

  // flush IppiEncodeHuffmanState
  switch(m_jpeg_mode)
  {
  case JPEG_BASELINE:
  case JPEG_EXTENDED:
    {
      status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
                 0,dst,dstLen,&currPos,0,0,0,m_state,1);
      break;
    }

  case JPEG_PROGRESSIVE:
    if(Ss == 0 && Se == 0)
    {
      // DC scan
      if(Ah == 0)
      {
        status = ippiEncodeHuffman8x8_DCFirst_JPEG_16s1u_C1(
                   0,dst,dstLen,&currPos,0,0,0,m_state,1);
      }
      else
      {
        status = ippiEncodeHuffman8x8_DCRefine_JPEG_16s1u_C1(
                   0,dst,dstLen,&currPos,0,m_state,1);
      }
    }
    else
    {
      // AC scan
      IppiEncodeHuffmanSpec* actbl = m_actbl[m_ccomp[id[0]].m_ac_selector];

      if(Ah == 0)
      {
        status = ippiEncodeHuffman8x8_ACFirst_JPEG_16s1u_C1(
                   0,dst,dstLen,&currPos,Ss,Se,Al,actbl,m_state,1);
      }
      else
      {
        status = ippiEncodeHuffman8x8_ACRefine_JPEG_16s1u_C1(
                   0,dst,dstLen,&currPos,Ss,Se,Al,actbl,m_state,1);
      }
    }
    break;

  case JPEG_LOSSLESS:
    status = ippiEncodeHuffmanOne_JPEG_16s1u_C1(
               0,dst,dstLen,&currPos,0,m_state,1);
    break;
  default:
    break;
  }

  m_BitStreamOut.SetCurrPos(currPos);

  if(ippStsNoErr > status)
  {
    LOG1("IPP Error: ippiEncodeHuffman8x8_JPEG_16s1u_C1() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  jerr = WriteRST(m_next_restart_num);
  if(JPEG_OK != jerr)
  {
    LOG1("IPP Error: WriteRST() failed - ",jerr);
    return jerr;
  }

  for(int c = 0; c < m_jpeg_ncomp; c++)
  {
    m_ccomp[c].m_lastDC = 0;
  }

  m_restarts_to_go = m_jpeg_restart_interval;

  return JPEG_OK;
} // CJPEGEncoder::ProcessRestart()


JERRCODE CJPEGEncoder::ProcessRestart(
  int    stat[2][256],
  int    id[MAX_COMPS_PER_SCAN],
  int    Ss,
  int    Se,
  int    Ah,
  int    Al)
{
  IppStatus status;

  TRC0("-> ProcessRestart");

  // flush IppiEncodeHuffmanState
  if(JPEG_PROGRESSIVE == m_jpeg_mode)
  {
    if(Ss == 0 && Se == 0)
    {
      // DC scan
      // nothing to do
    }
    else
    {
      // AC scan

      if(Ah == 0)
      {
        status = ippiGetHuffmanStatistics8x8_ACFirst_JPEG_16s_C1(
                   0,&stat[m_ccomp[id[0]].m_ac_selector][0],Ss,Se,Al,m_state,1);

        if(ippStsNoErr > status)
        {
          LOG1("IPP Error: ippiGetHuffmanStatistics8x8_ACFirst_JPEG_16s_C1() failed - ",status);
          return JPEG_ERR_INTERNAL;
        }
      }
      else
      {
        status = ippiGetHuffmanStatistics8x8_ACRefine_JPEG_16s_C1(
                   0,&stat[m_ccomp[id[0]].m_ac_selector][0],Ss,Se,Al,m_state,1);

        if(ippStsNoErr > status)
        {
          LOG1("IPP Error: ippiGetHuffmanStatistics8x8_ACRefine_JPEG_16s_C1() failed - ",status);
          return JPEG_ERR_INTERNAL;
        }
      }
    }
  }

  status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  for(int c = 0; c < m_jpeg_ncomp; c++)
  {
    m_ccomp[c].m_lastDC = 0;
  }

  m_restarts_to_go = m_jpeg_restart_interval;

  return JPEG_OK;
} // CJPEGEncoder::ProcessRestart()


JERRCODE CJPEGEncoder::WriteSOS(void)
{
  int len;
  JERRCODE jerr;

  TRC0("-> WriteSOS");

  len = 3 + m_jpeg_ncomp*2 + 3;

  TRC1("  emit marker ",JM_SOS);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_SOS);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(m_jpeg_ncomp);
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  ncomp ",m_jpeg_ncomp);

  for(int i = 0; i < m_jpeg_ncomp; i++)
  {
    jerr = m_BitStreamOut.WriteByte(i);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamOut.WriteByte((m_ccomp[i].m_dc_selector << 4) | m_ccomp[i].m_ac_selector);
    if(JPEG_OK != jerr)
      return jerr;

    TRC1("    id ",i);
    TRC1("      dc_selector ",m_ccomp[i].m_dc_selector);
    TRC1("      ac_selector ",m_ccomp[i].m_ac_selector);
  }

  jerr = m_BitStreamOut.WriteByte(m_ss); // Ss
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(m_se); // se
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(((m_ah << 4) | m_al));  // Ah/Al
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  Ss ",m_ss);
  TRC1("  Se ",m_se);
  TRC1("  Ah ",m_ah);
  TRC1("  Al ",m_al);

  return JPEG_OK;
} // CJPEGEncoder::WriteSOS()


JERRCODE CJPEGEncoder::WriteSOS(
  int    ncomp,
  int    id[MAX_COMPS_PER_SCAN],
  int    Ss,
  int    Se,
  int    Ah,
  int    Al)
{
  int len;
  JERRCODE jerr;

  TRC0("-> WriteSOS");

  len = 3 + ncomp*2 + 3;

  TRC1("  emit marker ",JM_SOS);
  TRC1("    length ",len);

  jerr = m_BitStreamOut.WriteWord(0xff00 | JM_SOS);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteWord(len);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(ncomp);
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  ncomp ",ncomp);

  for(int i = 0; i < ncomp; i++)
  {
    jerr = m_BitStreamOut.WriteByte(id[i]);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = m_BitStreamOut.WriteByte((m_ccomp[id[i]].m_dc_selector << 4) | m_ccomp[id[i]].m_ac_selector);
    if(JPEG_OK != jerr)
      return jerr;

    TRC1("    id ",id[i]);
    TRC1("    dc_selector ",m_ccomp[id[i]].m_dc_selector);
    TRC1("    ac_selector ",m_ccomp[id[i]].m_ac_selector);
  }

  jerr = m_BitStreamOut.WriteByte(Ss);       // Ss
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(Se);       // Se
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_BitStreamOut.WriteByte(((Ah & 0x0f) << 4) | (Al & 0x0f));  // Ah/Al
  if(JPEG_OK != jerr)
    return jerr;

  TRC1("  Ss ",Ss);
  TRC1("  Se ",Se);
  TRC1("  Ah ",Ah);
  TRC1("  Al ",Al);

  return JPEG_OK;
} // CJPEGEncoder::WriteSOS()


JERRCODE CJPEGEncoder::SelectScanScripts(void)
{
  if(0 != m_scan_script)
  {
    delete[] m_scan_script;
    m_scan_script = 0;
  }

  switch(m_jpeg_ncomp)
  {
  case 1:
    m_scan_count = 6;
    m_scan_script = new JPEG_SCAN [m_scan_count];

    // 1 DC scan, def
    m_scan_script[0].ncomp = 1;
    m_scan_script[0].id[0] = 0;
    m_scan_script[0].Ss    = 0;
    m_scan_script[0].Se    = 0;
    m_scan_script[0].Ah    = 0;
    m_scan_script[0].Al    = 1;
    // 2 AC scan, def(luma)
    m_scan_script[1].ncomp = 1;
    m_scan_script[1].id[0] = 0;
    m_scan_script[1].Ss    = 1;
    m_scan_script[1].Se    = 5;
    m_scan_script[1].Ah    = 0;
    m_scan_script[1].Al    = 2;
    // 3 AC scan, def(luma)
    m_scan_script[2].ncomp = 1;
    m_scan_script[2].id[0] = 0;
    m_scan_script[2].Ss    = 6;
    m_scan_script[2].Se    = 63;
    m_scan_script[2].Ah    = 0;
    m_scan_script[2].Al    = 2;
    // 4 AC scan, ref(luma)
    m_scan_script[3].ncomp = 1;
    m_scan_script[3].id[0] = 0;
    m_scan_script[3].Ss    = 1;
    m_scan_script[3].Se    = 63;
    m_scan_script[3].Ah    = 2;
    m_scan_script[3].Al    = 1;
    // 5 DC scan, ref
    m_scan_script[4].ncomp = 1;
    m_scan_script[4].id[0] = 0;
    m_scan_script[4].Ss    = 0;
    m_scan_script[4].Se    = 0;
    m_scan_script[4].Ah    = 1;
    m_scan_script[4].Al    = 0;
    // 6 AC scan, ref(luma)
    m_scan_script[5].ncomp = 1;
    m_scan_script[5].id[0] = 0;
    m_scan_script[5].Ss    = 1;
    m_scan_script[5].Se    = 63;
    m_scan_script[5].Ah    = 1;
    m_scan_script[5].Al    = 0;
    break;

  case 3:
    m_scan_count = 10;
    m_scan_script = new JPEG_SCAN [m_scan_count];

    // 1 DC scan, def
    m_scan_script[0].ncomp = 3;
    m_scan_script[0].id[0] = 0;
    m_scan_script[0].id[1] = 1;
    m_scan_script[0].id[2] = 2;
    m_scan_script[0].Ss    = 0;
    m_scan_script[0].Se    = 0;
    m_scan_script[0].Ah    = 0;
    m_scan_script[0].Al    = 1;
    // 2 AC scan, def(luma)
    m_scan_script[1].ncomp = 1;
    m_scan_script[1].id[0] = 0;
    m_scan_script[1].Ss    = 1;
    m_scan_script[1].Se    = 5;
    m_scan_script[1].Ah    = 0;
    m_scan_script[1].Al    = 2;
    // 3 AC scan, def(cr)
    m_scan_script[2].ncomp = 1;
    m_scan_script[2].id[0] = 2;
    m_scan_script[2].Ss    = 1;
    m_scan_script[2].Se    = 63;
    m_scan_script[2].Ah    = 0;
    m_scan_script[2].Al    = 1;
    // 4 AC scan, def(cb)
    m_scan_script[3].ncomp = 1;
    m_scan_script[3].id[0] = 1;
    m_scan_script[3].Ss    = 1;
    m_scan_script[3].Se    = 63;
    m_scan_script[3].Ah    = 0;
    m_scan_script[3].Al    = 1;
    // 5 AC scan, def(luma)
    m_scan_script[4].ncomp = 1;
    m_scan_script[4].id[0] = 0;
    m_scan_script[4].Ss    = 6;
    m_scan_script[4].Se    = 63;
    m_scan_script[4].Ah    = 0;
    m_scan_script[4].Al    = 2;
    // 7 AC scan, ref(luma)
    m_scan_script[5].ncomp = 1;
    m_scan_script[5].id[0] = 0;
    m_scan_script[5].Ss    = 1;
    m_scan_script[5].Se    = 63;
    m_scan_script[5].Ah    = 2;
    m_scan_script[5].Al    = 1;
    // 6 DC scan, ref
    m_scan_script[6].ncomp = 3;
    m_scan_script[6].id[0] = 0;
    m_scan_script[6].id[1] = 1;
    m_scan_script[6].id[2] = 2;
    m_scan_script[6].Ss    = 0;
    m_scan_script[6].Se    = 0;
    m_scan_script[6].Ah    = 1;
    m_scan_script[6].Al    = 0;
    // 8 AC scan, ref(cr)
    m_scan_script[7].ncomp = 1;
    m_scan_script[7].id[0] = 2;
    m_scan_script[7].Ss    = 1;
    m_scan_script[7].Se    = 63;
    m_scan_script[7].Ah    = 1;
    m_scan_script[7].Al    = 0;
    // 9 AC scan, ref(cb)
    m_scan_script[8].ncomp = 1;
    m_scan_script[8].id[0] = 1;
    m_scan_script[8].Ss    = 1;
    m_scan_script[8].Se    = 63;
    m_scan_script[8].Ah    = 1;
    m_scan_script[8].Al    = 0;
    // 10 AC scan, ref(luma)
    m_scan_script[9].ncomp = 1;
    m_scan_script[9].id[0] = 0;
    m_scan_script[9].Ss    = 1;
    m_scan_script[9].Se    = 63;
    m_scan_script[9].Ah    = 1;
    m_scan_script[9].Al    = 0;
    break;

  case 4:
    m_scan_count = 18;
    m_scan_script = new JPEG_SCAN [m_scan_count];

    // 1 DC scan, def
    m_scan_script[0].ncomp = 4;
    m_scan_script[0].id[0] = 0;
    m_scan_script[0].id[1] = 1;
    m_scan_script[0].id[2] = 2;
    m_scan_script[0].id[3] = 3;
    m_scan_script[0].Ss    = 0;
    m_scan_script[0].Se    = 0;
    m_scan_script[0].Ah    = 0;
    m_scan_script[0].Al    = 1;
    // 2 AC scan, def(0)
    m_scan_script[1].ncomp = 1;
    m_scan_script[1].id[0] = 0;
    m_scan_script[1].Ss    = 1;
    m_scan_script[1].Se    = 5;
    m_scan_script[1].Ah    = 0;
    m_scan_script[1].Al    = 2;
    // 3 AC scan, def(1)
    m_scan_script[2].ncomp = 1;
    m_scan_script[2].id[0] = 1;
    m_scan_script[2].Ss    = 1;
    m_scan_script[2].Se    = 5;
    m_scan_script[2].Ah    = 0;
    m_scan_script[2].Al    = 2;
    // 4 AC scan, def(2)
    m_scan_script[3].ncomp = 1;
    m_scan_script[3].id[0] = 2;
    m_scan_script[3].Ss    = 1;
    m_scan_script[3].Se    = 5;
    m_scan_script[3].Ah    = 0;
    m_scan_script[3].Al    = 2;
    // 5 AC scan, def(3)
    m_scan_script[4].ncomp = 1;
    m_scan_script[4].id[0] = 3;
    m_scan_script[4].Ss    = 1;
    m_scan_script[4].Se    = 5;
    m_scan_script[4].Ah    = 0;
    m_scan_script[4].Al    = 2;
    // 6 AC scan, def(0)
    m_scan_script[5].ncomp = 1;
    m_scan_script[5].id[0] = 0;
    m_scan_script[5].Ss    = 6;
    m_scan_script[5].Se    = 63;
    m_scan_script[5].Ah    = 0;
    m_scan_script[5].Al    = 2;
    // 7 AC scan, def(1)
    m_scan_script[6].ncomp = 1;
    m_scan_script[6].id[0] = 1;
    m_scan_script[6].Ss    = 6;
    m_scan_script[6].Se    = 63;
    m_scan_script[6].Ah    = 0;
    m_scan_script[6].Al    = 2;
    // 8 AC scan, def(2)
    m_scan_script[7].ncomp = 1;
    m_scan_script[7].id[0] = 2;
    m_scan_script[7].Ss    = 6;
    m_scan_script[7].Se    = 63;
    m_scan_script[7].Ah    = 0;
    m_scan_script[7].Al    = 2;
    // 9 AC scan, def(3)
    m_scan_script[8].ncomp = 1;
    m_scan_script[8].id[0] = 3;
    m_scan_script[8].Ss    = 6;
    m_scan_script[8].Se    = 63;
    m_scan_script[8].Ah    = 0;
    m_scan_script[8].Al    = 2;
    // 10 AC scan, ref(0)
    m_scan_script[9].ncomp = 1;
    m_scan_script[9].id[0] = 0;
    m_scan_script[9].Ss    = 1;
    m_scan_script[9].Se    = 63;
    m_scan_script[9].Ah    = 2;
    m_scan_script[9].Al    = 1;
    // 11 AC scan, ref(1)
    m_scan_script[10].ncomp = 1;
    m_scan_script[10].id[0] = 1;
    m_scan_script[10].Ss    = 1;
    m_scan_script[10].Se    = 63;
    m_scan_script[10].Ah    = 2;
    m_scan_script[10].Al    = 1;
    // 12 AC scan, ref(2)
    m_scan_script[11].ncomp = 1;
    m_scan_script[11].id[0] = 2;
    m_scan_script[11].Ss    = 1;
    m_scan_script[11].Se    = 63;
    m_scan_script[11].Ah    = 2;
    m_scan_script[11].Al    = 1;
    // 13 AC scan, ref(3)
    m_scan_script[12].ncomp = 1;
    m_scan_script[12].id[0] = 3;
    m_scan_script[12].Ss    = 1;
    m_scan_script[12].Se    = 63;
    m_scan_script[12].Ah    = 2;
    m_scan_script[12].Al    = 1;
    // 14 DC scan, ref
    m_scan_script[13].ncomp = 4;
    m_scan_script[13].id[0] = 0;
    m_scan_script[13].id[1] = 1;
    m_scan_script[13].id[2] = 2;
    m_scan_script[13].id[3] = 3;
    m_scan_script[13].Ss    = 0;
    m_scan_script[13].Se    = 0;
    m_scan_script[13].Ah    = 1;
    m_scan_script[13].Al    = 0;
    // 15 AC scan, ref(0)
    m_scan_script[14].ncomp = 1;
    m_scan_script[14].id[0] = 0;
    m_scan_script[14].Ss    = 1;
    m_scan_script[14].Se    = 63;
    m_scan_script[14].Ah    = 1;
    m_scan_script[14].Al    = 0;
    // 16 AC scan, ref(1)
    m_scan_script[15].ncomp = 1;
    m_scan_script[15].id[0] = 1;
    m_scan_script[15].Ss    = 1;
    m_scan_script[15].Se    = 63;
    m_scan_script[15].Ah    = 1;
    m_scan_script[15].Al    = 0;
    // 17 AC scan, ref(2)
    m_scan_script[16].ncomp = 1;
    m_scan_script[16].id[0] = 2;
    m_scan_script[16].Ss    = 1;
    m_scan_script[16].Se    = 63;
    m_scan_script[16].Ah    = 1;
    m_scan_script[16].Al    = 0;
    // 18 AC scan, ref(3)
    m_scan_script[17].ncomp = 1;
    m_scan_script[17].id[0] = 3;
    m_scan_script[17].Ss    = 1;
    m_scan_script[17].Se    = 63;
    m_scan_script[17].Ah    = 1;
    m_scan_script[17].Al    = 0;
    break;

  default:
    return JPEG_NOT_IMPLEMENTED;
  }

  return JPEG_OK;
} // CJPEGEncoder::SelectScanScripts()


JERRCODE CJPEGEncoder::Init(void)
{
  int       i;
  int       tr_buf_size = 0;
  CJPEGColorComponent* curr_comp;
  JERRCODE  jerr;

  m_num_threads = get_num_threads();

  m_xPadding = m_numxMCU * m_mcuWidth  - m_src.width;
  m_yPadding = m_numyMCU * m_mcuHeight - m_src.height;

  m_ccWidth  = m_mcuWidth * m_numxMCU;
  m_ccHeight = m_mcuHeight;

  for(m_nblock = 0, i = 0; i < m_jpeg_ncomp; i++)
  {
    curr_comp = &m_ccomp[i];

    curr_comp->m_id          = i;
    curr_comp->m_comp_no     = i;
    curr_comp->m_hsampling   = (m_jpeg_sampling == JS_444) ? 1 : (i == 0 || i == 3 ? 2 : 1);
    curr_comp->m_vsampling   = (m_jpeg_sampling == JS_411) ? (i == 0 || i == 3 ? 2 : 1) : 1;
    curr_comp->m_h_factor    = (m_jpeg_sampling == JS_444) ? 1 : (i == 0 || i == 3 ? 1 : 2);
    curr_comp->m_v_factor    = (m_jpeg_sampling == JS_411) ? (i == 0 || i == 3 ? 1 : 2) : 1;
    curr_comp->m_nblocks     = curr_comp->m_hsampling * curr_comp->m_vsampling;

    m_nblock += curr_comp->m_nblocks;

    switch(m_jpeg_mode)
    {
    case JPEG_BASELINE:
      curr_comp->m_cc_height = m_mcuHeight;
      curr_comp->m_cc_step   = m_numxMCU * m_mcuWidth;
      break;

    case JPEG_EXTENDED:
      curr_comp->m_cc_height = m_mcuHeight;
      curr_comp->m_cc_step   = m_numxMCU * m_mcuWidth * ((m_jpeg_precision <= 8) ? sizeof(Ipp8u) : sizeof(Ipp16s));
      break;

    case JPEG_PROGRESSIVE:
      curr_comp->m_cc_height = m_mcuHeight;
      curr_comp->m_cc_step   = m_numxMCU * m_mcuWidth;
      break;

    case JPEG_LOSSLESS:
      curr_comp->m_cc_height = m_mcuHeight;
      curr_comp->m_cc_step   = m_numxMCU * m_mcuWidth * sizeof(Ipp16s);
      break;

    default:
      return JPEG_ERR_PARAMS;
    }

    curr_comp->m_ss_height = curr_comp->m_cc_height / curr_comp->m_v_factor;
    curr_comp->m_ss_step   = curr_comp->m_cc_step   / curr_comp->m_h_factor;

    // color convert buffer
    jerr = curr_comp->CreateBufferCC(m_num_threads);
    if(JPEG_OK != jerr)
      return jerr;

    // sub-sampling buffer
    jerr = curr_comp->CreateBufferSS(m_num_threads);
    if(JPEG_OK != jerr)
      return jerr;

    if(JPEG_LOSSLESS == m_jpeg_mode)
    {
      curr_comp->m_row1.Allocate(curr_comp->m_cc_step);
      curr_comp->m_row2.Allocate(curr_comp->m_cc_step);
      curr_comp->m_curr_row = (Ipp16s*)curr_comp->m_row1.m_buffer;
      curr_comp->m_prev_row = (Ipp16s*)curr_comp->m_row2.m_buffer;
    }
  } // for m_jpeg_ncomp

  if(JPEG_PROGRESSIVE == m_jpeg_mode)
  {
    SelectScanScripts();
  }

  switch(m_jpeg_mode)
  {
  case JPEG_BASELINE:
    if(!m_optimal_htbl)
    tr_buf_size = m_numxMCU * m_nblock * DCTSIZE2 * sizeof(Ipp16s) * m_num_threads * m_rstiHeight;
    else
      tr_buf_size = m_numxMCU * m_numyMCU * m_nblock * DCTSIZE2 * sizeof(Ipp16s) * m_num_threads;
    break;

  case JPEG_EXTENDED:
    if(!m_optimal_htbl)
      tr_buf_size = m_numxMCU * m_nblock * DCTSIZE2 * sizeof(Ipp16s) * m_num_threads;
    else
      tr_buf_size = m_numxMCU * m_numyMCU * m_nblock * DCTSIZE2 * sizeof(Ipp16s) * m_num_threads;
    break;

  case JPEG_PROGRESSIVE:
    tr_buf_size = m_numxMCU * m_numyMCU * m_nblock * DCTSIZE2 * sizeof(Ipp16s) * m_num_threads;
    break;

  case JPEG_LOSSLESS:
    if(!m_optimal_htbl)
      tr_buf_size = m_numxMCU * m_nblock * sizeof(Ipp16s) * m_num_threads;
    else
      tr_buf_size = m_numxMCU * m_numyMCU * m_nblock * sizeof(Ipp16s) * m_num_threads;
    break;

  default:
    return JPEG_ERR_PARAMS;
  }

  // MCUs buffer
  if(0 == m_block_buffer)
  {
    m_block_buffer = (Ipp16s*)ippMalloc(tr_buf_size);
    if(0 == m_block_buffer)
    {
      return JPEG_ERR_ALLOC;
    }

    ippsZero_8u((Ipp8u*)m_block_buffer,tr_buf_size);
  }

  int buflen;

  buflen = (m_jpeg_mode == JPEG_LOSSLESS) ?
    IPP_MAX(ENC_DEFAULT_BUFLEN,m_numxMCU * m_jpeg_ncomp * 2 * 2) :
    ENC_DEFAULT_BUFLEN;

  jerr = m_BitStreamOut.Init(buflen);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = m_state.Create();
  if(JPEG_OK != jerr)
    return jerr;
#ifdef _OPENMP

  if(m_jpeg_restart_interval != 0 && JT_RSTI == m_threading_mode)
  {
    m_BitStreamOutT = new CBitStreamOutput[m_num_threads];

    buflen = m_numxMCU * m_mcuWidth*m_mcuHeight*m_rstiHeight*m_jpeg_ncomp*2;

    for(i =0; i < m_num_threads; i++)
    {
      m_BitStreamOutT[i].Init(buflen);
    }

    m_state_t = new CJPEGEncoderHuffmanState[m_num_threads];

    for(i =0; i < m_num_threads; i++)
    {
      jerr = m_state_t[i].Create();
      if(JPEG_OK != jerr)
        return jerr;
    }

    m_lastDC = new Ipp16s*[m_num_threads];

    for(i =0; i < m_num_threads; i++)
    {
      m_lastDC[i] = new Ipp16s[4];

      m_lastDC[i][0] = 0;
      m_lastDC[i][1] = 0;
      m_lastDC[i][2] = 0;
      m_lastDC[i][3] = 0;
    }
  }

#endif

  return JPEG_OK;
} // CJPEGEncoder::Init()


JERRCODE CJPEGEncoder::ColorConvert(int nMCURow, int thread_id)
{
  int       cc_h;
  int       srcStep;
  int       convert = 0;

  Ipp8u*    pSrc8u  = 0;
  Ipp16u*   pSrc16u = 0;
  IppiSize  roi;
  IppStatus status;

  cc_h = m_ccHeight;

  if(nMCURow == m_numyMCU - 1)
  {
    cc_h = m_mcuHeight - m_yPadding;
  }

  roi.width  = m_src.width;
  roi.height = cc_h;

  srcStep = m_src.lineStep[0];

  if(m_src.precision <= 8)
    pSrc8u  =                   m_src.p.Data8u[0]  + nMCURow * m_mcuHeight * srcStep;
  else
    pSrc16u = (Ipp16u*)((Ipp8u*)m_src.p.Data16s[0] + nMCURow * m_mcuHeight * srcStep);

  if(m_jpeg_color == JC_UNKNOWN && m_src.color == JC_UNKNOWN)
  {

    switch(m_jpeg_ncomp)
    {
    case 1:
      {
        int     dstStep;
        Ipp8u*  pDst8u;
        Ipp16u* pDst16u;

        dstStep = m_ccomp[0].m_cc_step;
        convert = 1;

        if(m_src.precision <= 8)
        {
          pDst8u = m_ccomp[0].GetCCBufferPtr(thread_id);

          status = ippiCopy_8u_C1R(pSrc8u,srcStep,pDst8u,dstStep,roi);
        }
        else
        {
          pDst16u = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);

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
        int     dstStep;
        Ipp8u*  pDst8u[3];
        Ipp16u* pDst16u[3];

        dstStep = m_ccomp[0].m_cc_step;
        convert = 1;

        if(m_src.precision <= 8)
        {
          pDst8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
          pDst8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
          pDst8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

          status = ippiCopy_8u_C3P3R(pSrc8u,srcStep,pDst8u,dstStep,roi);
        }
        else
        {
          pDst16u[0] = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);
          pDst16u[1] = (Ipp16u*)m_ccomp[1].GetCCBufferPtr(thread_id);
          pDst16u[2] = (Ipp16u*)m_ccomp[2].GetCCBufferPtr(thread_id);

          status = ippiCopy_16s_C3P3R((Ipp16s*)pSrc16u,srcStep,(Ipp16s**)pDst16u,dstStep,roi);
        }

        if(ippStsNoErr != status)
        {
          LOG1("IPP Error: ippiCopy_8u_C3P3R() failed - ",status);
          return JPEG_ERR_INTERNAL;
        }
      }
      break;

    case 4:
      {
        int     dstStep;
        Ipp8u*  pDst8u[4];
        Ipp16u* pDst16u[4];

        dstStep = m_ccomp[0].m_cc_step;
        convert = 1;

        if(m_src.precision <= 8)
        {
          pDst8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
          pDst8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
          pDst8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);
          pDst8u[3] = m_ccomp[3].GetCCBufferPtr(thread_id);

          status = ippiCopy_8u_C4P4R(pSrc8u,srcStep,pDst8u,dstStep,roi);
        }
        else
        {
          pDst16u[0] = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);
          pDst16u[1] = (Ipp16u*)m_ccomp[1].GetCCBufferPtr(thread_id);
          pDst16u[2] = (Ipp16u*)m_ccomp[2].GetCCBufferPtr(thread_id);
          pDst16u[3] = (Ipp16u*)m_ccomp[3].GetCCBufferPtr(thread_id);

          status = ippiCopy_16s_C4P4R((Ipp16s*)pSrc16u,srcStep,(Ipp16s**)pDst16u,dstStep,roi);
        }

        if(ippStsNoErr != status)
        {
          LOG1("IPP Error: ippiCopy_8u_C4P4R() failed - ",status);
          return JPEG_ERR_INTERNAL;
        }
      }
      break;

    default:
      return JPEG_NOT_IMPLEMENTED;
    }
  }

  // Gray to Gray
  if(m_src.color == JC_GRAY && m_jpeg_color == JC_GRAY)
  {
    int     dstStep;
    Ipp8u*  pDst8u;
    Ipp16u* pDst16u;

    dstStep = m_ccomp[0].m_cc_step;
    convert = 1;

    if(m_src.precision <= 8)
    {
      pDst8u  = m_ccomp[0].GetCCBufferPtr(thread_id);

      status  = ippiCopy_8u_C1R(pSrc8u,srcStep,pDst8u,dstStep,roi);
    }
    else
    {
      pDst16u = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);

      status = ippiCopy_16s_C1R((Ipp16s*)pSrc16u,srcStep,(Ipp16s*)pDst16u,dstStep,roi);
    }

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_C1R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // RGB to Gray
  if(m_src.color == JC_RGB && m_jpeg_color == JC_GRAY)
  {
    int    dstStep;
    Ipp8u* pDst8u;

    dstStep = m_ccomp[0].m_cc_step;
    convert = 1;

    pDst8u = m_ccomp[0].GetCCBufferPtr(thread_id);

    status = ippiRGBToY_JPEG_8u_C3C1R(pSrc8u,srcStep,pDst8u,dstStep,roi);
    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiRGBToY_JPEG_8u_C3C1R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // RGB to RGB
  if(m_src.color == JC_RGB && m_jpeg_color == JC_RGB)
  {
    int     dstStep;
    Ipp8u*  pDst8u[3];
    Ipp16u* pDst16u[3];

    dstStep = m_ccomp[0].m_cc_step;
    convert = 1;

    if(m_src.precision <= 8)
    {
      pDst8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
      pDst8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
      pDst8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

      status = ippiCopy_8u_C3P3R(pSrc8u,srcStep,pDst8u,dstStep,roi);
    }
    else
    {
      pDst16u[0] = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);
      pDst16u[1] = (Ipp16u*)m_ccomp[1].GetCCBufferPtr(thread_id);
      pDst16u[2] = (Ipp16u*)m_ccomp[2].GetCCBufferPtr(thread_id);

      status = ippiCopy_16s_C3P3R((Ipp16s*)pSrc16u,srcStep,(Ipp16s**)pDst16u,dstStep,roi);
    }

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_C3P3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // BGR to RGB
  if(m_src.color == JC_BGR && m_jpeg_color == JC_RGB)
  {
    int     dstStep;
    Ipp8u*  pDst8u[3];
    Ipp16u* pDst16u[3];

    dstStep = m_ccomp[0].m_cc_step;
    convert = 1;

    if(m_src.precision <= 8)
    {
      pDst8u[2] = m_ccomp[0].GetCCBufferPtr(thread_id);
      pDst8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
      pDst8u[0] = m_ccomp[2].GetCCBufferPtr(thread_id);

      status = ippiCopy_8u_C3P3R(pSrc8u,srcStep,pDst8u,dstStep,roi);
    }
    else
    {
      pDst16u[2] = (Ipp16u*)m_ccomp[0].GetCCBufferPtr(thread_id);
      pDst16u[1] = (Ipp16u*)m_ccomp[1].GetCCBufferPtr(thread_id);
      pDst16u[0] = (Ipp16u*)m_ccomp[2].GetCCBufferPtr(thread_id);

      status = ippiCopy_16s_C3P3R((Ipp16s*)pSrc16u,srcStep,(Ipp16s**)pDst16u,dstStep,roi);
    }

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_C3P3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // RGB to YCbCr
  if(m_src.color == JC_RGB && m_jpeg_color == JC_YCBCR)
  {
    int    dstStep;
    Ipp8u* pDst8u[3];

    dstStep = m_ccomp[0].m_cc_step;
    convert = 1;

    pDst8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pDst8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pDst8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

    if(JD_PIXEL == m_src.order)
    {
      status = ippiRGBToYCbCr_JPEG_8u_C3P3R(pSrc8u,srcStep,pDst8u,dstStep,roi);
    }
    else
    {
      const Ipp8u* pSrcP[3];

      pSrcP[0] = m_src.p.Data8u[0]  + nMCURow * m_mcuHeight * srcStep;
      pSrcP[1] = m_src.p.Data8u[1]  + nMCURow * m_mcuHeight * srcStep;
      pSrcP[2] = m_src.p.Data8u[2]  + nMCURow * m_mcuHeight * srcStep;

      status = ippiRGBToYCbCr_JPEG_8u_P3R(pSrcP,srcStep,pDst8u,dstStep,roi);
    }

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiRGBToYCbCr_JPEG_8u_C3P3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // BGR to YCbCr
  if(m_src.color == JC_BGR && m_jpeg_color == JC_YCBCR)
  {
    int    dstStep;
    Ipp8u* pDst8u[3];

    dstStep = m_ccomp[0].m_cc_step;
    convert = 1;

    pDst8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pDst8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pDst8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

    status = ippiBGRToYCbCr_JPEG_8u_C3P3R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiBGRToYCbCr_JPEG_8u_C3P3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // RGBA to YCbCr
  if(m_src.color == JC_RGBA && m_jpeg_color == JC_YCBCR)
  {
    int    dstStep;
    Ipp8u* pDst8u[3];

    dstStep = m_ccomp[0].m_cc_step;
    convert = 1;

    pDst8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pDst8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pDst8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

    status = ippiRGBToYCbCr_JPEG_8u_C4P3R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiRGBToYCbCr_JPEG_8u_C4P3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // YCbCr to YCbCr (422 sampling)
  if(m_src.color == JC_YCBCR && m_jpeg_color == JC_YCBCR &&
     m_src.sampling == JS_422 && m_jpeg_sampling == JS_422)
  {
    int    dstStep[3];
    Ipp8u* pDst8u[3];

    convert = 1;

    dstStep[0] = m_ccomp[0].m_cc_step;
    dstStep[1] = m_ccomp[1].m_cc_step;
    dstStep[2] = m_ccomp[2].m_cc_step;

    pDst8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pDst8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pDst8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);

    status = ippiYCbCr422_8u_C2P3R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiYCbCr422_8u_C2P3R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // CMYK to CMYK
  if(m_src.color == JC_CMYK && m_jpeg_color == JC_CMYK)
  {
    int    dstStep;
    Ipp8u* pDst8u[4];

    dstStep = m_ccomp[0].m_cc_step;
    convert = 1;

    pDst8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pDst8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pDst8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);
    pDst8u[3] = m_ccomp[3].GetCCBufferPtr(thread_id);

    status = ippiCopy_8u_C4P4R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_C4P4R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  // CMYK to YCCK
  if(m_src.color == JC_CMYK && m_jpeg_color == JC_YCCK)
  {
    int    dstStep;
    Ipp8u* pDst8u[4];

    dstStep = m_ccomp[0].m_cc_step;
    convert = 1;

    pDst8u[0] = m_ccomp[0].GetCCBufferPtr(thread_id);
    pDst8u[1] = m_ccomp[1].GetCCBufferPtr(thread_id);
    pDst8u[2] = m_ccomp[2].GetCCBufferPtr(thread_id);
    pDst8u[3] = m_ccomp[3].GetCCBufferPtr(thread_id);

    status = ippiCMYKToYCCK_JPEG_8u_C4P4R(pSrc8u,srcStep,pDst8u,dstStep,roi);

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCMYKToYCCK_JPEG_8u_C4P4R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  if(!convert)
    return JPEG_NOT_IMPLEMENTED;

  return JPEG_OK;
} // CJPEGEncoder::ColorConvert()


JERRCODE CJPEGEncoder::DownSampling(int nMCURow, int thread_id)
{
  int i, j, k;
  int cc_h;
  CJPEGColorComponent* curr_comp;
  IppStatus status;

  Ipp8u  val;
  Ipp8u* p;
  Ipp8u* p1;
  Ipp8u* p2;

  for(k = 0; k < m_jpeg_ncomp; k++)
  {
    curr_comp = &m_ccomp[k];
    cc_h = curr_comp->m_cc_height;

    // expand right edge
    if(m_xPadding)
    {
      for(i = 0; i < cc_h; i++)
      {
        if(m_src.precision <= 8)
        {
          p = curr_comp->GetCCBufferPtr(thread_id) + i*curr_comp->m_cc_step;
          val = p[m_src.width - 1];
          for(j = 0; j < m_xPadding; j++)
          {
            p[m_src.width + j] = val;
          }
        }
        else
        {
          Ipp16u  v16;
          Ipp16u* p16;
          p16 = (Ipp16u*)(curr_comp->GetCCBufferPtr(thread_id) + i*curr_comp->m_cc_step);
          v16 = p16[m_src.width - 1];
          for(j = 0; j < m_xPadding; j++)
          {
            p16[m_src.width + j] = v16;
          }
        }
      }
    }

    // expand bottom edge only for last MCU row
    if(nMCURow == m_numyMCU - 1)
    {
      cc_h = cc_h - m_yPadding;
      p = curr_comp->GetCCBufferPtr(thread_id) + (cc_h-1) * curr_comp->m_cc_step;
      p1 = p;

      for(i = 0; i < m_yPadding; i++)
      {
        p1 += curr_comp->m_cc_step;
        ippsCopy_8u(p,p1,curr_comp->m_cc_step);
      }
    }

    // sampling 444
    if(curr_comp->m_h_factor == 1 && curr_comp->m_v_factor == 1)
    {
      Ipp8u* pSrc = curr_comp->GetCCBufferPtr(thread_id);
      Ipp8u* pDst = curr_comp->GetSSBufferPtr(thread_id);

      ippsCopy_8u(pSrc,pDst,curr_comp->m_cc_bufsize);
    }

    // sampling 422
    if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 1)
    {
      int    srcStep;
      int    dstStep;
      Ipp8u* pSrc;
      Ipp8u* pDst;

      srcStep = curr_comp->m_cc_step;
      dstStep = curr_comp->m_ss_step;

      pSrc = curr_comp->GetCCBufferPtr(thread_id);
      pDst = curr_comp->GetSSBufferPtr(thread_id);

      if(m_src.sampling == JS_422)
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
        for(i = 0; i < m_mcuHeight; i++)
        {
          status = ippiSampleDownRowH2V1_Box_JPEG_8u_C1(pSrc,srcStep,pDst);
          if(ippStsNoErr != status)
          {
            LOG0("Error: ippiSampleDownRowH2V1_Box_JPEG_8u_C1() failed!");
            return JPEG_ERR_INTERNAL;
          }

          pSrc += srcStep;
          pDst += dstStep;
        }
      }
    }

    // sampling 411
    if(curr_comp->m_h_factor == 2 && curr_comp->m_v_factor == 2)
    {
      int    srcStep;
      Ipp8u* pSrc;
      Ipp8u* pDst;

      srcStep = curr_comp->m_cc_step;

      pSrc = curr_comp->GetCCBufferPtr(thread_id);
      pDst = curr_comp->GetSSBufferPtr(thread_id);

      for(i = 0; i < cc_h; i += 2)
      {
        p1 = pSrc + (i+0)*srcStep;
        p2 = pSrc + (i+1)*srcStep;

        status = ippiSampleDownRowH2V2_Box_JPEG_8u_C1(p1,p2,srcStep,pDst);
        if(ippStsNoErr != status)
        {
          LOG0("Error: ippiSampleUpRowH2V2_Triangle_JPEG_8u_C1() failed!");
          return JPEG_ERR_INTERNAL;
        }

        pDst += curr_comp->m_ss_step;
      }
    }
  } // for m_jpeg_ncomp

  return JPEG_OK;
} // CJPEGEncoder::DownSampling()


JERRCODE CJPEGEncoder::ProcessBuffer(int nMCURow, int thread_id)
{
  int                  i, j, c;
  int                  copyHeight;
  int                  yPadd   = 0;
  int                  xPadd   = 0;
  int                  srcStep;
  Ipp8u*               pSrc8u  = 0;
  Ipp8u*               pDst8u  = 0;
  Ipp16u*              pSrc16u = 0;
  Ipp16u*              pDst16u = 0;
  CJPEGColorComponent* curr_comp;
  IppStatus            status;
  IppiSize             roi;

  Ipp8u  val;
  Ipp8u* p;
  Ipp8u* p1;

  for(c = 0; c < m_jpeg_ncomp; c++)
  {
    curr_comp = &m_ccomp[c];
    srcStep   = m_src.lineStep[c];

    if(m_src.precision <= 8)
    {
      pSrc8u     = m_src.p.Data8u[c]  + nMCURow * curr_comp->m_ss_height * srcStep;
      copyHeight = curr_comp->m_ss_height;
      if(curr_comp->m_v_factor == 1)
      {
        yPadd = m_yPadding;
      }
      else
      {
        yPadd = m_yPadding/2;
      }

      if(nMCURow == m_numyMCU - 1)
      {
        copyHeight -= yPadd;
      }

      roi.width  = (curr_comp->m_h_factor == 1) ? m_src.width : (m_src.width + 1) / 2;
      roi.height = copyHeight;

      pDst8u = curr_comp->GetSSBufferPtr(thread_id);

      status = ippiCopy_8u_C1R(pSrc8u, srcStep, pDst8u, curr_comp->m_ss_step, roi);
    }
    else
    {
      pSrc16u    = (Ipp16u*)((Ipp8u*)m_src.p.Data16s[c] + nMCURow * curr_comp->m_ss_height * srcStep);
      copyHeight = curr_comp->m_ss_height;

      if(nMCURow == m_numyMCU - 1)
      {
        yPadd       = m_yPadding;
        copyHeight -= m_yPadding;
      }

      roi.width  = m_src.width;
      roi.height = copyHeight;

      pDst16u = (Ipp16u*)curr_comp->GetSSBufferPtr(thread_id);
      status = ippiCopy_16s_C1R((Ipp16s*)pSrc16u,srcStep,(Ipp16s*)pDst16u,curr_comp->m_ss_step,roi);
    }

    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiCopy_8u_C1R() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }

    // expand right edge
    if(m_xPadding)
    {
      for(i = 0; i < curr_comp->m_ss_height; i++)
      {
        if(m_src.precision <= 8)
        {
          int width;

          if(curr_comp->m_h_factor == 1)
          {
            width = m_src.width;
            xPadd = m_xPadding;
          }
          else
          {
            width = (m_src.width) / 2;
            xPadd = (m_xPadding + 1) / 2;
          }

          p     = curr_comp->GetSSBufferPtr(thread_id) + i*curr_comp->m_ss_step;
          val   = p[width - 1];

          for(j = 0; j < xPadd; j++)
          {
            p[width + j] = val;
          }
        }
        else // 16 bit only 444 sampling
        {
          Ipp16u* p16;
          Ipp16u  v16;
          p16 = (Ipp16u*)(curr_comp->GetSSBufferPtr(thread_id) + i * curr_comp->m_ss_step);
          v16 = p16[m_src.width - 1];
          for(j = 0; j < m_xPadding; j++)
          {
            p16[m_src.width + j] = v16;
          }
        }
      }
    }

    // expand bottom edge only for last MCU row
    if(nMCURow == m_numyMCU - 1)
    {
      p = curr_comp->GetSSBufferPtr(thread_id) + (copyHeight - 1) * curr_comp->m_ss_step;
      p1 = p;

      for(i = 0; i < yPadd; i++)
      {
        p1 += curr_comp->m_ss_step;
        ippsCopy_8u(p,p1,curr_comp->m_ss_step);
      }
    }
  } // for m_jpeg_comp

  return JPEG_OK;
} // CJPEGEncoder::ProcessBuffer()


JERRCODE CJPEGEncoder::TransformMCURowBL(
  Ipp16s* pMCUBuf,
  int     thread_id)
{
  int c;
  int vs;
  int hs;
  int curr_mcu;
  int srcStep;
  Ipp8u*               src;
  Ipp16u*              qtbl;
  CJPEGColorComponent* curr_comp;
  IppStatus            status;

  for(curr_mcu = 0; curr_mcu < m_numxMCU; curr_mcu++)
  {
    for(c = 0; c < m_jpeg_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];

      qtbl = m_qntbl[curr_comp->m_q_selector];

      srcStep = curr_comp->m_ss_step;

      for(vs = 0; vs < curr_comp->m_vsampling; vs++)
      {
        src  = curr_comp->GetSSBufferPtr(thread_id) +
               8*curr_mcu*curr_comp->m_hsampling +
               8*vs*srcStep;

        for(hs = 0; hs < curr_comp->m_hsampling; hs++)
        {
          src += 8*hs;

          status = ippiDCTQuantFwd8x8LS_JPEG_8u16s_C1R(
                     src,srcStep,pMCUBuf,qtbl);

          if(ippStsNoErr != status)
          {
            LOG0("Error: ippiDCTQuantFwd8x8LS_JPEG_8u16s_C1R() failed!");
            return JPEG_ERR_INTERNAL;
          }

          pMCUBuf += DCTSIZE2;
        } // for m_hsampling
      } // for m_vsampling
    } // for m_jpeg_ncomp
  } // for m_numxMCU

  return JPEG_OK;
} // CJPEGEncoder::TransformMCURowBL()


JERRCODE CJPEGEncoder::TransformMCURowEX(
  Ipp16s* pMCUBuf,
  int     thread_id)
{
  int c;
  int vs;
  int hs;
  int curr_mcu;
  int srcStep;
  Ipp16u*              src;
  Ipp32f*              qtbl;
  CJPEGColorComponent* curr_comp;
  IppStatus            status;

  for(curr_mcu = 0; curr_mcu < m_numxMCU; curr_mcu++)
  {
    for(c = 0; c < m_jpeg_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];

      qtbl = m_qntbl[curr_comp->m_q_selector];

      srcStep = curr_comp->m_ss_step;

      for(vs = 0; vs < curr_comp->m_vsampling; vs++)
      {
        src  = (Ipp16u*)curr_comp->GetSSBufferPtr(thread_id) +
               8*curr_mcu*curr_comp->m_hsampling +
               8*vs*srcStep;

        for(hs = 0; hs < curr_comp->m_hsampling; hs++)
        {
          src += 8*hs;

          status = ippiDCTQuantFwd8x8LS_JPEG_16u16s_C1R(
                     src,srcStep,pMCUBuf,qtbl);

          if(ippStsNoErr != status)
          {
            LOG0("Error: ippiDCTQuantFwd8x8LS_JPEG_8u16s_C1R() failed!");
            return JPEG_ERR_INTERNAL;
          }

          pMCUBuf += DCTSIZE2;
        } // for m_hsampling
      } // for m_vsampling
    } // for m_jpeg_ncomp
  } // for m_numxMCU

  return JPEG_OK;
} // CJPEGEncoder::TransformMCURowEX()


JERRCODE CJPEGEncoder::TransformMCURowLS(Ipp16s* pMCUBuf, int nMCURow, int thread_id)
{
  int                    c;
  Ipp16s*                pDst;
  Ipp16s*                pCurrRow;
  Ipp16s*                pPrevRow;
  CJPEGColorComponent*   curr_comp;
  IppStatus              status;

  thread_id = thread_id;

  for(c = 0; c < m_jpeg_ncomp; c++)
  {
    curr_comp = &m_ccomp[c];

    pCurrRow = curr_comp->m_curr_row;
    pPrevRow = curr_comp->m_prev_row;

    pDst = pMCUBuf + c*m_numxMCU;

    if(m_src.precision <= 8)
    {
      IppiSize roi = { m_numxMCU, 1 };
      ippiConvert_8u16u_C1R((Ipp8u*)curr_comp->GetCCBufferPtr(),curr_comp->m_cc_step,(Ipp16u*)pCurrRow,m_numxMCU,roi);
    }
    else
    {
      ippsCopy_16s((Ipp16s*)curr_comp->GetCCBufferPtr(),pCurrRow,m_numxMCU);
    }

    if(m_pt)
    {
      // do point transform
      ippsRShiftC_16s_I(m_pt,pCurrRow,m_numxMCU);
    }

    if(0 != nMCURow && 0 == m_rst_go)
    {
      status = ippiDiffPredRow_JPEG_16s_C1(pCurrRow,pPrevRow,pDst,m_src.width,m_predictor);
      if(ippStsNoErr > status)
      {
        LOG1("IPP Error: ippiDiffPredRow_JPEG_16s_C1() failed - ",status);
        return JPEG_ERR_INTERNAL;
      }
    }
    else
    {
      status = ippiDiffPredFirstRow_JPEG_16s_C1(pCurrRow,pDst,m_src.width,m_jpeg_precision,m_pt);
      if(ippStsNoErr > status)
      {
        LOG1("IPP Error: ippiDiffPredFirstRow_JPEG_16s_C1() failed - ",status);
        return JPEG_ERR_INTERNAL;
      }
    }

    curr_comp->m_prev_row = pCurrRow;
    curr_comp->m_curr_row = pPrevRow;
  }

  m_rst_go = 0;

  return JPEG_OK;
} // CJPEGEncoder::TransformMCURowLS()


JERRCODE CJPEGEncoder::EncodeHuffmanMCURowBL(Ipp16s* pMCUBuf)
{
  int                    c;
  int                    vs;
  int                    hs;
  int                    mcu;
  int                    dstLen;
  int                    currPos;
  Ipp8u*                 dst;
  CJPEGColorComponent*   curr_comp;
  IppiEncodeHuffmanSpec* pDCTbl = 0;
  IppiEncodeHuffmanSpec* pACTbl = 0;
  JERRCODE               jerr;
  IppStatus              status;

  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();

  for(mcu = 0; mcu < m_numxMCU; mcu++)
  {
    // process restart interval, if any
    if(m_jpeg_restart_interval)
    {
      if(m_restarts_to_go == 0)
      {
        jerr = ProcessRestart(0,0,63,0,0);
        if(JPEG_OK != jerr)
          return jerr;
      }
    }

    for(c = 0; c < m_jpeg_ncomp; c++)
    {
      curr_comp = &m_ccomp[c];
      pDCTbl = m_dctbl[curr_comp->m_dc_selector];
      pACTbl = m_actbl[curr_comp->m_ac_selector];

      for(vs = 0; vs < curr_comp->m_vsampling; vs++)
      {
        for(hs = 0; hs < curr_comp->m_hsampling; hs++)
        {
          jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
          if(JPEG_OK != jerr)
            return jerr;

          currPos = m_BitStreamOut.GetCurrPos();

          status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
                     pMCUBuf,dst,dstLen,&currPos,
                     &curr_comp->m_lastDC,pDCTbl,pACTbl,m_state,0);

          m_BitStreamOut.SetCurrPos(currPos);

          if(ippStsNoErr > status)
          {
            LOG1("IPP Error: ippiEncodeHuffman8x8_JPEG_16s1u_C1() failed - ",status);
            return JPEG_ERR_INTERNAL;
          }

          pMCUBuf += DCTSIZE2;
        } // for m_hsampling
      } // for m_vsampling
    } // for m_jpeg_ncomp

    if(m_jpeg_restart_interval)
    {
      if(m_restarts_to_go == 0)
      {
        m_restarts_to_go = m_jpeg_restart_interval;
      }
      m_restarts_to_go --;
    }
  } // for numxMCU

  return JPEG_OK;
} // CJPEGEncoder::EncodeHuffmanMCURowBL()


JERRCODE CJPEGEncoder::EncodeHuffmanMCURowLS(Ipp16s* pMCUBuf)
{
  int                    c;
  int                    dstLen;
  int                    currPos;
  const Ipp16s*          src[4] = { 0, 0, 0, 0 };
  Ipp8u*                 dst;
  const IppiEncodeHuffmanSpec* pDCTbl[4] = { 0, 0, 0, 0 };
  IppStatus              status;
  JERRCODE               jerr;

  for(c = 0; c < m_jpeg_ncomp; c++)
  {
    src[c]    = pMCUBuf + c * m_numxMCU;
    pDCTbl[c] = m_dctbl[m_ccomp[c].m_dc_selector];
  }

  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();

  // process restart interval, if any
  if(m_jpeg_restart_interval)
  {
    if(m_restarts_to_go == 0)
    {
      jerr = ProcessRestart(0,0,63,0,0);
      if(JPEG_OK != jerr)
        return jerr;
    }
  }

  jerr = m_BitStreamOut.FlushBuffer();
  if(JPEG_OK != jerr)
    return jerr;

  currPos = m_BitStreamOut.GetCurrPos();

  status = ippiEncodeHuffmanRow_JPEG_16s1u_P4C1(
             src,m_numxMCU,m_jpeg_ncomp,dst,dstLen,&currPos,pDCTbl,m_state,0);

  m_BitStreamOut.SetCurrPos(currPos);

  if(ippStsNoErr > status)
  {
    LOG1("IPP Error: ippiEncodeHuffmanRow_JPEG_16s1u_P4C1() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  if(m_jpeg_restart_interval)
  {
    m_restarts_to_go -= m_numxMCU;
    if(m_restarts_to_go == 0)
    {
      m_rst_go = 1;
    }
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeHuffmanMCURowLS()


JERRCODE CJPEGEncoder::GenerateHuffmanTables(
  int ncomp,
  int id[MAX_COMPS_PER_SCAN],
  int Ss,
  int Se,
  int Ah,
  int Al)
{
  int  i;
  int  j;
  int  k;
  int  n;
  int  l;
  int  c;
  int  dc_statistics[2][256];
  int  ac_statistics[2][256];
  Ipp8u bits[16];
  Ipp8u vals[256];
  JERRCODE  jerr;
  IppStatus status;

  ippsZero_8u((Ipp8u*)dc_statistics,sizeof(dc_statistics));
  ippsZero_8u((Ipp8u*)ac_statistics,sizeof(ac_statistics));

  ippsZero_8u(bits,sizeof(bits));
  ippsZero_8u(vals,sizeof(vals));

  for(n = 0; n < m_jpeg_ncomp; n++)
  {
    m_ccomp[n].m_lastDC = 0;
  }

  m_next_restart_num = 0;
  m_restarts_to_go   = m_jpeg_restart_interval;

  status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  Ipp16s* block;

  if(Ss != 0 && Se != 0)
  {
    // AC scan
    for(i = 0; i < m_numyMCU; i++)
    {
      for(k = 0; k < m_ccomp[id[0]].m_vsampling; k++)
      {
        if(i*m_ccomp[id[0]].m_vsampling*8 + k*8 >= m_src.height)
          break;

        for(j = 0; j < m_numxMCU; j++)
        {
          block = m_block_buffer + (DCTSIZE2 * m_nblock * (j + (i * m_numxMCU)));

          // skip any relevant components
          for(c = 0; c < m_ccomp[id[0]].m_comp_no; c++)
          {
            block += (DCTSIZE2 * m_ccomp[c].m_hsampling *
                                 m_ccomp[c].m_vsampling);
          }

          // Skip over relevant 8x8 blocks from this component.
          block += (k * DCTSIZE2 * m_ccomp[id[0]].m_hsampling);

          for(l = 0; l < m_ccomp[id[0]].m_hsampling; l++)
          {
            if(m_jpeg_restart_interval)
            {
              if(m_restarts_to_go == 0)
              {
                jerr = ProcessRestart(ac_statistics,id,Ss,Se,Ah,Al);
                if(JPEG_OK != jerr)
                {
                  LOG0("Error: ProcessRestart() failed!");
                  return jerr;
                }
              }
            }

            // Ignore the last column(s) of the image.
            if(((j * m_ccomp[id[0]].m_hsampling * 8) + (l * 8)) >= m_src.width)
              break;

            if(Ah == 0)
            {
              status = ippiGetHuffmanStatistics8x8_ACFirst_JPEG_16s_C1(
                         block,&ac_statistics[m_ccomp[id[0]].m_ac_selector][0],
                         Ss,Se,Al,m_state,0);

              if(ippStsNoErr > status)
              {
                LOG0("Error: ippiGetHuffmanStatistics8x8_ACFirst_JPEG_16s_C1() failed!");
                return JPEG_ERR_INTERNAL;
              }
            }
            else
            {
              status = ippiGetHuffmanStatistics8x8_ACRefine_JPEG_16s_C1(
                         block,&ac_statistics[m_ccomp[id[0]].m_ac_selector][0],
                         Ss,Se,Al,m_state,0);

              if(ippStsNoErr > status)
              {
                LOG0("Error: ippiGetHuffmanStatistics8x8_ACRefine_JPEG_16s_C1() failed!");
                return JPEG_ERR_INTERNAL;
              }
            }

            block += DCTSIZE2;
            m_restarts_to_go --;
          } // for m_hsampling
        } // for m_numxMCU
      } // for m_vsampling
    } // for m_numyMCU

    if(Ah == 0)
    {
      status = ippiGetHuffmanStatistics8x8_ACFirst_JPEG_16s_C1(
                 0,ac_statistics[m_ccomp[id[0]].m_ac_selector],
                 Ss,Se,Al,m_state,1);

      if(ippStsNoErr > status)
      {
        LOG0("Error: ippiGetHuffmanStatistics8x8_ACFirst_JPEG_16s_C1() failed!");
        return JPEG_ERR_INTERNAL;
      }
    }
    else
    {
      status = ippiGetHuffmanStatistics8x8_ACRefine_JPEG_16s_C1(
                 0,ac_statistics[m_ccomp[id[0]].m_ac_selector],
                 Ss,Se,Al,m_state,1);

      if(ippStsNoErr > status)
      {
        LOG0("Error: ippiGetHuffmanStatistics8x8_ACRefine_JPEG_16s_C1() failed!");
        return JPEG_ERR_INTERNAL;
      }
    }

    status = ippiEncodeHuffmanRawTableInit_JPEG_8u(
               &ac_statistics[m_ccomp[id[0]].m_ac_selector][0],
               bits,vals);

    if(ippStsNoErr > status)
    {
      LOG0("Error: ippiEncodeHuffmanRawTableInit_JPEG_8u() failed!");
      return JPEG_ERR_INTERNAL;
    }

    jerr = m_actbl[m_ccomp[id[0]].m_ac_selector].Init(m_ccomp[id[0]].m_ac_selector,1,bits,vals);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: can't init huffman table");
      return jerr;
    }

    jerr = WriteDHT(&m_actbl[m_ccomp[id[0]].m_ac_selector]);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: WriteDHT() failed");
      return jerr;
    }
  }
  else
  {
    // DC scan
    if(Ah == 0)
    {
      for(i = 0; i < m_numyMCU; i++)
      {
        for(j = 0; j < m_numxMCU; j++)
        {
          if(m_jpeg_restart_interval)
          {
            if(m_restarts_to_go == 0)
            {
              jerr = ProcessRestart(dc_statistics,id,Ss,Se,Ah,Al);
              if(JPEG_OK != jerr)
              {
                LOG0("Error: ProcessRestart() failed!");
                return jerr;
              }
            }
          }

          block = m_block_buffer + (DCTSIZE2 * m_nblock * (j + (i * m_numxMCU)));

          // first DC scan
          for(n = 0; n < m_jpeg_ncomp; n++)
          {
            Ipp16s* lastDC = &m_ccomp[n].m_lastDC;

            for(k = 0; k < m_ccomp[n].m_vsampling; k++)
            {
              for(l = 0; l < m_ccomp[n].m_hsampling; l++)
              {
                status = ippiGetHuffmanStatistics8x8_DCFirst_JPEG_16s_C1(
                           block,dc_statistics[m_ccomp[n].m_dc_selector],
                           lastDC,Al);

                if(ippStsNoErr > status)
                {
                  LOG0("Error: ippiGetHuffmanStatistics8x8_DCFirst_JPEG_16s_C1() failed!");
                  return JPEG_ERR_INTERNAL;
                }

                block += DCTSIZE2;
              } // for m_hsampling
            } // for m_vsampling
          } // for m_jpeg_ncomp
          m_restarts_to_go --;
        } // for m_numxMCU
      } // for m_numyMCU

      for(n = 0; n < ncomp; n++)
      {
        status = ippiEncodeHuffmanRawTableInit_JPEG_8u(
                   dc_statistics[m_ccomp[n].m_dc_selector],
                   bits,vals);

        if(ippStsNoErr > status)
        {
          LOG0("Error: ippiEncodeHuffmanRawTableInit_JPEG_8u() failed!");
          return JPEG_ERR_INTERNAL;
        }

        jerr = m_dctbl[m_ccomp[n].m_dc_selector].Init(m_ccomp[n].m_dc_selector,0,bits,vals);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: can't init huffman table");
          return jerr;
        }

        jerr = WriteDHT(&m_dctbl[m_ccomp[n].m_dc_selector]);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: WriteDHT() failed");
          return jerr;
        }
      } // for ncomp
    } // Ah == 0
  }

  return JPEG_OK;
} // CJPEGEncoder::GenerateHuffmanTables()


JERRCODE CJPEGEncoder::GenerateHuffmanTables(void)
{
  int       i, j, c;
  int       huffStatistics[4][256];
  Ipp8u     bits[16];
  Ipp8u     vals[256];
  Ipp16s*   ptr;
  Ipp16s*   pMCUBuf;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif
  JERRCODE  jerr;
  IppStatus status;

  m_next_restart_num = 0;
  m_restarts_to_go   = m_jpeg_restart_interval;

  status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  ippsZero_8u((Ipp8u*)huffStatistics,sizeof(huffStatistics));

  for(i = 0; i < m_numyMCU; i++)
  {
    pMCUBuf = m_block_buffer + i * m_jpeg_ncomp * m_numxMCU;

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
#ifdef __TIMING__
    c0 = ippGetCpuClocks();
#endif
    jerr = TransformMCURowLS(pMCUBuf, i);
    if(JPEG_OK != jerr)
      return jerr;
#ifdef __TIMING__
    c1 = ippGetCpuClocks();
    m_clk_dct += c1 - c0;
#endif

    // process restart interval, if any
    if(m_jpeg_restart_interval)
    {
      if(m_restarts_to_go == 0)
      {
        jerr = ProcessRestart(0,0,63,0,0);
        if(JPEG_OK != jerr)
          return jerr;
      }
    }

    for(c = 0; c < m_jpeg_ncomp; c++)
    {
      ptr = pMCUBuf + c * m_numxMCU;
      for(j = 0; j < m_numxMCU; j++)
      {
        status = ippiGetHuffmanStatisticsOne_JPEG_16s_C1(
                   ptr, huffStatistics[c]);

        if(ippStsNoErr > status)
        {
          LOG1("IPP Error: ippiGetHuffmanStatisticsOne_JPEG_16s_C1() failed - ",status);
          return JPEG_ERR_INTERNAL;
        }

        ptr++;
      } // for numxMCU
    } // for m_jpeg_ncomp

    if(m_jpeg_restart_interval)
    {
      m_restarts_to_go -= m_numxMCU;
      if(m_restarts_to_go == 0)
      {
        m_rst_go = 1;
      }
    }
  } // for numyMCU

  for(c = 0; c < m_jpeg_ncomp; c++)
  {
    ippsZero_8u(bits,sizeof(bits));
    ippsZero_8u(vals,sizeof(vals));

    status = ippiEncodeHuffmanRawTableInit_JPEG_8u(huffStatistics[c],bits,vals);
    if(ippStsNoErr > status)
    {
      LOG0("Error: ippiEncodeHuffmanRawTableInit_JPEG_8u() failed!");
      return JPEG_ERR_INTERNAL;
    }

    jerr = AttachHuffmanTable(c,DC,c);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: WriteDHT() failed");
      return jerr;
    }

    jerr = InitHuffmanTable(bits, vals, m_ccomp[c].m_dc_selector, DC);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: can't init huffman table");
      return jerr;
    }

    jerr = WriteDHT(&m_dctbl[c]);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: WriteDHT() failed");
      return jerr;
    }
  } // for m_jpeg_ncomp

  return JPEG_OK;
} // CJPEGEncoder::GenerateHuffmanTables()


JERRCODE CJPEGEncoder::GenerateHuffmanTablesEX(void)
{
  int       i, j, c, cc, htbl_limit;
  int       vs;
  int       hs;
  int       dc_Statistics[4][256];
  int       ac_Statistics[4][256];
  Ipp8u     bits[16];
  Ipp8u     vals[256];
  Ipp16s    lastDC;
  Ipp16s*   pMCUBuf;
  CJPEGColorComponent*   curr_comp;
#ifdef __TIMING__
  Ipp64u   c0 = 0;
  Ipp64u   c1 = 0;
#endif
  JERRCODE  jerr;
  IppStatus status;

  m_next_restart_num = 0;
  m_restarts_to_go   = m_jpeg_restart_interval;

  status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  ippsZero_8u((Ipp8u*)dc_Statistics,sizeof(dc_Statistics));
  ippsZero_8u((Ipp8u*)ac_Statistics,sizeof(ac_Statistics));

  lastDC = 0;

  for(i = 0; i < m_numyMCU; i++)
  {
    pMCUBuf = m_block_buffer + i * m_numxMCU * m_nblock * DCTSIZE2;
    if(JD_PIXEL == m_src.order)
    {
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
#ifdef __TIMING__
      c0 = ippGetCpuClocks();
#endif
      jerr = DownSampling(i);
      if(JPEG_OK != jerr)
        return jerr;
#ifdef __TIMING__
      c1 = ippGetCpuClocks();
      m_clk_ss += c1 - c0;
#endif
    }
    else // m_src.order == JD_PLANE
    {
      jerr = ProcessBuffer(i);
      if(JPEG_OK != jerr)
        return jerr;
    }

#ifdef __TIMING__
      c0 = ippGetCpuClocks();
#endif
    if(JPEG_BASELINE == m_jpeg_mode)
    {
      jerr = TransformMCURowBL(pMCUBuf, 0);
    }
    else
    {
      if(m_jpeg_precision > 8)
        jerr = TransformMCURowEX(pMCUBuf, 0);
      else
        jerr = TransformMCURowBL(pMCUBuf, 0);
    }

    if(JPEG_OK != jerr)
      return jerr;
#ifdef __TIMING__
    c1 = ippGetCpuClocks();
    m_clk_dct += c1 - c0;
#endif

    for(j = 0; j < m_numxMCU; j++)
    {
      // process restart interval, if any
      if(m_jpeg_restart_interval)
      {
        if(m_restarts_to_go == 0)
        {
          jerr = ProcessRestart(0,0,0,63,0,0);
          if(JPEG_OK != jerr)
            return jerr;
        }
      }

      for(c = 0; c < m_jpeg_ncomp; c++)
      {
        cc = c;
        if(JPEG_BASELINE == m_jpeg_mode)
        {
          if(m_jpeg_ncomp > 1 && (m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK))
            cc = !!c;
          else
            cc = 0;
        }


        curr_comp = &m_ccomp[c];
        for(vs = 0; vs < curr_comp->m_vsampling; vs++)
        {
          for(hs = 0; hs < curr_comp->m_hsampling; hs++)
          {
            status = ippiGetHuffmanStatistics8x8_JPEG_16s_C1(
                       pMCUBuf, dc_Statistics[cc], ac_Statistics[cc], &m_ccomp[c].m_lastDC);

          if(ippStsNoErr > status)
          {
            LOG1("IPP Error: ippiGetHuffmanStatistics8x8_JPEG_16s_C1() failed - ",status);
            return JPEG_ERR_INTERNAL;
          }

          pMCUBuf += DCTSIZE2;
          }
        }
      } // for m_jpeg_ncomp

      if(m_jpeg_restart_interval)
      {
        if(m_restarts_to_go == 0)
        {
          m_restarts_to_go = m_jpeg_restart_interval;
        }
        m_restarts_to_go --;
      }
    } // for numxMCU
  } // for numyMCU



  for(c = 0; c < m_jpeg_ncomp; c++)
  {
    cc = c;
    if(JPEG_BASELINE == m_jpeg_mode)
    {
      if(m_jpeg_ncomp > 1 && (m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK))
        cc = !!c;
      else
        cc = 0;
    }

    ippsZero_8u(bits,sizeof(bits));
    ippsZero_8u(vals,sizeof(vals));

    status = ippiEncodeHuffmanRawTableInit_JPEG_8u(dc_Statistics[cc],bits,vals);
    if(ippStsNoErr > status)
    {
      LOG0("Error: ippiEncodeHuffmanRawTableInit_JPEG_8u() failed!");
      return JPEG_ERR_INTERNAL;
    }

    jerr = AttachHuffmanTable(cc, DC, c);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = InitHuffmanTable(bits, vals, m_ccomp[c].m_dc_selector, DC);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: can't init huffman table");
      return jerr;
    }

    ippsZero_8u(bits,sizeof(bits));
    ippsZero_8u(vals,sizeof(vals));

    status = ippiEncodeHuffmanRawTableInit_JPEG_8u(ac_Statistics[cc],bits,vals);
    if(ippStsNoErr > status)
    {
      LOG0("Error: ippiEncodeHuffmanRawTableInit_JPEG_8u() failed!");
      return JPEG_ERR_INTERNAL;
    }

    jerr = AttachHuffmanTable(cc, AC, c);
    if(JPEG_OK != jerr)
      return jerr;

    jerr = InitHuffmanTable(bits, vals, m_ccomp[c].m_ac_selector, AC);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: can't init huffman table");
      return jerr;
    }

    if(JPEG_BASELINE == m_jpeg_mode)
    {
      if(m_jpeg_ncomp > 1 && (m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK))
        htbl_limit = 2;
      else
        htbl_limit = 1;

      if(c < htbl_limit)
      {
        jerr = WriteDHT(&m_dctbl[cc]);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: WriteDHT() failed");
          return jerr;
        }

        jerr = WriteDHT(&m_actbl[cc]);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: WriteDHT() failed");
          return jerr;
        }
      }
    }
    else
    {
      jerr = WriteDHT(&m_dctbl[c]);
      if(JPEG_OK != jerr)
      {
        LOG0("Error: WriteDHT() failed");
        return jerr;
      }

      jerr = WriteDHT(&m_actbl[c]);
      if(JPEG_OK != jerr)
      {
        LOG0("Error: WriteDHT() failed");
        return jerr;
      }
    }
  }

  return JPEG_OK;
} // CJPEGEncoder::GenerateHuffmanTablesEX()


JERRCODE CJPEGEncoder::EncodeScan(
  int ncomp,
  int id[MAX_COMPS_PER_SCAN],
  int Ss,
  int Se,
  int Ah,
  int Al)
{
  int  i;
  int  j;
  int  k;
  int  n;
  int  l;
  int  c;
  int  dstLen;
  int  currPos;
  Ipp8u*    dst;
  Ipp16s*   block;
  JERRCODE  jerr;
  IppStatus status;

  m_next_restart_num = 0;
  m_restarts_to_go   = m_jpeg_restart_interval;

  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();

  for(n = 0; n < m_jpeg_ncomp; n++)
  {
    m_ccomp[n].m_lastDC = 0;
  }

  status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  jerr = WriteSOS(ncomp,id,Ss,Se,Ah,Al);
  if(JPEG_OK != jerr)
  {
    LOG0("Error: WriteSOS() failed");
    return jerr;
  }

  if(Ss != 0 && Se != 0)
  {
    // AC scan
    for(i = 0; i < m_numyMCU; i++)
    {
      for(k = 0; k < m_ccomp[id[0]].m_vsampling; k++)
      {
        if(i*m_ccomp[id[0]].m_vsampling*8 + k*8 >= m_src.height)
          break;

        for(j = 0; j < m_numxMCU; j++)
        {
          block = m_block_buffer + (DCTSIZE2 * m_nblock * (j + (i * m_numxMCU)));

          // skip any relevant components
          for(c = 0; c < m_ccomp[id[0]].m_comp_no; c++)
          {
            block += (DCTSIZE2 * m_ccomp[c].m_hsampling *
                                 m_ccomp[c].m_vsampling);
          }

          // Skip over relevant 8x8 blocks from this component.
          block += (k * DCTSIZE2 * m_ccomp[id[0]].m_hsampling);

          for(l = 0; l < m_ccomp[id[0]].m_hsampling; l++)
          {
            // Ignore the last column(s) of the image.
            if(((j * m_ccomp[id[0]].m_hsampling * 8) + (l * 8)) >= m_src.width)
              break;

            if(m_jpeg_restart_interval)
            {
              if(m_restarts_to_go == 0)
              {
                jerr = ProcessRestart(id,Ss,Se,Ah,Al);
                if(JPEG_OK != jerr)
                {
                  LOG0("Error: ProcessRestart() failed!");
                  return jerr;
                }
              }
            }

            IppiEncodeHuffmanSpec* actbl = m_actbl[m_ccomp[id[0]].m_ac_selector];

            if(Ah == 0)
            {
              jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
              if(JPEG_OK != jerr)
                return jerr;

              currPos = m_BitStreamOut.GetCurrPos();

              status = ippiEncodeHuffman8x8_ACFirst_JPEG_16s1u_C1(
                         block,dst,dstLen,&currPos,Ss,Se,
                         Al,actbl,m_state,0);

              m_BitStreamOut.SetCurrPos(currPos);

              if(ippStsNoErr > status)
              {
                LOG1("Error: ippiEncodeHuffman8x8_ACFirst_JPEG_16s1u_C1() failed!",ippGetStatusString(status));
                return JPEG_ERR_INTERNAL;
              }
            }
            else
            {
              jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
              if(JPEG_OK != jerr)
                return jerr;

              currPos = m_BitStreamOut.GetCurrPos();

              status = ippiEncodeHuffman8x8_ACRefine_JPEG_16s1u_C1(
                         block,dst,dstLen,&currPos,Ss,Se,
                         Al,actbl,m_state,0);

              m_BitStreamOut.SetCurrPos(currPos);

              if(ippStsNoErr > status)
              {
                LOG1("Error: ippiEncodeHuffman8x8_ACRefine_JPEG_16s1u_C1() failed!",ippGetStatusString(status));
                return JPEG_ERR_INTERNAL;
              }
            }

            block += DCTSIZE2;

            m_restarts_to_go --;
          } // for m_hsampling
        } // for m_numxMCU
      } // for m_vsampling
    } // for m_numyMCU

    IppiEncodeHuffmanSpec* actbl = m_actbl[m_ccomp[id[0]].m_ac_selector];

    if(Ah == 0)
    {
      jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
      if(JPEG_OK != jerr)
        return jerr;

      currPos = m_BitStreamOut.GetCurrPos();

      status = ippiEncodeHuffman8x8_ACFirst_JPEG_16s1u_C1(
                 0,dst,dstLen,&currPos,Ss,Se,
                 Al,actbl,m_state,1);

      m_BitStreamOut.SetCurrPos(currPos);

      if(ippStsNoErr > status)
      {
        LOG0("Error: ippiEncodeHuffman8x8_ACFirst_JPEG_16s1u_C1() failed!");
        return JPEG_ERR_INTERNAL;
      }
    }
    else
    {
      jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
      if(JPEG_OK != jerr)
        return jerr;

      currPos = m_BitStreamOut.GetCurrPos();

      status = ippiEncodeHuffman8x8_ACRefine_JPEG_16s1u_C1(
                 0,dst,dstLen,&currPos,Ss,Se,
                 Al,actbl,m_state,1);

      m_BitStreamOut.SetCurrPos(currPos);

      if(ippStsNoErr > status)
      {
        LOG0("Error: ippiEncodeHuffman8x8_ACRefine_JPEG_16s1u_C1() failed!");
        return JPEG_ERR_INTERNAL;
      }
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
            jerr = ProcessRestart(id,Ss,Se,Ah,Al);
            if(JPEG_OK != jerr)
            {
              LOG0("Error: ProcessRestart() failed!");
              return jerr;
            }
          }
        }

        block = m_block_buffer + (DCTSIZE2 * m_nblock * (j + (i * m_numxMCU)));

        if(Ah == 0)
        {
          // first DC scan
          for(n = 0; n < m_jpeg_ncomp; n++)
          {
            Ipp16s* lastDC = &m_ccomp[n].m_lastDC;
            IppiEncodeHuffmanSpec* dctbl = m_dctbl[m_ccomp[n].m_dc_selector];

            for(k = 0; k < m_ccomp[n].m_vsampling; k++)
            {
              for(l = 0; l < m_ccomp[n].m_hsampling; l++)
              {
                jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
                if(JPEG_OK != jerr)
                  return jerr;

                currPos = m_BitStreamOut.GetCurrPos();

                status = ippiEncodeHuffman8x8_DCFirst_JPEG_16s1u_C1(
                           block,dst,dstLen,&currPos,
                           lastDC,Al,dctbl,m_state,0);

                m_BitStreamOut.SetCurrPos(currPos);

                if(ippStsNoErr > status)
                {
                  LOG1("Error: ippiEncodeHuffman8x8_DCFirst_JPEG_16s1u_C1() failed!",ippGetStatusString(status));
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
                jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
                if(JPEG_OK != jerr)
                  return jerr;

                currPos = m_BitStreamOut.GetCurrPos();

                status = ippiEncodeHuffman8x8_DCRefine_JPEG_16s1u_C1(
                           block,dst,dstLen,&currPos,
                           Al,m_state,0);

                m_BitStreamOut.SetCurrPos(currPos);

                if(ippStsNoErr > status)
                {
                  LOG0("Error: ippiEncodeHuffman8x8_DCRefine_JPEG_16s1u_C1() failed!");
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

    if(Ah == 0)
    {
      jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
      if(JPEG_OK != jerr)
        return jerr;

      currPos = m_BitStreamOut.GetCurrPos();

      status = ippiEncodeHuffman8x8_DCFirst_JPEG_16s1u_C1(
                 0,dst,dstLen,&currPos,0,0,0,m_state,1);

      m_BitStreamOut.SetCurrPos(currPos);

      if(ippStsNoErr > status)
      {
        LOG0("Error: ippiEncodeHuffman8x8_DCFirst_JPEG_16s1u_C1() failed!");
        return JPEG_ERR_INTERNAL;
      }
    }
    else
    {
      jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
      if(JPEG_OK != jerr)
        return jerr;

      currPos = m_BitStreamOut.GetCurrPos();

      status = ippiEncodeHuffman8x8_DCRefine_JPEG_16s1u_C1(
                 0,dst,dstLen,&currPos,0,m_state,1);

      m_BitStreamOut.SetCurrPos(currPos);

      if(ippStsNoErr > status)
      {
        LOG0("Error: ippiEncodeHuffman8x8_DCRefine_JPEG_16s1u_C1() failed!");
        return JPEG_ERR_INTERNAL;
      }
    }
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeScan()


JERRCODE CJPEGEncoder::EncodeScanBaseline(void)
{
  int i;
  int dstLen;
  int currPos;
  Ipp8u* dst;
#ifdef _OPENMP
  omp_lock_t* locks;
#endif
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif
  JERRCODE  jerr;
  IppStatus status;

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    m_ccomp[i].m_lastDC = 0;
  }

  m_next_restart_num = 0;
  m_restarts_to_go   = m_jpeg_restart_interval;

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
  locks = (omp_lock_t*)ippMalloc(sizeof(omp_lock_t) * m_numyMCU);
  for(i = 0; i < m_numyMCU; i++)
  {
    omp_init_lock(&locks[i]);
  }
#endif

  i = 0;

#ifdef _OPENMP
#pragma omp parallel shared(i, locks)
#endif
  {
    int     curr_row  = 0;
    int     thread_id = 0;
    Ipp16s* pMCUBuf   = 0;  // the pointer to Buffer for a current thread.

#ifdef _OPENMP
    thread_id = omp_get_thread_num(); // the thread id of the calling thread.
#endif

    pMCUBuf = m_block_buffer + thread_id * m_numxMCU * m_nblock * DCTSIZE2;

    while(curr_row < m_numyMCU)
    {
#ifdef _OPENMP
#pragma omp critical
    {
#endif
      curr_row = i;
      i++;
#ifdef _OPENMP
      if(curr_row < m_numyMCU)
        omp_set_lock(&locks[curr_row]);
    }
#endif
      if(curr_row < m_numyMCU)
      {
#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        jerr = ColorConvert(curr_row,thread_id);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_cc += c1 - c0;
#endif

#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        DownSampling(curr_row,thread_id);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_ss += c1 - c0;
#endif

#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        TransformMCURowBL(pMCUBuf, thread_id);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_dct += c1 - c0;
#endif


#ifdef _OPENMP
        if(curr_row > 0)
        {
          omp_set_lock(&locks[curr_row-1]);
          omp_unset_lock(&locks[curr_row-1]);
        }
#endif
#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        EncodeHuffmanMCURowBL(pMCUBuf);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_huff += (c1 - c0);
#endif
#ifdef _OPENMP
        omp_unset_lock(&locks[curr_row]);
#endif
      }

      curr_row++;
    } // for m_numyMCU
  } // OMP

#ifdef _OPENMP
  for(i = 0; i < m_numyMCU; i++)
  {
    omp_destroy_lock(&locks[i]);
  }

  ippFree(locks);
  locks = 0;
#endif

  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();

  jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
  if(JPEG_OK != jerr)
    return jerr;

  currPos = m_BitStreamOut.GetCurrPos();

  // flush IppiEncodeHuffmanState
  status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
             0,dst,dstLen,&currPos,0,0,0,m_state,1);

  m_BitStreamOut.SetCurrPos(currPos);

  if(ippStsNoErr > status)
  {
    LOG1("IPP Error: ippiEncodeHuffman8x8_JPEG_16s1u_C1() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeScanBaseline()


JERRCODE CJPEGEncoder::EncodeScanBaseline_P(void)
{
  int i;
  int dstLen;
  int currPos;
  Ipp8u* dst;
#ifdef _OPENMP
  omp_lock_t* locks;
#endif
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif
  JERRCODE  jerr;
  IppStatus status;

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    m_ccomp[i].m_lastDC = 0;
  }

  m_next_restart_num = 0;
  m_restarts_to_go   = m_jpeg_restart_interval;

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
  locks = (omp_lock_t*)ippMalloc(sizeof(omp_lock_t) * m_numyMCU);
  for(i = 0; i < m_numyMCU; i++)
  {
    omp_init_lock(&locks[i]);
  }
#endif

  i = 0;

#ifdef _OPENMP
#pragma omp parallel shared(i, locks)
#endif
  {
    int     curr_row  = 0;
    int     thread_id = 0;
    Ipp16s* pMCUBuf   = 0;  // the pointer to Buffer for a current thread.

    curr_row = i;

#ifdef _OPENMP
    thread_id = omp_get_thread_num(); // the thread id of the calling thread.
#endif

    pMCUBuf = m_block_buffer + thread_id * m_numxMCU * m_nblock * DCTSIZE2;

    while(curr_row < m_numyMCU)
    {
#ifdef _OPENMP
#pragma omp critical
    {
#endif
      curr_row = i;
      i++;
#ifdef _OPENMP
      if(curr_row < m_numyMCU)
        omp_set_lock(&locks[curr_row]);
    }
#endif

      if(curr_row < m_numyMCU)
      {
#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        if(m_src.color == m_jpeg_color)
        {
          jerr = ProcessBuffer(curr_row,thread_id);
        }
        else
        {
          jerr = ColorConvert(curr_row,thread_id);

          jerr = DownSampling(curr_row,thread_id);
        }
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_cc += c1 - c0;
#endif

#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        TransformMCURowBL(pMCUBuf, thread_id);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_dct += c1 - c0;
#endif

#ifdef _OPENMP
        if(curr_row > 0)
        {
          omp_set_lock(&locks[curr_row-1]);
          omp_unset_lock(&locks[curr_row-1]);
        }
#endif
#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        EncodeHuffmanMCURowBL(pMCUBuf);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_huff += (c1 - c0);
#endif
#ifdef _OPENMP
        omp_unset_lock(&locks[curr_row]);
#endif
      }

      curr_row++;
    } // for m_numyMCU
  } // OMP

#ifdef _OPENMP
  for(i = 0; i < m_numyMCU; i++)
  {
    omp_destroy_lock(&locks[i]);
  }

  ippFree(locks);
  locks = 0;
#endif

  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();

  jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
  if(JPEG_OK != jerr)
    return jerr;

  currPos = m_BitStreamOut.GetCurrPos();

  // flush IppiEncodeHuffmanState
  status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
             0,dst,dstLen,&currPos,0,0,0,m_state,1);

  m_BitStreamOut.SetCurrPos(currPos);

  if(ippStsNoErr > status)
  {
    LOG1("IPP Error: ippiEncodeHuffman8x8_JPEG_16s1u_C1() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeScanBaseline_P()


JERRCODE CJPEGEncoder::EncodeScanExtended(void)
{
  int i;
  int dstLen;
  int currPos;
  Ipp8u* dst;
#ifdef _OPENMP
  omp_lock_t* locks;
#endif
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif
  JERRCODE  jerr;
  IppStatus status;

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    m_ccomp[i].m_lastDC = 0;
  }

  m_next_restart_num = 0;
  m_restarts_to_go   = m_jpeg_restart_interval;

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

  if(!m_optimal_htbl)
  {
#ifdef _OPENMP
  locks = (omp_lock_t*)ippMalloc(sizeof(omp_lock_t) * m_numyMCU);
  for(i = 0; i < m_numyMCU; i++)
  {
    omp_init_lock(&locks[i]);
  }
#endif

  i = 0;

#ifdef _OPENMP
#pragma omp parallel shared(i, locks)
#endif
  {
    int     curr_row  = 0;
    int     thread_id = 0;
    Ipp16s* pMCUBuf   = 0;  // the pointer to Buffer for a current thread.

    curr_row = i;

#ifdef _OPENMP
    thread_id = omp_get_thread_num(); // the thread id of the calling thread.
#endif

    pMCUBuf = m_block_buffer + thread_id * m_numxMCU * m_nblock * DCTSIZE2;

    while(curr_row < m_numyMCU)
    {
#ifdef _OPENMP
#pragma omp critical
    {
#endif
      curr_row = i;
      i++;
#ifdef _OPENMP
      if(curr_row < m_numyMCU)
        omp_set_lock(&locks[curr_row]);
    }
#endif
      if(curr_row < m_numyMCU)
      {
#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        ColorConvert(curr_row,thread_id);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_cc += c1 - c0;
#endif

#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        DownSampling(curr_row,thread_id);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_ss += c1 - c0;
#endif

#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        TransformMCURowEX(pMCUBuf, thread_id);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_dct += c1 - c0;
#endif

#ifdef _OPENMP
        if(curr_row > 0)
        {
          omp_set_lock(&locks[curr_row-1]);
          omp_unset_lock(&locks[curr_row-1]);
        }
#endif
#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        EncodeHuffmanMCURowBL(pMCUBuf);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_huff += (c1 - c0);
#endif
#ifdef _OPENMP
        omp_unset_lock(&locks[curr_row]);
#endif
      }

      curr_row++;
    } // for m_numyMCU
  } // OMP

#ifdef _OPENMP
  for(i = 0; i < m_numyMCU; i++)
  {
    omp_destroy_lock(&locks[i]);
  }

  ippFree(locks);
  locks = 0;
#endif
  }
  else
  {
    Ipp16s* mcurow;
    for(i = 0; i < m_numyMCU; i++)
    {
      mcurow = m_block_buffer + i * m_numxMCU * m_nblock * DCTSIZE2;
#ifdef __TIMING__
      c0 = ippGetCpuClocks();
#endif
      jerr = EncodeHuffmanMCURowBL(mcurow);
      if(JPEG_OK != jerr)
        return jerr;
#ifdef __TIMING__
      c1 = ippGetCpuClocks();
      m_clk_huff += (c1 - c0);
#endif
    }
  }

  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();

  jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
  if(JPEG_OK != jerr)
    return jerr;

  currPos = m_BitStreamOut.GetCurrPos();

  // flush IppiEncodeHuffmanState
  status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
             0,dst,dstLen,&currPos,0,0,0,m_state,1);

  m_BitStreamOut.SetCurrPos(currPos);

  if(ippStsNoErr > status)
  {
    LOG1("IPP Error: ippiEncodeHuffman8x8_JPEG_16s1u_C1() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeScanExtended()


JERRCODE CJPEGEncoder::EncodeScanExtended_P(void)
{
  int i;
  int dstLen;
  int currPos;
  Ipp8u* dst;
#ifdef _OPENMP
  omp_lock_t* locks;
#endif
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif
  JERRCODE  jerr;
  IppStatus status;

  for(i = 0; i < m_jpeg_ncomp; i++)
  {
    m_ccomp[i].m_lastDC = 0;
  }

  m_next_restart_num = 0;
  m_restarts_to_go   = m_jpeg_restart_interval;

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

  if(!m_optimal_htbl)
  {
#ifdef _OPENMP
  locks = (omp_lock_t*)ippMalloc(sizeof(omp_lock_t) * m_numyMCU);
  for(i = 0; i < m_numyMCU; i++)
  {
    omp_init_lock(&locks[i]);
  }
#endif

  i = 0;

#ifdef _OPENMP
#pragma omp parallel shared(i)
#endif
  {
    int     curr_row  = 0;
    int     thread_id = 0;
    Ipp16s* pMCUBuf   = 0;  // the pointer to Buffer for a current thread.

    curr_row = i;

#ifdef _OPENMP
    thread_id = omp_get_thread_num(); // the thread id of the calling thread.
#endif

    pMCUBuf = m_block_buffer + thread_id * m_numxMCU * m_nblock * DCTSIZE2;

    while(curr_row < m_numyMCU)
    {
#ifdef _OPENMP
#pragma omp critical
    {
#endif
      curr_row = i;
      i++;
#ifdef _OPENMP

      if(curr_row < m_numyMCU)
        omp_set_lock(&locks[curr_row]);
    }
#endif
      if(curr_row < m_numyMCU)
      {
#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        ProcessBuffer(curr_row,thread_id);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_cc += c1 - c0;
#endif

#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        TransformMCURowEX(pMCUBuf, thread_id);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_dct += c1 - c0;
#endif

#ifdef _OPENMP
        if(curr_row > 0)
        {
          omp_set_lock(&locks[curr_row-1]);
          omp_unset_lock(&locks[curr_row-1]);
        }
#endif
#ifdef __TIMING__
        c0 = ippGetCpuClocks();
#endif
        EncodeHuffmanMCURowBL(pMCUBuf);
#ifdef __TIMING__
        c1 = ippGetCpuClocks();
        m_clk_huff += (c1 - c0);
#endif
#ifdef _OPENMP
        omp_unset_lock(&locks[curr_row]);
#endif
      }

      curr_row++;
    } // for m_numyMCU
  } // OMP

#ifdef _OPENMP
  for(i = 0; i < m_numyMCU; i++)
  {
    omp_destroy_lock(&locks[i]);
  }

  ippFree(locks);
  locks = 0;
#endif
  }
  else
  {
    Ipp16s* mcurow;
    for(i = 0; i < m_numyMCU; i++)
    {
      mcurow = m_block_buffer + i * m_numxMCU * m_nblock * DCTSIZE2;
#ifdef __TIMING__
      c0 = ippGetCpuClocks();
#endif
      jerr = EncodeHuffmanMCURowBL(mcurow);
      if(JPEG_OK != jerr)
        return jerr;
#ifdef __TIMING__
      c1 = ippGetCpuClocks();
      m_clk_huff += (c1 - c0);
#endif
    }
  }

  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();

  jerr = m_BitStreamOut.FlushBuffer(SAFE_NBYTES);
  if(JPEG_OK != jerr)
    return jerr;

  currPos = m_BitStreamOut.GetCurrPos();

  // flush IppiEncodeHuffmanState
  status = ippiEncodeHuffman8x8_JPEG_16s1u_C1(
             0,dst,dstLen,&currPos,0,0,0,m_state,1);

  m_BitStreamOut.SetCurrPos(currPos);

  if(ippStsNoErr > status)
  {
    LOG1("IPP Error: ippiEncodeHuffman8x8_JPEG_16s1u_C1() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeScanExtended_P()


JERRCODE CJPEGEncoder::EncodeScanProgressive(void)
{
  int      i;
  Ipp16s*  pMCUBuf;
  JERRCODE jerr;

  for(i = 0; i < m_numyMCU; i++)
  {
    jerr = ColorConvert(i);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: ColorConvert() failed");
      return jerr;
    }

    jerr = DownSampling(i);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: DownSampling() failed");
      return jerr;
    }

    pMCUBuf = m_block_buffer + (i * m_numxMCU * DCTSIZE2 * m_nblock);

    jerr = TransformMCURowBL(pMCUBuf);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: PerformDCT() failed");
      return jerr;
    }
  }

  for(i = 0; i < m_scan_count; i++)
  {
    jerr = GenerateHuffmanTables(
             m_scan_script[i].ncomp,
             m_scan_script[i].id,
             m_scan_script[i].Ss,
             m_scan_script[i].Se,
             m_scan_script[i].Ah,
             m_scan_script[i].Al);

    if(JPEG_OK != jerr)
    {
      LOG0("Error: GenerateHuffmanTables() failed");
      return jerr;
    }

    jerr = EncodeScan(
             m_scan_script[i].ncomp,
             m_scan_script[i].id,
             m_scan_script[i].Ss,
             m_scan_script[i].Se,
             m_scan_script[i].Ah,
             m_scan_script[i].Al);

    if(JPEG_OK != jerr)
    {
      LOG0("Error: EncodeScan() failed");
      return jerr;
    }
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeScanProgressive()


JERRCODE CJPEGEncoder::EncodeScanProgressive_P(void)
{
  int      i;
  Ipp16s*  pMCUBuf;
  JERRCODE jerr;

  for(i = 0; i < m_numyMCU; i++)
  {
    jerr = ProcessBuffer(i);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: ProcessBuffer() failed");
      return jerr;
    }

    pMCUBuf = m_block_buffer + (i * m_numxMCU * DCTSIZE2 * m_nblock);

    jerr = TransformMCURowBL(pMCUBuf);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: PerformDCT() failed");
      return jerr;
    }
  }

  for(i = 0; i < m_scan_count; i++)
  {
    jerr = GenerateHuffmanTables(
             m_scan_script[i].ncomp,
             m_scan_script[i].id,
             m_scan_script[i].Ss,
             m_scan_script[i].Se,
             m_scan_script[i].Ah,
             m_scan_script[i].Al);

    if(JPEG_OK != jerr)
    {
      LOG0("Error: GenerateHuffmanTables() failed");
      return jerr;
    }

    jerr = EncodeScan(
             m_scan_script[i].ncomp,
             m_scan_script[i].id,
             m_scan_script[i].Ss,
             m_scan_script[i].Se,
             m_scan_script[i].Ah,
             m_scan_script[i].Al);

    if(JPEG_OK != jerr)
    {
      LOG0("Error: EncodeScan() failed");
      return jerr;
    }
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeScanProgressive_P()


JERRCODE CJPEGEncoder::EncodeScanLossless(void)
{
  int         i;
  int         dstLen;
  int         currPos;
  Ipp8u*      dst;
  Ipp16s*     pMCUBuf;
#ifdef __TIMING__
  Ipp64u   c0;
  Ipp64u   c1;
#endif
  IppStatus   status;
  JERRCODE    jerr;

  m_next_restart_num = 0;
  m_restarts_to_go   = m_jpeg_restart_interval;

  status = ippiEncodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  m_ss = m_predictor;
  m_se = 0;
  m_ah = 0;
  m_al = m_pt;

  jerr = WriteSOS();
  if(JPEG_OK != jerr)
  {
    LOG0("Error: WriteSOS() failed");
    return jerr;
  }

  if(!m_optimal_htbl)
  {
    for(i = 0; i < m_numyMCU; i++)
    {
      pMCUBuf  = m_block_buffer;
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

#ifdef __TIMING__
      c0 = ippGetCpuClocks();
#endif
      jerr = TransformMCURowLS(pMCUBuf, i);
      if(JPEG_OK != jerr)
        return jerr;
#ifdef __TIMING__
      c1 = ippGetCpuClocks();
      m_clk_dct += c1 - c0;
#endif

#ifdef __TIMING__
      c0 = ippGetCpuClocks();
#endif
      jerr = EncodeHuffmanMCURowLS(pMCUBuf);
      if(JPEG_OK != jerr)
        return jerr;
#ifdef __TIMING__
      c1 = ippGetCpuClocks();
      m_clk_huff += (c1 - c0);
#endif
    } // for m_numyMCU
  }
  else
  {
#ifdef __TIMING__
      c0 = ippGetCpuClocks();
#endif
    for(i = 0; i < m_numyMCU; i++)
    {
      pMCUBuf  = m_block_buffer + i * m_jpeg_ncomp * m_numxMCU;

      jerr = EncodeHuffmanMCURowLS(pMCUBuf);
      if(JPEG_OK != jerr)
        return jerr;
    } // for m_numyMCU
#ifdef __TIMING__
      c1 = ippGetCpuClocks();
      m_clk_huff += (c1 - c0);
#endif
  }

  jerr = m_BitStreamOut.FlushBuffer();
  if(JPEG_OK != jerr)
    return jerr;

  dst    = m_BitStreamOut.GetDataPtr();
  dstLen = m_BitStreamOut.GetDataLen();

  currPos = m_BitStreamOut.GetCurrPos();

  // flush IppiEncodeHuffmanState
  status = ippiEncodeHuffmanOne_JPEG_16s1u_C1(
             0,dst,dstLen,&currPos,0,m_state,1);

  m_BitStreamOut.SetCurrPos(currPos);

  if(ippStsNoErr > status)
  {
    LOG1("IPP Error: ippiEncodeHuffmanOne_JPEG_16s1u_C1() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  return JPEG_OK;
} // CJPEGEncoder::EncodeScanLossless()


JERRCODE CJPEGEncoder::WriteHeader(void)
{
  JERRCODE jerr;

  jerr = Init();
  if(JPEG_OK != jerr)
  {
    LOG0("Error: can't init encoder");
    return jerr;
  }

  jerr = WriteSOI();
  if(JPEG_OK != jerr)
  {
    LOG0("Error: WriteSOI() failed");
    return jerr;
  }

  if(m_jpeg_mode != JPEG_LOSSLESS)
  {
    switch(m_jpeg_color)
    {
      case JC_GRAY:
      case JC_YCBCR:
        jerr = WriteAPP0();
        if(JPEG_OK != jerr)
        {
          LOG0("Error: WriteAPP0() failed");
          return jerr;
        }
        break;

      case JC_RGB:
      case JC_CMYK:
      case JC_YCCK:
        jerr = WriteAPP14();
        if(JPEG_OK != jerr)
        {
          LOG0("Error: WriteAPP14() failed");
          return jerr;
        }
        break;

      default:
        break;
    }
  }

  jerr = WriteCOM(m_jpeg_comment);
  if(JPEG_OK != jerr)
  {
    LOG0("Error: WriteCOM() failed");
    return jerr;
  }

  if(m_jpeg_mode != JPEG_LOSSLESS)
  {
    jerr = WriteDQT(&m_qntbl[0]);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: WriteDQT() failed");
      return jerr;
    }

    if(m_jpeg_ncomp != 1 && (m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK))
    {
      jerr = WriteDQT(&m_qntbl[1]);
      if(JPEG_OK != jerr)
      {
        LOG0("Error: WriteDQT() failed");
        return jerr;
      }
    }
  }

  switch(m_jpeg_mode)
  {
  case JPEG_BASELINE:    jerr = WriteSOF0(); break;
  case JPEG_EXTENDED:    jerr = WriteSOF1(); break;
  case JPEG_PROGRESSIVE: jerr = WriteSOF2(); break;
  case JPEG_LOSSLESS:    jerr = WriteSOF3(); break;
  default:
    return JPEG_ERR_PARAMS;
  }

  if(JPEG_OK != jerr)
  {
    LOG0("Error: WriteSOFx() failed");
    return jerr;
  }

  switch(m_jpeg_mode)
  {
  case JPEG_BASELINE:
    if(!m_optimal_htbl)
    {
      jerr = WriteDHT(&m_dctbl[0]);
      if(JPEG_OK != jerr)
      {
        LOG0("Error: WriteDHT() failed");
        return jerr;
      }

      jerr = WriteDHT(&m_actbl[0]);
      if(JPEG_OK != jerr)
      {
        LOG0("Error: WriteDHT() failed");
        return jerr;
      }

      if(m_jpeg_ncomp != 1 && (m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK))
      {
        jerr = WriteDHT(&m_dctbl[1]);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: WriteDHT() failed");
          return jerr;
        }

        jerr = WriteDHT(&m_actbl[1]);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: WriteDHT() failed");
          return jerr;
        }
      }
    }
    else
    {
      GenerateHuffmanTablesEX();
    }
    break;

  case JPEG_EXTENDED:
    if(!m_optimal_htbl)
    {
      jerr = WriteDHT(&m_dctbl[0]);
      if(JPEG_OK != jerr)
      {
        LOG0("Error: WriteDHT() failed");
        return jerr;
      }

      jerr = WriteDHT(&m_actbl[0]);
      if(JPEG_OK != jerr)
      {
        LOG0("Error: WriteDHT() failed");
        return jerr;
      }

      if(m_jpeg_ncomp != 1 && (m_jpeg_color == JC_YCBCR || m_jpeg_color == JC_YCCK))
      {
        jerr = WriteDHT(&m_dctbl[1]);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: WriteDHT() failed");
          return jerr;
        }

        jerr = WriteDHT(&m_actbl[1]);
        if(JPEG_OK != jerr)
        {
          LOG0("Error: WriteDHT() failed");
          return jerr;
        }
      }
    }
    else
    {
      GenerateHuffmanTablesEX();
    }
    break;

  case JPEG_PROGRESSIVE:
    // always generated tables
    break;

  case JPEG_LOSSLESS:
    if(!m_optimal_htbl)
    {
      jerr = WriteDHT(&m_dctbl[0]);
      if(JPEG_OK != jerr)
      {
        LOG0("Error: WriteDHT() failed");
        return jerr;
      }
    }
    else
    {
      jerr = GenerateHuffmanTables();
      if(JPEG_OK != jerr)
      {
        LOG0("Error: GenerateHuffmanTables() failed");
        return jerr;
      }
    }
    break;
  default:
    break;
  }

  if(m_jpeg_restart_interval)
  {
    jerr = WriteDRI(m_jpeg_restart_interval);
    if(JPEG_OK != jerr)
    {
      LOG0("Error: WriteDRI() failed");
      return jerr;
    }
  }

  jerr = m_BitStreamOut.FlushBuffer();
  if(JPEG_OK != jerr)
    return jerr;

  return JPEG_OK;
} // CJPEGEncoder::WriteHeader()


JERRCODE CJPEGEncoder::WriteData(void)
{
  JERRCODE  jerr = JPEG_OK;

  switch(m_jpeg_mode)
  {
    case JPEG_BASELINE:
      {
        if(JD_PLANE == m_src.order)
        {
#ifdef _OPENMP
          if(m_jpeg_restart_interval != 0 && m_threading_mode == JT_RSTI)
            jerr = EncodeScanBaselineRSTI_P();
          else
#endif
            jerr = EncodeScanBaseline_P();
        }
        else
        {
#ifdef _OPENMP
          if(m_jpeg_restart_interval != 0 && m_threading_mode == JT_RSTI)
            jerr = EncodeScanBaselineRSTI();
          else
#endif
            jerr = EncodeScanBaseline();
        }
      }
      break;

    case JPEG_EXTENDED:
      {
        if(JD_PLANE == m_src.order)
          jerr = EncodeScanExtended_P();
        else
          jerr = EncodeScanExtended();
      }
      break;

    case JPEG_PROGRESSIVE:
      {
        if(JD_PLANE == m_src.order)
          jerr = EncodeScanProgressive_P();
        else
          jerr = EncodeScanProgressive();
      }
      break;

    case JPEG_LOSSLESS:
      jerr = EncodeScanLossless();
      break;

    default:
      return JPEG_ERR_INTERNAL;
  }

  if(JPEG_OK != jerr)
  {
    LOG0("Error: EncodeScanX() failed");
    return jerr;
  }

  jerr = WriteEOI();
  if(JPEG_OK != jerr)
  {
    LOG0("Error: WriteEOI() failed");
    return jerr;
  }

  jerr = m_BitStreamOut.FlushBuffer();
  if(JPEG_OK != jerr)
    return jerr;

  return JPEG_OK;
} // CJPEGEncoder::WriteData()


JERRCODE CJPEGEncoder::SetComment( int comment_size, char* comment)
{
  if(comment_size > 65533)
    return JPEG_ERR_PARAMS;

  if(comment != 0)
    m_jpeg_comment = comment;

  return JPEG_OK;
} // CJPEGEncoder::SetComment()


JERRCODE CJPEGEncoder::SetJFIFApp0Resolution(JRESUNITS units, int xdensity, int ydensity)
{
  if(units > JRU_DPC  || units < JRU_NONE)
    return JPEG_ERR_PARAMS;

  m_jfif_app0_units    = units;
  m_jfif_app0_xDensity = xdensity;
  m_jfif_app0_yDensity = ydensity;

  return JPEG_OK;
} // CJPEGEncoder::SetApp0Params()
