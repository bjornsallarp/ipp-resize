/*
//
//             INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2006-2012 Intel Corporation. All Rights Reserved.
//
*/
#include "stdafx.h"
#include <stdlib.h>
#include "uic_new.h"

using namespace UIC;

static const unsigned int ARR_HDR_SIZE = 32;

void* UIC::ArrAlloc   (Ipp32u itemSize, Ipp32u nOfItems)
{
    void *buff = malloc(itemSize * nOfItems + ARR_HDR_SIZE);
    unsigned int *countOf = (unsigned int*)buff;

    *countOf = nOfItems;

    return ((char*)buff) + ARR_HDR_SIZE;
}

void  UIC::ArrFree    (const void* arr)
{
    char *tmp = (char*) arr;
    tmp -= ARR_HDR_SIZE;
    free(tmp);
}

Ipp32u UIC::ArrCountOf(const void* arr)
{
    char *tmp = (char*) arr;
    tmp -= ARR_HDR_SIZE;
    return *((unsigned int *)tmp );
}
