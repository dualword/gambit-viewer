// ***************************************************************************
// GVisibleReferenceItem.cpp (c) 2009 Derek Barnett
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

#include <QtGui>
#include <QtDebug>
#include "Viewer/GVisibleReferenceItem.h"
#include "DataStructures/GReference.h"
using namespace Gambit;
using namespace Gambit::Viewer;

QRectF GVisibleReferenceItem::boundingRect(void) const {
    return QRectF(0,0, 10+m_rectLength, 25+m_rectHeight);
}

void GVisibleReferenceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

    Q_UNUSED(option);
    Q_UNUSED(widget);

    // define reference rectangle
    QRectF referenceRect = QRectF(5,25,m_rectLength,m_rectHeight);

    // draw reference name label
    QString label = QString("%1 : %2 bp").arg(m_ref.Name).arg(m_ref.Length);
    painter->setPen( Qt::white );
    painter->drawText(5,20,label);

    // set up reference rectangle background brush
    QColor backgroundColor = QColor("steelblue");
    QLinearGradient gradient(referenceRect.topLeft(), referenceRect.bottomLeft());
    gradient.setSpread(QGradient::ReflectSpread);
    gradient.setColorAt(0.0, backgroundColor.lighter() );
    gradient.setColorAt(0.4, backgroundColor);
    gradient.setColorAt(0.6, backgroundColor);
    gradient.setColorAt(1.0, Qt::black);
    painter->setBrush( QBrush(gradient) );

    // set up reference rectangle border pen
    if ( backgroundColor == Qt::white ) { painter->setPen(backgroundColor); }
    else { painter->setPen(Qt::black); }

    // draw reference rectangle
    painter->drawRect( referenceRect );
}

void GVisibleReferenceItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    // catch and ignore any mouse event that is not a 'left-click'
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    ungrabMouse();

    // emit signal
    emit clicked(m_ref);
}
