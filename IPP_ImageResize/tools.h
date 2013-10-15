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

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "ippi.h"
#include <windows.h>

#if defined( __cplusplus )
extern "C" {
#endif

/// not all bmp-files reader.
/// USE the free function for freeing
BITMAPINFOHEADER* ipLoad( const char* fname );

struct DrawingSettings
{
   int thickness1, thickness2;
   int style1, style2;
   COLORREF color1, color2;
   int gridFlag, axesFlag;
   COLORREF gridColor, axesColor;
};

const DrawingSettings defaultSettings =
{
	1,1,
	PS_SOLID, PS_SOLID,
//	0x00606060, 0x00000000,
	0x00000000, 0x00000000,
	1,1,
	0x00bFbFbF, 0x007F7F7F
};

/// draw 2D data
HANDLE ipView_8u_C3R(const Ipp8u* pData, int step, IppiSize roiSize, 
					 const char* title, int isModal );
HANDLE ipView_8u_C1R(const Ipp8u* pData, int step, IppiSize roiSize,
					 const char* title, int isModal );
int ipViewVol_8u_C3R(const Ipp8u* pData, int step, IppiSize roiSize, 
					 const char* title, int isModal );
int ipViewVol_8u_C1R(const Ipp8u* pData, int step, IppiSize roiSize,
					 const char* title, int isModal );
int ipViewVolUpdate( int threadID );

/// draw one or two curves with points connected by line

/// y range of the picture is defined by signal's min and max
HANDLE spView_16s( const short* signal, int len, const char* caption,
				   int isModal, DrawingSettings settings = defaultSettings );
HANDLE spView_32f( const float* signal, int len, const char* caption,
				   int isModal, DrawingSettings settings = defaultSettings );
HANDLE spViewX_32f( const float* signal, int len, const char* caption, 
	float xmin, float xmax, int isModal,
	DrawingSettings settings = defaultSettings );
/// y range of the picture is defined by the given min and max
HANDLE spViewY_32f( const float* signal, int len, const char* caption, 
	float ymin, float ymax, int isModal,
	DrawingSettings settings = defaultSettings );
HANDLE spViewXY_32f( const float* signal, int len, const char* caption, 
	float xmin, float xmax, float ymin, float ymax, int isModal,
	DrawingSettings settings = defaultSettings );

/// y range of the picture is defined by signals' min and max
HANDLE spView2_16s( const short* asig, int alen, const short* bsig, int blen, 
	const char* caption, int isModal,
	DrawingSettings settings = defaultSettings );
HANDLE spView2_32f( const float* asig, int alen, const float* bsig, int blen, 
	const char* caption, int isModal,
	DrawingSettings settings = defaultSettings );
HANDLE spView2X_32f( const float* asig, int alen, const float* bsig, int blen, 
	const char* caption, float xmn, float xmx, int isModal,
	DrawingSettings settings = defaultSettings );
/// y range of the picture is defined by the given min and max
HANDLE spView2Y_32f( const float* asig, int alen, const float* bsig, int blen, 
	const char* caption, float ymn, float ymx, int isModal,
	DrawingSettings settings = defaultSettings );
/// note that the given x range defines x axis grid title
HANDLE spView2XY_32f( const float* asig, int alen, const float* bsig,
	int blen, const char* caption, float xmn, float xmx, float ymn, float ymx,
	int isModal, DrawingSettings settings = defaultSettings );

__int64 getClocks( void );

/* //////////////////////////////////////////////////////////////////////
// Name:       getKeyArg
// Purpose:    get string from command line which corresponds to key
// Context:    parameters of applications are set by using command line
// Returns:    ptr to an argument string or NULL
// Parameters: 
      int argc          - number of parameters
      char** argv       - parameters
      const char* key   - key for parameter
//
//  Notes:     used by the getIntArg, getStrArg functions
*/

char* getKeyArg( int argc, char** argv, const char* key );

/* //////////////////////////////////////////////////////////////////////
// Name:       getIntArg, getStrArg
// Purpose:    get value from command line with a key
// Context:    parameters of applications are set by using command line
// Returns:    Fail as a rule
// Parameters: 
      int argc          - number of parameters
      char** argv       - parameters
      const char* key   - key for parameter
      int* value        - value to be returned
      char* value       - value to be returned
//
//  Notes:     Use getStrArg for string parameter getting
               please use as one string, for example, "-n2000"
*/
int getIntArg( int argc, char** argv, const char* key, int* value );
int getStrArg( int argc, char** argv, const char* key, char* value );
int getFloatArg( int argc, char** argv, const char* key, float* value );

int isKeyUsed( int argc, char** argv, const char* key );


/// windowing function prototype
typedef IppStatus (__stdcall *Win32fI)( Ipp32f*, int );
/// this is a rectangle window. Dummy function
IppStatus __stdcall winNone( Ipp32f*buf, int len );

/// compute FIR coefficients with use the simple windowing method
int genLowpassFir32f( Ipp32f *fir, int len, float rfreq, Win32fI win );

/// print several items with the prompt msg
/// if status is not ok error or warning message will be printed as well
void printf_32f( const char* msg, Ipp32f* buf, int len, IppStatus st );


#if defined( __cplusplus )
}
#endif

#endif // __TOOLS_H__