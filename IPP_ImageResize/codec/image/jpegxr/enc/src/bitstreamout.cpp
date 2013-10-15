/*
//               INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//       Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __IPPDEFS_H__
#include "ippdefs.h"
#endif
#ifndef __IPPCORE_H__
#include "ippcore.h"
#endif
#ifndef __IPPS_H__
#include "ipps.h"
#endif
#ifndef __BITSTREAMOUT_H__
#include "bitstreamout.h"
#endif


using namespace UIC;

CBitStreamOutput::CBitStreamOutput(void)
{
  m_out       = 0;
  m_pData     = 0;
  m_DataLen   = 0;
  m_currPos   = 0;
  m_BufferLen = 0;
  m_nBytesWritten = 0;

  return;
} // ctor


CBitStreamOutput::~CBitStreamOutput(void)
{
  Detach();
  return;
} // dtor


ExcStatus CBitStreamOutput::Attach(UIC::BaseStreamOutput &out)
{
  Detach();

  m_out = &out;

  return ExcStatusOk;
} // CBitStreamOutput::Attach()


ExcStatus CBitStreamOutput::Detach(void)
{
  if(0 != m_pData)
  {
    // deallocate internal memory
    ippFree(m_pData);
  }

  m_out       = 0;
  m_pData     = 0;
  m_DataLen   = 0;
  m_currPos   = 0;
  m_bitBuffer = 0;
  m_numBits   = 0;
  m_BufferLen = 0;

  m_nBytesWritten = 0;

  return ExcStatusOk;
} // CBitStreamOutput::Detach()


ExcStatus CBitStreamOutput::Init(Ipp32u bufSize)
{
  if(ENC_MIN_BUFLEN > bufSize)
    m_BufferLen = ENC_MIN_BUFLEN;
  else if(ENC_MAX_BUFLEN < bufSize)
    m_BufferLen = ENC_MAX_BUFLEN;
  else
    m_BufferLen = bufSize;

  m_DataLen = m_BufferLen;

  if(0 != m_pData)
  {
    ippFree(m_pData);
    m_pData = 0;
  }

  m_pData = (Ipp8u*)ippMalloc(m_BufferLen);
  if(0 == m_pData)
    return ExcStatusFail;

  m_currPos = 0; // no data yet
  m_nBytesWritten = 0;

  return ExcStatusOk;
} // CBitStreamOutput::Init()


ExcStatus CBitStreamOutput::FlushBuffer(int nMinBytes)
{
  int   remainder;
  UIC::BaseStream::TSize cnt;

  if(m_currPos > m_DataLen)
    return ExcStatusFail;

  if(nMinBytes)
  {
    remainder = m_DataLen - m_currPos;

    if(remainder > nMinBytes)
      return ExcStatusOk;
  }

  m_out->Write(m_pData, m_currPos, cnt);
  if((int)cnt != m_currPos)
    return ExcStatusFail;

  m_nBytesWritten += m_currPos;
  m_currPos = 0;

  return ExcStatusOk;
} // CBitStreamOutput::FlushBuffer()


ExcStatus CBitStreamOutput::Seek(
  BaseStream::TOffset    offset,
  BaseStream::SeekOrigin origin)
{
  BaseStream::TStatus status;

  if(m_numBits)
    FlushToByte();
  if(m_currPos)
    FlushBuffer();

  status = m_out->Seek(offset, origin);
  if(BaseStream::StatusOk != status)
    return ExcStatusFail;

  m_nBytesWritten = (Ipp32u)offset;

  return ExcStatusOk;
} // CBitStreamOutput::Seek()


ExcStatus CBitStreamOutput::GetStream(Ipp8u **pStream, Ipp32u *pBitBuf, Ipp32u *pBitValid)
{
  *pStream = &m_pData[m_currPos];
  *pBitBuf = m_bitBuffer;
  *pBitValid = m_numBits;

  return ExcStatusOk;
} // CBitStreamOutput::GetStream()


ExcStatus CBitStreamOutput::UpdateStream(Ipp32u iOffset, Ipp32u iBitBuf, Ipp32u iBitValid)
{
  m_currPos += iOffset;
  m_bitBuffer = iBitBuf;
  m_numBits = iBitValid;

  return ExcStatusOk;
} // CBitStreamOutput::UpdateStream()


ExcStatus CBitStreamOutput::Write(Ipp8u *inBuf, Ipp32u iSize)
{
  ExcStatus status;
  Ipp32u iDataLen = m_DataLen - m_currPos;

  if(iDataLen < 2048 || iDataLen < iSize)
    status = FlushBuffer(0);
  iDataLen = m_DataLen - m_currPos;
  iSize = (iSize <= iDataLen)?iSize:iDataLen;

  ippsCopy_8u(inBuf, &m_pData[m_currPos], iSize);
  m_currPos += iSize;

  return ExcStatusOk;
} // CBitStreamOutput::Write()


ExcStatus CBitStreamOutput::WriteBits16(Ipp8u iLen, Ipp16u iVal)
{
  ExcStatus status;
  Ipp8u  iByte;
  Ipp32u iMask = (iLen == 32)?IPP_MAX_32U:(1 << iLen) - 1;
  iVal &= iMask;

  if((32-m_numBits) >= iLen)
  {
    m_bitBuffer |= (iVal << ((32-m_numBits) - iLen));
    m_numBits += iLen;
  }
  else
  {
    while(m_numBits >= 8)
    {
      iByte = (m_bitBuffer >> 24) & 0xFF;
      status = Write(&iByte, 1);
      if(ExcStatusOk != status)
        return status;

      m_bitBuffer <<= 8;
      m_numBits -= 8;
    }

    m_bitBuffer |= iVal << ((32-m_numBits) - iLen);
    m_numBits += iLen;
  }

  return ExcStatusOk;
} // CBitStreamOutput::WriteBits16()


ExcStatus CBitStreamOutput::WriteBits(Ipp8u iLen, Ipp32u iVal)
{
  ExcStatus status;
  Ipp16u iWord0;
  Ipp16u iWord1;

  if(iLen <= 32)
  {
    if(iLen <= 16)
      return WriteBits16(iLen, (Ipp16u)iVal);
    else
    {
      iWord0 = iVal & 0xFFFF;
      iWord1 = iVal >> 16;

      status = WriteBits16(iLen - 16, iWord1);
      status = WriteBits16(16, iWord0);
    }
  }
  else
    return ExcStatusFail;

  return status;
} // CBitStreamOutput::WriteBits()


ExcStatus CBitStreamOutput::FlushBits()
{
  ExcStatus status;
  Ipp8u iByte;

  while(m_numBits >= 8)
  {
    iByte = (m_bitBuffer >> 24) & 0xFF;
    status = Write(&iByte, 1);
    if(ExcStatusOk != status)
      return status;

    m_bitBuffer <<= 8;
    m_numBits -= 8;
  }

  return ExcStatusOk;
} // CBitStreamOutput::FlushBits()


ExcStatus CBitStreamOutput::FlushToByte()
{
  Ipp32u iTail = m_numBits%8;
  Ipp32u iVoid = 0;

  if(iTail)
  {
    WriteBits((Ipp8u)(8 - iTail), iVoid);
    FlushBits();
  }
  else
    FlushBits();

  return ExcStatusOk;
} // CBitStreamOutput::FlushToByte()


ExcStatus CBitStreamOutput::Write(Ipp8u iByte)
{
  if(m_numBits == 0)
    return Write(&iByte, 1);
  else
    return WriteBits(8, iByte);
} // CBitStreamOutput::Write()


ExcStatus CBitStreamOutput::Write(Ipp16u iWord)
{
  if(m_numBits == 0)
    return Write((Ipp8u*)&iWord, 2);
  else
    return WriteBits(16, iWord);
} // CBitStreamOutput::Write()


ExcStatus CBitStreamOutput::Write(Ipp32u iDWord)
{
  if(m_numBits == 0)
    return Write((Ipp8u*)&iDWord, 4);
  else
    return WriteBits(32, iDWord);
} // CBitStreamOutput::Write()
