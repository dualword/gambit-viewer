// ***************************************************************************
// GFileFormatData.h (c) 2009 Derek Barnett
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
// Describes main file format data structure.
// ***************************************************************************

#ifndef G_FILEFORMATDATA_H
#define G_FILEFORMATDATA_H

#include <QStringList>
#include "DataStructures/GFileInfo.h"

namespace Gambit {

struct GFileFormatData {

    // data members
    QString     Name;
    QStringList Extensions;
    QStringList IndexExtensions;
    QList<GFileInfo::FileType> Types;
    bool UsesIndex;

    // constructor
    GFileFormatData(const QString name = "",
                    bool useIndex = false)
        : Name(name)
        , UsesIndex(useIndex)
    { }
};

typedef QList<GFileFormatData> GFileFormatList;

} // namespace Gambit

#endif // G_FILEFORMATDATA_H
