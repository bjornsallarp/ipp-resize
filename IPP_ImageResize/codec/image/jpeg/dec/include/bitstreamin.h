/*
//
//             INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __BITSTREAMIN_H__
#define __BITSTREAMIN_H__

#ifndef __JPEGBASE_H__
#include "jpegbase.h"
#endif
#ifndef __UIC_BASE_STREAM_INPUT_H__
#include "uic_base_stream_input.h"
#endif


class UIC::BaseStreamInput;
typedef UIC::BaseStreamInput CBaseStreamInput;

const int DEC_DEFAULT_BUFLEN = 4096; // internal intermediate input buffer

class CBitStreamInput
{
public:
  CBitStreamInput(void);
  virtual ~CBitStreamInput(void);

  JERRCODE Attach(CBaseStreamInput* in);
  JERRCODE Attach(CBitStreamInput* in);

  JERRCODE Detach(void);

  JERRCODE Init(int bufSize = DEC_DEFAULT_BUFLEN);
  JERRCODE FillBuffer(int nMinBytes = 0);
  JERRCODE FillBitStream(CBitStreamInput* bitStream, int offset, int bufSize);

  Ipp8u* GetDataPtr(void) const { return m_pData; }
  int    GetDataLen(void) const { return m_DataLen; }
  Ipp8u* GetCurrPtr(void) const { return m_pData + m_currPos; }
  int    GetCurrPos(void) const { return m_currPos; }
  void   SetCurrPos(int cp) { m_currPos = cp; return; }

  int    GetStreamPos(void);
  void   SetDataLen(int dataLen) {m_DataLen = dataLen; return;}
  CBaseStreamInput* GetStream(void) {return m_in;}

  JERRCODE Seek(UIC::BaseStream::TOffset offset, UIC::BaseStreamInput::SeekOrigin origin = UIC::BaseStreamInput::Current);
  JERRCODE CheckByte(int pos, int* byte);
  JERRCODE ReadByte(int* byte);
  JERRCODE ReadWord(int* word);
  JERRCODE ReadDword(int* dword);

protected:
  CBaseStreamInput* m_in;

  Ipp8u*            m_pData;
  int               m_DataLen;
  int               m_currPos;

  int               m_eod;

  int               m_bufOffset;
};


#endif // __BITSTREAMIN_H__

