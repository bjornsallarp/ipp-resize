
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __IMAGEROWOUTPUT_H__
#define __IMAGEROWOUTPUT_H__

#include "rowiterator.h"

template <class T, unsigned int nOfChannels>
class ImageRowOutput
{
public:
    ImageRowOutput() : m_width(0) {}
    ImageRowOutput(const ImageCoreC<T, nOfChannels> &img, unsigned int width)
    : m_row(img), m_width(width) {}

    void         AttachImage(const ImageCoreC<T, nOfChannels> &img)       { m_row   = img;   }
    void         SetWidth   (unsigned int width)                          { m_width = width; }
    unsigned int Width      ()                                      const { return m_width;  }

    void         Write      (const T* data)
    {
        Copy(data, m_row, m_width * nOfChannels);
        ++m_row;
    }

protected:
    RowIterator<T> m_row;
    unsigned int   m_width;
};

#endif // __IMAGEROWOUTPUT_H__
