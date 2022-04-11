// ***************************************************************************
// GVisibleReferenceItem.h (c) 2009 Derek Barnett
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
// Describes a 'visible' SNP that is displayed in the GReferenceView.
// Manages drawing and catches user interaction.
// ***************************************************************************

#ifndef G_VISIBLEREFERENCEITEM_H
#define G_VISIBLEREFERENCEITEM_H

#include <QGraphicsItem>
#include <QObject>
#include "DataStructures/GReference.h"

namespace Gambit {
namespace Viewer {

class GVisibleReferenceItem : public QObject, public QGraphicsItem {

    Q_OBJECT

    // constructors & destructors
    public:
        GVisibleReferenceItem(GReference ref, qreal length)
            : QGraphicsItem()
            , m_ref(ref)
            , m_rectHeight(10)
            , m_rectLength(length)
        { }

    // QGraphicsItem implementation
    public:
        QRectF boundingRect(void) const;
        void   paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

    // handle user interaction
    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent*);

    // signal user interaction
    signals:
        void clicked(const GReference& ref);

    // data access
    public:
        GReference& reference(void) { return m_ref; }

    // data members
    private:
        GReference m_ref;
        qreal m_rectHeight;
        qreal m_rectLength;
};

} // namespace Viewer
} // namespace Gambit

#endif // G_VISIBLEREFERENCEITEM_H
