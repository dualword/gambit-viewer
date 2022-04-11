// ***************************************************************************
// GVisibleSnpItem.h (c) 2009 Derek Barnett
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

#ifndef G_VISIBLESNPITEM_H
#define G_VISIBLESNPITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QList>
#include "DataStructures/GSnp.h"
class QColor;
class QFont;
class QGraphicsSceneMouseEvent;

namespace Gambit {

class GSnp;

namespace Viewer {

class GVisibleSnpItem : public QObject, public QGraphicsItem {

    Q_OBJECT

    // constructors & destructors
    public:
        GVisibleSnpItem(const GSnp& snp,
                        const int&  fontHeight = 10,
                        const int&  fontWidth  = 10);
        ~GVisibleSnpItem(void);

    // QGraphicsItem implementation
    public:
        QRectF boundingRect(void) const;
        void   paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

    // handle user interaction
    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent*);

    // signal user interaction
    signals:
        void clicked(GSnp);

    // data members
    private:
        GSnp m_snp;
        int  m_fontHeight;
        int  m_fontWidth;

    private:
        static const QString TOOLTIP_TEMPLATE;
};

} // namespace Viewer
} // namespace Gambit

#endif // G_VISIBLESNPITEM_H
