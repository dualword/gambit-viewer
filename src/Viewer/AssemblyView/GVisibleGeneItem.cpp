// ***************************************************************************
// GVisibleGeneItem.cpp (c) 2009 Derek Barnett
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
// Describes a 'visible' gene that is displayed in the GAssemblyView.
// Manages drawing and catches user interaction.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/AssemblyView/GVisibleGeneItem.h"
using namespace Gambit;
using namespace Gambit::Viewer;

GVisibleGeneItem::GVisibleGeneItem(const GGene& gene,
                                   const int& fontHeight,
                                   const int& fontWidth)
    : QGraphicsItem()
    , m_gene(gene)
    , m_fontHeight(fontHeight)
    , m_fontWidth(fontWidth)
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setToolTip(m_gene.Name);
}

GVisibleGeneItem::~GVisibleGeneItem(void) { }

QRectF GVisibleGeneItem::boundingRect(void) const {
    qreal rectWidth  = ((m_gene.Stop - m_gene.Start) + m_gene.StopOffset + 1) * m_fontWidth + 2;
    qreal rectHeight = m_fontHeight + 2;
    return QRectF(0,0,rectWidth,rectHeight);
}

void GVisibleGeneItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw gene rectangle
    QLinearGradient gradient(QPointF(0,0), QPointF(0, m_fontHeight+2));
    gradient.setColorAt(0.5, QColor("firebrick"));
    gradient.setColorAt(1, QColor(Qt::black));

    painter->setBrush( QBrush(gradient) );
    painter->setPen(Qt::black);
    painter->drawRect( QRectF(0,0,( ((m_gene.Stop - m_gene.Start) + m_gene.StopOffset + 1)*m_fontWidth ),m_fontHeight) );
}

void GVisibleGeneItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    // catch and ignore any mouse event that is not a 'left-click'
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    ungrabMouse();

    // emit signal
    emit clicked(m_gene);

    // send event to QGraphicsItem (base class) to do what it needs
    QGraphicsItem::mousePressEvent(event);
    update();
}
