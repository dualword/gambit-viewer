// ***************************************************************************
// GSliderView.h (c) 2009 Derek Barnett
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
// Provides reference 'slider'. Displays full view of current reference.
// Draws a rectangle around currently displayed region.  Catches user
// interaction to provide for jumping to new position in reference.
// ***************************************************************************

#ifndef G_SLIDERVIEW_H
#define G_SLIDERVIEW_H

#include <QGraphicsView>

namespace Gambit {

class GGenomicDataRegion;
class GReference;

namespace Viewer {

class GSliderView : public QGraphicsView {

    Q_OBJECT

    public:
        GSliderView(QWidget* parent = 0);
        ~GSliderView(void);

    signals:
        void RegionRequested(const GGenomicDataRegion& region);

    public:
        void resizeEvent(QResizeEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);

    public slots:
        void SetReference(const GReference& reference);
        void SetSelectedRegion(const GGenomicDataRegion& region);

    private:
        struct GSliderViewPrivate;
        GSliderViewPrivate* d;
};

} // namespace Viewer
} // namespace Gambit

#endif // G_SLIDERVIEW_H
