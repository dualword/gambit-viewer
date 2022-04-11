// ***************************************************************************
// GVisibleAlignmentItem.h (c) 2009 Derek Barnett
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

#ifndef G_VISIBLEALIGNMENTITEM_H
#define G_VISIBLEALIGNMENTITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QFont>
#include "DataStructures/GAlignment.h"
#include "DataStructures/GColorScheme.h"
class QColor;
class QGraphicsSceneMouseEvent;

namespace Gambit {

class GAlignment;

namespace Viewer {

class GVisibleAlignmentItem : public QObject, public QGraphicsItem {

    Q_OBJECT

    public: enum AlignmentItemViewMode { Normal = 0, Dimmed, Highlighted };

    // constructors & destructors
    public:
        GVisibleAlignmentItem(const GAlignment& alignment,
                              bool              basesVisible = false,
                              const QFont&      font         = QFont(),
                              const int&        fontHeight   = 10,
                              const int&        fontWidth    = 10);
        ~GVisibleAlignmentItem(void);

    // QGraphicsItem implementation
    public:
        QRectF boundingRect(void) const;
        void   paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

    // data access
    public:
        void SetBasesVisible(bool ok);
        void SetColorScheme(GColorScheme& colorScheme);
        const GAlignment& alignment(void) { return m_alignment; }

    // handle user interaction
    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent*);

    // signal user interaction
    signals:
        void clicked(GAlignment);

    // data members
    private:
        GAlignment   m_alignment;
        GColorScheme m_colorScheme;
        bool         m_basesVisible;
        QFont        m_font;
        int          m_fontHeight;
        int          m_fontWidth;

   // static constants for all GVisibleAlignmentItems
   private:
        static const QString DELETION_BASE;
        static const QString N_BASE;
        static const QString PADDING_BASE;
        static const QString TOOLTIP_TEMPLATE;
};

inline
const QStringList ViewModeNames(void) {
    QStringList names;
    names << "Normal" << "Dimmed" << "Highlighted";
    return names;
}

} // namespace Viewer
} // namespace Gambit

#endif // G_VISIBLEALIGNMENTITEM_H

