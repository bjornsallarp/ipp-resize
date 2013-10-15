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

#ifndef __COLORCOMP_H__
#include "colorcomp.h"
#endif


CJPEGColorComponent::CJPEGColorComponent(void)
{
  m_id          = 0;
  m_comp_no     = 0;
  m_hsampling   = 0;
  m_vsampling   = 0;
  m_h_factor    = 0;
  m_v_factor    = 0;
  m_nblocks     = 0;
  m_q_selector  = 0;
  m_dc_selector = 0;
  m_ac_selector = 0;
  m_lastDC      = 0;
  m_ac_scan_completed = 0;
  m_cc_height   = 0;
  m_cc_step     = 0;
  m_cc_bufsize  = 0;
  m_ss_height   = 0;
  m_ss_step     = 0;
  m_ss_bufsize  = 0;
  m_curr_row    = 0;
  m_prev_row    = 0;

  m_lnz_bufsize = 0;
  m_lnz_ds      = 0;

  m_need_upsampling   = 0;

  return;
} // ctor


CJPEGColorComponent::~CJPEGColorComponent(void)
{
  return;
} // dtor;


JERRCODE CJPEGColorComponent::CreateBufferCC(int num_threads)
{
  m_cc_bufsize = m_cc_step * m_cc_height;

  return m_cc_buf.Allocate(m_cc_bufsize * num_threads);
} // CJPEGColorComponent::CreateBufferCC()


JERRCODE CJPEGColorComponent::DeleteBufferCC(void)
{
  return m_cc_buf.Delete();
} // CJPEGColorComponent::DeleteBufferCC()


JERRCODE CJPEGColorComponent::CreateBufferSS(int num_threads)
{
  m_ss_bufsize = m_ss_step * m_ss_height;

  return m_ss_buf.Allocate(m_ss_bufsize * num_threads);
} // CJPEGColorComponent::CreateBufferSS()


JERRCODE CJPEGColorComponent::DeleteBufferSS(void)
{
  return m_ss_buf.Delete();
} // CJPEGColorComponent::DeleteBufferSS()


Ipp8u* CJPEGColorComponent::GetCCBufferPtr(int thread_id)
{
  Ipp8u* ptr = m_cc_buf;

  return &ptr[m_cc_bufsize * thread_id];
} // CJPEGColorComponent::GetCCBufferPtr()


Ipp8u* CJPEGColorComponent::GetSSBufferPtr(int thread_id)
{
  Ipp8u* ptr = m_ss_buf;

  return &ptr[m_ss_bufsize*thread_id];
} // CJPEGColorComponent::GetCCBufferPtr()


JERRCODE CJPEGColorComponent::CreateBufferLNZ(int num_threads)
{
  return m_lnz_buf.Allocate(m_lnz_bufsize * num_threads);
} // CJPEGColorComponent::CreateBufferLNZ()


JERRCODE CJPEGColorComponent::DeleteBufferLNZ(void)
{
  return m_lnz_buf.Delete();
} // CJPEGColorComponent::DeleteBufferLNZ()


Ipp8u* CJPEGColorComponent::GetLNZBufferPtr(int thread_id)
{
  Ipp8u* ptr = m_lnz_buf;

  return &ptr[m_lnz_bufsize * thread_id];
} // CJPEGColorComponent::GetLNZBufferPtr()

