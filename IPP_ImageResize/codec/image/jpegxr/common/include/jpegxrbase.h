/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __JPEGXRBASE_H__
#define __JPEGXRBASE_H__


#define JXR_MAX_CHANNELS  16
#define JXR_MAX_TILES     256
#define JXR_MAX_MEMORY    268435456 // Memory limit for internal allocation

#define JXR_PLANE_PRIMARY 0
#define JXR_PLANE_ALPHA   1

#define JXR_BORDER_TOP    0x0010
#define JXR_BORDER_BOTTOM 0x0020
#define JXR_BORDER_LEFT   0x0040
#define JXR_BORDER_RIGHT  0x0080

#define JXR_FLAG_SIGNED 0x100
#define JXR_FLAG_FLOAT  0x200
#define JXR_FLAG_5      0x400
#define JXR_FLAG_565    0x800
#define JXR_FLAG_10     0x1000
#define JXR_FLAG_BW     0x2000
#define JXR_FLAG_WB     0x4000
#define JXR_DEPTH_FIELD 0xff

#define JXR_TAGS_MAX      28
#define JXR_TAG_SIZE      12
#define JXR_PXF_SIZE      16
#define JXR_HEADER_SIZE   8
#define JXR_ORDER         0x4949
#define JXR_ID            0xbc
#define JXR_VERSION       1

#define      _PEL(type, pPel, x, y, imgStep) (type*)((Ipp8u*)((type*)(pPel)  + (x)) + (y) * (size_t)(imgStep))
#define  _NEARROW(type, pPel,    y, imgStep) (type*)((Ipp8u*)((type*)(pPel)) +                (y) * (size_t)(imgStep))
#define _NEXTROW4(type, pPel,       imgStep) (type*)((Ipp8u*)((type*)(pPel)) +                 4  * (size_t)(imgStep))
#define  _NEXTROW(type, pPel,       imgStep) (type*)((Ipp8u*)((type*)(pPel)) +                      (size_t)(imgStep))
#define PEL(pPel, x, y, imgStep) _PEL(Ipp32s, pPel, x, y, imgStep)
#define ABS(A) (((A) < 0)?-(A):(A))

#define RET_(excCall) {if(excCall != ExcStatusOk) return ExcStatusFail;}
#define RET_TBB_(excCall) {if(excCall != ExcStatusOk) { m_bTBBError = 1; return NULL;}}

typedef enum _JXR_TAG_TYPE
{
  JXR_TT_BYTE      = 1,
  JXR_TT_ASCII     = 2,
  JXR_TT_SHORT     = 3,
  JXR_TT_LONG      = 4,
  JXR_TT_RATIONAL  = 5,
  JXR_TT_SBYTE     = 6,
  JXR_TT_UNDEFINED = 7,
  JXR_TT_SSHORT    = 8,
  JXR_TT_SLONG     = 9,
  JXR_TT_SRATIONAL = 10,
  JXR_TT_FLOAT     = 11,
  JXR_TT_DOUBLE    = 12

} JXR_TAG_TYPE;


typedef enum _JXR_TAG
{
  JXRT_DOCUMENT_NAME         = 0x010d,
  JXRT_IMAGE_DESCRIPTION     = 0x010e,
  JXRT_MAKE                  = 0x010f,
  JXRT_MODEL                 = 0x0110,
  JXRT_PAGE_NAME             = 0x011d,
  JXRT_PAGE_NUMBER           = 0x0129,
  JXRT_SOFTWARE              = 0x0131,
  JXRT_DATE_TIME             = 0x0132,
  JXRT_ARTIST                = 0x013b,
  JXRT_HOST_COMPUTER         = 0x013c,
  JXRT_COPYRIGHT             = 0x8298,
  JXRT_COLORSPACE            = 0xa001,
  JXRT_PIXEL_FORMAT          = 0xbc01,
  JXRT_SPATIAL_XFRM_PRIMARY  = 0xbc02,
  JXRT_IMAGE_TYPE            = 0xbc04,
  JXRT_PTM_COLOR_INFO        = 0xbc05,
  JXRT_PROFILE_LEVEL_INFO    = 0xbc06,
  JXRT_IMAGE_WIDTH           = 0xbc80,
  JXRT_IMAGE_HEIGHT          = 0xbc81,
  JXRT_WIDTH_RESOLUTION      = 0xbc82,
  JXRT_HEIGHT_RESOLUTION     = 0xbc83,
  JXRT_IMAGE_OFFSET          = 0xbcc0,
  JXRT_IMAGE_BYTE_COUNT      = 0xbcc1,
  JXRT_ALPHA_OFFSET          = 0xbcc2,
  JXRT_ALPHA_BYTE_COUNT      = 0xbcc3,
  JXRT_IMAGE_BAND_PRESENCE   = 0xbcc4,
  JXRT_ALPHA_BAND_PRESENCE   = 0xbcc5,
  JXRT_PADDING_DATA          = 0xea1c

} JXR_TAG;


typedef enum _JXR_PIXEL_FORMAT
{
  // GRAY
  JXR_PF_GRAY_8U        = 0x08,
  JXR_PF_GRAY_16U       = 0x0b,
  JXR_PF_GRAY_16S       = 0x13,
  JXR_PF_GRAY_16F       = 0x3e,
  JXR_PF_GRAY_32S       = 0x3f,
  JXR_PF_GRAY_32F       = 0x11,
  JXR_PF_GRAY_1U        = 0x05,

  // BGR
  JXR_PF_BGR_8U_C3      = 0x0c,
  JXR_PF_BGR_8U_C4      = 0x0e,
  JXR_PF_BGRA_8U_C4     = 0x0f,
  JXR_PF_BGR_5U_C3      = 0x09,
  JXR_PF_BGR_565U_C3    = 0x0a,
  JXR_PF_BGR_10U_C3     = 0x14,
  JXR_PF_PBGRA_8U_C4    = 0x10,

  // RGB
  JXR_PF_RGB_8U_C3      = 0x0d,
  JXR_PF_RGB_16U_C3     = 0x15,
  JXR_PF_RGB_16S_C3     = 0x12,
  JXR_PF_RGB_16F_C3     = 0x3b,
  JXR_PF_RGB_32S_C3     = 0x18,
  JXR_PF_RGB_16S_C4     = 0x40,
  JXR_PF_RGB_16F_C4     = 0x42,
  JXR_PF_RGB_32S_C4     = 0x41,
  JXR_PF_RGB_32F_C4     = 0x1b,
  JXR_PF_RGBA_16U_C4    = 0x16,
  JXR_PF_RGBA_16S_C4    = 0x1d,
  JXR_PF_RGBA_16F_C4    = 0x3a,
  JXR_PF_RGBA_32S_C4    = 0x1e,
  JXR_PF_RGBA_32F_C4    = 0x19,
  JXR_PF_PRGBA_16U_C4   = 0x17,
  JXR_PF_PRGBA_32F_C4   = 0x1a,
  JXR_PF_RGBE_8U_C3     = 0x3d,

  // CMYK
  JXR_PF_CMYK_8U_C4     = 0x1c,
  JXR_PF_CMYKA_8U_C5    = 0x2c,
  JXR_PF_CMYK_16U_C4    = 0x1f,
  JXR_PF_CMYKA_16U_C5   = 0x2d,
  JXR_PF_CMYKD_8U_C4    = 0x54,
  JXR_PF_CMYKD_16U_C4   = 0x55,
  JXR_PF_CMYKDA_8U_C5   = 0x56,
  JXR_PF_CMYKDA_16U_C5  = 0x57,

  // NCH
  JXR_PF_NCH_8U_C3      = 0x20,
  JXR_PF_NCH_8U_C4      = 0x21,
  JXR_PF_NCH_8U_C5      = 0x22,
  JXR_PF_NCH_8U_C6      = 0x23,
  JXR_PF_NCH_8U_C7      = 0x24,
  JXR_PF_NCH_8U_C8      = 0x25,
  JXR_PF_NCHA_8U_C4     = 0x2e,
  JXR_PF_NCHA_8U_C5     = 0x2f,
  JXR_PF_NCHA_8U_C6     = 0x30,
  JXR_PF_NCHA_8U_C7     = 0x31,
  JXR_PF_NCHA_8U_C8     = 0x32,
  JXR_PF_NCHA_8U_C9     = 0x33,
  JXR_PF_NCH_16U_C3     = 0x26,
  JXR_PF_NCH_16U_C4     = 0x27,
  JXR_PF_NCH_16U_C5     = 0x28,
  JXR_PF_NCH_16U_C6     = 0x29,
  JXR_PF_NCH_16U_C7     = 0x2a,
  JXR_PF_NCH_16U_C8     = 0x2b,
  JXR_PF_NCHA_16U_C4    = 0x34,
  JXR_PF_NCHA_16U_C5    = 0x35,
  JXR_PF_NCHA_16U_C6    = 0x36,
  JXR_PF_NCHA_16U_C7    = 0x37,
  JXR_PF_NCHA_16U_C8    = 0x38,
  JXR_PF_NCHA_16U_C9    = 0x39,

  // YUV
  JXR_PF_YCC420_8U_C3   = 0x44,
  JXR_PF_YCC422_8U_C3   = 0x45,
  JXR_PF_YCC422_10U_C3  = 0x46,
  JXR_PF_YCC422_16U_C3  = 0x47,
  JXR_PF_YCC444_8U_C3   = 0x48,
  JXR_PF_YCC444_10U_C3  = 0x49,
  JXR_PF_YCC444_16U_C3  = 0x4a,
  JXR_PF_YCC444_16S_C3  = 0x4b,
  JXR_PF_YCCA420_8U_C4  = 0x4c,
  JXR_PF_YCCA422_8U_C4  = 0x4d,
  JXR_PF_YCCA422_10U_C4 = 0x4e,
  JXR_PF_YCCA422_16U_C4 = 0x4f,
  JXR_PF_YCCA444_8U_C4  = 0x50,
  JXR_PF_YCCA444_10U_C4 = 0x51,
  JXR_PF_YCCA444_16U_C4 = 0x52,
  JXR_PF_YCCA444_16S_C4 = 0x53

} JXR_PIXEL_FORMAT;


typedef enum _JXR_STREAM_FORMAT
{
  JXR_SF_SPATIAL   = 0,
  JXR_SF_FREQUENCY = 1,

} JXR_STREAM_FORMAT;


typedef enum _JXR_CHANNELS_MODE
{
  JXR_CM_UNIFORM     = 0,
  JXR_CM_SEPARATE    = 1,
  JXR_CM_INDEPENDENT = 2

} JXR_CHANNELS_MODE;


typedef enum _JXR_OUT_COLOR_FORMAT
{
  JXR_OCF_Y      = 0,
  JXR_OCF_YUV420 = 1,
  JXR_OCF_YUV422 = 2,
  JXR_OCF_YUV444 = 3,
  JXR_OCF_CMYK   = 4,
  JXR_OCF_CMYKD  = 5,
  JXR_OCF_NCH    = 6,
  JXR_OCF_RGB    = 7,
  JXR_OCF_RGBE   = 8,
  // Out of specification formats
  JXR_OCF_AUTO   = 0xff

} JXR_OUT_COLOR_FORMAT;


typedef enum _JXR_INT_COLOR_FORMAT
{
  JXR_ICF_Y      = 0,
  JXR_ICF_YUV420 = 1,
  JXR_ICF_YUV422 = 2,
  JXR_ICF_YUV444 = 3,
  JXR_ICF_YUVK   = 4,
  JXR_ICF_NCH    = 6

} JXR_INT_COLOR_FORMAT;


typedef enum _JXR_BIT_DEPTH
{
  JXR_BD_1W  = 0,
  JXR_BD_8   = 1,
  JXR_BD_16  = 2,
  JXR_BD_16S = 3,
  JXR_BD_16F = 4,
  JXR_BD_32S = 6,
  JXR_BD_32F = 7,
  JXR_BD_5   = 8,
  JXR_BD_10  = 9,
  JXR_BD_565 = 10,
  JXR_BD_1B  = 15

} JXR_BIT_DEPTH;


typedef enum _JXR_STREAM_BANDS
{
  JXR_SB_ALL    = 0,
  JXR_SB_NOFLEX = 1,
  JXR_SB_NOHP   = 2,
  JXR_SB_DCONLY = 3

} JXR_STREAM_BANDS;


typedef enum _JXR_TILE_TYPE
{
  JXR_TT_SPATIAL  = 0,
  JXR_TT_DC       = 1,
  JXR_TT_LP       = 2,
  JXR_TT_HP       = 3,
  JXR_TT_FB       = 4

} JXR_TILE_TYPE;


typedef enum _JXR_BAND_TAG
{
  JXR_BT_DC     = 0,
  JXR_BT_LP     = 1,
  JXR_BT_HP     = 2

} JXR_BAND_TAG;

// Old buffers, depricated, still used in encoder
typedef struct MBTab
{
  Ipp32s* pMB4;       // pointer to MB4
  Ipp32s* pSliceMB4;  // pointer to MB4 slice
  Ipp32s* pSliceCC0;  // pointer to prev slice
  Ipp32s* pCC0;       // pointer to current MB in prev slice
  Ipp32s* pSliceCC1;  // pointer to current slice
  Ipp32s* pCC1;       // pointer to MB in current slice
  Ipp32s  iStepMB4;   // step MB4
  Ipp32s  iStepCC;    // common CC step

} MBTab;

// Image buffers; Specialized pointers for multithreading purpose
typedef struct ImgBuf
{
  Ipp32s* pBuf;     // Pointer to first row (border row)
  Ipp32s* pImgBuf;  // Pointer to first image row
  Ipp32s* pHufPtr;  // Pointer to current huffman process position
  Ipp32s* pPCTPtr;  // Pointer to current PCT process position
  Ipp32s* pCCPtr;   // Pointer to current color convert process position
  Ipp32u  iStep;

} ImgBuf;


typedef struct AdaptiveHuffman
{
  const Ipp16s* pTable;
  const Ipp8s*  pDelta;
  Ipp32s        iSymbols;
  Ipp32s        iLevel;
  Ipp32s        iDiscr;
  Ipp32s        iUBound;
  Ipp32s        iLBound;
  Ipp32s        bInitialize;

} AdaptiveHuffman;


typedef struct AdaptiveModel
{
  Ipp32s iMBState[2];
  Ipp32s iModelBits[2];
  Ipp8u  iBand;

} AdaptiveModel;


typedef struct CBPModel
{
  Ipp32s iCount0[2];
  Ipp32s iCount1[2];
  Ipp32s iState[2];

} CBPModel;


typedef struct PredictorInf
{
  Ipp32s* iDCLP;
  Ipp32s  iQPIndex;
  Ipp32s  iCBP;
  Ipp32s  iDC;
  Ipp32s  iLP[6];

} PredictorInf;


#endif // __JPEGXRBASE_H__
