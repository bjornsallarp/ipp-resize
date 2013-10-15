
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2002-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __WT97CONST32F_H__
#define __WT97CONST32F_H__

static const Ipp32f FWDH3      = (Ipp32f) 1.115087052457;
static const Ipp32f FWDH1x2    = (Ipp32f)-0.115087052457;

static const Ipp32f FWDL2x2    = (Ipp32f)-0.15644653305798;
static const Ipp32f FWDL4pL0x2 = (Ipp32f) 0.65644653305798;

static const Ipp32f ALPHA   = (Ipp32f)-1.586134342059924;
static const Ipp32f BETA    = (Ipp32f)-0.052980118572961;
static const Ipp32f GAMMA   = (Ipp32f) 0.882911075530934;
static const Ipp32f DELTA   = (Ipp32f) 0.443506852043971;
static const Ipp32f KH      = (Ipp32f) 1.230174104914001;
static const Ipp32f KL      = (Ipp32f) 0.812893066115960;

static const Ipp32f ALPHAx2 = (Ipp32f)(2.*-1.586134342059924);
static const Ipp32f BETAx2  = (Ipp32f)(2.*-0.052980118572961);
static const Ipp32f GAMMAx2 = (Ipp32f)(2.* 0.882911075530934);
static const Ipp32f DELTAx2 = (Ipp32f)(2.* 0.443506852043971);

static const Ipp32f DELTAxKL = (Ipp32f) 0.3605236448014610;

static const Ipp32f HALFmFWDL2x4 = (Ipp32f)0.81289306611596;
static const Ipp32f DELTAxKLx2 = (Ipp32f) 0.721047289602922;

#endif // __WT97CONST32F_H__
