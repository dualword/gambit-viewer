// ***************************************************************************
// GAbstractFormatManager.h (c) 2009 Derek Barnett
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
// Describes main file format manager interface.  Specific file format
// managers must implement this interface.
//
// * Note - This is the entry point for file format plugins.  Each plugin must
// inherit from this interface, implement the CreateNewReader() method, which
// returns an file reader that implements the GAbstractFileReader interface.
// GAbstractFormatManager provides reasonable defaults for the remaining
// methods that should rarely need overriding.
//
// Please see the file format plugins supplied 'out-of-the-box' for examples
// of properly implementing this interface.
// ***************************************************************************

#ifndef G_ABSTRACTFORMATMANAGER_H
#define G_ABSTRACTFORMATMANAGER_H

#include <QMap>
#include <QtPlugin>
#include "DataStructures/GFileFormatData.h"
#include "DataStructures/GFileInfo.h"
#include "DataStructures/GReference.h"

namespace Gambit {

class GGenomicDataSet;

namespace FileIO {

class GAbstractFileReader;
typedef QMap<GFileInfo, GAbstractFileReader*> GReaderMap;

class GAbstractFormatManager {

    public:
        GAbstractFormatManager(void) { }
        virtual ~GAbstractFormatManager(void) { }

    public:
        void CloseFiles(const GFileInfoList& files);
        void OpenFiles(const GFileInfoList& files);
        void LoadData(GGenomicDataSet& data);
        bool LoadReferences(GReferenceList& references);
        const GFileFormatData& FormatData(void);
        const GFileInfoList CurrentFiles(void);

    protected:
        virtual GAbstractFileReader* CreateNewReader(void) =0;

    protected:
        GReaderMap      m_readerMap;
        GFileFormatData m_formatData;
};

} // namespace FileIO
} // namespace Gambit

Q_DECLARE_INTERFACE(Gambit::FileIO::GAbstractFormatManager, "Gambit.GFileIO.GAbstractFormatManager/1.0")

#endif // G_ABSTRACTFORMATMANAGER_H
