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

#ifndef __DCODEBLOCK_H__
#define __DCODEBLOCK_H__

#include "fixedarray.h"
#include "bcodeblock.h"
#include "cbcodingstyle.h"
#include "dcblayer.h"
#include "debcotwarning.h"

#include "bdiagnoutput.h"
#include "bufferstream.h"

class DCodeBlock : public BCodeBlock
{
public:
    DCodeBlock();

    void Init(
        const ImageCore32sC1 &cbImage,
        const RectSize       &size,
        unsigned int          nOfLayers,
        IppiWTSubband         subband,
        unsigned int          cmpGuardBits,
        unsigned int          sbDynRange,
        unsigned int          sbLowestBitOffset,
        const CBCodingStyle  &codingStyle);

    void         AttachDiagnOutput (BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    void         SetNextSegmentSize(unsigned int layer, unsigned int size, unsigned int nOfPasses);
    void         SetNOfZeroBits    (unsigned int nOfZeroBits);

    void         AddLblock         (unsigned int value)        { m_Lblock += value; }
    unsigned int Lblock            ()                   const  { return m_Lblock; }

    void         SaveComprData     (Ipp8u *dst)         const;

    unsigned int LayerSize         (unsigned int layer) const  { return m_layer[layer].Size(); }

    template<class ByteInput>
        void DecodeLayer(unsigned int layer, ByteInput &data)
    {
        if(m_codingStyle.IsTerminateOnEveryPass() || m_layer.Size() == 1)
        {
            FixedBuffer<Ipp8u> segmData;

            ippiDecodeCBProgrSetPassCounter_JPEG2K(m_layer[layer].NOfPasses(), State());

            for(List<DCBSegment>::Iterator
                segment  = m_layer[layer].Segment().ItrFront();
                segment != m_layer[layer].Segment().ItrBackBound();
                ++segment)
            {
                unsigned int segmentSize = segment->Size();
                segmData.ReAlloc(segmentSize+1);
                data.Read(segmData, segmentSize);
                if(ippStsJPEG2KDamagedCodeBlock ==
                    ippiDecodeCBProgrStep_JPEG2K(segmData, segmentSize, State()) )
                {
                    m_diagnOutputPtr->Warning(DiagnDescrCT<DEBCOTWarning, CodeBlockLayerDamage>());
                }
            }

            int nOfResidualPasses;
            ippiDecodeCBProgrGetPassCounter_JPEG2K(State(), &nOfResidualPasses);
            if(nOfResidualPasses!=0)
                m_diagnOutputPtr->Warning(DiagnDescrCT<DEBCOTWarning, CodeBlockLayerNOfPassesMissmatch>());
        }
        else if(m_codingStyle.IsSelectiveMQBypass())
        {
            for(List<DCBSegment>::Iterator
                segment  = m_layer[layer].Segment().ItrFront();
                segment != m_layer[layer].Segment().ItrBackBound();
                ++segment)
            {
                // accumulate data
                Transfer(data, m_blockData, segment->Size());


                m_currPass += segment->NOfPasses();
                m_sectNOfPasses += segment->NOfPasses();

                if( (segment->NOfPasses()) && IsTerminatedInMQBypass(m_currPass)) // it's possible to make progression step
                {
                    m_lastPassDone = m_currPass;

                    int size = m_blockData.Size();

                    FixedBuffer<Ipp8u> segmData;

                    segmData.ReAlloc(size+1);
                    m_blockData.Read(segmData, size);

                    ippiDecodeCBProgrSetPassCounter_JPEG2K(1024, State());
                    m_sectNOfPasses = 0;


                    if(ippStsJPEG2KDamagedCodeBlock ==
                        ippiDecodeCBProgrStep_JPEG2K(segmData, size, State()) )
                    {
                        m_diagnOutputPtr->Warning(DiagnDescrCT<DEBCOTWarning, CodeBlockLayerDamage>());
                    }
               }
            }

            int size = m_blockData.Size();

            if( (layer + 1 >= m_layer.Size()) && size) // last layer
            {

                FixedBuffer<Ipp8u> segmData;

                segmData.ReAlloc(size+1);
                m_blockData.Read(segmData, size);

                ippiDecodeCBProgrSetPassCounter_JPEG2K(m_sectNOfPasses, State());


                if(ippStsJPEG2KDamagedCodeBlock ==
                    ippiDecodeCBProgrStep_JPEG2K(segmData, size, State()) )
                {
                    m_diagnOutputPtr->Warning(DiagnDescrCT<DEBCOTWarning, CodeBlockLayerDamage>());
                }

                int nOfResidualPasses;
                ippiDecodeCBProgrGetPassCounter_JPEG2K(State(), &nOfResidualPasses);
                if(nOfResidualPasses!=0)
                    m_diagnOutputPtr->Warning(DiagnDescrCT<DEBCOTWarning, CodeBlockLayerNOfPassesMissmatch>());
           }
        }
        else
        {
            // accumulate data
            Transfer(data, m_blockData, m_layer[layer].Size());

            if(layer + 1 >= m_layer.Size()) // last layer
            {
                int size = m_blockData.Size();

                FixedBuffer<Ipp8u> segmData;

                segmData.ReAlloc(size+1);
                m_blockData.Read(segmData, size);

                ippiDecodeCBProgrSetPassCounter_JPEG2K(m_nOfPasses, State());


                if(ippStsJPEG2KDamagedCodeBlock ==
                    ippiDecodeCBProgrStep_JPEG2K(segmData, size, State()) )
                {
                    m_diagnOutputPtr->Warning(DiagnDescrCT<DEBCOTWarning, CodeBlockLayerDamage>());
                }

                int nOfResidualPasses;
                ippiDecodeCBProgrGetPassCounter_JPEG2K(State(), &nOfResidualPasses);
                if(nOfResidualPasses!=0)
                    m_diagnOutputPtr->Warning(DiagnDescrCT<DEBCOTWarning, CodeBlockLayerNOfPassesMissmatch>());
           }
        }
    }

    static bool IsTerminatedInMQBypass(unsigned int nOfPasses)
    {
        if(nOfPasses < 10)     return false; // initial arithmetic coding can not be terminated
        if(nOfPasses % 3 == 2) return false; // significance propagation can not be terminated
        return true;
    }

protected:
    IppiDecodeCBProgrState_JPEG2K *State() { return (IppiDecodeCBProgrState_JPEG2K*)(Ipp8u*)m_state; }

    BDiagnOutputPtr       m_diagnOutputPtr;

    FixedBuffer<Ipp8u>    m_state;
    FixedArray<DCBLayer>  m_layer;

    unsigned int          m_Lblock;

    ImageCore32sC1        m_cbImage;
    RectSize              m_size;
    IppiWTSubband         m_subband;
    unsigned int          m_cmpGuardBits;
    unsigned int          m_sbDynRange;
    unsigned int          m_sbLowestBitOffset;
    CBCodingStyle         m_codingStyle;

    BufferStream          m_blockData;
    unsigned int          m_currPass;
    unsigned int          m_sectNOfPasses;

    unsigned int          m_lastPassDone;
    bool done;
};

#endif // __DCODEBLOCK_H__
