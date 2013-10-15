
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

/*
//  encoder code-block pass structure
//
//
//
*/

#ifndef __ECBPASS_H__
#define __ECBPASS_H__

class ECBPass
{
public:
    ECBPass() : m_start(0), m_end(0), m_dist(0.0), m_rdSlope(-1.0), m_layer(0), m_isTerm(false)
    {}

    template<class ByteOutput>
        void Store(ByteOutput &dst) { dst.Write(m_data, m_end - m_start); }

    Ipp8u     *m_data;
    int        m_start;
    int        m_end;
    double     m_dist;
    double     m_rdSlope;
    int        m_layer;
    bool       m_isTerm;
};

#endif // __ECBPASS_H__

