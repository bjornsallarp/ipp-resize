
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __IMAGECONVERT_H__
#define __IMAGECONVERT_H__

#include "imagecore.h"

void ConvertDynRange(
    const ImageCoreC<Ipp32s, 1> &srcDst,
    const RectSize              &size,
    unsigned int srcBitDepth, bool srcIsSigned,
    unsigned int dstBitDepth, bool dstIsSigned);

void Saturate(
    const ImageCoreC<Ipp32s, 1> &srcDst,
    const RectSize              &size,
    Ipp32s                       min,
    Ipp32s                       max);

#endif // __IMAGECONVERT_H__

