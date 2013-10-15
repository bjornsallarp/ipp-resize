/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __JPEGXRDEC_H__
#define __JPEGXRDEC_H__

#ifndef __UIC_EXC_STATUS_H__
#include "uic_exc_status.h"
#endif
#ifndef __UIC_IMAGE_H__
#include "uic_image.h"
#endif
#ifndef __UIC_BASE_STREAM_INPUT_H__
#include "uic_base_stream_input.h"
#endif
#ifndef __JPEGXRBASE_H__
#include "jpegxrbase.h"
#endif
#ifndef __BITSTREAMIN_H__
#include "bitstreamin.h"
#endif
#ifndef __IPPJ_H__
#include "ippj.h"
#endif
#ifndef __HDP_H__
#include "hdp.h"
#endif

#ifdef USE_TBB
#include "tbb/spin_mutex.h"
#include "tbb/task_scheduler_init.h"
#endif


using namespace UIC;


typedef struct QuantInf
{
  Ipp8u  iIndex;
  Ipp32s iQP;

} QuantInf;


typedef struct ImagePlane
{
  Ipp8u  iColorFormat;
  Ipp8u  bScalingFlag;
  Ipp8u  iBands;
  Ipp8u  iCCenteringX;
  Ipp8u  iCCenteringY;
  Ipp16u iChannels;
  Ipp8u  iShift;
  Ipp8u  iMantiss;
  Ipp8u  iExp;
  Ipp8u  bDCUniformFlag;
  Ipp8u  bLPUniformFlag;
  Ipp8u  bHPUniformFlag;
  Ipp32u iNumLPQ;
  Ipp32u iNumHPQ;

  Ipp8u bResampling;
  Ipp8u iCHModeDC;
  Ipp8u iCHModeLP;
  Ipp8u iCHModeHP;
  Ipp8u iDCQuant[JXR_MAX_CHANNELS];
  Ipp8u iLPQuant[JXR_MAX_CHANNELS];
  Ipp8u iHPQuant[JXR_MAX_CHANNELS];

} ImagePlane;


typedef struct HuffmanPlane
{
  IppiVLCState_JPEGXR* pHuffState;
  AdaptiveHuffman pAdaptCBP;
  AdaptiveHuffman pAdaptBlockCBP;
  AdaptiveHuffman pAdaptDCY;
  AdaptiveHuffman pAdaptDCUV;

  AdaptiveModel pModelDC;
  AdaptiveModel pModelLP;
  AdaptiveModel pModelHP;

  CBPModel pCBPModel;
  Ipp32s   iCBPCountZero;
  Ipp32s   iCBPCountMax;

} HuffmanPlane;


class CJPEGXRDecoder
{
public:
  CJPEGXRDecoder(void);
  ~CJPEGXRDecoder(void);

  friend class CJPEGXRTileDecoder;

  ExcStatus AttachStream(BaseStreamInput& in);

  ExcStatus SetPlaneOffset(Ipp32u iImageOffset);

  ExcStatus SetParams(Ipp32u iForceColor, Ipp32u iBandsLimit, Ipp32u iMultiMode);

  Ipp32u SetThreads(Ipp32u iThreads);
  Ipp32u Threads();

  ExcStatus ReadTileInfo(
              Ipp32u* iTileWidth, Ipp32u* iTileHeight);

  ExcStatus ReadFileHeader(Ipp32u& iImageOffset, Ipp32u& iAlphaOffset, Ipp8u& bPreAlpha);

  ExcStatus ReadHeader(
              int* width, int* height, Ipp32u* nchannels,
              ImageEnumColorSpace* color, ImageEnumSampling* sampling,
              Ipp32u* precision);

  ExcStatus ReadData(void);

  ExcStatus ReadTile(
            const ImageDataPtr* dataPtr,
            const ImageDataOrder* dataOrder,
            Ipp32u iTileX,
            Ipp32u iTileY);

  ExcStatus KillDecoder(void);

protected:
  ExcStatus initDecoder(void);

  // ReadHeader functions
  ExcStatus readIFDValue(Ipp16u iType, Ipp32u iCount, Ipp32u iValue, void* pValue);
  ExcStatus readImageHeader(Ipp32u offset);
  ExcStatus readImagePlaneHeader(void);
  ExcStatus readIndexTable(void);
  ExcStatus readUnifQuantizer(Ipp8u* iCHMode, Ipp8u* iQuant);

  // ReadData functions
  ExcStatus readFrame(void);
  ExcStatus readFrameTBB(void);

protected:
#ifdef USE_TBB
  tbb::spin_mutex  m_Mutex;
  tbb::task_scheduler_init* scheduler;
#endif

  BaseStreamInput* m_in;
  CBitStreamInput  m_BitStreamIn;
  Ipp8u*           m_pData;
  Ipp32u           m_iStep;

  // IFD Header
  Ipp32u m_iImageOffset;
  Ipp32u m_iImageNumBytes;
  Ipp32u m_iAlphaOffset;
  Ipp32u m_iAlphaNumBytes;
  Ipp32u m_iCurrentOffset;
  Ipp8u  m_bBGR;
  Ipp8u  m_bFakeAlpha;
  Ipp8u  m_bPreAlpha;

  // Image plane header
  ImagePlane  m_ImagePlane[2];
  ImagePlane* m_pImagePlane;

  // Image header
  Ipp8u   m_bHardTilingFlag;
  Ipp8u   m_bTilingFlag;
  Ipp8u   m_bFrequencyFlag;
  Ipp8u   m_iOrientation;
  Ipp8u   m_bIndexFlag;
  Ipp8u   m_iOverlap;
  Ipp8u   m_bShortHeaderFlag;
  Ipp8u   m_bLongWordFlag;
  Ipp8u   m_bWindowingFlag;
  Ipp8u   m_bTrimFlexFlag;
  Ipp8u   m_bAlphaFlag;
  Ipp8u   m_iSrcColorFormat;
  Ipp8u   m_iBitDepth;
  Ipp32u  m_iWidth;
  Ipp32u  m_iHeight;
  Ipp16u  m_iTilesWidth;
  Ipp16u  m_iTilesHeight;
  Ipp16u* m_iTileMBWidth;
  Ipp16u* m_iTileMBHeight;
  Ipp8u   m_iTopExtra;
  Ipp8u   m_iLeftExtra;
  Ipp8u   m_iBottomExtra;
  Ipp8u   m_iRightExtra;

  // Threads params
  Ipp8u   m_bThreadsInit;
  Ipp8u   m_iThreadMode;
  Ipp16u  m_iThreads;
  Ipp16u  m_iTileThreads;
  Ipp16u  m_iPipeThreads;
  Ipp16u  m_iPipeLength;
  Ipp16u* m_pPipeArray;
  Ipp8u   m_bPipeArrayInit;
  Ipp16u  m_iInstances;

  // Tile params
  Ipp64u* m_IndexTable;
  Ipp32u  m_iIndexSize;
  Ipp8u   m_bReadTile;
  Ipp16u  m_iReadTileRow;
  Ipp16u  m_iReadTileColumn;
  Ipp32u  m_iFirstOffset;
  Ipp8u   m_bSoftTiling;

  // Internal buffers
  Ipp8u* m_pAllocBuffer;
  Ipp8u* m_pPredBuffer;
  Ipp8u* m_pHuffBuffer;
  Ipp8u* m_pImgBuffer;
  Ipp8u* m_pResBuffer;
  Ipp8u* m_pCCBuf;
  Ipp8u* m_pFCBuffer;
  Ipp8u* m_pFRBuffer;
  Ipp8u  m_iCUSize;
  Ipp32u m_iExMBSize;
  Ipp32u m_iInMBSize;

  // Subsampling
  IppiSize m_lRoi;
  IppiSize m_cRoi;
  Ipp32u   m_iLumaBlock;
  Ipp32u   m_iChromaBlock;
  Ipp32u   m_iLumaSize;    // byte size of luma macroblock
  Ipp32u   m_iChromaSize;  // byte size of chroma macroblock

  // Options
  Ipp8u  m_iBandsLimit;
  Ipp8u  m_iForceColor;

  QuantInf* m_pQuantDC[JXR_MAX_CHANNELS];
  QuantInf* m_pQuantLP[JXR_MAX_CHANNELS];
  QuantInf* m_pQuantHP[JXR_MAX_CHANNELS];

  Ipp16u m_iACHOffset;
  Ipp8u  m_bPAlphaProcess;
  Ipp16u m_iChannels; // Total channels (with alpha)
  Ipp8u  m_bFirstLoop;

  Ipp16u m_iMaxTileMBHeight;
  Ipp16u m_iMaxTileMBWidth;
  Ipp32u m_iMaxTileBitstream;

  Ipp8u  m_iNumTileBands;
  Ipp8u  m_iNumBands;
  Ipp32u m_iTilesTotal;
  Ipp32u m_iMBTotalWidth;
  Ipp32u m_iMBTotalHeight;
  Ipp8u  m_iUnitSize;
  Ipp8u  m_iOutColorFormat;

  Ipp8u m_iLPQuantizerIndex;
  Ipp8u m_iHPQuantizerIndex;

};


class CJPEGXRTileDecoder
{
public:
  CJPEGXRTileDecoder(void);
  ~CJPEGXRTileDecoder(void);

  friend class CJPEGXRDecoder;
  CJPEGXRDecoder* m_pParent;

private:
  // ReadData functions
  ExcStatus readMacroblock(void);
  ExcStatus readMacroblockDC(void);
  ExcStatus readMacroblockLP(void);
  ExcStatus readMacroblockHP(void);
  ExcStatus predDCLP(void);
  ExcStatus dequantDCLP(void);
  ExcStatus PCTransform(void);
  ExcStatus colorConvert(Ipp8u *pDst, Ipp32u iDstStep, IppiSize ccRoi);

public:
  ExcStatus initDecoder(CJPEGXRDecoder* pParent);
  ExcStatus resetDecoder(void);

  ExcStatus readTile(void);
  ExcStatus readTileTBB(void);

  // TBB process functions
  Ipp8u isLocked(void) {return m_bThreadLock;}
  void  lock(void) {m_bThreadLock = 1;}
  void  release(void) {m_bThreadLock = 0;}
  void  readFrameSeparate(Ipp16u iTileRow, Ipp16u iTileColumn);
  void* readTileSeparate(void);
  void* PCTransformSeparate(void);
  void* colorConvertSeparate(void);

private:
  CBitStreamInput* m_pStream;
  CBitStreamInput  m_Stream[4]; // Multiple streams for frequency decoding
  Ipp8u*           m_pDst;

  HuffmanPlane  m_HuffmanPlane[2];
  HuffmanPlane* m_pHuffmanPlane;
  ImagePlane*   m_pImagePlane;    // Current plane pointer
  ImagePlane*   m_pImagePlanePCT; // Current plane pointer for PCT
  Ipp8u         m_bAlphaProcess;
  Ipp8u         m_bAlphaProcessPCT;

  Ipp8u  m_iBorder;
  Ipp8u  m_iBorderPCT;
  Ipp32u m_iMBRow;
  Ipp32u m_iMBColumn;
  Ipp32u m_iMBRowPCT;
  Ipp32u m_iMBColumnPCT;
  Ipp32u m_iMBRowCC;
  Ipp32u m_iMBColumnCC;
  Ipp16u m_iTileRow;
  Ipp16u m_iTileColumn;
  Ipp32u m_iCurTileMBRow;
  Ipp32u m_iCurTileMBColumn;
  Ipp32u m_iCurTileMBWidth;
  Ipp32u m_iCurTileMBHeight;
  Ipp8u  m_iTrimFlex;

  Ipp8u m_iTileType;
  Ipp8u m_bHufDone;
  Ipp8u m_bPCTDone;
  Ipp8u m_bCCDone;
  Ipp8u m_bTBBError;

  Ipp32s*       m_pResU;
  Ipp32s*       m_pResV;
  ImgBuf        m_ImgBuf[JXR_MAX_CHANNELS];
  Ipp32s        m_iDCLPTable[JXR_MAX_CHANNELS][16];
  PredictorInf* m_pPred[JXR_MAX_CHANNELS];
  PredictorInf* m_pPredPrev[JXR_MAX_CHANNELS];

  Ipp16u m_iPipeThreads;
  Ipp8u  m_bThreadLock;
  Ipp8u  m_bResetContext;
  Ipp8u  m_bResetTotals;
  Ipp8u  m_iOrientation;

};

#endif // __JPEGXRDEC_H__
