
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

#ifndef __DJP2MARKEREXCEPTION_H__
#define __DJP2MARKEREXCEPTION_H__

typedef enum {
    NegativeBodyLen,
    SIZGeometryTooLage,
    SIZImageNeagativeSize,
    BadCODProgressionOrder,
    BadCODOrCOCNumberOfDecompositionLevels,
    BadCODOrCOCCodeBlockWidth,
    BadCODOrCOCCodeBlockHeight,
    BadCODOrCOCTransformationType,
    POCResLevelIndexExceedBound,
    POCCompIndexExceedNOfComponents,
    POCInverseResLevelProgr,
    POCInverseCompProgr
} DJP2MarkerException;

#endif // __DJP2MARKEREXCEPTION_H__
