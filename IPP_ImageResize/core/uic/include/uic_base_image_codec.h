/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_BASE_CODEC_H__
#define __UIC_BASE_CODEC_H__

#include "uic_image.h"
#include "uic_base_stream_diagn.h"

namespace UIC {

typedef struct
{
    int         major;      /* e.g. 1                               */
    int         minor;      /* e.g. 2                               */
    int         micro;      /* e.g. 3                               */
    int         build;      /* e.g. 10, always >= majorBuild        */
    const char* name;       /* e.g. "Intel(R) JPEG Decoder"         */
    const char* version;    /* e.g. "v1.2 Beta"                     */
    const char* buildDate;  /* e.g. "Jul 20 99"                     */

} CodecVersion;


class UICAPI BaseImageCodec
{
public:
    BaseImageCodec(void) {}
    virtual ~BaseImageCodec(void) {}

    virtual void                AttachDiagnOut(BaseStreamDiagn &diagnOutput) = 0;
    virtual void                DetachDiagnOut()                             = 0;

    virtual ExcStatus           Init          ()                             = 0;
    virtual ExcStatus           Close         ()                             = 0;

    virtual const CodecVersion& GetVersion    ()                             = 0;

    virtual Ipp32u              SetNOfThreads (Ipp32u nOfThreads)            = 0;
    virtual Ipp32u              NOfThreads    ()                             = 0;

    // Detach input or output stream
    virtual ExcStatus           DetachStream  ()                             = 0;

};

} // namespace UIC

#endif // __UIC_BASE_IMAGE_CODEC_H__

