// ***************************************************************************
// GManhattanStyle.h (c) 2009 Derek Barnett
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
// (ManhattanStyle.h/.cpp), distributed under LGPL v2.1.
// Modified to fit Gambit style conventions, and inclusion in to Gambit
// namespaces
// ***************************************************************************

#ifndef G_MANHATTANSTYLE_H
#define G_MANHATTANSTYLE_H

#include <QStyle>
#include <QWindowsStyle>
class QLinearGradient;
class QBrush;

namespace Gambit {

class GManhattanStylePrivate;

class GManhattanStyle : public QWindowsStyle
{
    Q_OBJECT

public:
    GManhattanStyle(const QString &);

    ~GManhattanStyle();

    QStyle *systemStyle() const;

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = 0) const;

    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const;
    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget) const;

    SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option, const QPoint &pos, const QWidget *widget = 0) const;
    QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget = 0) const;
    int styleHint(StyleHint hint, const QStyleOption *option = 0, const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const;
    QRect itemRect(QPainter *p, const QRect &r, int flags, bool enabled, const QPixmap *pixmap, const QString &text, int len = -1) const;
    QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *opt) const;

    int pixelMetric(PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const;

    QPalette standardPalette() const;

    void polish(QWidget *widget);
    void polish(QPalette &pal);
    void polish(QApplication *app);

    void unpolish(QWidget *widget);
    void unpolish(QApplication *app);

protected:
    bool event(QEvent *e);

protected Q_SLOTS:
    QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const;
    int layoutSpacingImplementation(QSizePolicy::ControlType control1,
                                    QSizePolicy::ControlType control2,
                                    Qt::Orientation orientation,
                                    const QStyleOption *option = 0,
                                    const QWidget *widget = 0) const;

private:
    GManhattanStylePrivate *d;
    Q_DISABLE_COPY(GManhattanStyle)
};

} // namespace Gambit

#endif // G_MANHATTANSTYLE_H
