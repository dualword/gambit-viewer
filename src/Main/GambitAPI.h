// ***************************************************************************
// GambitAPI.h (c) 2009 Derek Barnett
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
// Describes the top-level API class.  This class links all the sub-modules in
// the Gambit architecture, 'under the hood' in the implementation file.
// ***************************************************************************

#ifndef GAMBITAPI_H
#define GAMBITAPI_H

#include <QObject>
class QString;
class QWidget;

namespace Gambit {

class GGenomicDataRegion;

namespace Core {
    class GSessionManager;
    class GViewer;
}

class GambitAPI : public QObject {

    Q_OBJECT

    public:
        GambitAPI(QObject* parent = 0);
        ~GambitAPI(void);

    signals:
        void SessionActivated(void);
        void SessionDeactivated(void);

    public slots:

        // SessionManager API
        void CreateSession(void);
        void LoadSession(void);
        void SaveSession(void);

        // FileManager API
        void CloseFiles(void);
        void OpenFiles(void);

        // DataManager API
        void LoadDataForViewer(const GGenomicDataRegion& region);

        // Viewer API
        void Print(void); // keep for now? works somewhat, but not extremely useful...
        void ZoomIn(void);
        void ZoomOut(void);
        void ZoomReset(void);

        // Gambit app & developer info
        const QString GambitVersion(void) const;
        const QString GambitProjectUrl(void) const;
        const QString MarthLabUrl(void) const;

    public:
        QWidget* Viewer(void);

    private:
        void CreateModules(void);
        void ConnectModules(void);

    private:
        Core::GSessionManager* m_sessionManager;
        Core::GViewer*         m_viewer;

    private:
        static const QString GAMBIT_CURRENT_VERSION;
        static const QString GAMBIT_MARTHLAB_URL;
        static const QString GAMBIT_PROJECT_URL;

}; // class GambitAPI
}  // namespace Gambit

#endif // GAMBITAPI_H
