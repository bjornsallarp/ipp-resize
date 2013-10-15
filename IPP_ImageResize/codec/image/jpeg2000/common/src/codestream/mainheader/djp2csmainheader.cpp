
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2003-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/


#include "djp2csmainheader.h"

void DJP2CSMainHeader::Reset()
{
    m_imageRectRG = Rect();
    m_tileGridOriginRG = Point();
    m_tileSizeRG = RectSize();

    m_nOfComponents = 0;

    m_bitsPerComp.Free();
    m_sampleSizeRG.Free();

    m_quant.Free();

    m_codingStyleComp.Free();
    m_codingStyleJoint.Reset();

    m_changedProgrRange.Free();

    m_CODNotFound = true;
}



