
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

#ifndef __BYTEINPUT_H__
#define __BYTEINPUT_H__

#include "ippdefs.h"
#include "stack.h"
#include "diagndescr.h"
#include "byteinputexception.h"

/*

  Often such things are maked using technique of virtual functions.
For example the following class can be used as base class for streams

    class ByteInput
    {
    public:
        ...
        virtual Ipp8u Read8u () = 0;
        ...
    }

It's well known solution, but it is not ideal.

One of negative aspects is that this solution not so easy to
optimize by compiler because virtual function
cann't be inlined by compiler (or it's very hard to
determine when it can be inlined) and actually
it requires special calling organization.

Another aspect of virtual function technique is that
it's not so easy to reuse code if you wish to bind another
interface model with abstract classes, wich similar if not
equal to native in functionality, but different in names.
In the other words you're out of luck with abstract virtual classes,
if an abstract virtual class cannot be retrofitted on top of existing
class hierarchies.

For example if you need to bind with interface with function
'GetByte' instead of 'Read8u' it's impossible to substitute it in
a 'native 8u style' base abstract class. And in that case
you need to rewrite all derived stream classes
from 'GetByte style' interface.

For example if you bind with interface

    class FileInputGetByteStyle : public ByteInputGetByteStyle { ...

you have to write somthing about the following:

    class FileInputAdapted : public ByteInput
    {
    ...
    virtual Ipp8u Read8u() { return m_file.GetByte(); }

    FileInputGetByteStyle m_file;
    }

And so on for each stream (file, network, memory, x input/output versions ...).
So you actually need many wrappers.

At this point you could start to think about template using
and write somthing about:

    template <class Original>
    class Adapted : public ByteInput
    {
    ...
    virtual Ipp8u Read8u() { return m_instance.GetByte(); }

    Original m_instance;
    }


But you can note, that it is possible to use templates at the beginning of design
to resolve this issue in couple with perfomance aspect.
The advantage is in tuch only if there is no ACTUAL
needs to switch stream data domains during work.
Of course you can use it with abstract base class too
and you need only to substitute it in template argument list.

The similar solution is used below.

In template argument list for stream templates we use the following convention
about the class substituted under 'ByteInput' type name.
It's required to support in 'ByteInput' one or more (depending on template)
of the following functions:

    Ipp8u        Read8u  ();
    Ipp16u       Read16u ();
    Ipp32u       Read32u ();
    Ipp64u       Read64u ();
    Ipp8s        Read8s  ();
    Ipp16s       Read16s ();
    Ipp32s       Read32s ();
    Ipp64s       Read64s ();
    void         Read    (Ipp8u *dst, unsigned int size);
    unsigned int Position() const;
    void         Seek    ();
    void         Seek    (unsigned int offset);
    unsigned int TailSize() const;
    unsigned int Size()     const;

This list is conventional for input stream using.

In general you need to start from 'Read8u' function and decide how you will obtain the byte.
After this you can construct the other 'Read...' functions using
big/little endian byte input stream templates and template for signed function versions.
'Seek' function can be defined only when you choose data domain
(file, memory, network ...).
The defenition of block reading function 'Read (Ipp8u *dst, unsigned int size)' is
depending on perfomance requirements. You can implement it using general block reading template
or using domain-depended perfomance-optimized version.

The manner of the use of these templates is showed below.

1) At least it's needed to define how to read the byte for given data domain, for example

    class MemoryStreamBase
    {
        Ipp8u Read8u() { return m_data[m_pos]; Seek(); }
    }

2) Then it becames possible to construct memory stream with required functionality,
   in the following example it's the big endian stream with signed reading support

    typedef ByteInputSigned<ByteInputBE<MemoryStreamBase> >    MemoryStreamSignedBE;
*/


/*
// unidirectional abstract input stream
class ByteInput
{
public:
    virtual Ipp8u  Read8u () = 0;
    virtual Ipp16u Read16u() = 0;
    virtual Ipp32u Read32u() = 0;
    virtual Ipp64u Read64u() = 0;

    // signed versions are covered here
    Ipp8s  Read8s()  { return (Ipp8s) Read8u (); }
    Ipp16s Read16s() { return (Ipp16s)Read16u(); }
    Ipp32s Read32s() { return (Ipp32s)Read32u(); }
    Ipp64s Read64s() { return (Ipp64s)Read64u(); }

    virtual void Read(Ipp8u *dst, unsigned int size) = 0;

    virtual unsigned int Position() = 0;

    virtual void Seek()                    = 0;
    virtual void Seek(unsigned int offset) = 0;
private:
    ByteInput() {}
};
*/

template<class ByteInput>
class ByteInputDriver
{
public:
    ByteInputDriver() : m_stream(0) {}

    void Attach(ByteInput &stream) { m_stream = &stream; }

    Ipp8u Read8u()                              const { return m_stream->Read8u(); }
    void  Read  (Ipp8u *dst, unsigned int size) const { m_stream->Read(dst, size); }

protected:
    ByteInput *m_stream;
};


// Little endian byte input stream template
// (it implements functions only for multibyte unsigned data types)
template<class ByteInput>
class ByteInputLE : public ByteInput
{
public:
    ByteInputLE() {}

    Ipp16u Read16u()
    {
        Ipp16u result = this->Read8u();
        result |= ((Ipp16u)this->Read8u() <<  8u);

        return result;
    }

    Ipp32u Read32u()
    {
        Ipp32u result = this->Read8u();

        result |= ((Ipp32u)this->Read8u() <<  8u);
        result |= ((Ipp32u)this->Read8u() << 16u);
        result |= ((Ipp32u)this->Read8u() << 24u);

        return result;
    }

    Ipp64u Read64u()
    {
        Ipp64u result = this->Read8u();

        result |= ((Ipp64u)this->Read8u() << (Ipp64u) 8u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u)16u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u)24u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u)32u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u)40u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u)48u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u)56u);

        return result;
    }
};

// Big endian byte input stream template
// (it implements functions only for multibyte unsigned data types)
template<class ByteInput>
class ByteInputBE : public ByteInput
{
public:
    ByteInputBE() {}

    Ipp16u Read16u()
    {
        Ipp16u result = (Ipp16u)(this->Read8u() <<  8u);
        result |= (Ipp16u)this->Read8u();

        return result;
    }

    Ipp32u Read32u()
    {
        Ipp32u result = (Ipp32u)this->Read8u() << 24u;

        result |= ((Ipp32u)this->Read8u() << 16u);
        result |= ((Ipp32u)this->Read8u() <<  8u);
        result |=  (Ipp32u)this->Read8u();

        return result;
    }

    Ipp64u Read64u()
    {
        Ipp64u result = (Ipp64u)this->Read8u() << (Ipp64u)56u;

        result |= ((Ipp64u)this->Read8u() << (Ipp64u)48u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u)40u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u)32u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u)24u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u)16u);
        result |= ((Ipp64u)this->Read8u() << (Ipp64u) 8u);
        result |=  (Ipp64u)this->Read8u();

        return result;
    }
};

// Byte input stream template implementing signed version of functions
template<class ByteInput>
class ByteInputSigned : public ByteInput
{
public:
    ByteInputSigned() {}

    Ipp8s  Read8s () { return (Ipp8s) this->Read8u (); }
    Ipp16s Read16s() { return (Ipp16s)this->Read16u(); }
    Ipp32s Read32s() { return (Ipp32s)this->Read32u(); }
    Ipp64s Read64s() { return (Ipp64s)this->Read64u(); }
};

template<class T>
class SerialTransport
{
public:
    SerialTransport() : m_position(0), m_size(0) {}
    SerialTransport(const T& position, const T& size) : m_position(position), m_size(size) {}

    const T &Position() const { return m_position;          }
    const T &Size()     const { return m_size;              }
    T        TailSize() const { return m_size - m_position; }

    bool Seek(unsigned int offset)
    {
        if(offset > TailSize()) return false;

        m_position += offset;
        return true;
    }

    bool Seek()
    {
        if(!TailSize()) return false;

        m_position ++;
    }

    void SeekEnd() { m_position = m_size; }

protected:
    T m_position;
    T m_size;
};

template<class ByteInput>
class ByteInputBoundService : public ByteInput
{
public:
    ByteInputBoundService() {}

    virtual ~ByteInputBoundService() {}

    Ipp8u  Read8u () { SeekPos(sizeof(Ipp8u )); return ByteInput::Read8u (); }
    Ipp16u Read16u() { SeekPos(sizeof(Ipp16u)); return ByteInput::Read16u(); }
    Ipp32u Read32u() { SeekPos(sizeof(Ipp32u)); return ByteInput::Read32u(); }
    Ipp64u Read64u() { SeekPos(sizeof(Ipp64u)); return ByteInput::Read64u(); }
    Ipp8s  Read8s () { SeekPos(sizeof(Ipp8s )); return ByteInput::Read8s (); }
    Ipp16s Read16s() { SeekPos(sizeof(Ipp16s)); return ByteInput::Read16s(); }
    Ipp32s Read32s() { SeekPos(sizeof(Ipp32s)); return ByteInput::Read32s(); }
    Ipp64s Read64s() { SeekPos(sizeof(Ipp64s)); return ByteInput::Read64s(); }

    void Read(Ipp8u *dst, unsigned int size)
    {
        SeekPos(size);
        ByteInput::Read(dst, size);
    }

    // Size, Position, TailSize and any of Seek functions
    // can be used only after PushSize function call.
    void Seek()                    { SeekPos();       ByteInput::Seek();       }
    void Seek(unsigned int offset) { SeekPos(offset); ByteInput::Seek(offset); }
    void SeekEnd()
    {
        unsigned int tailSize = TailSize();
        SeekPosEnd();
        ByteInput::Seek(tailSize);
    }

    unsigned int Position() const { return Top().Position(); }
    unsigned int Size()     const { return Top().Size();     }
    unsigned int TailSize() const { return Top().TailSize(); }

    //
    // It helps to gain performance when it's required to control segment
    // of "ByteInputBoundService". It's possible to do without creation of additional
    // super template instantiation "ByteInputBoundService<ByteInputBoundService<ByteInput> >".
    // If you actually do not need cross access to stream
    // it's better to use this function and do not check position twice more times
    // per every "Read" call.
    //
    // Do not forgot to call 'PopSize' when you finish the work with subsegment.
    //
    // it returns actual size of stream
    //
    unsigned int PushSize(unsigned int size)
    {
        if(!m_transportStack.IsEmpty())
            size = Min(size, TailSize()); // when it's called not in first time, otherwise there is no choise and we should confide to the size parameter

        m_transportStack.Push();
        m_transportStack.Top() = SerialTransport<unsigned int>(0, size);

        return size;
    }

    //
    // it returns unhandled size of 'popped' stream
    //
    unsigned int PopSize()
    {
        unsigned int tailSize = TailSize();
        if(tailSize) ByteInput::Seek(tailSize); // we do not need to care about bounds, because it's exactly tail size

        unsigned int subSize = Size();
        m_transportStack.Pop();

        if(!m_transportStack.IsEmpty())
            Top().Seek(subSize); // we do not need to care about bounds, because PushSize() already cares about this bound

        return tailSize;
    }

    virtual void OnLackOfData()
    {
        throw DiagnDescrCT<ByteInputException,noDataToRead>();
    }

protected:
    Stack<SerialTransport<unsigned int> > m_transportStack;

    SerialTransport<unsigned int>       &Top()       { return m_transportStack.Top(); }
    const SerialTransport<unsigned int> &Top() const { return m_transportStack.Top(); }

    void SeekPos()                    { if(!Top().Seek())       OnLackOfData(); }
    void SeekPos(unsigned int offset) { if(!Top().Seek(offset)) OnLackOfData(); }
    void SeekPosEnd()                 {     Top().SeekEnd();                    }
};


#endif // __BYTEINPUT_H__
