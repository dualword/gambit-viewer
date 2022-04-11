// ***************************************************************************
// GGffFormatManager.h (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 17 Dec 2009 (DB)
// ---------------------------------------------------------------------------
// This file is part of Gambit plugin: (lib)gambit_fileformat_gff.
// Plugin license rights are same as main application.
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
// Implements GAbstractFormatManager to handle GFF(2) format.
// ***************************************************************************

#ifndef G_GFFFORMATMANAGER_H
#define G_GFFFORMATMANAGER_H

#include <QObject>
#include "SessionManager/FileManager/GAbstractFormatManager.h"
class QString;

namespace Gambit {
namespace FileIO {

class GGffFormatManager : public QObject, public GAbstractFormatManager {

    Q_OBJECT
    Q_INTERFACES(Gambit::FileIO::GAbstractFormatManager)

    public:
        GGffFormatManager(QObject* parent = 0);
        ~GGffFormatManager(void);

    public:
        GAbstractFileReader* CreateNewReader(void);
};

} // namespace FileIO
} // namespace Gambit

#endif // G_GFFFORMATMANAGER_H
