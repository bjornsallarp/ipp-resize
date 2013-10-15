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
#ifndef __BITSTREAMIN_H__
#include "bitstreamin.h"
#endif


using namespace UIC;

CBitStreamInput::CBitStreamInput(void)
{
  m_in       = 0;
  m_pData    = 0;
  m_DataLen  = 0;
  m_currPos  = 0;
  m_eod      = 0;
  m_totalPos = 0;
  m_filePos  = 0;
  m_BufferLen = 0;
  m_bExBuf    = 0;

  return;
} // ctor


CBitStreamInput::~CBitStreamInput(void)
{
  Detach();
  return;
} // dtor


ExcStatus CBitStreamInput::Attach(BaseStreamInput& in)
{
  BaseStream::TSize size;
  Detach();

  m_in = &in;
  m_in->Size(size);
  m_fileSize = (Ipp32u)size;

  return ExcStatusOk;
} // CBitStreamInput::Attach()


ExcStatus CBitStreamInput::Detach(void)
{
  if(0 != m_pData && !m_bExBuf)
  {
    // deallocate internal memory
    ippFree(m_pData);
    m_pData = 0;
  }

  m_in       = 0;
  m_pData    = 0;
  m_DataLen  = 0;
  m_currPos  = 0;
  m_eod      = 0;
  m_totalPos = 0;
  m_filePos  = 0;
  m_BufferLen = 0;

  m_numBits   = 0;
  m_bitBuffer = 0;

  return ExcStatusOk;
} // CBitStreamInput::Detach()


ExcStatus CBitStreamInput::Init(Ipp32u bufSize)
{
  if(DEC_MIN_BUFLEN > bufSize)
    m_BufferLen = DEC_MIN_BUFLEN;
  else if(DEC_MAX_BUFLEN < bufSize)
    m_BufferLen = DEC_MAX_BUFLEN;
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

  m_currPos = m_DataLen; // no data yet

  FillBuffer();

  return ExcStatusOk;
} // CBitStreamInput::Init()


ExcStatus CBitStreamInput::FillBuffer(void)
{
  BaseStream::TSize cnt;
  BaseStream::TPosition pos;
  BaseStream::TStatus status;
  int remainder = m_DataLen - m_currPos;

  if(!m_eod)
  {
    if(remainder)
      ippsMove_8u(&m_pData[m_currPos], m_pData, remainder);

    status = m_in->Position(pos);
    if(BaseStream::StatusOk != status)
      return ExcStatusFail;

    if(m_filePos != (Ipp32u)pos)
      m_in->Seek(m_filePos, BaseStreamInput::Beginning);

    status = m_in->Read(m_pData + remainder, m_BufferLen - remainder, cnt);
    if(cnt != (UIC::BaseStream::TSize)(m_BufferLen - remainder))
      m_eod = 1;

    status = m_in->Position(pos);
    if(BaseStream::StatusOk != status)
      return ExcStatusFail;

    m_filePos = (Ipp32u)pos;
    m_currPos = 0;
    m_DataLen = remainder + (int)cnt;
  }
  else
    m_eod = 1;

  if(m_eod && m_currPos > m_DataLen && m_numBits == 0)
    return ExcStatusFail;

  return ExcStatusOk;
} // CBitStreamInput::FillBuffer()


ExcStatus CBitStreamInput::Seek(
  BaseStream::TOffset    offset,
  BaseStream::SeekOrigin origin)
{
  BaseStream::TPosition pos;
  BaseStream::TStatus status;
  int _offset;
  int bits_shift;
  ExcStatus exc;

  bits_shift = m_numBits/8;
  m_bitBuffer = 0;
  m_numBits   = 0;

  switch(origin)
  {
    case BaseStreamInput::Current:
    {
      _offset = m_currPos + (int)offset - bits_shift;
      m_totalPos += _offset;

      if(_offset > 0 && _offset < m_DataLen)
      {
        m_currPos = _offset;
      }
      else
      {
        m_currPos = m_DataLen;

        status = m_in->Position(pos);
        if(BaseStream::StatusOk != status)
          return ExcStatusFail;

        if(m_filePos != (Ipp32u)pos)
        {
          status = m_in->Seek(m_filePos + _offset - m_DataLen, BaseStreamInput::Beginning);
          if(BaseStream::StatusOk != status)
            return ExcStatusFail;
        }
        else
        {
          status = m_in->Seek(_offset - m_DataLen, BaseStreamInput::Current);
          if(BaseStream::StatusOk != status)
            return ExcStatusFail;
        }

        status = m_in->Position(pos);
        if(BaseStream::StatusOk != status)
          return ExcStatusFail;
        m_filePos = (Ipp32u)pos;

        m_eod = 0;
        exc = FillBuffer();
        if(ExcStatusOk != exc)
          return exc;
      }
      break;
    }

  case BaseStreamInput::Beginning:
    {
      m_currPos = m_DataLen;

      if(offset < m_filePos && offset > (m_filePos - m_DataLen))
      {
        m_currPos = (int)offset - (m_filePos - m_DataLen);
      }
      else
      {
        status = m_in->Seek(offset, BaseStreamInput::Beginning);
        if(BaseStream::StatusOk != status)
          return ExcStatusFail;

        status = m_in->Position(pos);
        if(BaseStream::StatusOk != status)
          return ExcStatusFail;
        m_filePos = (Ipp32u)pos;

        m_eod = 0;
        exc = FillBuffer();
        if(ExcStatusOk != exc)
          return exc;
      }

      m_totalPos = (Ipp32u)offset;
    }
    break;

  case BaseStreamInput::End:
  default:
    return ExcStatusFail;
  }

  return ExcStatusOk;
} // CBitStreamInput::Seek()


ExcStatus CBitStreamInput::GetStream(Ipp8u** pStream, Ipp32u* pBitBuf, Ipp32u* pBitValid)
{
  *pStream    = &m_pData[m_currPos];
  *pBitBuf    = m_bitBuffer;
  *pBitValid  = m_numBits;

  return ExcStatusOk;
}


ExcStatus CBitStreamInput::GetBuffer(Ipp8u** pStream, Ipp32u* pStreamLen)
{
  *pStream    = &m_pData[0];
  *pStreamLen = m_DataLen;

  return ExcStatusOk;
}


ExcStatus CBitStreamInput::UpdateStream(Ipp8u* pStream, Ipp32u iBitBuf, Ipp32u iBitValid)
{
  int iLen = (int)(pStream - &m_pData[m_currPos]);
  if((m_currPos + iLen > m_DataLen) && !m_eod)
    return ExcStatusFail;
  m_currPos  += iLen;
  m_bitBuffer = iBitBuf;
  m_numBits   = iBitValid;

  return ExcStatusOk;
}


ExcStatus CBitStreamInput::GetHuffman(const Ipp16s* pDecodeTable, Ipp32s& iSymbol)
{
  Ipp32s iCode;
  Ipp32u iDWord;
  Ipp8u  iBit;

  RET_(PeekBits(5, iDWord));
  iCode = pDecodeTable[iDWord];
  RET_(Flush(((iCode < 0)?5:(iCode & 0x7))));
  iSymbol = iCode >> 3;

  if(iSymbol < 0)
  {
    RET_(ReadBits(1, iBit));
    iSymbol = pDecodeTable[iCode + iBit + 0x8000];
    while(iSymbol < 0)
    {
      RET_(ReadBits(1, iBit));
      iSymbol = pDecodeTable[iSymbol + iBit + 0x8000];
    }
  }

  return ExcStatusOk;
} // CBitStreamInput::GetHuffman()


ExcStatus CBitStreamInput::PeekBits(Ipp8u iLen, Ipp32u& pVal)
{
  ExcStatus status;
  Ipp8u iByte;

  if(m_numBits >= iLen)
    pVal = m_bitBuffer >> (32 - iLen);
  else
  {
    m_bitBuffer >>= 32 - m_numBits;

    while(m_numBits <= 24)
    {
      m_bitBuffer <<= 8;
      status = Read(&iByte, 1);
      if(ExcStatusOk != status)
        return status;

      m_bitBuffer |= iByte;
      m_numBits += 8;
    }

    m_bitBuffer <<= (32 - m_numBits);

    pVal = m_bitBuffer >> (32 - iLen);
  }

  return ExcStatusOk;
} // CBitStreamInput::PeekBits()


ExcStatus CBitStreamInput::ReadBits(Ipp8u iLen, Ipp8u& pVal)
{
  ExcStatus status;
  Ipp8u iByte;

  if(iLen == 0)
    return ExcStatusOk;

  if(m_numBits >= iLen)
  {
    pVal = (Ipp8u)(m_bitBuffer >> (32 - iLen));
    m_bitBuffer <<= iLen;
    m_numBits -= iLen;
  }
  else
  {
    m_bitBuffer >>= 32 - m_numBits;

    while(m_numBits <= 24)
    {
      m_bitBuffer <<= 8;
      status = Read(&iByte, 1);
      if(ExcStatusOk != status)
        return status;

      m_bitBuffer |= iByte;
      m_numBits += 8;
    }

    m_bitBuffer <<= (32 - m_numBits);

    pVal = (Ipp8u)(m_bitBuffer >> (32 - iLen));
    m_bitBuffer <<= iLen;
    m_numBits -= iLen;
  }

  return ExcStatusOk;
} // CBitStreamInput::ReadBits()


ExcStatus CBitStreamInput::ReadBits(Ipp8u iLen, Ipp16u& pVal)
{
  ExcStatus status;
  Ipp8u iByte0;
  Ipp8u iByte1;

  if(iLen == 0)
    return ExcStatusOk;

  if(iLen <= 16)
  {
    if(iLen <= 8)
    {
      status = ReadBits(iLen, iByte0);
      pVal = iByte0;
    }
    else
    {
      status = ReadBits(8, iByte0);
      status = ReadBits(iLen - 8, iByte1);

      pVal = (iByte0 << (iLen - 8)) | iByte1;
    }
  }
  else
    return ExcStatusFail;

  return status;
} // CBitStreamInput::ReadBits()


ExcStatus CBitStreamInput::ReadBits(Ipp8u iLen, Ipp32u& pVal)
{
  ExcStatus status;
  Ipp16u iWord0;
  Ipp16u iWord1;

  if(iLen == 0)
    return ExcStatusOk;

  if(iLen <= 32)
  {
    if(iLen <= 16)
    {
      status = ReadBits(iLen, iWord0);
      pVal = iWord0;
    }
    else
    {
      status = ReadBits(16, iWord0);
      status = ReadBits(iLen - 16, iWord1);

      pVal = (iWord0 << (iLen - 16)) | iWord1;
    }
  }
  else
    return ExcStatusFail;

  return status;
} // CBitStreamInput::ReadBits()


ExcStatus CBitStreamInput::FlushToByte(void)
{
  Ipp32u iBTF = m_numBits % 8;

  if(iBTF)
  {
    m_bitBuffer <<= iBTF;
    m_numBits    -= iBTF;
  }

  return ExcStatusOk;
} // CBitStreamInput::FlushToByte()


ExcStatus CBitStreamInput::Flush(Ipp8u iLen)
{
  Ipp32u iBin;

  if(m_numBits >= iLen)
  {
    m_bitBuffer <<= iLen;
    m_numBits -= iLen;
  }
  else
    ReadBits(iLen, iBin);

  return ExcStatusOk;
} // CBitStreamInput::Flush()


ExcStatus CBitStreamInput::Read(Ipp8u* outBuf, Ipp32u iSize)
{
  ExcStatus status;
  Ipp32s iDataLen = m_DataLen - m_currPos;

  if(m_eod)
  {
    if(iDataLen == 0)
      return ExcStatusOk;
    else if(iDataLen < 0)
      return ExcStatusFail;
  }
  else
  {
    if((Ipp32u)iDataLen < iSize)
      status = FillBuffer();
  }

  iDataLen = m_DataLen - m_currPos;
  iSize = (iSize <= (Ipp32u)iDataLen) ? iSize : iDataLen;

  ippsCopy_8u(&m_pData[m_currPos], outBuf, iSize);

  m_currPos  += iSize;
  m_totalPos += iSize;

  return ExcStatusOk;
} // CBitStreamInput::Read()


ExcStatus CBitStreamInput::Read(Ipp8u& iByte)
{
  if(m_numBits == 0)
    return Read(&iByte, 1);
  else
    return ReadBits(8, (Ipp32u&)iByte);
} // CBitStreamInput::Read()


ExcStatus CBitStreamInput::Read(Ipp16u& iWord)
{
  if(m_numBits == 0)
    return Read((Ipp8u*)&iWord, 2);
  else
    return ReadBits(16, (Ipp32u&)iWord);
} // CBitStreamInput::Read()


ExcStatus CBitStreamInput::Read(Ipp32u& iDWord)
{
  if(m_numBits == 0)
    return Read((Ipp8u*)&iDWord, 4);
  else
    return ReadBits(32, iDWord);
} // CBitStreamInput::Read()
