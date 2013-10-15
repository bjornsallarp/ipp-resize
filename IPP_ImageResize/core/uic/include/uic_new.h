/*
//
//             INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __UIC_NEW_H__
#define __UIC_NEW_H__

#include <stddef.h>
#include "uic_defs.h"

namespace UIC {
    class NewBuffer
    {
    public:
        NewBuffer() : m_ptr(0) {}
        NewBuffer(void* ptr) : m_ptr(ptr) {}
        NewBuffer Offset(Ipp32u offset) const { return NewBuffer((char*)m_ptr + offset); }
        operator void*() const { return m_ptr; }
    protected:
        void *m_ptr;
    };
} // namespace UIC

inline void* operator new(size_t, const UIC::NewBuffer &buffer)
{
    return buffer;
}

// delete for exceptions cases only (i.e. non-UIC)
inline void operator delete(void*, const UIC::NewBuffer &) {}

namespace UIC {

    UICAPI void*  ArrAlloc  (Ipp32u itemSize, Ipp32u nOfItems);
    UICAPI void   ArrFree   (const void* arr);
    UICAPI Ipp32u ArrCountOf(const void* arr);


    template<class T> T *NewArr(Ipp32u n, const T* = 0)
    {
        NewBuffer buffer (ArrAlloc(sizeof(T), n) );
        unsigned int i = 0;
        for(; i < n; i++)
            new(buffer.Offset(i * sizeof(T))) T;
        return (T*)(void*)buffer;
    }

    template<class T> void DeleteArr(T *arr)
    {
        unsigned int i = 0;
        unsigned int n = ArrCountOf(arr);
        for(; i < n; i++)
            arr[i].~T();
        ArrFree(arr);
    }

} // namespace UIC

#endif // __UIC_NEW_H__
