
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

#ifndef __DJP2BOXEXCEPTION_H__
#define __DJP2BOXEXCEPTION_H__

typedef enum {
    boxOfUnknownType,
    badLBoxValue,
    badXLBoxValue,
    tooLongBoxLen,
    imageHeaderBoxNotFound,
    imageHeaderBoxSize,
    imageHeaderBoxComponentNumber,
    imageHeaderBoxCompressionType,
    imageHeaderBoxIntellectualProperty
} DJP2BoxException;

#endif // __DJP2BOXEXCEPTION_H__
