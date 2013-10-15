/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __BITSTREAMOUT_H__
#define __BITSTREAMOUT_H__

#ifndef __UIC_EXC_STATUS_H__
#include "uic_exc_status.h"
#endif
#ifndef __UIC_BASE_STREAM_OUTPUT_H__
#include "uic_base_stream_output.h"
#endif


class UIC::BaseStreamOutput;

#define ENC_MIN_BUFLEN 16384
#define ENC_MAX_BUFLEN 1048576

namespace UIC {

class CBitStreamOutput
{
public:
  CBitStreamOutput(void);
  virtual ~CBitStreamOutput(void);

  ExcStatus Attach(BaseStreamOutput &out);
  ExcStatus Detach(void);

  ExcStatus Init(Ipp32u bufSize = ENC_MIN_BUFLEN);
  ExcStatus FlushBuffer(int nMinBytes = 0);

  Ipp8u* GetDataPtr(void)  const { return m_pData; }
  int    GetDataLen(void)  const { return m_DataLen; }
  int    GetCurrPos(void)  const { return m_currPos; }
  void   SetCurrPos(int cp)      { m_currPos = cp; return; }
  int    GetFilePos(void)  const { return m_nBytesWritten; }
  int    GetTotalPos(void) const { return (m_nBytesWritten + m_currPos + m_numBits/8); }

  ExcStatus Seek(
    BaseStream::TOffset offset,
    BaseStream::SeekOrigin origin = BaseStream::Beginning);

  ExcStatus Write(Ipp8u *inBuf, Ipp32u iSize);
  ExcStatus WriteBits(Ipp8u nBits, Ipp32u val);
  ExcStatus FlushBits(void);
  ExcStatus FlushToByte(void);
  ExcStatus GetStream(Ipp8u **pStream, Ipp32u *pBitBuf, Ipp32u *pBitValid);
  ExcStatus UpdateStream(Ipp32u iOffset, Ipp32u iBitBuf, Ipp32u iBitValid);

  ExcStatus Write(Ipp8u byte);
  ExcStatus Write(Ipp16u word);
  ExcStatus Write(Ipp32u dword);

protected:
  ExcStatus WriteBits16(Ipp8u nBits, Ipp16u val);

  BaseStreamOutput*  m_out;

  Ipp8u*             m_pData;
  Ipp32u             m_BufferLen;
  int                m_DataLen;
  int                m_currPos;
  Ipp32u             m_bitBuffer;
  Ipp32u             m_numBits;

  int                m_nBytesWritten;
};

} // namespace UIC

#endif // __BITSTREAMOUT_H__

