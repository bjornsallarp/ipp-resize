
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

#ifndef __JP2CODINGSTYLECOMP_H__
#define __JP2CODINGSTYLECOMP_H__

#include "cbcodingstyle.h"
#include "wtmetric.h"
#include "optionalarray.h"

class JP2CodingStyleComp
{
public:
    JP2CodingStyleComp()
    : m_wtType(JP2V_WT53_USED)
    , m_nOfWTLevels(0)
    {}

    const CBCodingStyle& CBStyle()       const { return   m_cbCodingStyle;             }
    JP2WTSpecType        WTType()        const { return   m_wtType;                    }
    unsigned int         NOfWTLevels()   const { return   m_nOfWTLevels;               }
    unsigned int         NOfResLevels()  const { return ::NOfResLevels(NOfWTLevels()); }
    const RectSize&      CBStepOrder()   const { return   m_cbStepOrder;               }
    const RectSize*      PrecStepOrder() const { return   m_precStepOrder;             }

    void ReAlloc(unsigned int nOfWTLevels)
    {
        m_nOfWTLevels = nOfWTLevels;
        m_precStepOrder.ReAlloc(NOfResLevels());
    }

    void SetCBStyle      (const CBCodingStyle& cbCodingStyle) { m_cbCodingStyle = cbCodingStyle; }
    void SetWTType       (JP2WTSpecType        wtType       ) { m_wtType        = wtType       ; }
    void SetCBStepOrder  (const RectSize&      cbStepOrder  ) { m_cbStepOrder   = cbStepOrder  ; }

    void SetPrecStepOrder(const RectSize& precStepOrder, unsigned int resLevel)
    {
        m_precStepOrder[resLevel] = precStepOrder;
    }

protected:
    CBCodingStyle         m_cbCodingStyle;
    JP2WTSpecType         m_wtType;
    unsigned int          m_nOfWTLevels;
    RectSize              m_cbStepOrder;
    FixedBuffer<RectSize> m_precStepOrder;
};

typedef OptionalArray<JP2CodingStyleComp>       JP2CodingStyleCompMain;
typedef OptionalSuperArray <JP2CodingStyleComp> JP2CodingStyleCompTile;

#endif // __JP2CODINGSTYLECOMP_H__

