///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004 Intel Corporation 
// All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met: 
//
// * Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
// * Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution. 
// * Neither name of Intel Corporation nor the names of its contributors 
// may be used to endorse or promote products derived from this software 
// without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resize.h"

Resize::Resize() : isSrcROI_(0), isDstROI_(0), isCenter_(0),
	interp_(IPPI_INTER_LINEAR)
{
}

Resize::~Resize() {}

void Resize::SetInterpType(int type)
{
	if (type == NN) interp_ = IPPI_INTER_NN;
	else if (type == LINEAR) interp_ = IPPI_INTER_LINEAR;
	else if (type == CUBIC) interp_ = IPPI_INTER_CUBIC;
	else if (type == SUPERSAMPLE) interp_ = IPPI_INTER_SUPER;
	else if (type == LANCZOS) interp_ = IPPI_INTER_LANCZOS;
	else if (type == LANCZOS_SMOOTH) interp_ = IPPI_INTER_LANCZOS | IPPI_SMOOTH_EDGE;
}

int Resize::GetInterpType() const
{
	if (interp_ == IPPI_INTER_NN) return NN;
	else if (interp_ == IPPI_INTER_LINEAR) return LINEAR;
	else if (interp_ == IPPI_INTER_CUBIC) return CUBIC;
	else if (interp_ == IPPI_INTER_SUPER) return SUPERSAMPLE;
	else if (interp_ == IPPI_INTER_LANCZOS) return LANCZOS;
	else return 0;
}

void Resize::SetFactors(double xFactor, double yFactor)
{
	xFactor_ = xFactor;
	yFactor_ = yFactor;
}

void Resize::SetFactors(IppiSize srcSize, IppiSize dstSize)
{
	xFactor_ = (double)dstSize.width / (double)srcSize.width;
	yFactor_ = (double)dstSize.height / (double)srcSize.height;
}

void Resize::SetSrcROI(IppiRect srcROI)
{
	srcROI_.x = srcROI.x;
	srcROI_.y = srcROI.y;
	srcROI_.width = srcROI.width;
	srcROI_.height = srcROI.height;

	isSrcROI_ = 1;
}

void Resize::SetDestROI(IppiRect dstROI)
{
	dstROI_.width = dstROI.width;
	dstROI_.height = dstROI.height;
	dstXOffset_ = dstROI.x;
	dstYOffset_ = dstROI.y;
	
	isDstROI_ = 1;
}

void Resize::SetCenter(double xCenter, double yCenter)
{
	xCenter_ = xCenter;
	yCenter_ = yCenter;
	isCenter_ = 1;
}

IppiSize Resize::GetDestSize() const
{
	IppiSize dstSize = {0,0};
	if (!isSrcROI_) return dstSize;
	dstSize.width = (int)((double)srcROI_.width * xFactor_);
	dstSize.height = (int)((double)srcROI_.height * yFactor_);
	return dstSize;
}

IppiSize Resize::GetDestSize(IppiSize srcSize) const
{
	IppiSize dstSize;
	if (isSrcROI_)
	{
		dstSize.width = IPP_MIN(srcSize.width-srcROI_.x, srcROI_.width) * xFactor_;
		dstSize.height = IPP_MIN(srcSize.height-srcROI_.x, srcROI_.height) * yFactor_;

	}
	else
	{
		dstSize.width = (int)((double)srcSize.width * xFactor_);
		dstSize.height = (int)((double)srcSize.height * yFactor_);
	}

	return dstSize;
}

IppiSize Resize::GetSrcSize() const
{
	IppiSize srcSize = {0,0};
	if (!isSrcROI_) return srcSize;
	srcSize.width = (int)((double)dstROI_.width / xFactor_);
	srcSize.height = (int)((double)dstROI_.height / yFactor_);
	return srcSize;
}

IppiSize Resize::GetSrcSize(IppiSize dstSize) const
{
	IppiSize srcSize;
	if (isDstROI_)
	{
		if (dstSize.width > dstROI_.width)
			srcSize.width = dstROI_.width;
		else
			srcSize.width = dstSize.width;
		if (dstSize.height > dstROI_.height)
			srcSize.height = dstROI_.height;
		else
			srcSize.height = dstSize.height;
		srcSize.width = (int)((double)srcSize.width / xFactor_);
		srcSize.height = (int)((double)srcSize.height / yFactor_);
	}
	else
	{
		srcSize.width = (int)((double)dstSize.width / xFactor_);
		srcSize.height = (int)((double)dstSize.height / yFactor_);
	}

	return srcSize;
}

int Resize::Go(const CIppImage* pSrc, CIppImage* pDst)
{
	int channelCount = pSrc->NChannels();
	if (channelCount != pDst->NChannels() || (channelCount < 1 || channelCount > 4)) 
	{
		return -1;
	}

	if (!isDstROI_) {
		dstXOffset_ = dstYOffset_ = 0;
		dstROI_.x = dstROI_.y = 0;
		dstROI_.width = pDst->Size().width;
		dstROI_.height = pDst->Size().height;
	}
	if (!isSrcROI_) {
		srcROI_.x = srcROI_.y = 0;
		srcROI_.width = pSrc->Size().width;
		srcROI_.height = pSrc->Size().height;
	}

	int bufferSize;
	Ipp8u *pBuffer;
	ippiResizeGetBufSize(srcROI_, dstROI_, channelCount, interp_, &bufferSize);
	pBuffer = ippsMalloc_8u(bufferSize);

	IppStatus st;
	if (channelCount == 1)
	{
		st = ippiResizeSqrPixel_8u_C1R(pSrc->DataPtr(),
			pSrc->Size(), pSrc->Step(), srcROI_,
			pDst->DataPtr(),
			pDst->Step(), dstROI_,
			xFactor_, yFactor_, xCenter_, yCenter_, interp_, pBuffer);
	}
	else if (channelCount == 3)
	{
		st = ippiResizeSqrPixel_8u_C3R(pSrc->DataPtr(),
			pSrc->Size(), pSrc->Step(), srcROI_,
			pDst->DataPtr(),
			pDst->Step(), dstROI_,
			xFactor_, yFactor_, xCenter_, yCenter_, interp_, pBuffer);
	}
	else if (channelCount == 4)
	{
		st = ippiResizeSqrPixel_8u_C4R(pSrc->DataPtr(),
			pSrc->Size(), pSrc->Step(), srcROI_,
			pDst->DataPtr(),
			pDst->Step(), dstROI_,
			xFactor_, yFactor_, xCenter_, yCenter_, interp_, pBuffer);
	}
	
	if (NULL != pBuffer) 
	{
		ippsFree(pBuffer);
	}

	return ((int)st);
}