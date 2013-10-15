/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __JPEGENC_H__
#define __JPEGENC_H__

#ifndef __IPPDEFS_H__
#include "ippdefs.h"
#endif
#ifndef __IPPCORE_H__
#include "ippcore.h"
#endif
#ifndef __IPPS_H__
#include "ipps.h"
#endif
#ifndef __IPPI_H__
#include "ippi.h"
#endif
#ifndef __IPPJ_H__
#include "ippj.h"
#endif
#ifndef __UIC_BASE_STREAM_OUTPUT_H__
#include "uic_base_stream_output.h"
#endif
#ifndef __JPEGBASE_H__
#include "jpegbase.h"
#endif
#ifndef __ENCQTBL_H__
#include "encqtbl.h"
#endif
#ifndef __ENCHTBL_H__
#include "enchtbl.h"
#endif
#ifndef __COLORCOMP_H__
#include "colorcomp.h"
#endif
#ifndef __BITSTREAMOUT_H__
#include "bitstreamout.h"
#endif


class UIC::BaseStreamOutput;

typedef struct _JPEG_SCAN
{
  int ncomp;
  int id[MAX_COMPS_PER_SCAN];
  int Ss;
  int Se;
  int Ah;
  int Al;

} JPEG_SCAN;


class CJPEGEncoder
{
public:

  CJPEGEncoder(void);
  virtual ~CJPEGEncoder(void);

  JERRCODE SetSource(
    Ipp8u*   pSrc,
    int      srcStep,
    IppiSize srcSize,
    int      srcChannels,
    JCOLOR   srcColor,
    JSS      srcSampling  = JS_444,
    int      srcPrecision = 8);

  JERRCODE SetSource(
    Ipp16s*  pSrc,
    int      srcStep,
    IppiSize srcSize,
    int      srcChannels,
    JCOLOR   srcColor,
    JSS      srcSampling = JS_444,
    int      srcPrecision = 16);

  JERRCODE SetSource(
    Ipp8u*   pSrc[4],
    int      srcStep[4],
    IppiSize srcSize,
    int      srcChannels,
    JCOLOR   srcColor,
    JSS      srcSampling  = JS_411,
    int      srcPrecision = 8);

  JERRCODE SetSource(
    Ipp16s*  pSrc[4],
    int      srcStep[4],
    IppiSize srcSize,
    int      srcChannels,
    JCOLOR   srcColor,
    JSS      srcSampling  = JS_444,
    int      srcPrecision = 16);

  JERRCODE SetDestination(
    UIC::BaseStreamOutput* pStreamOut);

  JERRCODE SetParams(
             JMODE mode,
             JCOLOR color,
             JSS sampling,
             int restart_interval,
             int huff_opt,
             int quality,
             JTMODE threading_mode = JT_OLD);

  JERRCODE SetParams(
             JMODE mode,
             JCOLOR color,
             JSS sampling,
             int restart_interval,
             int huff_opt,
             int point_transform,
             int predictor);

  JERRCODE InitHuffmanTable(Ipp8u bits[16], Ipp8u vals[256], int tbl_id, HTBL_CLASS tbl_class);
  JERRCODE InitQuantTable(Ipp8u  qnt[64], int tbl_id, int quality);
  JERRCODE InitQuantTable(Ipp16u qnt[64], int tbl_id, int quality);

  JERRCODE AttachHuffmanTable(int tbl_id, HTBL_CLASS tbl_class, int comp_no);
  JERRCODE AttachQuantTable(int tbl_id, int comp_no);

  JERRCODE WriteHeader(void);
  JERRCODE WriteData(void);

  int NumOfBytes(void) { return m_BitStreamOut.NumOfBytes(); }

  JERRCODE SetComment( int comment_size, char* comment = 0);
  JERRCODE SetJFIFApp0Resolution( JRESUNITS units, int xdensity, int ydensity);

#ifdef __TIMING__
  Ipp64u   m_clk_dct;
  Ipp64u   m_clk_ss;
  Ipp64u   m_clk_cc;
  Ipp64u   m_clk_diff;
  Ipp64u   m_clk_huff;
#endif

protected:
  IMAGE      m_src;

  CBitStreamOutput m_BitStreamOut;
  CBitStreamOutput* m_BitStreamOutT;

  int        m_jpeg_ncomp;
  int        m_jpeg_precision;
  JSS        m_jpeg_sampling;
  JCOLOR     m_jpeg_color;
  int        m_jpeg_quality;
  int        m_jpeg_restart_interval;
  JMODE      m_jpeg_mode;

  int        m_numxMCU;
  int        m_numyMCU;
  int        m_mcuWidth;
  int        m_mcuHeight;
  int        m_ccWidth;
  int        m_ccHeight;
  int        m_xPadding;
  int        m_yPadding;
  int        m_rst_go;
  int        m_restarts_to_go;
  int        m_next_restart_num;
  int        m_scan_count;
  int        m_ss;
  int        m_se;
  int        m_al;
  int        m_ah;
  int        m_predictor;
  int        m_pt;
  int        m_optimal_htbl;
  JPEG_SCAN* m_scan_script;

  Ipp16s*    m_block_buffer;
  int        m_num_threads;
  int        m_nblock;

  int        m_jfif_app0_xDensity;
  int        m_jfif_app0_yDensity;
  JRESUNITS  m_jfif_app0_units;

  char*      m_jpeg_comment;

  int        m_num_rsti;
  int        m_rstiHeight;
  JTMODE     m_threading_mode;

  Ipp16s**   m_lastDC;

  CJPEGEncoderHuffmanState*   m_state_t;

  CJPEGColorComponent        m_ccomp[MAX_COMPS_PER_SCAN];
  CJPEGEncoderQuantTable     m_qntbl[MAX_QUANT_TABLES];
  CJPEGEncoderHuffmanTable   m_dctbl[MAX_HUFF_TABLES];
  CJPEGEncoderHuffmanTable   m_actbl[MAX_HUFF_TABLES];
  CJPEGEncoderHuffmanState   m_state;

  JERRCODE Init(void);
  JERRCODE Clean(void);
  JERRCODE ColorConvert(int nMCURow, int thread_id = 0);
  JERRCODE DownSampling(int nMCURow, int thread_id = 0);

  JERRCODE WriteSOI(void);
  JERRCODE WriteEOI(void);
  JERRCODE WriteAPP0(void);
  JERRCODE WriteAPP14(void);
  JERRCODE WriteSOF0(void);
  JERRCODE WriteSOF1(void);
  JERRCODE WriteSOF2(void);
  JERRCODE WriteSOF3(void);
  JERRCODE WriteDRI(int restart_interval);
  JERRCODE WriteRST(int next_restart_num);
  JERRCODE WriteSOS(void);
  JERRCODE WriteSOS(int ncomp,int id[MAX_COMPS_PER_SCAN],int Ss,int Se,int Ah,int Al);
  JERRCODE WriteDQT(CJPEGEncoderQuantTable* tbl);
  JERRCODE WriteDHT(CJPEGEncoderHuffmanTable* tbl);
  JERRCODE WriteCOM(char* comment = 0);

  JERRCODE EncodeScanBaseline(void);

  JERRCODE EncodeScanBaselineRSTI(void);
  JERRCODE EncodeScanBaselineRSTI_P(void);

  JERRCODE EncodeHuffmanMCURowBL_RSTI(Ipp16s* pMCUBuf, int thread_id = 0);
  JERRCODE ProcessRestart(int id[MAX_COMPS_PER_SCAN],int Ss,int Se,int Ah,int Al, int nRSTI, int thread_id);
  JERRCODE WriteRST_T(int next_restart_num,  int thread_id = 0);

  JERRCODE EncodeScanBaseline_P(void);
  JERRCODE EncodeScanExtended(void);
  JERRCODE EncodeScanExtended_P(void);
  JERRCODE EncodeScanLossless(void);
  JERRCODE EncodeScanProgressive(void);

  JERRCODE EncodeScan(int ncomp,int id[MAX_COMPS_PER_SCAN],int Ss,int Se,int Ah,int Al);
  JERRCODE SelectScanScripts(void);
  JERRCODE GenerateHuffmanTables(int ncomp,int id[MAX_COMPS_PER_SCAN],int Ss,int Se,int Ah,int Al);
  JERRCODE GenerateHuffmanTables(void);
  JERRCODE GenerateHuffmanTablesEX(void);

  JERRCODE ProcessRestart(int id[MAX_COMPS_PER_SCAN],int Ss,int Se,int Ah,int Al);
  JERRCODE ProcessRestart(int stat[2][256],int id[MAX_COMPS_PER_SCAN],int Ss,int Se,int Ah,int Al);

  JERRCODE EncodeHuffmanMCURowBL(Ipp16s* pMCUBuf);
  JERRCODE EncodeHuffmanMCURowLS(Ipp16s* pMCUBuf);

  JERRCODE TransformMCURowBL(Ipp16s* pMCUBuf, int thread_id = 0);

  JERRCODE ProcessBuffer(int nMCURow, int thread_id = 0);
  JERRCODE EncodeScanProgressive_P(void);

  JERRCODE TransformMCURowEX(Ipp16s* pMCUBuf, int thread_id = 0);
  JERRCODE TransformMCURowLS(Ipp16s* pMCUBuf, int nMCURow, int thread_id = 0);

};

#endif // __JPEGENC_H__

