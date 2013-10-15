/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __STDFILEIN_H__
#define __STDFILEIN_H__

#include <stdio.h>
#ifndef __UIC_BASE_STREAM_INPUT_H__
#include "uic_base_stream_input.h"
#endif


using namespace UIC;

class UICAPI CStdFileInput : public BaseStreamInput
{
public:
  CStdFileInput(void);
  ~CStdFileInput(void);

  TStatus Open(const char* name);
  TStatus Close(void);

  TStatus Size(TSize& size);
  TStatus Position(TPosition& pos);

  TStatus Seek(TOffset offset, SeekOrigin origin);
  TStatus Read(void* buf,TSize len,TSize& cnt);

protected:
  FILE*  m_fin;

};


#endif // __STDFILEIN_H__

