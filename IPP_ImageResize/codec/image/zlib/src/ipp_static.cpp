/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2011-2012 Intel Corporation. All Rights Reserved.
//
//     Intel(R) Integrated Performance Primitives Ipp_zlib Sample
//
//  By downloading and installing this sample, you hereby agree that the
//  accompanying Materials are being provided to you under the terms and
//  conditions of the End User License Agreement for the Intel(R) Integrated
//  Performance Primitives product previously accepted by you. Please refer
//  to the file ippEULA.rtf located in the root directory of your Intel(R) IPP
//  product installation for more information.
//
*/

#include "ippcore.h"

extern "C" {
  int link_to_ipp_zlib;
}

struct IPP_ZLIB_STATIC_INIT {
  int tmp;
  IPP_ZLIB_STATIC_INIT(int tmp_init){this->tmp = tmp_init; ippStaticInit();}
};

IPP_ZLIB_STATIC_INIT IPP_ZLIB_X(link_to_ipp_zlib);
