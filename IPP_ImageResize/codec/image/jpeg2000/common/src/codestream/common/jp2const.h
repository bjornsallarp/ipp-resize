
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

#ifndef __JP2CONST_H__
#define __JP2CONST_H__

// JP2F_... - flags
// JP2M_... - masks
// JP2V_... - values (sometimes grouped in type enumerators)
// JP2S_... - shift for masked values

enum {
    // Sqcd and Sqcc quantization type masks
    JP2M_QUANT_TYPE = 0x1F,
    // Sqcd and Sqcc guard bits masks
    JP2M_GUARD_BITS = 0xE0,

    // guatd bits shift
    JP2S_GUARD_BITS = 5,


    // SPqcd and SPqcc masks
    JP2M_QUANT_MANTISSA = 0x7FF,

    // quantization step exponent shift
    JP2S_QUANT_EXP   = 11,
    JP2S_NOQUANT_EXP = 3,

    // everywhere in calcuslations actual bit assumed as '_bit_depth_' + 1
    // but such defenition of '_bit_depth_' token helps to simplify expressions
    // with shift operation and helps to do not check zero value for _bit_depth_
    // because zero value for such a '_bit_depth_' is NOT degenerated case
    JP2V_MAX_BIT_DEPTH = 38 - 1,

    // Ssiz (and many other fields) signed component token
    JP2M_IS_SIGNED = 0x80,
    JP2M_BIT_DEPTH = 0x7F,

    // indication allowable only in image header box
    JP2V_BITS_PER_COMP_VARIOUS = 0xFF,

    JP2M_PRECINCT_STEP_ORDER_WIDTH  = 0x0F,
    JP2M_PRECINCT_STEP_ORDER_HEIGHT = 0xF0,
    JP2S_PRECINCT_STEP_ORDER_HEIGHT = 4
};

enum
{
    // SPcodG, SPcocD flags
    JP2F_SELECTIVE_MQ_BYPASS       = 1,
    JP2F_RESETCTX_ON_EVERY_PASS    = 1 << 1,
    JP2F_TERMINATE_ON_EVERY_PASS   = 1 << 2,
    JP2F_VERTICALLY_CAUSAL_CONTEXT = 1 << 3,
    JP2F_PREDICTABLE_TERMINATION   = 1 << 4,
    JP2F_USE_SEGMENTATION_SYMBOLS  = 1 << 5
};

// SGcodC values
typedef enum
{
    JP2V_MCT_NOT_USED = 0,
    JP2V_MCT_012_USED = 1
} JP2MCTSpecType;

// SPcodH values
typedef enum
{
    JP2V_WT97_USED = 0,
    JP2V_WT53_USED = 1
} JP2WTSpecType;


// Scod flags
enum
{
    JP2F_PRECINCT_SIZE_SPEC = 1,
    JP2F_SOP_MAY_BE_USED    = 1 << 1,
    JP2F_EPH_MAY_BE_USED    = 1 << 2
};

// SPcodA, and Ppoc values
typedef enum
{
    JP2V_LRCP_PROGR = 0, // Layer-resolution level-component-position progression
    JP2V_RLCP_PROGR = 1, // Resolution level-layer-component-position progression
    JP2V_RPCL_PROGR = 2, // Resolution level-position-component-layer progression
    JP2V_PCRL_PROGR = 3, // Position-component-resolution level-layer progression
    JP2V_CPRL_PROGR = 4  // Component-position-resolution level-layer progression
} JP2ProgrType;

// Sqcd and Sqcc quantization type
typedef enum
{
    JP2V_QUANT_NO        = 0,
    JP2V_QUANT_DERIVED   = 1,
    JP2V_QUANT_EXPOUNDED = 2
} JP2QuantMode;

typedef enum
{
    JP2V_ENUMERATED_CS = 1,
    JP2V_ICC_POROFILE  = 2
} JP2ColourspaceMethod;

typedef enum
{
    JP2V_CMYK      = 12,
    JP2V_SRGB      = 16,
    JP2V_GRAYSCALE = 17,
    JP2V_YCC       = 18
} JP2EnumColourspace;

typedef enum
{
    JP2V_DIRECT_USE      = 0,
    JP2V_PALETTE_MAPPING = 1
} JP2CompMapType;


static const int DEFAULT_PREC_STEP_ORDER = 15;

// everywhere in calculations actual bit numbers assumed as '_bit_depth_' + 1
// but such definition of '_bit_depth_' token helps to simplify expressions
// with shift operation and helps to do not check zero value for _bit_depth_
// because zero value for such a '_bit_depth_' is NOT degenerated case
static const int DIB_PALETTE_BIT_DEPTH = 7;

#endif // __JP2CONST_H__

