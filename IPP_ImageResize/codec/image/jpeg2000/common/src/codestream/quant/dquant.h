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

#ifndef __DQUANT_H__
#define __DQUANT_H__


#include "quant.h"
#include "jp2const.h"
#include "sbtree.h"

inline void QuantInv(const ImageCoreC<Ipp16s, 1> &img, const IppiSize &size, QuantValue quant, unsigned int sbDynRange)
{
    ippiMulC_16s_C1IRSfs((Ipp16s)(quant.Mant() + 2048), img.Data(), img.LineStep(), size, -((int)sbDynRange - quant.Expn() - 11));
}

inline void QuantInv(const ImageCoreC<Ipp32s, 1> &img, const IppiSize &size, QuantValue quant, unsigned int sbDynRange)
{
    Mul(img, size, quant.Mant() + 2048, -((int)sbDynRange - quant.Expn() - 11));
}

inline void QuantInv(const ImageCoreC<Ipp32f, 1> &img, const IppiSize &size, QuantValue quant, unsigned int sbDynRange)
{
    ippiMulC_32f_C1IR((Ipp32f)quant.Delta(sbDynRange), img.Data(), img.LineStep(),  size);
}

template<class T>
inline void QuantInv(const SBTree<T> &sbtree, const QuantComponent &qComponent, unsigned int bitDepth)
{
    unsigned int dynRange = bitDepth + 1;

    unsigned int nOfWTLevels = sbtree.NOfWTLevels();

    for(unsigned int sbDepth = 0; sbDepth < nOfWTLevels; sbDepth++)
    {
        QuantInv(sbtree.HxLy(sbDepth), sbtree.HxLy(sbDepth).Size(), qComponent.HxLy(sbDepth), dynRange + DYN_RANGE_GAIN_HxLy);
        QuantInv(sbtree.LxHy(sbDepth), sbtree.LxHy(sbDepth).Size(), qComponent.LxHy(sbDepth), dynRange + DYN_RANGE_GAIN_LxHy);
        QuantInv(sbtree.HxHy(sbDepth), sbtree.HxHy(sbDepth).Size(), qComponent.HxHy(sbDepth), dynRange + DYN_RANGE_GAIN_HxHy);
    }
    QuantInv(sbtree.LxLy(), sbtree.LxLy().Size(), qComponent.LxLy(), dynRange + DYN_RANGE_GAIN_LxLy);
}

#endif // __DQUANT_H__
