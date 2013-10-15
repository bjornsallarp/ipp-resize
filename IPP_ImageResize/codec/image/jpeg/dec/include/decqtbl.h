/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __DECQTBL_H__
#define __DECQTBL_H__

#ifndef __IPPJ_H__
#include "ippj.h"
#endif
#ifndef __JPEGBASE_H__
#include "jpegbase.h"
#endif




class CJPEGDecoderQuantTable
{
private:
  Ipp8u   m_rbf[DCTSIZE2*sizeof(Ipp16u)+(CPU_CACHE_LINE-1)];
  Ipp8u   m_qbf[DCTSIZE2*sizeof(Ipp32f)+(CPU_CACHE_LINE-1)];
  Ipp16u* m_qnt16u;
  Ipp32f* m_qnt32f;

public:
  int     m_id;
  int     m_precision;
  int     m_initialized;
  Ipp8u*  m_raw8u;
  Ipp16u* m_raw16u;

  CJPEGDecoderQuantTable(void);
  virtual ~CJPEGDecoderQuantTable(void);

  JERRCODE Init(int id,Ipp8u  raw[DCTSIZE2]);
  JERRCODE Init(int id,Ipp16u raw[DCTSIZE2]);
  JERRCODE ConvertToLowPrecision(void);
  JERRCODE ConvertToHighPrecision(void);

  operator Ipp16u*() { return m_precision == 0 ? m_qnt16u : 0; }
  operator Ipp32f*() { return m_precision == 1 ? m_qnt32f : 0; }
};



#endif // __DECQTBL_H__
