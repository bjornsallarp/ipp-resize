
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

#ifndef __OPTIONALVALUE_H__
#define __OPTIONALVALUE_H__


template<class T> class OptionalValue
{
public:
    OptionalValue() : m_ptr(0) {}

    OptionalValue(const OptionalValue &value)
    {
        m_ptr = 0;
        if(value.m_ptr)  Create() = value;
    }

    OptionalValue& operator=(const OptionalValue& value)
    {
        if(this != &value)
        {
            if(value.m_ptr)  Create() = value;
            else             m_ptr    = 0;
        }
        return *this;
    }

    ~OptionalValue() { Destroy(); }

    operator       T&()       { return *m_ptr; }
    operator const T&() const { return *m_ptr; }

    T &Create()
    {
        if(m_ptr) delete m_ptr;
        m_ptr   = new T;
        return *m_ptr;
    }

    void Destroy() { if(m_ptr) delete m_ptr; }

    bool IsExist() const { return m_ptr != 0; }

protected:
    T*   m_ptr;
};

template<class T> class OptionalRefValue
{
public:
    OptionalRefValue() : m_ptr(0), m_isOwn(false) {}

    ~OptionalRefValue() { if(m_isOwn) delete m_ptr; }

    operator       T&()       { return *m_ptr; }
    operator const T&() const { return *m_ptr; }

    T &CreateOwn()
    {
        if(m_isOwn) delete m_ptr;
        m_ptr   = new T;
        m_isOwn = true;
        return *m_ptr;
    }

    void SetRef(const T &reference)
    {
        if(m_isOwn) delete m_ptr;
        m_ptr = (T*)&reference;
        m_isOwn = false;
    }

    bool IsOwn() const { return m_isOwn; }

protected:
    T*   m_ptr;
    bool m_isOwn;
};

#endif // __OPTIONALVALUE_H__
