// ***************************************************************************
// GReference.h (c) 2009 Derek Barnett
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
// Describes main reference data structure and some associated methods.
// ***************************************************************************

#ifndef G_REFERENCE_H
#define G_REFERENCE_H

#include <QStringList>

namespace Gambit {

struct GReference {

    // data members
    quint32 ID;
    QString Name;
    qint32 Length;

    // constructor
    GReference(const quint32& id     = 0,
               const QString& name   = QString(),
               const qint32&  length = 0)
        : ID(id)
        , Name(name)
        , Length(length)
    { }
};

typedef QList<GReference> GReferenceList;

// --------------------------------------------------------------
// GReference 'utilities'
// --------------------------------------------------------------

inline
bool operator== (const GReference& lhs, const GReference& rhs) {
    return ( lhs.ID == rhs.ID);
}

inline
bool operator< (const GReference& lhs, const GReference& rhs) {
    return (lhs.ID < rhs.ID);
}

// input/output operators
inline
QDataStream& operator<< (QDataStream& out, GReference r) {
    out << r.ID;
    out << r.Name;
    out << r.Length;
    return out;
}

inline
QDataStream& operator>> (QDataStream& in,  GReference& r) {
    in >> r.ID;
    in >> r.Name;
    in >> r.Length;
    return in;
}

// --------------------------------------------------------------
// GReference 'convenience functions'
// --------------------------------------------------------------

// returns names of given GReferences
inline
QStringList ReferenceNames(const GReferenceList& references) {

    QStringList names;

    // iterate over references
    GReference reference;
    foreach (reference, references) {
        names << reference.Name;
    }

    return names;
}

} // namespace Gambit

#endif // G_REFERENCE_H
