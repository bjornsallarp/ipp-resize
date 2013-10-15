
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

#ifndef __ADDRARITH_H__
#define __ADDRARITH_H__

template<class T> T* addrAdd(const T* ptr, int bytes)
{
    return (T*)(&((char*)ptr)[bytes]);
}

template<class T> void addrInc(T& ptr, int bytes)
{
    ptr = addrAdd(ptr, bytes);
}

#endif // __ADDRARITH_H__
