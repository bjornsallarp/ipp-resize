/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2012 Intel Corporation. All Rights Reserved.
//
*/


#ifndef __BMP_H__
#define __BMP_H__

#define BMP_IFH_SIZE  14
#define BMP_IH_SIZE   40

#ifndef __IPP_H__
#include "ipp.h"
#endif

namespace UIC {

#pragma pack(push, 2)
typedef struct tBMPImageFileHeader
{
  Ipp16u   bfType;
  Ipp32u   bfSize;
  Ipp16u   bfReserved1;
  Ipp16u   bfReserved2;
  Ipp32u   bfOffBits;

} BMPImageFileHeader;
#pragma pack(pop)


typedef struct tBMPImageHeader
{
  Ipp32u    biSize;
  Ipp32u    biWidth;
  Ipp32s    biHeight;
  Ipp16u    biPlanes;
  Ipp16u    biBitCount;
  Ipp32u    biCompression;
  Ipp32u    biSizeImage;
  Ipp32u    biXPelsPerMeter;
  Ipp32u    biYPelsPerMeter;
  Ipp32u    biClrUsed;
  Ipp32u    biClrImportant;

} BMPImageHeader;


typedef struct tRGBquad
{
  unsigned char    rgbBlue;
  unsigned char    rgbGreen;
  unsigned char    rgbRed;
  unsigned char    rgbReserved;

} RGBquad;


typedef struct tImageInfo
{
  BMPImageHeader   bmiHeader;
  RGBquad          bmiColors[1];

} BMPImageInfo;

} // namespace UIC

#endif // __BMP_H__
