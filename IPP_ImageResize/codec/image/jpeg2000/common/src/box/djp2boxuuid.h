/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __DJP2BOXUUID_H__
#define __DJP2BOXUUID_H__

#include "djp2box.h"

#include "bdiagnoutput.h"


class DJP2BoxUUID
{
public:
    DJP2BoxUUID() { }

    void AttachDiagnOutput(BDiagnOutput     &diagnOutput) { m_diagnOutputPtr =  diagnOutput; }

    template<class ByteInput>
        void Read(ByteInput &stream)
    {

        static const Ipp8u ID_XMP_EXIF[] = { 0xBE, 0x7A, 0xCF, 0xCB, 0x97, 0xA9, 0x42, 0xE8, 0x9C, 0x71, 0x99, 0x94, 0x91, 0xE3, 0xAF, 0xAC };

        for(unsigned int i = 0; i < 16; i++)
        {
            if(stream.Read8u() != ID_XMP_EXIF[i])
            {
                m_diagnOutputPtr->Warning(DiagnDescrCT<DJP2BoxWarning, UnknownUUID>());
                return;
            }
        }

        m_xmp_exif_data.ReAlloc(stream.TailSize());
        stream.Read(m_xmp_exif_data, stream.TailSize());
    }

    const FixedArray<Ipp8u> &XMPEXIFData() const { return m_xmp_exif_data; }

protected:
    FixedArray<Ipp8u> m_xmp_exif_data;
    BDiagnOutputPtr   m_diagnOutputPtr;
};



#endif // __DJP2BOXUUID_H__
