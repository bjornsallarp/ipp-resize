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
//     PCT 1st stage forward filters
//
//  Contents:
//    ippiFilterFwd_HDP_32s_C1IR
//    ippiFilterFwd16x16DC_HDP_32s_C1IR
//    ippiFilterFwd8x16DC_HDP_32s_C1IR
//    ippiFilterFwd8x8DC_HDP_32s_C1IR
//
*/
#if defined(_WIN32) && defined(_MSC_VER)
#pragma warning ( disable : 4100 )
#endif
#include "hdp.h"
#include "pjxr.h"
#include "pjxrfltgen.h"

/* /////////////////////////////////////////////////////////////////////////////
//               Photo Core Transform Forward filters
///////////////////////////////////////////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////////
//                  PCT Forward filter,  1st stage
*/
GEN_ownCornerFwdFlt2_HDP(16s)
GEN_ownCornerFwdFlt8_HDP(16s)
GEN_ownHorFwdFlt2_HDP(16s)
GEN_ownVertFwdFlt2_HDP(16s)
GEN_ownCenterFwdFlt2_HDP(16s)
GEN_ownHorFwdFlt4_HDP(16s)
GEN_ownVertFwdFlt4_HDP(16s)
GEN_ownCenterFwdFlt4_HDP(16s)
GEN_ownHorFwdFlt8_HDP(16s)
GEN_ownVertFwdFlt8_HDP(16s)
GEN_ownCenterFwdFlt8_HDP(16s)

GEN_ownCornerFwdFlt2_HDP(32s)
GEN_ownCornerFwdFlt8_HDP(32s)
GEN_ownHorFwdFlt2_HDP(32s)
GEN_ownVertFwdFlt2_HDP(32s)
GEN_ownCenterFwdFlt2_HDP(32s)
GEN_ownHorFwdFlt4_HDP(32s)
GEN_ownVertFwdFlt4_HDP(32s)
GEN_ownCenterFwdFlt4_HDP(32s)
GEN_ownHorFwdFlt8_HDP(32s)
GEN_ownVertFwdFlt8_HDP(32s)
GEN_ownCenterFwdFlt8_HDP(32s)

GEN_ownCornerFwdFlt2_HDP(32f)
GEN_ownCornerFwdFlt8_HDP(32f)
GEN_ownHorFwdFlt2_HDP(32f)
GEN_ownVertFwdFlt2_HDP(32f)
GEN_ownCenterFwdFlt2_HDP(32f)
GEN_ownHorFwdFlt4_HDP(32f)
GEN_ownVertFwdFlt4_HDP(32f)
GEN_ownCenterFwdFlt4_HDP(32f)
GEN_ownHorFwdFlt8_HDP(32f)
GEN_ownVertFwdFlt8_HDP(32f)
GEN_ownCenterFwdFlt8_HDP(32f)


/* /////////////////////////////////////////////////////////////////////////////
//                         PCT forward filter, 1st stage
*/
#define IPPiFilterFwd_C1IR(FVR) \
IPPFUN(IppStatus, ippiFilterFwd_HDP_##FVR##_C1IR,(Ipp##FVR* pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))\
{ \
    GenFilter(Fwd,2,FVR);\
    return ippStsNoErr;\
}

IPPiFilterFwd_C1IR(16s) // ippiFilterFwd_HDP_16s_C1IR
IPPiFilterFwd_C1IR(32s) // ippiFilterFwd_HDP_32s_C1IR
IPPiFilterFwd_C1IR(32f) // ippiFilterFwd_HDP_32f_C1IR

/* /////////////////////////////////////////////////////////////////////////////
//                         PCT forward DC filters, 2nd stage
*/

#define IPPiFilterFwdDCYUV_C1IR(FVR,YUV,N) \
IPPFUN(IppStatus, ippiFilterFwdDCYUV##YUV##_HDP_##FVR##_C1IR, (Ipp##FVR* pSrcDst,\
       Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))\
{ \
    GenFilterDC(Fwd,N,FVR,YUV);  \
    return ippStsNoErr;\
}
IPPiFilterFwdDCYUV_C1IR(16s,444,8) // ippiFilterFwdDCYUV444_HDP_16s_C1I
IPPiFilterFwdDCYUV_C1IR(32s,444,8) // ippiFilterFwdDCYUV444_HDP_32s_C1I
IPPiFilterFwdDCYUV_C1IR(32f,444,8) // ippiFilterFwdDCYUV444_HDP_32f_C1I
IPPiFilterFwdDCYUV_C1IR(16s,420,4) // ippiFilterFwdDCYUV420_HDP_16s_C1I
IPPiFilterFwdDCYUV_C1IR(32s,420,4) // ippiFilterFwdDCYUV420_HDP_32s_C1I
IPPiFilterFwdDCYUV_C1IR(32f,420,4) // ippiFilterFwdDCYUV420_HDP_32f_C1I

