
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//       Copyright(c) 2002-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __BYTEBUFFER_H__
#define __BYTEBUFFER_H__

#include "bufferstream.h"
#include "byteoutput.h"

typedef ByteOutputSigned<ByteOutputBE<BufferStream> > ByteBuffer;

#endif // __BYTEBUFFER_H__
