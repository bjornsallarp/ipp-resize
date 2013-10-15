
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2007-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __WT97INVBORDERINDEXLMT_H__
#define __WT97INVBORDERINDEXLMT_H__

static const int WT97L1INDEX_L_m_1[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0, -1, -1}, //   0
      {0,  0, -1, -1}, //   1
      {1,  0, -1, -1}, //   2
      {1,  0, -1, -1}  // >=3
};
static const int WT97L1INDEX_L_p_1[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0, -1, -1}, //   0
      {0,  0,  0,  0}, //   1
      {1,  1,  1,  1}, //   2
      {1,  1,  1,  1}  // >=3
};
static const int WT97L1INDEX_H_m_2[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0, -1, -1, -2}, //   0
      {0,  0, -1, -2}, //   1
      {0,  0, -1, -2}, //   2
      {1,  0, -1, -2}  // >=3
};
static const int WT97L1INDEX_H_m_1[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0, -1, -1, -1}, //   0
      {0, -1, -1, -1}, //   1
      {0, -1, -1, -1}, //   2
      {0, -1, -1, -1}  // >=3
};
static const int WT97L1INDEX_H_p_0[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0, -1, -1, -1}, //   0
      {0,  0,  0,  0}, //   1
      {0,  0,  0,  0}, //   2
      {0,  0,  0,  0}  // >=3
};
static const int WT97L1INDEX_H_p_1[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0, -1, -1, -2}, //   0
      {0, -1, -1, -1}, //   1
      {0,  0,  0,  0}, //   2
      {1,  1,  1,  1}  // >=3
};

static const int WT97L2INDEX_L_m_1[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0, -1, -1}, //   0
      {1,  0, -1, -1}, //   1
      {1,  0, -1, -1}, //   2
      {1,  0, -1, -1}, //   3
      {1,  0, -1, -1}  // >=4
};
static const int WT97L2INDEX_L_p_1[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0,  0,  0}, //   0
      {1,  1,  1,  1}, //   1
      {1,  1,  1,  1}, //   2
      {1,  1,  1,  1}, //   3
      {1,  1,  1,  1}  // >=4
};
static const int WT97L2INDEX_L_p_2[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0, -1, -1}, //   0
      {0,  0,  0,  0}, //   1
      {1,  1,  1,  1}, //   2
      {2,  2,  2,  2}, //   3
      {2,  2,  2,  2}  // >=4
};
static const int WT97L2INDEX_H_m_2[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0, -1, -2}, //   0
      {0,  0, -1, -2}, //   1
      {1,  0, -1, -2}, //   2
      {1,  0, -1, -2}, //   3
      {1,  0, -1, -2}  // >=4
};
static const int WT97L2INDEX_H_m_1[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0, -1, -1, -1}, //   0
      {0, -1, -1, -1}, //   1
      {0, -1, -1, -1}, //   2
      {0, -1, -1, -1}, //   3
      {0, -1, -1, -1}  // >=4
};
static const int WT97L2INDEX_H_p_1[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0, -1, -1, -1}, //   0
      {0,  0,  0,  0}, //   1
      {1,  1,  1,  1}, //   2
      {1,  1,  1,  1}, //   3
      {1,  1,  1,  1}  // >=4
};
static const int WT97L2INDEX_H_p_2[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0, -1, -2}, //   0
      {0, -1, -1, -1}, //   1
      {0,  0,  0,  0}, //   2
      {1,  1,  1,  1}, //   3
      {2,  2,  2,  2}  // >=4
};


static const int WT97L3INDEX_L_m_1[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {1,  0, -1, -1}, //   0
      {1,  0, -1, -1}, //   1
      {1,  0, -1, -1}, //   2
      {1,  0, -1, -1}  // >=3
};
static const int WT97L3INDEX_L_p_2[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0,  0,  0}, //   0
      {1,  1,  1,  1}, //   1
      {2,  2,  2,  2}, //   2
      {2,  2,  2,  2}  // >=3
};
static const int WT97L3INDEX_H_m_2[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0, -1, -2}, //   0
      {1,  0, -1, -2}, //   1
      {1,  0, -1, -2}, //   2
      {1,  0, -1, -2}  // >=3
};
static const int WT97L3INDEX_H_m_1[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0, -1, -1, -1}, //   0
      {0, -1, -1, -1}, //   1
      {0, -1, -1, -1}, //   2
      {0, -1, -1, -1}  // >=3
};
static const int WT97L3INDEX_H_p_1[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0,  0,  0}, //   0
      {1,  1,  1,  1}, //   1
      {1,  1,  1,  1}, //   2
      {1,  1,  1,  1}  // >=3
};
static const int WT97L3INDEX_H_p_2[4][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0, -1, -1, -1}, //   0
      {0,  0,  0,  0}, //   1
      {1,  1,  1,  1}, //   2
      {2,  2,  2,  2}  // >=3
};


static const int WT97L4INDEX_L_m_1[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {1,  0, -1, -1}, //   0
      {1,  0, -1, -1}, //   1
      {1,  0, -1, -1}, //   2
      {1,  0, -1, -1}, //   3
      {1,  0, -1, -1}  // >=4
};
static const int WT97L4INDEX_L_p_2[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {1,  1,  1,  1}, //   0
      {2,  2,  2,  2}, //   1
      {2,  2,  2,  2}, //   2
      {2,  2,  2,  2}, //   3
      {2,  2,  2,  2}  // >=4
};
static const int WT97L4INDEX_L_p_3[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0,  0,  0}, //   0
      {1,  1,  1,  1}, //   1
      {2,  2,  2,  2}, //   2
      {3,  3,  3,  3}, //   3
      {3,  3,  3,  3}  // >=4
};
static const int WT97L4INDEX_H_m_2[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {1,  0, -1, -2}, //   0
      {1,  0, -1, -2}, //   1
      {1,  0, -1, -2}, //   2
      {1,  0, -1, -2}, //   3
      {1,  0, -1, -2}  // >=4
};
static const int WT97L4INDEX_H_m_1[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0, -1, -1, -1}, //   0
      {0, -1, -1, -1}, //   1
      {0, -1, -1, -1}, //   2
      {0, -1, -1, -1}, //   3
      {0, -1, -1, -1}  // >=4
};
static const int WT97L4INDEX_H_p_2[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0,  0,  0,  0}, //   0
      {1,  1,  1,  1}, //   1
      {2,  2,  2,  2}, //   2
      {2,  2,  2,  2}, //   3
      {2,  2,  2,  2}  // >=4
};
static const int WT97L4INDEX_H_p_3[5][4] =
{
//    toBegBorder
//    0   1   2 >=3  |    toEndBorder
      {0, -1, -1, -1}, //   0
      {0,  0,  0,  0}, //   1
      {1,  1,  1,  1}, //   2
      {2,  2,  2,  2}, //   3
      {3,  3,  3,  3}  // >=4
};


static const int WT97L5INDEX_L_m_1[4] = { 1,  0, -1, -1};
static const int WT97L5INDEX_H_m_1[4] = { 0, -1, -1, -1};
static const int WT97L5INDEX_H_m_2[4] = { 1,  0, -1, -2};

static const int WT97L5INDEX_L_p_1[5] = { 0,  1,  1,  1,  1};
static const int WT97L5INDEX_L_p_2[5] = {-1,  0,  1,  2,  2};

static const int WT97L5INDEX_H_p_1[5] = {-1,  0,  1,  1,  1};
static const int WT97L5INDEX_H_p_2[5] = {-2, -1,  0,  1,  2};


#endif // __WT97INVBORDERINDEXLMT_H__

