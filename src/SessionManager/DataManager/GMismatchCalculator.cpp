// ***************************************************************************
// GMismatchCaluclator.cpp (c) 2009 Derek Barnett
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
// Describes the pre-processing tool that calculates and stores mismatch data.
// ***************************************************************************

#include <QtCore>
#include <QtDebug>
#include "SessionManager/DataManager/GMismatchCalculator.h"
#include "DataStructures/GGenomicDataSet.h"
using namespace Gambit;

void GMismatchCalculator::Exec(GGenomicDataSet& data) {

    // get reference sequence, skip if empty
    QString reference = data.Sequence;
    if ( reference.isEmpty() ) { return; }

    // get alignments, skip if empty
    GAlignmentList& alignments = data.Alignments;
    if ( alignments.isEmpty() ) { return; }

    // iterate over all alignments
    for ( int index = 0; index < data.Alignments.size(); ++index ) {
        GAlignment& gAlignment = data.Alignments[index];

        // get alignment sequence info
        QString bases = gAlignment.PaddedBases;
        int baseLength = bases.length();
        qint32 alignmentStart = (gAlignment.Position + gAlignment.PadsBefore - data.Region.LeftBound);

        // get reference sequence info
        QString refRegion = reference.mid(alignmentStart, baseLength);
        if (refRegion.length() < baseLength) {
            baseLength = refRegion.length();
        }

        // compare bases
        for ( int index = 0; index < baseLength; ++index ) {
            if ( refRegion.at(index) != bases.at(index) ) {
                gAlignment.Mismatches.append(index);
            }
        }
    }
}
