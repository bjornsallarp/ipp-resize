/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_MATH_H__
#define __UIC_MATH_H__

namespace UIC {

inline unsigned int Log2(unsigned int value)
{
    int i = 31;
    for(; i >= 0; i--)
    {
        if(value&0x80000000) break;
        value = value << 1;
    }
    return i;
}

// for zero value this function returns 0,
// for value == 1 this function returns 1,
// for value == 3 this function returns 2,
// and so on
inline unsigned int MSBitPos(unsigned int value)
{
    unsigned int i = 0;
    for(; value > 0; i++) value >>= 1;
    return i;
}

inline unsigned int MSBitPos64(Ipp64u value)
{
    unsigned int i = 0;
    for(; value > 0; i++) value >>= (Ipp64u)1;
    return i;
}

// for value == 0, 1 this function returns 0,
// for value == 2, 3 this function returns 1,
// for value == 4 - 7 this function returns 2,
// and so on
inline unsigned int BitDepth(unsigned int maxValue)
{
    unsigned int bitDepth = MSBitPos(maxValue);
    if(bitDepth) bitDepth--;
    return bitDepth;
}

inline unsigned int BitDepth64(Ipp64u maxValue)
{
    unsigned int bitDepth = MSBitPos64(maxValue);
    if(bitDepth) bitDepth--;
    return bitDepth;
}

inline unsigned int Mod2(unsigned int value, unsigned int order)
{
    return value - ((value >> order) << order);
}

inline unsigned int RShiftCeil(unsigned int value, unsigned int order)
{
    return ( value + ( (1u << order) - 1u ) ) >> order;
}

inline unsigned int RShiftFloor(unsigned int value, unsigned int order)
{
    return value  >> order;
}

inline unsigned int DivCeil(unsigned int value, unsigned int divizor)
{
    return ( (value + divizor - 1u) / divizor );
}

inline unsigned int RoundCenterU(double value)
{
    return (unsigned int)(value + 0.5);
}


inline unsigned int AlignPow2(unsigned int value, unsigned int alignOrder)
{
    unsigned int align      = 1 << alignOrder;
    unsigned int mask       = (~(align-1));

    return (value + align - 1) & mask;
}

inline unsigned int GCD(unsigned int a, unsigned int b)
{
    if(!a) return b;
    if(!b) return a;

    unsigned int gcd = 1;

    while (! ((a|b)&1) )
    {
        a >>= 1;
        b >>= 1;
        gcd <<= 1;
    }

    while ( (a != 0) && (b != 0) )
    {
        if((a^b)&1)
        {
            if(b&1) a >>= 1;
            else    b >>= 1;
        }
        else
        {
            if(a > b) a = a - b;
            else      b = b - a;
        }
    }

    if(a) gcd *= a;
    else  gcd *= b;

    return gcd;
}

} // namespace UIC

#endif // __UIC_MATH_H__
