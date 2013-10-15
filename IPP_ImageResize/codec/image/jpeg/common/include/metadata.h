/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifndef __METADATA_H__
#define __METADATA_H__

#ifndef __IPPDEFS_H__
#include "ippdefs.h"
#endif
#ifndef __IPPCORE_H__
#include "ippcore.h"
#endif
#ifndef __JPEGBASE_H__
#include "jpegbase.h"
#endif


const unsigned short EXIF_LITTLE_ENDIAN = 0x4949;
const unsigned short EXIF_BIG_ENDIAN    = 0x4d4d;
const unsigned short EXIF_MAGIC         = 0x002a;


enum
{
  TIFF_IFD_0                = 0,
  TIFF_IFD_1                = 1,
  TIFF_IFD_EXIF             = 2,
  TIFF_IFD_GPS              = 3,
  TIFF_IFD_INTEROPERABILITY = 4,
  TIFF_IFD_MAX              = 5
};


typedef enum
{
  TAG_INTEROPERABILITY_INDEX   = 0x0001,
  TAG_INTEROPERABILITY_VERSION = 0x0002,
  TAG_IMAGE_WIDTH              = 0x0100,
  TAG_IMAGE_HEIGHT             = 0x0101,
  TAG_IMAGE_BITS_PER_SAMPLE    = 0x0102,
  TAG_IMAGE_COMPRESSION        = 0x0103,
  TAG_PIXEL_COMPOSITION        = 0x0106,
  TAG_IMAGE_TITLE              = 0x010e,
  TAG_IMAGE_MAKER              = 0x010f,
  TAG_EQUIPMENT_MODEL          = 0x0110,
  TAG_IMAGE_DATA_LOCATION      = 0x0111,
  TAG_IMAGE_ORIENTATION        = 0x0112,
  TAG_IMAGE_NUM_OF_COMPONENT   = 0x0115,
  TAG_NUM_ROW_PER_STRIP        = 0x0116,
  TAG_BYTES_PER_CSTRIP         = 0x0117,
  TAG_IMAGE_XRESOLUTION        = 0x011a,
  TAG_IMAGE_YRESOLUTION        = 0x011b,
  TAG_IMAGE_PLANE              = 0x011c,
  TAG_IMAGE_RESOLUTION_UNIT    = 0x0128,
  TAG_TRANSFER_FUNCTION        = 0x012d,
  TAG_SOFTWARE                 = 0x0131,
  TAG_FILE_CREATE_DATE         = 0x0132,
  TAG_ARTIST                   = 0x013b,
  TAG_WHITE_POINT_CHR          = 0x013e,
  TAG_PRIMIARY_CHROMACITIES    = 0x013f,
  TAG_OFFSET_TO_JPEG           = 0x0201,
  TAG_LENGTH_OF_JPEG           = 0x0202,
  TAG_YCBCR_COEFFICIENTS       = 0x0211,
  TAG_YCBCR_SUBSAMPLING        = 0x0212,
  TAG_YCBCR_POSITIONING        = 0x0213,
  TAG_REFERENCE_BLACK_WHITE    = 0x0214,
  TAG_COPYRIGHT                = 0x8298,
  TAG_EXPOSURE_TIME            = 0x829a,
  TAG_FNUMBER                  = 0x829d,
  TAG_EXIF_IFD_PTR             = 0x8769,
  TAG_EXPOSURE_PROGRAM         = 0x8822,
  TAG_SPECTRAL_SENSITIVITY     = 0x8824,
  TAG_GPS_IFD_PTR              = 0x8825,
  TAG_ISO_SPEED                = 0x8827,
  TAG_OECF                     = 0x8828,

  TAG_EXIF_VERSION             = 0x9000,
  TAG_ORIGINAL_DATE            = 0x9003,
  TAG_DIGITIZED_DATE           = 0x9004,
  TAG_COMPONENTS_CONFIGURATION = 0x9101,
  TAG_COMPR_RATIO              = 0x9102,
  TAG_SHUTTER_SPEED            = 0x9201,
  TAG_APERTURE                 = 0x9202,
  TAG_BRIGHTNESS               = 0x9203,
  TAG_EXPOSURE_BIAS            = 0x9204,
  TAG_MAX_LENS_APERTURE        = 0x9205,
  TAG_SUBJECT_DISTANCE         = 0x9206,
  TAG_METERING_MODE            = 0x9207,
  TAG_LIGHT_SOURCE             = 0x9208,
  TAG_FLASH                    = 0x9209,
  TAG_LENS_FOCAL               = 0x920a,
  TAG_MAKER_NOTE               = 0x927c,
  TAG_USER_COMMENTS            = 0x9286,
  TAG_SUBSEC                   = 0x9290,
  TAG_SUBSEC_ORIG              = 0x9291,
  TAG_SUBSEC_DIGIT             = 0x9292,
  TAG_FPX_VERSION              = 0xa000,
  TAG_COLOR_SPACE              = 0xa001,
  TAG_VIMAGE_WIDTH             = 0xa002,
  TAG_VIMAGE_HEIGHT            = 0xa003,
  TAG_RELATED_AUDIO_FILE       = 0xa004,
  TAG_INTEROPERABILITY_IFD_PTR = 0xa005,

  TAG_FLASH_ENERGY             = 0xa20b,
  TAG_SPATIAL_FREQ_RESP        = 0xa20c,
  TAG_FOCAL_XRESOLUTION        = 0xa20e,
  TAG_FOCAL_YRESOLUTION        = 0xa20f,
  TAG_FOCAL_RESOLUTION_UNIT    = 0xa210,
  TAG_SUBJ_LOCATION            = 0xa214,
  TAG_EXPOSURE_INDEX           = 0xa215,
  TAG_SENSE_METHOD             = 0xa217,
  TAG_FILE_SOURCE              = 0xa300,
  TAG_SCENE_TYPE               = 0xa301,
  TAG_CFA_PATTERN              = 0xa302,

  TAG_MAX                      = 0xa410

} TIFF_TAG;


typedef struct _tiff_dictionary
{
  TIFF_TAG tag;
  const char* name;
  char* title;
  const char* description;

} TIFF_DICTIONARY;


typedef enum
{
  TIFF_BYTE      = 0,
  TIFF_ASCII     = 2,
  TIFF_SHORT     = 3,
  TIFF_LONG      = 4,
  TIFF_RATIONAL  = 5,
  TIFF_UNDEFINED = 7,
  TIFF_SLONG     = 9,
  TIFF_SRATIONAL = 10

} TIFF_VALTYPE;


typedef struct _tiff_hdr
{
  unsigned short endianess;
  unsigned short magic;
  unsigned int   offset;

} TIFF_HEADER;


typedef struct _tiff_ifd_entry
{
  TIFF_TAG       tag;
  TIFF_VALTYPE   type;
  unsigned int   count;
  unsigned int   value;

  union
  {
    void*           anyptr;
    unsigned char*  byte;
    unsigned short* word;
    unsigned int*   dword;
  } p;

  const char*  name;

} TIFF_IFD_ENTRY;


typedef struct _tiff_ifd
{
  unsigned short   count;
  TIFF_IFD_ENTRY** entry;
  unsigned int     next;

} TIFF_IFD;



class CJPEGMetaData
{
public:
  CJPEGMetaData(void);
  virtual ~CJPEGMetaData(void);

  JERRCODE Destroy(void);

  JERRCODE ProcessAPP1_Exif(Ipp8u* pData,int nDataSize);

  int    GetIfdNumEntries(int ifdno);
  char*  ShowIfdData(int ifdno, int entryno);
  int    GetJPEGThumbnailsLen();
  Ipp8u* GetJPEGThumbnailsData();

private:
  JERRCODE ReadTiffHdr(TIFF_HEADER* hdr);
  JERRCODE ReadIfdData(TIFF_IFD* ifd);
  JERRCODE ReadIfdEntry(TIFF_IFD_ENTRY* entry);
  JERRCODE ReadIfdEntryVal(TIFF_IFD_ENTRY* entry);
  char* ShowIfdEntry(TIFF_IFD_ENTRY* entry);

  JERRCODE DestroyIfd(TIFF_IFD* ifd);
  JERRCODE InitTiffDictionary(TIFF_DICTIONARY* dict);

  JERRCODE GET_BYTE(Ipp8u* pData,int* curPos, int nDataSize, unsigned char* byte);
  JERRCODE GET_WORD(Ipp8u* pData,int* curPos, int nDataSize, unsigned short* word);
  JERRCODE GET_DWORD(Ipp8u* pData,int* curPos, int nDataSize, unsigned int* dword);


  TIFF_HEADER m_hdr;
  TIFF_IFD    m_ifd[TIFF_IFD_MAX];
  Ipp8u*      m_rawData;
  int         m_currPos;
  int         m_nDataSize;
};

#endif // __METADATA_H__
