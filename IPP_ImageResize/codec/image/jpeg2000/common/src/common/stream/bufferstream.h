
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

#ifndef __BUFFERSTREAM_H__
#define __BUFFERSTREAM_H__

#include "ippdefs.h"
#include "genalg.h"
#include "fixedbuffer.h"
#include "list_.h"
#include "diagndescr.h"
#include "byteinputexception.h"

//
// It could be much more simplier to implement "Transfer" template functions,
// but some compilers have a bug and do not support
// "partial ordering of function templates" (or specialization)
// and for example the following code does not compiled.
//
/*
template<class T1, class T2>
void Foo(T1 val1, T2 val2) { val1; val2; cout << "base template was called" << endl; }

template<class T1>
void Foo(T1 val1, int val2) { val1; val2; cout << "specialization on second template parameter for type 'int'" << endl; }
*/

//
// We try to work around it to keep "Transfer(src, dst, size);" sytax
//  for 'auto' caling of appropriate functionality.
//
// Sorry, but all the abstract cases can not be collected in this way,
// so it's not very 'general' solution and you can
// consider it only in this class framework.
//
//
class MemoryBlock;
class BufferStream;

template<class ByteInput, class ByteOutput> class TransferFactory
{
public:
    //
    // Some additional, more or less general templates can be defined here.
    //
    // template<class SRC, class DST>
    //     static void Transfer(SRC val1, DST val2, unsigned int size) {  }
    //
    // if it's required
    //

    static unsigned int Transfer(MemoryBlock  &src, ByteOutput   &dst, unsigned int size) { return Transfer_ISpec(src, dst, size); }
    static unsigned int Transfer(ByteInput    &src, MemoryBlock  &dst, unsigned int size) { return Transfer_OSpec(src, dst, size); }

    static unsigned int Transfer(BufferStream &src, ByteOutput   &dst, unsigned int size) { return Transfer_ISpec(src, dst, size); }
    static unsigned int Transfer(ByteInput    &src, BufferStream &dst, unsigned int size) { return Transfer_OSpec(src, dst, size); }
};

template<class ByteInput, class ByteOutput>
unsigned int Transfer(ByteInput &val1, ByteOutput &val2, unsigned int size)
{
    return TransferFactory<ByteInput, ByteOutput>::Transfer(val1, val2, size);
}


class MemoryBlock
{
public:
    MemoryBlock() : m_buffer(m_size)
    {
        m_bound = &m_buffer[m_size];
        m_wPos = m_buffer;
        m_rPos = m_buffer;
    }

    bool Write(Ipp8u byte)
    {
        if(!TailSize()) return false;

        *m_wPos = byte;
        m_wPos++;
        return true;
    }

    unsigned int Write(const Ipp8u *src, unsigned int size)
    {
        unsigned int actualSize = Min(TailSize(), size);

        Copy(src, m_wPos, actualSize);
        m_wPos += actualSize;

        return actualSize;
    }

    bool Read(Ipp8u &byte)
    {
        if(!FillSize()) return false;

        byte = *m_rPos;
        m_rPos++;
        return true;
    }

    unsigned int Read(Ipp8u *dst, unsigned int size)
    {
        unsigned int actualSize = Min(FillSize(), size);

        Copy(m_rPos, dst, actualSize);
        m_rPos += actualSize;

        return actualSize;
    }

    unsigned int FillSize() const { return (unsigned int)(m_wPos  - m_rPos); }
    unsigned int TailSize() const { return (unsigned int)(m_bound - m_wPos); }


//
// Another part of work around of
// "partial ordering of function templates" (or specialization) compiler bug.
//
friend unsigned int Transfer      (MemoryBlock& src, MemoryBlock& dst, unsigned int size);

template<class ByteInput>
friend unsigned int Transfer_OSpec(ByteInput&   src, MemoryBlock& dst, unsigned int size);

template<class ByteOutput>
friend unsigned int Transfer_ISpec(MemoryBlock& src, ByteOutput&  dst, unsigned int size);

protected:
    static const unsigned int m_size;
    FixedBuffer<Ipp8u>        m_buffer;
    const Ipp8u              *m_rPos;
    Ipp8u                    *m_wPos;
    const Ipp8u              *m_bound;
};

template<typename ByteInput>
unsigned int Transfer_OSpec(ByteInput& src, MemoryBlock& dst, unsigned int size)
{
    unsigned int actualSize = Min(dst.TailSize(), size);

    src.Read(dst.m_wPos, actualSize);
    dst.m_wPos += actualSize;

    return actualSize;
}

unsigned int Transfer(MemoryBlock& src, MemoryBlock& dst, unsigned int size);

template<class ByteOutput>
unsigned int Transfer_ISpec(MemoryBlock& src, ByteOutput& dst, unsigned int size)
{
    unsigned int actualSize = Min(size, src.FillSize());

    dst.Write(src.m_rPos, actualSize);
    src.m_rPos += actualSize;

    return actualSize;
}


class BufferStream {
public:
    BufferStream() : m_size(0) { m_memBlockList.PushBack(); }

    void Clear()
    {
        m_memBlockList.Clear();
        m_size = 0;
        m_memBlockList.PushBack();
    }

    void Write8u(Ipp8u byte)
    {
        if(!BackBlock().Write(byte))
        {
            PushBackBlock();
            BackBlock().Write(byte);
        }
        m_size++;
    }

    void Write(const Ipp8u *src, unsigned int size)
    {
        unsigned int usedSize = 0;

        while(usedSize < size)
        {
            unsigned int blockUsedSize = BackBlock().Write(&src[usedSize], size - usedSize);

            if(!blockUsedSize) PushBackBlock();

            usedSize += blockUsedSize;
        }
        m_size += size;
    }

    // This function corresponds to 'ByteInput' convention
    // and it throws exception in case of buffer data lack
    Ipp8u Read8u()
    {
        Ipp8u value;

        if(!Read(value))
            throw DiagnDescrCT<ByteInputException,noDataToRead>();

        return value;
    }

    // This function corresponds to 'ByteInput' convention
    // and it throws exception in case of buffer data lack
    void Read(Ipp8u *dst, unsigned int size)
    {
        if(size != ReadBytes(dst, size))
            throw DiagnDescrCT<ByteInputException,noDataToRead>();
    }


    unsigned int Size() const { return m_size; }

    // This function cannot throw exceptions
    // it returns number of bytes which were actually read
    unsigned int ReadBytes(Ipp8u *dst, unsigned int size)
    {
        unsigned int usedSize = 0;

        while(usedSize < size)
        {
            unsigned int blockUsedSize = FrontBlock().Read(&dst[usedSize], size - usedSize);

            if(!blockUsedSize)
            {
                if(IsSingleBlock()) break;
                PopFrontBlock();
            }
            else usedSize += blockUsedSize;
        }
        m_size -= usedSize;

        return usedSize;
    }

    // This function cannot throw exceptions
    // it returns false if there's no data
    bool Read(Ipp8u &byte)
    {
        while(!FrontBlock().Read(byte))
        {
            if(IsSingleBlock()) return false;
            PopFrontBlock();
        }
        m_size--;
        return true;
    }

friend unsigned int Transfer      (BufferStream& src, BufferStream& dst, unsigned int size);

template<class ByteInput>
friend unsigned int Transfer_OSpec(ByteInput&    src, BufferStream& dst, unsigned int size);

template<class ByteOutput>
friend unsigned int Transfer_ISpec(BufferStream& src, ByteOutput&   dst, unsigned int size);

protected:
    List<MemoryBlock> m_memBlockList;
    unsigned int      m_size;

    MemoryBlock &FrontBlock() { return m_memBlockList.Front(); }
    MemoryBlock &BackBlock () { return m_memBlockList.Back(); }

    void PopFrontBlock() { m_memBlockList.PopFront(); }
    void PushBackBlock() { m_memBlockList.PushBack(); }

    bool IsSingleBlock() const { return m_memBlockList.ItrFront() == m_memBlockList.ItrBack(); }
};

unsigned int Transfer(BufferStream& src, BufferStream& dst, unsigned int size);

template<class ByteInput>
unsigned int Transfer_OSpec(ByteInput& src, BufferStream& dst, unsigned int size)
{
    unsigned int usedSize = 0;

    while(usedSize < size)
    {
        if( ! dst.BackBlock().TailSize())
            dst.PushBackBlock();

        unsigned int blockUsedSize
            = Transfer(src, dst.BackBlock(), size - usedSize);

        usedSize += blockUsedSize;
    }
    dst.m_size += usedSize;

    return usedSize;
}

template<class ByteOutput>
unsigned int Transfer_ISpec(BufferStream& src, ByteOutput& dst, unsigned int size)
{
    unsigned int usedSize = 0;

    while(usedSize < size)
    {
        unsigned int blockUsedSize
            = Transfer(src.FrontBlock(), dst, size - usedSize);

        if(!blockUsedSize)
        {
            if(src.IsSingleBlock()) break;
            src.PopFrontBlock();
        }
        else usedSize += blockUsedSize;
    }
    src.m_size -= usedSize;

    return usedSize;
}

// mostly for current version of encoder
// it suppose that destination could receive all the source data
template<class ByteOutput>
unsigned int TransferAllSrc(BufferStream& src, ByteOutput& dst)
{
    return Transfer(src, dst, src.Size());
}


#endif // __BUFFERSTREAM_H__
