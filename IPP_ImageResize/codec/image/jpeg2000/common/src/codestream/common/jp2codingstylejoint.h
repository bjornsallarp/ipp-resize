
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

#ifndef __JP2CODINGSTYLEJOINT_H__
#define __JP2CODINGSTYLEJOINT_H__

#include "jp2const.h"

class JP2CodingStyleJoint
{
public:
    JP2CodingStyleJoint()
    : m_progrType(JP2V_LRCP_PROGR)
    , m_nOfLayers(0)
    , m_isSOPUsed(false)
    , m_isEPHUsed(false)
    , m_isMCTUsed(false)
    {}

    void Reset()
    {
        m_progrType = JP2V_LRCP_PROGR;
        m_nOfLayers = 0;
        m_isSOPUsed = false;
        m_isEPHUsed = false;
        m_isMCTUsed = false;

    }

    void SetProgrType(JP2ProgrType progrType) { m_progrType = progrType; }
    void SetNOfLayers(unsigned int nOfLayers) { m_nOfLayers = nOfLayers; }
    void SetIsSOPUsed(bool isSOPUsed = true)  { m_isSOPUsed = isSOPUsed; }
    void SetIsEPHUsed(bool isEPHUsed = true)  { m_isEPHUsed = isEPHUsed; }
    void SetIsMCTUsed(bool isMCTUsed = true)  { m_isMCTUsed = isMCTUsed; }

    JP2ProgrType ProgrType() const { return m_progrType; }
    unsigned int NOfLayers() const { return m_nOfLayers; }
    bool         IsSOPUsed() const { return m_isSOPUsed; }
    bool         IsEPHUsed() const { return m_isEPHUsed; }
    bool         IsMCTUsed() const { return m_isMCTUsed; }

protected:
    JP2ProgrType m_progrType;
    unsigned int m_nOfLayers;
    bool         m_isSOPUsed;
    bool         m_isEPHUsed;
    bool         m_isMCTUsed;
};

#endif // __JP2CODINGSTYLEJOINT_H__

