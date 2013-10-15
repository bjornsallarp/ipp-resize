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
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning ( disable : 4996 )
#endif
#include <stdio.h>
#ifndef __STDFILEOUT_H__
#include "stdfileout.h"
#endif

typedef BaseStream::TStatus Status;

CStdFileOutput::CStdFileOutput(void)
{
  m_fout = 0;
  return;
} // ctor


CStdFileOutput::~CStdFileOutput(void)
{
  Close();
  return;
} // dtor


Status CStdFileOutput::Open(const char* name)
{
  m_fout = fopen(name,"wb");
  if(0 == m_fout)
    return StatusFail;

  return StatusOk;
} // CStdFileOutput::Open()


Status CStdFileOutput::Close(void)
{
  if(0 != m_fout)
  {
    fclose(m_fout);
    m_fout = 0;
  }

  return StatusOk;
} // CStdFileOutput::Close()


Status CStdFileOutput::Seek(TOffset offset, SeekOrigin origin)
{
  int r;

  switch(origin)
  {
  case Beginning:
    r = fseek(m_fout,(long)offset, SEEK_SET);
    if(0 != r)
      return StatusFail;
    break;

  case Current:
    break;

  case End:
    break;
  }

  return StatusOk;
} // CStdFileOutput::Seek()


Status CStdFileOutput::Write(const void* buf,TSize len,TSize& cnt)
{
  size_t cb;
  size_t rb;

  rb = (size_t)len;

  cb = fwrite(buf,sizeof(unsigned char),rb,m_fout);

  cnt = (TSize)cb;

  if(cb != rb)
    return StatusFail;

  return StatusOk;
} // CStdFileOutput::Read()


Status CStdFileOutput::Position(TPosition& pos)
{
  pos = ftell(m_fout);

  return StatusOk;
} // CStdFileOutput::Position()
