
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

#ifndef __JP2BOX_H__
#define __JP2BOX_H__

#include "ippdefs.h"
                    // +----------------------------------------+----------+-----------+
typedef enum {      // |        Description                     | Superbox | Required? |
                    // +----------------------------------------+----------+-----------+
 jp__ = 0x6A502020, // | JPEG 2000 Signature box 'jP\040\040'   |    No    |  Required |
 ftyp = 0x66747970, // | File Type box 'ftyp'                   |    No    |  Required |
 jp2h = 0x6A703268, // | JP2 Header box 'jp2h'                  |    Yes   |  Required |
 ihdr = 0x69686472, // | Image Header box 'ihdr'                |    No    |  Required |
 bpcc = 0x62706363, // | Bits Per Component box 'bpcc'          |    No    |  Optional |
 colr = 0x636F6C72, // | Colour Specification box 'colr'        |    No    |  Required |
 pclr = 0x70636C72, // | Palette box 'pclr'                     |    No    |  Optional |
 cmap = 0x636D6170, // | Component Mapping box 'cmap'           |    No    |  Optional |
 cdef = 0x63646566, // | Channel Definition box 'cdef'          |    No    |  Optional |
 res_ = 0x72657320, // | Resolution box 'res\040'               |    Yes   |  Optional |
 resc = 0x72657363, // | Capture Resolution box 'resc'          |    No    |  Optional |
 resd = 0x72657364, // | Default Display Resolution box 'resd'  |    No    |  Optional |
 jp2c = 0x6A703263, // | Contiguous Codestream box 'jp2c'       |    No    |  Required |
 jp2i = 0x6A703269, // | Intellectual Property box 'jp2i'       |    No    |  Optional |
 xml_ = 0x786D6C20, // | XML box 'xml\040'                      |    No    |  Optional |
 uuid = 0x75756964, // | UUID box 'uuid'                        |    No    |  Optional |
 uinf = 0x75696E66, // | UUID Info box 'uinf'                   |    Yes   |  Optional |
 ulst = 0x75637374, // | UUID List box 'ulst'                   |    No    |  Optional |
 url_ = 0x75726C20, // | URL box 'url\040'                      |    No    |  Optional |
                    // +----------------------------------------+----------+-----------+
 unkn = 0x00000000
} JP2BoxType;


static const Ipp32u jp2id = 0x0D0A870A;
static const Ipp32u jp2_  = 0x6A703220;


#endif

