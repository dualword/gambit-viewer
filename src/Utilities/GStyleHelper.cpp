// ***************************************************************************
// GStyleHelper.cpp (c) 2009 Derek Barnett
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

#include <QtGui>
#include "Utilities/GStyleHelper.h"
using namespace Gambit;

// Clamps float color values within (0, 255)
static int clamp(float x)
{
    const int val = x > 255 ? 255 : static_cast<int>(x);
    return val < 0 ? 0 : val;
}

QColor GStyleHelper::mergedColors(const QColor &colorA, const QColor &colorB, int factor)
{
    const int maxFactor = 100;
    QColor tmp = colorA;
    tmp.setRed((tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor);
    tmp.setGreen((tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor);
    tmp.setBlue((tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor);
    return tmp;
}

qreal GStyleHelper::sidebarFontSize()
{
#if defined(Q_WS_MAC)
    return 9;
#else
    return 7.5;
#endif
}

QPalette GStyleHelper::sidebarFontPalette(const QPalette &original)
{
    QPalette palette = original;
    palette.setColor(QPalette::Active, QPalette::Text, panelTextColor());
    palette.setColor(QPalette::Active, QPalette::WindowText, panelTextColor());
    palette.setColor(QPalette::Inactive, QPalette::Text, panelTextColor().darker());
    palette.setColor(QPalette::Inactive, QPalette::WindowText, panelTextColor().darker());
    return palette;
}

QColor GStyleHelper::panelTextColor()
{
    //qApp->palette().highlightedText().color();
    return Qt::white;
}

QColor GStyleHelper::m_baseColor(0x666600);   // a good yellow-green - goes with 'Gambit logo' on Home page
//QColor GStyleHelper::m_baseColor(0x666666);   // original QtCreator gray

QColor GStyleHelper::baseColor()
{
    return m_baseColor;
}

QColor GStyleHelper::highlightColor()
{
    QColor result = baseColor();
    result.setHsv(result.hue(),
                  clamp(result.saturation()),
                  clamp(result.value() * 1.16));
    return result;
}

QColor GStyleHelper::shadowColor()
{
    QColor result = baseColor();
    result.setHsv(result.hue(),
                  clamp(result.saturation() * 1.1),
                  clamp(result.value() * 0.70));
    return result;
}

QColor GStyleHelper::borderColor()
{
    QColor result = baseColor();
    result.setHsv(result.hue(),
                  result.saturation(),
                  result.value() / 2);
    return result;
}

void GStyleHelper::setBaseColor(const QColor &color)
{
    if (color.isValid() && color != m_baseColor) {
        m_baseColor = color;
        foreach (QWidget *w, QApplication::topLevelWidgets())
            w->update();
    }
}

static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50)
{
    const int maxFactor = 100;
    QColor tmp = colorA;
    tmp.setRed((tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor);
    tmp.setGreen((tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor);
    tmp.setBlue((tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor);
    return tmp;
}

void GStyleHelper::verticalGradient(QPainter *painter, const QRect &spanRect, const QRect &clipRect)
{
    QString key;
    key.sprintf("mh_toolbar %d %d %d %d %d", spanRect.width(), spanRect.height(), clipRect.width(),
                                             clipRect.height(), GStyleHelper::baseColor().rgb());;
    QPixmap pixmap;
    QPainter *p = painter;
    QRect rect = clipRect;
    if (GStyleHelper::usePixmapCache() && !QPixmapCache::find(key, pixmap)) {
        pixmap = QPixmap(clipRect.size());
        p = new QPainter(&pixmap);
        rect = QRect(0, 0, clipRect.width(), clipRect.height());
    }

    QColor base = GStyleHelper::baseColor();
    QLinearGradient grad(spanRect.topRight(), spanRect.topLeft());
    grad.setColorAt(0, highlightColor());
    grad.setColorAt(0.301, base);
    grad.setColorAt(1, shadowColor());
    p->fillRect(rect, grad);

    QColor light(255, 255, 255, 80);
    p->setPen(light);
    p->drawLine(rect.topRight() - QPoint(1, 0), rect.bottomRight() - QPoint(1, 0));

    if (GStyleHelper::usePixmapCache() && !QPixmapCache::find(key, pixmap)) {
        painter->drawPixmap(clipRect.topLeft(), pixmap);
        p->end();
        delete p;
        QPixmapCache::insert(key, pixmap);
    }
}

void GStyleHelper::horizontalGradient(QPainter *painter, const QRect &spanRect, const QRect &clipRect)
{
    QString key;
    key.sprintf("mh_toolbar %d %d %d %d %d", spanRect.width(), spanRect.height(),
                clipRect.width(), clipRect.height(), GStyleHelper::baseColor().rgb());
    QPixmap pixmap;
    QPainter *p = painter;
    QRect rect = clipRect;
    if (GStyleHelper::usePixmapCache() && !QPixmapCache::find(key, pixmap)) {
        pixmap = QPixmap(clipRect.size());
        p = new QPainter(&pixmap);
        rect = QRect(0, 0, clipRect.width(), clipRect.height());
    }

    QColor base = GStyleHelper::baseColor();
    QLinearGradient grad(rect.topLeft(), rect.bottomLeft());

    grad.setColorAt(0, highlightColor().lighter(120));
    grad.setColorAt(0.4, highlightColor());
    grad.setColorAt(0.401, base);
    grad.setColorAt(1, shadowColor());
    p->fillRect(rect, grad);

    QLinearGradient shadowGradient(spanRect.topLeft(), spanRect.topRight());
    shadowGradient.setColorAt(0, QColor(0, 0, 0, 30));
    QColor highlight = highlightColor().lighter(130);
    highlight.setAlpha(100);
    shadowGradient.setColorAt(0.7, highlight);
    shadowGradient.setColorAt(1, QColor(0, 0, 0, 40));
    p->fillRect(rect, shadowGradient);

    if (GStyleHelper::usePixmapCache() && !QPixmapCache::find(key, pixmap)) {
        painter->drawPixmap(clipRect.topLeft(), pixmap);
        p->end();
        delete p;
        QPixmapCache::insert(key, pixmap);
    }
}

void GStyleHelper::menuGradient(QPainter *painter, const QRect &spanRect, const QRect &clipRect)
{
    QString key;
    key.sprintf("mh_toolbar %d %d %d %d %d", spanRect.width(), spanRect.height(), clipRect.width(),
                                             clipRect.height(), GStyleHelper::baseColor().rgb());;
    QPixmap pixmap;
    QPainter *p = painter;
    QRect rect = clipRect;
    if (GStyleHelper::usePixmapCache() && !QPixmapCache::find(key, pixmap)) {
        pixmap = QPixmap(clipRect.size());
        p = new QPainter(&pixmap);
        rect = QRect(0, 0, clipRect.width(), clipRect.height());
    }

    QLinearGradient grad(spanRect.topLeft(), spanRect.bottomLeft());
    QColor menuColor = mergedColors(GStyleHelper::baseColor(), QColor(244, 244, 244), 25);
    grad.setColorAt(0, menuColor.lighter(112));
    grad.setColorAt(1, menuColor);
    p->fillRect(rect, grad);

    if (GStyleHelper::usePixmapCache() && !QPixmapCache::find(key, pixmap)) {
        painter->drawPixmap(clipRect.topLeft(), pixmap);
        p->end();
        delete p;
        QPixmapCache::insert(key, pixmap);
    }
}
