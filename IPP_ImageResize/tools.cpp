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

/*
//
//               INTeL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2001 Intel Corporation. All Rights Reserved.
//
//  VSS
//    $Workfile:: tools.cpp             $
//    $Revision:: 2                     $
//    $Modtime:: 2/11/01 3:39p          $
//
//  Created: Sut 10-Feb-2001 15:32 by chan
//
*/
#include "stdafx.h"

#pragma warning(disable:4786)
#include <string>
#include <stdexcept>
#include <fstream>
using namespace std;

#include <ipp.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "tools.h"

typedef enum { Error = -1, Fail = 0, Good = 1 } ipStatus;

static const char* g_imgErrStr = "Error drawing IPP image";
static const char* g_vecErrStr = "Invalid data to draw";
static const char* g_memErrStr = "Memory allocation error";
static const char* g_thdErrStr = "Error allocating thread data";
static const char* g_name      = "IPP Viewer";

static char g_strbuf[256];

/// class to simplify a work with WIN32 CriticalSection
/// Now the following declaration is needed only to
/// execute Enter and Leave procedures. Example of using
///   CritSect cs( a_section_ptr );

class CritSect {
   CRITICAL_SECTION* m_section;
public:
   CritSect( CRITICAL_SECTION* section )  : m_section(section) {
      EnterCriticalSection( m_section ); 
   }
   ~CritSect() { LeaveCriticalSection( m_section ); }
};

struct IppiImage {
   Ipp8u*   pData;
   int      step;
   IppiSize roi;
   int      planes;
};

static const int spWidth   = 400;
static const int spHeight  = 256;
static const int XGRD      = 4;
static const int YGRD      = 4;

struct IppsVector {
   float *pData, *pData2;
   int len, len2, xmin, xmax;
   float ymin, ymax, xaxismin, xaxismax;
   DrawingSettings settings;
   bool initAlloc( int ln, int ln2 ) {
      if( 0 == ( pData = ippsMalloc_32f( len = ln ) ) ) return false;
      pData2 = ln2 > 0 ? ippsMalloc_32f( len2 = ln2 ) : 0;
      return true;
   }
   bool setMinMax( float mn, float mx) {
      if( mx > mn ) {
         ymin = mn;
         ymax = mx;
         return true;
      }
      if( !pData ) return false;
      ippsMin_32f( pData, len, &ymin );
      ippsMax_32f( pData, len, &ymax );
      if( pData2 ) {
         float mn, mx;
         ippsMin_32f( pData2, len2, &mn );
         ippsMax_32f( pData2, len2, &mx );
         ymin = min( mn, ymin );
         ymax = max( mx, ymax );
      }
      return ymax > ymin;
   }
};

ipStatus ipDraw( HDC hdc, IppiImage* img );
ipStatus spDraw( HDC hdc, IppsVector* vec, int flag);

/// Thread Data item

struct ThreadData {
   DWORD id;                           /// thread-owner identificator
   bool isModalView;                   /// is modal or thread viewer
   int flag;
   bool drawImage;
   IppiImage img;                      /// image to draw
   IppsVector vec;
   char text[128];                     /// text about IplImage image
   HWND hwnd;
   int vol;								// volatile - do not free memory
   /// non standard copy operator
   void operator=( const ThreadData & thd ) {
      id = thd.id;
      isModalView = thd.isModalView;
      drawImage = thd.drawImage;
	  flag = thd.flag;
      img = thd.img;
      vec = thd.vec;
	  hwnd = thd.hwnd;
	  vol = thd.vol;
	  lstrcpyA( text, thd.text );
   }
};

/// Array of thread data

class ThreadDataArray {

   enum { NUMOF = 10 };                /// num of threads allowed

   ThreadData m_data[ NUMOF ];         /// data of threads
   CRITICAL_SECTION m_cs_data;         /// its critical section
   
   int m_numofThreads;                 /// current num of threads
   CRITICAL_SECTION m_cs_numof;        /// its critical section

public:
   /// ctr inits crit sects, event, id's
   ThreadDataArray();
   /// dtr dels crit sects, close event
   ~ThreadDataArray() {
      CritSect csn( &m_cs_numof );
      if( m_numofThreads <= 0 ) {
         ::DeleteCriticalSection( &m_cs_numof );
         ::DeleteCriticalSection( &m_cs_data );
         ::CloseHandle( m_hevent );
      }
   }
   /// find id thread data, id may be 0
   ThreadData* operator[] (DWORD id);
   /// find id thread, del img, dec counter, clear id
   bool close( DWORD idthread );
   /// find id=0, copy thread data, inc counter
   bool open ( const ThreadData& thd );
   /// check if no more place
   bool isfull() {
      CritSect csn( &m_cs_numof );
      return m_numofThreads >= NUMOF;
   }
   /// num of threads which are being executed
   int numof() {
      CritSect csn( &m_cs_numof );
      return m_numofThreads;
   }

   bool closeNonModal();
   HANDLE m_hevent;                    /// event to wait for dialog inited
};

/// Create two critical sections and event
ThreadDataArray::ThreadDataArray() : m_numofThreads(0) {

   InitializeCriticalSection( &m_cs_data );
   InitializeCriticalSection( &m_cs_numof );

   /// security_attr=NULL, manual_reset=FALSE, init_state=busy=FALSE
   m_hevent = ::CreateEventA( NULL, FALSE, FALSE, "Thread Data Array Event");

   for( int i=0; i<NUMOF; ++i ) m_data[i].id = 0;
}

/// id may be 0 as well, then a free cell is found
ThreadData* ThreadDataArray::operator[] ( DWORD id ) {

   CritSect csd( &m_cs_data );
   for( int i=0; i<NUMOF; ++i ) 
      if( m_data[i].id == id ) return &m_data[i];
   return NULL;
}

/// find id thread, delete img, clear id, dec counter
bool ThreadDataArray::close( DWORD idthread ) {

   CritSect csd( &m_cs_data );
   for( int i=0; i<NUMOF; ++i ) 
      if( m_data[i].id == idthread ) {
         //iplDeallocate( m_data[i].img, IPL_IMAGE_ALL );
		 if (!m_data[i].vol)
		 {
			 if ( m_data[i].img.pData ) ippsFree( m_data[i].img.pData );
			 if ( m_data[i].vec.pData ) ippsFree( m_data[i].vec.pData );
			 if ( m_data[i].vec.pData2 ) ippsFree( m_data[i].vec.pData2 );
		 }
         CritSect csn( &m_cs_numof );
         m_data[i].id = 0;
         --m_numofThreads;
         return true;
      }
   return false;
}

bool ThreadDataArray::closeNonModal() {
	for( int i=0; i<NUMOF; ++i ) 
	{
		if (m_data[i].id != 0)
		{
			HWND hdlg = m_data[i].hwnd;
			close(m_data[i].id);
			EndDialog( hdlg, 0 );
		}
        return true;
      }
   return false;
}

/// find 0 id, copy thread data, inc counter
bool ThreadDataArray::open( const ThreadData& thd ) {

   CritSect csd( &m_cs_data );
   for( int i=0; i<NUMOF; ++i ) 
      if( 0 == m_data[i].id ) {
         CritSect csn( &m_cs_numof );
         m_data[i] = thd;
         ++m_numofThreads;
         return true;
      }
   return true;
}

/// Global thread data array
static ThreadDataArray g_threads;

/* //////////////////////////////////////////////////////////////////////
// Name:       ViewProc
// Purpose:    Dialog Procedure
// Context:    IPLib viewer
// Returns:    TRUE as a rule
// Parameters: 
      HWND hdlg - handle of dialog window
      UINT message - message code
      WPARAM wParam - commnad code
      LPARAM - not used
//
//  Notes:

*/

static LRESULT CALLBACK ViewProc( HWND hdlg, UINT message, WPARAM wParam, LPARAM ) {

   switch( message ) {
      
   case WM_INITDIALOG : {

      ThreadData *threadData = g_threads[ ::GetCurrentThreadId() ];
	  threadData->hwnd = hdlg;

      assert( NULL != threadData );

      int offset = g_threads.numof() * 15;

      /// Resize and move dialog window
      RECT rect = {0,0,0,0};
      if( threadData->drawImage ) {
         rect.right = threadData->img.roi.width;
         rect.bottom = threadData->img.roi.height;
      } else {
         rect.right = spWidth;
         rect.bottom = spHeight;
      }

      /// define window size if client region size is given
      ::AdjustWindowRect(
         &rect,                              // pointer to client-rect structure
         GetWindowLong( hdlg, GWL_STYLE ),   // window styles
         FALSE                               // menu-present flag
      );
      /// window position depends on viewing mode.
      /// The dialog style has DS_CENTER bit, so the place of
      /// the dialog window is center if mode is modal
      /// It doesn't work if Borland is used
      ::SetWindowPos( hdlg, HWND_TOPMOST, offset, offset, 
         rect.right - rect.left, rect.bottom - rect.top, 
         SWP_NOZORDER | (threadData->isModalView ? SWP_NOMOVE : 0 ));

      /// Set window caption
      ::SetWindowTextA( hdlg, threadData->text );
      /// signal that dialog has been initialized 
      ::SetEvent( g_threads.m_hevent );
      return TRUE;
   }

   case WM_CHAR :
   case WM_COMMAND :
   {
//      if (GetKeyState(VK_MENU) < 0)
//         switch (wParam) {
//         case 'c':
		 ThreadData *threadData = g_threads[ ::GetCurrentThreadId() ];
		 threadData->hwnd = hdlg;
	     assert( NULL != threadData );

		 int w,h;
		 if( threadData->drawImage ) {
			 w = threadData->img.roi.width;
			 h = threadData->img.roi.height;
			 IppiImage* img = &threadData->img;
//			 HBITMAP hBitmap = CreateCompatibleBitmap( hdcMem, w, h );
//			 SelectObject(hdcMem, hBitmap);
//			 BitBlt(hdcMem,0,0,w,h, hdc, 0,0, SRCCOPY);

//			 BITMAP bm =
//			 {0, w, h, img->step, 3, 8*img->planes,(void*)img->pData};

//			 OpenClipboard(hdlg);
//			 EmptyClipboard();
//			 SetClipboardData(CF_BITMAP, CreateBitmapIndirect(&bm));
//			 CloseClipboard();

			 if( !img || !img->pData ) return Error;

			 bool isrgb = (1 != img->planes);

			 bool isgray = (1 == img->planes);

			 if( !isgray && !isrgb ) return Fail;

			 char buf[ sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256 ];
			 BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER*)buf;
			 COLORREF* rgb = (COLORREF*)( buf + sizeof(BITMAPINFOHEADER) );

			 if( isgray ) for( int i = 0; i < 256; i++) rgb[i] = RGB( i,i,i );

			 dibhdr->biSize = sizeof( BITMAPINFOHEADER );
			 dibhdr->biWidth = w;
			 dibhdr->biHeight = h;
			 dibhdr->biPlanes = 1;
			 dibhdr->biBitCount = (DWORD)(8 * img->planes); // 1 or 3 == 8 or 24
			 dibhdr->biCompression = BI_RGB;
			 dibhdr->biSizeImage = img->step*h;
//			 dibhdr->biSizeImage = 0;
			 dibhdr->biXPelsPerMeter = 21000;
			 dibhdr->biYPelsPerMeter = 21000;
			 dibhdr->biClrUsed = 0;
			 dibhdr->biClrImportant = 0; 

			 HDC hdcMeta = CreateMetaFile(NULL);
//			 ::StretchDIBits( hdcMeta, 0,0,img->roi.width,img->roi.height,
//								0,0, img->roi.width,img->roi.height,
//								img->pData, (BITMAPINFO*)dibhdr, DIB_RGB_COLORS,
//								SRCCOPY);
			 ::SetDIBitsToDevice( hdcMeta, 0,0,img->roi.width,img->roi.height, 
								  0,0,0,img->roi.height, img->pData,
								  (BITMAPINFO*)dibhdr, DIB_RGB_COLORS );
			 HGLOBAL hGMem = GlobalAlloc(GHND, sizeof(METAFILEPICT));
			 LPMETAFILEPICT pMFP;
//			 hGMem = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER + dibhdr->SizeImage));
			 pMFP = (LPMETAFILEPICT) GlobalLock(hGMem);
//			 pMFP->mm = MM_ISOTROPIC;
			 pMFP->mm = MM_ANISOTROPIC;
			 pMFP->xExt = w;
			 pMFP->yExt = h;
			 pMFP->hMF = CloseMetaFile(hdcMeta);
			 GlobalUnlock(hGMem);

			 OpenClipboard(hdlg);
			 EmptyClipboard();
//			 SetClipboardData(CF_DIB, (BITMAPINFO*)buf);
			 SetClipboardData(CF_METAFILEPICT, hGMem);
			 CloseClipboard();

		 } else {
			 w = spWidth;
			 h = spHeight;
			 HDC hdcMeta = CreateMetaFile(NULL);
			 spDraw( hdcMeta, &threadData->vec, threadData->flag);
			 HGLOBAL hGMem;
			 LPMETAFILEPICT pMFP;
			 hGMem = GlobalAlloc(GHND, sizeof(METAFILEPICT));
			 pMFP = (LPMETAFILEPICT) GlobalLock(hGMem);
			 pMFP->mm = MM_ANISOTROPIC;
			 pMFP->xExt = w;
			 pMFP->yExt = h;
			 pMFP->hMF = CloseMetaFile(hdcMeta);
			 GlobalUnlock(hGMem);
			 OpenClipboard(hdlg);
			 EmptyClipboard();
			 SetClipboardData(CF_METAFILEPICT, hGMem);
			 CloseClipboard();
		 }
		  EndDialog( hdlg, 0 );
		  g_threads.close( ::GetCurrentThreadId() );
		  g_threads.closeNonModal();
//	  }
	  return TRUE;
      break;
	   }
//   case WM_COMMAND :
//     switch( wParam ) {
//       case IDOK : case IDCANCEL :
//         EndDialog( hdlg, 0 );
//         g_threads.close( ::GetCurrentThreadId() );
//         return TRUE;
//      }
//      break;

   case WM_PAINT : {

      ThreadData *threadData = g_threads[ ::GetCurrentThreadId() ];
      assert( NULL != threadData );

      PAINTSTRUCT ps;
      HDC hdc = ::BeginPaint( hdlg, &ps );
      if( threadData->drawImage )
         ipDraw( hdc, &threadData->img );
      else
         spDraw( hdc, &threadData->vec, threadData->flag);
      ::EndPaint( hdlg, &ps );
      return TRUE;
   }
   
   }
   return FALSE;
}

/* //////////////////////////////////////////////////////////////////////
// Name:       ViewerThread
// Purpose:    Thread Procedure
// Context:    IPLib viewer
// Returns:    If the DialogBoxIndirectParam fails, the return value is -1
// Parameters: ThreadData* - not used
//
//  Notes:     
*/

int WINAPI ViewerThread( ThreadData* ) {

   static char buf[1024] = {0};
   static DLGTEMPLATE* dlg = (DLGTEMPLATE*)buf;

   dlg->style = DS_CENTER | WS_POPUPWINDOW | WS_CAPTION | DS_MODALFRAME |  DS_SETFOREGROUND;
   dlg->dwExtendedStyle = 0;
   dlg->cdit = 0;
   dlg->x = dlg->y = 0;
   dlg->cx = 100;
   dlg->cy = 100;

   /// create a modal dialog box from a dialog box template in memory
   ThreadData *threadData = g_threads[ ::GetCurrentThreadId() ];
   return ::DialogBoxIndirectParam( 
      ::GetModuleHandle(NULL),         // handle to application instance
      dlg,                            // identifies dialog box template
      NULL,                            // handle to owner window
      (DLGPROC)ViewProc,               // pointer to dialog box procedure
      (DWORD)0 );                 // initialization value
   
}

class bexception : public exception {
   string m_msg;
public:
   bexception( bool should_delete ) : m_should_delete( should_delete ) {}
   bexception( bool should_delete, const char* msg ) {
      m_should_delete = should_delete;
      m_msg = msg;
   }
   virtual const char *what() const throw() {
      return m_msg.c_str();
   }
   bool m_should_delete;
};

/* //////////////////////////////////////////////////////////////////////
// Name:       ippView
// Purpose:    to show images
// Context:    IPPI viewer
// Returns:    thread handle if there is not a modal view and all is ok
// Parameters: 
      const Ipp8u* pSrc - pointer to data
      const char* caption - window caption
      bool isModalView - modal viewer if true, thread waits for a
         viewer closing. Thread doesn't wait. The following viewer
         can be started.
//
*/

HANDLE ipView_8u( const Ipp8u* pData, int step, IppiSize roi,
   const char* caption, int isModalView, int nchan ) 
{
   if( !pData ) return 0;
   if( g_threads.isfull() ) {
      MessageBoxA( NULL, "Too many threads are beign executed. "
         "Please decrease number of viewers or use Modal mode.", 
         g_name, MB_OK | MB_ICONSTOP );
      return 0;
   }
   IppiImage img8u;
   HANDLE hThread = 0;
   try {
      /// Create header for the image to be drawn
      int abit      = (4 << 3) - 1;
      int widthstep = ((roi.width * 8 * nchan + abit) & ~abit)>>3;
      int imagesize = widthstep * roi.height;
      img8u.pData = ippsMalloc_8u(imagesize);
      img8u.step = step;
      img8u.roi = roi;
      img8u.planes = nchan;
      if( !img8u.pData ) throw bexception( true, g_memErrStr );
      if( 3 == nchan )
         ippiCopy_8u_C3R(pData,step,img8u.pData,widthstep,roi);
      else
         ippiCopy_8u_C1R(pData,step,img8u.pData,widthstep,roi);

      static ThreadData param;
      lstrcpyA( param.text, NULL == caption ? "IPPI image" : caption );
      param.img = img8u;
      param.isModalView = isModalView & 1;
      param.drawImage = 1;
      param.vec.pData = 0;
	  param.vol = 0;

      /// prepare for waiting
      ::ResetEvent( g_threads.m_hevent );

      hThread = CreateThread( 
         NULL,                                  // pointer to thread security attributes
         0,                                     // initial thread stack size, in bytes
         (LPTHREAD_START_ROUTINE) ViewerThread, // pointer to thread function
         (LPVOID)(0),                           // argument for new thread
         0,                                     // creation flags, runs immediately
         &param.id);                            // pointer to returned thread identifier
      
      if( !hThread ) throw bexception( true, g_thdErrStr );

      /// set thread data for thread has been started
      if( false == g_threads.open( param )) {
         ::CloseHandle( hThread );
         throw bexception( true );
      }
      if( isModalView &1) {
         /// wait for the thread will be ended
         ::WaitForSingleObject( hThread, INFINITE );
         ::CloseHandle( hThread );
         return 0;
      }
      else
         /// wait for dialog window will be initialized
         /// thread will be alive after that
         ::WaitForSingleObject ( g_threads.m_hevent, 10000 );
   }
   catch( bexception e ) {
      printf( "--err in ipView. %s\n", e.what() );
      if( e.m_should_delete) ippsFree( img8u.pData );
   }
   return hThread;
}

HANDLE ipView_8u_C3R( const Ipp8u* pData, int step, IppiSize roi,
   const char* caption, int isModalView ) 
{
   return ipView_8u( pData, step, roi, caption, isModalView, 3 );
};

HANDLE ipView_8u_C1R( const Ipp8u* pData, int step, IppiSize roi,
   const char* caption, int isModalView ) 
{
   return ipView_8u( pData, step, roi, caption, isModalView, 1 );
};

int ipViewVolUpdate( int threadID) 
{
	ThreadData *threadData = g_threads[ threadID ];
//	if (threadData) SendMessage(threadData->hwnd, WM_COMMAND, IDOK,  0);
//	if (threadData) SendMessage(threadData->hwnd, WM_PAINT, 0, 0);
	if (threadData) { InvalidateRect(threadData->hwnd, NULL, FALSE);
		UpdateWindow(threadData->hwnd); return 1; }
	else return 0;
}

int ipViewVol_8u( const Ipp8u* pData, int step, IppiSize roi,
				  const char* caption, int isModalView, int nchan ) 
{
	if( !pData ) return 0;
	if( g_threads.isfull() ) {
		MessageBoxA( NULL, "Too many threads are beign executed. "
					"Please decrease number of viewers or use Modal mode.", 
					g_name, MB_OK | MB_ICONSTOP );
		return 0;
	}
	IppiImage img8u;
	HANDLE hThread = 0;
	int id = 0;
	try {
	  /// Create header for the image to be drawn
		int abit      = (4 << 3) - 1;
		int widthstep = ((roi.width * 8 * nchan + abit) & ~abit)>>3;
		int imagesize = widthstep * roi.height;
//		img8u.pData = ippsMalloc_8u(imagesize);
		img8u.pData = (unsigned char*)pData;
		img8u.step = step;
		img8u.roi = roi;
		img8u.planes = nchan;
		if( !img8u.pData ) throw bexception( true, g_memErrStr );
//		if( 3 == nchan )
//			ippiCopy_8u_C3R(pData,step,img8u.pData,widthstep,roi);
//		else
//			ippiCopy_8u_C1R(pData,step,img8u.pData,widthstep,roi);

		static ThreadData param;
		lstrcpyA( param.text, NULL == caption ? "IPPI image" : caption );
		param.img = img8u;
		param.isModalView = isModalView & 1;
		param.drawImage = 1;
		param.vec.pData = 0;
		param.vol = 1;

	  /// prepare for waiting
		::ResetEvent( g_threads.m_hevent );

		hThread = CreateThread( 
							   NULL,                                  // pointer to thread security attributes
							   0,                                     // initial thread stack size, in bytes
							   (LPTHREAD_START_ROUTINE) ViewerThread, // pointer to thread function
							   (LPVOID)(0),                           // argument for new thread
							   0,                                     // creation flags, runs immediately
							   &param.id);                            // pointer to returned thread identifier

		id = param.id;
		if( !hThread ) throw bexception( true, g_thdErrStr );

	  /// set thread data for thread has been started
		if( false == g_threads.open( param )) {
			::CloseHandle( hThread );
			throw bexception( true );
		}
		if( isModalView & 1 ) {
		 /// wait for the thread will be ended
			::WaitForSingleObject( hThread, INFINITE );
			::CloseHandle( hThread );
			return 0;
		}
		else
		 /// wait for dialog window will be initialized
		 /// thread will be alive after that
			::WaitForSingleObject ( g_threads.m_hevent, 10000 );
	}
	catch( bexception e ) {
		printf( "--err in ipView. %s\n", e.what() );
		if( e.m_should_delete) ippsFree( img8u.pData );
	}
	return id;
}

int ipViewVol_8u_C3R( const Ipp8u* pData, int step, IppiSize roi,
					  const char* caption, int isModalView ) 
{
	return ipViewVol_8u( pData, step, roi, caption, isModalView, 3 );
};

int ipViewVol_8u_C1R( const Ipp8u* pData, int step, IppiSize roi,
					  const char* caption, int isModalView ) 
{
	return ipViewVol_8u( pData, step, roi, caption, isModalView, 1 );
};


/* //////////////////////////////////////////////////////////////////////
// Name:       ipDraw
// Purpose:    draw 8u pixel IPLib images
// Context:    IPLib viewer
// Returns:    Error if NULL, Fail if wrong parameters
// Parameters: 
      HDC hdc - defines device context
      IplImage* - IPLib image to draw 
//
//  Notes: 
      supports
         1 channel, IPL_DEPTH_8U  with color mode "GRAY"
         3,4 channels, IPL_DEPTH_8U with color mode "RGB" or "RGBA"
*/

static ipStatus ipDraw( HDC hdc, IppiImage* img ) {

   if( !img || !img->pData ) return Error;
   
   bool isrgb = (1 != img->planes);
   
   bool isgray = (1 == img->planes);

   if( !isgray && !isrgb ) return Fail;

   char buf[ sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256 ];
   BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER*)buf;
   COLORREF* rgb = (COLORREF*)( buf + sizeof(BITMAPINFOHEADER) );

   if( isgray ) for( int i = 0; i < 256; i++) rgb[i] = RGB( i,i,i );

   dibhdr->biSize = sizeof( BITMAPINFOHEADER );
   dibhdr->biWidth = img->roi.width;
   dibhdr->biHeight = img->roi.height;
   dibhdr->biPlanes = 1;
   dibhdr->biBitCount = (DWORD)(8 * img->planes/*1 3*/);
   dibhdr->biCompression = BI_RGB;
   dibhdr->biSizeImage = img->roi.width*img->roi.height;
   dibhdr->biXPelsPerMeter = 0;
   dibhdr->biYPelsPerMeter = 0;
   dibhdr->biClrUsed = 0;
   dibhdr->biClrImportant = 0; 

   ::SetDIBitsToDevice( hdc, 0,0,img->roi.width,img->roi.height, 
      0,0,0,img->roi.height, img->pData, (BITMAPINFO*)dibhdr, DIB_RGB_COLORS );

   return Good;
}

/* //////////////////////////////////////////////////////////////////////
// Name:       ipLoad
// Purpose:    load bmp-file
// Context:    IPLib tutorial
// Returns:    bmp
// Parameters: fname - bmp-file name to read
//
//  Notes:     
*/

BITMAPINFOHEADER* ipLoad( const char* fname ) {

   if( !fname ) return NULL;
   
   BITMAPINFOHEADER* infohdr = NULL;

   ifstream fsrc;

   try {
      BITMAPFILEHEADER filehdr;
      BITMAPINFOHEADER bmphdr;

      fsrc.open( fname, ios::in | ios::binary );
      if( fsrc.fail() ) throw runtime_error("Problem to open source file");


      fsrc.read( (char*)&filehdr, sizeof(BITMAPFILEHEADER) );
      if( fsrc.fail() ) throw runtime_error("Problem to read bmp-file header from source file");

      if( 0x4d42 != filehdr.bfType ) throw runtime_error("Wrong type of source file");

      fsrc.read( (char*)&bmphdr, sizeof(BITMAPINFOHEADER) );
      if( fsrc.fail() ) throw runtime_error("Problem to read bmp-header from source file");

      if( 0 == bmphdr.biSizeImage )
         bmphdr.biSizeImage =
            (((( bmphdr.biWidth * bmphdr.biBitCount ) + 31) & ~31) >> 3) * bmphdr.biHeight;

      if( 0 == bmphdr.biClrUsed ) {
         if( BI_BITFIELDS == bmphdr.biCompression )
            bmphdr.biClrUsed = 3;
         else 
            bmphdr.biClrUsed = bmphdr.biBitCount < 16 ? 1 << bmphdr.biBitCount : 0;
      }

      int colorbytes = sizeof(RGBQUAD) * bmphdr.biClrUsed;
      int totalbytes = sizeof(BITMAPINFOHEADER) + colorbytes + bmphdr.biSizeImage;

      infohdr = (BITMAPINFOHEADER*)malloc( totalbytes );
      if( !infohdr ) throw runtime_error("Problem to allocate memory for DIB data");
      
      char* quads = (char*)infohdr + sizeof(BITMAPINFOHEADER);
      char* pixels = (char*)quads + colorbytes;

      ippsCopy_8u( (Ipp8u*)&bmphdr, (Ipp8u*)infohdr, sizeof( bmphdr) );

      fsrc.read( (char*)quads, colorbytes );
      fsrc.read( (char*)pixels, bmphdr.biSizeImage );

      fsrc.close();
   }
   catch( runtime_error e ) {
      if( infohdr ) { 
         free( infohdr );
         infohdr = 0;
      }
   }

   return infohdr;
}

__int64 getClocks( void ) {
   __int64 cnt;
   __asm rdtsc
   __asm mov dword ptr cnt, eax
   __asm mov dword ptr cnt+4, edx
   return cnt;
}

HANDLE spView_16s( const short* signal, int len, const char* caption, int isModal,
				  DrawingSettings settings) {
	float* signal32f = ippsMalloc_32f(len);
	ippsConvert_16s32f(signal, signal32f, len);
	HANDLE h = spViewXY_32f( signal32f, len, caption, 0,0, 0,0, isModal, settings);
	ippsFree(signal32f);
	return h;
}
HANDLE spView_32f( const float* signal, int len, const char* caption, int isModal,
				  DrawingSettings settings) {
   return spViewXY_32f( signal, len, caption, 0,0, 0,0, isModal, settings );
}
HANDLE spViewX_32f( const float* signal, int len, const char* caption,
	float xmn, float xmx, int isModal, DrawingSettings settings)
{
   return spViewXY_32f( signal, len, caption, xmn,xmx, 0,0, isModal,
						settings );
}
HANDLE spViewY_32f( const float* signal, int len, const char* caption,
   float ymn, float ymx, int isModal, DrawingSettings settings)
{
   return spViewXY_32f( signal, len, caption, 0,0, ymn,ymx, isModal,
					  settings);
}
HANDLE spViewXY_32f( const float* signal, int len, const char* caption,
	float xmn, float xmx, float ymn, float ymx, int isModal,
	DrawingSettings settings) 
{
   if( !signal || len < 4 ) return 0;

   if( g_threads.isfull() ) {
      MessageBoxA( NULL, "Too many threads are beign executed. "
         "Please decrease number of viewers or use Modal mode.", 
         g_name, MB_OK | MB_ICONSTOP );
      return 0;
   }

   HANDLE hThread = 0;
   static ThreadData param;

   try {
      param.vec.xmin = 0;  param.vec.xmax = len;
      if( xmx > xmn ) {
         param.vec.xaxismin = xmn;  param.vec.xaxismax = xmx;
      } else {
         param.vec.xaxismin = param.vec.xmin;
         param.vec.xaxismax = param.vec.xmax;
      }
      if( !param.vec.initAlloc( len, 0 ) ) 
         throw bexception( false, g_memErrStr );
      ippsCopy_32f( signal, param.vec.pData, len );
      if( !param.vec.setMinMax( ymn, ymx) ) 
         throw bexception( true, "Problem to find min and max values" );
      lstrcpyA( param.text, NULL == caption ? "IPPS signal" : caption );
      param.isModalView = isModal & 1;
	  param.flag = isModal & ~1;
      param.drawImage = 0;
      param.img.pData = 0;
	  param.vol = 0;
	  param.vec.settings = settings;

      /// prepare for waiting
      ::ResetEvent( g_threads.m_hevent );

      hThread = CreateThread( 
         NULL,                                  // pointer to thread security attributes
         0,                                     // initial thread stack size, in bytes
         (LPTHREAD_START_ROUTINE) ViewerThread, // pointer to thread function
         (LPVOID)(0),                           // argument for new thread
         0,                                     // creation flags, runs immediately
         &param.id);                            // pointer to returned thread identifier
      
      if( !hThread ) throw bexception( true, g_thdErrStr );

      /// set thread data for thread has been started
      if( false == g_threads.open( param )) {
         ::CloseHandle( hThread );
         throw bexception( true );
      }
      
      if( isModal & 1) {
         /// wait for the thread will be ended
         ::WaitForSingleObject( hThread, INFINITE );
         ::CloseHandle( hThread );
         return 0;
      }
      else
         /// wait for dialog window will be initialized
         /// thread will be alive after that
         ::WaitForSingleObject ( g_threads.m_hevent, 10000 );
   }
   catch( bexception e ) {
      printf( "--err in spView. %s\n", e.what() );
      if( e.m_should_delete) ippsFree( param.vec.pData );
   }

   return hThread;
}

HANDLE spView2_16s( const short* asig, int alen, const short* bsig, int blen, 
   const char* caption, int isModal, DrawingSettings settings)
{
	float* asig32f = ippsMalloc_32f(alen);
	float* bsig32f = ippsMalloc_32f(blen);
	ippsConvert_16s32f(asig, asig32f, alen);
	ippsConvert_16s32f(bsig, bsig32f, blen);
	HANDLE h = spView2XY_32f( asig32f, alen, bsig32f,
		blen, caption, 0,0, 0,0, isModal, settings );
	ippsFree(asig32f);
	ippsFree(bsig32f);
	return h;
}
HANDLE spView2_32f( const float* asig, int alen, const float* bsig, int blen, 
	const char* caption, int isModal, DrawingSettings settings)
{
   return spView2XY_32f( asig, alen, bsig, blen, caption, 0.0,0.0, 0.0,0.0,
		isModal, settings);
}
HANDLE spView2X_32f( const float* asig, int alen, const float* bsig, int blen, 
   const char* caption, float xmn, float xmx, int isModal,
	DrawingSettings settings)
{
	return spView2XY_32f( asig, alen, bsig, blen, caption, xmn,xmx, 0.0,0.0,
		isModal, settings );
}
HANDLE spView2Y_32f( const float* asig, int alen, const float* bsig, int blen, 
	const char* caption, float ymn, float ymx, int isModal,
	DrawingSettings settings)
{
	return spView2XY_32f( asig, alen, bsig, blen, caption, 0.0,0.0,
		ymn,ymx, isModal, settings );
}
HANDLE spView2XY_32f( const float* asig, int alen, const float* bsig,
	int blen, const char* caption, float xmn, float xmx,
	float ymn, float ymx, int isModal, DrawingSettings settings)
{
   if( !asig || alen < 4 ) return 0;
   if( !bsig || blen < 4 ) return 0;

   if( g_threads.isfull() ) {
      MessageBoxA( NULL, "Too many threads are beign executed. "
         "Please decrease number of viewers or use Modal mode.", 
         g_name, MB_OK | MB_ICONSTOP );
      return 0;
   }

   HANDLE hThread = 0;
   static ThreadData param;

   try {
      param.vec.xmin = 0;  param.vec.xmax = max( alen, blen );
      if( xmx > xmn ) {
         param.vec.xaxismin = xmn;  param.vec.xaxismax = xmx;
      } else {
         param.vec.xaxismin = param.vec.xmin;
         param.vec.xaxismax = param.vec.xmax;
      }
      if( !param.vec.initAlloc( alen, blen ) ) throw bexception( true, g_memErrStr );
      ippsCopy_32f( asig, param.vec.pData, alen );
      ippsCopy_32f( bsig, param.vec.pData2, blen );
      if( !param.vec.setMinMax( ymn, ymx) ) 
         throw bexception( true, "Problem to find min and max values" );
      lstrcpyA( param.text, NULL == caption ? "IPPS signal" : caption );
      param.isModalView = isModal & 1;
      param.drawImage = 0;
      param.img.pData = 0;
	  param.vol = 0;
	  param.vec.settings = settings;

      /// prepare for wainting
      ::ResetEvent( g_threads.m_hevent );

      hThread = CreateThread( 
         NULL,                                  // pointer to thread security attributes
         0,                                     // initial thread stack size, in bytes
         (LPTHREAD_START_ROUTINE) ViewerThread, // pointer to thread function
         (LPVOID)(0),                           // argument for new thread
         0,                                     // creation flags, runs immediately
         &param.id);                            // pointer to returned thread identifier
      
      if( !hThread ) throw bexception( true );

      /// set thread data for thread has been started
      if( false == g_threads.open( param )) {
         ::CloseHandle( hThread );
         throw bexception( true, g_thdErrStr );
      }
      
      if( isModal & 1 ) {
         /// wait for the thread will be ended
         ::WaitForSingleObject( hThread, INFINITE );
         ::CloseHandle( hThread );
         return 0;
      }
      else
         /// wait for dialog window will be initialized
         /// thread will be alive after that
         ::WaitForSingleObject ( g_threads.m_hevent, 10000 );
   }
   catch( bexception e ) {
      printf( "--err in spView2. %s\n", e.what() );
      if( e.m_should_delete) {
         if( param.vec.pData ) ippsFree( param.vec.pData );
         if( param.vec.pData2 ) ippsFree( param.vec.pData2 );
      }
   }
   return hThread;
}

/// r = a + b * v
class LinApprox {
   float m_a, m_b;
public:
   LinApprox( float a, float b ) : m_a( a ), m_b( b ) {}
   LinApprox( float rmax, float rmin, float vmax, float vmin ) {
      float tmp = vmax - vmin;
      m_b = tmp ? ( rmax - rmin ) / tmp : 0;
      m_a = rmax - m_b * vmax;
   }
   float operator () ( float x ) { return m_a + m_b * x; }
};

struct GText {
   HDC m_hdc;
   HFONT m_oldfont;
   COLORREF m_oldTextColor;
   GText( HDC hdc, int height, COLORREF color ) : m_hdc( hdc ) {
      LOGFONT lf = { 0 };
      lf.lfHeight = height;
      m_oldfont = (HFONT)::SelectObject( m_hdc, ::CreateFontIndirect( &lf ) );
      m_oldTextColor = ::SetTextColor( m_hdc, color );
   }
   ~GText() {
      ::SelectObject( m_hdc, m_oldfont );
      ::SetTextColor( m_hdc, m_oldTextColor );
   }
   void draw( const char* txt, int x, int y, int align ) {
      ::SetTextAlign( m_hdc, align );
      ::TextOutA( m_hdc, x, y, txt, strlen( txt ) );
   }
};

static int INSIDEY = 15;

static void drawAsLine( float* vec, int len, HDC hdc, LinApprox& appx, 
   LinApprox& appy, int color, int thickness, int style )
{
   if( !vec ) return;
   HGDIOBJ pen = ::SelectObject( hdc, CreatePen( style, thickness, color ) );
   ::MoveToEx( hdc, appx(0), appy( vec[0] ), 0 );
   for( int n=1; n<len; n++) ::LineTo( hdc, appx( n ), appy( vec[n] ) );
   ::SelectObject( hdc, pen );
}

static ipStatus spDraw( HDC hdc, IppsVector* vec, int flag) {

   if( !vec || !vec->pData ) return Error;
      
   RECT r = { 0, 0, spWidth, spHeight };

   ::PatBlt( hdc, 0, 0, spWidth, spHeight, WHITENESS );
//   ::PatBlt( hdc, 0, 0, spWidth, spHeight, BLACKNESS );
   ::InflateRect( &r, -1, -INSIDEY );
   int dx = r.right - r.left, dy = r.bottom - r.top;
   int ddx = dx / XGRD, ddy = dy / YGRD;

   if( ddx >0 && ddy >0 ) {
      int i;
//      HGDIOBJ  gridBrush = ::SelectObject( hdc, CreateSolidBrush( 0x007F7F7F ) );
      HGDIOBJ  gridBrush =
		::SelectObject( hdc, CreateSolidBrush( vec->settings.gridColor ) );
	  if ((~flag & 2) && vec->settings.gridFlag)
	  {
		  i=1; ::PatBlt( hdc, r.left+ddx*i, r.top, 1, dy, PATCOPY );
		  i=3; ::PatBlt( hdc, r.left+ddx*i, r.top, 1, dy, PATCOPY );
		  i=0; ::PatBlt( hdc, r.left, r.top+ddy*i, dx, 1, PATCOPY );
		  i=1; ::PatBlt( hdc, r.left, r.top+ddy*i, dx, 1, PATCOPY );
		  i=3; ::PatBlt( hdc, r.left, r.top+ddy*i, dx, 1, PATCOPY );
		  i=4; ::PatBlt( hdc, r.left, r.top+ddy*i, dx, 1, PATCOPY );
	  }
      HGDIOBJ  axesBrush =
		::SelectObject( hdc, CreateSolidBrush( vec->settings.axesColor ) );
	  if ((~flag & 2) && vec->settings.axesFlag)

	  {
		  i=2; ::PatBlt( hdc, r.left+ddx*i, r.top, 1, dy, PATCOPY );
		  i=2; ::PatBlt( hdc, r.left, r.top+ddy*i, dx, 1, PATCOPY );
		  // for( i=1; i< YGRD; i++)
			//  ::PatBlt( hdc, r.left+ddx*i, r.top, 1, dy, PATCOPY );
		 // for( i=0; i<=XGRD; i++)
			  //::PatBlt( hdc, r.left, r.top+ddy*i, dx, 1, PATCOPY );
	  }

      //::SelectObject( hdc, brush );

      LinApprox appx( r.left, r.right, vec->xmin, vec->xmax );
      LinApprox appy( r.top, r.bottom, vec->ymax, vec->ymin );

//      drawAsLine( vec->pData , vec->len,  hdc, appx, appy, 0x0000FF00 );
//      drawAsLine( vec->pData2, vec->len2, hdc, appx, appy, 0x0000FFFF );
      drawAsLine( vec->pData , vec->len,  hdc, appx, appy,
				  vec->settings.color1, vec->settings.thickness1,
				  vec->settings.style1);
      drawAsLine( vec->pData2, vec->len2, hdc, appx, appy,
				  vec->settings.color2, vec->settings.thickness2,
				  vec->settings.style2);

//      COLORREF oldBk = ::SetBkColor( hdc, 0x00000000 );
      COLORREF oldBk = ::SetBkColor( hdc, 0x00FFFFFF );
      int oldMode = ::SetBkMode( hdc, OPAQUE );

//      GText txt( hdc, INSIDEY-2, 0x00F0F0F0 );
      GText txt( hdc, INSIDEY-2, 0x00F0F0F );

      sprintf_s( g_strbuf, "%g", vec->ymax );
      txt.draw( g_strbuf, r.right, r.top-2, TA_RIGHT | TA_BOTTOM );

      sprintf_s( g_strbuf, "%g", vec->ymin );
      txt.draw( g_strbuf, r.right, r.bottom+2, TA_RIGHT | TA_TOP );

      LinApprox xaxis( vec->xaxismin, vec->xaxismax / XGRD );
	  for( i=0; i<4; ++i ) {
		  sprintf_s( g_strbuf, "%d", (int)xaxis( i ) );
		  txt.draw( g_strbuf, r.left+ddx*i+2, r.top-2, TA_LEFT | TA_BOTTOM );
		  txt.draw( g_strbuf, r.left+ddx*i+2, r.bottom+2, TA_LEFT | TA_TOP );
	  }

      ::SetBkMode( hdc, oldMode );
      ::SetBkColor( hdc, oldBk );

      return Good;
   }
   ::TextOutA( hdc, r.left+15, r.top+15, g_vecErrStr, strlen( g_vecErrStr ) );
   return Fail;
}

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

char* getKeyArg( int argc, char** argv, const char* key ) {

   for( int i=1; i<argc; ++i ) {
      char* p = argv[i];
      if( p && strlen(p) >= 2 && ('/' == p[0] || '-' == p[0]) && key[0] == p[1] ) return p+2;
   }
   return NULL;
}


/* //////////////////////////////////////////////////////////////////////
// Name:       getIntArg
// Purpose:    get integer value from command line with a key
// Context:    parameters of applications are set by using command line
// Returns:    Fail as a rule
// Parameters: 
      int argc          - number of parameters
      char** argv       - parameters
      const char* key   - key for parameter
      int* value        - value to be returned
//
//  Notes:     Use getStrArg for string parameter getting
*/
int getIntArg( int argc, char** argv, const char* key, int* value ) {
   assert( value != (int*)0 );
   char* p = getKeyArg( argc, argv, key );
   if( p && *p ) {
      *value = atoi( p );
      return 1;
   }
   return 0;
}

int getFloatArg( int argc, char** argv, const char* key, float* value ) {
   assert( value != (float*)0 );
   char* p = getKeyArg( argc, argv, key );
   if( p && *p ) {
      *value = atof( p );
      return 1;
   }
   return 0;
}

int isKeyUsed( int argc, char** argv, const char* key ) {
   return NULL != getKeyArg( argc, argv, key );
}

/* //////////////////////////////////////////////////////////////////////
// Name:       getStrArg
// Purpose:    get string value from command line with a key
// Context:    parameters of applications are set by using command line
// Returns:    Fail as a rule
// Parameters: 
      int argc          - number of parameters
      char** argv       - parameters
      const char* key   - key for parameter
      char* value       - value to be returned
//
//  Notes:     Use getIntArg for integer parameter getting
*/
int getStrArg( int argc, char** argv, const char* key, char* value ) {

   char* p = getKeyArg( argc, argv, key );
   if( p && *p ) {
      strcpy( value, p );
      return 1;
   }
   return 0;
}


/// this is a rectangle window
IppStatus __stdcall winNone( Ipp32f*buf, int len ) { return ippStsNoErr; }

/// compute FIR coefficients with use the simple windowing method
int genLowpassFir32f( Ipp32f *fir, int len, float rfreq, Win32fI win ) {

   int center, i;
   if( !(len & 1) ) fir[--len] = 0;
   center = len/2;
   fir[center] = rfreq;
   for( i=1; i<=center; i++ )
      fir[center-i] = fir[center+i] = (Ipp32f)(sin( IPP_PI*rfreq*i )/(IPP_PI*i));
   win( fir, len );
   return len;
}


#define genPRINT(TYPE,FMT) \
void printf_##TYPE( const char* msg, Ipp##TYPE* buf, int len, IppStatus st ) { \
   int n; \
   if( st > ippStsNoErr ) \
      printf( "-- warning %d, %s\n", st, ippGetStatusString( st )); \
   else if( st < ippStsNoErr ) \
      printf( "-- error %d, %s\n", st, ippGetStatusString( st )); \
   printf(" %s ", msg ); \
   for( n=0; n<len; ++n ) printf( FMT, buf[n] ); \
   printf("\n" ); \
}

genPRINT( 32f, " %f" )

/*
  ----------------------------------------------------------------
  $Log:: /exerciser/tools/tools.cpp                              $
 * 
 * 2     2/11/01 3:45p Chan
 * ipps functions instead of malloc, free and memcpy. Range for the
 * X and Y axis can be defined. Some classes were entered, for
 * example Linear Approximator
 * 
 * 1     2/10/01 3:35p Chan
 * Tools for the ipp exerciser - command line processing, 1D and 2D
 * Painters, etc
*/
