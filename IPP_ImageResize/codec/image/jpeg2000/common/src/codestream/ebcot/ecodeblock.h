
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
//  ECodeBlock class intended to store parameters of encoding codeblocks
//      needed between different stage of encoding. But in this implementation
///     it does not include any data to store information about codeblock image rectangle.
//
//
//
*/

#ifndef __ECODEBLOCK_H__
#define __ECODEBLOCK_H__

#include "bcodeblock.h"
#include "list_.h"
#include "ecbpass.h"
#include "fixedbuffer.h"

typedef List<ECBPass>::Iterator ECBPassIterator;

class ECodeBlock : public BCodeBlock {
public:
    ECodeBlock();

    void SetNOfZeroBits(int nOfZeroBits);

    template<class ByteInput>
        void ReceiveComprData(ByteInput &src)
    {
        m_data.ReAlloc(src.Size());
        src.Read(m_data, src.Size());
    }

    void PushPass(int end, double dist);

    void ScaleDist(double factor);

    void CalcRDSlopes();

    void RDSlopesMinMax(double &min, double &max) const;

    void SetAlreadyIncluded()
    {
        m_isFirstInclusion = true;
    }

    bool IsIncluded(int layer) const
    {
        return NOfPasses() && m_curPass->m_layer == layer;
    }

    bool IsFirstInclusion() const
    {
        return m_isFirstInclusion;
    }

    int Lblock() const { return m_Lblock; }
    void AdjustLblock(int adjustment) { m_Lblock += adjustment; }


    const ECBPassIterator BoundOfPasses() const { return m_passes.ItrBackBound(); }
    const ECBPassIterator CurPass()       const { return m_curPass; }

    void SetCurPassToFirst()
    {
        m_curPass = m_passes.ItrFront();
    }

    void SetCurPass(ECBPassIterator &pass)
    {
        m_curPass = pass;
    }


    void StoreEncodingState()
    {
        m_curPassStore          = m_curPass;
        m_LblockStore           = m_Lblock;
        m_isFirstInclusionStore = m_isFirstInclusion;
    }

    void RestoreEncodingState()
    {
        m_curPass          = m_curPassStore;
        m_Lblock           = m_LblockStore;
        m_isFirstInclusion = m_isFirstInclusionStore;
    }

    void AssignLayerTillCurPass(int layer, double threshold);
    void AssignLayerTillCurPass(int layer);

protected:
    FixedBuffer<Ipp8u> m_data;
    List<ECBPass>      m_passes;

    ECBPassIterator    m_curPass;
    int                m_Lblock;
    bool               m_isFirstInclusion;

    ECBPassIterator    m_curPassStore;
    int                m_LblockStore;
    bool               m_isFirstInclusionStore;

    static const double m_BadRDSlope;
};


#endif // __ECODEBLOCK_H__

