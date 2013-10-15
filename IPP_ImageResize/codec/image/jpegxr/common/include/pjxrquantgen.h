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
//    Encode quantization functions generators
*/

#include "hdp.h"
#include "pjxr.h"
// QUANT_Mulless_INT(x, o, sfs, m) == ((( (abs(x) + (o)) >> (sfs)) ^ (m)) - (m))
#define QOFFSET_INT(x,o,m)  (((x) ^ (m)) - (m) + (o)) // abs(x) + o
#define QRSHIFT_INT(x, sfs, m) ((( (x) >> (sfs)) ^ (m)) - (m));
#define QUANT_Mulless_INT(x, o, sfs, m) QRSHIFT_INT(QOFFSET_INT(x,o,m),sfs,m)


#ifdef _BIG__ENDIAN_
#define iLong0 0
#else
#define iLong0 1
#endif

#define  MUL_32U(x, man, dst)   dst = (Ipp32u)(((Ipp64u)(x) * (man)) >> 32)
#define   QUANT_INT(x, o, man, sfs, m)\
{\
    Ipp32u xQ;\
    MUL_32U(QOFFSET_INT(x,o,m), man, xQ);\
    x = QRSHIFT_INT(xQ, sfs, m);\
}
#define QUANT_MB_INT(x,off,man,sfs)\
{\
    int m = x>>31;\
    if(man == 0){\
        x = QUANT_Mulless_INT(x, off, sfs, m);\
    }else{\
        QUANT_INT(x, off, man, sfs, m);\
    }\
}
#define QUANT_MB_16s(x,off,man,sfs) QUANT_MB_INT(x,off,man,sfs)
#define QUANT_MB_32s(x,off,man,sfs) QUANT_MB_INT(x,off,man,sfs)
#define QUANT_MB(FVR,x,off,man,sfs) QUANT_MB_##FVR(x,off,man,sfs)

#define QUANT_MB_0_INT(x,off,sfs)\
{\
    int m = x>>31;\
    x = QUANT_Mulless_INT(x, off, sfs, m);\
}
#define QUANT_MB_0_16s(x,off,sfs) QUANT_MB_0_INT(x,off,sfs)
#define QUANT_MB_0_32s(x,off,sfs) QUANT_MB_0_INT(x,off,sfs)
#define QUANT_MB_0(FVR,x,off,sfs) QUANT_MB_0_##FVR(x,off,sfs)

#define QUANT_MB_1_INT(x,off,man,sfs)\
{\
    int m = x>>31;\
    QUANT_INT(x, off, man, sfs, m);\
}
#define QUANT_MB_1_16s(x,off,man,sfs) QUANT_MB_1_INT(x,off,man,sfs)
#define QUANT_MB_1_32s(x,off,man,sfs) QUANT_MB_1_INT(x,off,man,sfs)
#define QUANT_MB_1(FVR,x,off,man,sfs) QUANT_MB_1_##FVR(x,off,man,sfs)

#define GEN_ownQuantAll(FVR)\
LOCFUN(void, ownQuantAll_##FVR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u* pQnts, int wBlk, int hBlk))\
{\
    int i, j;\
    /* DC0 */\
    Ipp32u off0 = pQnts[0].offset;\
    Ipp32u man0 = pQnts[0].mantissa;\
    Ipp32u exp0 = pQnts[0].exponent;\
    /* DCs other */\
    Ipp32u off1 = pQnts[1].offset;\
    Ipp32u man1 = pQnts[1].mantissa;\
    Ipp32u exp1 = pQnts[1].exponent;\
    /* PCT coeffs */\
    Ipp32u off2 = pQnts[2].offset;\
    Ipp32u man2 = pQnts[2].mantissa;\
    Ipp32u exp2 = pQnts[2].exponent;\
\
    Ipp##FVR *p00 = pSrcDst;\
    Ipp##FVR *p0,*p1,*p2,*p3;\
\
    /* 16, 8 or 4 4x4 block per MB, each has single DC (left-top coeff) and 15 PCTs */\
    /* DC of left-top block is quantized with different offset, mantissa and exponent than other DCs */\
    /* PCT are quantized with offset, mantissa and exponent */\
    /* Quantized independently in arbitrary order */\
\
    if(0 == man0 && 0 == man1 && 0 == man2) {/* high spot: quality = lossless, 2,4,8,16*/\
        for(i = 0; i < hBlk; i++ ){\
            p0 = p00;\
            p1 = _NEXTROW(Ipp##FVR,p0,srcDstStep);\
            p2 = _NEXTROW(Ipp##FVR,p1,srcDstStep);\
            p3 = _NEXTROW(Ipp##FVR,p2,srcDstStep);\
            p00 =  _NEXTROW(Ipp##FVR,p3,srcDstStep);\
            for(j = 0; j < wBlk; j++, p0+=4,p1+=4,p2+=4,p3+=4,exp0 = exp1, off0 = off1){\
                /* LP DCs */\
                QUANT_MB_0(FVR,p0[0],off0,exp0);\
                /* HP PCT coeffs */\
                QUANT_MB_0(FVR,p0[1],off2,exp2)\
                QUANT_MB_0(FVR,p0[2],off2,exp2)\
                QUANT_MB_0(FVR,p0[3],off2,exp2)\
\
                QUANT_MB_0(FVR,p1[0],off2,exp2)\
                QUANT_MB_0(FVR,p1[1],off2,exp2)\
                QUANT_MB_0(FVR,p1[2],off2,exp2)\
                QUANT_MB_0(FVR,p1[3],off2,exp2)\
\
                QUANT_MB_0(FVR,p2[0],off2,exp2)\
                QUANT_MB_0(FVR,p2[1],off2,exp2)\
                QUANT_MB_0(FVR,p2[2],off2,exp2)\
                QUANT_MB_0(FVR,p2[3],off2,exp2)\
\
                QUANT_MB_0(FVR,p3[0],off2,exp2)\
                QUANT_MB_0(FVR,p3[1],off2,exp2)\
                QUANT_MB_0(FVR,p3[2],off2,exp2)\
                QUANT_MB_0(FVR,p3[3],off2,exp2)\
            }\
       }\
    }else{\
        for(i = 0; i < hBlk; i++){\
            p0 = p00;\
            p1 = _NEXTROW(Ipp##FVR,p0,srcDstStep);\
            p2 = _NEXTROW(Ipp##FVR,p1,srcDstStep);\
            p3 = _NEXTROW(Ipp##FVR,p2,srcDstStep);\
            p00 =  _NEXTROW(Ipp##FVR,p3,srcDstStep);\
            for(j = 0; j < wBlk; j++, p0+=4,p1+=4,p2+=4,p3+=4, exp0 = exp1, off0 = off1, man0 = man1){\
                /* LP DCs */\
                QUANT_MB_1(FVR,p0[0],off0,man0,exp0)\
                /* HP PCT coeffs */\
                QUANT_MB_1(FVR,p0[1],off2,man2,exp2)\
                QUANT_MB_1(FVR,p0[2],off2,man2,exp2)\
                QUANT_MB_1(FVR,p0[3],off2,man2,exp2)\
\
                QUANT_MB_1(FVR,p1[0],off2,man2,exp2)\
                QUANT_MB_1(FVR,p1[1],off2,man2,exp2)\
                QUANT_MB_1(FVR,p1[2],off2,man2,exp2)\
                QUANT_MB_1(FVR,p1[3],off2,man2,exp2)\
\
                QUANT_MB_1(FVR,p2[0],off2,man2,exp2)\
                QUANT_MB_1(FVR,p2[1],off2,man2,exp2)\
                QUANT_MB_1(FVR,p2[2],off2,man2,exp2)\
                QUANT_MB_1(FVR,p2[3],off2,man2,exp2)\
\
                QUANT_MB_1(FVR,p3[0],off2,man2,exp2)\
                QUANT_MB_1(FVR,p3[1],off2,man2,exp2)\
                QUANT_MB_1(FVR,p3[2],off2,man2,exp2)\
                QUANT_MB_1(FVR,p3[3],off2,man2,exp2)\
            }\
        }\
    }\
}
#define GEN_ownQuantLP(FVR)\
LOCFUN(void, ownQuantLP_##FVR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, int wBlk, int hBlk))\
{\
    int i, j;\
    /* DC0*/\
    Ipp32u off0 = pQnts[0].offset;\
    Ipp32u man0 = pQnts[0].mantissa;\
    Ipp32u exp0 = pQnts[0].exponent;\
    /* DCs other*/\
    Ipp32u off1 = pQnts[1].offset;\
    Ipp32u man1 = pQnts[1].mantissa;\
    Ipp32u exp1 = pQnts[1].exponent;\
\
    Ipp##FVR *p00 = pSrcDst;\
    Ipp##FVR *p0;\
\
    /* 16, 8 or 4 4x4 block per MB, each has single DC (left-top coeff) and 15 PCTs */\
    /* DC of left-top block is quantized with different offset, mantissa and exponent than other DCs */\
\
    if(0 == man0 && 0 == man1) {/* high spot: quality = lossless, 2,4,8,16 */\
        for(i = 0; i < hBlk; i++ ){\
            p0 = p00;\
            p00 =  _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
            for(j = 0; j < wBlk; j++, p0+=4,exp0 = exp1, off0 = off1){\
                /* LP DCs */\
                QUANT_MB_0(FVR,p0[0],off0,exp0);\
            }\
       }\
    }else{\
        for(i = 0; i < hBlk; i++){\
            p0 = p00;\
            p00 =  _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
            for(j = 0; j < wBlk; j++, p0+=4, exp0 = exp1, off0 = off1, man0 = man1){\
                /* LP DCs */\
                QUANT_MB_1(FVR,p0[0],off0,man0,exp0)\
            }\
        }\
    }\
}
#define GEN_ownQuant_HDP_C1IR(FVR)\
LOCFUN(IppStatus, ownQuant_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, Ipp32u numQnts, int wBlk, int hBlk))\
{\
    /* to check params here */\
    switch(numQnts){\
        case 0:\
            {\
                Ipp32u off0 = pQnts[0].offset;\
                Ipp32u man0 = pQnts[0].mantissa;\
                Ipp32u exp0 = pQnts[0].exponent;\
                /* DC0 */\
                QUANT_MB(FVR, pSrcDst[0],off0, man0, exp0)\
            }\
            break;\
        case 1:\
            ownQuantLP_##FVR(pSrcDst,srcDstStep,pQnts,wBlk,hBlk);\
            break;\
        default:\
            ownQuantAll_##FVR(pSrcDst,srcDstStep,pQnts,wBlk,hBlk);\
            break;\
    }\
    return ippStsNoErr;\
}

#define GEN_ippQuant16x16_HDP_C1IR(FVR)\
IPPFUN(IppStatus, ippiQuant16x16_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, Ipp32u numQnts))\
{\
    return ownQuant_HDP_##FVR##_C1IR(pSrcDst,srcDstStep,pQnts,numQnts,4,4);\
}
#define GEN_ippQuant8x16_HDP_C1IR(FVR)\
IPPFUN(IppStatus, ippiQuant8x16_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, Ipp32u numQnts))\
{\
    return ownQuant_HDP_##FVR##_C1IR(pSrcDst,srcDstStep,pQnts,numQnts,2,4);\
}
#define GEN_ippQuant8x8_HDP_C1IR(FVR)\
IPPFUN(IppStatus, ippiQuant8x8_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, Ipp32u numQnts))\
{\
    return ownQuant_HDP_##FVR##_C1IR(pSrcDst,srcDstStep,pQnts,numQnts,2,2);\
}

