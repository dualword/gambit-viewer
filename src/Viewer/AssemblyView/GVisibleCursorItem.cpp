// ***************************************************************************
// GVisibleCursorItem.cpp (c) 2009 Derek Barnett
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

#include <QtGui>
#include <QtDebug>
#include "Viewer/AssemblyView/GVisibleCursorItem.h"
using namespace Gambit;
using namespace Gambit::Viewer;

GVisibleCursorItem::GVisibleCursorItem(qreal width)
    : QGraphicsItem()
    , m_height(40)
    , m_width(width)
{
    setZValue(10);
    setEnabled(true);
    setVisible(true);
    show();
}

QRectF GVisibleCursorItem::boundingRect(void) const {
    return QRectF(0,m_width,0,m_height);
}

void GVisibleCursorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor brushColor = Qt::white;

    QBrush brush(brushColor);
    painter->setBrush(brush);
    painter->setPen(Qt::white);
    painter->drawRect(0,m_width,0,m_height);
}

void GVisibleCursorItem::SetHeight(qreal height) {
    m_height = height;
    update( boundingRect() );
}

void GVisibleCursorItem::SetWidth(qreal width) {
    m_width = width;
    update( boundingRect() );
}
