/*
//               INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//       Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/
#include "stdafx.h"
#ifndef __BITSTREAMIN_H__
#include "bitstreamin.h"
#endif




CBitStreamInput::CBitStreamInput(void)
{
  m_in      = 0;
  m_pData   = 0;
  m_DataLen = 0;
  m_currPos = 0;
  m_eod     = 0;

  return;
} // ctor


CBitStreamInput::~CBitStreamInput(void)
{
  Detach();
  return;
} // dtor


JERRCODE CBitStreamInput::Attach(CBaseStreamInput* in)
{
  Detach();

  m_in = in;

  return JPEG_OK;
} // CBitStreamInput::Attach()


JERRCODE CBitStreamInput::Attach(CBitStreamInput* in)
{
  Detach();

  m_in = in->GetStream();

  return JPEG_OK;
} // CBitStreamInput::Attach()


JERRCODE CBitStreamInput::Detach(void)
{
  if(0 != m_pData)
  {
    // deallocate internal memory
    ippFree(m_pData);
    m_pData = 0;
  }

  m_in      = 0;
  m_pData   = 0;
  m_DataLen = 0;
  m_currPos = 0;
  m_eod     = 0;

  m_bufOffset = 0;

  return JPEG_OK;
} // CBitStreamInput::Detach()


JERRCODE CBitStreamInput::Init(int bufSize)
{
  m_DataLen = (int)bufSize;

  if(0 != m_pData)
  {
    ippFree(m_pData);
    m_pData = 0;
  }

  m_pData = (Ipp8u*)ippMalloc(m_DataLen);
  if(0 == m_pData)
  {
    return JPEG_ERR_ALLOC;
  }

  m_currPos = m_DataLen; // no data yet

  return JPEG_OK;
} // CBitStreamInput::Init()


JERRCODE CBitStreamInput::FillBuffer(int nMinBytes)
{
  int   remainder;
  UIC::BaseStream::TSize cnt;

  remainder = m_DataLen - m_currPos;

  if(nMinBytes)
  {
    if(remainder >= nMinBytes)
      return JPEG_OK;
  }

  if(remainder && !m_eod)
  {
    ippsMove_8u(&m_pData[m_currPos],m_pData,remainder);
    m_currPos = 0;
  }

  if(!m_eod)
  {
    m_in->Read(m_pData + remainder,m_DataLen - remainder,cnt);
    if((int)cnt != m_DataLen - remainder)
      m_eod = 1;

    m_currPos = 0;
    m_DataLen = remainder + (int)cnt;
  }
  else
  {
    m_eod = 1;
  }

  if(m_eod && m_currPos >= m_DataLen)
  {
    return JPEG_ERR_BUFF;
  }

  return JPEG_OK;
} // CBitStreamInput::FillBuffer()


JERRCODE CBitStreamInput::Seek(UIC::BaseStream::TOffset offset, UIC::BaseStreamInput::SeekOrigin origin)
{
  int _offset;
  JERRCODE jerr;
  UIC::BaseStream::TStatus status;

  switch(origin)
  {
    case UIC::BaseStreamInput::Current:
    {
      _offset = m_currPos + (int)offset;

      if(_offset > 0 && _offset < m_DataLen)
      {
        m_currPos = _offset;
      }
      else
      {
        m_currPos = m_DataLen;
        status = m_in->Seek(_offset - m_DataLen,UIC::BaseStreamInput::Current);
        if(UIC::BaseStream::StatusOk != status)
          return JPEG_ERR_FILE;

        jerr = FillBuffer();
        if(JPEG_OK != jerr)
          return jerr;
      }
      break;
    }

  case UIC::BaseStreamInput::Beginning:
    {
      m_currPos = m_DataLen;
      status = m_in->Seek(offset,UIC::BaseStreamInput::Beginning);
      if(UIC::BaseStream::StatusOk != status)
        return JPEG_ERR_FILE;

      jerr = FillBuffer();
      if(JPEG_OK != jerr)
        return jerr;
    }
    break;

  case UIC::BaseStreamInput::End:
  default:
    return JPEG_NOT_IMPLEMENTED;
  }

  return JPEG_OK;
} // CBitStreamInput::Seek()


JERRCODE CBitStreamInput::CheckByte(int pos, int* byte)
{
  JERRCODE jerr;

  if(m_currPos + pos >= m_DataLen)
  {
    //jerr = Seek(pos);
    jerr = FillBuffer();
    if(JPEG_OK != jerr)
      return jerr;

    //*byte = m_pData[0];
    *byte = m_pData[pos];
  }
  else
    *byte = m_pData[m_currPos + pos];

  return JPEG_OK;
} // CBitStreamInput::CheckByte()


JERRCODE CBitStreamInput::ReadByte(int* byte)
{
  JERRCODE jerr;

  if(m_currPos >= m_DataLen)
  {
    jerr = FillBuffer();
    if(JPEG_OK != jerr)
      return jerr;
  }

  *byte = m_pData[m_currPos];
  m_currPos++;

  return JPEG_OK;
} // CBitStreamInput::ReadByte()


JERRCODE CBitStreamInput::ReadWord(int* word)
{
  int byte0;
  int byte1;
  JERRCODE jerr;

  jerr = ReadByte(&byte0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = ReadByte(&byte1);
  if(JPEG_OK != jerr)
    return jerr;

  *word = (byte0 << 8) | byte1;

  return JPEG_OK;
} // CBitStreamInput::ReadWord()


JERRCODE CBitStreamInput::ReadDword(int* dword)
{
  int word0;
  int word1;
  JERRCODE jerr;

  jerr = ReadWord(&word0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = ReadWord(&word1);
  if(JPEG_OK != jerr)
    return jerr;

  *dword = (word0 << 16) | word1;

  return JPEG_OK;
} // CBitStreamInput::ReadDword()


int CBitStreamInput::GetStreamPos(void)
{
  UIC::BaseStream::TPosition pos = 0;

  m_in->Position(pos);

  return (int)(pos - (m_DataLen - m_currPos));
} // CBitStreamInput::GetStreamPos()


JERRCODE CBitStreamInput::FillBitStream(CBitStreamInput* bitStream, int rstiStart, int rstiSize)
{
  int                        offset;
  Ipp8u*                     streamBuf = 0;
  UIC::BaseStream::TSize     cnt;
  UIC::BaseStream::TStatus   status;
  UIC::BaseStream::TPosition pos;

  pos    = 0;

  m_in->Position(pos);

  offset = (int)pos - rstiStart;

  status = m_in->Seek(-offset, UIC::BaseStreamInput::Current); // Seek <- in BaseStream to start of current RSTI
  if(UIC::BaseStream::StatusOk  != status)
    return JPEG_ERR_FILE;

  streamBuf = bitStream->GetDataPtr();

  status = m_in->Read(streamBuf, rstiSize, cnt);
  if(UIC::BaseStream::StatusOk  != status)
    return JPEG_ERR_FILE;

  status = m_in->Seek(offset - rstiSize, UIC::BaseStreamInput::Current); // Seek -> to current position in BaseStream
  if(UIC::BaseStream::StatusOk  != status)
    return JPEG_ERR_FILE;

  return JPEG_OK;
} // CBitStreamInput::FillBitStream()


