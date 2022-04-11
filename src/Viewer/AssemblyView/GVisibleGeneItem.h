// ***************************************************************************
// GVisibleGeneItem.h (c) 2009 Derek Barnett
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

#ifndef G_VISIBLEGENEITEM_H
#define G_VISIBLEGENEITEM_H

#include <QObject>
#include <QGraphicsItem>
#include "DataStructures/GGene.h"
class QColor;
class QFont;
class QGraphicsSceneMouseEvent;

namespace Gambit {
namespace Viewer {

class GVisibleGeneItem : public QObject, public QGraphicsItem {

    Q_OBJECT

    // constructors & destructors
    public:
        GVisibleGeneItem(const GGene& gene,
                         const int& fontHeight = 10,
                         const int& fontWidth  = 10);
        ~GVisibleGeneItem(void);

    // QGraphicsItem implementation
    public:
        QRectF boundingRect(void) const;
        void   paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

    // handle user interaction
    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent*);

    // signal user interaction
    signals:
        void clicked(GGene);

    // data members
    private:
        GGene m_gene;
        int   m_fontHeight;
        int   m_fontWidth;
};

} // namespace Viewer
} // namespace Gambit

#endif // G_VISIBLEGENEITEM_H
