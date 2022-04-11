// ***************************************************************************
// GVisibleAlignmentGroup.h (c) 2009 Derek Barnett
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
// Describes a group of alignments (usally from same read group).  Handles
// indiviual alignment layout within the group as well as responding to user-
// directed events and settings modifications.
// ***************************************************************************

#ifndef G_VISIBLEALIGNMENTGROUP_H
#define G_VISIBLEALIGNMENTGROUP_H

#include <QFont>
#include <QGraphicsItemGroup>
#include <QStringList>
#include "DataStructures/GAlignment.h"
class QFont;
class QGraphicsItem;

namespace Gambit {

class GColorScheme;

namespace Viewer {

class GVisibleAlignmentItem;
class GVisibleAlignmentGroupHeader;

class GVisibleAlignmentGroup : public QObject, public QGraphicsItemGroup {

    Q_OBJECT

    public:
        GVisibleAlignmentGroup(const QString&      readGroup,
                               const GColorScheme& colorScheme,
                               const QFont&        font,
                               const int&          fontHeight,
                               const int&          fontWidth,
                               const qint32&       leftBound,
                               QGraphicsItem*      parent = 0);
        ~GVisibleAlignmentGroup(void);

        QRectF boundingRect(void) const;

    public:
        QString Label(void);
        void SetReadGroup(QString& readGroup);
        void SetColorScheme(const GColorScheme& colorScheme);
        void SetBasesVisible(bool ok);

    public slots:
        void MoveHeaderToPosition(int value, qreal horizontalScaleFactor);

    signals:
        void GroupExpanded(void);
        void GroupCollapsed(void);

    public:
        void AddAlignment(GAlignment& alignment);
        void AddAlignments(GAlignmentList alignments);
        void AddAlignmentItem(GVisibleAlignmentItem* item);
        GAlleleList AllelesOverlappingPosition(qint32 position);
        GAlignmentList Alignments(void);
        const QList<GVisibleAlignmentItem*>& AlignmentItems(void);
        void Clear(void);
        void SetAlignmentItems(const QList<GVisibleAlignmentItem*>& items);
        void SetLeftBound(qint32 left);

    public slots:
        void Collapse(void);
        void Draw(void);
        void Expand(void);

    private:
        void DrawAlignments(void);
        void DrawHeader(void);

    private:
        struct GVisibleAlignmentGroupPrivate;
        GVisibleAlignmentGroupPrivate* d;
};

} // namespace Viewer
} // namespace Gambit

#endif // G_VISIBLEALIGNMENTGROUP_H
