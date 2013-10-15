
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

#ifndef __DJP2QUANTCOMP_H__
#define __DJP2QUANTCOMP_H__

#include "quant.h"
#include "optionalarray.h"
#include "djp2cswarning.h"
#include "jp2codingstylecomp.h"
#include "ippdefs.h"
#include "bdiagnoutput.h"

class DJP2QuantComp
{
public:
    DJP2QuantComp()
    : m_mode(JP2V_QUANT_NO)
    , m_guardBits(0)
    , m_nOfWTLevels(0)
    , m_isEmpty(true)
    {}

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    void Reset();
    // should be called after reading but before actual using
    void Conform(unsigned int nOfWTLevels);

    const QuantComponent &Quant()     const { return m_quant; }
    int                   GuardBits() const { return m_guardBits; }

    bool IsEmpty() const { return m_isEmpty; }

    template<class ByteInput>
        void Read(ByteInput &body)
    {
        m_mode = ReadQuantMode(body, m_guardBits, m_diagnOutputPtr);

        switch(m_mode)
        {
            case JP2V_QUANT_DERIVED:   ReadDerived  (body); break;
            case JP2V_QUANT_EXPOUNDED: ReadExpounded(body); break;
            default:                   ReadNoQuant  (body);
        }

        m_isEmpty = false;
    }


protected:
    template<class ByteInput>
        void ReadDerived  (ByteInput &body)
    {
        m_mode = JP2V_QUANT_DERIVED;
        m_derivedBaseValue = ReadQuantValue(body);
    }

    template<class ByteInput>
        void ReadExpounded(ByteInput &body)
    {
        m_mode = JP2V_QUANT_EXPOUNDED;

        m_nOfWTLevels = (body.TailSize() - 2) / 6;
        m_quant.ReAlloc(m_nOfWTLevels);

        m_quant.SetLxLy(ReadQuantValue(body));
        for(unsigned int i = m_nOfWTLevels; i > 0; )
        {
            i--;
            m_quant.SetHxLy(ReadQuantValue(body), i);
            m_quant.SetLxHy(ReadQuantValue(body), i);
            m_quant.SetHxHy(ReadQuantValue(body), i);
        }
    }

    template<class ByteInput>
        void ReadNoQuant  (ByteInput &body)
    {
        m_mode = JP2V_QUANT_NO;

        m_nOfWTLevels = (body.TailSize() - 1) / 3;
        m_quant.ReAlloc(m_nOfWTLevels);

        m_quant.SetLxLy(ReadNoQuantValue(body));
        for(unsigned int i = m_nOfWTLevels; i > 0; )
        {
            i--;
            m_quant.SetHxLy(ReadNoQuantValue(body), i);
            m_quant.SetLxHy(ReadNoQuantValue(body), i);
            m_quant.SetHxHy(ReadNoQuantValue(body), i);
        }
    }

    template<class ByteInput>
        static JP2QuantMode ReadQuantMode(ByteInput &stream, unsigned int &guardBits, BDiagnOutputPtr &diagnOutputPtr)
    {
        Ipp8u qfield = stream.Read8u();
        Ipp8u style = (Ipp8u)(qfield & JP2M_QUANT_TYPE);

        JP2QuantMode mode;

        switch(style)
        {
        case JP2V_QUANT_NO:
        case JP2V_QUANT_DERIVED:
        case JP2V_QUANT_EXPOUNDED:
            mode = (JP2QuantMode)style;
            break;
        default:
            diagnOutputPtr->Warning(DiagnDescrCT<DJP2CSWarning, UnknownQuantStyle>());
            mode = JP2V_QUANT_NO;
        }

        guardBits = (qfield & JP2M_GUARD_BITS) >> JP2S_GUARD_BITS;

        return mode;
    }

    template<class ByteInput>
        static QuantValue ReadQuantValue(ByteInput &stream)
    {
        Ipp16u qfield = stream.Read16u();

        return QuantValue(qfield & JP2M_QUANT_MANTISSA, qfield >> JP2S_QUANT_EXP);
    }

    template<class ByteInput>
        static QuantValue ReadNoQuantValue(ByteInput &stream)
    {
        Ipp16u qfield = stream.Read8u();

        return QuantValue(0, qfield >> JP2S_NOQUANT_EXP);
    }

    BDiagnOutputPtr m_diagnOutputPtr;
    JP2QuantMode    m_mode;
    QuantComponent  m_quant;
    QuantValue      m_derivedBaseValue;
    unsigned int    m_guardBits;
    unsigned int    m_nOfWTLevels;
    bool            m_isEmpty;
};

class DJP2QuantCompTile : public OptionalSuperArray<DJP2QuantComp>
{
public:
    DJP2QuantCompTile() {}

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    void ReAlloc(const OptionalArray<DJP2QuantComp> &parent)
    {
        OptionalSuperArray<DJP2QuantComp>::ReAlloc(parent);

        for(unsigned int i = 0; i < Size(); i++)
            m_value[i].operator DJP2QuantComp&().AttachDiagnOutput(m_diagnOutputPtr);
    }

    void Conform(const JP2CodingStyleCompTile &codingStyle)
    {
        for(unsigned int component = 0; component < codingStyle.Size(); component++)
        {
            // derived quantization may be tuned for each tile-part header
            // expounded is only checked for appropriate amount of values

            DJP2QuantComp &compValue = m_value[component].operator DJP2QuantComp&();
            compValue.Conform(codingStyle[component].NOfWTLevels());
        }
    }
protected:
    BDiagnOutputPtr m_diagnOutputPtr;
};

typedef OptionalArray<DJP2QuantComp>      DJP2QuantCompMain;

#endif // __DJP2QUANTCOMP_H__
