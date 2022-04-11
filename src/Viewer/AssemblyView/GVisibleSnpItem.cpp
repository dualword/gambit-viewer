// ***************************************************************************
// GVisibleSnpItem.cpp (c) 2009 Derek Barnett
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
// Describes a 'visible' SNP that is displayed in the GAssemblyView.
// Manages drawing and catches user interaction.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/AssemblyView/GVisibleSnpItem.h"
using namespace Gambit;
using namespace Gambit::Viewer;

const QString GVisibleSnpItem::TOOLTIP_TEMPLATE = "<h4>%1</h4>"
                                                  "Position: %2<br>"
                                                  "Score: %3";

GVisibleSnpItem::GVisibleSnpItem(const GSnp& snp, const int& fontHeight, const int& fontWidth)
    : QGraphicsItem()
    , m_snp(snp)
    , m_fontHeight(fontHeight)
    , m_fontWidth(fontWidth)
{
    QString tooltipText = TOOLTIP_TEMPLATE.arg(m_snp.Name)
                                          .arg(m_snp.Position)
                                          .arg(m_snp.Score);

    // show visible SNP
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setToolTip(tooltipText);
}

GVisibleSnpItem::~GVisibleSnpItem(void) { }

QRectF GVisibleSnpItem::boundingRect(void) const {
    return QRectF( -1, -1, m_fontWidth+1, m_fontHeight+2 );
}

void GVisibleSnpItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw snp box
    QLinearGradient gradient(0, 0, 0, m_fontHeight);
    gradient.setColorAt(0.5, QColor("orange"));
    gradient.setColorAt(1, QColor(Qt::black));

    painter->setBrush( QBrush(gradient) );
    painter->setPen(QColor("orange"));
    painter->drawRect( QRectF(0, 0, m_fontWidth, m_fontHeight) );
}

void GVisibleSnpItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    // ignore any mouse event that is not a 'left-click'
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    ungrabMouse();

    // emit clicked signal
    emit clicked(m_snp);
}
