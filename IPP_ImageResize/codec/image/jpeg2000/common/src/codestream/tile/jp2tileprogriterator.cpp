
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

#include "jp2tileprogriterator.h"

JP2TileProgrIterator::JP2TileProgrIterator()
: m_codingStyle(0)
, m_sampleSizeRG(0)
, m_progrType(JP2V_LRCP_PROGR)
, m_layer(0)
, m_nOfLayers(0)
, m_nOfComponents(0)
, m_resLevel(0)
, m_resLevelFirst(0)
, m_resLevelBound(0)
, m_maxNOfResLevels(0)
, m_component(0)
, m_componentFirst(0)
, m_componentBound(0)
, m_isFirstVolume(true)
, m_precinctX(0)
, m_precinctY(0)
, m_x(0)
, m_originX(0)
, m_boundX(0)
, m_stepX(1)
, m_y(0)
, m_originY(0)
, m_boundY(0)
, m_stepY(1)
, m_isInRange(false)
, m_changedProgrRangeTilePart0(0)
, m_nOfProgrRangesTilePart0(0)
, m_currProgrTilePart0(0)
, m_changedProgrRange(0)
, m_nOfProgrRanges(0)
, m_currProgr(0)
{}

void JP2TileProgrIterator::InitAttachStartProgrRangeTilePart0(
    const Rect                      &tileRectRG,
    const RectSize                  *sampleSizeRG,
    const JP2CodingStyleJoint       &codingStyleJoint,
    const JP2CodingStyleCompTile    &codingStyle,
    const FixedArray<JP2ProgrRange> &changedProgrRange)
{
    m_sampleSizeRG   = sampleSizeRG;
    m_codingStyle    = &codingStyle;

    m_nOfLayers      = codingStyleJoint.NOfLayers();
    m_nOfComponents  = codingStyle.Size();

    m_resLevelFirst  = 0;
    m_resLevelBound  = 0;

    m_componentFirst = 0;
    m_componentBound = 0;

    m_originX = tileRectRG.X();
    m_boundX  = tileRectRG.X() + tileRectRG.Width();

    m_originY = tileRectRG.Y();
    m_boundY  = tileRectRG.Y() + tileRectRG.Height();


    m_isInRange = false;
    m_changedProgrRange = 0;
    m_nOfProgrRanges = 0;
    m_currProgr = 0;
    m_changedProgrRangeTilePart0 = 0;
    m_nOfProgrRangesTilePart0 = 0;
    m_currProgrTilePart0 = 0;
    m_changedProgrRange = 0;
    m_nOfProgrRanges = 0;
    m_currProgr = 0;

    m_maxNOfResLevels = 0;
    unsigned int component = 0;
    for ( ; component < NOfComponents(); component++)
    {
        unsigned int nOfResLevels  = CodingStyle(component).NOfResLevels();
        m_maxNOfResLevels = Max(nOfResLevels, m_maxNOfResLevels);
    }

    m_precinctLayer.ReAlloc(NOfComponents());

    for (component = 0; component < NOfComponents(); component++)
    {
        const RectSize *precStepOrder = CodingStyle(component).PrecStepOrder();
        m_precinctLayer[component].ReAlloc(m_maxNOfResLevels);

        Rect compRect = DecimateRect(tileRectRG, m_sampleSizeRG[component]);

        unsigned int nOfResLevels  = CodingStyle(component).NOfResLevels();
        for (unsigned int resLevel  = 0; resLevel  < m_maxNOfResLevels; resLevel ++)
        {
            if(resLevel > nOfResLevels) m_precinctLayer[component][resLevel].ReAlloc(0, 0);
            else
                m_precinctLayer[component][resLevel].ReAlloc
                (
                    NOfPrecincts(compRect.RangeY(), resLevel, nOfResLevels, precStepOrder[resLevel].Height()),
                    NOfPrecincts(compRect.RangeX(), resLevel, nOfResLevels, precStepOrder[resLevel].Width ())
                );
        }
    }

    m_progrRangeDefault.SetProgrType    (codingStyleJoint.ProgrType());
    m_progrRangeDefault.SetResLevelRange(UIntRange(0, m_maxNOfResLevels));
    m_progrRangeDefault.SetCompRange    (UIntRange(0, NOfComponents()));
    m_progrRangeDefault.SetNOfLayers    (NOfLayers());

    if(changedProgrRange.Size())
    {
        m_changedProgrRange =  changedProgrRange;
        m_nOfProgrRanges    =  changedProgrRange.Size();
        m_currProgr         = 0;
    }
    else
    {
        m_changedProgrRange = &m_progrRangeDefault;
        m_nOfProgrRanges    =  1;
        m_currProgr         =  0;
    }

    // just flags to store actual value on AttachChangeProgrRangeTilePartN call
    m_currProgrTilePart0         = 0xFFFFFFFF;
    m_changedProgrRangeTilePart0 = changedProgrRange;
    m_nOfProgrRangesTilePart0    = m_nOfProgrRanges;

    SetProgrRange(m_changedProgrRange[m_currProgr]);
    Start();
}

void JP2TileProgrIterator::AttachChangeProgrRangeTilePartN(
    const FixedArray<JP2ProgrRange> &changedProgrRange)
{
    if(changedProgrRange.Size())
    {
        if(m_currProgrTilePart0==0xFFFFFFFF)
            m_currProgrTilePart0 = m_currProgr;

        m_changedProgrRange =  changedProgrRange;
        m_nOfProgrRanges    =  changedProgrRange.Size();
        m_currProgr         = 0;
        SetProgrRange(m_changedProgrRange[m_currProgr]);
    }
    else RestoreChangeProgrRangeTilePart0();
}

void JP2TileProgrIterator::RestoreChangeProgrRangeTilePart0()
{
    if(m_currProgrTilePart0!=0xFFFFFFFF)
    {
        m_changedProgrRange =  m_changedProgrRangeTilePart0;
        m_nOfProgrRanges    =  m_nOfProgrRangesTilePart0;
        m_currProgr         =  m_currProgrTilePart0;
        SetProgrRange(m_changedProgrRange[m_currProgr]);
    }
    else RestoreProgrRangeDefault();
}

void JP2TileProgrIterator::RestoreProgrRangeDefault()
{
    m_changedProgrRange = &m_progrRangeDefault;
    m_nOfProgrRanges    =  1;
    m_currProgr         =  0;

    SetProgrRange(m_changedProgrRange[m_currProgr]);
}


void JP2TileProgrIterator::SetProgrRange(const JP2ProgrRange &progrRange)
{
    m_progrType      = progrRange.ProgrType    ();

    m_resLevelFirst  = progrRange.ResLevelRange().Origin();
    m_resLevelBound  = Min(m_maxNOfResLevels, progrRange.ResLevelRange().Bound ());

    m_componentFirst = progrRange.CompRange    ().Origin();
    m_componentBound = Min(m_nOfComponents  , progrRange.CompRange    ().Bound ());

    m_nOfLayers      = progrRange.NOfLayers    ();
}

void  JP2TileProgrIterator::operator++(int)
{
    if( !Step() )
    {
        m_currProgr++;
        if(m_currProgr < m_nOfProgrRanges)
        {
            SetProgrRange(m_changedProgrRange[m_currProgr]);
            m_isInRange = Step();
        }
        else
        {
            if(m_changedProgrRange != &m_progrRangeDefault)
            {
                RestoreProgrRangeDefault();
                m_isInRange = Step();
            }
            else m_isInRange = false;
        }
    }
    else m_isInRange = true;

    return;
}

bool JP2TileProgrIterator::Step()
{
    switch(m_progrType)
    {
    case JP2V_LRCP_PROGR: return StepLRCP();
    case JP2V_RLCP_PROGR: return StepRLCP();
    case JP2V_RPCL_PROGR: return StepRPCL();
    case JP2V_PCRL_PROGR: return StepPCRL();
    default: /*JP2V_CPRL_PROGR*/
                          return StepCPRL();
    }
}


void JP2TileProgrIterator::ClearPrecinctLayer()
{
    for (unsigned int component = 0; component < NOfComponents(); component++)
    {
        unsigned int  nOfResLevels  = CodingStyle(component).NOfResLevels();

        for (unsigned int resLevel  = 0; resLevel  < nOfResLevels                                  ; resLevel ++)
        for (unsigned int precinctY = 0; precinctY < m_precinctLayer[component][resLevel].NOfRows(); precinctY++)
        for (unsigned int precinctX = 0; precinctX < m_precinctLayer[component][resLevel].NOfCols(); precinctX++)
        {
            m_precinctLayer[component][resLevel][precinctY][precinctX] = 0;
        }
    }
}


void JP2TileProgrIterator::MinimizeStepPerComponent(unsigned int component)
{
    unsigned int    nOfResLevels  = CodingStyle(m_component).NOfResLevels ();
    const RectSize *precStepOrder = CodingStyle(m_component).PrecStepOrder();

    for (unsigned int resLevel = 0 ; resLevel < nOfResLevels; ++resLevel)
    {
        unsigned int stepX = m_sampleSizeRG[component].Width () * (1 << (precStepOrder[resLevel].Width () + nOfResLevels - resLevel - 1));
        unsigned int stepY = m_sampleSizeRG[component].Height() * (1 << (precStepOrder[resLevel].Height() + nOfResLevels - resLevel - 1));
        m_stepX = (!m_stepX) ? stepX : Min(m_stepX, stepX);
        m_stepY = (!m_stepY) ? stepY : Min(m_stepY, stepY);
    }
}

void JP2TileProgrIterator::OptimizeStepPerComponent(unsigned int component)
{
    m_stepX = 0; m_stepY = 0;
    MinimizeStepPerComponent(component);
}

void JP2TileProgrIterator::OptimizeStepAll()
{
    m_stepX = 0; m_stepY = 0;
    for (unsigned int component = 0; component < NOfComponents(); component++)
        MinimizeStepPerComponent(component);
}

// pcrl, rpcl,
bool JP2TileProgrIterator::DoPrecinctChange(const RectSize &precStepOrder, unsigned int nOfResLevels)
{
    unsigned int r    = nOfResLevels - 1 - m_resLevel;
    unsigned int rpx  = r + precStepOrder.Width() ;
    unsigned int rpy  = r + precStepOrder.Height();

    unsigned int trx0 = DivCeil(m_originX, m_sampleSizeRG[m_component].Width()  << r);
    unsigned int try0 = DivCeil(m_originY, m_sampleSizeRG[m_component].Height() << r);

    bool res =  ((m_x == m_originX && ((trx0 << r) % (1 << rpx))) || !(m_x % (1 << rpx)))
             && ((m_y == m_originY && ((try0 << r) % (1 << rpy))) || !(m_y % (1 << rpy)));

    if(res)
    {
        m_precinctX = RShiftFloor(DivCeil(m_x, m_sampleSizeRG[m_component].Width()  << r), precStepOrder.Width() ) - RShiftFloor(trx0, precStepOrder.Width() );
        m_precinctY = RShiftFloor(DivCeil(m_y, m_sampleSizeRG[m_component].Height() << r), precStepOrder.Height()) - RShiftFloor(try0, precStepOrder.Height());
    }

    return res;
}

bool JP2TileProgrIterator::DoPrecinctChangeCPRL(const RectSize &precStepOrder, unsigned int nOfResLevels)
{
    unsigned int r    = nOfResLevels - 1 - m_resLevel;
    unsigned int rpx  = r + precStepOrder.Width() ;
    unsigned int rpy  = r + precStepOrder.Height();

    unsigned int trx0 = DivCeil(m_originX, m_sampleSizeRG[m_component].Width()  << r);
    unsigned int try0 = DivCeil(m_originY, m_sampleSizeRG[m_component].Height() << r);

    bool res =  ((m_x == m_originX && ((trx0 << r) % (1 << rpx))) || !(m_x % (m_sampleSizeRG[m_component].Width()  << rpx)))
             && ((m_y == m_originY && ((try0 << r) % (1 << rpy))) || !(m_y % (m_sampleSizeRG[m_component].Height() << rpy)));

    if(res)
    {
        m_precinctX = RShiftFloor(DivCeil(m_x, m_sampleSizeRG[m_component].Width()  << r), precStepOrder.Width() ) - RShiftFloor(trx0, precStepOrder.Width() );
        m_precinctY = RShiftFloor(DivCeil(m_y, m_sampleSizeRG[m_component].Height() << r), precStepOrder.Height()) - RShiftFloor(try0, precStepOrder.Height());
    }

    return res;
}

bool JP2TileProgrIterator::StepLRCP()
{
    if (!m_isFirstVolume)  goto enter;
    else m_isFirstVolume = false;

    for (m_layer     =                0; m_layer     < NOfLayers     (); m_layer    ++)
    for (m_resLevel  = ResLevelFirst (); m_resLevel  < ResLevelBound (); m_resLevel ++)
    for (m_component = ComponentFirst(); m_component < ComponentBound(); m_component++)
    {
        if (m_resLevel >= CodingStyle(m_component).NOfResLevels()) continue;

        for (m_precinctY = 0; m_precinctY < PrecinctNOfRows(); m_precinctY++)
        for (m_precinctX = 0; m_precinctX < PrecinctNOfCols(); m_precinctX++)
        {
            if (m_layer >= PrecinctLayer())
            {
                PrecinctLayer() = m_layer;
                PrecinctLayer()++;
                return true;
            }
enter:
            ;
        }
    }
    return false;
}

bool JP2TileProgrIterator::StepRLCP()
{
    if (!m_isFirstVolume)  goto enter;
    else m_isFirstVolume = false;

    for (m_resLevel  = ResLevelFirst (); m_resLevel  < ResLevelBound (); m_resLevel ++)
    for (m_layer     =                0; m_layer     < NOfLayers     (); m_layer    ++)
    for (m_component = ComponentFirst(); m_component < ComponentBound(); m_component++)
    {
        if (m_resLevel >= CodingStyle(m_component).NOfResLevels()) continue;

        for (m_precinctY = 0; m_precinctY < PrecinctNOfRows(); m_precinctY++)
        for (m_precinctX = 0; m_precinctX < PrecinctNOfCols(); m_precinctX++)
        {
            if (m_layer >= PrecinctLayer())
            {
                PrecinctLayer() = m_layer;
                PrecinctLayer()++;
                return true;
            }
enter:
            ;
        }
    }
    return false;
}

bool JP2TileProgrIterator::StepRPCL()
{
    unsigned int    nOfResLevels;
    const RectSize *precStepOrder;

    if (!m_isFirstVolume)
    {
        nOfResLevels  = CodingStyle(m_component).NOfResLevels ();
        precStepOrder = CodingStyle(m_component).PrecStepOrder();
        goto  enter;
    }
    else
    {
        OptimizeStepAll();
        m_isFirstVolume = false;
    }


    for (m_resLevel  = ResLevelFirst() ; m_resLevel  < ResLevelBound (); m_resLevel++)
    for (m_y         = m_originY       ; m_y         < m_boundY        ; m_y += m_stepY - (m_y % m_stepY))
    for (m_x         = m_originX       ; m_x         < m_boundX        ; m_x += m_stepX - (m_x % m_stepX))
    for (m_component = ComponentFirst(); m_component < ComponentBound(); m_component++)
    {
        nOfResLevels  = CodingStyle(m_component).NOfResLevels ();
        precStepOrder = CodingStyle(m_component).PrecStepOrder();

        if (m_resLevel >= nOfResLevels)  continue;
        if (IsEmptyResLevel())           continue;

        if (DoPrecinctChange(precStepOrder[m_resLevel], nOfResLevels))
        {
            for (m_layer = 0; m_layer < NOfLayers(); m_layer++)
            {
                if (m_layer >= PrecinctLayer())
                {
                    PrecinctLayer()++;
                    return true;
                }
enter:
                ;
            }
        }
    }
    return false;
}

bool JP2TileProgrIterator::StepPCRL()
{
    unsigned int    nOfResLevels;
    const RectSize *precStepOrder;

    if (!m_isFirstVolume)
    {
        nOfResLevels  = CodingStyle(m_component).NOfResLevels ();
        precStepOrder = CodingStyle(m_component).PrecStepOrder();
        goto  enter;
    }
    else
    {
        OptimizeStepAll();
        m_isFirstVolume = false;
    }

    for (m_y         =        m_originY; m_y         < m_boundY        ; m_y += m_stepY - (m_y % m_stepY))
    for (m_x         =        m_originX; m_x         < m_boundX        ; m_x += m_stepX - (m_x % m_stepX))
    for (m_component = ComponentFirst(); m_component < ComponentBound(); m_component++)
    {
        nOfResLevels  = CodingStyle(m_component).NOfResLevels ();
        precStepOrder = CodingStyle(m_component).PrecStepOrder();

        for (m_resLevel = ResLevelFirst(); m_resLevel < nOfResLevels && m_resLevel < ResLevelBound(); ++m_resLevel)
        {
            if (IsEmptyResLevel()) continue;

            if (DoPrecinctChange(precStepOrder[m_resLevel], nOfResLevels))
            {
                for (m_layer = 0; m_layer < NOfLayers(); m_layer++)
                {
                    if (m_layer >= PrecinctLayer())
                    {
                        PrecinctLayer()++;
                        return true;
                    }
enter:
                    ;
                }
            }
        }
    }

    return false;
}

bool JP2TileProgrIterator::StepCPRL()
{
    unsigned int    nOfResLevels;
    const RectSize *precStepOrder;

    if (!m_isFirstVolume)
    {
        nOfResLevels  = CodingStyle(m_component).NOfResLevels ();
        precStepOrder = CodingStyle(m_component).PrecStepOrder();
        goto  enter;
    }
    else m_isFirstVolume = false;

    for (m_component = ComponentFirst(); m_component < ComponentBound(); ++m_component)
    {
        nOfResLevels  = CodingStyle(m_component).NOfResLevels ();
        precStepOrder = CodingStyle(m_component).PrecStepOrder();

        OptimizeStepPerComponent(m_component);

        for (m_y = m_originY; m_y < m_boundY; m_y += m_stepY - (m_y % m_stepY))
        for (m_x = m_originX; m_x < m_boundX; m_x += m_stepX - (m_x % m_stepX))
        {
            for (m_resLevel = ResLevelFirst(); m_resLevel < nOfResLevels && m_resLevel < ResLevelBound(); ++m_resLevel)
            {
                if (IsEmptyResLevel()) continue;

                if (DoPrecinctChangeCPRL(precStepOrder[m_resLevel], nOfResLevels))
                {
                    for (m_layer = 0; m_layer < NOfLayers(); m_layer++)
                    {
                        if (m_layer >= PrecinctLayer())
                        {
                            PrecinctLayer()++;
                            return true;
                        }
enter:
                        ;
                    }
                }
            }
        }
    }
    return false;
}
