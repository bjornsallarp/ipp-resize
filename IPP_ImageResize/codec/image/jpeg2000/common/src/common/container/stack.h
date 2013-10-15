
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

#ifndef __STACK_H__
#define __STACK_H__

#include "list_.h"

template<class T>
class Stack
{
public:
    Stack() {}

    void     Push()      { m_container.PushBack();    }
    void     Pop()       { m_container.PopBack();     }

    const T& Top() const { return m_container.Back(); }
    T&       Top()       { return m_container.Back(); }

    void Clear  ()       { m_container.Clear();          }
    bool IsEmpty()       { return m_container.IsEmpty(); }

protected:
    List<T> m_container;
};

#endif // __STACK_H__
