// ***************************************************************************
// GDataManager.h (c) 2009 Derek Barnett
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
// Describes the main data processing manager.  Provides basic pre-processing
// of data loaded from files.
// ***************************************************************************

#ifndef G_DATAMANAGER_H
#define G_DATAMANAGER_H

#include <QObject>

namespace Gambit {

class GGenomicDataSet;

namespace Core   {

class GDataManager : public QObject {

    Q_OBJECT

    public:
        GDataManager(QObject* parent = 0);
        ~GDataManager(void);

    public slots:
        void ProcessData(GGenomicDataSet& data);

    private:
        struct GDataManagerPrivate;
        GDataManagerPrivate* d;
};

} // namespace Core
} // namespace Gambit

#endif // G_DATAMANAGER_H
