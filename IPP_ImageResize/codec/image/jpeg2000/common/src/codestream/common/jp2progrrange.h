
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

#ifndef __JP2PROGRRANGE_H__
#define __JP2PROGRRANGE_H__

#include "jp2const.h"
#include "geometry1d.h"

class JP2ProgrRange
{
public:
    JP2ProgrRange()
    : m_progrType(JP2V_LRCP_PROGR)
    , m_nOfLayers(0)
    {}

    void SetProgrType    (JP2ProgrType     progrType)     { m_progrType     = progrType;     }
    void SetResLevelRange(const UIntRange &resLevelRange) { m_resLevelRange = resLevelRange; }
    void SetCompRange    (const UIntRange &compRange)     { m_compRange     = compRange;     }
    void SetNOfLayers    (unsigned int     nOfLayers)     { m_nOfLayers     = nOfLayers;     }

    JP2ProgrType     ProgrType    () const { return m_progrType;     }
    const UIntRange &ResLevelRange() const { return m_resLevelRange; }
    const UIntRange &CompRange    () const { return m_compRange;     }
    unsigned int     NOfLayers    () const { return m_nOfLayers;     }

protected:
    JP2ProgrType m_progrType;
    UIntRange    m_resLevelRange;
    UIntRange    m_compRange;
    unsigned int m_nOfLayers;
};

#endif // __JP2PROGRRANGE_H__
