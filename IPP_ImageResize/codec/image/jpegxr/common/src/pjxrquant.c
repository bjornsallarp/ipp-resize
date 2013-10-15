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
//    Quantization
//  Contents:
//    ippiQuant16x16_HDP_32s_C1IR        ippiQuant16x16_HDP_16s_C1IR
//    ippiQuant8x16_HDP_32s_C1IR         ippiQuant8x16_HDP_16s_C1IR
//    ippiQuant8x8_HDP_32s_C1IR          ippiQuant8x8_HDP_16s_C1IR
//
*/
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning ( disable : 4244 )
#endif

#include "hdp.h"
#include "pjxr.h"
#include "pjxrquantgen.h"

GEN_ownQuantAll(16s)
GEN_ownQuantLP(16s)
GEN_ownQuant_HDP_C1IR(16s)
GEN_ippQuant16x16_HDP_C1IR(16s)  // ippiQuant16x16_HDP_16s_C1IR
GEN_ippQuant8x16_HDP_C1IR(16s)   // ippiQuant8x16_HDP_16s_C1IR
GEN_ippQuant8x8_HDP_C1IR(16s)    // ippiQuant8x8_HDP_16s_C1IR

GEN_ownQuantAll(32s)
GEN_ownQuantLP(32s)
GEN_ownQuant_HDP_C1IR(32s)
GEN_ippQuant16x16_HDP_C1IR(32s) // ippiQuant16x16_HDP_32s_C1IR
GEN_ippQuant8x16_HDP_C1IR(32s)  // ippiQuant8x16_HDP_32s_C1IR
GEN_ippQuant8x8_HDP_C1IR(32s)   // ippiQuant8x8_HDP_32s_C1IR
