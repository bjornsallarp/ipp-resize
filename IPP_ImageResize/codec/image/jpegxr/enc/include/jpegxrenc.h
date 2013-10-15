/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//     Copyright (c) 2009-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __JPEGXRENC_H__
#define __JPEGXRENC_H__

#ifndef __UIC_EXC_STATUS_H__
#include "uic_exc_status.h"
#endif
#ifndef __UIC_IMAGE_H__
#include "uic_image.h"
#endif
#ifndef __UIC_BASE_STREAM_OUTPUT_H__
#include "uic_base_stream_output.h"
#endif
#ifndef __JPEGXRBASE_H__
#include "jpegxrbase.h"
#endif
#ifndef __BITSTREAMOUT_H__
#include "bitstreamout.h"
#endif
#ifndef __IPPJ_H__
#include "ippj.h"
#endif
#ifndef __HDP_H__
#include "hdp.h"
#endif

using namespace UIC;


typedef struct QuantInf
{
  Ipp8u  iIndex;
  Ipp32s iQP;
  Ipp32u iMan;
  Ipp32u iExp;
  Ipp32u iOffset;

} QuantInf;


typedef struct ImagePlane
{
  Ipp32u iColorFormat;
  Ipp32u bResampling;
  Ipp32u bScalingFlag;
  Ipp32u iBands;
  Ipp32u iChannels;
  Ipp32u iCCenteringX;
  Ipp32u iCCenteringY;
  Ipp32u iShift;
  Ipp32u iMantiss;
  Ipp32u iExp;
  Ipp32u iCHModeDC;
  Ipp32u iCHModeLP;
  Ipp32u iCHModeHP;
  Ipp32u bDCFrameUniform;
  Ipp32u bLPFrameUniform;
  Ipp32u bHPFrameUniform;
  Ipp32u iDCQuant[JXR_MAX_CHANNELS];
  Ipp32u iLPQuant[JXR_MAX_CHANNELS];
  Ipp32u iHPQuant[JXR_MAX_CHANNELS];

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

} ImagePlane;


class CJPEGXREncoder
{
public:
  CJPEGXREncoder(void);
  ~CJPEGXREncoder(void);

  ExcStatus AttachStream(BaseStreamOutput& out);

  ExcStatus SetParams(Ipp8u iQuality, Ipp8u iOverlap, Ipp8u iBands,
    Ipp8u iSampling, Ipp8u iTrim, Ipp8u iShift, Ipp8u bFrequency, Ipp8u bCMYKD, Ipp8u bAlphaPlane, Ipp16u* iTilesUniform);

  ExcStatus WriteFileHeader(Ipp32u iPAlphaPresent);

  ExcStatus WriteHeader(void);

  ExcStatus WriteData(void);

  ExcStatus KillEncoder(void);

protected:
  ExcStatus initEncoder(void);
  ExcStatus resetEncoder(void);

  // WriteHeader functions
  ExcStatus writeIFDValue(Ipp32u iTAG, Ipp32u iType, Ipp32u iCount, void* pValue);
  ExcStatus writeIFDTable(void);
  ExcStatus writeJXRImageHeader(void);
  ExcStatus writeJXRImagePlaneHeader(void);
  ExcStatus writeJXRIndexTable(void);
  ExcStatus fillJXRIndexTable(void);
  ExcStatus writeUnifQuantizer(Ipp32u iCHMode, Ipp32u *iQuant);

  // WriteData functions
  ExcStatus writeJXRMacroblock(void);
  ExcStatus writeJXRTile(void);

  ExcStatus colorConvert(Ipp8u *pDst, Ipp32u iDstStep);
  ExcStatus PCTransform(void);
  ExcStatus quantMacroblock(void);
  ExcStatus predMacroblock(void);
  ExcStatus writeMacroblockDC(void);
  ExcStatus writeMacroblockLP(void);
  ExcStatus writeMacroblockHP(void);

protected:
  CBitStreamOutput m_BitStreamOut;
  Ipp8u*           m_pData;
  Ipp32u           m_iStep;

  // IFD Table support
  Ipp8u  m_pIFDBuffer[512]; // Temporary buffer for IFD table; enough for 42 entrys
  Ipp32u m_iIFDSize;
  Ipp32u m_iIOEntryOffset;
  Ipp32u m_iILEntryOffset;
  Ipp32u m_iAOEntryOffset;
  Ipp32u m_iALEntryOffset;

  // Inter alpha support
  ImagePlane *m_pImagePlane; // Current plane pointer
  ImagePlane m_ImagePlane[2];
  Ipp32u m_iACHOffset;
  Ipp32u m_bAlphaProcess;
  Ipp32u m_bPAlphaProcess;

  Ipp8u* m_pCCBuf;
  Ipp32u m_iThumbnailScale;
  Ipp32u m_iBorder;
  Ipp32u m_iBorderPCT;
  Ipp32u m_iMBSize;
  Ipp64u *m_pIndexTable;
  Ipp32u m_iIndexOffset;
  Ipp32u m_iIndexSize;
  Ipp32u m_bFirstLoop;
  Ipp32u m_iCUSize;
  Ipp32u m_bBGR;
  Ipp32u m_iNumBands;
  Ipp8u  m_bFakeAlpha;

  Ipp32u m_iMBRow;
  Ipp32u m_iMBColumn;
  Ipp32u m_iMBAbsRow;
  Ipp32u m_iMBAbsColumn;
  Ipp32u m_iTileRow;
  Ipp32u m_iTileColumn;
  Ipp32u m_iTileStartRow;
  Ipp32u m_iTileStartColumn;
  Ipp32u m_iCurTileMBWidth;
  Ipp32u m_iCurTileMBHeight;
  Ipp32u m_iCurTileWidth;
  Ipp32u m_iCurTileHeight;
  Ipp32u m_iChannels; // Total channels (with alpha)

  Ipp32u m_iImageOffset;
  Ipp32u m_iImageNumBytes;
  Ipp32u m_iAlphaOffset;
  Ipp32u m_iAlphaNumBytes;

  Ipp8u *m_pAllocBuffer;

  MBTab   m_pMBTab[JXR_MAX_CHANNELS];
  Ipp32s* m_pResU;
  Ipp32s* m_pResV;
  Ipp16u  m_iCBP[JXR_MAX_CHANNELS];
  Ipp32s  m_iDCLPTable[JXR_MAX_CHANNELS][16];

  PredictorInf *m_pPred[JXR_MAX_CHANNELS];
  PredictorInf *m_pPredPrev[JXR_MAX_CHANNELS];

  Ipp32u m_bResetContext;
  Ipp32u m_bResetTotals;

  QuantInf *m_pQuantDC[JXR_MAX_CHANNELS];
  QuantInf *m_pQuantLP[JXR_MAX_CHANNELS];
  QuantInf *m_pQuantHP[JXR_MAX_CHANNELS];
  Ipp32u m_iInputQuant;
  Ipp32u m_iYQuant;
  Ipp32u m_iUVQuant;

  Ipp32u m_iTileType;
  Ipp32u m_bHardTilingFlag;
  Ipp32u m_bTilingFlag;
  Ipp16u m_iTilesUniform[4];
  Ipp32u m_iTilesTotal;
  Ipp16u* m_iTileMBWidth;
  Ipp16u* m_iTileMBHeight;
  Ipp32u m_iMBTotalWidth;
  Ipp32u m_iMBTotalHeight;

  Ipp32u m_bFrequencyFlag;
  Ipp32u m_iOrientation;
  Ipp32u m_bIndexFlag;
  Ipp32u m_iOverlap;
  Ipp32u m_iSampling;
  Ipp32u m_bShortHeaderFlag;
  Ipp32u m_bLongWordFlag;
  Ipp32u m_bWindowingFlag;
  Ipp32u m_iTopExtra;
  Ipp32u m_iLeftExtra;
  Ipp32u m_iBottomExtra;
  Ipp32u m_iRightExtra;
  Ipp32u m_bTrimFlexFlag;
  Ipp32u m_iTrimFlex;
  Ipp32u m_bAlphaFlag;
  Ipp32u m_bPreAlpha;
  Ipp32u m_bCMYKD;
  Ipp32u m_iUnitSize;

  Ipp32u m_iWidth;
  Ipp32u m_iHeight;
  Ipp32u m_iBitDepth;
  Ipp32u m_iSrcColorFormat;

  Ipp32u m_iNumLPQuantizers;
  Ipp32u m_iNumHPQuantizers;
  Ipp32u m_iLPQuantizerIndex;
  Ipp32u m_iHPQuantizerIndex;

};

#endif // __JPEGXRENC_H__
