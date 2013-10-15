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
//     PCT Filters forward and inverse functions generation macroses
*/
#if !defined( __PJXRFLTGEN_H__ )
#define __PJXRFLTGEN_H__

#define GEN_ownCornerInvFlt2_HDP(FVR)\
LOCFUN(void, ownCornerInvFilter2_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep))\
{\
    Ipp##FVR *p0 = pSrcDst;\
    Ipp##FVR *p1 = _NEXTROW(Ipp##FVR,p0,srcDstStep);\
    strPost4_##FVR(p0[0], p0[1], p1[0], p1[1]);\
}
#define GEN_ownCornerInvFlt8_HDP(FVR)\
LOCFUN(void, ownCornerInvFilter8_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep))\
{\
    Ipp##FVR *p0 = pSrcDst;\
    Ipp##FVR *p4 = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
    strPost4_##FVR(p0[0], p0[4], p4[0], p4[4]);\
}


#define GEN_ownHorInvFlt2_HDP(FVR)\
LOCFUN(void, ownHorInvFilter2_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR *p0 = pSrcDst;\
    Ipp##FVR *p1 = _NEXTROW(Ipp##FVR,p0,srcDstStep);\
    for(i=0; i< nBlks; i++, p0+=4, p1+=4){\
        strPost4_##FVR(p0[0], p0[1], p0[2], p0[3]);\
        strPost4_##FVR(p1[0], p1[1], p1[2], p1[3]);\
    }\
}
#define GEN_ownVertInvFlt2_HDP(FVR)\
LOCFUN(void, ownVertInvFilter2_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR *p00 = pSrcDst;\
    for(i=0; i< nBlks; i++){\
        Ipp##FVR *p0 = p00;\
        Ipp##FVR *p1 = _NEXTROW(Ipp##FVR,p0,srcDstStep);\
        Ipp##FVR *p2 = _NEXTROW(Ipp##FVR,p1,srcDstStep);\
        Ipp##FVR *p3 = _NEXTROW(Ipp##FVR,p2,srcDstStep);\
        p00 = _NEXTROW(Ipp##FVR,p3,srcDstStep);\
        strPost4_##FVR(p0[0], p1[0], p2[0], p3[0]);\
        strPost4_##FVR(p0[1], p1[1], p2[1], p3[1]);\
    }\
}

#define GEN_ownCenterInvFlt2_HDP(FVR)\
LOCFUN(void,ownCenterInvFilter2_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep, int wBlks, int hBlks))\
{\
    int i,j;\
    Ipp##FVR *p00 = pSrcDst;\
    for(i=0; i< hBlks; i++)\
    {\
        Ipp##FVR *p0 = p00;\
        Ipp##FVR *p1 = _NEXTROW(Ipp##FVR,p0,srcDstStep);\
        Ipp##FVR *p2 = _NEXTROW(Ipp##FVR,p1,srcDstStep);\
        Ipp##FVR *p3 = _NEXTROW(Ipp##FVR,p2,srcDstStep);\
        p00 = _NEXTROW(Ipp##FVR,p3,srcDstStep);\
\
        for(j=0; j< wBlks; j++, p0+=4,p1+=4,p2+=4,p3+=4)\
        {\
            /** butterfly **/\
            strDCT2x2dn_##FVR(p1[1], p1[2], p2[1], p2[2]);\
            strDCT2x2dn_##FVR(p1[0], p1[3], p2[0], p2[3]);\
            strDCT2x2dn_##FVR(p0[1], p0[2], p3[1], p3[2]);\
            strDCT2x2dn_##FVR(p0[0], p0[3], p3[0], p3[3]);\
\
            /** bottom right corner: -pi/8 rotation => -pi/8 rotation **/\
            invOddOddPost_##FVR(p2[2], p2[3], p3[2], p3[3]);\
\
            /** anti diagonal corners: rotation by -pi/8 **/\
            IROTATE1_##FVR(p3[1], p3[0]);\
            IROTATE1_##FVR(p2[1], p2[0]);\
            IROTATE1_##FVR(p1[3], p0[3]);\
            IROTATE1_##FVR(p1[2], p0[2]);\
\
            /** butterfly **/\
            strHSTdec_##FVR(p1[1], p1[2], p2[1], p2[2]);\
            strHSTdec_##FVR(p1[0], p1[3], p2[0], p2[3]);\
            strHSTdec_##FVR(p0[1], p0[2], p3[1], p3[2]);\
            strHSTdec_##FVR(p0[0], p0[3], p3[0], p3[3]);\
        }\
    }\
}

#define GEN_ownCornerFwdFlt2_HDP(FVR)\
LOCFUN(void, ownCornerFwdFilter2_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep))\
{\
    Ipp##FVR *p0 = pSrcDst;\
    Ipp##FVR *p1 = _NEXTROW(Ipp##FVR,p0,srcDstStep);\
    strPre4_##FVR(p0[0], p0[1], p1[0], p1[1]);\
}
#define GEN_ownCornerFwdFlt8_HDP(FVR)\
LOCFUN(void, ownCornerFwdFilter8_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep))\
{\
    Ipp##FVR *p0 = pSrcDst;\
    Ipp##FVR *p4 = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
    strPre4_##FVR(p0[0], p0[4], p4[0], p4[4]);\
}

#define GEN_ownHorFwdFlt2_HDP(FVR)\
LOCFUN(void, ownHorFwdFilter2_##FVR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR *p0 = pSrcDst;\
    Ipp##FVR *p1 = _NEXTROW(Ipp##FVR,p0,srcDstStep);\
    for(i=0; i< nBlks; i++, p0+=4, p1+=4){\
        strPre4_##FVR(p0[0], p0[1], p0[2], p0[3]);\
        strPre4_##FVR(p1[0], p1[1], p1[2], p1[3]);\
    }\
}
#define GEN_ownVertFwdFlt2_HDP(FVR)\
LOCFUN(void, ownVertFwdFilter2_##FVR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR *p0 = pSrcDst;\
    for(i=0; i< nBlks; i++, p0 = _NEXTROW4(Ipp##FVR,p0,srcDstStep)){\
        Ipp##FVR *p1, *p2, *p3;\
        p1 = _NEXTROW(Ipp##FVR,p0,srcDstStep);\
        p2 = _NEXTROW(Ipp##FVR,p1,srcDstStep);\
        p3 = _NEXTROW(Ipp##FVR,p2,srcDstStep);\
        strPre4_##FVR(p0[0], p1[0], p2[0], p3[0]);\
        strPre4_##FVR(p0[1], p1[1], p2[1], p3[1]);\
   }\
}
#define GEN_ownCenterFwdFlt2_HDP(FVR)\
LOCFUN(void, ownCenterFwdFilter2_##FVR,(Ipp##FVR* pSrcDst, int srcDstStep, int wBlks, int hBlks))\
{\
    int i,j;\
    Ipp##FVR *p00 = pSrcDst;\
    for(i=0; i< hBlks; i++, p00 = _NEXTROW4(Ipp##FVR,p00,srcDstStep))\
    {\
        Ipp##FVR *p0 = p00;\
        Ipp##FVR *p1 = _NEXTROW(Ipp##FVR,p0,srcDstStep);\
        Ipp##FVR *p2 = _NEXTROW(Ipp##FVR,p1,srcDstStep);\
        Ipp##FVR *p3 = _NEXTROW(Ipp##FVR,p2,srcDstStep);\
\
        for(j=0; j< wBlks; j++, p0+=4,p1+=4,p2+=4,p3+=4)\
        {\
\
            /** butterfly & scaling **/\
            strHSTenc_##FVR(p1[1], p1[2], p2[1], p2[2], 1);\
            strHSTenc_##FVR(p1[0], p1[3], p2[0], p2[3], 1);\
            strHSTenc_##FVR(p0[1], p0[2], p3[1], p3[2], 1);\
            strHSTenc_##FVR(p0[0], p0[3], p3[0], p3[3], 1);\
\
            /** anti diagonal corners: rotation by pi/8 **/\
            ROTATE1_##FVR(p3[1],  p3[0]);\
            ROTATE1_##FVR(p2[1],  p2[0]);\
            ROTATE1_##FVR(p1[3],  p0[3]);\
            ROTATE1_##FVR(p1[2],  p0[2]);\
\
            /** bottom right corner: pi/8 rotation => pi/8 rotation **/\
            fwdOddOddPre_##FVR(p2[2], p2[3], p3[2], p3[3]);\
\
            /** butterfly **/\
            strDCT2x2dn_##FVR(p1[1], p1[2], p2[1], p2[2]);\
            strDCT2x2dn_##FVR(p1[0], p1[3], p2[0], p2[3]);\
            strDCT2x2dn_##FVR(p0[1], p0[2], p3[1], p3[2]);\
            strDCT2x2dn_##FVR(p0[0], p0[3], p3[0], p3[3]);\
        }\
    }\
}
#define GEN_ownCenterFwdFlt8_HDP(FVR)\
LOCFUN(void, ownCenterFwdFilter8_##FVR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, int wBlks, int hBlks))\
{\
    int i,j;\
    Ipp##FVR *p00  = pSrcDst;\
    for(i=0; i< hBlks; i++)\
    {\
        Ipp##FVR *p0 = p00;\
        Ipp##FVR *p4  = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
        Ipp##FVR *p08 = _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
        Ipp##FVR *p12 = _NEXTROW4(Ipp##FVR,p08,srcDstStep);\
        p00 = _NEXTROW4(Ipp##FVR,p12,srcDstStep);\
\
        for(j=0; j< wBlks; j++, p0+=16,p4+=16,p08+=16,p12+=16)\
        {\
            /** butterfly **/\
            strHSTenc_##FVR(p0[0], p0[12], p12[0], p12[12], 1);\
            strHSTenc_##FVR(p0[4], p0[ 8], p12[4], p12[ 8], 1);\
            strHSTenc_##FVR(p4[0], p4[12], p08[0], p08[12], 1);\
            strHSTenc_##FVR(p4[4], p4[ 8], p08[4], p08[ 8], 1);\
\
            /** anti diagonal corners: rotation **/\
            ROTATE1_##FVR(p12[4], p12[0]);\
            ROTATE1_##FVR(p08[4], p08[0]);\
            ROTATE1_##FVR(p4[12], p0[12]);\
            ROTATE1_##FVR(p4[ 8], p0[ 8]);\
\
            /** bottom right corner: pi/8 rotation => pi/8 rotation **/\
            fwdOddOddPre_##FVR(p08[8], p08[12], p12[8], p12[12]);\
\
            /** butterfly **/\
            strDCT2x2dn_##FVR(p0[0], p0[12], p12[0], p12[12]);\
            strDCT2x2dn_##FVR(p0[4], p0[ 8], p12[4], p12[ 8]);\
            strDCT2x2dn_##FVR(p4[0], p4[12], p08[0], p08[12]);\
            strDCT2x2dn_##FVR(p4[4], p4[ 8], p08[4], p08[ 8]);\
        }\
    }\
}

#define GEN_ownVertFwdFlt8_HDP(FVR)\
LOCFUN(void, ownVertFwdFilter8_##FVR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR *p0  = pSrcDst;\
    for(i=0; i< nBlks; i++ ){\
        Ipp##FVR *p4  = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
        Ipp##FVR *p8  = _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
        Ipp##FVR *p12 = _NEXTROW4(Ipp##FVR,p8,srcDstStep);\
\
        strPre4_##FVR(p0[0], p4[0], p8[0], p12[0]);\
        strPre4_##FVR(p0[4], p4[4], p8[4], p12[4]);\
\
        p0 = _NEXTROW4(Ipp##FVR,p12,srcDstStep);\
    }\
}

#define GEN_ownHorFwdFlt8_HDP(FVR)\
LOCFUN(void, ownHorFwdFilter8_##FVR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR *p0  = pSrcDst;\
    Ipp##FVR *p4  = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
    Ipp##FVR *p8  = _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
    Ipp##FVR *p12 = _NEXTROW4(Ipp##FVR,p8,srcDstStep);\
    for(i=0; i< nBlks; i++, p0+=4,p4+=4,p8+=4,p12+=4){\
        strPre4_##FVR(p0[0], p0[4], p0[8], p0[12]);\
        strPre4_##FVR(p4[0], p4[4], p4[8], p4[12]);\
    }\
}

#define GEN_ownVertFwdFlt4_HDP(FVR)\
LOCFUN(void, ownVertFwdFilter4_##FVR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR* p00 =  pSrcDst;\
    for(i=0; i<nBlks; i++){\
        Ipp##FVR* p0 =  p00;\
        Ipp##FVR* p4 =  _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
        p00 = _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
        strPre2_##FVR(p0[0], p4[0]);\
    }\
}
#define GEN_ownHorFwdFlt4_HDP(FVR)\
LOCFUN(void, ownHorFwdFilter4_##FVR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR* p0 =  pSrcDst;\
    for(i=0; i<nBlks; i++, p0+=8){\
        strPre2_##FVR(p0[0], p0[4]);\
    }\
}
#define GEN_ownCenterFwdFlt4_HDP(FVR)\
LOCFUN(void, ownCenterFwdFilter4_##FVR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, int wBlks, int hBlks))\
{\
    int i,j;\
    Ipp##FVR* p00 = pSrcDst;\
    for(i=0; i<hBlks; i++){\
        Ipp##FVR* p0 =  p00;\
        Ipp##FVR* p4  =  _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
        p00 =  _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
        for(j=0; j<wBlks; j++,p0+=8,p4+=8){ /* process in raster order*/\
            strPre2x2_##FVR(p0[0], p0[4], p4[0], p4[4]);\
        }\
    }\
}

#define GEN_ownCenterInvFlt8_HDP(FVR)\
LOCFUN(void, ownCenterInvFilter8_##FVR,(Ipp##FVR *pSrcDst, Ipp32u srcDstStep, int wBlks, int hBlks))\
{\
    int i,j;\
    Ipp##FVR *p00  = pSrcDst;\
\
    for(i=0; i< hBlks; i++)\
    {\
        Ipp##FVR *p0  = p00;\
        Ipp##FVR *p4  = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
        Ipp##FVR *p8  = _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
        Ipp##FVR *p12 = _NEXTROW4(Ipp##FVR,p8,srcDstStep);\
        p00 = _NEXTROW4(Ipp##FVR,p12,srcDstStep);\
\
        for(j=0; j< wBlks; j++, p0+=16,p4+=16,p8+=16,p12+=16){\
\
            /** butterfly **/\
            strDCT2x2dn_##FVR(p0[0], p0[12], p12[0], p12[12]);\
            strDCT2x2dn_##FVR(p0[4], p0[ 8], p12[4], p12[ 8]);\
            strDCT2x2dn_##FVR(p4[0], p4[12], p8 [0], p8 [12]);\
            strDCT2x2dn_##FVR(p4[4], p4[ 8], p8 [4], p8 [ 8]);\
\
            /** bottom right corner: -pi/8 rotation => -pi/8 rotation **/\
            invOddOddPost_##FVR(p8[8], p8[12], p12[8], p12[12]);\
\
            /** anti diagonal corners: rotation by -pi/8 **/\
            IROTATE1_##FVR(p4 [ 8], p0 [ 8]);\
            IROTATE1_##FVR(p4 [12], p0 [12]);\
            IROTATE1_##FVR(p8 [ 4], p8 [ 0]);\
            IROTATE1_##FVR(p12[ 4], p12[ 0]);\
\
            /** butterfly **/\
            strHSTdec_##FVR(p0[0], p0[12], p12[0], p12[12]);\
            strHSTdec_##FVR(p0[4], p0[ 8], p12[4], p12[ 8]);\
            strHSTdec_##FVR(p4[0], p4[12], p8 [0], p8 [12]);\
            strHSTdec_##FVR(p4[4], p4[ 8], p8 [4], p8 [ 8]);\
        }\
    }\
}

#define GEN_ownHorInvFlt8_HDP(FVR)\
LOCFUN(void, ownHorInvFilter8_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR *p0 =  pSrcDst;\
    Ipp##FVR *p4 =  _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
    for(i=0;i<nBlks;i++, p0+=16,p4+=16){\
        strPost4_##FVR(p0[0], p0[4], p0[8], p0[12]);\
        strPost4_##FVR(p4[0], p4[4], p4[8], p4[12]);\
    }\
}
#define GEN_ownVertInvFlt8_HDP(FVR)\
LOCFUN(void, ownVertInvFilter8_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR *p00 =  pSrcDst;\
    for(i=0;i<nBlks;i++){\
        Ipp##FVR *p0 =  p00;\
        Ipp##FVR *p4  = _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
        Ipp##FVR *p8  = _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
        Ipp##FVR *p12 = _NEXTROW4(Ipp##FVR,p8,srcDstStep);\
        p00 = _NEXTROW4(Ipp##FVR,p12,srcDstStep);\
        strPost4_##FVR(p0[0], p4[0], p8[0], p12[0]);\
        strPost4_##FVR(p0[4], p4[4], p8[4], p12[4]);\
    }\
}

#define GEN_ownHorInvFlt4_HDP(FVR)\
LOCFUN(void, ownHorInvFilter4_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR *p0 = pSrcDst;\
    for(i=0;i<nBlks;i++, p0+=8){\
        strPost2_##FVR(p0[0], p0[4]);\
    }\
}
#define GEN_ownVertInvFlt4_HDP(FVR)\
LOCFUN(void, ownVertInvFilter4_##FVR,(Ipp##FVR *pSrcDst, int srcDstStep, int nBlks))\
{\
    int i;\
    Ipp##FVR *p00 =  pSrcDst;\
    for(i=0;i<nBlks;i++){\
        Ipp##FVR *p0 =  p00;\
        Ipp##FVR *p4 =  _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
        p00 =  _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
        strPost2_##FVR(p0[0], p4[0]);\
    }\
}
#define GEN_ownCenterInvFlt4_HDP(FVR)\
    LOCFUN(void, ownCenterInvFilter4_##FVR,(Ipp##FVR *pSrcDst, Ipp32u srcDstStep, int wBlks, int hBlks))\
{\
    int i,j;\
    Ipp##FVR *p00 =  pSrcDst;\
    for(i=0;i<hBlks;i++){\
        Ipp##FVR *p0 =  p00;\
        Ipp##FVR *p4 =  _NEXTROW4(Ipp##FVR,p0,srcDstStep);\
        p00 =  _NEXTROW4(Ipp##FVR,p4,srcDstStep);\
        for(j=0;j<wBlks;j++, p0+=8,p4+=8){\
            strPost2x2_##FVR(p0[0], p0[4], p4[0], p4[4]);\
        }\
    }\
}
#define GenFilterCorner444(Dir,N,FVR)\
    /* top-left corner*/\
    if(left && top) \
        ownCorner##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,0,0,srcDstStep),srcDstStep);\
    if(left && bottom) \
        ownCorner##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,0,roiSize.height,srcDstStep),srcDstStep);\
    if(right && top) \
        ownCorner##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,roiSize.width,0,srcDstStep),srcDstStep);\
    if(right && bottom) \
        ownCorner##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,roiSize.width,roiSize.height,srcDstStep),srcDstStep)

#define GenFilterCorner422(Dir,N,FVR)
#define GenFilterCorner420(Dir,N,FVR)


#define GenFilter(Dir,N,FVR)\
    int x=0,y=0;\
    int top    = !(ippBorderInMemTop    & borderType);\
    int bottom = !(ippBorderInMemBottom & borderType);\
    int left   = !(ippBorderInMemLeft   & borderType);\
    int right  = !(ippBorderInMemRight  & borderType);\
    int wBlks, hBlks;\
\
    if(left)   roiSize.width -= N, x = N;\
    if(right)  roiSize.width -= N;\
\
    if(top)    roiSize.height -= N, y = N;\
    if(bottom) roiSize.height -= N;\
\
    wBlks = roiSize.width/(2*N);\
    hBlks = roiSize.height/(2*N);\
\
    /* corner */\
    GenFilterCorner444(Dir,N,FVR);\
\
    /* vertical border */\
    if(hBlks){\
        if(left)   ownVert##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,0,y,srcDstStep),srcDstStep,hBlks);\
        if(right)  ownVert##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,2*N*wBlks,y,srcDstStep),srcDstStep,hBlks);\
    }\
\
    /* horizontal border */\
    if(wBlks){\
        if (top)    ownHor##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,x, 0,srcDstStep),srcDstStep,wBlks);\
        if (bottom) ownHor##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,x, 2*N*hBlks+y,srcDstStep),srcDstStep,wBlks);\
    }\
    /* center with no border */\
    if(wBlks && hBlks)\
        ownCenter##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,x,y,srcDstStep),srcDstStep, wBlks, hBlks)


// template function
#define GenFilterDC(Dir,N,FVR,YUV)\
    int x=0,y=0;\
    int top    = !(ippBorderInMemTop    & borderType);\
    int bottom = !(ippBorderInMemBottom & borderType);\
    int left   = !(ippBorderInMemLeft   & borderType);\
    int right  = !(ippBorderInMemRight  & borderType);\
    int wBlks, hBlks;\
\
    if(left)   roiSize.width -= N, x = N;\
    if(right)  roiSize.width -= N;\
\
    if(top)    roiSize.height -= N, y = N;\
    if(bottom) roiSize.height -= N;\
\
    wBlks = roiSize.width/(2*N);\
    hBlks = roiSize.height/(2*N);\
\
    GenFilterCorner##YUV(Dir,N,FVR);\
    /* vertical border */\
    if(hBlks){\
        if(left)   ownVert##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,0,y,srcDstStep),srcDstStep,hBlks);\
        if(right)  ownVert##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,2*N*wBlks,y,srcDstStep),srcDstStep,hBlks);\
    }\
\
    /* horizontal border */\
    if(wBlks){\
        if (top)    ownHor##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,x, 0,srcDstStep),srcDstStep,wBlks);\
        if (bottom) ownHor##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,x, 2*N*hBlks+y,srcDstStep),srcDstStep,wBlks);\
    }\
    /* center with no border */\
    if(wBlks && hBlks)\
        ownCenter##Dir##Filter##N##_##FVR(_PEL(Ipp##FVR,pSrcDst,x,y,srcDstStep),srcDstStep, wBlks, hBlks)


#endif /*__PJXRFLTGEN_H__*/
