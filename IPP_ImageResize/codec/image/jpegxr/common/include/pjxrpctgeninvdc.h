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
//     PCT, second stage inverse transform functions macro generator by flavor
*/

#include "hdp.h"
#include "pjxr.h"

/* /////////////////////////////////////////////////////////////////////////////
//                       Photo Core Transform
//                         Inverse, 2nd stage
*/
/*
// Public function:
        PCT inverse 16x16 DC,  2nd stage
*/

#define GEN_ippPCTInv16x16DC_HDP_C1IR(FVR)\
IPPFUN(IppStatus, ippiPCTInv16x16DC_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))\
{\
\
    Ipp##FVR  *p0 = pSrcDst;\
    Ipp##FVR  *p4 = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
    Ipp##FVR *p08 = _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
    Ipp##FVR *p12 = _NEXTROW4(Ipp##FVR,p08,srcDstStep);\
\
    /** bottom left corner, pi/8 rotation => butterfly **/\
    invOdd_##FVR(p08[0], p12[0], p08[4], p12[4]);\
    /** top right corner, butterfly => pi/8 rotation **/\
    invOdd_##FVR(p0[8], p0[12], p4[8], p4[12]);\
    /** bottom right corner, pi/8 rotation => pi/8 rotation **/\
    invOddOdd_##FVR(p08[8], p08[12], p12[8], p12[12]);\
    /** top left corner, butterfly => butterfly **/\
    strDCT2x2up_##FVR(p0[0], p0[4], p4[0], p4[4]);\
    /** butterfly **/\
    strDCT2x2dn_##FVR(p0[0], p0[12], p12[0], p12[12]);\
    strDCT2x2dn_##FVR(p0[4], p0[ 8], p12[4], p12[ 8]);\
    strDCT2x2dn_##FVR(p4[0], p4[12], p08[0], p08[12]);\
    strDCT2x2dn_##FVR(p4[4], p4[ 8], p08[4], p08[ 8])\
\
    if (upscale) {\
        SCALE_UP4x4_##FVR(p0,p4,0,upscale);\
        SCALE_UP4x4_##FVR(p0,p4,8,upscale);\
        SCALE_UP4x4_##FVR(p08,p12,0,upscale);\
        SCALE_UP4x4_##FVR(p08,p12,8,upscale);\
    }\
\
    return ippStsNoErr;\
}
/*
// Public function:
        PCT inverse 8x16 DC,  2nd stage
*/
#define HT1DINV_INT(px,py)     px[0] -= ((py[0] + 1) >> 1)
#define HT1DINV_FP(px,py)     px[0] -= py[0] * 0.5f
#define HT1DINV_16s(px,py) HT1DINV_INT(px,py)
#define HT1DINV_32s(px,py) HT1DINV_INT(px,py)
#define HT1DINV_32f(px,py) HT1DINV_FP(px,py)

#define GEN_ippPCTInv8x16DC_HDP_C1IR(FVR)\
IPPFUN(IppStatus, ippiPCTInv8x16DC_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))\
{\
    Ipp##FVR  *p0 = pSrcDst;\
    Ipp##FVR  *p4 = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
    Ipp##FVR *p08 = _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
    Ipp##FVR *p12 = _NEXTROW4(Ipp##FVR,p08,srcDstStep);\
\
    /* 1D lossless HT */\
    HT1DINV_##FVR(p0,p08);\
    p08[0] += p0[0];\
\
    strDCT2x2dn_##FVR(p0[0], p0[4],  p4[0] ,  p4[4]);\
    strDCT2x2dn_##FVR(p08[0], p08[4], p12[0] , p12[4]);\
    if (upscale) {\
        SCALE_UP4x4_##FVR(p0,p4,0,upscale);\
        SCALE_UP4x4_##FVR(p08,p12,0,upscale);\
    }\
    return ippStsNoErr;\
}
/*
// Public function:
        PCT inverse 8x8 DC,  2nd stage
*/
#define GEN_ippPCTInv8x8DC_HDP_C1IR(FVR)\
IPPFUN(IppStatus, ippiPCTInv8x8DC_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))\
{\
    Ipp##FVR  *p0 = pSrcDst;\
    Ipp##FVR  *p4 = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
\
    strDCT2x2dn_##FVR(p0[0], p4[0], p0[4], p4[4]);\
    if (upscale) {\
        SCALE_UP4x4_##FVR(p0,p4,0,upscale);\
    }\
\
    return ippStsNoErr;\
}
