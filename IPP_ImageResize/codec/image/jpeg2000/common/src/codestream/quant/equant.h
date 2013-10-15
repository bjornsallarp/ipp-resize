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

#include "quant.h"
#include "bytebuffer.h"
#include "jp2const.h"
#include "sbtree.h"

inline void WriteQuantMode(JP2QuantMode type, unsigned int guardBits,
    ByteBuffer &stream)
{
    stream.Write8u((Ipp8u)((guardBits << JP2S_GUARD_BITS) | type));
}

inline void WriteQuantValue(const QuantValue &value, ByteBuffer &stream)
{
    stream.Write16u((Ipp16u)(value.Mant() | (value.Expn() << JP2S_QUANT_EXP) ));
}

/*
inline void WriteQuantExpounded(const QuantComponent &qComponent, unsigned int guardBits,
    ByteBuffer &stream)
{
    WriteQuantMode(JP2V_QUANT_EXPOUNDED, guardBits, stream);

    WriteQuantValue(qComponent.LxLy(), stream);

    for(unsigned int i = qComponent.NOfWTLevels(); i > 0; )
    {
        i--;
        WriteQuantValue(qComponent.HxLy(i), stream);
        WriteQuantValue(qComponent.LxHy(i), stream);
        WriteQuantValue(qComponent.HxHy(i), stream);
    }
}
*/

inline void WriteQuantDerived(const QuantValue &value, unsigned int guardBits,
    ByteBuffer &stream)
{
    WriteQuantMode(JP2V_QUANT_DERIVED, guardBits, stream);

    WriteQuantValue(value, stream);
}

inline void WriteQuantWT53(unsigned int cmpDynRange, unsigned int guardBits,
    unsigned int nOfWTLevels, ByteBuffer &stream)
{
    WriteQuantMode(JP2V_QUANT_NO, guardBits, stream);

    stream.Write8u((Ipp8u)((cmpDynRange + DYN_RANGE_GAIN_LxLy) << JP2S_NOQUANT_EXP));

    for (unsigned int i = 0; i < nOfWTLevels; i++)
    {
        stream.Write8u((Ipp8u)((cmpDynRange + DYN_RANGE_GAIN_HxLy) << JP2S_NOQUANT_EXP));
        stream.Write8u((Ipp8u)((cmpDynRange + DYN_RANGE_GAIN_LxHy) << JP2S_NOQUANT_EXP));
        stream.Write8u((Ipp8u)((cmpDynRange + DYN_RANGE_GAIN_HxHy) << JP2S_NOQUANT_EXP));
    }
}

inline void WriteQuantUnitStep(unsigned int cmpDynRange, unsigned int guardBits,
    unsigned int nOfWTLevels, ByteBuffer &stream)
{
    WriteQuantMode(JP2V_QUANT_EXPOUNDED, guardBits, stream);

    stream.Write16u((Ipp16u)((cmpDynRange + DYN_RANGE_GAIN_LxLy) << JP2S_QUANT_EXP));

    for (Ipp8u i = 0; i < nOfWTLevels; i++)
    {
        stream.Write16u((Ipp16u)((cmpDynRange + DYN_RANGE_GAIN_HxLy) << JP2S_QUANT_EXP));
        stream.Write16u((Ipp16u)((cmpDynRange + DYN_RANGE_GAIN_LxHy) << JP2S_QUANT_EXP));
        stream.Write16u((Ipp16u)((cmpDynRange + DYN_RANGE_GAIN_HxHy) << JP2S_QUANT_EXP));
    }
}

/*
inline void QuantFwd(const Image16sC1 &img, int mant, int expn, int dynRange)
{
    Ipp16s *pSrcDst = img.Data();
    int srcDstStep = img.LineStep();

    for(int y = 0; y < img.Height(); y++)
    {
        ippsDivC_16s_ISfs((mant + 2048), pSrcDst, img.Width(), (dynRange - expn - 11));
        pSrcDst = addrAdd(pSrcDst, srcDstStep);
    }
}
*/

inline void QuantFwd(const Image32sC1 &img, int mant, int expn, int dynRange)
{
    Ipp32s *pSrcDst = img.Data();
    int srcDstStep = img.LineStep();

    Ipp32f val = (Ipp32f)(mant + 2048);
    Ipp32s scaleFactor = dynRange - expn - 11;

    if(scaleFactor > 0)
        val = 1 / (val * (1 << scaleFactor));
    else
        val = (1 << (-scaleFactor)) / val;


    for(unsigned int y = 0; y < img.Height(); y++)
    {
        for(unsigned int x = 0; x < img.Width(); x++)
            pSrcDst[x] = (Ipp32s)(val * (Ipp32f)pSrcDst[x]);

        pSrcDst = addrAdd(pSrcDst, srcDstStep);
    }
}

inline void QuantFwd(const Image32fC1 &img, double delta)
{
//    Mul(img, img.Size(), (Ipp32f)(1.0/delta));
//inline void Mul(const ImageCoreC<Ipp32f, 1> &srcDst, const IppiSize &size, Ipp32f value)
//{
    ippiMulC_32f_C1IR((Ipp32f)(1.0/delta), img.Data(), img.LineStep(),  img.Size());
//}
}

inline void QuantFwd(const SBTree<Ipp32f> &sbtree, const QuantComponent &qComponent, int dynRange)
{
    int nOfWTLevels = sbtree.NOfWTLevels();

    for(int sbDepth = 0; sbDepth < nOfWTLevels; sbDepth++)
    {
        QuantFwd(sbtree.HxLy(sbDepth), qComponent.HxLy(sbDepth).Delta(dynRange));
        QuantFwd(sbtree.LxHy(sbDepth), qComponent.LxHy(sbDepth).Delta(dynRange));
        QuantFwd(sbtree.HxHy(sbDepth), qComponent.HxHy(sbDepth).Delta(dynRange));
    }
    QuantFwd(sbtree.LxLy(), qComponent.LxLy().Delta(dynRange));
}

template<class T>
void QuantFwd(const SBTree<T> &sbtree, const QuantComponent &qComponent, int dynRange)
{
    int nOfWTLevels = sbtree.NOfWTLevels();

    for(int sbDepth = 0; sbDepth < nOfWTLevels; sbDepth++)
    {
        QuantFwd(sbtree.HxLy(sbDepth), qComponent.HxLy(sbDepth).Mant(), qComponent.HxLy(sbDepth).Expn(), dynRange);
        QuantFwd(sbtree.LxHy(sbDepth), qComponent.LxHy(sbDepth).Mant(), qComponent.LxHy(sbDepth).Expn(), dynRange);
        QuantFwd(sbtree.HxHy(sbDepth), qComponent.HxHy(sbDepth).Mant(), qComponent.HxHy(sbDepth).Expn(), dynRange);
    }
    QuantFwd(sbtree.LxLy(), qComponent.LxLy().Mant(), qComponent.LxLy().Expn(), dynRange);
}


