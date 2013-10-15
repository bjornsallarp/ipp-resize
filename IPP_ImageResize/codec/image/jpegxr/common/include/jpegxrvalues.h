/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __JPEGXRVALUES_H__
#define __JPEGXRVALUES_H__

// Huffman tables for adaptHuffman()
static const Ipp16s iHuffmanLookup5[2][42] =
{
  { 28,28,36,36,19,19,19,19,10,10,10,10,10,10,10,10,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0},
  { 11,11,11,11,19,19,19,19,27,27,27,27,35,35,35,35,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0}
};

static const Ipp16s iHuffmanLookup7[2][46] =
{
  { 45,53,36,36,27,27,27,27,2,2,2,2,2,2,2,2,
    10,10,10,10,10,10,10,10,18,18,18,18,18,18,18,18,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  { -32736,37,28,28,19,19,19,19,10,10,10,10,10,10,10,10,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    5,6,0,0,0,0,0,0,0,0,0,0,0,0}
};

static const Ipp16s iHuffmanLookup9[2][50] =
{
  { 13,29,37,61,20,20,68,68,3,3,3,3,51,51,51,51,
    41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  { -32736,53,28,28,11,11,11,11,19,19,19,19,43,43,43,43,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    -32734,4,7,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

static const Ipp16s iHuffmanCode5[] =
{
  5,1,1,1,2,1,3,0,4,1,4,
  5,1,1,0,3,1,3,2,3,3,3
};

static const Ipp16s iHuffmanCode7[] =
{
  7,1,2,2,2,3,2,1,3,1,4,0,5,1,5,
  7,1,1,1,2,1,3,1,4,1,5,0,6,1,6
};

static const Ipp16s iHuffmanCode9[] =
{
  9,2,3,0,5,2,4,1,5,2,5,1,1,3,3,3,5,3,4,
  9,1,1,1,3,2,3,1,4,1,6,3,3,1,5,0,7,1,7
};

static const Ipp8s iHuffmanDelta5[5] = { 0,-1, 0, 1, 1};
static const Ipp8s iHuffmanDelta7[7] = { 1, 0,-1,-1,-1,-1,-1};
static const Ipp8s iHuffmanDelta9[9] = { 2, 2, 1, 1,-1,-2,-2,-2,-3};
static const Ipp8u iMaxTables[13]    = { 0, 0, 0, 0, 1, 2, 4, 2, 2, 2, 0, 0, 5};

// Model bits weight coefficients for updateModel()
static const Ipp8u iWeightY[3]     = {240, 12, 1};
static const Ipp8u iWeightUV422[3] = {120, 18, 1};
static const Ipp8u iWeightUV420[3] = {120, 37, 2};
static const Ipp8u iWeightNCH[3][16] = {
  {0,240,120,80,60,48,40,34,30,27,24,22,20,18,17,16},
  {0, 12,  6, 4, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1},
  {0, 16,  8, 5, 4, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1}
};

static const Ipp8u iDCLength[] = { 0, 0, 1, 2, 2, 2};
static const Ipp8u iLP4XXRemap[] = {4, 1, 2, 3, 5, 6, 7};

// 4 bit ones count for predHPCBP()
static const Ipp8u iUnitCount[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

// Blocks lengths/offsets for readMacroblockHP(), writeMacroblockHP()
static const Ipp8u iBlocks444[32] = {0,0, 4,0, 0,4, 4,4, 8,0, 12,0, 8,4, 12,4, 0,8, 4,8, 0,12, 4,12, 8,8, 12,8, 8,12, 12,12};
static const Ipp8u iBlocks422[16] = {0,0, 4,0, 0,4, 4,4,                       0,8, 4,8, 0,12, 4,12};
static const Ipp8u iBlocks420[8]  = {0,0, 4,0, 0,4, 4,4};

static const Ipp8u iBOffset444[16] = {0, 64, 16, 80, 128, 192, 144, 208, 32, 96, 48, 112, 160, 224, 176, 240};
static const Ipp8u iBOffset422[8]  = {0, 64, 16, 80, 32, 96, 48, 112};
static const Ipp8u iBOffset420[4]  = {0, 32, 16, 48};


#endif // __JPEGXRVALUES_H__
