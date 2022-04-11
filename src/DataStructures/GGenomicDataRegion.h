// ***************************************************************************
// GGenomicDataRegion.h (c) 2009 Derek Barnett
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
// Describes a genomic region used by a GGenomicDataSet.
// ***************************************************************************

#ifndef G_GENOMICDATAREGION_H
#define G_GENOMICDATAREGION_H

#include <QString>

namespace Gambit {

struct GGenomicDataRegion {

    // data members
    QString RefName;
    qint32  LeftBound;
    qint32  RightBound;

    // constructor
    // default left/right bound are 1 because Gambit is all 1-based
    // 0-based formats are converted in their respective format readers
    GGenomicDataRegion(const QString& name  = QString(),
                       const qint32&  left  = 1,
                       const qint32&  right = 1)
        : RefName(name)
        , LeftBound(left)
        , RightBound(right)
    { }
};

} // namespace Gambit

#endif // G_GENOMICDATAREGION_H
