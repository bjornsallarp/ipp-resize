/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2001-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __JPEG_H__
#define __JPEG_H__

#ifndef __UIC_BASE_STREAM_INPUT_H__
#include "uic_base_stream_input.h"
#endif
#ifndef __UIC_BASE_STREAM_OUTPUT_H__
#include "uic_base_stream_output.h"
#endif
#ifndef __IPPIMAGE_H__
#include "ippimage.h"
#endif

using namespace UIC;

#define MAX_JPEG_COMMENT_BUF 128

typedef struct _PARAMS_JPEG
{
  int           nthreads;
  int           quality;
  int           restart_interval;
  int           huffman_opt;
  int           point_transform;
  int           predictor;
  int           use_qdct;
  int           dct_scale;
  int           tmode;
  int           mode;
  IM_COLOR      color;
  IM_SAMPLING   sampling;
  int           comment_size;
  Ipp8u         comment[MAX_JPEG_COMMENT_BUF];

} PARAMS_JPEG;


IM_ERROR ReadImageJPEG(BaseStreamInput& in, PARAMS_JPEG& param, CIppImage& image);
IM_ERROR SaveImageJPEG(CIppImage& image, PARAMS_JPEG& param, BaseStreamOutput& out);

#endif // __JPEG_H__
