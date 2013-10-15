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
//
//                    JPEG-XR (HD Photo)
//
//     YUV Resampling functions generation macroses
*/
#include "hdp.h"
#include "pjxr.h"


#define DF_ODD_INT ((((d1 + d2 + d3) << 2) + (d2 << 1) + d0 + d4 + 8) >> 4)
#define DF_ODD_32f ( (d1 + d2 + d3) * 0.25f +  d2 * 0.125f + d0 + d4 )
#define DF_ODD_32s DF_ODD_INT
#define DF_ODD_16s DF_ODD_INT
// horizontal downsampling
#define GEN_YUV444To422_HDP_C1R(FVR)\
IPPFUN(IppStatus, ippiYUV444To422_HDP_##FVR##_C1R,(const Ipp##FVR* pSrc, Ipp32u srcStep,\
    Ipp##FVR* pDst, Ipp32u dstStep, IppiSize dstRoi))\
{\
    const Ipp##FVR *pSrcR = pSrc;\
    Ipp##FVR *pDstW = pDst;\
    int width_1 = dstRoi.width - 1;\
    int y, x;\
    for(y = 0; y < dstRoi.height; y++){\
        Ipp##FVR d0, d1, d2, d3, d4;\
        d0 = d4 = pSrcR[2], d1 = d3 = pSrcR[1], d2 = pSrcR[0]; /* left boundary*/\
\
        for(x = 0; x < width_1; x++){\
        pDstW[x] = DF_ODD_##FVR;\
            d0 = d2, d1 = d3, d2 = d4, d3 = pSrcR[2 * x + 3], d4 = pSrcR[2 * x + 4];\
        }\
        d4 = d2; /* right boundary*/\
        pDstW[x] = DF_ODD_##FVR;\
\
        pSrcR = _NEXTROW(Ipp##FVR,pSrcR,srcStep);\
        pDstW = _NEXTROW(Ipp##FVR,pDstW,dstStep);\
    }\
    return ippStsNoErr;\
}
// vertical downsampling
// dst[n] = DF_ODD(d0=src[2n-4],d1=src[2n-3],d2=src[2n-2],d3=src[2n-1],d4=src[2n])
#define GEN_YUV422To420_HDP_C1R(FVR)\
IPPFUN(IppStatus,  ippiYUV422To420_HDP_##FVR##_C1R, (const Ipp##FVR* pSrc, Ipp32u srcStep,\
    Ipp##FVR* pDst, Ipp32u dstStep, IppiSize dstRoi, IppiBorderType borderType))\
{\
    int x, y;\
    int height_1 = dstRoi.height - ((ippBorderInMemBottom & borderType)? 1 : 2);\
    const Ipp##FVR *pSrc1 = _NEXTROW(Ipp##FVR,pSrc, srcStep);\
    const Ipp##FVR *pSrc2 = _NEXTROW(Ipp##FVR,pSrc1, srcStep);\
    const Ipp##FVR *pSrc_4 = _PEL(Ipp##FVR,pSrc, 0,-4, srcStep);\
    const Ipp##FVR *pSrc_3 = _NEXTROW(Ipp##FVR,pSrc_4, srcStep);\
    const Ipp##FVR *pSrc_2 = _NEXTROW(Ipp##FVR,pSrc_3, srcStep);\
    const Ipp##FVR *pSrc_1 = _NEXTROW(Ipp##FVR,pSrc_2, srcStep);\
\
    if(!(ippBorderInMemTop    & borderType)){ /* top image boundary (border mirrored over 0)*/\
        pSrc_1 = pSrc1;\
        pSrc_2 = pSrc2;\
        pSrc_3 = _NEXTROW(Ipp##FVR,pSrc2, srcStep);\
        pSrc_4 = _NEXTROW(Ipp##FVR,pSrc_3, srcStep);\
    }\
    for(x = 0; x < dstRoi.width; x++){\
        const Ipp##FVR *pSrcR = pSrc1;\
        const Ipp##FVR *pSrcR1 = pSrc2;\
        Ipp##FVR d0 = pSrc_4[x];\
        Ipp##FVR d1 = pSrc_3[x];\
        Ipp##FVR d2 = pSrc_2[x];\
        Ipp##FVR d3 = pSrc_1[x];\
        Ipp##FVR d4 = pSrc[x];\
        Ipp##FVR *pDstW;\
\
        pDstW = pDst;\
        pDstW[x] = DF_ODD_##FVR;\
\
        for(y = 0; y < height_1; y++){\
            d0 = d2, d1 = d3, d2 = d4, d3 = pSrcR[x], d4 = pSrcR1[x];\
            pDstW = _NEXTROW(Ipp##FVR,pDstW, dstStep);\
            pDstW[x] = DF_ODD_##FVR;\
            pSrcR = _NEARROW(Ipp##FVR,pSrcR, 2, srcStep);\
            pSrcR1 = _NEXTROW(Ipp##FVR,pSrcR, srcStep);\
        }\
\
        if(!(ippBorderInMemBottom & borderType)){ /* bottom image boundary*/\
            d0 = d2, d1 = d3, d2 = d4, d3 = pSrcR[x];\
            pDstW = _NEXTROW(Ipp##FVR,pDstW, dstStep);\
            pDstW[x] = DF_ODD_##FVR;\
        }\
    }\
    return ippStsNoErr;\
}

static const int iH[5][4] = {{4, 4 , 0, 8}, {5, 3, 1, 7}, {6, 2, 2, 6}, {7, 1, 3, 5}, {8, 0, 4, 4}};
#define INTERP_INT(a,b,center,i) ( (iH[center][i] * a + iH[center][i+1] * b + 4) >> 3 )
#define INTERP_32s(a,b,center,i)  INTERP_INT(a,b,center,i)
#define INTERP_16s(a,b,center,i)  INTERP_INT(a,b,center,i)
#define INTERP_32f(a,b,center,i) ( (iH[center][i] * a + iH[center][i+1] * b) * 0.125f )

// Reference:
// for (k = 0; k < N; k++){
//    dst[2k]   = interp2(src[k-1],src[k])
//    dst[2k+1] = interp0(src[k],src[k+1])
// }
// 0, N - borders
// src[-1] = src[0]
// src[N]  = src[N-1]


// vertical upsampling
//
#define GEN_YUV420To422_HDP_C1R(FVR)\
IPPFUN(IppStatus, ippiYUV420To422_HDP_##FVR##_C1R,(const Ipp##FVR *pSrc, Ipp32u srcStep,\
    Ipp##FVR *pDst, Ipp32u dstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))\
{\
    int top    = !(ippBorderInMemTop & borderType); \
    int bottom = !(ippBorderInMemBottom & borderType); \
    int iRow, iColumn;\
    for(iColumn = 0; iColumn < srcRoi.width; iColumn++){\
        const Ipp##FVR* pSrcImg_1 = _NEARROW(Ipp##FVR, pSrc, -1, srcStep);   /* src -1 */\
        const Ipp##FVR* pSrcImg = pSrc;                                      /* src  0 */\
        const Ipp##FVR* pSrcImg1 = _NEXTROW(Ipp##FVR, pSrc, srcStep);        /* src  1 */\
        Ipp##FVR* pDstImg = pDst;                                            /* dst  0 */\
        Ipp##FVR* pDstImg1 = _NEXTROW(Ipp##FVR, pDstImg, dstStep);           /* dst  1 */\
        /* iRow = 0 */\
        if (top)\
            pDstImg[iColumn] = pSrcImg[iColumn];\
        else \
            pDstImg[iColumn]  = INTERP_##FVR(pSrcImg_1[iColumn], pSrcImg[iColumn], chromaCentring, 2);\
        \
        pDstImg1[iColumn] = INTERP_##FVR(pSrcImg[iColumn], pSrcImg1[iColumn], chromaCentring, 0);\
        \
        for(iRow = 1; iRow < srcRoi.height-1; iRow++){\
            pSrcImg_1 = pSrcImg;                                  /* src i-1 */\
            pSrcImg   = pSrcImg1;                                 /* src i   */\
            pSrcImg1  = _NEXTROW(Ipp##FVR, pSrcImg1, srcStep);    /* src i+1 */\
            pDstImg   = _NEXTROW(Ipp##FVR, pDstImg1, dstStep);    /* dst 2i  */\
            pDstImg1  = _NEXTROW(Ipp##FVR, pDstImg, dstStep);     /* dst 2i+1 */\
            pDstImg[iColumn]  = INTERP_##FVR(pSrcImg_1[iColumn], pSrcImg[iColumn], chromaCentring, 2);\
            pDstImg1[iColumn] = INTERP_##FVR(pSrcImg[iColumn], pSrcImg1[iColumn], chromaCentring, 0);\
        }\
        pSrcImg_1 = pSrcImg;                                  /* src N-2 */\
        pSrcImg   = pSrcImg1;                                 /* src N-1 */\
        pSrcImg1  = _NEXTROW(Ipp##FVR, pSrcImg1, srcStep);    /* src N   */\
        pDstImg   = _NEARROW(Ipp##FVR, pDstImg, 2, dstStep);  /* dst 2N-2  */\
        pDstImg1   = _NEXTROW(Ipp##FVR, pDstImg, dstStep);    /* dst 2N-1 */\
        /* iRow = srcRoi.height-1 */\
        pDstImg[iColumn]  = INTERP_##FVR(pSrcImg_1[iColumn], pSrcImg[iColumn], chromaCentring, 2);\
        if (bottom)\
            pDstImg1[iColumn] = pSrcImg[iColumn];\
        else \
            pDstImg1[iColumn] = INTERP_##FVR(pSrcImg[iColumn], pSrcImg1[iColumn], chromaCentring, 0);\
    }\
    return ippStsNoErr;\
}

// horizontal upsampling HDP
#define GEN_YUV422To444_HDP_C1R(FVR)\
IPPFUN(IppStatus, ippiYUV422To444_HDP_##FVR##_C1R,(const Ipp##FVR *pSrc, Ipp32u srcStep,\
    Ipp##FVR *pDst, Ipp32u dstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))\
{\
    int left  = !(ippBorderInMemLeft & borderType); \
    int right = !(ippBorderInMemRight & borderType); \
    int x, y;\
    \
    for(y = 0; y < srcRoi.height; y++){\
        pDst[2 * srcRoi.width - 2] = INTERP_##FVR(pSrc[srcRoi.width - 2], pSrc[srcRoi.width - 1], chromaCentring, 2);\
        if(right) \
            pDst[2 * srcRoi.width - 1] = pSrc[srcRoi.width - 1];\
        else \
            pDst[2 * srcRoi.width - 1] = INTERP_##FVR(pSrc[srcRoi.width - 1], pSrc[srcRoi.width], chromaCentring, 0);\
        \
        for(x = srcRoi.width - 2; x > 0; x--){\
            pDst[2 * x + 1] = INTERP_##FVR(pSrc[x    ], pSrc[x + 1], chromaCentring, 0);\
            pDst[2 * x]     = INTERP_##FVR(pSrc[x - 1], pSrc[x    ], chromaCentring, 2);\
        }\
        \
        if(left) \
            pDst[0]     = pSrc[0];\
        else \
            pDst[0]     = INTERP_##FVR(pSrc[-1], pSrc[0], chromaCentring, 2);\
        pDst[1]     = INTERP_##FVR(pSrc[0] , pSrc[1], chromaCentring, 0);\
        pSrc = _NEXTROW(Ipp##FVR,pSrc, srcStep);\
        pDst = _NEXTROW(Ipp##FVR,pDst, dstStep);\
    }\
    return ippStsNoErr;\
}
// horizontal upsampling in-place

#define GEN_YUV422To444_HDP_C1IR(FVR)\
IPPFUN(IppStatus, ippiYUV422To444_HDP_##FVR##_C1IR,(Ipp##FVR *pSrcDst, Ipp32u srcDstStep, \
    IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))\
{\
    int left  = !(ippBorderInMemLeft & borderType); \
    int right = !(ippBorderInMemRight & borderType); \
    int x, y;\
    \
    for(y = 0; y < srcRoi.height; y++){\
        /* in-place: backward order processing is a must    */\
        if(right) \
            pSrcDst[2 * srcRoi.width - 1] = pSrcDst[srcRoi.width - 1];\
        else \
            pSrcDst[2 * srcRoi.width - 1] = INTERP_##FVR(pSrcDst[srcRoi.width - 1], pSrcDst[srcRoi.width], chromaCentring, 0);\
        \
        pSrcDst[2 * srcRoi.width - 2] = INTERP_##FVR(pSrcDst[srcRoi.width - 2], pSrcDst[srcRoi.width - 1], chromaCentring, 2);\
        for(x = srcRoi.width - 2; x > 0; x--){\
            pSrcDst[2 * x + 1] = INTERP_##FVR(pSrcDst[x    ], pSrcDst[x + 1], chromaCentring, 0);\
            pSrcDst[2 * x]     = INTERP_##FVR(pSrcDst[x - 1], pSrcDst[x    ], chromaCentring, 2);\
        }\
        pSrcDst[1]     = INTERP_##FVR(pSrcDst[0] , pSrcDst[1], chromaCentring, 0);\
        if(!left) \
            pSrcDst[0]    = INTERP_##FVR(pSrcDst[-1], pSrcDst[0], chromaCentring, 2);\
        pSrcDst = _NEXTROW(Ipp##FVR,pSrcDst, srcDstStep);\
    }\
    return ippStsNoErr;\
}

