/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/
#include "stdafx.h"
#include <stdio.h>
#include "ipps.h"
#ifndef __MEMBUFFOUT_H__
#include "membuffout.h"
#endif

typedef BaseStream::TStatus Status;

CMemBuffOutput::CMemBuffOutput(void)
{
  m_buf     = 0;
  m_buflen  = 0;
  m_currpos = 0;

  return;
} // ctor


CMemBuffOutput::~CMemBuffOutput(void)
{
  Close();
  return;
} // dtor


Status CMemBuffOutput::Open(Ipp8u* pBuf, int buflen)
{
  if(0 == pBuf)
    return StatusFail;

  m_buf     = pBuf;
  m_buflen  = buflen;
  m_currpos = 0;

  return StatusOk;
} // CMemBuffOutput::Open()


Status CMemBuffOutput::Close(void)
{
  m_buf     = 0;
  m_buflen  = 0;
  m_currpos = 0;

  return StatusOk;
} // CMemBuffOutput::Close()


Status CMemBuffOutput::Seek(TOffset offset, SeekOrigin origin)
{
  switch(origin)
  {
  case Beginning:
    if(offset >= m_buflen || offset < 0)
      return StatusFail;

    m_currpos = (int)offset;
    break;

  case Current:
    if(offset >= m_buflen)
      return StatusFail;

    m_currpos += (int)offset;
    break;

  case End:
    if(offset >= m_buflen || offset > 0)
      return StatusFail;

    m_currpos = m_buflen - (int)offset;
    break;
  }

  return StatusOk;
} // CMemBuffOutput::Seek()


Status CMemBuffOutput::Write(const void* buf,TSize len,TSize& cnt)
{
  TSize wb;

  wb = (TSize)IPP_MIN(len, (TSize)(m_buflen - m_currpos));

  ippsCopy_8u((Ipp8u*)buf,m_buf + m_currpos,(int)wb);

  m_currpos += (int)wb;

  cnt = wb;

  if(len != wb)
    return StatusFail;

  return StatusOk;
} // CMemBuffOutput::Write()

