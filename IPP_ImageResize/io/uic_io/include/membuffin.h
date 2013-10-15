/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __MEMBUFFIN_H__
#define __MEMBUFFIN_H__

#include <stdio.h>
#ifndef __UIC_BASE_STREAM_INPUT_H__
#include "uic_base_stream_input.h"
#endif

using namespace UIC;

class UICAPI CMemBuffInput : public BaseStreamInput
{
public:
  CMemBuffInput(void);
  ~CMemBuffInput(void);

  TStatus Open(const Ipp8u* pBuf, int buflen);
  TStatus Close(void);

  TStatus Size(TSize& size) { size = m_buflen; return StatusOk; }
  TStatus Position(TPosition& pos) { pos = m_currpos; return StatusOk; }
  TStatus Seek(TOffset offset, SeekOrigin origin);

  TStatus Read(void* buf,TSize len,TSize& cnt);

protected:
  Ipp8u*  m_buf;
  int     m_buflen;
  int     m_currpos;

};


#endif // __MEMBUFFIN_H__

