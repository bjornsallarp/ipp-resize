
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

#ifndef __QUANT_H__
#define __QUANT_H__

#include <math.h>

#include "jp2const.h"
#include "fixedarray.h"

static const unsigned int DYN_RANGE_GAIN_LxLy = 0;
static const unsigned int DYN_RANGE_GAIN_HxLy = 1;
static const unsigned int DYN_RANGE_GAIN_LxHy = 1;
static const unsigned int DYN_RANGE_GAIN_HxHy = 2;

class QuantValue
{
public:
    QuantValue()                        : m_mant(0),            m_expn(0)            {}
    QuantValue(int mant, int expn)      : m_mant(mant),         m_expn(expn)         {}
    QuantValue(const QuantValue &quant) : m_mant(quant.Mant()), m_expn(quant.Expn()) {}

    QuantValue& operator=(const QuantValue& quant)
    {
        if(this != &quant)
        {
            m_mant = quant.m_mant;
            m_expn = quant.m_expn;
        }
        return *this;
    }

    void SetMant(int mant) { m_mant = mant; }
    void SetExpn(int expn) { m_expn = expn; }

    int Mant() const { return m_mant; }
    int Expn() const { return m_expn; }

    double Delta(int DynRange) const
    {
        static const double K = 1.0 / (double)(1 << 11);
        return pow(2.0, DynRange - m_expn) * (1.0 + (double)m_mant * K);
    }

protected:
    int m_mant;
    int m_expn;
};

inline QuantValue QuantFromDelta(float delta, int DynRange)
{
    QuantValue value;

    int    exponent;
    double mantissa = fabs(frexp(delta, &exponent));

    if(!(mantissa > 0))
    {
        value.SetMant(0);
        value.SetExpn(31);
        return value;
    }

    mantissa = (mantissa - 0.5) * 4096.0;

    if     (mantissa < 0.0)    value.SetMant(0);
    else if(mantissa > 2047.0) value.SetMant(2047);
    else    value.SetMant((int)(mantissa+0.5));

    value.SetExpn(DynRange - exponent + 1);

    return value;
}

class QuantTriplet
{
public:
    QuantTriplet() {}

    QuantTriplet(const QuantValue &quant)
    : m_HxLy(quant)
    , m_LxHy(quant)
    , m_HxHy(quant)
    {}

    QuantTriplet(
        const QuantValue &quantHxLy,
        const QuantValue &quantLxHy,
        const QuantValue &quantHxHy)
    : m_HxLy(quantHxLy)
    , m_LxHy(quantLxHy)
    , m_HxHy(quantHxHy)
    {}

    QuantTriplet(const QuantTriplet &triplet)
    : m_HxLy(triplet.HxLy())
    , m_LxHy(triplet.LxHy())
    , m_HxHy(triplet.HxHy())
    {}

    QuantTriplet& operator=(const QuantTriplet& triplet)
    {
        if(this != &triplet)
        {
            m_HxLy = triplet.HxLy();
            m_LxHy = triplet.LxHy();
            m_HxHy = triplet.HxHy();
        }
        return *this;
    }

    void SetHxLy(const QuantValue &HxLy) { m_HxLy = HxLy; }
    void SetLxHy(const QuantValue &LxHy) { m_LxHy = LxHy; }
    void SetHxHy(const QuantValue &HxHy) { m_HxHy = HxHy; }

    const QuantValue &HxLy() const { return m_HxLy; }
    const QuantValue &LxHy() const { return m_LxHy; }
    const QuantValue &HxHy() const { return m_HxHy; }

protected:
    QuantValue m_HxLy;
    QuantValue m_LxHy;
    QuantValue m_HxHy;
};

class QuantComponent
{
public:
    QuantComponent() {}
    QuantComponent(unsigned int nOfWTLevels) { ReAlloc(nOfWTLevels); }

/*
    QuantComponent(const QuantComponent &quantComponent)
    : m_LxLy(quantComponent.m_LxLy)
    , m_triplets(quantComponent.m_triplets)
    {}

    QuantComponent& operator=(const QuantComponent& quantComponent)
    {
        if(this != &quantComponent)
        {
            m_LxLy     = quantComponent.m_LxLy;
            m_triplets = quantComponent.m_triplets;
        }
        return *this;
    }
*/

    void ReAlloc(unsigned int nOfWTLevels)
    {
        m_triplets.ReAlloc(nOfWTLevels);
    }

    void InitUnitStep(int cmpDynRange, int nOfWTLevels) // unit step quantization
    {
        m_triplets.ReAlloc(nOfWTLevels);

        SetLxLy(QuantValue(0, cmpDynRange + DYN_RANGE_GAIN_LxLy));

        for (int sbDepth = 0; sbDepth < nOfWTLevels; sbDepth++)
        {
            SetHxLy(QuantValue(0, cmpDynRange + DYN_RANGE_GAIN_HxLy), sbDepth);
            SetLxHy(QuantValue(0, cmpDynRange + DYN_RANGE_GAIN_LxHy), sbDepth);
            SetHxHy(QuantValue(0, cmpDynRange + DYN_RANGE_GAIN_HxHy), sbDepth);
        }
    }

    void InitDerived(const QuantValue &baseValue, int nOfWTLevels) // derived quantization
    {
        m_triplets.ReAlloc(nOfWTLevels);

        SetLxLy(baseValue);

        int mantissa = baseValue.Mant();
        int exponent = baseValue.Expn();

        for (int sbDepth = nOfWTLevels - 1; sbDepth >= 0; sbDepth--)
        {
            SetHxLy(QuantValue(mantissa, exponent), sbDepth);
            SetLxHy(QuantValue(mantissa, exponent), sbDepth);
            SetHxHy(QuantValue(mantissa, exponent), sbDepth);

            exponent--;
        }
    }

    void Free()
    {
        m_triplets.Free();
        m_LxLy = QuantValue();
    }

    void SetLxLy(const QuantValue &LxLy)              { m_LxLy = LxLy; }
    void SetHxLy(const QuantValue &HxLy, int sbDepth) { m_triplets[sbDepth].SetHxLy(HxLy); }
    void SetLxHy(const QuantValue &LxHy, int sbDepth) { m_triplets[sbDepth].SetLxHy(LxHy); }
    void SetHxHy(const QuantValue &HxHy, int sbDepth) { m_triplets[sbDepth].SetHxHy(HxHy); }

    const QuantValue &LxLy()            const { return m_LxLy; }
    const QuantValue &HxLy(int sbDepth) const { return m_triplets[sbDepth].HxLy(); }
    const QuantValue &LxHy(int sbDepth) const { return m_triplets[sbDepth].LxHy(); }
    const QuantValue &HxHy(int sbDepth) const { return m_triplets[sbDepth].HxHy(); }

/*
    int   NOfWTLevels() const { return m_triplets.Size(); }
*/

protected:
    QuantValue                m_LxLy;
    FixedBuffer<QuantTriplet> m_triplets;
};

#endif // __QUANT_H__
