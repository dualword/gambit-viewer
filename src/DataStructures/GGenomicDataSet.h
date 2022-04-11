// ***************************************************************************
// GGenomicDataSet.h (c) 2009 Derek Barnett
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
// Aggregates data from a common region.
// ***************************************************************************

#ifndef G_GENOMICDATASET_H
#define G_GENOMICDATASET_H

#include <QMap>
#include <QString>
#include "DataStructures/GAlignment.h"
#include "DataStructures/GGene.h"
#include "DataStructures/GGenomicDataRegion.h"
#include "DataStructures/GReference.h"
#include "DataStructures/GSnp.h"

namespace Gambit {

// padding data typedef
typedef QMap<qint32, qint32> GPaddingMap;

struct GGenomicDataSet {

    // data set genomic source
    GGenomicDataRegion Region;

    // 'raw' data
    GAlignmentList Alignments;
    GGeneList      Genes;
    QString        Sequence;
    GSnpList       Snps;

    // reference meta-data
    GReferenceList References;

    // derived padding data
    GPaddingMap Padding;

    // constructor
    GGenomicDataSet(const GGenomicDataRegion& region = GGenomicDataRegion())
        : Region(region)
        , Sequence("")
    { }
};

} // namespace Gambit

#endif // G_GENOMICDATASET_H
