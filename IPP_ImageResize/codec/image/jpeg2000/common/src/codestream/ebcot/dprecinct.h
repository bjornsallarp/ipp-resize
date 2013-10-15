
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

#ifndef __DPRECINCT_H__
#define __DPRECINCT_H__
#include "dcodeblock.h"
#include "tagtreedecoder.h"
#include "rectgrid.h"

#include "bufferstream.h"

class DPrecinct
{
public:
    DPrecinct() {}

    void Init(
        const ImageCore32sC1 &precinct,
        const Rect           &rect,
        const RectSize       &cbStepOrder,
        unsigned int          nOfLayers,
        IppiWTSubband         subband,
        unsigned int          cmpGuardBits,
        unsigned int          sbDynRange,
        unsigned int          sbLowestBitOffset,
        const CBCodingStyle  &codingStyle);

    template<class ByteInput>
        void ReadPacketBody  (unsigned int layer, ByteInput &packetBody)
    {

#ifdef _OPENMP

        int cbIndx;

        int nOfCbs = m_cbs.Size();

        for(cbIndx = 0; cbIndx < nOfCbs; cbIndx++)
        {
            m_cbBody[cbIndx].Clear();
            Transfer(packetBody, m_cbBody[cbIndx], m_cbs[cbIndx].LayerSize(layer));
        }
/*
#pragma  omp parallel for
        for(cbIndx = 0; cbIndx < nOfCbs; cbIndx++)
            m_cbs[cbIndx].DecodeLayer(layer, m_cbBody[cbIndx]);
*/

        cbIndx = 0;
#pragma omp parallel shared(cbIndx)
        {
        int cbIndxThr;
loop_start:
#pragma omp critical
        {
        cbIndxThr = cbIndx;
        cbIndx++;
        }
        if(! (cbIndxThr < nOfCbs) ) goto loop_end;
        m_cbs[cbIndxThr].DecodeLayer(layer, m_cbBody[cbIndxThr]);
        goto loop_start;
loop_end:
                ;
        }

#else  // _OPENMP

        for(unsigned int cbIndx = 0; cbIndx < m_cbs.Size(); cbIndx++)
            m_cbs[cbIndx].DecodeLayer(layer, packetBody);

#endif // _OPENMP

    }

    template<class ByteInput>
        void ReadPacketHeader(unsigned int layer, PHBitIn<ByteInput> &phBitIn)
    {
        for(unsigned int cbIndx = 0; cbIndx < m_cbs.Size(); cbIndx++)
        {
            DCodeBlock &cb = m_cbs[cbIndx];

            bool included;

            if (!cb.NOfPasses())
                included = m_inclTagTree.Decode(phBitIn, cbIndx, layer + 1);
            else
                included = phBitIn.Read();

            if (!included) continue;

            if (!cb.NOfPasses())
            {
                unsigned int zbpl = 1; for (;;)
                {
                    if (m_zbplTagTree.Decode(phBitIn, cbIndx, zbpl)) break;
                    zbpl++;
                }
                cb.SetNOfZeroBits(zbpl - 1);
            }

            unsigned int layerNOfPasses = ReadNOfPasses(phBitIn);

            if (layerNOfPasses > 0) cb.AddLblock(ReadCommaCode(phBitIn));

            unsigned int pass = cb.NOfPasses();
            unsigned int nOfRemainingPasses = layerNOfPasses;

            while (nOfRemainingPasses > 0)
            {
                unsigned int segmentNOfPasses = Min(nOfRemainingPasses,
                    SegmentMaxNOfPasses(pass, m_isSelectiveMQBypass, m_isTermOnEveryPass));

                unsigned int segmentLength = phBitIn.Read(cb.Lblock() + Log2(segmentNOfPasses));

                cb.SetNextSegmentSize(layer, segmentLength, segmentNOfPasses);

                pass               += segmentNOfPasses;
                nOfRemainingPasses -= segmentNOfPasses;
            }
        }
    }

protected:
    FixedArray<DCodeBlock> m_cbs;

#ifdef _OPENMP
    FixedBuffer<BufferStream> m_cbBody;
#endif

    TagTreeDecoder         m_inclTagTree;
    TagTreeDecoder         m_zbplTagTree;

    // they are stored in Init() only for number of coding pass calculations in ReadPacketHeader
    bool                   m_isSelectiveMQBypass;
    bool                   m_isTermOnEveryPass;

    static unsigned int SegmentMaxNOfPasses(unsigned int startPass, bool isSelectiveMQBypass, bool isTermOnEveryPass);

    template<class ByteInput>
        static unsigned int ReadNOfPasses(PHBitIn<ByteInput> &phBitIn)
    {
        if(!phBitIn.Read()) return 1;
        if(!phBitIn.Read()) return 2;

        int n = phBitIn.Read(2);

        if (n != 3) return n + 3;

        n = phBitIn.Read(5);

        if(n != 31) return n + 6;

        return phBitIn.Read(7) + 37;
    }

    template<class ByteInput>
        static unsigned int ReadCommaCode(PHBitIn<ByteInput> &phBitIn)
    {
        unsigned int i = 0; for (;;)
        {
            if (!phBitIn.Read())  break;
            i++;
        }

        return i;
    }
};

#endif // __DPRECINCT_H__
