/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2011-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_JP2_ENC_H__
#define __UIC_JP2_ENC_H__

#include "uic_base_image_encoder.h"

namespace UIC {

class OwnJP2Encoder;

class UICAPI JP2Encoder : public BaseImageEncoder
{
public:
    JP2Encoder();
    virtual ~JP2Encoder();

    virtual ExcStatus           Init          ();
    virtual ExcStatus           Close         ();

    virtual const CodecVersion& GetVersion    ();

    virtual Ipp32u              SetNOfThreads (Ipp32u nOfThreads);
    virtual Ipp32u              NOfThreads    ();

    virtual void                AttachDiagnOut(BaseStreamDiagn &diagnOutput);
    virtual void                DetachDiagnOut();

    virtual ExcStatus           AttachStream  (BaseStreamOutput &stream);
    virtual ExcStatus           DetachStream  ();

    virtual ExcStatus           AttachImage   (const Image      &image);
    virtual ExcStatus           DetachImage   ();

    ExcStatus                   SetParams(
        unsigned int maxNOfWTLevels,
        bool         isUseMCT,
        bool         isWT53Used,
        bool         isDerivedQuant,
        int          derivedQuantBaseValueMantissa,
        int          derivedQuantBaseValueExponent,
        unsigned int dstSize);

    virtual ExcStatus           WriteHeader   ();

    virtual ExcStatus           WriteData     ();

    // error & warning codes that can be placed to BaseStreamDiagn
    enum {

    Except_writeError

    };
protected:
    OwnJP2Encoder *m_encoder;
};

}

#endif // __UIC_JPEG2000_ENC_H__
