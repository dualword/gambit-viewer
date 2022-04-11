// ***************************************************************************
// GAssemblyToolbar.h (c) 2009 Derek Barnett
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
// Provides the toolbar that sits above the GAssemblyView.  Provides access
// to common view actions.
// ***************************************************************************

#ifndef G_ASSEMBLYTOOLBAR_H
#define G_ASSEMBLYTOOLBAR_H

#include <QList>
#include <QToolBar>
#include "DataStructures/GAlignment.h"
#include "DataStructures/GGenomicDataRegion.h"
#include "Viewer/AssemblyView/GVisibleAlignmentItem.h"

namespace Gambit {
namespace Viewer {

class GAssemblyToolbar : public QToolBar {

    Q_OBJECT

    public:
        GAssemblyToolbar(const QList<QAction*>& actions, QWidget* parent = 0);
        ~GAssemblyToolbar(void);

     public:
        void SetPosition(qint32 position);
        void SetRange(qint32 value);
        void SetReferenceNames(QStringList names);
        void SetSelectedReference(const QString& name);

    signals:
        void JumpRequested(const GGenomicDataRegion& region);
        void ViewCenterOnRequested(qint32 position);
        void AlignmentOptionChangeRequested(const GVisibleAlignmentItem::AlignmentItemViewMode mode,
                                            const GAlignment::GAlignmentOption option,
                                            bool ok);

    private slots:
        void JumpTriggered(void);

    private:
        struct GAssemblyToolbarPrivate;
        GAssemblyToolbarPrivate* d;

    public:
        static const qint32 DEFAULT_POSITION;
        static const qint32 DEFAULT_RANGE;
};

} // namespace Viewer
} // namespace Gambit

#endif // GASSEMBLYTOOLBAR_H
