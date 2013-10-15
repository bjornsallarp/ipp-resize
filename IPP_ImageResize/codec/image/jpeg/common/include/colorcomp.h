/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __COLORCOMP_H__
#define __COLORCOMP_H__

#ifndef __IPPJ_H__
#include "ippj.h"
#endif
#ifndef __JPEGBASE_H__
#include "jpegbase.h"
#endif




class CJPEGColorComponent
{
public:
  int m_id;
  int m_comp_no;
  int m_hsampling;
  int m_vsampling;
  int m_h_factor;
  int m_v_factor;
  int m_nblocks;
  int m_q_selector;
  int m_dc_selector;
  int m_ac_selector;
  int m_ac_scan_completed;
  int m_cc_height;
  int m_cc_step;
  int m_cc_bufsize;
  int m_ss_height;
  int m_ss_step;
  int m_ss_bufsize;
  int m_need_upsampling;
  Ipp16s m_lastDC;

  CMemoryBuffer m_cc_buf;
  CMemoryBuffer m_ss_buf;
  CMemoryBuffer m_row1;
  CMemoryBuffer m_row2;

  CMemoryBuffer m_lnz_buf;
  int           m_lnz_bufsize;
  int           m_lnz_ds;

  Ipp16s* m_curr_row;
  Ipp16s* m_prev_row;

  CJPEGColorComponent(void);
  virtual ~CJPEGColorComponent(void);

  JERRCODE CreateBufferCC(int num_threads = 1);
  JERRCODE DeleteBufferCC(void);

  JERRCODE CreateBufferSS(int num_threads = 1);
  JERRCODE DeleteBufferSS(void);

  JERRCODE CreateBufferLNZ(int num_threads = 1);
  JERRCODE DeleteBufferLNZ(void);

  Ipp8u* GetCCBufferPtr (int thread_id = 0);
  Ipp8u* GetSSBufferPtr (int thread_id = 0);
  Ipp8u* GetLNZBufferPtr(int thread_id = 0);

};


#endif // __COLORCOMP_H__

