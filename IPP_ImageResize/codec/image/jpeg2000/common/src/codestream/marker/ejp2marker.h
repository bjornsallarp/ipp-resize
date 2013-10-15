
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

#ifndef __EJP2MARKER_H__
#define __EJP2MARKER_H__

#include "jp2marker.h"

template<class ByteOutput>
void WriteMarker(JP2Marker marker, ByteOutput& stream)
{
    stream.Write16u((Ipp16u)marker);
}

template<class ByteOutput>
void WriteMarker(JP2Marker marker, Ipp16u bodySize, ByteOutput& stream)
{
    stream.Write16u((Ipp16u)marker);
    stream.Write16u((Ipp16u)(bodySize + 2));
}


#endif // __EJP2MARKER_H__

