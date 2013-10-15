
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __JP2BITSPERCOMP_H__
#define __JP2BITSPERCOMP_H__

#include "ippdefs.h"
#include "jp2const.h"

class JP2BitsPerComp
{
public:
    JP2BitsPerComp(Ipp8u code = 0) : m_code(code) {}
    JP2BitsPerComp(const JP2BitsPerComp &value) : m_code(value.m_code) {}

    JP2BitsPerComp& operator=(const JP2BitsPerComp& value)
    {
        m_code = value.m_code;
        return *this;
    }

    JP2BitsPerComp& operator=(Ipp8u code)
    {
        m_code = code;
        return *this;
    }

    // NOTE: Actual bit depth is BitDepth + 1;
    unsigned int BitDepth()     const { return  m_code & JP2M_BIT_DEPTH; }
    bool         IsSigned()     const { return (m_code & JP2M_IS_SIGNED) != 0; }
    bool         IsConformant() const { return BitDepth() <= JP2V_MAX_BIT_DEPTH; }

    bool operator ==(const JP2BitsPerComp& value) const
    {
        return m_code == value.m_code;
    }

    bool operator !=(const JP2BitsPerComp& value) const
    {
        return !operator==(value);
    }

protected:
    Ipp8u m_code;
};

#endif // __JP2BITSPERCOMP_H__

