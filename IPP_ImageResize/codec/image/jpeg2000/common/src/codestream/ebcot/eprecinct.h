
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
//  EPrecinct class intended to store parameters of encoded precinct.
//
//
//
*/

#ifndef __EPRECINCT_H__
#define __EPRECINCT_H__

#include "ecodeblock.h"
#include "tagtreeencoder.h"
#include "rectgrid.h"
#include "cbencoder.h"

class EPrecinct
{
public:
    EPrecinct() {}

    void Init(const Rect &rect, int cbXStepOrder, int cbYStepOrder);

    void Encode(
              CBEncoder      *cbEncoder,
        const ImageCore32sC1 &precinct,
              IppiWTSubband   subband,
              int             sbDynRange,
              int             sbLowestBitOffset
    );

    void SetCurPassToFirst();

    void ScaleDist(double factor);
    void CalcRDSlopes();
    void RDSlopesMinMax(double &min, double &max)  const;
    void AssignLayerTillCurPass(int layer, double threshold);
    void AssignLayerTillCurPass(int layer);

    void StoreEncodingState();
    void RestoreEncodingState();

    template<class ByteOutput, class BitOutput>
        void WriteToPacket(int layer, BitOutput &phBitOut, ByteOutput &packetBody)
    {
        SetTagTrees(layer);

        for (int cbIndx = 0; cbIndx < NOfCBs(); cbIndx++)
        {
            ECodeBlock &cb = m_cbs[cbIndx];

            bool included = cb.IsIncluded(layer);

            if (cb.IsFirstInclusion())
            {
                m_inclTagTree.Encode(cbIndx, layer + 1, phBitOut);
            }
            else
            {
                phBitOut.Write(included);
            }

            if (!included) continue;

            if (cb.IsFirstInclusion())
            {
                for (int zbpl = 1;; zbpl++)
                {
                    if (m_zbplTagTree.Encode(cbIndx, zbpl, phBitOut))
                        break;
                }
            }

            int layerNOfPasses;

            ECBPassIterator layerFirstPass     = cb.CurPass();
            ECBPassIterator layerBoundOfPasses = FindLayerBoundOfPasses(cb, layer, layerNOfPasses);

            WriteNOfPasses(layerNOfPasses, phBitOut);

            int LblockAdjustment = FindLblockAdjustment(layerFirstPass, layerBoundOfPasses, cb.Lblock());

            WriteCommaCode(LblockAdjustment, phBitOut);

            cb.AdjustLblock(LblockAdjustment);

            WriteLengthsOfLayerSegments(layerFirstPass, layerBoundOfPasses, cb.Lblock(), phBitOut);

            WriteLayerData(layerFirstPass, layerBoundOfPasses, packetBody);

            cb.SetCurPass(layerBoundOfPasses);

            if(layerFirstPass != layerBoundOfPasses) cb.SetAlreadyIncluded();
        }
    }

protected:
    GridPow2                m_cbGrid;
    FixedBuffer<ECodeBlock> m_cbs;

#ifdef _OPENMP
    FixedBuffer<Rect>       m_cbRect;
#endif


    TagTreeEncoder m_inclTagTree;
    TagTreeEncoder m_zbplTagTree;

    TagTreeEncoder m_inclTagTreeStore;
    TagTreeEncoder m_zbplTagTreeStore;

    int NOfCBs() const { return m_cbGrid.NOfCells(); }
    void SetTagTrees(int layer);

    template<class BitOutput>
        void WriteNOfPasses(int nOfPasses, BitOutput &bitOut)
    {
        if     (nOfPasses ==  1) bitOut.Write(0x00, 1);
        else if(nOfPasses ==  2) bitOut.Write(0x02, 2);
        else if(nOfPasses ==  3) bitOut.Write(0x0C, 4);
        else if(nOfPasses ==  4) bitOut.Write(0x0D, 4);
        else if(nOfPasses ==  5) bitOut.Write(0x0E, 4);
        else if(nOfPasses <= 36) bitOut.Write(( 15 << 5) | (nOfPasses -  6),  9);
        else                     bitOut.Write((511 << 7) | (nOfPasses - 37), 16);
    }

    template<class BitOutput>
        void WriteCommaCode(int commaCode, BitOutput &bitOut)
    {
        for(int i = 0; i < commaCode; i++)
            bitOut.Write1();
        bitOut.Write0();
    }

    template<class ByteOutput>
        void WriteLayerData(const ECBPassIterator &first, const ECBPassIterator &bound, ByteOutput &dst)
    {
        for (ECBPassIterator pass = first; pass != bound; ++pass)
            pass->Store(dst);
    }

    template<class BitOutput>
        void WriteLengthsOfLayerSegments(const ECBPassIterator &first, const ECBPassIterator &bound, int Lblock, BitOutput &bitOut)
    {
        ECBPassIterator lastPass = bound;
        --lastPass;

        int n = first->m_start;
        int passcount = 1;

        for (ECBPassIterator pass = first; pass != bound; ++pass)
        {
            if (pass->m_isTerm || pass == lastPass)
            {
                int datalen = pass->m_end - n;

                bitOut.Write(datalen, Lblock + Log2(passcount));

                n += datalen;
                passcount = 1;
            }
            else passcount++;
        }
    }


    static int FindLblockAdjustment(const ECBPassIterator &first, const ECBPassIterator &bound, int Lblock)
    {
        ECBPassIterator lastPass = bound;
        --lastPass;

        int n = first->m_start;

        int passcount     = 1;
        int maxAdjustment = 0;

        for (ECBPassIterator pass = first; pass != bound; ++pass)
        {
            if (pass->m_isTerm || pass == lastPass)
            {
                int datalen = pass->m_end - n;

                int adjustment = MSBitPos(datalen) - (Lblock + Log2(passcount));

                if(adjustment > maxAdjustment) maxAdjustment = adjustment;

                n += datalen;
                passcount = 1;
            }
            else passcount++;
        }

        return maxAdjustment;
    }

    static ECBPassIterator FindLayerBoundOfPasses(const ECodeBlock &cb, int layer, int &nOfPasses)
    {
        const ECBPassIterator &first = cb.CurPass();

        ECBPassIterator bound = first;

        nOfPasses = 0;

        while (bound != cb.BoundOfPasses() && bound->m_layer == layer)
        {
            ++bound;
            nOfPasses++;
        }

        return bound;
    }

};

#endif // __EPRECINCT_H__
