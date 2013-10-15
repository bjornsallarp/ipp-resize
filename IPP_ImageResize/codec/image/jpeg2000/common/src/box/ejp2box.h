
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

#ifndef __EJP2BOX_H__
#define __EJP2BOX_H__

#include "jp2box.h"

template<class ByteOutput>
void WriteBoxHeader(JP2BoxType boxType, unsigned int bodySize, ByteOutput& stream)
{
    if ((Ipp64u)bodySize > (Ipp64u)IPP_MAX_32U - (Ipp64u)8)
    {
        stream.Write32u(1);
        stream.Write32u(boxType);

        Ipp64u boxSize = (Ipp64u)bodySize + (Ipp64u)16;
        stream.Write64u(boxSize);
    }
    else
    {
        Ipp32u boxSize = bodySize + 8;
        stream.Write32u(boxSize);

        stream.Write32u(boxType);
    }
}

template<class ByteOutput>
void WriteUknLenBoxHeader(JP2BoxType boxType, ByteOutput& stream)
{
    stream.Write32u(0);
    stream.Write32u(boxType);
}



#endif // __EJP2BOX_H__

