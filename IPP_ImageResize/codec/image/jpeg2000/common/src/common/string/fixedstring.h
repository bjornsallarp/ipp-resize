
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

#ifndef __FIXEDSTRING_H__
#define __FIXEDSTRING_H__

#include "fixedbuffer.h"
#include "stralg.h"
#include "genalg.h"

// NOTE: unsigned int StrSize(CHARTYPE) should be supported
template<class CHARTYPE>
class FixedString
{
public:
    FixedString() { Resize(0); }

    FixedString(const FixedString &src)
    {
        Resize(src.Size());
        Copy(src, m_data, Size());
    }

    FixedString(const CHARTYPE *src)
    {
        Resize(StrSize(src));
        Copy(src, m_data, Size());
    }

    FixedString(const CHARTYPE *src, unsigned int maxSize)
    {
        unsigned int size = Min(maxSize, StrSize(src));
        Resize(size);
        Copy(src, m_data, Size());
    }

    FixedString& operator=(const CHARTYPE *src)
    {
        if(m_data != src)
        {
            Resize(StrSize(src));
            Copy(src, m_data, Size());
        }
        return *this;
    }

    FixedString& operator=(const FixedString &str)
    {
        if(this != &str)
        {
            Resize(str.Size());
            Copy(str, m_data, Size());
        }
        return *this;
    }

    operator const CHARTYPE*     () const { return m_data; }
    unsigned int             Size() const { return m_size; }

    FixedString<CHARTYPE> operator + (const FixedString<CHARTYPE> &right)
    {
        return FixedString<CHARTYPE>(*this, right);
    }
    FixedString<CHARTYPE> operator + (const CHARTYPE *right)
    {
        return FixedString<CHARTYPE>(*this, right);
    }
    FixedString<CHARTYPE> operator + (const CHARTYPE right)
    {
        return FixedString<CHARTYPE>(*this, right);
    }

    void CutTail(unsigned int size)
    {
        if(size < m_size) m_size -= size;
        else              m_size  = 0;
        Terminate();
    }

    void Clear() { Resize(0); }

protected:
    FixedBuffer<CHARTYPE> m_data;
    unsigned int          m_size;

    void Resize(unsigned int size)
    {
        m_size = size;
        m_data.ReAlloc(m_size + 1);
        Terminate();
    }

    void Terminate() { m_data[Size()] = StrNature<CHARTYPE>::m_terminator; }

    FixedString(const FixedString &left, const FixedString &right)
    {
        Resize(left.Size() + right.Size());
        Copy(left ,  m_data,              left .Size());
        Copy(right, &m_data[left.Size()], right.Size());
    }

    FixedString(const FixedString &left, const CHARTYPE *right)
    {
        unsigned int rightSize = StrSize(right);
        Resize(left.Size() + rightSize);
        Copy(left,   m_data,              left.Size());
        Copy(right, &m_data[left.Size()], rightSize);
    }

    FixedString(const CHARTYPE *left, const FixedString &right)
    {
        unsigned int leftSize = StrSize(left);
        Resize(leftSize + right.Size());
        Copy(left,   m_data,           leftSize);
        Copy(right, &m_data[leftSize], right.Size());
    }

    FixedString(const FixedString &left, const CHARTYPE right)
    {
        Resize(left.Size() + 1);
        Copy(left, m_data, left.Size());
        m_data[left.Size()] = right;
    }

/*
    FixedString(const CHARTYPE *left, const CHARTYPE *right)
    {
        unsigned int leftSize  = StrSize(left);
        unsigned int rightSize = StrSize(right);

        m_data.ReAlloc(leftSize + rightSize + 1);

        Copy(left,   m_data,           leftSize);
        Copy(right, &m_data[leftSize], rightSize);
        Terminate();
    }
*/

/*
template<class CHARTYPE>
    friend FixedString<CHARTYPE> operator + (const FixedString<CHARTYPE> &left, const FixedString<CHARTYPE> &right);
template<class CHARTYPE>
    friend FixedString<CHARTYPE> operator + (const FixedString<CHARTYPE> &left, const CHARTYPE *right);
template<class CHARTYPE>
    friend FixedString<CHARTYPE> operator + (const CHARTYPE *left, const FixedString<CHARTYPE> &right);
*/
};

template<class CHARTYPE>
bool operator == (const FixedString<CHARTYPE> str1, const CHARTYPE *str2)
{
    unsigned int i = 0;
    for( ; str1[i] && str2[i]; i++)
    {
        if(str1[i] != str2[i]) return false;
    }

    if(str1[i] != str2[i]) return false;

    return true;
}

template<class CHARTYPE>
bool operator == (const CHARTYPE *str1, const FixedString<CHARTYPE> str2)
{
    return (str2==str1);
}

template<class CHARTYPE>
bool operator != (const FixedString<CHARTYPE> str1, const CHARTYPE *str2)
{
    return !(str1 == str2);
}

template<class CHARTYPE>
bool operator != (const CHARTYPE *str1, const FixedString<CHARTYPE> str2)
{
    return (str2!=str1);
}

/*
template<class CHARTYPE>
FixedString<CHARTYPE> operator + (const FixedString<CHARTYPE> &left, const FixedString<CHARTYPE> &right)
{
    return FixedString<CHARTYPE>(left, right);
}

template<class CHARTYPE>
FixedString<CHARTYPE> operator + (const FixedString<CHARTYPE> &left, const CHARTYPE *right)
{
    return FixedString<CHARTYPE>(left, right);
}

template<class CHARTYPE>
FixedString<CHARTYPE> operator + (const CHARTYPE *left, const FixedString<CHARTYPE> &right)
{
    return FixedString<CHARTYPE>(left, right);
}
*/

typedef FixedString<char> StringA;

//
// some stralg.h routines that cover the use of FixedString template in interface
//

template<class CHARTYPE>
void IntToStrR10(unsigned int num, FixedString<CHARTYPE> &dst)
{
    CHARTYPE buf[INT_TO_STR_MAX_SIZE];

    IntToStrR10(num, buf);

    dst = buf;
}

#endif // __FIXEDSTRING_H__
