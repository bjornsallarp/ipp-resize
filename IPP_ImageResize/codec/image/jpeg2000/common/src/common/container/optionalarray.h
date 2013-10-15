
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

#ifndef __OPTIONALARRAY_H__
#define __OPTIONALARRAY_H__

#include "fixedarray.h"
#include "optionalvalue.h"

template<class T> class OptionalArray
{
public:
    OptionalArray() {}
    ~OptionalArray() { Free(); }

    void ReAlloc(unsigned int size)
    {
        Free();
        m_ptr.ReAlloc(size);
        for(unsigned int i = 0; i < size; i++)
            m_ptr[i] = &m_default;
    }

    void Free()
    {
        for(unsigned int i = 0; i < Size(); i++)
        {
            if(IsCustom(i)) delete m_ptr[i];
        }

        m_ptr.Free();
    }

    T       &Default() { return m_default; }
    const T &operator[](unsigned int i) const { return *(m_ptr[i]); }

    T       &CreateCustom(unsigned int i)
    {
        if(IsCustom(i)) delete m_ptr[i];
        m_ptr[i] = new T;
        return *(m_ptr[i]);
    }

    unsigned int Size() const { return m_ptr.Size(); }

    bool IsCustom(unsigned int i) const { return m_ptr[i] != &m_default; }

protected:
    T              m_default;
    FixedArray<T*> m_ptr;
};

template<class T> class OptionalSuperArray
{
public:
    OptionalSuperArray() : m_default(0) {}
    ~OptionalSuperArray() { Free(); }

    void ReAlloc(const OptionalArray<T> &parent)
    {
        Free();

        m_value.ReAlloc(parent.Size());

        for(unsigned int i = 0; i < Size(); i++)
            m_value[i].SetRef(parent[i]);
    }

    void Free()
    {
        m_value.Free();

        if(m_default) delete m_default;
        m_default  = 0;
    }

    const T &operator[](unsigned int i) const { return m_value[i]; }

    const T &Default() const { return *m_default; }

    T& CreateCustom(unsigned int i)
    {
        return m_value[i].CreateOwn();
    }

    T& CreateDefaultOwn()
    {
        if(m_default) delete m_default;
        m_default = new T;

        for(unsigned int i = 0; i < Size(); i++)
            m_value[i].SetRef(*m_default);

        return *m_default;
    }

    unsigned int Size() const { return m_value.Size(); }

    bool IsCustom(unsigned int i) const { return m_value[i].IsOwn(); }
    bool IsDefaultOwn() const { return m_default!=0; }

protected:
    T                                *m_default;
    FixedArray<OptionalRefValue<T> >  m_value;
};

#endif // __OPTIONALARRAY_H__

