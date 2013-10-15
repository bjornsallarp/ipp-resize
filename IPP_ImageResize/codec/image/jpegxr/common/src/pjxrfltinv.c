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
//    PCT 1st stage inverse filters 
//  Contents:
//    ippiFilterInv_HDP_16s_C1IR         ippiFilterInv_HDP_32s_C1IR         ippiFilterInv_HDP_32f_C1IR
//    ippiFilterInvDCYUV444_HDP_16s_C1IR ippiFilterInvDCYUV444_HDP_32s_C1IR ippiFilterInvDCYUV444_HDP_32f_C1IR
//    ippiFilterInvDCYUV420_HDP_16s_C1IR ippiFilterInvDCYUV420_HDP_32s_C1IR ippiFilterInvDCYUV420_HDP_32f_C1IR
*/
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning ( disable : 4100 )
#endif
#include "hdp.h"
#include "pjxr.h"
#include "pjxrfltgen.h"

/* /////////////////////////////////////////////////////////////////////////////
//               Photo Core Transform Inverse filters
///////////////////////////////////////////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////////
                 Procedure generation
*/
GEN_ownCornerInvFlt2_HDP(16s)
GEN_ownCornerInvFlt8_HDP(16s)
GEN_ownHorInvFlt2_HDP(16s)
GEN_ownVertInvFlt2_HDP(16s)
GEN_ownCenterInvFlt2_HDP(16s)
GEN_ownHorInvFlt4_HDP(16s)
GEN_ownVertInvFlt4_HDP(16s)
GEN_ownCenterInvFlt4_HDP(16s)
GEN_ownHorInvFlt8_HDP(16s)
GEN_ownVertInvFlt8_HDP(16s)
GEN_ownCenterInvFlt8_HDP(16s)

GEN_ownCornerInvFlt2_HDP(32s)
GEN_ownCornerInvFlt8_HDP(32s)
GEN_ownHorInvFlt2_HDP(32s)
GEN_ownVertInvFlt2_HDP(32s)
GEN_ownCenterInvFlt2_HDP(32s)
GEN_ownHorInvFlt4_HDP(32s)
GEN_ownVertInvFlt4_HDP(32s)
GEN_ownCenterInvFlt4_HDP(32s)
GEN_ownHorInvFlt8_HDP(32s)
GEN_ownVertInvFlt8_HDP(32s)
GEN_ownCenterInvFlt8_HDP(32s)

GEN_ownCornerInvFlt2_HDP(32f)
GEN_ownCornerInvFlt8_HDP(32f)
GEN_ownHorInvFlt2_HDP(32f)
GEN_ownVertInvFlt2_HDP(32f)
GEN_ownCenterInvFlt2_HDP(32f)
GEN_ownHorInvFlt4_HDP(32f)
GEN_ownVertInvFlt4_HDP(32f)
GEN_ownCenterInvFlt4_HDP(32f)
GEN_ownHorInvFlt8_HDP(32f)
GEN_ownVertInvFlt8_HDP(32f)
GEN_ownCenterInvFlt8_HDP(32f)

/* /////////////////////////////////////////////////////////////////////////////
// PCT inverse filter public functions:
*/
/*
   1st stage
*/

#define IPPiFilterInv_C1IR(FVR) \
IPPFUN(IppStatus, ippiFilterInv_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))\
{ \
    GenFilter(Inv,2,FVR);\
    return ippStsNoErr;\
}
IPPiFilterInv_C1IR(16s) // ippiFilterInv_HDP_16s_C1IR
IPPiFilterInv_C1IR(32s) // ippiFilterInv_HDP_32s_C1IR
IPPiFilterInv_C1IR(32f) // ippiFilterInv_HDP_32f_C1IR

/* /////////////////////////////////////////////////////////////////////////////
//                         PCT inverse filter, 2nd stage
*/

#define IPPiFilterInvDCYUV_C1IR(FVR,YUV,N) \
IPPFUN(IppStatus, ippiFilterInvDCYUV##YUV##_HDP_##FVR##_C1IR,(Ipp##FVR *pSrcDst,\
       Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))\
{\
    GenFilterDC(Inv,N,FVR,YUV);\
\
    return ippStsNoErr;\
}
IPPiFilterInvDCYUV_C1IR(16s,444,8) // ippiFilterInvDCYUV444_HDP_16s_C1IR
IPPiFilterInvDCYUV_C1IR(32s,444,8) // ippiFilterInvDCYUV444_HDP_32s_C1IR
IPPiFilterInvDCYUV_C1IR(32f,444,8) // ippiFilterInvDCYUV444_HDP_32f_C1IR
IPPiFilterInvDCYUV_C1IR(16s,420,4) // ippiFilterInvDCYUV420_HDP_16s_C1IR
IPPiFilterInvDCYUV_C1IR(32s,420,4) // ippiFilterInvDCYUV420_HDP_32s_C1IR
IPPiFilterInvDCYUV_C1IR(32f,420,4) // ippiFilterInvDCYUV420_HDP_32f_C1IR
