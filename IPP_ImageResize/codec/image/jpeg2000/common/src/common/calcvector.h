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

#ifndef __CALCVECTOR_H__
#define __CALCVECTOR_H__

inline void SubRev(const Ipp32f* src, Ipp32f *srcDst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++)
        srcDst[i] = src[i] - srcDst[i];
}

inline void SubRev(const Ipp32s* src, Ipp32s *srcDst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++)
        srcDst[i] = src[i] - srcDst[i];
}

inline void SubRev(const Ipp16s* src, Ipp16s *srcDst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++)
        srcDst[i] = src[i] - srcDst[i];
}

inline void MulAdd(Ipp32f multiplier, const Ipp32f* src, Ipp32f* srcDst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++)
        srcDst[i] += multiplier * src[i];
}

inline void AddMulAdd(Ipp32f multiplier, const Ipp32f* src1, const Ipp32f* src2, Ipp32f* srcDst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++)
        srcDst[i] += multiplier * (src1[i] + src2[i]);
}

template<class T>
void MulRoundSF16(Ipp32s value, const T* src, T *dst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++)
        dst[i] = (T)(( ((Ipp64s)value * src[i]) + (1 << 15) ) >> 16);
}

template<class T>
void MulRoundSF16(Ipp32s value, T* srcDst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++)
        srcDst[i] = (T)(( ((Ipp64s)value * srcDst[i]) + (1 << 15) ) >> 16);
}

template<class T>
void MulAddRoundSF16(const Ipp32s &multiplier, const T* src, T* srcDst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++)
        srcDst[i] += (T)(( ((Ipp64s)multiplier * src[i]) + (1 << 15)) >> 16);
}

template<class T>
void AddMulAddRoundSF16(const Ipp32s &multiplier, const T* src1, const T* src2, T* srcDst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++)
        srcDst[i] += (T)(( ((Ipp64s)multiplier * (src1[i] + src2[i])) + (1 << 15)) >> 16);
}

// no saturation
template<class TSRC, class TDST>
void Convert(const TSRC *src, TDST *dst, unsigned int size)
{
    for(unsigned int i = 0; i < size; i++) dst[i] = (TDST)src[i];
}

template<class TSRC, class TDST, class TINTERCALC>
void AddConvert(const TSRC *src, TDST *dst, unsigned int size, const TINTERCALC &add)
{
    if(!add) { Convert(src, dst, size); return; }

    for(unsigned int i = 0; i < size; i++)
    {
        TINTERCALC value = (TINTERCALC)src[i] + add;
        dst[i] = (TDST)value;
    }
}

template<class TSRC, class TDST, class TINTERCALC>
void LShiftConvert(const TSRC *src, TDST *dst, unsigned int size, const TINTERCALC &shift)
{
    if(!shift) { Convert(src, dst, size); return; }

    for(unsigned int i = 0; i < size; i++)
    {
        TINTERCALC value = (TINTERCALC)src[i] << shift;
        dst[i] = (TDST)value;
    }
}

template<class TSRC, class TDST, class TINTERCALC>
void RShiftConvert(const TSRC *src, TDST *dst, unsigned int size, const TINTERCALC &shift)
{
    if(!shift) { Convert(src, dst, size); return; }

    for(unsigned int i = 0; i < size; i++)
    {
        TINTERCALC value = (TINTERCALC)src[i] >> shift;
        dst[i] = (TDST)value;
    }
}

template<class TSRC, class TDST, class TINTERCALC>
void ShiftConvert(const TSRC *src, TDST *dst, unsigned int size, const TINTERCALC &shift)
{
    if(shift >= 0)
        LShiftConvert(src, dst, size, shift);
    else
        RShiftConvert(src, dst, size, shift);
}

template<class TSRC, class TDST, class TINTERCALC>
void LShiftAddConvert(const TSRC *src, TDST *dst, unsigned int size, const TINTERCALC &shift, const TINTERCALC &add)
{
    if(!add)
        ShiftConvert(src, dst, size, shift);
    else if(!shift)
        AddConvert(src, dst, size, shift);
    else
        for(unsigned int i = 0; i < size; i++)
        {
            TINTERCALC value = ((TINTERCALC)src[i] << shift) + add;
            dst[i] = (TDST)value;
        }
}

template<class TSRC, class TDST, class TINTERCALC>
void RShiftAddConvert(const TSRC *src, TDST *dst, unsigned int size, const TINTERCALC &shift, const TINTERCALC &add)
{
    if(!add)
        ShiftConvert(src, dst, size, shift);
    else if(!shift)
        AddConvert(src, dst, size, shift);
    else
        for(unsigned int i = 0; i < size; i++)
        {
            TINTERCALC value = ((TINTERCALC)src[i] >> shift) + add;
            dst[i] = (TDST)value;
        }
}

template<class TSRC, class TDST, class TINTERCALC>
void ShiftAddConvert(const TSRC *src, TDST *dst, unsigned int size, const TINTERCALC &shift, const TINTERCALC &add)
{
    if(shift >= 0)
        LShiftAddConvert(src, dst, size, shift, add);
    else
        RShiftAddConvert(src, dst, size, shift, add);
}

#endif // __CALCVECTOR_H__

