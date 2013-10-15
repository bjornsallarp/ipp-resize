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
//     PCT, second stage forward transform
*/

#include "hdp.h"
#include "pjxr.h"
#include "pjxrpctgenfwddc.h"

/* /////////////////////////////////////////////////////////////////////////////
//         Photo Core Transform, Inverse/Forward, 2nd stage
*/

GEN_ippPCTFwd16x16DC_HDP_C1IR(16s)   // ippiPCTFwd16x16DC_HDP_16s_C1IR
GEN_ippPCTFwd8x16DC_HDP_C1IR(16s)    // ippiPCTFwd8x16DC_HDP_16s_C1IR
GEN_ippPCTFwd8x8DC_HDP_C1IR(16s)     // ippiPCTFwd8x8DC_HDP_16s_C1IR

GEN_ippPCTFwd16x16DC_HDP_C1IR(32s)   // ippiPCTFwd16x16DC_HDP_32s_C1IR
GEN_ippPCTFwd8x16DC_HDP_C1IR(32s)    // ippiPCTFwd8x16DC_HDP_32s_C1IR
GEN_ippPCTFwd8x8DC_HDP_C1IR(32s)     // ippiPCTFwd8x8DC_HDP_32s_C1IR

GEN_ippPCTFwd16x16DC_HDP_C1IR(32f)   // ippiPCTFwd16x16DC_HDP_32f_C1IR
GEN_ippPCTFwd8x16DC_HDP_C1IR(32f)    // ippiPCTFwd8x16DC_HDP_32f_C1IR
GEN_ippPCTFwd8x8DC_HDP_C1IR(32f)     // ippiPCTFwd8x8DC_HDP_32f_C1IR
