/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2009-2012 Intel Corporation. All Rights Reserved.
//
//          Intel(R) Integrated Performance Primitives
//
//                    JPEG-XR (HD Photo)
//
//    YUV Resampling functions
//  Contents:
//       ippiYUV444To422_HDP_16s_C1R
//       ippiYUV444To420_HDP_16s_C1R
//       ippiYUV444To422_HDP_16s_C1R
//       ippiYUV444To424_HDP_16s_C1R
//       ippiYUV444To424_HDP_16s_C1IR
//       ippiYUV444To422_HDP_32s_C1R
//       ippiYUV444To422_HDP_32s_C1R
//       ippiYUV444To422_HDP_32s_C1R
//       ippiYUV444To422_HDP_32s_C1R
//       ippiYUV444To422_HDP_32s_C1IR
//
*/
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning ( disable : 4244 )
#endif
#include "hdp.h"
#include "pjxr.h"
#include "pjxrresgen.h"

GEN_YUV444To422_HDP_C1R(16s)  // ippiYUV444To422_HDP_16s_C1R
GEN_YUV422To420_HDP_C1R(16s)  // ippiYUV422To420_HDP_16s_C1R
GEN_YUV420To422_HDP_C1R(16s)  // ippiYUV420To422_HDP_16s_C1R
GEN_YUV422To444_HDP_C1R(16s)  // ippiYUV422To444_HDP_16s_C1R
GEN_YUV422To444_HDP_C1IR(16s) // ippiYUV422To444_HDP_16s_C1IR

GEN_YUV444To422_HDP_C1R(32s)  // ippiYUV444To422_HDP_32s_C1R
GEN_YUV422To420_HDP_C1R(32s)  // ippiYUV422To420_HDP_32s_C1R
GEN_YUV420To422_HDP_C1R(32s)  // ippiYUV420To422_HDP_32s_C1R
GEN_YUV422To444_HDP_C1R(32s)  // ippiYUV422To444_HDP_32s_C1R
GEN_YUV422To444_HDP_C1IR(32s) // ippiYUV422To444_HDP_32s_C1IR

GEN_YUV444To422_HDP_C1R(32f)  // ippiYUV444To422_HDP_32f_C1R
GEN_YUV422To420_HDP_C1R(32f)  // ippiYUV422To420_HDP_32f_C1R
GEN_YUV420To422_HDP_C1R(32f)  // ippiYUV420To422_HDP_32f_C1R
GEN_YUV422To444_HDP_C1R(32f)  // ippiYUV422To444_HDP_32f_C1R
GEN_YUV422To444_HDP_C1IR(32f) // ippiYUV422To444_HDP_32f_C1IR

