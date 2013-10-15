/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_EXC_STATUS_H__
#define __UIC_EXC_STATUS_H__

namespace UIC {

typedef int ExcStatus;

static const ExcStatus ExcStatusOk   =  0;
static const ExcStatus ExcStatusFail = -1;

inline bool IsOk(const ExcStatus &status) { return status == ExcStatusOk; }

#define RET_ON_EXC(excCall) { ExcStatus status = excCall; if(!IsOk(status)) return status; }
#define BEGIN_FINALIZE_RET_ON_EXC(excCall) { ExcStatus status = excCall; if(!IsOk(status))  {{
#define END_FINALIZE_RET_ON_EXC } return status; } }

} // namespace UIC

#endif // __UIC_EXC_STATUS_H__
