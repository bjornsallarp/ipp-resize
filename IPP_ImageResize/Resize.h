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
#include "ipp.h"
#include "ippimage.h"

class Resize
{
private:


public:
		double xFactor_, yFactor_;
	int interp_;
	IppiRect srcROI_;
	IppiRect dstROI_;
	int dstXOffset_, dstYOffset_;
	int isSrcROI_, isDstROI_;

	double xCenter_, yCenter_;
	int isCenter_;

	Resize();
	~Resize();

	enum { NN, LINEAR, CUBIC, SUPERSAMPLE, LANCZOS, LANCZOS_SMOOTH };
	void SetInterpType(int type);
	int GetInterpType() const;

	void SetFactors(double xFactor, double yFactor);
	void SetFactors(IppiSize srcSize, IppiSize dstSize);

	void SetSrcROI(IppiRect srcROI);
	void ClearSrcROI() {isSrcROI_ = 0;}
	void SetDestROI(IppiRect dstROI);
	void ClearDestROI() {isDstROI_ = 0;}

	void SetCenter(double xCenter, double yCenter);
	void ClearCenter() { isCenter_ = 0; }

	IppiSize GetDestSize(IppiSize srcSize) const;
	IppiSize GetDestSize() const;
	IppiSize GetSrcSize(IppiSize dstSize) const;
	IppiSize GetSrcSize() const;

	int Go(const CIppImage *pSrc, CIppImage *pDst);
};
