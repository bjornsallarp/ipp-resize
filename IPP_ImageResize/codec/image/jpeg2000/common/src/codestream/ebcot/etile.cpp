
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2003-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/


#include <cfloat>
#include <climits>
#include "etile.h"
#include "dummyoutput.h"

const double ETile::m_rctDistWeights[3] =
{
    3.0000,
    0.6875,
    0.6875
};

const double ETile::m_ictDistWeights[3] =
{
    3.0000,
    3.2584,
    2.4755
};

ETile::ETile()
{}

void ETile::Init(
    const Rect *cmpRect,
    const Int2D &prXStepOrder, const Int2D &prYStepOrder, /* [nOfComponents][nOfWTLevels+1] */
    const Int2D &cbXStepOrder, const Int2D &cbYStepOrder,
    const int *cmpDynRange, const QuantComponent *cmpQuant,
    const int *nOfWTLevels, int nOfComponents)
{
    m_components.ReAlloc(nOfComponents);
    for(int i = 0; i < NOfComponents(); i++)
        m_components[i].Init(cmpRect[i],
            cmpDynRange[i], cmpQuant[i],
            prXStepOrder[i], prYStepOrder[i],
            cbXStepOrder[i], cbYStepOrder[i], nOfWTLevels[i]);
}

void ETile::Encode(CBEncoder *cbEncoder, const SBTree<Ipp32s> *sbTrees)
{
    for(int i = 0; i < NOfComponents(); i++)
        m_components[i].Encode(cbEncoder, sbTrees[i]);
}

void ETile::WeightDistAsWT53()
{
    for(int i = 0; i < NOfComponents(); i++)
        m_components[i].WeightDistAsWT53();
}

void ETile::WeightDistAsWT97()
{
    for(int i = 0; i < NOfComponents(); i++)
        m_components[i].WeightDistAsWT97();
}

void ETile::WeightDistAsRCT()
{
    for(int i = 0; i < 3; i++)
        m_components[i].ScaleDist(m_rctDistWeights[i]);
}

void ETile::WeightDistAsICT()
{
    for(int i = 0; i < 3; i++)
        m_components[i].ScaleDist(m_ictDistWeights[i]);
}

void ETile::SetCurPassToFirst()
{
    for(int i = 0; i < NOfComponents(); i++)
        m_components[i].SetCurPassToFirst();
}

int ETile::PacketsLengthSimulate(int layer)
{
    DummyOutput output;

    int nOfComponents = NOfComponents();
    for(int component = 0; component < nOfComponents; component++)
    {
        int nOfResLevels = NOfResLevels(component);
        for(int resLevel = 0; resLevel < nOfResLevels; resLevel++)
        {
            int nOfPrecincts = NOfPrecincts(component, resLevel);
            for(int precinct = 0; precinct < nOfPrecincts; precinct++)
            {
                WriteToPacket(component, resLevel, precinct, layer, output);
            }
        }
    }

    return output.Position();
}

void ETile::CalcRDSlopes()
{
    for(int i = 0; i < NOfComponents(); i++)
        m_components[i].CalcRDSlopes();
}

void ETile::RDSlopesMinMax(double &min, double &max) const
{
    min = DBL_MAX;
    max = 0;

    for(int i = 0; i < NOfComponents(); i++)
    {
        double tileMin;
        double tileMax;

        m_components[i].RDSlopesMinMax(tileMin, tileMax);

        if(tileMax > max) max = tileMax;
        if(tileMin < min) min = tileMin;
    }
}

void ETile::AssignLayerTillCurPass(int layer, double threshold)
{
    for(int i = 0; i < NOfComponents(); i++)
        m_components[i].AssignLayerTillCurPass(layer, threshold);
}

void ETile::AssignLayerTillCurPass(int layer)
{
    for(int i = 0; i < NOfComponents(); i++)
        m_components[i].AssignLayerTillCurPass(layer);
}

void ETile::StoreEncodingState()
{
    for(int i = 0; i < NOfComponents(); i++)
        m_components[i].StoreEncodingState();
}

void ETile::RestoreEncodingState()
{
    for(int i = 0; i < NOfComponents(); i++)
        m_components[i].RestoreEncodingState();
}

int ETile::NOfResLevels(int comp) const
{
    return m_components[comp].NOfResLevels();
}

int ETile::NOfPrecincts(int comp, int resLevel) const
{
    return m_components[comp].NOfPrecincts(resLevel);
}


void ETile::AssignLayers(int *lengths, int nOfLayers)
{
    double maxRDSlope;
    double minRDSlope;

    CalcRDSlopes();
    RDSlopesMinMax(minRDSlope, maxRDSlope);

    for (int layer = 0; layer < nOfLayers; ++layer)
    {
        double floor = minRDSlope;
        double ceil  = maxRDSlope;

        bool   success   = false;
        double threshold = 0;
        int    nOfIters  = 0;

        int length = lengths[layer];

        do
        {
            if (length == INT_MAX)
            {
                threshold = -1;
                success   = true;
                break;
            }

            double curThreshold = (floor + ceil) / 2.0;

            /* Save the tier 2 coding state. */
            StoreEncodingState();

            AssignLayerTillCurPass(layer, curThreshold);

            int  curLength = PacketsLengthSimulate(layer);

            if (curLength > length)
                floor = curThreshold;
            else
            {
                ceil = curThreshold;
                if (!success || curThreshold < threshold)
                {
                    threshold = curThreshold;
                    success = true;
                }
            }

            RestoreEncodingState();

            ++nOfIters;

        } while ( (ceil - floor) >= 1e-3 && nOfIters < 64);

        if(success)
            AssignLayerTillCurPass(layer, threshold);
        else
            AssignLayerTillCurPass(layer);
    }
}
