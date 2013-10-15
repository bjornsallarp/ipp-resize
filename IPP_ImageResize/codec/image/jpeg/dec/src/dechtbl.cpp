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

#ifndef __JPEGBASE_H__
#include "jpegbase.h"
#endif
#ifndef __DECHTBL_H__
#include "dechtbl.h"
#endif




CJPEGDecoderHuffmanTable::CJPEGDecoderHuffmanTable(void)
{
  m_id     = 0;
  m_hclass = 0;
  m_table  = 0;
  m_bEmpty = 1;
  m_bValid = 0;

  ippsZero_8u(m_bits,sizeof(m_bits));
  ippsZero_8u(m_vals,sizeof(m_vals));

  return;
} // ctor


CJPEGDecoderHuffmanTable::~CJPEGDecoderHuffmanTable(void)
{
  Destroy();
  return;
} // dtor


JERRCODE CJPEGDecoderHuffmanTable::Create(void)
{
  int       size;
  IppStatus status;

  status = ippiDecodeHuffmanSpecGetBufSize_JPEG_8u(&size);
  if(ippStsNoErr != status)
  {
    LOG1("IPP Error: ippiDecodeHuffmanSpecGetBufSize_JPEG_8u() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  if(0 != m_table)
  {
    ippFree(m_table);
    m_table = 0;
  }

  m_table = (IppiDecodeHuffmanSpec*)ippMalloc(size);
  if(0 == m_table)
  {
    LOG0("IPP Error: ippMalloc() failed");
    return JPEG_ERR_ALLOC;
  }

  m_bEmpty = 0;
  m_bValid = 0;

  return JPEG_OK;
} // CJPEGDecoderHuffmanTable::Create()


JERRCODE CJPEGDecoderHuffmanTable::Destroy(void)
{
  m_id     = 0;
  m_hclass = 0;

  ippsZero_8u(m_bits,sizeof(m_bits));
  ippsZero_8u(m_vals,sizeof(m_vals));

  if(0 != m_table)
  {
    ippFree(m_table);
    m_table = 0;
  }

  m_bValid = 0;
  m_bEmpty = 1;

  return JPEG_OK;
} // CJPEGDecoderHuffmanTable::Destroy()


JERRCODE CJPEGDecoderHuffmanTable::Init(int id,int hclass,Ipp8u* bits,Ipp8u* vals)
{
  IppStatus status;

  m_id     = id     & 0x0f;
  m_hclass = hclass & 0x0f;

  ippsCopy_8u(bits,m_bits,16);
  ippsCopy_8u(vals,m_vals,256);

  status = ippiDecodeHuffmanSpecInit_JPEG_8u(m_bits,m_vals,m_table);
  if(ippStsNoErr != status)
  {
    LOG1("IPP Error: ippiDecodeHuffmanSpecInit_JPEG_8u() failed - ",status);
    return JPEG_ERR_DHT_DATA;
  }

  m_bValid = 1;
  m_bEmpty = 0;

  return JPEG_OK;
} // CJPEGDecoderHuffmanTable::Init()




CJPEGDecoderHuffmanState::CJPEGDecoderHuffmanState(void)
{
  m_state = 0;
  return;
} // ctor


CJPEGDecoderHuffmanState::~CJPEGDecoderHuffmanState(void)
{
  Destroy();
  return;
} // dtor


JERRCODE CJPEGDecoderHuffmanState::Create(void)
{
  int       size;
  IppStatus status;

  status = ippiDecodeHuffmanStateGetBufSize_JPEG_8u(&size);
  if(ippStsNoErr != status)
  {
    LOG1("IPP Error: ippiDecodeHuffmanStateGetBufSize_JPEG_8u() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  if(0 != m_state)
  {
    ippFree(m_state);
    m_state = 0;
  }

  m_state = (IppiDecodeHuffmanState*)ippMalloc(size);
  if(0 == m_state)
  {
    LOG0("IPP Error: ippMalloc() failed");
    return JPEG_ERR_ALLOC;
  }

  return JPEG_OK;
} // CJPEGDecoderHuffmanState::Create()


JERRCODE CJPEGDecoderHuffmanState::Destroy(void)
{
  if(0 != m_state)
  {
    ippFree(m_state);
    m_state = 0;
  }

  return JPEG_OK;
} // CJPEGDecoderHuffmanState::Destroy()


JERRCODE CJPEGDecoderHuffmanState::Init(void)
{
  IppStatus status;

  status = ippiDecodeHuffmanStateInit_JPEG_8u(m_state);
  if(ippStsNoErr != status)
  {
    LOG1("IPP Error: ippiDecodeHuffmanStateInit_JPEG_8u() failed - ",status);
    return JPEG_ERR_INTERNAL;
  }

  return JPEG_OK;
} // CJPEGDecoderHuffmanState::Init()
