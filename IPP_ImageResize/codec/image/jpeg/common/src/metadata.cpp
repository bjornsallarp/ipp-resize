/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//    Copyright (c) 2005-2012 Intel Corporation. All Rights Reserved.
//
*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4996 )
#endif
#include "stdafx.h"
#include "precomp.h"

#include <stdio.h>
#include <stdlib.h>
#ifndef __METADATA_H__
#include "metadata.h"
#endif


static int sizeof_val[] =
{
  1, //  0 TIFF_BYTE
  0, //  1 n/a
  1, //  2 TIFF_ASCII
  2, //  3 TIFF_SHORT
  4, //  4 TIFF_LONG
  8, //  5 TIFF_RATIONAL
  0, //  6 n/a
  1, //  7 TIFF_UNDEFINED
  0, //  8 n/a
  4, //  9 TIFF_SLONG
  8  // 10 TIFF_SRATIONAL
};

static TIFF_DICTIONARY tiff_dict[TAG_MAX];

CJPEGMetaData::CJPEGMetaData(void)
{
  ippsZero_8u((Ipp8u*)&m_hdr,sizeof(m_hdr));
  ippsZero_8u((Ipp8u*)&m_ifd,sizeof(m_ifd));
  ippsZero_8u((Ipp8u*)&tiff_dict,sizeof(tiff_dict));

  m_rawData   = 0;
  m_currPos   = 0;
  m_nDataSize = 0;

  InitTiffDictionary(&tiff_dict[0]);

  return;
} // ctor


CJPEGMetaData::~CJPEGMetaData(void)
{
  Destroy();
  return;
} // dtor

JERRCODE CJPEGMetaData::Destroy(void)
{
  int i;

  for(i = TIFF_IFD_0; i < TIFF_IFD_MAX; i++)
  {
    DestroyIfd(&m_ifd[i]);
  }

  ippsZero_8u((Ipp8u*)&m_ifd,sizeof(m_ifd));

  m_rawData   = 0;
  m_currPos   = 0;
  m_nDataSize = 0;

  return JPEG_OK;
} // CJPEGMetaData::Destroy()


JERRCODE CJPEGMetaData::DestroyIfd(TIFF_IFD* ifd)
{
  int i;

  for(i = 0; i < ifd->count; i++)
  {
    if(0 != ifd->entry[i])
    {
      if(0 != ifd->entry[i]->p.anyptr)
      {
        ippFree(ifd->entry[i]->p.anyptr);
      }
      ippFree(ifd->entry[i]);
      ifd->entry[i] = 0;
    }
  }

  if(0 != ifd->entry)
    ippFree(ifd->entry);

  ifd->entry = 0;

  return JPEG_OK;
} // CJPEGMetaData::DestroyIfd()


#define MAKE_DICT_ENTRY(dict,t,n,d) \
{ \
  dict[t].tag = t; \
  dict[t].name = n; \
  dict[t].description = d; \
}

JERRCODE CJPEGMetaData::InitTiffDictionary(TIFF_DICTIONARY* dict)
{
  MAKE_DICT_ENTRY(dict,TAG_INTEROPERABILITY_INDEX,"InteroperabilityIndex","Indicates the identification of the Interoperability rule.");
  MAKE_DICT_ENTRY(dict,TAG_INTEROPERABILITY_VERSION,"InteroperabilityVersion","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_WIDTH,"ImageWidth","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_HEIGHT,"ImageHeight","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_BITS_PER_SAMPLE,"BitsPerSample","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_COMPRESSION,"Compression","");
  MAKE_DICT_ENTRY(dict,TAG_PIXEL_COMPOSITION,"PhotometricInterpretation","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_TITLE,"ImageDescription","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_MAKER,"Make","");
  MAKE_DICT_ENTRY(dict,TAG_EQUIPMENT_MODEL,"Model","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_DATA_LOCATION,"StripOffset","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_ORIENTATION,"Orientation","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_NUM_OF_COMPONENT,"SamplesPerPixel","");
  MAKE_DICT_ENTRY(dict,TAG_NUM_ROW_PER_STRIP,"RowsPerStrip","");
  MAKE_DICT_ENTRY(dict,TAG_BYTES_PER_CSTRIP,"StripByteCounts","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_XRESOLUTION,"XResolution","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_YRESOLUTION,"YResolution","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_PLANE,"PlanarConfiguration","");
  MAKE_DICT_ENTRY(dict,TAG_IMAGE_RESOLUTION_UNIT,"ResolutionUnit","");
  MAKE_DICT_ENTRY(dict,TAG_TRANSFER_FUNCTION,"TransferFunction","");
  MAKE_DICT_ENTRY(dict,TAG_SOFTWARE,"Software","");
  MAKE_DICT_ENTRY(dict,TAG_FILE_CREATE_DATE,"DateTime","");
  MAKE_DICT_ENTRY(dict,TAG_ARTIST,"Artist","");
  MAKE_DICT_ENTRY(dict,TAG_WHITE_POINT_CHR,"WhitePoint","");
  MAKE_DICT_ENTRY(dict,TAG_PRIMIARY_CHROMACITIES,"PrimaryChromacities","");
  MAKE_DICT_ENTRY(dict,TAG_OFFSET_TO_JPEG,"JPEGInterchangeFormat","");
  MAKE_DICT_ENTRY(dict,TAG_LENGTH_OF_JPEG,"JPEGInterchangeFormatLength","");
  MAKE_DICT_ENTRY(dict,TAG_YCBCR_COEFFICIENTS,"YCbCrCoefficients","");
  MAKE_DICT_ENTRY(dict,TAG_YCBCR_SUBSAMPLING,"YCbCrSubSampling","");
  MAKE_DICT_ENTRY(dict,TAG_YCBCR_POSITIONING,"YCbCrPositioning","");
  MAKE_DICT_ENTRY(dict,TAG_REFERENCE_BLACK_WHITE,"ReferenceBlackWhite","");
  MAKE_DICT_ENTRY(dict,TAG_COPYRIGHT,"Copyright","");
  MAKE_DICT_ENTRY(dict,TAG_EXPOSURE_TIME,"ExposureTime","");
  MAKE_DICT_ENTRY(dict,TAG_FNUMBER,"FNumber","");
  MAKE_DICT_ENTRY(dict,TAG_EXIF_IFD_PTR,"ExifIFDPointer","");
  MAKE_DICT_ENTRY(dict,TAG_EXPOSURE_PROGRAM,"ExposureProgram","");
  MAKE_DICT_ENTRY(dict,TAG_SPECTRAL_SENSITIVITY,"SpectralSensitivity","");
  MAKE_DICT_ENTRY(dict,TAG_GPS_IFD_PTR,"GPSInfoIFDPointer","");
  MAKE_DICT_ENTRY(dict,TAG_ISO_SPEED,"ISOSpeedRatings","");
  MAKE_DICT_ENTRY(dict,TAG_OECF,"OECF","");
  MAKE_DICT_ENTRY(dict,TAG_EXIF_VERSION,"ExifVersion","");
  MAKE_DICT_ENTRY(dict,TAG_ORIGINAL_DATE,"DateTimeOriginal","");
  MAKE_DICT_ENTRY(dict,TAG_DIGITIZED_DATE,"DateTimeDigitized","");
  MAKE_DICT_ENTRY(dict,TAG_COMPONENTS_CONFIGURATION,"ComponentsConfiguration","");
  MAKE_DICT_ENTRY(dict,TAG_COMPR_RATIO,"CompressedBitsPerPixel","");
  MAKE_DICT_ENTRY(dict,TAG_SHUTTER_SPEED,"ShutterSpeedValue","");
  MAKE_DICT_ENTRY(dict,TAG_APERTURE,"ApertureValue","");
  MAKE_DICT_ENTRY(dict,TAG_BRIGHTNESS,"BrightnessValue","");
  MAKE_DICT_ENTRY(dict,TAG_EXPOSURE_BIAS,"ExposureBiasValue","");
  MAKE_DICT_ENTRY(dict,TAG_MAX_LENS_APERTURE,"MaxApertureValue","");
  MAKE_DICT_ENTRY(dict,TAG_SUBJECT_DISTANCE,"SubjectDistance","");
  MAKE_DICT_ENTRY(dict,TAG_METERING_MODE,"MeteringMode","");
  MAKE_DICT_ENTRY(dict,TAG_LIGHT_SOURCE,"LightSource","");
  MAKE_DICT_ENTRY(dict,TAG_FLASH,"Flash","");
  MAKE_DICT_ENTRY(dict,TAG_LENS_FOCAL,"FocalLength","");
  MAKE_DICT_ENTRY(dict,TAG_MAKER_NOTE,"MakerNote","");
  MAKE_DICT_ENTRY(dict,TAG_USER_COMMENTS,"UserComment","");
  MAKE_DICT_ENTRY(dict,TAG_SUBSEC,"SubsecTime","");
  MAKE_DICT_ENTRY(dict,TAG_SUBSEC_ORIG,"SubsecTimeOriginal","");
  MAKE_DICT_ENTRY(dict,TAG_SUBSEC_DIGIT,"SubsecTimeDigitized","");
  MAKE_DICT_ENTRY(dict,TAG_FPX_VERSION,"FlashPixVersion","");
  MAKE_DICT_ENTRY(dict,TAG_COLOR_SPACE,"ColorSpace","");
  MAKE_DICT_ENTRY(dict,TAG_VIMAGE_WIDTH,"PixelXDimension","");
  MAKE_DICT_ENTRY(dict,TAG_VIMAGE_HEIGHT,"PixelYDimension","");
  MAKE_DICT_ENTRY(dict,TAG_RELATED_AUDIO_FILE,"RelatedAudioFile","");
  MAKE_DICT_ENTRY(dict,TAG_INTEROPERABILITY_IFD_PTR,"InteroperabilityIFDPointer","");
  MAKE_DICT_ENTRY(dict,TAG_FLASH_ENERGY,"FlashEnergy","");
  MAKE_DICT_ENTRY(dict,TAG_SPATIAL_FREQ_RESP,"SpatialFrequencyResponse","");
  MAKE_DICT_ENTRY(dict,TAG_FOCAL_XRESOLUTION,"FocalPlaneXResolution","");
  MAKE_DICT_ENTRY(dict,TAG_FOCAL_YRESOLUTION,"FocalPlaneYResolution","");
  MAKE_DICT_ENTRY(dict,TAG_FOCAL_RESOLUTION_UNIT,"FocalPlaneResolutionUnit","");
  MAKE_DICT_ENTRY(dict,TAG_SUBJ_LOCATION,"SubjectLocation","");
  MAKE_DICT_ENTRY(dict,TAG_EXPOSURE_INDEX,"ExposureIndex","");
  MAKE_DICT_ENTRY(dict,TAG_SENSE_METHOD,"SensingMethod","");
  MAKE_DICT_ENTRY(dict,TAG_FILE_SOURCE,"FileSource","");
  MAKE_DICT_ENTRY(dict,TAG_SCENE_TYPE,"SceneType","");
  MAKE_DICT_ENTRY(dict,TAG_CFA_PATTERN,"CFAPattern","");

  return JPEG_OK;
} // CJPEGMetaData::InitTiffDictionary()


JERRCODE CJPEGMetaData::GET_BYTE(Ipp8u* pData,int* curPos, int nDataSize, unsigned char* byte)
{
  if(*curPos >= nDataSize)
  {
    return JPEG_ERR_BUFF;
  }

  *byte = (unsigned char)pData[*curPos];
  *curPos += 1;

  return JPEG_OK;
} // CJPEGMetaData::GET_BYTE()


JERRCODE CJPEGMetaData::GET_WORD(Ipp8u* pData,int* curPos, int nDataSize, unsigned short* word)
{
  JERRCODE jerr;
  unsigned char byte0;
  unsigned char byte1;

  jerr = GET_BYTE(pData,curPos,nDataSize,&byte0);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  jerr = GET_BYTE(pData,curPos,nDataSize,&byte1);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  if(m_hdr.endianess == EXIF_LITTLE_ENDIAN)
    *word = (unsigned short)((byte1 << 8) | byte0);
  else
    *word = (unsigned short)((byte0 << 8) | byte1);

  return JPEG_OK;
} // CJPEGMetaData::GET_WORD()


JERRCODE CJPEGMetaData::GET_DWORD(Ipp8u* pData,int* curPos, int nDataSize, unsigned int* dword)
{
  JERRCODE jerr;
  unsigned short word0;
  unsigned short word1;

  jerr = GET_WORD(pData,curPos,nDataSize,&word0);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  jerr = GET_WORD(pData,curPos,nDataSize,&word1);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  if(m_hdr.endianess == EXIF_LITTLE_ENDIAN)
    *dword = (unsigned int)((word1 << 16) | word0);
  else
    *dword = (unsigned int)((word0 << 16) | word1);

  return JPEG_OK;
} // GET_DWORD()


JERRCODE CJPEGMetaData::ReadTiffHdr(TIFF_HEADER* hdr)
{
  JERRCODE jerr;

  jerr = GET_WORD(m_rawData,&m_currPos,m_nDataSize,&hdr->endianess);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  jerr = GET_WORD(m_rawData,&m_currPos,m_nDataSize,&hdr->magic);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  jerr = GET_DWORD(m_rawData,&m_currPos,m_nDataSize,&hdr->offset);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  return JPEG_OK;
} // CJPEGMetaData::ReadTiffHdr()


JERRCODE CJPEGMetaData::ReadIfdEntryVal(TIFF_IFD_ENTRY* entry)
{
  unsigned int i;
  JERRCODE jerr;

  switch(entry->type)
  {
  case TIFF_BYTE:
  case TIFF_ASCII:
  case TIFF_UNDEFINED:
    if(entry->count * sizeof(unsigned char) <= sizeof(unsigned int))
    {
      // entry->value contains actual value(s)
      for(i = 0; i < entry->count; i++)
      {
        entry->p.byte[i] = (unsigned char)((entry->value >> (24 - i*8)) & 0xff);
      }
    }
    else
    {
      // entry->value contains offset to actual value(s)
      for(i = 0; i < entry->count; i++)
      {
        jerr = GET_BYTE(m_rawData,(int*)&entry->value,m_nDataSize,&entry->p.byte[i]);
        if(JPEG_OK != jerr)
        {
          return jerr;
        }
      }
    }
    break;

  case TIFF_SHORT:
    if(entry->count * sizeof(unsigned short) <= sizeof(unsigned int))
    {
      // entry->value contains actual value(s)
      for(i = 0; i < entry->count; i++)
      {
        entry->p.word[i] = (unsigned short)((entry->value >> (16 - i*16)) & 0xffff);
      }
    }
    else
    {
      // entry->value contains offset to actual value(s)
      for(i = 0; i < entry->count; i++)
      {
        jerr = GET_WORD(m_rawData,(int*)&entry->value,m_nDataSize,&entry->p.word[i]);
        if(JPEG_OK != jerr)
        {
          return jerr;
        }
      }
    }
    break;

  case TIFF_LONG:
  case TIFF_SLONG:
    if(entry->count * sizeof(unsigned int) <= sizeof(unsigned int))
    {
      // entry->value contains actual value
      entry->p.dword[0] = entry->value;
    }
    else
    {
      // entry->value contains offset to actual value(s)
      for(i = 0; i < entry->count; i++)
      {
        jerr = GET_DWORD(m_rawData,(int*)&entry->value,m_nDataSize,&entry->p.dword[i]);
        if(JPEG_OK != jerr)
        {
          return jerr;
        }
      }
    }
    break;

  case TIFF_RATIONAL:
  case TIFF_SRATIONAL:
    {
      // entry->value contains offset to actual values
      for(i = 0; i < entry->count; i += 2)
      {
        jerr = GET_DWORD(m_rawData,(int*)&entry->value,m_nDataSize,&entry->p.dword[i+0]);
        if(JPEG_OK != jerr)
        {
          return jerr;
        }

        jerr = GET_DWORD(m_rawData,(int*)&entry->value,m_nDataSize,&entry->p.dword[i+1]);
        if(JPEG_OK != jerr)
        {
          return jerr;
        }
      }
    }
    break;

  default:
    break;
  }

  return JPEG_OK;
} // CJPEGMetaData::ReadIfdEntryVal()


JERRCODE CJPEGMetaData::ReadIfdEntry(TIFF_IFD_ENTRY* entry)
{
  int sz;
  JERRCODE jerr;

  jerr = GET_WORD(m_rawData,&m_currPos,m_nDataSize,(unsigned short*)&entry->tag);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  entry->name = (entry->tag > TAG_MAX) ? 0 : tiff_dict[entry->tag].name;

  jerr = GET_WORD(m_rawData,&m_currPos,m_nDataSize,(unsigned short*)&entry->type);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  jerr = GET_DWORD(m_rawData,&m_currPos,m_nDataSize,&entry->count);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  jerr = GET_DWORD(m_rawData,&m_currPos,m_nDataSize,&entry->value);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  sz = IPP_MAX(sizeof(unsigned int),entry->count * sizeof_val[entry->type]);

  entry->p.anyptr = ippMalloc(sz);
  if(0 == entry->p.anyptr)
  {
    return JPEG_ERR_ALLOC;
  }

  jerr = ReadIfdEntryVal(entry);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  return JPEG_OK;
} // CJPEGMetaData::ReadIfdEntry()


JERRCODE CJPEGMetaData::ReadIfdData(TIFF_IFD* ifd)
{
  int i;
  JERRCODE jerr;

  jerr = GET_WORD(m_rawData,&m_currPos,m_nDataSize,&ifd->count);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  ifd->entry = (TIFF_IFD_ENTRY**)ippMalloc(sizeof(TIFF_IFD_ENTRY*) * ifd->count);
  if(0 == ifd->entry)
  {
    return JPEG_ERR_ALLOC;
  }

  for(i = 0; i < ifd->count; i++)
  {
    ifd->entry[i] = (TIFF_IFD_ENTRY*)ippMalloc(sizeof(TIFF_IFD_ENTRY));
    if(0 == ifd->entry[i])
    {
      return JPEG_ERR_ALLOC;
    }

    ifd->entry[i]->tag      = (TIFF_TAG)0;
    ifd->entry[i]->type     = (TIFF_VALTYPE)0;
    ifd->entry[i]->count    = 0;
    ifd->entry[i]->value    = 0;
    ifd->entry[i]->p.anyptr = 0;
    ifd->entry[i]->name     = 0;

    jerr = ReadIfdEntry(ifd->entry[i]);
    if(JPEG_OK != jerr)
    {
      return jerr;
    }
  }

  jerr = GET_DWORD(m_rawData,&m_currPos,m_nDataSize,&ifd->next);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  return JPEG_OK;
} // CJPEGMetaData::ReadIfdData()


JERRCODE CJPEGMetaData::ProcessAPP1_Exif(Ipp8u* pData,int nDataSize)
{
  int i, j;
  JERRCODE jerr;

  m_rawData   = pData;
  m_currPos   = 0;
  m_nDataSize = nDataSize;

  jerr = ReadTiffHdr(&m_hdr);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  m_currPos = m_hdr.offset;

  // first IFD should be IFD0
  jerr = ReadIfdData(&m_ifd[TIFF_IFD_0]);
  if(JPEG_OK != jerr)
  {
    return jerr;
  }

  // The IFD0 can contain pointers to Exif IFD and/or GPS IFD
  for(i = 0; i < m_ifd[TIFF_IFD_0].count; i++)
  {
    if(m_ifd[TIFF_IFD_0].entry[i]->tag == TAG_EXIF_IFD_PTR)
    {
      m_currPos = m_ifd[TIFF_IFD_0].entry[i]->value;

      jerr = ReadIfdData(&m_ifd[TIFF_IFD_EXIF]);
      if(JPEG_OK != jerr)
      {
        return jerr;
      }

      // The Exif IFD can contain pointer to Interoperability IFD
      for(j = 0; j < m_ifd[TIFF_IFD_EXIF].count; j++)
      {
        if(m_ifd[TIFF_IFD_EXIF].entry[j]->tag == TAG_INTEROPERABILITY_IFD_PTR)
        {
          m_currPos = m_ifd[TIFF_IFD_EXIF].entry[j]->value;

          jerr = ReadIfdData(&m_ifd[TIFF_IFD_INTEROPERABILITY]);
          if(JPEG_OK != jerr)
          {
            return jerr;
          }
        }
      }
    }

    if(m_ifd[TIFF_IFD_0].entry[i]->tag == TAG_GPS_IFD_PTR)
    {
      m_currPos = m_ifd[TIFF_IFD_0].entry[i]->value;

      jerr = ReadIfdData(&m_ifd[TIFF_IFD_GPS]);
      if(JPEG_OK != jerr)
      {
        return jerr;
      }
    }
  }

  // the next IFD should be IFD1
  if(0 != m_ifd[TIFF_IFD_0].next)
  {
    m_currPos = m_ifd[TIFF_IFD_0].next;

    jerr = ReadIfdData(&m_ifd[TIFF_IFD_1]);
    if(JPEG_OK != jerr)
    {
      return jerr;
    }
  }

  return JPEG_OK;
} // CJPEGMetaData::ProcessAPP1_Exif()


const int BUFSIZE = 128;

char* CJPEGMetaData::ShowIfdEntry(TIFF_IFD_ENTRY* entry)
{
  int i;
  char buf[BUFSIZE];
  char* str;

  ippsZero_8u((Ipp8u*)buf,BUFSIZE);

  str = (char*)ippMalloc(BUFSIZE);
  if(0 == str)
    return 0;

  switch(entry->type)
  {
    case TIFF_BYTE:
    case TIFF_UNDEFINED:
      for(i = 0; i < IPP_MIN(BUFSIZE,(int)entry->count); i++)
      {
        buf[i] = entry->p.byte[i];
      }

      sprintf(str,"tag:0x%04X, name:%s, value:%s",entry->tag,entry->name,buf);
      break;

    case TIFF_ASCII:
      sprintf(str,"tag:0x%04X, name:%s, value:%s",entry->tag,entry->name,entry->p.byte);
      break;

    case TIFF_SHORT:
      sprintf(str,"tag:0x%04X, name:%s, value:%d",entry->tag,entry->name,*entry->p.word);
      break;

    case TIFF_LONG:
    case TIFF_SLONG:
      sprintf(str,"tag:0x%04X, name:%s, value:%d",entry->tag,entry->name,*entry->p.dword);
      break;

    case TIFF_RATIONAL:
    case TIFF_SRATIONAL:
      sprintf(str,"tag:0x%04X, name:%s, value:%d/%d",entry->tag,entry->name,entry->p.dword[0],entry->p.dword[1]);
      break;

    default:
      return 0;
  }

  return str;
} // CJPEGMetaData::ShowIfdEntry()


int CJPEGMetaData::GetIfdNumEntries(int ifdno)
{
  if(TIFF_IFD_MAX <= ifdno)
    return 0;

  return m_ifd[ifdno].count;
} // CJPEGMetaData::GetIfdNumEntries()


int CJPEGMetaData::GetJPEGThumbnailsLen(void)
{
  int i;
  int n;

  n = GetIfdNumEntries(TIFF_IFD_1);

  for(i = 0; i < n; i++)
  {
    if(m_ifd[TIFF_IFD_1].entry[i]->tag == TAG_LENGTH_OF_JPEG)
      return m_ifd[TIFF_IFD_1].entry[i]->value;
  }

  return 0;
} // CJPEGMetaData::GetJPEGThumbnailsLen()


Ipp8u* CJPEGMetaData::GetJPEGThumbnailsData(void)
{
  int i, n;

  n = GetIfdNumEntries(TIFF_IFD_1);

  for(i = 0; i < n; i++)
  {
    if(m_ifd[TIFF_IFD_1].entry[i]->tag == TAG_OFFSET_TO_JPEG)
      return m_rawData + m_ifd[TIFF_IFD_1].entry[i]->value;
  }

  return 0;
} // CJPEGMetaData::GetJPEGThumbnailsData()


char* CJPEGMetaData::ShowIfdData(int ifdno, int entryno)
{
  if(TIFF_IFD_MAX <= ifdno)
    return 0;

  if(m_ifd[ifdno].count < entryno)
    return 0;

  return ShowIfdEntry(m_ifd[ifdno].entry[entryno]);
} // CJPEGMetaData::ShowIfdData()


