
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

#include "imageconvert.h"
#include "pp.h"


inline void Add(
    const ImageCoreC<Ipp32s, 1> &srcDst,
    const RectSize              &size,
    Ipp32s                       value)
{
    if(!value) return;

    Ipp32s *srcDstData  = srcDst.Data();
    unsigned int step   = srcDst.LineStep();
    unsigned int width  = size.Width();
    unsigned int height = size.Height();

    for(unsigned int y = 0; y < height; y++)
    {
        for(unsigned int x = 0; x < width; x++) srcDstData[x] += value;
        addrInc(srcDstData, step);
    }
}


void ConvertDynRange(
    const ImageCoreC<Ipp32s, 1> &srcDst,
    const RectSize              &size,
    unsigned int srcBitDepth, bool srcIsSigned,
    unsigned int dstBitDepth, bool dstIsSigned)
{
    int shift = dstBitDepth - srcBitDepth;

    Ipp32s add;

    if(dstIsSigned)
    {
        add = srcIsSigned ? 0 : -(Ipp32s)1 << (Ipp32s)srcBitDepth;
    }
    else
    {
        add = srcIsSigned ? (Ipp32s)1 << (Ipp32s)srcBitDepth : 0;
    }

    Shift(srcDst, size, shift);
    Add  (srcDst, size, add);
}

void Saturate(
    const ImageCoreC<Ipp32s, 1> &srcDst,
    const RectSize              &size,
    Ipp32s                       min,
    Ipp32s                       max)
{
    Ipp32s *srcDstData  = srcDst.Data();
    unsigned int step   = srcDst.LineStep();
    unsigned int width  = size.Width();
    unsigned int height = size.Height();

    for(unsigned int y = 0; y < height; y++)
    {
        for(unsigned int x = 0; x < width; x++)
        {
            if(srcDstData[x] > max) srcDstData[x] = max;
            if(srcDstData[x] < min) srcDstData[x] = min;
        }
        addrInc(srcDstData, step);
    }
}
