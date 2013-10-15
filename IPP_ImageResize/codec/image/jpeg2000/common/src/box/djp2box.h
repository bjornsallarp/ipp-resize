
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

#ifndef __DJP2BOX_H__
#define __DJP2BOX_H__

#include "jp2box.h"
#include "diagndescr.h"
#include "djp2boxexception.h"

inline JP2BoxType BoxType(Ipp32u TBox)
{
    switch(TBox)
    {
    case jp__:  return jp__;
    case ftyp:  return ftyp;
    case jp2h:  return jp2h;
    case ihdr:  return ihdr;
    case bpcc:  return bpcc;
    case colr:  return colr;
    case pclr:  return pclr;
    case cmap:  return cmap;
    case cdef:  return cdef;
    case res_:  return res_;
    case resc:  return resc;
    case resd:  return resd;
    case jp2c:  return jp2c;
    case jp2i:  return jp2i;
    case xml_:  return xml_;
    case uuid:  return uuid;
    case uinf:  return uinf;
    case ulst:  return ulst;
    case url_:  return url_;
    default:    return unkn;
    }
}

template<class ByteInput>
void ReadBoxHeader(ByteInput& stream, JP2BoxType &boxType, unsigned int &bodySize)
{
    Ipp32u LBox = stream.Read32u();

    if (LBox >= 2 && LBox <= 7) //  reserved for ISO use
        throw DiagnDescrCT<DJP2BoxException,badLBoxValue>();

    boxType = BoxType(stream.Read32u());

    if(LBox == 0)
    {
        bodySize = stream.TailSize();
    }
    else if (LBox == 1)
    {
        Ipp64u XLBox = stream.Read64u();

        if (XLBox < 16)
            throw DiagnDescrCT<DJP2BoxException,badXLBoxValue>();

        Ipp64u bodySize64 = XLBox - (Ipp64u)16;

        if(bodySize64 > IPP_MAX_32U)
            throw DiagnDescrCT<DJP2BoxException,tooLongBoxLen>();

        bodySize = (unsigned int)bodySize64;
    }
    else
    {
        bodySize = LBox - 8;
    }
}

#endif // __DJP2BOX_H__

