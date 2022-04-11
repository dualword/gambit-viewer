// ***************************************************************************
// GSnp.h (c) 2009 Derek Barnett
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
// Describes main snp data structure and some associated methods.
// ***************************************************************************

#ifndef G_SNP_H
#define G_SNP_H

#include <QList>
#include <QString>

namespace Gambit {

struct GSnp {

    // data members
    QString Name;
    qint32  Position;
    double  Score;
    qint32  PaddingOffset;

    // constructors
    GSnp(void)
        : Name("")
        , Position(0)
        , Score(0.0)
        , PaddingOffset(0)
    { }
};

// typedefs
typedef QList<GSnp> GSnpList;

} // namespace Gambit

#endif // G_SNP_H
