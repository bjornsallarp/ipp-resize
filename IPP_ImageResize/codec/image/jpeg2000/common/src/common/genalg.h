
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

#ifndef __GENALG_H__
#define __GENALG_H__

// SrcVector and DstVector should support operator[unsigned int] const
template<class SrcVector, class DstVector>
void Copy(const SrcVector &src, const DstVector &dst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++) dst[i] = src[i];
}

template<class T>
void Swap(T &a, T &b) { T tmp = a; a = b; b = tmp; }

template<class Vector>
inline void Reverse(const Vector &srcDst, unsigned int size)
{
    for(unsigned int i = 0u; i < size / 2u; i++)
        Swap(srcDst[size - i - 1], srcDst[i]);
}

template<class T> const T& Max(const T& x, const T& y) { return (x > y) ? x : y; }
template<class T> const T& Min(const T& x, const T& y) { return (x < y) ? x : y; }

template<class Element, class Vector>
Element Max(const Element &lowBound, const Vector &src, unsigned int size)
{
    Element max = lowBound;

    for(unsigned int i = 0; i < size; i++)
        max = Max<Element>(src[i], max);

    return max;
}

template<class Element, class Vector>
Element Min(const Element &highBound, const Vector &src, unsigned int size)
{
    Element min = highBound;

    for(unsigned int i = 0; i < size; i++)
        min = Min<Element>(src[i], min);

    return min;
}

template<class Element, class Vector>
void SetValue(const Element &value, const Vector &dst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++) dst[i] = value;
}


#endif // __GENALG_H__

