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

#ifndef __DECQTBL_H__
#include "decqtbl.h"
#endif




CJPEGDecoderQuantTable::CJPEGDecoderQuantTable(void)
{
  m_id          = 0;
  m_precision   = 0;
  m_initialized = 0;

  // align for max performance
  m_raw8u  = (Ipp8u*) OWN_ALIGN_PTR(m_rbf,CPU_CACHE_LINE);
  m_raw16u = (Ipp16u*)OWN_ALIGN_PTR(m_rbf,CPU_CACHE_LINE);
  m_qnt16u = (Ipp16u*)OWN_ALIGN_PTR(m_qbf,CPU_CACHE_LINE);
  m_qnt32f = (Ipp32f*)OWN_ALIGN_PTR(m_qbf,CPU_CACHE_LINE);

  ippsZero_8u(m_rbf,sizeof(m_rbf));
  ippsZero_8u(m_qbf,sizeof(m_qbf));

  return;
} // ctor


CJPEGDecoderQuantTable::~CJPEGDecoderQuantTable(void)
{
  m_id          = 0;
  m_precision   = 0;
  m_initialized = 0;

  ippsZero_8u(m_rbf,sizeof(m_rbf));
  ippsZero_8u(m_qbf,sizeof(m_qbf));

  return;
} // dtor


JERRCODE CJPEGDecoderQuantTable::Init(int id,Ipp8u raw[64])
{
  IppStatus status;

  m_id        = id & 0x0f;
  m_precision = 0; // 8-bit precision

  ippsCopy_8u(raw,m_raw8u,DCTSIZE2);

  status = ippiQuantInvTableInit_JPEG_8u16u(m_raw8u,m_qnt16u);
  if(ippStsNoErr != status)
  {
    LOG1("IPP Error: ippiQuantInvTableInit_JPEG_8u16u() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  m_initialized = 1;

  return JPEG_OK;
} // CJPEGDecoderQuantTable::Init()


static
IppStatus ippiQuantInvTableInit_JPEG_16u32f(
  Ipp16u* raw,
  Ipp32f* qnt)
{
  Ipp16u    wb[DCTSIZE2];
  IppStatus status;

  status = ippiZigzagInv8x8_16s_C1((Ipp16s*)raw,(Ipp16s*)wb);
  if(ippStsNoErr != status)
  {
    return status;
  }

  for(int i = 0; i < DCTSIZE2; i++)
    ((Ipp32f*)qnt)[i] = (Ipp32f)((Ipp16u*)wb)[i];

  return ippStsNoErr;
} // ippiQuantInvTableInit_JPEG_16u32f()


JERRCODE CJPEGDecoderQuantTable::Init(int id,Ipp16u raw[64])
{
  IppStatus status;

  m_id        = id & 0x0f;
  m_precision = 1; // 16-bit precision

  status = ippsCopy_16s((Ipp16s*)raw,(Ipp16s*)m_raw16u,DCTSIZE2);

  status = ippiQuantInvTableInit_JPEG_16u32f(m_raw16u,m_qnt32f);
  if(ippStsNoErr != status)
  {
    LOG1("IPP Error: ippiQuantInvTableInit_JPEG_16u32f() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  m_initialized = 1;

  return JPEG_OK;
} // CJPEGDecoderQuantTable::Init()


JERRCODE CJPEGDecoderQuantTable::ConvertToLowPrecision(void)
{
  IppStatus status;

  status = ippiZigzagInv8x8_16s_C1((Ipp16s*)m_raw16u,(Ipp16s*)m_qnt16u);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  m_precision   = 0; // 8-bit precision
  m_initialized = 1;

  return JPEG_OK;
} // CJPEGDecoderQuantTable::ConvertToLowPrecision()


JERRCODE CJPEGDecoderQuantTable::ConvertToHighPrecision(void)
{
  int       step;
  IppiSize  roi = { DCTSIZE, DCTSIZE };
  Ipp16u    wb[DCTSIZE2];
  IppStatus status;

  step = DCTSIZE * sizeof(Ipp16s);

  status = ippiConvert_8u16u_C1R(m_raw8u,DCTSIZE*sizeof(Ipp8u),wb,step,roi);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  status = ippiCopy_16s_C1R((Ipp16s*)wb,step,(Ipp16s*)m_raw16u,step,roi);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  status = ippiQuantInvTableInit_JPEG_16u32f(m_raw16u,m_qnt32f);
  if(ippStsNoErr != status)
  {
    return JPEG_ERR_INTERNAL;
  }

  m_precision   = 1; // 16-bit precision
  m_initialized = 1;

  return JPEG_OK;
} // CJPEGDecoderQuantTable::ConvertToHighPrecision()
