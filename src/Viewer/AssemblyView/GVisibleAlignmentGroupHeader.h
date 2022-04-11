// ***************************************************************************
// GVisibleAlignmentGroupHeader.h (c) 2009 Derek Barnett
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
// Describes the header widget that accompanies a GVisibleAlignmentGroup.
// Header displays the group name, current color scheme for "normal" items,
// and provides collapse/expand functionality to the group.
// ***************************************************************************

#ifndef G_VISIBLEALIGNMENTGROUPHEADER_H
#define G_VISIBLEALIGNMENTGROUPHEADER_H

#include <QWidget>
class QString;

namespace Gambit {

class GColorScheme;

namespace Viewer {

class GVisibleAlignmentGroupHeader : public QWidget {

    Q_OBJECT

    public:
        GVisibleAlignmentGroupHeader(const QString& readGroup, const GColorScheme& colorScheme, QWidget* parent = 0);
        ~GVisibleAlignmentGroupHeader(void);

    public:
        void SetColorScheme(const GColorScheme& colorScheme);

    public slots:
        void SetReadGroup(const QString& readGroup);

    signals:
        void ExpandRequested(void);
        void CollapseRequested(void);

    private slots:
        void ExpandButtonClicked(void);
        void CollapseButtonClicked(void);

    private:
        struct GVisibleAlignmentGroupHeaderPrivate;
        GVisibleAlignmentGroupHeaderPrivate* d;
};

} // namespace Viewer
} // namespace Gambit

#endif // G_VISIBLEALIGNMENTGROUPHEADER_H
