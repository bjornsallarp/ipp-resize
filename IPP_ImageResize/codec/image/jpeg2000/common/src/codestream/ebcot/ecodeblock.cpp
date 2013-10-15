
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2002-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#include <cfloat>
#include "ecodeblock.h"

const double ECodeBlock::m_BadRDSlope = -1.0;

ECodeBlock::ECodeBlock()
: m_curPass(m_passes.ItrBackBound())
, m_Lblock(3)
, m_isFirstInclusion(true)
, m_curPassStore(m_passes.ItrBackBound())
, m_LblockStore(3)
, m_isFirstInclusionStore(true)
{}

void ECodeBlock::SetNOfZeroBits(int nOfZeroBits)
{
    m_nOfZeroBits = nOfZeroBits;
}

void ECodeBlock::PushPass(int end, double dist)
{
    int start = 0;
    if( ! m_passes.IsEmpty() )
        start = m_passes.Back().m_end;

    m_passes.PushBack();
    m_passes.Back().m_start= start;
    m_passes.Back().m_end  = end;
    m_passes.Back().m_dist = dist;
    m_nOfPasses++;

    m_passes.Back().m_data = &m_data[m_passes.Back().m_start];
}

void ECodeBlock::ScaleDist(double factor)
{
    for (ECBPassIterator pass = m_passes.ItrFront(); pass != m_passes.ItrBackBound(); ++pass)
        pass->m_dist*=factor;
}

void ECodeBlock::CalcRDSlopes()
{
    ECBPassIterator pass2 = m_passes.ItrFront();

    while (pass2 != m_passes.ItrBackBound())
    {
        ECBPassIterator pass0 = m_passes.ItrBackBound();

        for (ECBPassIterator pass1 = m_passes.ItrFront(); pass1 != m_passes.ItrBackBound(); ++pass1)
        {
            double deltaDist = pass1->m_dist;
            int    deltaRate = pass1->m_end;

            if(pass0 != m_passes.ItrBackBound())
            {
                deltaDist -= pass0->m_dist;
                deltaRate -= pass0->m_end;
            }

            if (deltaDist > 0)
            {
                if (pass1 == pass2 || pass1->m_rdSlope > 0)
                {

                    if (!deltaRate)
                    {
                        pass0->m_rdSlope = 0;
                        break;
                    }

                    double slope = deltaDist / (double)deltaRate;

                    if (pass0!=m_passes.ItrBackBound())
                    {
                        if(slope >= pass0->m_rdSlope)
                        {
                            pass0->m_rdSlope = 0;
                            break;
                        }
                    }

                    pass0            = pass1;
                    pass1->m_rdSlope = slope;

               }
            }
            else pass1->m_rdSlope = m_BadRDSlope;

            if (pass1 == pass2) ++pass2;
        }
    }
}

void ECodeBlock::RDSlopesMinMax(double &min, double &max) const
{
    min = DBL_MAX;
    max = 0;

    for (ECBPassIterator pass = m_passes.ItrFront(); pass != m_passes.ItrBackBound(); ++pass)
    {
        if(pass->m_rdSlope > 0)
        {
            if(pass->m_rdSlope > max) max = pass->m_rdSlope;
            if(pass->m_rdSlope < min) min = pass->m_rdSlope;
        }
    }
}

void ECodeBlock::AssignLayerTillCurPass(int layer, double threshold)
{
    if (NOfPasses())
    {
        ECBPassIterator passThresh = m_curPass;
        ECBPassIterator pass       = m_curPass;

        for (pass = m_curPass; pass != m_passes.ItrBackBound(); ++pass)
        {
            if (pass->m_rdSlope >= threshold)
            {
                passThresh = pass;
                ++passThresh;
            }
        }

        for (pass = m_curPass; pass != passThresh; ++pass)
            pass->m_layer = layer;

        for (; pass != m_passes.ItrBackBound(); ++pass)
            pass->m_layer = -1;
    }
}

void ECodeBlock::AssignLayerTillCurPass(int layer)
{
    if(NOfPasses())
    {
        ECBPassIterator pass = m_passes.ItrFront();
        for (; pass != m_curPass; ++pass)
            pass->m_layer = layer;


        for (; pass != m_passes.ItrBackBound(); ++pass)
            pass->m_layer = -1;
    }
}
