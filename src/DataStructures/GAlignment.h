// ***************************************************************************
// GAlignment.h (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
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
// Describes main alignment data structure and some associated methods.
// ***************************************************************************

#ifndef G_ALIGNMENT_H
#define G_ALIGNMENT_H

#include <QFlags>
#include <QStringList>
#include <QMap>
#include "DataStructures/GAllele.h"

namespace Gambit {

struct GAlignment {

    // set up alignment flag options
    enum GAlignmentOption {
        None              = 0x0000,
        Paired            = 0x0001,
        ProperPair        = 0x0002,
        Unmapped          = 0x0004,
        MateUnmapped      = 0x0008,
        ReverseStrand     = 0x0010,
        MateReverseStrand = 0x0020,
        Read1             = 0x0040,
        Read2             = 0x0080,
        Secondary         = 0x0100,
        QcFailed          = 0x0200,
        Duplicate         = 0x0400
    };
    Q_DECLARE_FLAGS(GAlignmentOptions, GAlignmentOption);

    // alignment data
    QString Name;
    QString Bases;
    QString PaddedBases;
    qint32  Length;
    QString Qualities;
    qint32  RefId;
    qint32  Position;
    quint32 MapQuality;
    bool    IsReverseComplement;
    QString ReadGroup;
    GAlignmentOptions Flags;

    // assembly data
    QList<qint32>        Mismatches;
    QMap<qint32, qint32> Insertions;
    qint32               PadsBefore;

    // constructors
    GAlignment(void)
        : ReadGroup("")
        , Flags(GAlignment::None)
        , PadsBefore(0)
    { }

    // sort/search function object
    struct SortByPosition;
};

// container typedefs
typedef QList<GAlignment> GAlignmentList;

// sort/search function object
struct GAlignment::SortByPosition {
    bool operator() (const GAlignment& g1, const GAlignment& g2) {
        return g1.Position < g2.Position;
    }
};

// doesn't include 'None' option
inline
const QList<GAlignment::GAlignmentOption> AlignmentOptions(void) {
    QList<GAlignment::GAlignmentOption> options;
    options << GAlignment::Paired
            << GAlignment::ProperPair
            << GAlignment::Unmapped
            << GAlignment::MateUnmapped
            << GAlignment::ReverseStrand
            << GAlignment::MateReverseStrand
            << GAlignment::Read1
            << GAlignment::Read2
            << GAlignment::Secondary
            << GAlignment::QcFailed
            << GAlignment::Duplicate;
    return options;
}

// doesn't include 'None' option
inline
const QStringList AlignmentOptionNames(void) {
    QStringList names;
    names << "Paired"
          << "ProperPair"
          << "Unmapped"
          << "MateUnmapped"
          << "ReverseStrand"
          << "MateReverseStrand"
          << "Read1"
          << "Read2"
          << "SecondaryAlignment"
          << "QcFailed"
          << "PcrDuplicate";
    return names;
}

// --------------------------------------------------------------
// GAlignment 'convenience functions'
// --------------------------------------------------------------

// returns whether alignment(padded) overlaps given genomic position
inline
bool OverlapsPosition(const GAlignment& alignment, qint32 genomicPosition) {
    return ( (genomicPosition >= (alignment.Position + alignment.PadsBefore)) &&
             (genomicPosition <  (alignment.Position + alignment.PadsBefore + alignment.PaddedBases.length()))
           );
}

// returns allele data from alignment at given genomic position
inline
bool AlleleAt(GAllele& allele, const GAlignment& alignment, qint32 genomicPosition) {

    // if alignment overlaps position, return allele
    if ( OverlapsPosition(alignment, genomicPosition) ) {
        int index  = genomicPosition - alignment.Position;
        if ( (index >= 0) && (index < alignment.Bases.size()) ) {
            allele.Base    = alignment.Bases.at(index);
            allele.Quality = quint32( alignment.Qualities.at(index).toAscii() ) - 33; // FASTQ conversion
            allele.IsReverseComplement = alignment.IsReverseComplement;
            return true;
        }
    }

    return false;
}

} // end namespace Gambit

// declares global operator|() functions for AlignmentFlags
Q_DECLARE_OPERATORS_FOR_FLAGS(Gambit::GAlignment::GAlignmentOptions)

#endif // G_ALIGNMENT_H
