
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

#ifndef __DJP2CSUNSUPPEXCEPTION_H__
#define __DJP2CSUNSUPPEXCEPTION_H__

typedef enum {
    SOPStyleUsed,
    EPHStyleUsed,
    PPMUsed,
    PPTUsed,
    RGNUsed,
    POCUsed,
    badCodeblockSize,
    badCodeblockStyle,
    signedComponent,
    isNotEnumeratedColourspace,
    cmapNotSupportedNOfChannels,
    cmapChannelDirectUse,
    cmapBadComponentNumber,
    cmapBadPaletteComponentIndex,
    pclrBadNOfPaletteEntries,
    pclrBadNOfPaletteChannels,
    pclrBadPaletteBitDepth,
    NonterminatedMultilayer,
    mctIsResampled
} DJP2CSUnsuppException;

#endif // __DJP2CSUNSUPPEXCEPTION_H__
