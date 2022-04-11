// ***************************************************************************
// GViewer.h (c) 2009 Derek Barnett
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
// Provides main viewer area - currently is composite of GAssemblyView,
// GAssemblyToolbar, GReferenceView, and GSliderView.
//
// Main interface between view components and GambitAPI.
// ***************************************************************************

#ifndef G_VIEWER_H
#define G_VIEWER_H

#include <QFrame>
#include "DataStructures/GAlignment.h"
#include "DataStructures/GGenomicDataSet.h"
#include "DataStructures/GReference.h"
#include "Viewer/AssemblyView/GVisibleAlignmentItem.h"
class QStringList;

namespace Gambit {
namespace Core {

class GViewer : public QFrame {

    Q_OBJECT

    // constructor
    public:
        GViewer(QWidget* parent = 0);
        ~GViewer(void);

    // GViewer interface
    public slots:
        void Clear(void);
        void SetCenterOn(qint32 position);
        void ShowAssembly(GGenomicDataSet data);
        void ShowReferences(GReferenceList references);
        void Update(void);
        void Print(void);
        void ReferenceDefaultDataRequested(const GReference& reference);
        void ZoomIn(void);
        void ZoomOut(void);
        void ZoomReset(void);

    // internal slot interface
    private slots:
        void CatchSliderClick(const GGenomicDataRegion& region);
        void CatchToolbarJumpRequest(const GGenomicDataRegion& region);

    // signals
    signals:
        void ViewerDataRequested(const GGenomicDataRegion& region);

    // data members
    private:
        struct GViewerPrivate;
        GViewerPrivate* d;
};

} // namespace Core
} // namespace Gambit

#endif // G_VIEWER_H
