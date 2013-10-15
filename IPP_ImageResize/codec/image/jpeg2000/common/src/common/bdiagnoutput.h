
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __BDIAGNOUTPUT_H__
#define __BDIAGNOUTPUT_H__

#include "diagndescr.h"

class BDiagnOutput {
public:
    BDiagnOutput() {}
    virtual ~BDiagnOutput() {}
    virtual void Warning(const BDiagnDescr &) {}
    virtual void Error  (const BDiagnDescr &) {}
};

class BDiagnOutputPtr {
public:
    BDiagnOutputPtr() { m_ptr = &m_dummy; }

    BDiagnOutputPtr(BDiagnOutput& diagnOutput) : m_ptr(&diagnOutput) {}

    BDiagnOutputPtr& operator=(BDiagnOutput &diagnOutput) {
        m_ptr = &diagnOutput;
        return *this;
    }

    BDiagnOutput * operator->() {
        return m_ptr;
    }

    operator BDiagnOutput &() {
        return *m_ptr;
    }

protected:
    BDiagnOutput  m_dummy;
    BDiagnOutput *m_ptr;
};

#endif // __BDIAGNOUTPUT_H__
