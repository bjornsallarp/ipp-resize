
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

#ifndef __CBCODINGSTYLE_H__
#define __CBCODINGSTYLE_H__

#include "pp.h"
#include "jp2const.h"

class CBCodingStyle {
public:
    CBCodingStyle() : m_ippFlags(0) {}

    CBCodingStyle(const CBCodingStyle &codingStyle)
    : m_ippFlags(codingStyle.m_ippFlags)
    {}

    CBCodingStyle& operator=(const CBCodingStyle& codingStyle)
    {
        m_ippFlags = codingStyle.m_ippFlags;
        return *this;
    }

    bool IsSelectiveMQBypass()       const { return 0 != (m_ippFlags & IPP_JPEG2K_SELECTIVE_MQ_BYPASS); }
    bool IsResetCtxOnEveryPass()     const { return 0 != (m_ippFlags & IPP_JPEG2K_RESETCTX_ON_EVERY_PASS); }
    bool IsTerminateOnEveryPass()    const { return 0 != (m_ippFlags & IPP_JPEG2K_TERMINATE_ON_EVERY_PASS); }
    bool IsVerticallyCausalContext() const { return 0 != (m_ippFlags & IPP_JPEG2K_VERTICALLY_CAUSAL_CONTEXT); }
    bool IsPredictableTermination()  const { return 0 != (m_ippFlags & IPP_JPEG2K_DEC_CHECK_PRED_TERM); }
    bool IsUseSegmentationSymbols()  const { return 0 != (m_ippFlags & IPP_JPEG2K_USE_SEGMENTATION_SYMBOLS); }

    void SetSelectiveMQBypass      (bool value) { if(value) m_ippFlags |= IPP_JPEG2K_SELECTIVE_MQ_BYPASS;       else m_ippFlags &= ~IPP_JPEG2K_SELECTIVE_MQ_BYPASS;        }
    void SetResetCtxOnEveryPass    (bool value) { if(value) m_ippFlags |= IPP_JPEG2K_RESETCTX_ON_EVERY_PASS;    else m_ippFlags &= ~IPP_JPEG2K_RESETCTX_ON_EVERY_PASS;     }
    void SetTerminateOnEveryPass   (bool value) { if(value) m_ippFlags |= IPP_JPEG2K_TERMINATE_ON_EVERY_PASS;   else m_ippFlags &= ~IPP_JPEG2K_TERMINATE_ON_EVERY_PASS;    }
    void SetVerticallyCausalContext(bool value) { if(value) m_ippFlags |= IPP_JPEG2K_VERTICALLY_CAUSAL_CONTEXT; else m_ippFlags &= ~IPP_JPEG2K_VERTICALLY_CAUSAL_CONTEXT;  }
    void SetPredictableTermination (bool value) { if(value) m_ippFlags |= IPP_JPEG2K_DEC_CHECK_PRED_TERM;       else m_ippFlags &= ~IPP_JPEG2K_DEC_CHECK_PRED_TERM;        }
    void SetUseSegmentationSymbols (bool value) { if(value) m_ippFlags |= IPP_JPEG2K_USE_SEGMENTATION_SYMBOLS;  else m_ippFlags &= ~IPP_JPEG2K_USE_SEGMENTATION_SYMBOLS;   }

    void SetFromCODOrCOCField(Ipp8u SPCodField)
    {
        SetSelectiveMQBypass      ((SPCodField & JP2F_SELECTIVE_MQ_BYPASS      ) != 0);
        SetResetCtxOnEveryPass    ((SPCodField & JP2F_RESETCTX_ON_EVERY_PASS   ) != 0);
        SetTerminateOnEveryPass   ((SPCodField & JP2F_TERMINATE_ON_EVERY_PASS  ) != 0);
        SetVerticallyCausalContext((SPCodField & JP2F_VERTICALLY_CAUSAL_CONTEXT) != 0);
        SetPredictableTermination ((SPCodField & JP2F_PREDICTABLE_TERMINATION  ) != 0);
        SetUseSegmentationSymbols ((SPCodField & JP2F_USE_SEGMENTATION_SYMBOLS ) != 0);
    }

    Ipp8u CODOrCOCField() const
    {
        Ipp8u SPCodField = 0;

        if(IsSelectiveMQBypass())       SPCodField |= JP2F_SELECTIVE_MQ_BYPASS;
        if(IsResetCtxOnEveryPass())     SPCodField |= JP2F_RESETCTX_ON_EVERY_PASS;
        if(IsTerminateOnEveryPass())    SPCodField |= JP2F_TERMINATE_ON_EVERY_PASS;
        if(IsVerticallyCausalContext()) SPCodField |= JP2F_VERTICALLY_CAUSAL_CONTEXT;
        if(IsPredictableTermination())  SPCodField |= JP2F_PREDICTABLE_TERMINATION;
        if(IsUseSegmentationSymbols())  SPCodField |= JP2F_USE_SEGMENTATION_SYMBOLS;

        return SPCodField;
    }

    Ipp32u IPPFlags() const { return m_ippFlags; }


protected:
    Ipp32u m_ippFlags;
};

#endif // __CBCODINGSTYLE_H__

