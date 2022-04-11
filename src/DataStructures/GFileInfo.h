// ***************************************************************************
// GFileInfo.h (c) 2009 Derek Barnett
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

#ifndef G_FILEINFO_H
#define G_FILEINFO_H

#include <QList>
#include <QMap>
#include <QString>
class QDataStream;

namespace Gambit  {

struct GFileInfo {

    // FileType enum
    enum FileType { File_Alignment = 0,
                    File_Gene,
                    File_Reference,
                    File_Snp
                  };

    // constructor(s)
    GFileInfo(const FileType& type          = GFileInfo::File_Alignment,
              const QString&  format        = "",
              const QString&  filename      = "",
              const QString&  indexFilename = "");

    // data members
    quint16  ID;
    FileType Type;
    QString  Format;
    QString  Filename;
    QString  IndexFilename;

     // statics - private to this type
     private:
        static quint16 NEXT_ID;
        static quint16 NextID(void);
};

// --------------------------------------------------------------
// GFileInfo 'utilities'
// --------------------------------------------------------------

inline
bool operator== (const GFileInfo& lhs, const GFileInfo& rhs) {
    return (lhs.ID == rhs.ID);
}

inline
bool operator< (const GFileInfo& lhs, const GFileInfo& rhs) {
    return (lhs.ID < rhs.ID);
}

QDataStream& operator<< (QDataStream& out, GFileInfo f);
QDataStream& operator>> (QDataStream& in,  GFileInfo& f);

// --------------------------------------------------------------
// GFileInfo Typedefs
// --------------------------------------------------------------
typedef QMap<GFileInfo::FileType, QStringList> GDataFileFormatMap;
typedef QList<GFileInfo> GFileInfoList;

}   // end namespace Gambit

#endif // G_FILEINFO_H
