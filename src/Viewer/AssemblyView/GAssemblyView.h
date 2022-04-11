// ***************************************************************************
// GAssemblyView.h (c) 2009 Derek Barnett
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
// Describes the main assembly view widget.
// Handles drawing of coordinates, annotations, and alignments, as well as
// responding to various user-directed events and settings modifications.
// ***************************************************************************

#ifndef G_ASSEMBLYVIEW_H
#define G_ASSEMBLYVIEW_H

#include <QList>
#include <QGraphicsView>
#include "DataStructures/GAlignment.h"
#include "DataStructures/GAllele.h"
#include "Viewer/AssemblyView/GVisibleAlignmentItem.h"
class QAction;
class QString;

namespace Gambit {

class GGene;
class GGenomicDataSet;
class GSnp;

namespace Viewer {

class GAssemblyView : public QGraphicsView {

    Q_OBJECT

    public:
        GAssemblyView(QWidget* parent = 0);
        ~GAssemblyView(void);

    public:
        QList<QAction*> SettingsActions(void);

    signals:
        void AlignmentClicked(const GAlignment& alignment);
        void GeneClicked(const GGene& gene);
        void SnpClicked(const GSnp& snp);
        void MouseOverAlleles(GAlleleList alleles);
        void HorizontalScrollChanged(int value, qreal horizontalScaleFactor);
        void VerticalScrollChanged(int value, qreal verticalScaleFactor);

    public slots:
        void ClearCurrentData(void);
        void SetCenterOn(qint32 position);
        void MergeReadGroups(bool ok);
        void ShowBases(bool ok);
        void ShowData(const GGenomicDataSet& data);
        void ZoomIn(void);
        void ZoomOut(void);
        void ZoomReset(void);
        void AdjustGroupLayout(void);
        void CatchHorizontalScrollbarChange(int value);
        void CatchVerticalScrollbarChange(int value);
        void ViewSettingsChanged(void);

    private slots:
        void CatchAlignmentClick(const GAlignment& alignment);
        void CatchGeneClick(const GGene& gene);
        void CatchSnpClick(const GSnp& snp);
        void CatchMouseMove(const QPoint& mousePos);

    private:
        void ShowSnpInfoDialog(const GSnp& snp);
        void MoveCursor(const QPoint& mousePos);

    private:
        struct GAssemblyViewPrivate;
        GAssemblyViewPrivate* d;
};

} // namespace Viewer
} // namespace Gambit

#endif // G_ASSEMBLYVIEW_H
