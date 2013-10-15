/*
//
//             INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/
#include "stdafx.h"
#ifndef __BITSTREAMOUT_H__
#include "bitstreamout.h"
#endif


CBitStreamOutput::CBitStreamOutput(void)
{
  m_out     = 0;
  m_pData   = 0;
  m_DataLen = 0;
  m_currPos = 0;
  m_nBytesWritten = 0;

  return;
} // ctor


CBitStreamOutput::~CBitStreamOutput(void)
{
  Detach();
  return;
} // dtor


JERRCODE CBitStreamOutput::Attach(UIC::BaseStreamOutput* out)
{
  Detach();

  m_out = out;

  return JPEG_OK;
} // CBitStreamOutput::Attach()


JERRCODE CBitStreamOutput::Detach(void)
{
  if(0 != m_pData)
  {
    // deallocate internal memory
    ippFree(m_pData);
  }

  m_out     = 0;
  m_pData   = 0;
  m_DataLen = 0;
  m_currPos = 0;

  m_nBytesWritten = 0;

  return JPEG_OK;
} // CBitStreamOutput::Detach()


JERRCODE CBitStreamOutput::Init(int bufSize)
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

  m_currPos = 0; // no data yet

  m_nBytesWritten = 0;

  return JPEG_OK;
} // CBitStreamOutput::Init()


JERRCODE CBitStreamOutput::FlushBuffer(int nMinBytes)
{
  int   remainder;
  UIC::BaseStream::TSize cnt;

  if(m_currPos > m_DataLen)
    return JPEG_ERR_BUFF;

  if(nMinBytes)
  {
    remainder = m_DataLen - m_currPos;

    if(remainder > nMinBytes)
      return JPEG_OK;
  }

  m_out->Write(m_pData,m_currPos,cnt);
  if((int)cnt != m_currPos)
    return JPEG_ERR_FILE;

  m_nBytesWritten += m_currPos;
  m_currPos = 0;

  return JPEG_OK;
} // CBitStreamOutput::FlushBuffer()


JERRCODE CBitStreamOutput::FlushBitStream(CBitStreamOutput &bitStream)
{
  Ipp8u* buf     = 0;
  int    currPos  = 0;
  int    dataLen = 0;

  UIC::BaseStream::TSize cnt = 0;

  buf     = bitStream.GetDataPtr();
  currPos = bitStream.GetCurrPos();
  dataLen = bitStream.GetDataLen();

  if(currPos > dataLen)
    return JPEG_ERR_BUFF;

  m_out->Write(buf,currPos,cnt);
  if((int)cnt != currPos)
    return JPEG_ERR_FILE;

  m_nBytesWritten += currPos;
  m_currPos = 0;
  bitStream.SetCurrPos(0);

  return JPEG_OK;
} // CBitStreamOutput::FlushBitStream()


JERRCODE CBitStreamOutput::WriteByte(int byte)
{
  JERRCODE jerr;

  if(m_currPos >= m_DataLen)
  {
    jerr = FlushBuffer();
    if(JPEG_OK != jerr)
      return jerr;
  }

  m_pData[m_currPos] = (Ipp8u)byte;
  m_currPos++;

  return JPEG_OK;
} // CBitStreamOutput::WriteByte()


JERRCODE CBitStreamOutput::WriteWord(int word)
{
  int byte0;
  int byte1;
  JERRCODE jerr;

  byte0 = word >> 8;
  byte1 = word & 0x00ff;

  jerr = WriteByte(byte0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = WriteByte(byte1);
  if(JPEG_OK != jerr)
    return jerr;

  return JPEG_OK;
} // CBitStreamOutput::WriteWord()


JERRCODE CBitStreamOutput::WriteDword(int dword)
{
  int word0;
  int word1;
  JERRCODE jerr;

  word0 = dword >> 16;
  word1 = dword & 0x0000ffff;

  jerr = WriteWord(word0);
  if(JPEG_OK != jerr)
    return jerr;

  jerr = WriteWord(word1);
  if(JPEG_OK != jerr)
    return jerr;

  return JPEG_OK;
} // CBitStreamOutput::WriteDword()

