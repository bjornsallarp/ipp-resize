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
//    PCT, second stage forward transform functions macro generator by flavor
*/

#include "hdp.h"
#include "pjxr.h"

/* /////////////////////////////////////////////////////////////////////////////
//                       Photo Core Transform
//                         Forward, 2nd stage
*/

/*
// Public function:
//      PCT forward 16x16 DC,  2nd stage
*/

#define GEN_ippPCTFwd16x16DC_HDP_C1IR(FVR)\
IPPFUN(IppStatus, ippiPCTFwd16x16DC_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))\
{\
    Ipp##FVR  *p0 = pSrcDst;\
    Ipp##FVR  *p4 = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
    Ipp##FVR *p08 = _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
    Ipp##FVR *p12 = _NEXTROW4(Ipp##FVR,p08,srcDstStep);\
\
    if (downscale) {\
        SCALE_DOWN4x4_##FVR(p0,p4,0,downscale);\
        SCALE_DOWN4x4_##FVR(p0,p4,8,downscale);\
        SCALE_DOWN4x4_##FVR(p08,p12,0,downscale);\
        SCALE_DOWN4x4_##FVR(p08,p12,8,downscale);\
    }\
    /** butterfly **/\
    strDCT2x2dn_##FVR(p0[0], p0[12], p12[0], p12[12]);\
    strDCT2x2dn_##FVR(p0[4], p0[ 8], p12[4], p12[ 8]);\
    strDCT2x2dn_##FVR(p4[0], p4[12], p08[0], p08[12]);\
    strDCT2x2dn_##FVR(p4[4], p4[ 8], p08[4], p08[ 8])\
\
    /** top left corner, butterfly => butterfly **/\
    strDCT2x2up_##FVR(p0[0], p0[4], p4[0], p4[4]);\
\
    /** bottom right corner, pi/8 rotation => pi/8 rotation **/\
    fwdOddOdd_##FVR(p08[8], p08[12], p12[8], p12[12]);\
\
    /** top right corner, butterfly => pi/8 rotation **/\
    fwdOdd_##FVR(p0[8], p0[12], p4[8], p4[12]);\
\
    /** bottom left corner, pi/8 rotation => butterfly **/\
    fwdOdd_##FVR(p08[0], p12[0], p08[4], p12[4]);\
\
    return ippStsNoErr;\
}
/*
// Public function:
//      PCT forward 8x16 DC,  2nd stage
*/
#define HT1DFWD_INT(px,py)     px[0] += ((py[0] + 1) >> 1)
#define HT1DFWD_FP(px,py)     px[0] += py[0] * 0.5f
#define HT1DFWD_16s(px,py) HT1DFWD_INT(px,py)
#define HT1DFWD_32s(px,py) HT1DFWD_INT(px,py)
#define HT1DFWD_32f(px,py) HT1DFWD_FP(px,py)

#define GEN_ippPCTFwd8x16DC_HDP_C1IR(FVR)\
IPPFUN(IppStatus, ippiPCTFwd8x16DC_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))\
{\
    Ipp##FVR  *p0 = pSrcDst;\
    Ipp##FVR  *p4 = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
    Ipp##FVR *p08 = _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
    Ipp##FVR *p12 = _NEXTROW4(Ipp##FVR,p08,srcDstStep);\
\
    if (downscale) {\
        SCALE_DOWN4x4_##FVR(p0,p4,0,downscale);\
        SCALE_DOWN4x4_##FVR(p08,p12,0,downscale);\
    }\
    strDCT2x2dn_##FVR(p0[0], p0[4],  p4[0] ,  p4[4]);\
    strDCT2x2dn_##FVR(p08[0], p08[4], p12[0] , p12[4]);\
\
    /* 1D lossless HT */\
    p08[0] -= p0[0];\
    HT1DFWD_##FVR(p0,p08);\
\
    return ippStsNoErr;\
}
/*
// Public function:
//      PCT forward 8x8 DC,  2nd stage
*/
#define GEN_ippPCTFwd8x8DC_HDP_C1IR(FVR)\
IPPFUN(IppStatus, ippiPCTFwd8x8DC_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))\
{\
    Ipp##FVR  *p0 = pSrcDst;\
    Ipp##FVR  *p4 = _PEL(Ipp##FVR,pSrcDst,0,4,srcDstStep);\
\
    if (downscale) {\
        SCALE_DOWN4x4_##FVR(p0,p4,0,downscale);\
    }\
    strDCT2x2dn_##FVR(p0[0], p4[0], p0[4], p4[4]);\
\
    return ippStsNoErr;\
}



