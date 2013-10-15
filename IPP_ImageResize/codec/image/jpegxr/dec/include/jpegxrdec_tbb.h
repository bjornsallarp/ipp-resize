/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __JPEGXRDEC_TBB_H__
#define __JPEGXRDEC_TBB_H__

#include "jpegxrdec.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/pipeline.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include "tbb/spin_mutex.h"

using namespace UIC;


class CTBBFrame
{
public:
  CTBBFrame(CJPEGXRTileDecoder* pDecoder, Ipp32u iThreads, tbb::spin_mutex* mutex);
  void operator()(const tbb::blocked_range2d<Ipp16u> &r) const;

private:
  CJPEGXRTileDecoder* m_pDecoder;
  Ipp32u              m_iThreads;
  tbb::spin_mutex*    m_Mutex;
};


class CTBBHuffmanFilter: public tbb::filter
{
public:
  CTBBHuffmanFilter(CJPEGXRTileDecoder* pDecoder);

private:
  CJPEGXRTileDecoder* m_pDecoder;

private:
  void* operator()(void*);
};


class CTBBPCTFilter: public tbb::filter
{
public:
  CTBBPCTFilter(CJPEGXRTileDecoder* pDecoder);

private:
  CJPEGXRTileDecoder* m_pDecoder;

private:
  void* operator()(void*);
};


class CTBBCCFilter: public tbb::filter
{
public:
  CTBBCCFilter(CJPEGXRTileDecoder* pDecoder);

private:
  CJPEGXRTileDecoder* m_pDecoder;

private:
  void* operator()(void*);
};


#endif // __JPEGXRDEC_TBB_H__
