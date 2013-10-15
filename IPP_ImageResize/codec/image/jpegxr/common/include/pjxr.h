/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2008-2012 Intel Corporation. All Rights Reserved.
//
//          Intel(R) Integrated Performance Primitives
//
//                    JPEG-XR
//
//  Purpose:
//     Internals definitions and declarations
*/
#if !defined( __PJXR_H__ )
#define __PJXR_H__

#include <jpegxrbase.h>
#include <stddef.h>

#define SCALE_UP4x4_INT(px,py,x,s)   px[x]<<=s, px[x+4]<<=s, py[x]<<=s, py[x+4]<<=s

#define SCALE_UP4x4_32s SCALE_UP4x4_INT
#define SCALE_UP4x4_16s SCALE_UP4x4_INT
#define SCALE_UP4x4_32f(px,py,x,s)

#define SCALE_DOWN4x4_INT(px,py,x,s)   px[x]>>=s, px[x+4]>>=s, py[x]>>=s, py[x+4]>>=s

#define SCALE_DOWN4x4_32s SCALE_DOWN4x4_INT
#define SCALE_DOWN4x4_16s SCALE_DOWN4x4_INT
#define SCALE_DOWN4x4_32f(px,py,x,s)



/* JPEG-XR FwdRotate D.5.1.2 */
/* REFCode: fwdRotate */

#define ROTATE1(a, b) (b) -= (((a) + 1) >> 1), (a) += (((b) + 1) >> 1)
#define ROTATE1f(a, b) (b) -= (a) * 0.5f, (a) += (b) * 0.5f
#define ROTATE2(a, b) (b) -= (((a)*3 + 4) >> 3), (a) += (((b)*3 + 4) >> 3)
#define ROTATE2f(a, b) (b) -= (a) * 0.375f, (a) += (b) * 0.375f
#define ROTATE1_32s ROTATE1
#define ROTATE1_16s ROTATE1
#define ROTATE1_32f ROTATE1f
#define ROTATE2_32s ROTATE2
#define ROTATE2_16s ROTATE2
#define ROTATE2_32f ROTATE2f

/* JPEG-XR D.5.1.4 FwdTOddOdd */
/* REFCode: fwdT_Odd_Odd_PRE identical */
#define fwdOddOddPREt(type, a, b, c, d)\
{\
    type t1, t2;\
    /* butterflies */\
    d += a;\
    c -= b;\
    a -= (t1 = d >> 1);\
    b += (t2 = c >> 1);\
\
    /* rotate pi/4 */\
    a += (b * 3 + 4) >> 3;\
    b -= (a * 3 + 2) >> 2;\
    a += (b * 3 + 6) >> 3;\
\
    /* butterflies */\
    b -= t2;\
    a += t1;\
    c += b;\
    d -= a;\
}

#define fwdOddOddPREf(type, a, b, c, d)\
{\
    type t1, t2;\
    /* butterflies */\
    d += a;\
    c -= b;\
    a -= (t1 = d * 0.5f);\
    b += (t2 = c * 0.5f);\
\
    /* rotate pi/4 */\
    a += b * 0.375f;\
    b -= a * 0.75f;\
    a += b * 0.375f;\
\
    /* butterflies */\
    b -= t2;\
    a += t1;\
    c += b;\
    d -= a;\
}
#define fwdOddOddPre_32f(a, b, c, d) fwdOddOddPREf(Ipp32f, a, b, c, d)
#define fwdOddOddPre_32s(a, b, c, d) fwdOddOddPREt(Ipp32s, a, b, c, d)
#define fwdOddOddPre_16s(a, b, c, d) fwdOddOddPREt(Ipp16s, a, b, c, d)

/*
    Hadamard+Scale transform
*/
#define fwdScale(a, b)\
    b -= (a * 3 + 0) >> 4;\
    b -= a >> 7;\
    b += a >> 10;\
    a -= (b * 3 + 0) >> 3;\
    b = (a >> 1) - b;\
    a -= b
#define fwdScale32f(a, b)\
    b -= (a * 3) * 0.0625f;\
    b -= a * (1.f/128);\
    b += a  * (1.f/1024);\
    a -= (b * 3) * 0.125f;\
    b = (a * 0.5f) - b;\
    a -= b

#define fwdRotate(a, b)\
    b -= (a + 1) >> 1;\
    a += (b + 1) >> 1
#define fwdRotate32f(a, b)\
    b -= (a + 1) * 0.5f;\
    a += (b + 1) * 0.5f

/* JPEG-XR D.5.1.1 T2x2hEnc */
/* REFCode:_2x2T_h_Enc identical */
#define strHSTENCt(type, a, b, c, d, s)\
{\
    type t1, t2;\
    a += d;\
    b -= c;\
    t1 = d;\
    t2 = c;\
    c = ((a - b) >> 1) - t1;\
    d = t2 + (b >> 1);\
    b += c;\
    a -= (d * 3 + 4) >> 3;\
    fwdScale(a,d);\
}

#define strHSTENCf(type, a, b, c, d, s)\
{\
    type t1, t2;\
    a += d;\
    b -= c;\
    t1 = d;\
    t2 = c;\
    c = ((a - b) * 0.5f) - t1;\
    d = t2 + (b * 0.5f);\
    b += c;\
    a -= (d * 3 + 4) * 0.125f;\
    fwdScale32f(a,d);\
}
#define strHSTenc_32f(a,b,c,d,s) strHSTENCf(Ipp32f, a, b, c, d, s)
#define strHSTenc_32s(a,b,c,d,s) strHSTENCt(Ipp32s, a, b, c, d, s)
#define strHSTenc_16s(a,b,c,d,s) strHSTENCt(Ipp16s, a, b, c, d, s)

/* JPEG-XR D.5.1.5 OverlapPreFilter4x4 */
/* REFCode: _jxr_4x4PreFilter identical */
#define strPREt4(a, b, c, d)\
    a += d;\
    b += c;\
    d -= ((a + 1) >> 1);\
    c -= ((b + 1) >> 1);\
    fwdRotate(c, d);\
    d *= -1;\
    c *= -1;\
    a -= d;\
    b -= c;\
    d += (a >> 1);\
    c += (b >> 1);\
    a -= ((d * 3 + 4) >> 3);\
    b -= ((c * 3 + 4) >> 3);\
    fwdScale(a, d);\
    fwdScale(b, c);\
    d += ((a + 1) >> 1);\
    c += ((b + 1) >> 1);\
    a -= d;\
    b -= c

#define strPREf4(a, b, c, d)\
    a += d;\
    b += c;\
    d -= (a + 1) * 0.5f;\
    c -= (b + 1) * 0.5f;\
    fwdRotate32f(c, d);\
    d *= -1;\
    c *= -1;\
    a -= d;\
    b -= c;\
    d += a * 0.5f;\
    c += b * 0.5f;\
    a -= (d * 3 + 4) * 0.125f;\
    b -= (c * 3 + 4) * 0.125f;\
    fwdScale32f(a, d);\
    fwdScale32f(b, c);\
    d += (a + 1) * 0.5f;\
    c += (b + 1) * 0.5f;\
    a -= d;\
    b -= c
#define strPre4_32f(a, b, c, d) strPREf4(a, b, c, d)
#define strPre4_32s(a, b, c, d) strPREt4(a, b, c, d)
#define strPre4_16s(a, b, c, d) strPREt4(a, b, c, d)

#define strPREt2x2(type, a, b, c, d)\
{\
    /* butterflies */\
    a += d;\
    b += c;\
    d -= (a + 1) >> 1;\
    c -= (b + 1) >> 1;\
\
    /* rotate */\
    b -= ((a + 2) >> 2);\
    a -= ((b + 1) >> 1);\
    b -= ((a + 2) >> 2);\
\
    /* butterflies */\
    d += (a + 1) >> 1;\
    c += (b + 1) >> 1;\
    a -= d;\
    b -= c;\
}
#define strPREf2x2(a, b, c, d)\
{\
    /* butterflies */\
    a += d;\
    b += c;\
    d -= a * 0.5f;\
    c -= b * 0.5f;\
\
    /* rotate */\
    b -= a * 0.25f;\
    a -= b * 0.5f;\
    b -= a * 0.25f;\
\
    /* butterflies */\
    d += a * 0.5f;\
    c += b * 0.5f;\
    a -= d;\
    b -= c;\
}
#define strPre2x2_32f(a, b, c, d) strPREf2x2(a, b, c, d)
#define strPre2x2_32s(a, b, c, d) strPREt2x2(Ipp32s, a, b, c, d)
#define strPre2x2_16s(a, b, c, d) strPREt2x2(Ipp16s, a, b, c, d)

#define  strPre2(a,b)\
{\
    b -= ((a + 4) >> 3);\
    a -= ((b + 2) >> 2);\
    b -= ((a + 4) >> 3);\
}
#define  strPre2f(a,b)\
{\
    b -= (a + 4) * 0.125f;\
    a -= (b + 2) * 0.25f;\
    b -= (a + 4) * 0.125f;\
}
#define  strPre2_32s strPre2
#define  strPre2_16s strPre2
#define  strPre2_32f strPre2f

// 2D 2x2 Hadamard with rounding factor R=0
/* JPEG-XR T2x2h 9.9.7.2 */
/* REFCode: _2x2T_h identical*/
#define T2x2h(type, a, b, c, d, s)\
{\
    type t;\
    a += d;\
    b -= c;\
    t = ((a - b + s) >> 1) - d;\
    b += t;\
    d += t - c;\
    a -= d;\
    c = t;\
}

#define T2x2h_f(type, a, b, c, d, s)\
{\
    type t;\
    a += d;\
    b -= c;\
    t = (a - b + s) * 0.5f - d;\
    b += t;\
    d += t - c;\
    a -= d;\
    c = t;\
}

#define strDCT2x2dn_32f(a, b, c, d) T2x2h_f(Ipp32f, a, b, c, d, 0)
#define strDCT2x2dn_32s(a, b, c, d) T2x2h(Ipp32s, a, b, c, d, 0)
#define strDCT2x2dn_16s(a, b, c, d) T2x2h(Ipp16s, a, b, c, d, 0)
#define strDCT2x2dn(a, b, c, d) strDCT2x2dn_32s(a, b, c, d, 0)
// 2D 2x2 Hadamard with rounding factor R=1
/* JPEG_XR T2x2h with equ changes */


#define strDCT2x2up_32f(a, b, c, d) T2x2h_f(Ipp32f, a, b, c, d, 0)
#define strDCT2x2up_32s(a, b, c, d) T2x2h(Ipp32s, a, b, c, d, 1)
#define strDCT2x2up_16s(a, b, c, d) T2x2h(Ipp16s, a, b, c, d, 1)
#define strDCT2x2up(a, b, c, d) strDCT2x2up_32s(a, b, c, d)
/* 1D transform T_odd */
/* [a b c d] => [D C A B] */
/* JPEG_XR D.4.2 TOdd */
#define fwdODDt(a, b, c, d)\
{\
\
    /* butterflies */\
    b -= c;\
    a += d;\
    c += (b + 1) >> 1;\
    d = ((a + 1) >> 1) - d;\
\
    /* rotate pi/8 */\
    ROTATE2(a, b);\
    ROTATE2(c, d);\
\
    /* butterflies */\
    d += (b) >> 1;\
    c -= (a + 1) >> 1;\
    b -= d;\
    a += c;\
}
#define fwdODDf(a, b, c, d)\
{\
\
    /* butterflies */\
    b -= c;\
    a += d;\
    c += b * 0.5f;\
    d = a * 0.5f - d;\
\
    /* rotate pi/8 */\
    ROTATE2f(a, b);\
    ROTATE2f(c, d);\
\
    /* butterflies */\
    d += b * 0.5f;\
    c -= a * 0.5f;\
    b -= d;\
    a += c;\
}


#define fwdOdd_32f(a, b, c, d) fwdODDf(a, b, c, d)
#define fwdOdd_32s(a, b, c, d) fwdODDt(a, b, c, d)
#define fwdOdd_16s(a, b, c, d) fwdODDt(a, b, c, d)
#define fwdOdd(a, b, c, d) fwdOdd_32s(a, b, c, d)

/* 2D transform T_odd_odd */
/* Equ to JPEG_XR D.4.3 TOddOdd */
#define fwdOddODDt(type, a, b, c, d)\
{\
    type t1, t2;\
\
    b = -b;\
    c = -c;\
\
    /* butterflies */\
    d += a;\
    c -= b;\
    a -= (t1 = d >> 1);\
    b += (t2 = c >> 1);\
\
    /* rotate pi/4 */\
    a += (b * 3 + 4) >> 3;\
    b -= (a * 3 + 3) >> 2;\
    a += (b * 3 + 3) >> 3;\
\
    /* butterflies */\
    b -= t2;\
    a += t1;\
    c += b;\
    d -= a;\
}
#define fwdOddODDf(type, a, b, c, d)\
{\
    type t1, t2;\
\
    b = -b;\
    c = -c;\
\
    /* butterflies */\
    d += a;\
    c -= b;\
    a -= (t1 = d * 0.5f);\
    b += (t2 = c * 0.5f);\
\
    /* rotate pi/4 */\
    a += b * 0.375f;\
    b -= a * 0.75f;\
    a += b * 0.375f;\
\
    /* butterflies */\
    b -= t2;\
    a += t1;\
    c += b;\
    d -= a;\
}

#define fwdOddOdd_32f(a, b, c, d) fwdOddODDf(Ipp32f, a, b, c, d)
#define fwdOddOdd_32s(a, b, c, d) fwdOddODDt(Ipp32s, a, b, c, d)
#define fwdOddOdd_16s(a, b, c, d) fwdOddODDt(Ipp16s, a, b, c, d)
#define fwdOddOdd(a, b, c, d) fwdOddOdd_32s(a, b, c, d)

/**********************************************************************
// Inverse transform functions
*/
/*   inverse rotation by -pi/8 */
/* JPEG-XR InvRotate 9.9.8.5 */
/*REFCode: _InvRotate */
#define IROTATE1(a, b) (a) -= (((b) + 1) >> 1), (b) += (((a) + 1) >> 1)
#define IROTATE1f(a, b) (a) -= (b) * 0.5f, (b) += (a) * 0.5f
#define IROTATE1_32f  IROTATE1f
#define IROTATE1_16s  IROTATE1
#define IROTATE1_32s  IROTATE1


#define IROTATE2(a, b) (a) -= (((b)*3 + 4) >> 3), (b) += (((a)*3 + 4) >> 3)
#define IROTATE2f(a, b) (a) -= (b) * 0.375f, (b) += (a) * 0.375f
/* inverse of 2D transform T_odd_odd */
/* Kron(Rotate(pi/8), Rotate(pi/8)) */
/* JPEG_XR 9.9.7.4 InvToddodd */
/* REFCode: _InvT_odd_odd identical*/
#define invOddODDt(type, a, b, c, d)\
{\
    type t1, t2;\
    /* butterflies */\
    d += a;\
    c -= b;\
    a -= (t1 = d >> 1);\
    b += (t2 = c >> 1);\
    /* rotate pi/4 */\
    a -= (b * 3 + 3) >> 3;\
    b += (a * 3 + 3) >> 2;\
    a -= (b * 3 + 4) >> 3;\
    /* butterflies */\
    b -= t2;\
    a += t1;\
    c += b;\
    d -= a;\
    /* sign flips */\
    b = -b;\
    c = -c;\
}
#define invOddODDf(type, a, b, c, d)\
{\
    type t1, t2;\
    /* butterflies */\
    d += a;\
    c -= b;\
    a -= (t1 = d * 0.5f);\
    b += (t2 = c * 0.5f);\
    /* rotate pi/4 */\
    a -= b * 0.375f;\
    b += a * 0.75f;\
    a -= b * 0.375f;\
    /* butterflies */\
    b -= t2;\
    a += t1;\
    c += b;\
    d -= a;\
    /* sign flips */\
    b = -b;\
    c = -c;\
}
#define invOddOdd_32f(a, b, c, d) invOddODDf(Ipp32f, a, b, c, d)
#define invOddOdd_32s(a, b, c, d) invOddODDt(Ipp32s, a, b, c, d)
#define invOddOdd_16s(a, b, c, d) invOddODDt(Ipp16s, a, b, c, d)
#define invOddOdd(a, b, c, d) invOddOdd_32s(a, b, c, d)

/* lossless inverse of 1D transform T_odd */
/* [D C A B] => [a b c d] */
/* JPEG_XR 9.9.7.3 InvTodd */
/* REFCode: _InvT_odd identical*/
#define  invODDt(type, a, b, c, d)\
{\
    /* butterflies */\
    b += d;\
    a -= c;\
    d -= (b) >> 1;\
    c += (a + 1) >> 1;\
\
    /* rotate pi/8 */\
    IROTATE2(a, b);\
    IROTATE2(c, d);\
\
    /* butterflies */\
    c -= (b + 1) >> 1;\
    d = ((a + 1) >> 1) - d;\
    b += c;\
    a -= d;\
}
#define  invODDf(type, a, b, c, d)\
{\
    /* butterflies */\
    b += d;\
    a -= c;\
    d -= b * 0.5f;\
    c += a * 0.5f;\
\
    /* rotate pi/8 */\
    IROTATE2f(a, b);\
    IROTATE2f(c, d);\
\
    /* butterflies */\
    c -= b * 0.5f;\
    d = a * 0.5f - d;\
    b += c;\
    a -= d;\
}
#define  invOdd_32f(a, b, c, d) invODDf(Ipp32f, a, b, c, d)
#define  invOdd_32s(a, b, c, d) invODDt(Ipp32s, a, b, c, d)
#define  invOdd_16s(a, b, c, d) invODDt(Ipp16s, a, b, c, d)
#define  invOdd(a, b, c, d) invOdd_32s(a, b, c, d)

/* JPEG_XR 9.9.8.8 InvToddoddPOST */
/* REFCode: _InvT_odd_odd_POST identical*/
#define invOddOddPOSTt(type, a, b, c, d)\
{\
    type t1, t2;\
    /* butterflies */\
    d += a;\
    c -= b;\
    a -= (t1 = d >> 1);\
    b += (t2 = c >> 1);\
\
    /* rotate pi/4 */\
    a -= (b * 3 + 6) >> 3;\
    b += (a * 3 + 2) >> 2;\
    a -= (b * 3 + 4) >> 3;\
\
    /* butterflies */\
    b -= t2;\
    a += t1;\
    c += b;\
    d -= a;\
}
#define invOddOddPOSTf(type, a, b, c, d)\
{\
    type t1, t2;\
    /* butterflies */\
    d += a;\
    c -= b;\
    a -= (t1 = d * 0.5f);\
    b += (t2 = c * 0.5f);\
\
    /* rotate pi/4 */\
    a -= b * 0.375f;\
    b += a * 0.75f;\
    a -= b * 0.375f;\
\
    /* butterflies */\
    b -= t2;\
    a += t1;\
    c += b;\
    d -= a;\
}

#define invOddOddPost_32f(a, b, c, d) invOddOddPOSTf(Ipp32f, a, b, c, d)
#define invOddOddPost_32s(a, b, c, d) invOddOddPOSTt(Ipp32s, a, b, c, d)
#define invOddOddPost_16s(a, b, c, d) invOddOddPOSTt(Ipp16s, a, b, c, d)


/* 4-point post-filter for boundaries */
#define ISCALE(a, b) \
    a += b;\
    b = (a >> 1) - b;\
    a += (b * 3 + 0) >> 3;\
    b -= a >> 10;\
    b += a >> 7;\
    b += (a * 3 + 0) >> 4

#define ISCALEf(a, b) \
    a += b;\
    b = a * 0.5f - b;\
    a += b * 3.f/8;\
    b -= a * 1.f/1024;\
    b += a * 1.f/128;\
    b += a * 3.f/16

/* JPEG_XR 9.9.8.2 OverlapPostFilter4 */
/*REFCode: _jxr_4OverlapFilter identical*/
#define strPost4t(a, b, c, d)\
{\
    a += d, b += c;\
    d -= ((a + 1) >> 1), c -= ((b + 1) >> 1);\
\
    ISCALE(a, d);\
    ISCALE(b, c);\
\
    a += ((d * 3 + 4) >> 3);\
    b += ((c * 3 + 4) >> 3);\
    d -= (a >> 1);\
    c -= (b >> 1);\
    a += d;\
    b += c;\
    d = -d;\
    c = -c;\
\
    IROTATE1(c, d);\
\
    d += ((a + 1) >> 1);\
    c += ((b + 1) >> 1);\
    a -= d;\
    b -= c;\
}
#define strPost4f(a, b, c, d)\
{\
    a += d, b += c;\
    d -= a * 0.5f, c -= b * 0.5f;\
\
    ISCALEf(a, d);\
    ISCALEf(b, c);\
\
    a += d * (float)(3.f/8);\
    b += c * (float)(3.f/8);\
    d -= a * 0.5f;\
    c -= b * 0.5f;\
    a += d;\
    b += c;\
    d *= -1.f;\
    c *= -1.f;\
\
    IROTATE1f(c, d);\
\
    d += a * 0.5f;\
    c += b * 0.5f;\
    a -= d;\
    b -= c;\
}

#define strPost4_16s(a, b, c, d) strPost4t(a, b, c, d)
#define strPost4_32s(a, b, c, d) strPost4t(a, b, c, d)
#define strPost4_32f(a, b, c, d) strPost4f(a, b, c, d)


/*
    Hadamard+Scale inverse transform
*/
/*REFCode: InvScale(a,d) + 2x2T_h_POST(a,b,c,d)  identical*/
#define strHSTDECt(type,a,b,c,d)\
{\
    type t;\
    a += d;\
    d = (a >> 1) - d;\
    a += (d * 3 + 0) >> 3;\
\
    d -= a >> 10;/*JXR*/\
    d += a >> 7; /*JXR*/\
\
    d += (a * 3 + 0) >> 4;\
    a += (d * 3 + 4) >> 3;\
    b -= c;\
    d -= (b >> 1);\
    c = ((a - b) >> 1) - c;\
    a -= c, b += d;\
    t = d;\
    d = c;\
    c = t;\
}
#define strHSTDECf(type,a,b,c,d)\
{\
    type t;\
    a += d;\
    d = a * 0.5f - d;\
    a += d * 0.375f;\
\
    d -= a * (1.f/2048);\
    d += a * (1.f/128); \
\
    d += a * (3.f/16);\
    a += d * 0.375f;\
    b -= c;\
    d -= b * 0.5f;\
    c = (a - b) * 0.5f - c;\
    a -= c, b += d;\
    t = d;\
    d = c;\
    c = t;\
}

#define strHSTdec_32f(a, b, c, d) strHSTDECf(Ipp32f, a, b, c, d)
#define strHSTdec_32s(a, b, c, d) strHSTDECt(Ipp32s, a, b, c, d)
#define strHSTdec_16s(a, b, c, d) strHSTDECt(Ipp16s, a, b, c, d)
/* 2-point post-filter for boundaries */
/*REFCode: _jxr_2OverlapFilter identical*/
#define strPost2t(a, b)\
{\
    b += ((a + 2) >> 2);\
    a += ((b + 1) >> 1);\
    a += b >> 5;\
    a += b >> 9;\
    a += b >> 13;\
    b += ((a + 2) >> 2);\
}
#define strPost2f(a, b)\
{\
    b += a * 0.25f;\
    a += b * 0.5f;\
    a += b * 1.f/32;\
    a += b * 1.f/512;\
    a += b * 1.f/8192;\
    b += a * 0.25f;\
}
#define strPost2_32f(a, b) strPost2f(a, b)
#define strPost2_32s(a, b) strPost2t(a, b)
#define strPost2_16s(a, b) strPost2t(a, b)

// 2x2 post-filter (only used in 420 UV DC subband)
/*REFCode: _jxr_2x2OverlapFilter identical*/
#define strPost2x2t(a, b, c, d)\
{\
    /* butterflies */\
    a += d;\
    b += c;\
    d -= (a + 1) >> 1;\
    c -= (b + 1) >> 1;\
\
    /* rotate */\
    b += ((a + 2) >> 2);\
    a += ((b + 1) >> 1);\
    a += b >> 5;\
    a += b >> 9;\
    a += b >> 13;\
    b += ((a + 2) >> 2);\
\
    /* butterflies */\
    d += (a + 1) >> 1;\
    c += (b + 1) >> 1;\
    a -= d;\
    b -= c;\
}
#define strPost2x2f(a, b, c, d)\
{\
    /* butterflies */\
    a += d;\
    b += c;\
    d -= a * 0.5f;\
    c -= b * 0.5f;\
\
    /* rotate */\
    b += a * 0.25f;\
    a += b * 0.5f;\
    a += b * 1.f/32;\
    a += b * 1.f/512;\
    a += b * 1.f/8192;\
    b += a * 0.25f;\
\
    /* butterflies */\
    d += a * 0.5f;\
    c += b * 0.5f;\
    a -= d;\
    b -= c;\
}
#define strPost2x2_32f(a, b, c, d) strPost2x2f(a, b, c, d)
#define strPost2x2_32s(a, b, c, d) strPost2x2t(a, b, c, d)
#define strPost2x2_16s(a, b, c, d) strPost2x2t(a, b, c, d)

/*
    Huffman and bitstream
*/


#define PUTBIT32(STR, uiBits, cBits)\
{\
    if (16 < cBits)\
    {\
        cBits -= 16;\
        putBit16z(STR, uiBits >> cBits, 16);\
    }\
    putBit16z(STR, (uiBits) & bitMask[cBits], cBits);\
}
// Inlined Ipp32u accumulator before actual putBit
// cuts 30% of putBit calls
#define DEFACC(STR)        Ipp32u bitacc=0; int bitlen=0;
#define ZEROACC(STR)       bitacc=0; bitlen=0


#define PUTACC(STR)        {PUTBIT32(STR,bitacc,bitlen); ZEROACC(STR);}
#define CHECKACC(STR,l) if((bitlen + l) > 32) PUTACC(STR)
#define BITACC(STR,b,l)\
{\
    int ll = l;\
    CHECKACC(STR,ll)\
    bitacc = (bitacc << (ll)) | ((b) & bitMask[ll]);\
    bitlen += ll;\
}
#define BITACCZ(STR,b,l)\
{\
    int ll = l;\
    CHECKACC(STR,ll)\
    bitacc = (bitacc << (ll)) | (b);\
    bitlen += ll;\
}


#endif /*__PJXR_H__*/
