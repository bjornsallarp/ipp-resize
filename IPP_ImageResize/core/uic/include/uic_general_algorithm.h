/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_GENERAL_ALGORITHM_H__
#define __UIC_GENERAL_ALGORITHM_H__


namespace UIC {

// SrcVector and DstVector should support operator[unsigned int] const
template<class SrcVector, class DstVector>
void Copy(const SrcVector &src, const DstVector &dst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++) dst[i] = src[i];
}

template<class T>
void Swap(T &a, T &b) { T tmp = a; a = b; b = tmp; }

template<class T> const T& Max(const T& x, const T& y) { return (x > y) ? x : y; }
template<class T> const T& Min(const T& x, const T& y) { return (x < y) ? x : y; }

template<class Vector>
void Reverse(const Vector &srcDst, unsigned int size)
{
    for(unsigned int i = 0u; i < size / 2u; i++)
        Swap(srcDst[size - i - 1], srcDst[i]);
}

} // namespace UIC

#endif // __UIC_GENERAL_ALGORITHM_H__
