
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

/*
//  This template implements simple list.
//      It differs from STL list container. It is more simple (but it is not
//      so generalized). You can be sure that the object constructor
//      called such times as needed, no more. It intendeds
//      for correct exceptions handling in constructors.
//      And it requires default constructor for object.
//
//
//
*/

#ifndef __LIST_H__
#define __LIST_H__

#if _MSC_VER
#if _MSC_VER < 1300
#pragma warning(disable: 4284)
#endif
#endif

template<class T> class List
{
protected:
    class Node
    {
    public:
        Node()
        {
            m_value = 0;
            m_prev  = this;
            m_next  = this;
        }

        Node(Node *prev, Node *next)
        : m_prev(prev), m_next(next)
        {
            m_value  = new T;
        }

        ~Node() {  if(m_value) delete m_value; }

        Node *m_prev;
        Node *m_next;
        T    *m_value;
    };

public:
    class Iterator
    {
    public:
        Iterator()                     : m_curr(0)           {}
        Iterator(const Node *node)     : m_curr(node)        {}
        Iterator(const Iterator &iter) : m_curr(iter.m_curr) {}

        Iterator& operator=(const Iterator &iter)
        {
            if(this != &iter)
            {
                m_curr = iter.m_curr;
            }
            return *this;
        }

        operator T*()   const { return m_curr->m_value; }
        T* operator->() const { return (&**this);       }


        Iterator& operator++()
        {
            m_curr = m_curr->m_next;
            return *this;
        }

        Iterator& operator--()
        {
            m_curr = m_curr->m_prev;
            return *this;
        }

        bool operator == (const Iterator& iter) const
        {
            return m_curr == iter.m_curr;
        }

        bool operator != (const Iterator& iter) const
        {
            return !(*this == iter);
        }

    protected:
        const Node *m_curr;
    };


    List() {}

    ~List() { Clear(); }

    void Clear () { while(!IsEmpty()) PopBack(); }

    bool IsEmpty() { return m_keystone.m_next == &m_keystone; }

    void PushBack()
    {
        if(IsEmpty())
        {
            m_keystone.m_prev = new Node(&m_keystone, &m_keystone);
            m_keystone.m_next = m_keystone.m_prev;
        }
        else
        {
            Node* last   = m_keystone.m_prev;
            m_keystone.m_prev = new Node(m_keystone.m_prev, &m_keystone);
            last->m_next =  m_keystone.m_prev;
        }
    }

    void PopBack()
    {
        Node *last = m_keystone.m_prev->m_prev;
        delete m_keystone.m_prev;
        m_keystone.m_prev = last;
        last->m_next = &m_keystone;
    }

    void PopFront()
    {
        Node *first = m_keystone.m_next->m_next;
        delete m_keystone.m_next;
        m_keystone.m_next = first;
        first->m_prev = &m_keystone;
    }

    Iterator ItrFront()      const { return Iterator(m_keystone.m_next); }
    Iterator ItrBack ()      const { return Iterator(m_keystone.m_prev); }

    Iterator ItrBackBound () const { return Iterator(&m_keystone); }
    Iterator ItrFrontBound() const { return Iterator(&m_keystone); }

    const T& Front() const { return *ItrFront(); }
    T&       Front()       { return *ItrFront(); }

    const T& Back () const { return *ItrBack (); }
    T&       Back ()       { return *ItrBack (); }

protected:
    Node m_keystone;
};


#endif // __LIST_H__
