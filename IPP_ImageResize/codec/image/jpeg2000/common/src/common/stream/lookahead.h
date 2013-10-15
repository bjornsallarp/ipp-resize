
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

#ifndef __LOOKAHEAD_H__
#define __LOOKAHEAD_H__

#include "ringbuffer.h"

template<class T>
class eDelayLine : public RingBuffer<T>
{
public:
    eDelayLine() {}
    eDelayLine(unsigned int size) : RingBuffer<T>(size) {}

    void Push(const T&value)
    {
        this->SetEntry(value, this->m_position);
        this->operator++();
    }
};


#define dlysz 3

//look-ahead stream class
template<class ByteInput>
class ByteInputLookAhead : public ByteInput
{
public:
    ByteInputLookAhead() {
                           m_EPH = m_SOP = 0;
                           m_bigRead = 0;

                           //delay line init
                           m_dly.ReAlloc(dlysz); }
    ~ByteInputLookAhead() {}

    void Attach(ByteInput &stream)
    { m_stream = &stream;

      //fill the delay line
      for(int i = 0; i < dlysz; i++)
        m_dly.Push(m_stream->Read8u());
    }

   Ipp8u Read8u()
    {

         if(m_bigRead && (!m_dlycnt))
             return m_stream->Read8u();

         if (m_bigRead && (m_dlycnt < 0))
         {
             Ipp8u outbyte = *m_dly;
             m_dlycnt++;

             ++m_dly;

             return outbyte;
         }
         else
         {
            Ipp8u outbyte = *m_dly;

            Ipp16u xMark = LookAhead();

            //EPH?
            if (xMark == EPH)
            { //EPH at the beginning of the packet
              //this may happen in case the previous
              //packet was empty

                for(int i = 0; i < (dlysz - 1); i++)
                    m_dly.Push(m_stream->Read8u());
                xMark = LookAhead();
            }

            //SOP?
            if (xMark == SOP)
            {
                m_stream->Seek(3);
                m_SOP = 1;

                Ipp8u outbyte = m_stream->Read8u();
                if(outbyte != 0x80)
                    for(int i = 0; i < dlysz; i++)
                        m_dly.Push(m_stream->Read8u());

                return outbyte;
            }


            //EPH? - the second checkup
            xMark = LookAhead();
            if (xMark == EPH) m_EPH = 1;
            //if no marker encountered
            else { ++m_dly; m_dly.Push(m_stream->Read8u());}

            return outbyte;
         }
    }

    Ipp16u LookAhead()
    {
        //next byte will be the junior
        Ipp8u bt1 = *m_dly;

        ++m_dly;

        Ipp16u xMark = bt1 << 8;

        xMark += *m_dly;

        if (xMark != SOP && xMark != EPH) xMark = 0;
        return xMark;

    }

    void  Read(Ipp8u *dst, unsigned int size)
    {

        //if it is the first Read for this packet
        if(!m_bigRead)
        {
            m_bigRead = 1;

            //if we haven't met EPH in header
            //we have to return bytes from delay
            if(!m_EPH) m_dlycnt = -1 * dlysz;
            else m_dlycnt = 0;
        }

        Ipp8u *m_dst = dst;
        Ipp8u newbyte;

        for(unsigned int i = 0; i < size; i++)
        {
            newbyte = this->Read8u();
            *m_dst  = newbyte;

            m_dst++;
        }
     }


protected:
    eDelayLine<Ipp8u> m_dly;
    ByteInput        *m_stream;
    int               m_dlycnt;

    Ipp8u             m_EPH;
    Ipp8u             m_SOP;
    Ipp8u             m_bigRead;
};

#undef dlysz

#endif // __LOOKAHEAD_H__
