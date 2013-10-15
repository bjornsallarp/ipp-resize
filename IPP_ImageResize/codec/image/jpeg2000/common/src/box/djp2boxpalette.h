
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

#ifndef __DJP2BOXPALETTE_H__
#define __DJP2BOXPALETTE_H__

#include "fixedbuffer.h"
#include "jp2bitspercomp.h"
#include "djp2boxwarning.h"
#include "bdiagnoutput.h"

class DJP2BoxPalette
{
public:
    DJP2BoxPalette() { Reset(); }

    void Reset()
    {
        m_isEmpty = true;
        m_nOfEntries = 0;
        m_nOfColumns = 0;
        m_data.Free();
        m_bitsPerComp.Free();
    }

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    template<class ByteInput> void Read(ByteInput &stream)
    {
        if(!m_isEmpty)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ExtraPaletteBox>());

        m_nOfEntries = stream.Read16u();

        if(!m_nOfEntries)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxPalleteZeroNOfEntries>());
        if(m_nOfEntries > 1024)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxPalleteNOfEntriesExceedBounds>());

        m_nOfColumns = stream.Read8u();

        m_bitsPerComp.ReAlloc(m_nOfColumns);
        for(unsigned int column = 0; column < m_nOfColumns; column++)
        {
            m_bitsPerComp[column] = stream.Read8u();

            if(!m_bitsPerComp[column].IsConformant())
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxPalleteBitDepthExceedBounds>());
        }

        m_data.ReAlloc(m_nOfColumns, m_nOfEntries);

        for(unsigned int entry = 0; entry < m_nOfEntries; entry++)
        {
            for(unsigned int column = 0; column < m_nOfColumns; column++)
            {
                unsigned int bitDepth = m_bitsPerComp[column].BitDepth();
                bool         isSigned = m_bitsPerComp[column].IsSigned();

                Ipp64u value = 0;

                int maxBitShift = (bitDepth >> 3) << 3;

                for(int bitShift = maxBitShift; bitShift >= 0; bitShift-=8)
                {
                    if(bitShift < 64)
                        value |= ((Ipp64u)stream.Read8u() << (Ipp64u)bitShift);
                    else // only less then 38 bit depth supported by standard, above we even try to do more
                        stream.Read8u(); // so dummy reading
                }

                if(isSigned)
                {
                    Ipp64u signPos  = (Ipp64u)1 << (Ipp64u)bitDepth;
                    Ipp64u sign = value & signPos << (Ipp64u)(63 - bitDepth);
                    value = value ^ (~signPos); // clear sign in 'wrong' place
                    value = value | sign;       // place sign in 'right' place
                }
                m_data[column][entry] = value;
            }
        }

        m_isEmpty = false;
    }

    // NOTE: only for single-component data column is component index
    // the value of column could be obtained from component mapping box (DJP2BoxCompMapping)
    const Ipp64s         *Entry      (Ipp8u column) const { return m_data[column]; }
    const JP2BitsPerComp *BitsPerComp()             const { return m_bitsPerComp; }
    Ipp16u                NOfEntries()              const { return m_nOfEntries; }
    Ipp8u                 NOfColumns()              const { return m_nOfColumns; }
    bool                  IsEmpty   ()              const { return m_isEmpty; }

protected:
    bool                        m_isEmpty;
    Ipp16u                      m_nOfEntries;
    Ipp8u                       m_nOfColumns;
    FixedBuffer2D<Ipp64s>       m_data;
    FixedBuffer<JP2BitsPerComp> m_bitsPerComp;
    BDiagnOutputPtr             m_diagnOutputPtr;
};


#endif // __DJP2BOXPALETTE_H__
