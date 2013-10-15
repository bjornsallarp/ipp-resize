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
#ifndef __STDFILEIN_H__
#include "stdfilein.h"
#endif

typedef BaseStream::TStatus Status;

CStdFileInput::CStdFileInput(void)
{
  m_fin = 0;
  return;
} // ctor


CStdFileInput::~CStdFileInput(void)
{
  Close();
  return;
} // dtor


Status CStdFileInput::Open(const char* name)
{
  m_fin = fopen(name,"rb");
  if(0 == m_fin)
    return StatusFail;

  return StatusOk;
} // CStdFileInput::Open()


Status CStdFileInput::Close(void)
{
  if(0 != m_fin)
  {
    fclose(m_fin);
    m_fin = 0;
  }

  return StatusOk;
} // CStdFileInput::Close()


Status CStdFileInput::Seek(TOffset offset, SeekOrigin origin)
{
  int r;
  int fOrigin;

  switch(origin)
  {
  case Beginning: fOrigin = SEEK_SET; break;
  case End:       fOrigin = SEEK_END; break;
  default:        fOrigin = SEEK_CUR; break;
  }

  r = fseek(m_fin,(long)offset, fOrigin);
  if(0 != r)
    return StatusFail;

  return StatusOk;
} // CStdFileInput::Seek()


Status CStdFileInput::Read(void* buf,TSize len,TSize& cnt)
{
  size_t cb;
  size_t rb;

  rb = (size_t)len;

  cb = fread(buf,sizeof(unsigned char),rb,m_fin);

  cnt = (TSize)cb;

  if(cb != rb)
    return StatusFail;

  return StatusOk;
} // CStdFileInput::Read()


Status CStdFileInput::Size(TSize& pos)
{
  TPosition oldpos;
  Status status;

  status = Position(oldpos);
  if(StatusOk != status)
    return status;

  status = Seek(0,BaseStreamInput::End);
  if(StatusOk != status)
    return status;

  status = Position(pos);
  if(StatusOk != status)
    return status;

  status = Seek(oldpos,BaseStreamInput::Beginning);
  if(StatusOk != status)
    return status;

  return StatusOk;
} // CStdFileInput::Size()


Status CStdFileInput::Position(TPosition& pos)
{
  pos = ftell(m_fin);

  return StatusOk;
} // CStdFileInput::Position()
