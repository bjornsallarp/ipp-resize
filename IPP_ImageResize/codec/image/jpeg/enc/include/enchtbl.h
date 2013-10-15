/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2001-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __ENCHTBL_H__
#define __ENCHTBL_H__

#ifndef __IPPJ_H__
#include "ippj.h"
#endif
#ifndef __JPEGBASE_H__
#include "jpegbase.h"
#endif




class CJPEGEncoderHuffmanTable
{
private:
  IppiEncodeHuffmanSpec* m_table;

public:
  int                    m_id;
  int                    m_hclass;
  Ipp8u                  m_bits[16];
  Ipp8u                  m_vals[256];

  CJPEGEncoderHuffmanTable(void);
  virtual ~CJPEGEncoderHuffmanTable(void);

  JERRCODE Create(void);
  JERRCODE Destroy(void);
  JERRCODE Init(int id,int hclass,Ipp8u* bits,Ipp8u* vals);

  operator IppiEncodeHuffmanSpec*(void) { return m_table; }
};


class CJPEGEncoderHuffmanState
{
private:
  IppiEncodeHuffmanState* m_state;

public:
  CJPEGEncoderHuffmanState(void);
  virtual ~CJPEGEncoderHuffmanState(void);

  JERRCODE Create(void);
  JERRCODE Destroy(void);
  JERRCODE Init(void);

  operator IppiEncodeHuffmanState*(void) { return m_state; }
};


#endif // __ENCHTBL_H__


