
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

#ifndef __JP2TILEPROGRITERATOR_H__
#define __JP2TILEPROGRITERATOR_H__

#include "jp2codingstylejoint.h"
#include "jp2codingstylecomp.h"
#include "jp2progrrange.h"
#include "decimetric.h"
#include "wtmetric.h"

class JP2TileProgrIterator
{
public:
    JP2TileProgrIterator();

    void InitAttachStartProgrRangeTilePart0(
        const Rect                      &tileRectRG,
        const RectSize                  *sampleSizeRG,
        const JP2CodingStyleJoint       &codingStyleJoint,
        const JP2CodingStyleCompTile    &codingStyle,
        const FixedArray<JP2ProgrRange> &changedProgrRange);

    void AttachChangeProgrRangeTilePartN(
        const FixedArray<JP2ProgrRange> &changedProgrRange);

    bool  IsInRange() const { return m_isInRange; }
    void  operator++(int);

    Point         Precinct () const { return Point(m_precinctX, m_precinctY); }
    unsigned int  Component() const { return m_component; }
    unsigned int  ResLevel () const { return m_resLevel;  }
    unsigned int  Layer    () const { return m_layer;     }

protected:
    void RestoreChangeProgrRangeTilePart0();
    void RestoreProgrRangeDefault();


    void Start() { ClearPrecinctLayer(); m_isFirstVolume = true; (*this)++; }

    void SetProgrRange(
        const JP2ProgrRange          &progrRange);

    bool Step    ();
    bool StepLRCP();
    bool StepRLCP();
    bool StepRPCL();
    bool StepPCRL();
    bool StepCPRL();

    void MinimizeStepPerComponent(unsigned int component);
    void OptimizeStepPerComponent(unsigned int component);
    void OptimizeStepAll         ();

    bool DoPrecinctChange        (const RectSize &precStepOrder, unsigned int nOfResLevels);
    bool DoPrecinctChangeCPRL    (const RectSize &precStepOrder, unsigned int nOfResLevels);

    unsigned int NOfLayers       () const { return m_nOfLayers;      }
    unsigned int NOfComponents   () const { return m_nOfComponents;  }
    unsigned int ResLevelFirst   () const { return m_resLevelFirst;  }
    unsigned int ResLevelBound   () const { return m_resLevelBound;  }
    unsigned int ComponentFirst  () const { return m_componentFirst; }
    unsigned int ComponentBound  () const { return m_componentBound; }

    unsigned int PrecinctNOfCols () const { return m_precinctLayer[m_component][m_resLevel].NOfCols(); }
    unsigned int PrecinctNOfRows () const { return m_precinctLayer[m_component][m_resLevel].NOfRows(); }
    bool         IsEmptyResLevel () const { return PrecinctNOfCols()==0 || PrecinctNOfRows()==0; }

    unsigned int &PrecinctLayer  ()       { return m_precinctLayer[m_component][m_resLevel][m_precinctY][m_precinctX]; }

    void         ClearPrecinctLayer();

    const JP2CodingStyleComp &CodingStyle(unsigned int component) const { return (*m_codingStyle)[component]; }


    const JP2CodingStyleCompTile *m_codingStyle;
    const RectSize               *m_sampleSizeRG;

    JP2ProgrType m_progrType;

    unsigned int m_layer;

    unsigned int m_nOfLayers;
    unsigned int m_nOfComponents;

    unsigned int m_resLevel;
    unsigned int m_resLevelFirst;
    unsigned int m_resLevelBound;

    unsigned int m_maxNOfResLevels;

    unsigned int m_component;
    unsigned int m_componentFirst;
    unsigned int m_componentBound;

    bool         m_isFirstVolume;

    unsigned int m_precinctX;
    unsigned int m_precinctY;

    unsigned int m_x;
    unsigned int m_originX;
    unsigned int m_boundX;
    unsigned int m_stepX;

    unsigned int m_y;
    unsigned int m_originY;
    unsigned int m_boundY;
    unsigned int m_stepY;

    FixedBuffer<FixedBuffer<FixedArray2D<unsigned int> > >
                 m_precinctLayer; // [component][resLevel][precX][precY]

    bool m_isInRange;

    JP2ProgrRange  m_progrRangeDefault;
    JP2ProgrRange *m_changedProgrRangeTilePart0;
    unsigned int   m_nOfProgrRangesTilePart0;
    unsigned int   m_currProgrTilePart0;
    JP2ProgrRange *m_changedProgrRange;
    unsigned int   m_nOfProgrRanges;
    unsigned int   m_currProgr;
};


#endif // __JP2TILEPROGRITERATOR_H__

