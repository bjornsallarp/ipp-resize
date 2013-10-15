
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

#ifndef __JP2MARKER_H__
#define __JP2MARKER_H__

#include "ippdefs.h"

               // +---------------------------------------------+--------+----------+
               // |        Description                          | Main   | Tile-part|
               // |                                             | header | header   |
               // +---------------------------------------------+--------+----------+
typedef enum { // | DELIMITING MARKERS AND MARKER SEGMENTS      |        |          |
               // |                                             |        |          |
 SOC = 0xFF4F, // | Start of codestream                         |   R    |     -    |
 SOT = 0xFF90, // | Start of tile-part                          |   -    |     R    |
 SOD = 0xFF93, // | Start of data                               |   -    |          |
 EOC = 0xFFD9, // | End of codestream                           |   -    |     -    |
               // |                                             |        |          |
               // | FIXED INFORMATION MARKER SEGMENTS           |        |          |
               // |                                             |        |          |
 SIZ = 0xFF51, // | Image and tile size                         |   R    |     -    |
               // |                                             |        |          |
               // | Functional marker segments                  |        |          |
               // |                                             |        |          |
 COD = 0xFF52, // | Coding style default                        |   R    |     +    |
 COC = 0xFF53, // | Coding style component                      |   +    |     +    |
 RGN = 0xFF5E, // | Region-of-interest                          |   +    |     +    |
 QCD = 0xFF5C, // | Quantization default                        |   R    |     +    |
 QCC = 0xFF5D, // | Quantization component                      |   +    |     +    |
 POC = 0xFF5F, // | Progression order change (b)                |   +    |     +    |
               // |                                             |        |          |
               // | POINTER MARKER SEGMENTS                     |        |          |
               // |                                             |        |          |
 TLM = 0xFF55, // | Tile-part lengths                           |   +    |     -    |
 PLM = 0xFF57, // | Packet length, main header                  |   +    |     -    |
 PLT = 0xFF58, // | Packet length, tile-part header             |   -    |     +    |
 PPM = 0xFF60, // | Packed packet headers, main header (c)      |   +    |     -    |
 PPT = 0xFF61, // | Packed packet headers, tile-part header (c) |   -    |     +    |
               // |                                             |        |          |
               // | IN BIT STREAM MARKERS AND MARKER SEGMENTS   |        |          |
               // |                                             |        |          |
 SOP = 0xFF91, // | Start of packet                             |   -    |    (1)   |
 EPH = 0xFF92, // | End of packet header                        |  (2)   |    (3)   |
               // |                                             |        |          |
               // | Informational marker segments               |        |          |
               // |                                             |        |          |
 CRG = 0xFF63, // | Component registration                      |   +    |     -    |
 COM = 0xFF64, // | Comment                                     |   +    |     +    |
               // +---------------------------------------------+--------+----------+

 UKN = 0xFFFF

} JP2Marker;   //
               // R required, + optional, - not allowed (a)
               //
               // (1) - in tile-part header, + in bit stream
               //
               // (2) + inside PPM marker segment
               //
               // (3) + inside PPT marker segment or in bit stream
               //
               //
               // (a) Required means the marker or marker segment shall be in this
               //       header, optional means it may be used.
               //
               // (b) The POC marker segment is required
               //       if there are progression order changes.
               //
               // (c) Either the PPM or PPT marker segment is required if the packet
               //       headers are not distributed in the bit stream. If the PPM
               //       marker segment is used then PPT marker segments
               //       shall not be used, and vice versa.
               //



#endif

