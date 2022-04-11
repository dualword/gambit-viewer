// ***************************************************************************
// GAllele.h (c) 2009 Derek Barnett
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
// Describes main allele data structure.
// ***************************************************************************

#ifndef G_ALLELE_H
#define G_ALLELE_H

#include <QChar>
#include <QList>

namespace Gambit {

struct GAllele {

    // data members
    QChar   Base;
    quint32 Quality;
    bool    IsReverseComplement;

    // constructor
    GAllele(QChar   base    = QChar('?'),
            quint32 quality = 0,
            bool    isRC    = false)
        : Base(base)
        , Quality(quality)
        , IsReverseComplement(isRC)
    { }
};

typedef QList<GAllele> GAlleleList;

} // namespace Gambit

#endif // G_ALLELE_H
