/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/
#include "stdafx.h"
#include "ipps.h"
#ifndef __MEMBUFFIN_H__
#include "membuffin.h"
#endif

typedef BaseStream::TStatus Status;

CMemBuffInput::CMemBuffInput(void)
{
  m_buf     = 0;
  m_buflen  = 0;
  m_currpos = 0;

  return;
} // ctor


CMemBuffInput::~CMemBuffInput(void)
{
  Close();
  return;
} // dtor


Status CMemBuffInput::Open(const Ipp8u* pBuf, int buflen)
{
  if(0 == pBuf)
    return StatusFail;

  m_buf     = (Ipp8u*)pBuf;
  m_buflen  = buflen;
  m_currpos = 0;

  return StatusOk;
} // CMemBuffInput::Open()


Status CMemBuffInput::Close(void)
{
  m_buf     = 0;
  m_buflen  = 0;
  m_currpos = 0;

  return StatusOk;
} // CMemBuffInput::Close()


Status CMemBuffInput::Seek(TOffset offset, SeekOrigin origin)
{
  switch(origin)
  {
  case BaseStreamInput::Current:
    if(m_currpos + offset > m_buflen || m_currpos + offset < 0)
    {
      return StatusFail;
    }
    m_currpos += (int)offset;
    break;

  case BaseStreamInput::Beginning:
    if(offset >= m_buflen || offset < 0)
    {
      return StatusFail;
    }
    m_currpos = (int)offset;
    break;

  case BaseStreamInput::End:
    if(offset >= m_buflen || offset < 0)
    {
      return StatusFail;
    }
    m_currpos = m_buflen - (int)offset;
    break;

  default:
    return StatusFail;
  }

  return StatusOk;
} // CMemBuffInput::Seek()


Status CMemBuffInput::Read(void* buf,TSize len,TSize& cnt)
{
  TSize rb;

  rb = (TSize)IPP_MIN(len, (TSize)(m_buflen - m_currpos));

  ippsCopy_8u(m_buf + m_currpos,(Ipp8u*)buf,(int)rb);

  m_currpos += (int)rb;

  cnt = rb;

  if(len != rb)
    return StatusFail;

  return StatusOk;
} // CMemBuffInput::Read()

