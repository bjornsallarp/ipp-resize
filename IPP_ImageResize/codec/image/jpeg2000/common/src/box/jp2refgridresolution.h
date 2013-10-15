
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __JP2REFGRIDRESOLUTION_H__
#define __JP2REFGRIDRESOLUTION_H__

class JP2RefGridResolution
{
public:
    JP2RefGridResolution()
    : m_verNumerator(0)
    , m_verDenominator(1)
    , m_horNumerator(0)
    , m_horDenominator(1)
    , m_verExponent(0)
    , m_horExponent(0)
    {}

    void SetVerNumerator  (Ipp16u verNumerator  ) { m_verNumerator   = verNumerator;   }
    void SetVerDenominator(Ipp16u verDenominator) { m_verDenominator = verDenominator; }
    void SetHorNumerator  (Ipp16u horNumerator  ) { m_horNumerator   = horNumerator;   }
    void SetHorDenominator(Ipp16u horDenominator) { m_horDenominator = horDenominator; }
    void SetVerExponent   (Ipp8u  verExponent   ) { m_verExponent    = verExponent;    }
    void SetHorExponent   (Ipp8u  horExponent   ) { m_horExponent    = horExponent;    }

    Ipp16u VerNumerator  () const { return m_verNumerator;   }
    Ipp16u VerDenominator() const { return m_verDenominator; }
    Ipp16u HorNumerator  () const { return m_horNumerator;   }
    Ipp16u HorDenominator() const { return m_horDenominator; }
    Ipp8u  VerExponent   () const { return m_verExponent;    }
    Ipp8u  HorExponent   () const { return m_horExponent;    }

protected:
    Ipp16u m_verNumerator;
    Ipp16u m_verDenominator;
    Ipp16u m_horNumerator;
    Ipp16u m_horDenominator;
    Ipp8s  m_verExponent;
    Ipp8s  m_horExponent;
};

template<class ByteInput> void Read(ByteInput &stream, JP2RefGridResolution &resolution)
{
    resolution.SetVerNumerator  (stream.Read16u());
    resolution.SetVerDenominator(stream.Read16u());
    resolution.SetHorNumerator  (stream.Read16u());
    resolution.SetHorDenominator(stream.Read16u());
    resolution.SetVerExponent   (stream.Read8u ());
    resolution.SetHorExponent   (stream.Read8u ());
}


#endif // __JP2REFGRIDRESOLUTION_H__
