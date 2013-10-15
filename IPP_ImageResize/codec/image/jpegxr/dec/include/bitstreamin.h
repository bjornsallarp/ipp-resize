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

#ifndef __UIC_EXC_STATUS_H__
#include "uic_exc_status.h"
#endif
#ifndef __UIC_BASE_STREAM_INPUT_H__
#include "uic_base_stream_input.h"
#endif


class UIC::BaseStreamInput;

#define DEC_MIN_BUFLEN 16384
#define DEC_MAX_BUFLEN 1048576

#define RET_(excCall) {if(excCall != ExcStatusOk) return ExcStatusFail;}

namespace UIC {

class CBitStreamInput
{
public:
  CBitStreamInput(void);
  virtual ~CBitStreamInput(void);

  ExcStatus Attach(BaseStreamInput& in);
  ExcStatus Detach(void);

  ExcStatus Init(Ipp32u bufSize = DEC_MIN_BUFLEN);
  ExcStatus FillBuffer(void);

  Ipp8u* GetDataPtr(void) const { return m_pData; }
  Ipp32u GetDataLen(void) const { return m_DataLen - m_currPos; }
  Ipp8u* GetCurrPtr(void) const { return m_pData + m_currPos; }
  int    GetCurrPos(void) const { return m_currPos; }
  void   SetCurrPos(int cp) { m_currPos = cp; return; }
  Ipp32u GetTotalPos(void) { return (m_totalPos - m_numBits/8); }

  ExcStatus Seek(
    BaseStream::TOffset offset,
    BaseStream::SeekOrigin origin = BaseStream::Current);

  ExcStatus Read(Ipp8u *outBuf, Ipp32u iSize);
  ExcStatus ReadBits(Ipp8u nBits, Ipp8u&  val);
  ExcStatus ReadBits(Ipp8u nBits, Ipp16u& val);
  ExcStatus ReadBits(Ipp8u nBits, Ipp32u& val);
  ExcStatus PeekBits(Ipp8u nBits, Ipp32u& val);
  ExcStatus FlushToByte(void);
  ExcStatus Flush(Ipp8u iLen);
  ExcStatus GetHuffman(const Ipp16s* pDecodeTable, Ipp32s &iSymbol);
  ExcStatus GetStream(Ipp8u **pStream, Ipp32u *pBitBuf, Ipp32u *pBitValid);
  ExcStatus GetBuffer(Ipp8u **pStream, Ipp32u *pStreamLen);
  ExcStatus UpdateStream(Ipp8u *pStream, Ipp32u iBitBuf, Ipp32u iBitValid);

  ExcStatus Read(Ipp8u&  byte);
  ExcStatus Read(Ipp16u& word);
  ExcStatus Read(Ipp32u& dword);

public:
  Ipp32u           m_fileSize;

protected:
  BaseStreamInput*  m_in;

  Ipp8u*            m_pData;
  int               m_DataLen;
  int               m_currPos;
  Ipp32u            m_BufferLen;
  Ipp32u            m_totalPos;
  Ipp32u            m_filePos;
  Ipp32u            m_bitBuffer;
  Ipp32u            m_numBits;
  Ipp32u            m_bExBuf;

  int               m_eod;
};

} // namespace UIC

#endif // __BITSTREAMIN_H__

