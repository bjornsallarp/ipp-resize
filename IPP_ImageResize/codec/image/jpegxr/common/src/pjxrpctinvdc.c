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
//  Purpose:
//    HDP PCT, second stage inverse transform
*/

#include "hdp.h"
#include "pjxr.h"
#include "pjxrpctgeninvdc.h"

/* /////////////////////////////////////////////////////////////////////////////
//         Photo Core Transform, Inverse, 2nd stage
*/

GEN_ippPCTInv16x16DC_HDP_C1IR(16s)    //    ippiPCTInv16x16DC_HDP_16s_C1IR
GEN_ippPCTInv8x16DC_HDP_C1IR(16s)     //    ippiPCTInv8x16DC_HDP_16s_C1IR
GEN_ippPCTInv8x8DC_HDP_C1IR(16s)      //    ippiPCTInv8x8DC_HDP_16s_C1IR

GEN_ippPCTInv16x16DC_HDP_C1IR(32s)    //    ippiPCTInv16x16DC_HDP_32s_C1IR
GEN_ippPCTInv8x16DC_HDP_C1IR(32s)     //    ippiPCTInv8x16DC_HDP_32s_C1IR
GEN_ippPCTInv8x8DC_HDP_C1IR(32s)      //    ippiPCTInv8x8DC_HDP_32s_C1IR

GEN_ippPCTInv16x16DC_HDP_C1IR(32f)    //    ippiPCTInv16x16DC_HDP_32f_C1IR
GEN_ippPCTInv8x16DC_HDP_C1IR(32f)     //    ippiPCTInv8x16DC_HDP_32f_C1IR
GEN_ippPCTInv8x8DC_HDP_C1IR(32f)      //    ippiPCTInv8x8DC_HDP_32f_C1IR
