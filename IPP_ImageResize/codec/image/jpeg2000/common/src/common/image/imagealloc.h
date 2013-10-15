
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2003-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __IMAGEALLOC_H__
#define __IMAGEALLOC_H__

#include "ippi.h"

template <class T> T *ImageAllocC(int width, int height, unsigned int &step, int nOfChannels)
{
    return (T*)ippiMalloc_8u_C1(width * sizeof(T) * nOfChannels, height, (int*)&step);
}

template <class T> T *ImageAllocP(int width, int height, unsigned int &step, int nOfChannels)
{
    return (T*)ippiMalloc_8u_C1(width * sizeof(T), height * nOfChannels, (int*)&step);
}

inline void ImageFree(void *data)
{
    ippiFree(data);
}

#endif // __IMAGEALLOC_H__
