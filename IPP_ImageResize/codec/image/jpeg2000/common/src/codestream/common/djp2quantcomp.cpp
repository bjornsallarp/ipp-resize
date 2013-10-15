
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


#include "djp2quantcomp.h"
#include "djp2csexception.h"

void DJP2QuantComp::Reset()
{
    m_mode             = JP2V_QUANT_NO;
    m_guardBits        = 0;
    m_nOfWTLevels      = 0;
    m_isEmpty          = true;
    m_derivedBaseValue = QuantValue();
    m_quant.Free();
}

void DJP2QuantComp::Conform(unsigned int nOfWTLevels)
{
    if(m_mode == JP2V_QUANT_DERIVED)
    {
        // derived quantization may be tuned for each tile-part header
        m_quant.InitDerived(m_derivedBaseValue, nOfWTLevels);
    }
    else
    {
        if(nOfWTLevels > m_nOfWTLevels)
            throw DiagnDescrCT<DJP2CSException,OCDOrQCCLackOfQuantValues>();
    }
}

