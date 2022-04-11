// ***************************************************************************
// GAssemblySettingsManager.h (c) 2009 Derek Barnett
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
// Manages settings used by the GAssemblyView.
// ***************************************************************************

#ifndef G_ASSEMBLYSETTINGSMANAGER_H
#define G_ASSEMBLYSETTINGSMANAGER_H

#include <QList>
#include <QObject>
#include "DataStructures/GAlignment.h"
#include "Viewer/AssemblyView/GVisibleAlignmentItem.h"
class QAction;
class QWidget;

namespace Gambit {

class GColorScheme;
class GGenomicDataSet;

namespace Viewer {

class GVisibleAlignmentGroup;

class GAssemblySettingsManager : public QObject {

    Q_OBJECT

    // constructor/destructor
    public:
        GAssemblySettingsManager(QObject* parent = 0);
        ~GAssemblySettingsManager(void);

    public:
        QList<QAction*> Actions(void);

    // access settings state
    public:
        bool IsGroupsMerged(void) const;
        bool IsBasesVisible(void) const;

    public:
        void SetAlignmentFlag(const GVisibleAlignmentItem::AlignmentItemViewMode& mode,
                              const GAlignment::GAlignmentOption& option,
                              bool ok);

    // color scheme filtering
    public:
        const GColorScheme& NormalColorScheme(void);
        void SetGroupColorScheme(GVisibleAlignmentGroup* group);

    signals:
        void MergeGroupsChanged(bool ok);
        void ViewSettingsChanged(void);
        void ShowBasesChanged(bool ok);

    private slots:
        void SetMergeGroups(bool ok);
        void EditSettings(void);
        void SetShowBases(bool ok);

    // internals
    private:
        struct GAssemblySettingsManagerPrivate;
        GAssemblySettingsManagerPrivate* d;
};

} // namespace Viewer
} // namespace Gambit

#endif // G_ASSEMBLYSETTINGSMANAGER_H
