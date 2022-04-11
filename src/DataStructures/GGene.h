// ***************************************************************************
// GGene.h (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 17 Dec 2009 (DB)
// ---------------------------------------------------------------------------
// This file is part of Gambit.
//
// Gambit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Gambit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Gambit.  If not, see <http://www.gnu.org/licenses/>.
// ---------------------------------------------------------------------------
// Describes main gene & exon data structures and some associated methods.
// ***************************************************************************

#ifndef G_GENE_H
#define G_GENE_H

#include <QList>
#include <QString>

namespace Gambit {

// basic exon data structure
struct GExon {

    // data members
    qint32 Start;
    qint32 Stop;
    qint32 StartOffset;
    qint32 StopOffset;

    // constructors
    GExon(void)
        : Start(0)
        , Stop(0)
        , StartOffset(0)
        , StopOffset(0)
    { }
};

typedef QList<GExon> GExonList;

// basic gene data structure
struct GGene {

    // data members
    QString   Name;
    QString   Strand;
    qint32    Start;
    qint32    Stop;
    QString   AuxInfo;
    GExonList Exons;
    qint32    StartOffset;
    qint32    StopOffset;

    // constructors
    GGene(void)
        : Start(0)
        , Stop(0)
        , StartOffset(0)
        , StopOffset(0)
    { }
};

typedef QList<GGene> GGeneList;

}   // namespace Gambit

#endif // G_GENE_H
