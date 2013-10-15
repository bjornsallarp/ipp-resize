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

#ifndef __ENCQTBL_H__
#include "encqtbl.h"
#endif




CJPEGEncoderQuantTable::CJPEGEncoderQuantTable(void)
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


CJPEGEncoderQuantTable::~CJPEGEncoderQuantTable(void)
{
  m_id          = 0;
  m_precision   = 0;
  m_initialized = 0;

  ippsZero_8u(m_rbf,sizeof(m_rbf));
  ippsZero_8u(m_qbf,sizeof(m_qbf));

  return;
} // dtor


JERRCODE CJPEGEncoderQuantTable::Init(int id,Ipp8u raw[64],int quality)
{
  IppStatus status;

  m_id        = id;
  m_precision = 0; // 8-bit precision

  ippsCopy_8u(raw,m_raw8u,DCTSIZE2);

  // scale according quality parameter
  if(quality)
  {
    status = ippiQuantFwdRawTableInit_JPEG_8u(m_raw8u,quality);
    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiQuantFwdRawTableInit_JPEG_8u() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  status = ippiQuantFwdTableInit_JPEG_8u16u(m_raw8u,m_qnt16u);
  if(ippStsNoErr != status)
  {
    LOG1("IPP Error: ippiQuantFwdTableInit_JPEG_8u() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  m_initialized = 1;

  return JPEG_OK;
} // CJPEGEncoderQuantTable::Init()


static
IppStatus ippiQuantFwdRawTableInit_JPEG_16u(
  Ipp16u* raw,
  int     quality)
{
  int i;
  int val;

  if(quality > 100)
    quality = 100;

  if(quality < 50)
    quality = 5000 / quality;
  else
    quality = 200 - (quality * 2);

  for(i = 0; i < DCTSIZE2; i++)
  {
    val = (raw[i] * quality + 50) / 100;
    if(val < 1)
    {
      raw[i] = (Ipp16u)1;
    }
    else if(val > 65535)
    {
      raw[i] = (Ipp16u)65535;
    }
    else
    {
      raw[i] = (Ipp16u)val;
    }
  }

  return ippStsNoErr;
} // ippiQuantFwdRawTableInit_JPEG_16u()


static
IppStatus ippiQuantFwdTableInit_JPEG_16u(
  Ipp16u* raw,
  Ipp32f* qnt)
{
  int       i;
  Ipp16u    wb[DCTSIZE2];
  IppStatus status;

  status = ippiZigzagInv8x8_16s_C1((const Ipp16s*)&raw[0],(Ipp16s*)&wb[0]);
  if(ippStsNoErr != status)
  {
    LOG1("IPP Error: ippiZigzagInv8x8_16s_C1() failed - ",status);
    return status;
  }

  for(i = 0; i < DCTSIZE2; i++)
  {
    qnt[i] = (float)(1.0 / wb[i]);
  }

  return ippStsNoErr;
} // ippiQuantFwdTableInit_JPEG_16u()


JERRCODE CJPEGEncoderQuantTable::Init(int id,Ipp16u raw[64],int quality)
{
  IppStatus status;

  m_id        = id;
  m_precision = 1; // 16-bit precision

  ippsCopy_8u((Ipp8u*)raw,(Ipp8u*)m_raw16u,DCTSIZE2*sizeof(Ipp16u));

  if(quality)
  {
    status = ippiQuantFwdRawTableInit_JPEG_16u(m_raw16u,quality);
    if(ippStsNoErr != status)
    {
      LOG1("IPP Error: ippiQuantFwdRawTableInit_JPEG_16u() failed - ",status);
      return JPEG_ERR_INTERNAL;
    }
  }

  status = ippiQuantFwdTableInit_JPEG_16u(m_raw16u,m_qnt32f);

  if(ippStsNoErr != status)
  {
    LOG1("IPP Error: ippiQuantFwdTableInit_JPEG_16u() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  m_initialized = 1;

  return JPEG_OK;
} // CJPEGEncoderQuantTable::Init()

