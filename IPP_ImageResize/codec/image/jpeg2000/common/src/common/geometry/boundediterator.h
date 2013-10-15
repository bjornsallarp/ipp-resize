
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

#ifndef __BOUNDEDITERATOR_H__
#define __BOUNDEDITERATOR_H__

#include "geometry1d.h"

template<class T> class BoundedFwdIterator
{
public:
    BoundedFwdIterator() {}

    BoundedFwdIterator(const T &origin, const T &size)
    : m_value(origin)
    , m_range(origin, size)
    {}

    BoundedFwdIterator(const ROpenInterval<T> &range)
    : m_value(range.Origin())
    , m_range(range)
    {}

    operator const T& () const { return  m_value; }

    T                       Offset()    const { return m_value - m_range.Origin(); }
    bool                    IsInRange() const { return m_value < m_range.Bound();  }
    const ROpenInterval<T> &Range()     const { return m_range; }

    void operator++()    { m_value++; }
    void operator++(int) { ++(*this); }

    void Restart()                               { m_value = m_range.Origin(); }
    void Restart(const T &origin, const T &size) { m_range = ROpenInterval<T>(origin, size); Restart(); }
    void Restart(const ROpenInterval<T> &range)  { m_range = range; Restart(); }

protected:
    T                m_value;
    ROpenInterval<T> m_range;
};

template<class T> class BoundedFwdStepIterator : public BoundedFwdIterator<T>
{
public:
    BoundedFwdStepIterator() {}

    BoundedFwdStepIterator(const T &origin, const T &size, const T &step)
    : BoundedFwdIterator<T>(origin, size)
    , m_step (step)
    {}

    BoundedFwdStepIterator(const ROpenInterval<T> &range, const T &step)
    : BoundedFwdIterator<T>(range)
    , m_step (step)
    {}

    const T &Step() const { return m_step; }

    void operator++()    { m_value += m_step; }
    void operator++(int) { m_value += m_step; }

    void Restart(const T &step) { m_step = step;  Restart(); }
    // the help to deduce base class (BoundedFwdIterator) template functions
    // to avoid bug of one of C++ compilers
    void Restart()                               { BoundedFwdIterator<T>::Restart(); }
    void Restart(const T &origin, const T &size) { BoundedFwdIterator<T>::Restart(origin, size); }
    void Restart(const ROpenInterval<T> &range)  { BoundedFwdIterator<T>::Restart(range); }

protected:
    T m_step;
};

template<class T, class IndexT> class BoundedFwdStepIndexIterator
{
public:
    BoundedFwdStepIndexIterator() {}

    BoundedFwdStepIndexIterator(const T &origin, const T &size, const T &step, const IndexT &startIndex)
    : m_value     (origin, size, step)
    , m_startIndex(startIndex)
    , m_index     (startIndex)
    {}

    BoundedFwdStepIndexIterator(const ROpenInterval<T> &range, const T &step, const IndexT &startIndex)
    : m_value     (range, step)
    , m_startIndex(startIndex)
    , m_index     (startIndex)
    {}

    operator const T& () const { return  m_value; }

    T                       Offset()    const { return m_value.Offset();    }
    bool                    IsInRange() const { return m_value.IsInRange(); }
    const ROpenInterval<T> &Range()     const { return m_value.Range();     }
    const T                &Step()      const { return m_value.Step();      }
    const IndexT           &Index()     const { return m_index;             }

    void SetIndex(const IndexT &index) { m_index = index; }

    void operator++()    { m_value++; m_index++; }
    void operator++(int) { m_value++; m_index++; }

    void Restart()
    {
        m_index = m_startIndex;
        m_value.Restart();
    }

    void Restart(const T &step)
    {
        m_index = m_startIndex;
        m_value.Restart(step);
    }

    void Restart(const T &origin, const T &size)
    {
        m_index = m_startIndex;
        m_value.Restart(origin, size);
    }

    void Restart(const ROpenInterval<T> &range)
    {
        m_index = m_startIndex;
        m_value.Restart(range);
    }

protected:
    IndexT                    m_index;
    BoundedFwdStepIterator<T> m_value;
    IndexT                    m_startIndex;
};


#endif // __BOUNDEDITERATOR_H__

