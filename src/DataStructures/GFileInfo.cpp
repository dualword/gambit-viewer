// ***************************************************************************
// GFileInfo.cpp (c) 2009 Derek Barnett
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
// Describes main information about a certain file (and related index)
// instance.
// ***************************************************************************

// GFileInfo.cpp
#include <QtCore>
#include <QtDebug>
#include "DataStructures/GFileInfo.h"
using namespace Gambit;

quint16 GFileInfo::NEXT_ID = 0;
quint16 GFileInfo::NextID(void) { return NEXT_ID++; }

GFileInfo::GFileInfo(const FileType& type,
                     const QString& format,
                     const QString&  filename,
                     const QString&  indexFilename)
    : Type(type)
    , Format(format)
    , Filename(filename)
    , IndexFilename(indexFilename)
{
    ID = GFileInfo::NextID();
}

namespace Gambit {

QDataStream& operator<< (QDataStream& out, GFileInfo f) {
    qint32 type = (qint32)f.Type;
    out << type;

    out << f.Format;
    out << f.Filename;
    out << f.IndexFilename;
    return out;
}

QDataStream& operator>> (QDataStream& in,  GFileInfo& f) {
    qint32 type;
    in >> type;
    f.Type = (GFileInfo::FileType)type;

    in >> f.Format;
    in >> f.Filename;
    in >> f.IndexFilename;
    return in;
}

} // namespace Gambit
