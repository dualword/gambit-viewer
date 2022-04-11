// ***************************************************************************
// GVisibleAlignmentItem.cpp (c) 2009 Derek Barnett
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
// Describes a 'visible' alignment that is displayed in the GAssemblyView.
// Manages drawing and catches user interaction.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/AssemblyView/GVisibleAlignmentItem.h"
using namespace Gambit;
using namespace Gambit::Viewer;

const QString GVisibleAlignmentItem::DELETION_BASE  = QString("-");
const QString GVisibleAlignmentItem::N_BASE         = QString("N");
const QString GVisibleAlignmentItem::PADDING_BASE   = QString("*");
const QString GVisibleAlignmentItem::TOOLTIP_TEMPLATE = "<h4>%1</h4>"
                                                        "Position: %2<br>"
                                                        "Strand: %3<br>"
                                                        "Length: %4<br>"
                                                        "Read Group: %5<br>"
                                                        "MapQuality: %6";

GVisibleAlignmentItem::GVisibleAlignmentItem(const GAlignment& alignment,
                                             bool basesVisible,
                                             const QFont& font,
                                             const int& fontHeight,
                                             const int& fontWidth)
    : QGraphicsItem()
    , m_alignment(alignment)
    , m_basesVisible(basesVisible)
    , m_font(font)
    , m_fontHeight(fontHeight)
    , m_fontWidth(fontWidth)
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    QString tooltipText = TOOLTIP_TEMPLATE.arg(m_alignment.Name)
                                          .arg(QString::number(m_alignment.Position))
                                          .arg((m_alignment.IsReverseComplement ? "-" : "+"))
                                          .arg(QString::number(m_alignment.Length))
                                          .arg(m_alignment.ReadGroup)
                                          .arg(m_alignment.MapQuality);
    setToolTip(tooltipText);
    setZValue(-10);
}

GVisibleAlignmentItem::~GVisibleAlignmentItem(void) { }

QRectF GVisibleAlignmentItem::boundingRect(void) const {
    qreal rectWidth  = (m_alignment.PaddedBases.length() * m_fontWidth) + 2;
    qreal rectHeight = m_fontHeight + 2;
    return QRectF(0,0,rectWidth,rectHeight);
}

void GVisibleAlignmentItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

    Q_UNUSED(option);
    Q_UNUSED(widget);

    // get color scheme, strand-dependent
    QColor backgroundColor;
    if ( m_alignment.IsReverseComplement ) {
        backgroundColor = m_colorScheme.ReverseBackground;
    } else {
        backgroundColor = m_colorScheme.ForwardBackground;
    }

    // set up background color (gradient)
    QLinearGradient gradient(0,0,0,m_fontHeight);
    gradient.setSpread(QGradient::ReflectSpread);
    gradient.setColorAt(0.0, backgroundColor.lighter() );
    gradient.setColorAt(0.4, backgroundColor);
    gradient.setColorAt(0.6, backgroundColor);
    gradient.setColorAt(1.0, Qt::black);
    painter->setBrush( QBrush(gradient) );

    // set border pen (same as text color)
    if ( backgroundColor == Qt::black ) {
        painter->setPen(QColor("gainsboro"));
    } else {
        painter->setPen(Qt::transparent);
    }

    // perform the actual background drawing
    painter->drawRect( QRectF(-1, -1, ( m_alignment.PaddedBases.length()*m_fontWidth ), m_fontHeight) );

    // draw alignment text (GAlignment::Bases)
    painter->setFont(m_font);
    QString sequence = m_alignment.PaddedBases;
    QString base;
    int length = sequence.length();
    int xPos = 0;
    int yPos = m_fontHeight - 4;
    for (int index = 0; index < length; ++index) {

        // get current base
        base = sequence.at(index);

        // if base is padding
        if ( (base == GVisibleAlignmentItem::PADDING_BASE) ) {
            painter->setPen(m_colorScheme.PaddingText);
            painter->drawText(xPos, yPos, base);
        }

        // if base is mismatched from reference - (deletion or mismatch, but not N)
        else if ( (base == GVisibleAlignmentItem::DELETION_BASE) ||
                  (m_alignment.Mismatches.contains(index) && (base != GVisibleAlignmentItem::N_BASE))
                ) {
            painter->setPen(m_colorScheme.MismatchText);
            painter->drawText(xPos, yPos, base);
        }

        // 'normal' base - but draw only if flag set
        else {
            if ( m_basesVisible ) {
                painter->setPen(m_colorScheme.NormalText);
                painter->drawText(xPos, yPos, base);
            }
        }

        // move painter over for next character
        xPos += m_fontWidth;
    }
}

void GVisibleAlignmentItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    // catch and ignore any mouse event that is not a 'left-click'
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    // emit signal
    emit clicked(m_alignment);

    // send event to QGraphicsItem (base class) to do what it needs
    QGraphicsItem::mousePressEvent(event);
    update();
}

void GVisibleAlignmentItem::SetBasesVisible(bool ok) {
    m_basesVisible = ok;
    update( boundingRect() );
}

void GVisibleAlignmentItem::SetColorScheme(GColorScheme& colorScheme) {
    m_colorScheme = colorScheme;
    update( boundingRect() );
}
