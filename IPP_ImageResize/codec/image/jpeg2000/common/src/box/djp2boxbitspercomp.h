
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

#ifndef __DJP2BOXBITSPERCOMP_H__
#define __DJP2BOXBITSPERCOMP_H__

#include "fixedbuffer.h"

#include "jp2bitspercomp.h"
#include "djp2boxwarning.h"
#include "bdiagnoutput.h"

class DJP2BoxBitsPerComp
{
public:
    DJP2BoxBitsPerComp() { Reset(); }

    void Reset()
    {
        m_isEmpty = true;
        m_bitsPerComp.Free();
    }

    void AttachDiagnOutput(BDiagnOutput &diagnOutput) { m_diagnOutputPtr = diagnOutput; }

    template<class ByteInput>
        void Read(ByteInput &stream, unsigned int nOfComponents)
    {
        if(!m_isEmpty)
            m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, ExtraBitsPerCompBox>());

        m_bitsPerComp.ReAlloc(nOfComponents);

        for(unsigned int component = 0; component < nOfComponents; component++)
        {
            m_bitsPerComp[component] = stream.Read8u();

            if(!m_bitsPerComp[component].IsConformant())
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, BoxBitsPerCompBitDepthExceedBounds>());
        }

        m_isEmpty = false;
    }

    bool IsEmpty() const { return m_isEmpty; }

    // NOTE: Actual bit depth is BitDepth + 1;
    const JP2BitsPerComp *BitsPerComp() const { return m_bitsPerComp; }

protected:
    FixedBuffer<JP2BitsPerComp> m_bitsPerComp;
    bool                        m_isEmpty;
    BDiagnOutputPtr             m_diagnOutputPtr;
};

#endif // __DJP2BOXBITSPERCOMP_H__
