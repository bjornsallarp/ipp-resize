/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __STDFILEOUT_H__
#define __STDFILEOUT_H__

#ifndef __UIC_BASE_STREAM_OUTPUT_H__
#include "uic_base_stream_output.h"
#endif


using namespace UIC;

class UICAPI CStdFileOutput : public BaseStreamOutput
{
public:
  CStdFileOutput(void);
  ~CStdFileOutput(void);

  TStatus Open(const char* name);
  TStatus Close(void);

  TStatus Size(TSize&) { return StatusFail; }
  TStatus Position(TPosition& pos);
  TStatus Seek(TOffset offset, SeekOrigin origin);

  TStatus Write(const void* buf,TSize len,TSize& cnt);

protected:
  FILE*  m_fout;

};


#endif // __STDFILEOUT_H__

