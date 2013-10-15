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
//     IPP header
//
*/

#if !defined(__HDP_H__ )
#define __HDP_H__
#include <ippcore.h>
#include <ipps.h>
#include <ippj.h>


#undef __CDECL
#if defined( _WIN32 ) || defined ( _WIN64 )
  #define __CDECL    __cdecl
#else
  #define __CDECL
#endif

#define IPPFUN(type,name,arg)    type __STDCALL name arg
#define OWNAPI( type,name,arg )  type __CDECL name arg
#define OWNFUN( type,name,arg )  type __CDECL name arg
#define LOCFUN( type,name,arg )  static type __CDECL name arg

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    Ipp32u    mantissa;
    Ipp32u    exponent;
    Ipp32u    offset;
} IppiJPEGXRQuantizer_32u;

/* /////////////////////////////////////////////////////////////////////////////
//                       High Definition Photo
//                           (JPEG-XR)
///////////////////////////////////////////////////////////////////////////// */


/* /////////////////////////////////////////////////////////////////////////////
//  Names:
//       ippiPCT Fwd and Inv 2nd(DC) stages
//  Purpose:  Photo Core DC Transform Forward
//  Returns:
//    ippStsNoErr              OK
//    ippStsNullPtrErr         One of the pointers is 0
//    ippStsSizeErr            Width or height of ROI is less than or equal to zero
//
//  Parameters:
//    pSrcDst                  Pointer to the source/destination image
//    srcDstStep               Step through the source/destination image
//    roiSize                  Size of the ROI
*/

#define hdpRnd32s16u ippRndZero
#define hdpRnd32s16s ippRndZero
#define hdpRnd32f32s ippRndNear

//#define FP_CORE

IPPAPI(IppStatus, ippiPCTFwd16x16DC_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))
IPPAPI(IppStatus, ippiPCTFwd8x16DC_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))
IPPAPI(IppStatus, ippiPCTFwd8x8DC_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))
IPPAPI(IppStatus, ippiPCTInv16x16DC_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))
IPPAPI(IppStatus, ippiPCTInv8x16DC_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))
IPPAPI(IppStatus, ippiPCTInv8x8DC_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))

IPPAPI(IppStatus, ippiPCTFwd16x16DC_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))
IPPAPI(IppStatus, ippiPCTFwd8x16DC_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))
IPPAPI(IppStatus, ippiPCTFwd8x8DC_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))
IPPAPI(IppStatus, ippiPCTInv16x16DC_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))
IPPAPI(IppStatus, ippiPCTInv8x16DC_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))
IPPAPI(IppStatus, ippiPCTInv8x8DC_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))

IPPAPI(IppStatus, ippiPCTFwd16x16DC_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))
IPPAPI(IppStatus, ippiPCTFwd8x16DC_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))
IPPAPI(IppStatus, ippiPCTFwd8x8DC_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, Ipp32u downscale))
IPPAPI(IppStatus, ippiPCTInv16x16DC_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))
IPPAPI(IppStatus, ippiPCTInv8x16DC_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))
IPPAPI(IppStatus, ippiPCTInv8x8DC_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, Ipp32u upscale))


/* /////////////////////////////////////////////////////////////////////////////
//                       PCT Filters
///////////////////////////////////////////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////////
//   ROI --->  +--------------------------------------+
//             | X X |       top 2-rows         | X X |
//             | X X |       border             | X X |
//             |-----+--------------------------+-----+
//             |     |        |        |        |     |
//             |     |        |        |        |     |
//             |     |        |        |        |     |
//             |     |        |        |        |     |
//             |     +--------+--------+--------+     |
//             |     |        |        |        |     |
// left        |     | 4x4 block filtering area |     |   right 2-columns
// 2-columns   |     |        |        |        |     |   border
// border      |     |        |        |        |     |
//             |     +--------|--------+--------+     |
//             |     |        |        |        |     |
//             |     |        |        |        |     |
//             |     |        |        |        |     |
//             |     |        |        |        |     |
//             |-----+--------------------------+-----+
//             | X X |     bottom 2-rows        | X X |
//             | X X |     border               | X X |
//             +--------------------------------+------
//  ROI can be a whole image big
//  Borders are optional: top, bottom, left and rigth in all combination supported
//  2x2 border corners left unfiltered
*/
/* /////////////////////////////////////////////////////////////////////////////
//  Name:     ippiFilter Fwd and Inv 1st and 2nd(DC) stages
//  Purpose:  Photo Core Transform Filter

//  Returns:
//    ippStsNoErr              OK
//    ippStsNullPtrErr         One of the pointers is 0
//    ippStsSizeErr            Width or height of ROI is less than or equal to zero
//
//  Parameters:
//    pSrcDst                  Pointer to the source/destination image
//    srcDstStep               Step through the source/destination image
//    roiSize                  Size of the ROI
//    borderType               bitwise OR of flags ippBorderInMemTop, ippBorderInMemBottom, ippBorderInMemLeft and ippBorderInMemRight
//                             in arbitrary combinations
//                  PCT Filtering
*/
IPPAPI(IppStatus, ippiFilterFwd_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterFwdDCYUV444_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterFwdDCYUV420_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterInv_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterInvDCYUV444_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterInvDCYUV420_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))

IPPAPI(IppStatus, ippiFilterFwd_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterFwdDCYUV444_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterFwdDCYUV420_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterInv_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterInvDCYUV444_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterInvDCYUV420_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))

IPPAPI(IppStatus, ippiFilterFwd_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterFwdDCYUV444_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterFwdDCYUV420_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterInv_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterInvDCYUV444_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))
IPPAPI(IppStatus, ippiFilterInvDCYUV420_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, IppiSize roiSize, IppiBorderType borderType))

/* /////////////////////////////////////////////////////////////////////////////
//                       Quantizer
////////////////////////////////////////////////////////////////////////////////
//  Names:
//       ippiPCT Fwd and Inv 2nd(DC) stages
//  Purpose:  Quantization
//  Returns:
//    ippStsNoErr              OK
//    ippStsNullPtrErr         One of the pointers is 0
//    ippStsSizeErr            Width or height of ROI is less than or equal to zero
//
//  Parameters:
//    pSrcDst                  Pointer to the source/destination image
//    srcDstStep               Step through the source/destination image
//    pQnts                    Size of the ROI
*/

IPPAPI(IppStatus, ippiQuant16x16_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, Ipp32u numQnts))
IPPAPI(IppStatus, ippiQuant8x16_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, Ipp32u numQnts))
IPPAPI(IppStatus, ippiQuant8x8_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, Ipp32u numQnts))
IPPAPI(IppStatus, ippiQuant16x16_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, Ipp32u numQnts))
IPPAPI(IppStatus, ippiQuant8x16_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, Ipp32u numQnts))
IPPAPI(IppStatus, ippiQuant8x8_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiJPEGXRQuantizer_32u *pQnts, Ipp32u numQnts))


/* /////////////////////////////////////////////////////////////////////////////
//      HDP YUV Resolution Change functions
//      Purpose: YUV 444, 422, 420 resampling
*/
// Encode
IPPAPI(IppStatus, ippiYUV444To422_HDP_16s_C1R,(const Ipp16s *pSrc, Ipp32u srcStep, Ipp16s *pDst, Ipp32u dstStep, IppiSize dstRoi))
IPPAPI(IppStatus, ippiYUV422To420_HDP_16s_C1R,(const Ipp16s *pSrc, Ipp32u srcStep, Ipp16s *pDst, Ipp32u dstStep, IppiSize dstRoi, IppiBorderType borderType))
IPPAPI(IppStatus, ippiYUV444To422_HDP_32s_C1R,(const Ipp32s *pSrc, Ipp32u srcStep, Ipp32s *pDst, Ipp32u dstStep, IppiSize dstRoi))
IPPAPI(IppStatus, ippiYUV422To420_HDP_32s_C1R,(const Ipp32s *pSrc, Ipp32u srcStep, Ipp32s *pDst, Ipp32u dstStep, IppiSize dstRoi, IppiBorderType borderType))
IPPAPI(IppStatus, ippiYUV444To422_HDP_32f_C1R,(const Ipp32f *pSrc, Ipp32u srcStep, Ipp32f *pDst, Ipp32u dstStep, IppiSize dstRoi))
IPPAPI(IppStatus, ippiYUV422To420_HDP_32f_C1R,(const Ipp32f *pSrc, Ipp32u srcStep, Ipp32f *pDst, Ipp32u dstStep, IppiSize dstRoi, IppiBorderType borderType))
// Decode
IPPAPI(IppStatus, ippiYUV420To422_HDP_16s_C1R, (const Ipp16s *pSrc, Ipp32u srcStep, Ipp16s *pDst, Ipp32u dstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))
IPPAPI(IppStatus, ippiYUV422To444_HDP_16s_C1R, (const Ipp16s *pSrc, Ipp32u srcStep, Ipp16s *pDst, Ipp32u dstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))
IPPAPI(IppStatus, ippiYUV420To422_HDP_32s_C1R, (const Ipp32s *pSrc, Ipp32u srcStep, Ipp32s *pDst, Ipp32u dstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))
IPPAPI(IppStatus, ippiYUV422To444_HDP_32s_C1R, (const Ipp32s *pSrc, Ipp32u srcStep, Ipp32s *pDst, Ipp32u dstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))
IPPAPI(IppStatus, ippiYUV420To422_HDP_32f_C1R, (const Ipp32f *pSrc, Ipp32u srcStep, Ipp32f *pDst, Ipp32u dstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))
IPPAPI(IppStatus, ippiYUV422To444_HDP_32f_C1R, (const Ipp32f *pSrc, Ipp32u srcStep, Ipp32f *pDst, Ipp32u dstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))
IPPAPI(IppStatus, ippiYUV422To444_HDP_16s_C1IR,(Ipp16s *pSrcDst, Ipp32u srcDstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))
IPPAPI(IppStatus, ippiYUV422To444_HDP_32s_C1IR,(Ipp32s *pSrcDst, Ipp32u srcDstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))
IPPAPI(IppStatus, ippiYUV422To444_HDP_32f_C1IR,(Ipp32f *pSrcDst, Ipp32u srcDstStep, IppiSize srcRoi, IppiBorderType borderType, int chromaCentring))

#ifdef __cplusplus
}
#endif
#endif /*__HDP_H__*/
