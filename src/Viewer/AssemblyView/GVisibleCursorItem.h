// ***************************************************************************
// GVisibleCursorItem.h (c) 2009 Derek Barnett
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
// Describes the mouse-tracking 'cursor' on the GAssemblyView screen.
// ***************************************************************************

#ifndef G_VISIBLECURSORITEM_H
#define G_VISIBLECURSORITEM_H

#include <QGraphicsItem>
#include <QObject>

namespace Gambit {
namespace Viewer {

class GVisibleCursorItem : public QObject, public QGraphicsItem {

    Q_OBJECT

    public:
        GVisibleCursorItem(qreal width = 10);

    // QGraphicsItem implementation
    public:
        QRectF boundingRect(void) const;
        void   paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

    public:
        void SetHeight(qreal height);
        void SetWidth(qreal width);

    private:
        qreal m_height;
        qreal m_width;
};

} // namespace Viewer
} // namespace Gambit


#endif // G_VISIBLECURSORITEM_H
