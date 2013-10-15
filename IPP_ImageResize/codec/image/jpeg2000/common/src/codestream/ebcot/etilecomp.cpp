
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
#include "etilecomp.h"

const double ETileComp::m_wt53DistWeightsLxLy[33] =
{
    1.00000000000000E+00,
    2.25000000000000E+00,
    7.56250000000000E+00,
    2.88906250000000E+01,
    1.14222656250000E+02,
    4.55555664062500E+02,
    1.82088891601562E+03,
    7.28222222900390E+03,
    2.91275555572509E+04,
    1.16508888889312E+05,
    4.66034222222328E+05,
    1.86413555555558E+06,
    7.45654088888889E+06,
    2.98261622222222E+07,
    1.19304647555555E+08,
    4.77218588888888E+08,
    1.90887435422222E+09,
    7.63549741555555E+09,
    3.05419896603597E+10,
    1.22167958641931E+11,
    4.88671834570569E+11,
    1.95468733828742E+12,
    7.81874935313183E+12,
    3.12749974124875E+13,
    1.25099989649658E+14,
    5.00399958598306E+14,
    2.00159983439430E+15,
    8.00639933762275E+15,
    3.20255973503885E+16,
    1.28102389403149E+17,
    5.12409557616726E+17,
    2.04963822686699E+18,
    8.19855291546567E+18
};

const double ETileComp::m_wt53DistWeightsLxHyHxLy[33] =
{
    0.00000000000000E+00,
    1.07812499999999E+00,
    2.53515625000000E+00,
    8.52441406250000E+00,
    3.25217285156250E+01,
    1.28521057128906E+02,
    5.12520889282226E+02,
    2.04852084732055E+03,
    8.19252083683013E+03,
    3.27685208342075E+04,
    1.31072520833551E+05,
    5.24288520833387E+05,
    2.09715252083334E+06,
    8.38860852083333E+06,
    3.35544325208333E+07,
    1.34217728520833E+08,
    5.36870912520833E+08,
    2.14748364852083E+09,
    8.58993459238392E+09,
    3.43597383685714E+10,
    1.37438953470957E+11,
    5.49755813893781E+11,
    2.19902325557188E+12,
    8.79609302229619E+12,
    3.51843720890597E+13,
    1.40737488356084E+14,
    5.62949953423091E+14,
    2.25179981369054E+15,
    9.00719925474247E+15,
    3.60287970188289E+16,
    1.44115188074136E+17,
    5.76460752297632E+17,
    2.30584300918511E+18
};

const double ETileComp::m_wt53DistWeightsHxHy[33] =
{
    0.00000000000000E+00,
    5.16601562500000E-01,
    8.49853515625000E-01,
    2.51519775390625E+00,
    9.25965881347656E+00,
    3.62582740783691E+01,
    1.44257927894592E+02,
    5.76257841348648E+02,
    2.30425781971216E+03,
    9.21625781430304E+03,
    3.68642578129507E+04,
    1.47456257812612E+05,
    5.89824257812528E+05,
    2.35929625781250E+06,
    9.43718425781250E+06,
    3.77487362578125E+07,
    1.50994944257812E+08,
    6.03979776257812E+08,
    2.41591910422265E+09,
    9.66367641630925E+09,
    3.86547056631398E+10,
    1.54618822657750E+11,
    6.18475290630590E+11,
    2.47390116253038E+12,
    9.89560465007430E+12,
    3.95824186002364E+13,
    1.58329674400459E+14,
    6.33318697600811E+14,
    2.53327479039642E+15,
    1.01330991615292E+16,
    4.05323966456669E+16,
    1.62129586583241E+17,
    6.48518346330914E+17
};



const double ETileComp::m_wt97DistWeightsLxLy[33] =
{
    1.00000000000000E+00,
    3.86479156950064E+00,
    1.69942631689972E+01,
    7.08415825571010E+01,
    2.86813601440513E+02,
    1.15090065853513E+03,
    4.60730956366782E+03,
    1.84329626180208E+04,
    7.37355796680939E+04,
    2.94946049176288E+05,
    1.17978792755704E+06,
    4.71915544117161E+06,
    1.88766254956531E+07,
    7.55065057135853E+07,
    3.02026026585299E+08,
    1.20810411007217E+09,
    4.83241644401977E+09,
    1.93296657798112E+10,
    7.73186631229713E+10,
    3.09274652495580E+11,
    1.23709860997853E+12,
    4.94839443991639E+12,
    1.97935777595912E+13,
    7.91743110386140E+13,
    3.16697244136876E+14,
    1.26678897661055E+15,
    5.06715590609650E+15,
    2.02686235954946E+16,
    8.10744943275693E+16,
    3.24297977491264E+17,
    1.29719190909511E+18,
    5.18876763442348E+18,
    2.07550705298661E+19
};

const double ETileComp::m_wt97DistWeightsLxHyHxLy[33] =
{
    0.00000000000000E+00,
    1.02270033578581E+00,
    3.98725998904926E+00,
    1.75005622549498E+01,
    7.28311275711542E+01,
    2.94696473402807E+02,
    1.18234209028184E+03,
    4.73298083281588E+03,
    1.89355520197785E+04,
    7.57458412743299E+04,
    3.02986999514585E+05,
    1.21195163280124E+06,
    4.84781016603341E+06,
    1.93912442989845E+07,
    7.75649808307914E+07,
    3.10259926958032E+08,
    1.24103971146686E+09,
    4.96415884950230E+09,
    1.98566354016429E+10,
    7.94265416101189E+10,
    3.17706166443120E+11,
    1.27082466577461E+12,
    5.08329866309333E+12,
    2.03331946521769E+13,
    8.13327786045902E+13,
    3.25331114424021E+14,
    1.30132445737975E+15,
    5.20529782473854E+15,
    2.08211912922855E+16,
    8.32847650948469E+16,
    3.33139060382217E+17,
    1.33255624099058E+18,
    5.33022496180917E+18
};

const double ETileComp::m_wt97DistWeightsHxHy[33] =
{
    0.00000000000000E+00,
    2.70626748689468E-01,
    9.35506415440028E-01,
    4.32330374596743E+00,
    1.84941478250845E+01,
    7.54591725984318E+01,
    3.03416299498477E+02,
    1.21527439880457E+03,
    4.86271528491000E+03,
    1.94524811787756E+04,
    7.78115453892983E+04,
    3.11247802400201E+05,
    1.24499283048809E+06,
    4.97997294285127E+06,
    1.99198933923063E+07,
    7.96795751901319E+07,
    3.18718302381363E+08,
    1.27487321114626E+09,
    5.09949284620566E+09,
    2.03979713864010E+10,
    8.15918855472120E+10,
    3.26367542189794E+11,
    1.30547016876145E+12,
    5.22188067492852E+12,
    2.08875226987586E+13,
    8.35500907937832E+13,
    3.34200363035455E+14,
    1.33680145159193E+15,
    5.34720580653104E+15,
    2.13888231760270E+16,
    8.55552927400537E+16,
    3.42221170778466E+17,
    1.36888468238688E+18
};



void ETileComp::Init(
    const Rect &cmpRect,
    int cmpDynRange, const QuantComponent &quant,
    const int *prXStepOrder, const int *prYStepOrder, /* [nOfWTLevels+1] */
    const int *cbXStepOrder, const int *cbYStepOrder,
    int nOfWTLevels)
{
    m_triplets.ReAlloc(nOfWTLevels);

    m_LxLy.Init(cmpRect,
        prXStepOrder[0], prYStepOrder[0], cbXStepOrder[0], cbYStepOrder[0],
        nOfWTLevels, cmpDynRange, quant.LxLy().Expn());

    for(int sbDepth = 0; sbDepth < nOfWTLevels; sbDepth++)
    {
        int resLevel = ResLevel(sbDepth, nOfWTLevels);
        m_triplets[sbDepth].Init(cmpRect,
            prXStepOrder[resLevel], prYStepOrder[resLevel],
            cbXStepOrder[resLevel], cbYStepOrder[resLevel],
            sbDepth,
            cmpDynRange,
            quant.HxLy(sbDepth).Expn(),
            quant.LxHy(sbDepth).Expn(),
            quant.HxHy(sbDepth).Expn()
            );
    }
}

void ETileComp::Encode(CBEncoder *cbEncoder, const SBTree<Ipp32s> &sbTree)
{
    m_LxLy.Encode(cbEncoder, sbTree.LxLy());

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
        m_triplets[sbDepth].Encode(cbEncoder, sbTree.Triplet(sbDepth));
}

void ETileComp::WeightDistAsWT53()
{
    m_LxLy.ScaleDist(m_wt53DistWeightsLxLy[NOfWTLevels()]);

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
    {
        int wtLevel = WTLevel(sbDepth);

        m_triplets[sbDepth].ScaleDist(
            m_wt53DistWeightsLxHyHxLy[wtLevel],
            m_wt53DistWeightsLxHyHxLy[wtLevel],
            m_wt53DistWeightsHxHy    [wtLevel]
            );
    }
}

void ETileComp::WeightDistAsWT97()
{
    m_LxLy.ScaleDist(m_wt97DistWeightsLxLy[NOfWTLevels()]);

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
    {
        int wtLevel = WTLevel(sbDepth);

        m_triplets[sbDepth].ScaleDist(
            m_wt97DistWeightsLxHyHxLy[wtLevel],
            m_wt97DistWeightsLxHyHxLy[wtLevel],
            m_wt97DistWeightsHxHy    [wtLevel]
            );
    }
}

void ETileComp::ScaleDist(double factor)
{
    m_LxLy.ScaleDist(factor);

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
        m_triplets[sbDepth].ScaleDist(factor);
}

void ETileComp::SetCurPassToFirst()
{
    m_LxLy.SetCurPassToFirst();

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
        m_triplets[sbDepth].SetCurPassToFirst();
}

void ETileComp::CalcRDSlopes()
{
    m_LxLy.CalcRDSlopes();

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
        m_triplets[sbDepth].CalcRDSlopes();
}

void ETileComp::RDSlopesMinMax(double &min, double &max) const
{
    m_LxLy.RDSlopesMinMax(min, max);

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
    {
        double tileMin;
        double tileMax;

        m_triplets[sbDepth].RDSlopesMinMax(tileMin, tileMax);

        if(tileMax > max) max = tileMax;
        if(tileMin < min) min = tileMin;
    }
}

void ETileComp::AssignLayerTillCurPass(int layer, double threshold)
{
    m_LxLy.AssignLayerTillCurPass(layer, threshold);

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
        m_triplets[sbDepth].AssignLayerTillCurPass(layer, threshold);
}

void ETileComp::AssignLayerTillCurPass(int layer)
{
    m_LxLy.AssignLayerTillCurPass(layer);

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
        m_triplets[sbDepth].AssignLayerTillCurPass(layer);
}

void ETileComp::StoreEncodingState()
{
    m_LxLy.StoreEncodingState();

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
        m_triplets[sbDepth].StoreEncodingState();
}

void ETileComp::RestoreEncodingState()
{
    m_LxLy.RestoreEncodingState();

    for(int sbDepth = 0; sbDepth < NOfWTLevels(); sbDepth++)
        m_triplets[sbDepth].RestoreEncodingState();
}

int ETileComp::NOfPrecincts(int resLevel) const
{
    if(!resLevel) return m_LxLy.NOfPrecincts();

    int sbDepth = SBDepth(resLevel, NOfWTLevels());
    return m_triplets[sbDepth].NOfPrecincts();
}
