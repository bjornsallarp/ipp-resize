
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

#ifndef __IMAGECORE_H__
#define __IMAGECORE_H__

#include "geometry2d.h"
#include "addrarith.h"
#include "fixedarray.h"

template <class T> class ImageCore
{
public:
    ImageCore()                               : m_data(0),          m_lineStep(0)              {}
    ImageCore(T* data, unsigned int lineStep) : m_data(data),       m_lineStep(lineStep)       {}
    ImageCore(const ImageCore& img)           : m_data(img.Data()), m_lineStep(img.LineStep()) {}

    void SetData    (T* data)               { m_data     = data; }
    void SetLineStep(unsigned int lineStep) { m_lineStep = lineStep; }

    T*           Data()     const { return m_data;     }
    unsigned int LineStep() const { return m_lineStep; }

    ImageCore& operator=(const ImageCore& img)
    {
        if(this != &img)
        {
            SetData    (img.Data());
            SetLineStep(img.LineStep());
        }
        return *this;
    }

protected:
    T            *m_data;
    unsigned int  m_lineStep;
};


template <class T, unsigned int nOfChannels> class ImageCoreC
: public ImageCore<T>
{
public:
    ImageCoreC() {}
    ImageCoreC(T* data, unsigned int lineStep) : ImageCore<T>(data, lineStep) {}
    ImageCoreC(const ImageCoreC& img)          : ImageCore<T>(img) {}

    T* Line(int y)                  const { return addrAdd(this->Data(), y * this->LineStep()); }
    T* PixelPtr(int x, int y)       const { return &(Line(y)[nOfChannels * x]); }
    T* PixelPtr(const Point &point) const { return PixelPtr(point.X(), point.Y()); }

    ImageCoreC SubImage(int x, int y) const
    {
        ImageCoreC img(PixelPtr(x, y), this->LineStep());
        return img;
    }

    ImageCoreC SubImage(const Point &point) const
    {
        ImageCoreC img(PixelPtr(point), this->LineStep());
        return img;
    }

    ImageCoreC& operator=(const ImageCoreC& value)
    {
        ImageCore<T>::operator=(value);
        return *this;
    }
};


template <class T, unsigned int nOfChannels> class ImageCoreP
{
public:
    ImageCoreP() : m_lineStep(0)
    {
        for(int i = 0; i < nOfChannels; i++)
            SetData(i, 0);
    }

    ImageCoreP(T*const* data, unsigned int lineStep) : m_lineStep(lineStep)
    {
        SetData(data);
    }

    ImageCoreP(const ImageCoreP& img) : m_lineStep(img.LineStep())
    {
        SetData(img.Data());
    }

    void SetData(T*const* data)
    {
        for(int i = 0; i < nOfChannels; i++)
            SetData(i, data[i]);
    }

    void SetData(unsigned int channel, T* data) { m_data[channel] = data; }

    void SetLineStep(unsigned int lineStep)     { m_lineStep = lineStep; }

    T*const*     Data()                     const { return m_data;     }
    T*           Data(unsigned int channel) const { return m_data[channel]; }
    unsigned int LineStep()                 const { return m_lineStep; }

    ImageCoreC<T, 1> Channel(unsigned int channel) const
    {
        ImageCoreC<T, 1> img(Data(channel), LineStep());
        return img;
    }

    ImageCoreP& operator=(const ImageCoreP& img)
    {
        if ( this != &img )
        {
            SetData    (img.Data());
            SetLineStep(img.LineStep());
        }
        return *this;
    }

protected:
    T   *m_data[nOfChannels];
    int  m_lineStep;
};

template <class T> class ImageCoreCn
: public ImageCore<T>
{
public:
    ImageCoreCn() : m_nOfChannels(0) {}

    ImageCoreCn(T* data, unsigned int lineStep, unsigned int nOfChannels)
    : ImageCore<T>(data, lineStep), m_nOfChannels(nOfChannels) {}

    ImageCoreCn(const ImageCoreCn& img)
    : ImageCore<T>(img), m_nOfChannels(img.nOfChannels) {}

    void SetNOfChannels(unsigned int nOfChannels) { m_nOfChannels = nOfChannels; }
    unsigned int NOfChannels() const { return m_nOfChannels; }

    ImageCoreCn& operator=(const ImageCoreCn& img)
    {
        ImageCore<T>::operator=(img);

        if(this != &img) SetNOfChannels(img.NOfChannels());

        return *this;
    }

protected:
    unsigned int  m_nOfChannels;
};

template <class T> class ImageCorePn
{
public:
    ImageCorePn() : m_lineStep(0), m_nOfChannels(0) {}

    ImageCorePn(T*const* data, unsigned int lineStep, unsigned int nOfChannels)
    {
        SetNOfChannels(nOfChannels);
        SetData(data);
        SetLineStep(lineStep);
    }

    ImageCorePn(const ImageCorePn& img)
    {
        SetNOfChannels(img.NOfChannels());
        SetData(img.Data());
        SetLineStep(img.LineStep());
    }

    ImageCorePn SubImage(const Point &point) const
    {
        ImageCorePn img;

        img.SetLineStep   (m_lineStep);
        img.SetNOfChannels(m_nOfChannels);

        for(unsigned int i = 0; i < m_nOfChannels; i++)
            img.SetData(i, Channel(i).PixelPtr(point));

        return img;
    }

    void SetData(T*const* data)
    {
        for(unsigned int i = 0; i < m_nOfChannels; i++)
            SetData(i, data[i]);
    }

    void SetData(unsigned int channel, T* data) { m_data[channel] = data; }

    void SetLineStep(unsigned int lineStep)     { m_lineStep = lineStep; }

    void SetNOfChannels(unsigned int nOfChannels)
    {
        m_nOfChannels = nOfChannels;
        m_data.ReAlloc(nOfChannels);
        for(unsigned int i = 0; i < nOfChannels; i++)
            SetData(i, 0);
    }

    T*const*     Data()                     const { return m_data;     }
    T*           Data(unsigned int channel) const { return m_data[channel]; }
    unsigned int LineStep()                 const { return m_lineStep; }
    unsigned int NOfChannels()              const { return m_nOfChannels; }

    ImageCoreC<T, 1> Channel(unsigned int channel) const
    {
        ImageCoreC<T, 1> img(Data(channel), LineStep());
        return img;
    }

    FixedArray<ImageCoreC<T, 1> > Channels() const
    {
        FixedArray<ImageCoreC<T, 1> > channel(m_nOfChannels);
        for(unsigned int i = 0; i < m_nOfChannels; i++)
            channel[i] = ImageCoreC<T, 1>(Data(i), LineStep());

        return channel;
    }

    ImageCorePn& operator=(const ImageCorePn& img)
    {
        if ( this != &img )
        {
            SetNOfChannels(img.NOfChannels());
            SetData       (img.Data());
            SetLineStep   (img.LineStep());
        }
        return *this;
    }

protected:
    FixedBuffer<T*> m_data;
    unsigned int    m_lineStep;
    unsigned int    m_nOfChannels;
};

#endif // __IMAGECORE_H__
