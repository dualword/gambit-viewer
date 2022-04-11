// ***************************************************************************
// GFileManager.h (c) 2009 Derek Barnett
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
// Describes the main file manager.
// This manager is currently responsible for all major file-related operations:
//   - Opening/closing data files
//   - Retrieving data from a specified region (from all open files)
//   - Serialization (saves list of currently open files)
//   - Detecting/loading file format plugins
// ***************************************************************************

#ifndef G_FILEMANAGER_H
#define G_FILEMANAGER_H

#include <QObject>
#include "DataStructures/GFileInfo.h"
#include "DataStructures/GFileFormatData.h"
#include "DataStructures/GGenomicDataSet.h"
#include "DataStructures/GReference.h"

namespace Gambit {
namespace FileIO {

class GAbstractFormatManager;

class GFileManager : public QObject {

    Q_OBJECT

    public:
        GFileManager(QObject* parent = 0);
        ~GFileManager(void);

    signals:
        void FilesClosed(GFileInfoList files);
        void FilesOpened(GFileInfoList files);
        void SupportedFormatsLoaded(GFileFormatList formats);
        void ReferencesLoaded(GReferenceList references);

    public slots:
        // serialization
        void Load(QDataStream& in, int version);
        void Save(QDataStream& out);

        // data access
        GGenomicDataSet LoadData(const GGenomicDataRegion& region);

        // file access
        void CloseAll(void);
        void CloseFiles(GFileInfoList files = GFileInfoList());
        void OpenFiles(GFileInfoList files  = GFileInfoList());

    public:
        GFileInfoList GetOpenFiles(void);
        bool IsModified(void) { return m_isModified; }

    private:
        // plugin support
        void LoadPlugins(void);
        void SavePlugin(QObject* plugin);

        GFileFormatList GetSupportedFormats(void);

        // dialog
        const GFileInfoList ShowCloseFilesDialog(void);
        const GFileInfoList ShowOpenFilesDialog(void);

    private:
        bool m_isModified;
        QList<GAbstractFormatManager*> m_managers;
};

} // namespace FileIO
} // namespace Gambit

#endif // G_FILEMANAGER_H
