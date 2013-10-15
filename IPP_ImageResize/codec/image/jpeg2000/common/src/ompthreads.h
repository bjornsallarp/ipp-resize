/*
//
//                INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2011-2012 Intel Corporation. All Rights Reserved.
//
*/

#ifdef _OPENMP
#include <omp.h>
#endif

static int get_num_threads(void)
{
  int maxThreads = 1;
#ifdef _OPENMP
#ifdef __INTEL_COMPILER
  kmp_set_blocktime(0);
#endif
#pragma omp parallel shared(maxThreads)
  {
#pragma omp master
    {
      maxThreads = omp_get_num_threads();
    }
  }
#endif
  return maxThreads;
} // get_num_threads()


static void set_num_threads(int maxThreads)
{
  maxThreads = maxThreads <= 0 ? 1 : maxThreads;
#ifdef _OPENMP
      omp_set_num_threads(maxThreads);
#endif
  return;
} // set_num_threads()

