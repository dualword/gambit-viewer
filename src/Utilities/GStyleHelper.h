// ***************************************************************************
// GStyleHelper.h (c) 2009 Derek Barnett
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
// Provides application style.  Modified from QtCreator v1.1 source
// (StyleHelper.h/.cpp), distributed under LGPL v2.1.
// Modified to fit Gambit style conventions, and inclusion in to Gambit
// namespaces. Also modified to provide "gambit-green" as base color for some
// widgets.
// ***************************************************************************

#ifndef G_STYLEHELPER_H
#define G_STYLEHELPER_H

#include <QtCore/QRect>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QPalette>
#include <QtGui/QColor>

// Helper class holding all custom color values

namespace Gambit {

class GStyleHelper {
    public:
        // Height of the project explorer navigation bar
        static int navigationWidgetHeight() { return 24; }
        static qreal sidebarFontSize();
        static QPalette sidebarFontPalette(const QPalette &original);

        // This is our color table, all colors derive from baseColor
        static QColor baseColor();
        static QColor panelTextColor();
        static QColor highlightColor();
        static QColor shadowColor();
        static QColor borderColor();
        static QColor buttonTextColor() { return QColor(0x4c4c4c); }
        static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50);

        // Sets the base color and makes sure all top level widgets are updated
        static void setBaseColor(const QColor &color);

        // Gradients used for panels
        static void horizontalGradient(QPainter *painter, const QRect &spanRect, const QRect &clipRect);
        static void verticalGradient(QPainter *painter, const QRect &spanRect, const QRect &clipRect);
        static void menuGradient(QPainter *painter, const QRect &spanRect, const QRect &clipRect);

        // Pixmap cache should only be enabled for X11 due to slow gradients
        static bool usePixmapCache() { return true; }

    private:
        static QColor m_baseColor;
};

} // namespace Gambit

#endif // G_STYLEHELPER_H
