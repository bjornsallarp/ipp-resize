/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __MEMBUFFOUT_H__
#define __MEMBUFFOUT_H__

#ifndef __UIC_BASE_STREAM_OUTPUT_H__
#include "uic_base_stream_output.h"
#endif


using namespace UIC;

class UICAPI CMemBuffOutput : public BaseStreamOutput
{
public:
  CMemBuffOutput(void);
  ~CMemBuffOutput(void);

  TStatus Open(Ipp8u* pBuf, int buflen);
  TStatus Close(void);

  TStatus Size(TSize& size)  { size = m_buflen; return StatusOk; }
  TStatus Position(TPosition& pos )  { pos = m_currpos; return StatusOk; }
  TStatus SetPosition(int pos ) {m_currpos = pos; return StatusOk; }
  TStatus Seek(TOffset offset, SeekOrigin origin);

  TStatus Write(const void* buf,TSize len,TSize& cnt);

protected:
  Ipp8u*  m_buf;
  int     m_buflen;
  int     m_currpos;

};


#endif // __MEMBUFFOUT_H__

