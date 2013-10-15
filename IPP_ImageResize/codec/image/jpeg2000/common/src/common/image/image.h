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
//  Different image classes mostly intended for store and passing
//      image parameteres and for image memory allocation and deallocation enclosure.
//
//
//
*/

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "imagealloc.h"
#include "imagecore.h"
#include "diagndescr.h"
#include "memexception.h"

template <class T, unsigned int nOfChannels> class ImageC
{
public:
    ImageC() {}

    ImageC(unsigned int width, unsigned int height) { Alloc(width, height); }

    ImageC(const RectSize &size) { Alloc(size.Width(), size.Height()); }

    ~ImageC() { Free(); }

    void ReAlloc(const RectSize &size) { ReAlloc(size.Width(), size.Height()); }

    void ReAlloc(unsigned int width, unsigned int height)
    {
        Free();
        Alloc(width, height);
    }

    void Free()
    {
        if(Data())
        {
            ImageFree(Data());
            m_core = ImageCoreC<T, nOfChannels>();
            m_size = RectSize();
        }
    }

    T* PixelPtr(int x, int y)       const { return m_core.PixelPtr(x, y); }
    T* PixelPtr(const Point &point) const { return m_core.PixelPtr(point); }

    ImageCoreC<T, nOfChannels> SubImage(int x, int y)       const { return m_core.SubImage(x, y);  }
    ImageCoreC<T, nOfChannels> SubImage(const Point &point) const { return m_core.SubImage(point); }

    T*              Data()          const { return m_core.Data(); }
    unsigned int    LineStep()      const { return m_core.LineStep(); }
    unsigned int    Width()         const { return m_size.Width(); }
    unsigned int    Height()        const { return m_size.Height(); }
    const RectSize& Size()          const { return m_size; }

    const ImageCoreC<T, nOfChannels>&
                    Core()          const { return m_core; }

    operator const ImageCoreC<T, nOfChannels>&
                    ()              const { return m_core; }

protected:
    void Alloc(unsigned int width, unsigned int height)
    {
        m_size = RectSize(width, height);
        unsigned int lineStep = 0;
        if(width * height)
        {
            m_core.SetData(ImageAllocC<T>(width, height, lineStep, nOfChannels));
            if(!Data()) throw DiagnDescrCT<MemoryException,faultMalloc>();
        }
        else m_core.SetData(0);

        m_core.SetLineStep(lineStep);
    }

    ImageCoreC<T, nOfChannels> m_core;
    RectSize                   m_size;
};

template <class T, unsigned int nOfChannels> class ImageP
{
public:
    ImageP() {}

    ImageP(unsigned int width, unsigned int height) { Alloc(width, height); }

    ImageP(const RectSize &size) { Alloc(size.Width(), size.Height()); }

    ~ImageP() { Free(); }

    void ReAlloc(const RectSize &size) { ReAlloc(size.Width(), size.Height()); }

    void ReAlloc(unsigned int width, unsigned int height)
    {
        Free();
        Alloc(width, height);
    }

    void Free()
    {
        if(Data())
        {
            ippiFree(Data(0));
            m_core = ImageCoreP<T, nOfChannels>();
            m_size = RectSize();
        }
    }

    T*               PixelPtr(int x, int y)        const { return m_core.PixelPtr(x, y); }
    T*               PixelPtr(const Point &point)  const { return m_core.PixelPtr(point); }

    T*const*         Data()                        const { return m_core.Data(); }
    T*               Data(unsigned int channel)    const { return m_core.Data(channel); }
    unsigned int     LineStep()                    const { return m_core.LineStep(); }
    unsigned int     Width()                       const { return m_size.Width(); }
    unsigned int     Height()                      const { return m_size.Height(); }
    const RectSize&  Size()                        const { return m_size; }
    ImageCoreC<T, 1> Channel(unsigned int channel) const { return m_core.Channel(channel); }

    const ImageCoreP<T, nOfChannels>&
                     Core()                        const { return m_core; }

    operator const ImageCoreP<T, nOfChannels>&()   const { return m_core; }

protected:
    void Alloc(unsigned int width, unsigned int height)
    {
        m_size = RectSize(width, height);

        unsigned int lineStep = 0;
        void *data = 0;

        if(width * height)
        {
            data = ImageAllocP<T>(width, height, lineStep, nOfChannels);
            if(!data) throw DiagnDescrCT<MemoryException,faultMalloc>();

        }

        m_core.SetLineStep(lineStep);

        for(int channel = 0; channel < nOfChannels; channel++)
            m_core.SetData(channel, (T*)addrAdd(data, channel * height * LineStep()));
    }

    ImageCoreP<T, nOfChannels> m_core;
    RectSize                   m_size;
};

template <class T> class ImageCn
{
public:
    ImageCn() {}

    ImageCn(unsigned int width, unsigned int height, unsigned int nOfChannels)
    {
        Alloc(width, height, nOfChannels);
    }

    ImageCn(const RectSize &size, unsigned int nOfChannels)
    {
        Alloc(size.Width(), size.Height(), nOfChannels);
    }

    ~ImageCn() { Free(); }

    void ReAlloc(const RectSize &size, unsigned int nOfChannels)
    {
        ReAlloc(size.Width(), size.Height(), nOfChannels);
    }

    void ReAlloc(unsigned int width, unsigned int height, unsigned int nOfChannels)
    {
        Free();
        Alloc(width, height, nOfChannels);
    }

    void Free()
    {
        if(Data())
        {
            ippiFree(Data());
            m_core = ImageCoreCn<T>();
            m_size = RectSize();
        }
    }

    T*              Data()          const { return m_core.Data(); }
    unsigned int    LineStep()      const { return m_core.LineStep(); }
    unsigned int    Width()         const { return m_size.Width(); }
    unsigned int    Height()        const { return m_size.Height(); }
    const RectSize& Size()          const { return m_size; }
    unsigned int    NOfChannels()   const { return m_core.NOfChannels(); }

    const ImageCoreCn<T>&
                    Core()          const { return m_core; }

    operator const ImageCoreCn<T>&
                    ()              const { return m_core; }

protected:
    void Alloc(unsigned int width, unsigned int height, unsigned int nOfChannels)
    {
        m_size = RectSize(width, height);
        m_core.SetNOfChannels(nOfChannels);

        unsigned int lineStep = 0;
        if(width * height)
        {
            m_core.SetData(ImageAllocC<T>(width, height, lineStep, nOfChannels));
            if(!Data()) throw DiagnDescrCT<MemoryException,faultMalloc>();
        }
        else m_core.SetData(0);

        m_core.SetLineStep(lineStep);
    }

    ImageCoreCn<T> m_core;
    RectSize       m_size;
};

template <class T> class ImagePn
{
public:
    ImagePn() {}

    ImagePn(unsigned int width, unsigned int height, unsigned int nOfChannels)
    {
        Alloc(width, height, nOfChannels);
    }

    ImagePn(const RectSize &size, unsigned int nOfChannels)
    {
        Alloc(size.Width(), size.Height(), nOfChannels);
    }

    ~ImagePn() { Free(); }

    void ReAlloc(const RectSize &size, unsigned int nOfChannels)
    {
        ReAlloc(size.Width(), size.Height(), nOfChannels);
    }

    void ReAlloc(unsigned int width, unsigned int height, unsigned int nOfChannels)
    {
        Free();
        Alloc(width, height, nOfChannels);
    }

    void Free()
    {
        if(Data())
        {
            ippiFree(Data(0));
            m_core = ImageCorePn<T>();
            m_size = RectSize();
        }
    }

    T*const*         Data()                        const { return m_core.Data(); }
    T*               Data(unsigned int channel)    const { return m_core.Data(channel); }
    unsigned int     LineStep()                    const { return m_core.LineStep(); }
    unsigned int     Width()                       const { return m_size.Width(); }
    unsigned int     Height()                      const { return m_size.Height(); }
    const RectSize&  Size()                        const { return m_size; }
    unsigned int     NOfChannels()                 const { return m_core.NOfChannels(); }
    ImageCoreC<T, 1> Channel(unsigned int channel) const { return m_core.Channel(channel); }
    FixedArray<ImageCoreC<T, 1> >
                     Channels()                    const { return m_core.Channels(); }


    const ImageCorePn<T>&
                     Core()                        const { return m_core; }

    operator const ImageCorePn<T>&
                     ()                            const { return m_core; }

protected:
    void Alloc(unsigned int width, unsigned int height, unsigned int nOfChannels)
    {
        m_size = RectSize(width, height);
        m_core.SetNOfChannels(nOfChannels);

        unsigned int lineStep = 0;
        void *data = 0;

        if(width * height)
        {
            data = ImageAllocP<T>(width, height, lineStep, nOfChannels);
            if(!data) throw DiagnDescrCT<MemoryException,faultMalloc>();

        }

        m_core.SetLineStep(lineStep);

        for(unsigned int channel = 0; channel < nOfChannels; channel++)
            m_core.SetData(channel, (T*)addrAdd(data, channel * height * LineStep()));
    }

    ImageCorePn<T> m_core;
    RectSize       m_size;
};


#include "ippdefs.h"

typedef ImageCoreC<Ipp32s, 1> ImageCore32sC1;

typedef ImageC<Ipp32s, 1> Image32sC1;

typedef ImageC<Ipp32f, 1> Image32fC1;

#endif // __IMAGE_H__

