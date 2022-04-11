// ***************************************************************************
// GSessionManager.h (c) 2009 Derek Barnett
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
// Describes the main Gambit session manager.  This is a manager of
// sub-managers(currently the File- and Data-Managers).  Handles requests
// (input) and signals(output) as well as serialization of Gambit session
// state.
// Sessions are saved to a ".gss" (Gambit Session Settings) file.
// ***************************************************************************

#ifndef G_SESSIONMANAGER_H
#define G_SESSIONMANAGER_H

#include <QObject>
#include "DataStructures/GFileInfo.h"
#include "DataStructures/GGenomicDataSet.h"
#include "DataStructures/GReference.h"

namespace Gambit {
namespace Core {

class GSessionManager : public QObject {

    Q_OBJECT

    // constructors/destructors
    public:
        GSessionManager(QObject* parent = 0);
        ~GSessionManager(void);

    signals:
        void FilesClosed(GFileInfoList files);
        void FilesOpened(GFileInfoList files);
        void ReferencesLoaded(GReferenceList references);
        void SessionActivated(void);
        void SessionDeactivated(void);
        void ViewerDataLoaded(GGenomicDataSet data);

    public slots:

        // General session interface
        void Create(void);
        void Load(void);
        void Save(void);

        // Data access
        void LoadDataForViewer(const GGenomicDataRegion& region);

        // FileManager public interface
        void OpenFiles(const GFileInfoList& files = GFileInfoList());
        void CloseFiles(const GFileInfoList& files = GFileInfoList());

    private:
        struct GSessionManagerPrivate;
        GSessionManagerPrivate* d;
};

} // namespace Core
} // namespace Gambit

#endif // G_SESSIONMANAGER_H
