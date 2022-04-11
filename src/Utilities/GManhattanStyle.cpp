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

#include "Utilities/GManhattanStyle.h"
#include "Utilities/GStyleHelper.h"
#include "Utilities/GStyleAnimator.h"

#include <QtCore/QDebug>
#include <QtCore/QLibrary>
#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDockWidget>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QPixmapCache>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QStyleFactory>
#include <QtGui/QStyleOption>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>

using namespace Gambit;

// We define a currently unused state for indicating animations
#define State_Animating 0x00000040

namespace Gambit {

// Because designer needs to disable this for widget previews
// we have a custom property that is inherited
bool styleEnabled(const QWidget *widget)
{
    const QWidget *p = widget;
    while (p) {
        if (p->property("_q_custom_style_disabled").toBool())
            return false;
            p = p->parentWidget();
    }
    return true;
}

// Consider making this a QStyle state
bool panelWidget(const QWidget *widget)
{
    const QWidget *p = widget;

    while (p) {
        if (qobject_cast<const QToolBar *>(p) && styleEnabled(p))
            return true;
        else if (qobject_cast<const QStatusBar *>(p) && styleEnabled(p))
            return true;
        else if (qobject_cast<const QMenuBar *>(p) && styleEnabled(p))
            return true;
        p = p->parentWidget();
    }
    return false;
}

class GManhattanStylePrivate
{
public:
    GManhattanStylePrivate(const QString &baseStyleName)
    {
        style = QStyleFactory::create(baseStyleName);
        buttonImage_pressed = QImage(":/core/images/pushbutton_pressed.png");
        buttonImage = QImage(":/core/images/pushbutton.png");

        lineeditImage = QImage(":/core/images/inputfield.png");
        lineeditImage_disabled = QImage(":/core/images/inputfield_disabled.png");
    }

    ~GManhattanStylePrivate()
    {
        delete style;
        style = 0;
    }

    void init();

public:
    QStyle *style;
    QImage buttonImage;
    QImage buttonImage_pressed;
    QImage lineeditImage;
    QImage lineeditImage_disabled;

    GStyleAnimator animator;
};

GManhattanStyle::GManhattanStyle(const QString &baseStyleName)
    : QWindowsStyle(), d(new GManhattanStylePrivate(baseStyleName))
{
}

GManhattanStyle::~GManhattanStyle()
{
    delete d;
    d = 0;
}

QStyle *GManhattanStyle::systemStyle() const
{
    return d->style;
}

// Draws a CSS-like border image where the defined borders are not stretched
void drawCornerImage(const QImage &img, QPainter *painter, QRect rect,
                     int left = 0, int top = 0, int right = 0,
                     int bottom = 0)
{
    QSize size = img.size();
    if (top > 0) { //top
        painter->drawImage(QRect(rect.left() + left, rect.top(), rect.width() -right - left, top), img,
                           QRect(left, 0, size.width() -right - left, top));
        if (left > 0) //top-left
            painter->drawImage(QRect(rect.left(), rect.top(), left, top), img,
                               QRect(0, 0, left, top));
        if (right > 0) //top-right
            painter->drawImage(QRect(rect.left() + rect.width() - right, rect.top(), right, top), img,
                               QRect(size.width() - right, 0, right, top));
    }
    //left
    if (left > 0)
        painter->drawImage(QRect(rect.left(), rect.top()+top, left, rect.height() - top - bottom), img,
                           QRect(0, top, left, size.height() - bottom - top));
    //center
    painter->drawImage(QRect(rect.left() + left, rect.top()+top, rect.width() -right - left,
                             rect.height() - bottom - top), img,
                       QRect(left, top, size.width() -right -left,
                             size.height() - bottom - top));
    if (right > 0) //right
        painter->drawImage(QRect(rect.left() +rect.width() - right, rect.top()+top, right, rect.height() - top - bottom), img,
                           QRect(size.width() - right, top, right, size.height() - bottom - top));
    if (bottom > 0) { //bottom
        painter->drawImage(QRect(rect.left() +left, rect.top() + rect.height() - bottom,
                                 rect.width() - right - left, bottom), img,
                           QRect(left, size.height() - bottom,
                                 size.width() - right - left, bottom));
    if (left > 0) //bottom-left
        painter->drawImage(QRect(rect.left(), rect.top() + rect.height() - bottom, left, bottom), img,
                           QRect(0, size.height() - bottom, left, bottom));
    if (right > 0) //bottom-right
        painter->drawImage(QRect(rect.left() + rect.width() - right, rect.top() + rect.height() - bottom, right, bottom), img,
                           QRect(size.width() - right, size.height() - bottom, right, bottom));
    }
}

QPixmap GManhattanStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *opt) const
{
    QPixmap result;
    result = d->style->generatedIconPixmap(iconMode, pixmap, opt);
    return result;
}

int GManhattanStyle::layoutSpacingImplementation(QSizePolicy::ControlType control1,
                                                 QSizePolicy::ControlType control2,
                                                 Qt::Orientation orientation,
                                                 const QStyleOption * option ,
                                                 const QWidget * widget ) const
{
    return d->style->layoutSpacing(control1, control2, orientation, option, widget);

}

QSize GManhattanStyle::sizeFromContents(ContentsType type, const QStyleOption *option,
                                       const QSize &size, const QWidget *widget) const
{
    QSize newSize = d->style->sizeFromContents(type, option, size, widget);

    if (type == CT_Splitter && widget && widget->property("minisplitter").toBool())
        return QSize(1, 1);
    else if (type == CT_ComboBox && panelWidget(widget))
        newSize += QSize(14, 0);
    return newSize;
}

QRect GManhattanStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    QRect rect;
    rect = d->style->subElementRect(element, option, widget);
    return rect;
}

QRect GManhattanStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                     SubControl subControl, const QWidget *widget) const
{
    QRect rect;
    rect = d->style->subControlRect(control, option, subControl, widget);
    return rect;
}

QStyle::SubControl GManhattanStyle::hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                                         const QPoint &pos, const QWidget *widget) const
{
    SubControl result = QStyle::SC_None;
    result = d->style->hitTestComplexControl(control, option, pos, widget);
    return result;
}

int GManhattanStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    int retval = 0;
    retval = d->style->pixelMetric(metric, option, widget);
    switch (metric) {
    case PM_SplitterWidth:
        if (widget && widget->property("minisplitter").toBool())
            retval = 1;
        break;
    case PM_ToolBarIconSize:
        if (panelWidget(widget))
            retval = 16;
        break;
    case PM_MenuPanelWidth:
    case PM_MenuBarHMargin:
    case PM_MenuBarVMargin:
    case PM_ToolBarFrameWidth:
        if (panelWidget(widget))
            retval = 1;
        break;
    case PM_ButtonShiftVertical:
    case PM_ButtonShiftHorizontal:
    case PM_MenuBarPanelWidth:
    case PM_ToolBarItemMargin:
    case PM_ToolBarItemSpacing:
        if (panelWidget(widget))
            retval = 0;
        break;
    case PM_DefaultFrameWidth:
        if (qobject_cast<const QLineEdit*>(widget) && panelWidget(widget))
            return 1;
        break;
    default:
        break;
    }
    return retval;
}

QPalette GManhattanStyle::standardPalette() const
{
    QPalette result;
    result = d->style->standardPalette();
    return result;
}

void GManhattanStyle::polish(QApplication *app)
{
    d->style->polish(app);
}

void GManhattanStyle::unpolish(QApplication *app)
{
    d->style->unpolish(app);
}

QPalette panelPalette(const QPalette &oldPalette)
{
    QColor color = GStyleHelper::panelTextColor();
    QPalette pal = oldPalette;
    pal.setBrush(QPalette::All, QPalette::WindowText, color);
    pal.setBrush(QPalette::All, QPalette::ButtonText, color);
    pal.setBrush(QPalette::All, QPalette::Foreground, color);
    color.setAlpha(100);
    pal.setBrush(QPalette::Disabled, QPalette::WindowText, color);
    pal.setBrush(QPalette::Disabled, QPalette::ButtonText, color);
    pal.setBrush(QPalette::Disabled, QPalette::Foreground, color);
    return pal;
}

void GManhattanStyle::polish(QWidget *widget)
{
    d->style->polish(widget);

    // OxygenStyle forces a rounded widget mask on toolbars
    if (d->style->inherits("OxygenStyle")) {
        if (qobject_cast<QToolBar*>(widget))
            widget->removeEventFilter(d->style);
    }
    if (panelWidget(widget)) {
        if (qobject_cast<QToolButton*>(widget)) {
            widget->setAttribute(Qt::WA_Hover);
            widget->setMaximumHeight(GStyleHelper::navigationWidgetHeight() - 2);
            widget->setAttribute(Qt::WA_Hover);
        }
        else if (qobject_cast<QLineEdit*>(widget)) {
            widget->setAttribute(Qt::WA_Hover);
            widget->setMaximumHeight(GStyleHelper::navigationWidgetHeight() - 2);
        }
        else if (qobject_cast<QLabel*>(widget))
            widget->setPalette(panelPalette(widget->palette()));
        else if (qobject_cast<QToolBar*>(widget))
            widget->setMinimumHeight(GStyleHelper::navigationWidgetHeight());
        else if (qobject_cast<QStatusBar*>(widget))
            widget->setFixedHeight(GStyleHelper::navigationWidgetHeight() + 2);
        else if (qobject_cast<QComboBox*>(widget)) {
            widget->setMaximumHeight(GStyleHelper::navigationWidgetHeight() - 2);
            widget->setAttribute(Qt::WA_Hover);
        }
    }
}

void GManhattanStyle::unpolish(QWidget *widget)
{
    d->style->unpolish(widget);
    if (panelWidget(widget)) {
        if (qobject_cast<QTabBar*>(widget))
            widget->setAttribute(Qt::WA_Hover, false);
        else if (qobject_cast<QToolBar*>(widget))
            widget->setAttribute(Qt::WA_Hover, false);
        else if (qobject_cast<QComboBox*>(widget))
            widget->setAttribute(Qt::WA_Hover, false);
    }
}

void GManhattanStyle::polish(QPalette &pal)
{
    d->style->polish(pal);
}

QIcon GManhattanStyle::standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option,
                                                 const QWidget *widget) const
{
    QIcon icon;
    switch (standardIcon) {
    case QStyle::SP_TitleBarCloseButton:
    case QStyle::SP_ToolBarHorizontalExtensionButton:
        return QIcon(standardPixmap(standardIcon, option, widget));
    default:
        icon = d->style->standardIcon(standardIcon, option, widget);
    }
    return icon;
}

QPixmap GManhattanStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                                       const QWidget *widget) const
{
    if (widget && !panelWidget(widget))
        return d->style->standardPixmap(standardPixmap, opt, widget);

    QPixmap pixmap;
    switch (standardPixmap) {
    case QStyle::SP_ToolBarHorizontalExtensionButton: {
            static const QPixmap extButton(":/core/images/extension.png");
            pixmap = extButton;
        }
        break;
    case QStyle::SP_TitleBarCloseButton: {
            static const QPixmap closeButton(":/core/images/closebutton.png");
            pixmap = closeButton;
        }
        break;
    default:
        pixmap = d->style->standardPixmap(standardPixmap, opt, widget);
    }
    return pixmap;
}


int GManhattanStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget,
                              QStyleHintReturn *returnData) const
{
    int ret = d->style->styleHint(hint, option, widget, returnData);
    switch (hint) {
    // Make project explorer alternate rows all the way
    case QStyle::SH_ItemView_PaintAlternatingRowColorsForEmptyArea:
        if (widget && widget->property("AlternateEmpty").toBool())
            ret = true;
        break;
    case QStyle::SH_EtchDisabledText:
        if (panelWidget(widget))
            ret = false;
        break;
    default:
        break;
    }
    return ret;
}

void GManhattanStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                                   QPainter *painter, const QWidget *widget) const
{
    if (!panelWidget(widget))
        return d->style->drawPrimitive(element, option, painter, widget);

    bool animating = (option->state & State_Animating);
    int state = option->state;
    QRect rect = option->rect;
    QRect oldRect;
    QRect newRect;
    if (widget && (element == PE_PanelButtonTool) && !animating) {
        QWidget *w = const_cast<QWidget *> (widget);
        int oldState = w->property("_q_stylestate").toInt();
        oldRect = w->property("_q_stylerect").toRect();
        newRect = w->rect();
        w->setProperty("_q_stylestate", (int)option->state);
        w->setProperty("_q_stylerect", w->rect());

        // Determine the animated transition
        bool doTransition = ((state & State_On)         != (oldState & State_On)     ||
                             (state & State_MouseOver)  != (oldState & State_MouseOver));
        if (oldRect != newRect)
        {
            doTransition = false;
            d->animator.stopAnimation(widget);
        }

        if (doTransition) {
            QImage startImage(option->rect.size(), QImage::Format_ARGB32_Premultiplied);
            QImage endImage(option->rect.size(), QImage::Format_ARGB32_Premultiplied);
            Animation *anim = d->animator.widgetAnimation(widget);
            QStyleOption opt = *option;
            opt.state = (QStyle::State)oldState;
            opt.state |= (State)State_Animating;
            startImage.fill(0);
            Transition *t = new Transition;
            t->setWidget(w);
            QPainter startPainter(&startImage);
            if (!anim) {
                drawPrimitive(element, &opt, &startPainter, widget);
            } else {
                anim->paint(&startPainter, &opt);
                d->animator.stopAnimation(widget);
            }
            QStyleOption endOpt = *option;
            endOpt.state |= (State)State_Animating;
            t->setStartImage(startImage);
            d->animator.startAnimation(t);
            endImage.fill(0);
            QPainter endPainter(&endImage);
            drawPrimitive(element, &endOpt, &endPainter, widget);
            t->setEndImage(endImage);
            t->setDuration(130);
            t->setStartTime(QTime::currentTime());
        }
    }

    switch (element) {
    case PE_PanelLineEdit:
        {
            painter->save();
            if (option->state & State_Enabled)
                drawCornerImage(d->lineeditImage, painter, option->rect, 2, 2, 2, 2);
            else
                drawCornerImage(d->lineeditImage_disabled, painter, option->rect, 2, 2, 2, 2);

            if (option->state & State_HasFocus || option->state & State_MouseOver) {
                QColor hover = GStyleHelper::baseColor();
                if (state & State_HasFocus)
                    hover.setAlpha(100);
                else
                    hover.setAlpha(50);

                painter->setPen(QPen(hover, 1));
                painter->drawRect(option->rect.adjusted(1, 1, -2 ,-2));
            }
            painter->restore();
        }
        break;

    case PE_FrameStatusBarItem:
        break;

    case PE_PanelButtonTool: {
            Animation *anim = d->animator.widgetAnimation(widget);
            if (!animating && anim) {
                anim->paint(painter, option);
            } else {
                bool pressed = option->state & State_Sunken || option->state & State_On;
                QColor shadow(0, 0, 0, 30);
                painter->setPen(shadow);
                if (pressed) {
                    QColor shade(0, 0, 0, 40);
                    painter->fillRect(rect, shade);
                    painter->drawLine(rect.topLeft() + QPoint(1, 0), rect.topRight() - QPoint(1, 0));
                    painter->drawLine(rect.topLeft(), rect.bottomLeft());
                    painter->drawLine(rect.topRight(), rect.bottomRight());
                   // painter->drawLine(rect.bottomLeft()  + QPoint(1, 0), rect.bottomRight()  - QPoint(1, 0));
                    QColor highlight(255, 255, 255, 30);
                    painter->setPen(highlight);
                }
                else if (option->state & State_Enabled &&
                         option->state & State_MouseOver) {
                    QColor lighter(255, 255, 255, 37);
                    painter->fillRect(rect, lighter);
                }
           }
        }
        break;

    case PE_PanelStatusBar:
        {
            painter->save();
            QLinearGradient grad(option->rect.topLeft(), QPoint(rect.center().x(), rect.bottom()));
            QColor startColor = GStyleHelper::shadowColor().darker(164);
            QColor endColor = GStyleHelper::baseColor().darker(130);
            grad.setColorAt(0, startColor);
            grad.setColorAt(1, endColor);
            painter->fillRect(option->rect, grad);
            painter->setPen(QColor(255, 255, 255, 60));
            painter->drawLine(rect.topLeft() + QPoint(0,1),
                              rect.topRight()+ QPoint(0,1));
            painter->setPen(GStyleHelper::borderColor().darker(110));
            painter->drawLine(rect.topLeft(), rect.topRight());
            painter->restore();
        }
        break;

    case PE_IndicatorToolBarSeparator:
        {
            QColor separatorColor = GStyleHelper::borderColor();
            separatorColor.setAlpha(100);
            painter->setPen(separatorColor);
            const int margin = 6;
            if (option->state & State_Horizontal) {
                const int offset = rect.width()/2;
                painter->drawLine(rect.bottomLeft().x() + offset,
                            rect.bottomLeft().y() - margin,
                            rect.topLeft().x() + offset,
                            rect.topLeft().y() + margin);
            } else { //Draw vertical separator
                const int offset = rect.height()/2;
                painter->setPen(QPen(option->palette.background().color().darker(110)));
                painter->drawLine(rect.topLeft().x() + margin ,
                            rect.topLeft().y() + offset,
                            rect.topRight().x() - margin,
                            rect.topRight().y() + offset);
            }
        }
        break;

    case PE_IndicatorToolBarHandle:
        {
            bool horizontal = option->state & State_Horizontal;
            painter->save();
            QPainterPath path;
            int x = option->rect.x() + horizontal ? 2 : 6;
            int y = option->rect.y() + horizontal ? 6 : 2;
            static const int RectHeight = 2;
            if (horizontal) {
                while (y < option->rect.height() - RectHeight - 6) {
                    path.moveTo(x, y);
                    path.addRect(x, y, RectHeight, RectHeight);
                    y += 6;
                }
            } else {
                while (x < option->rect.width() - RectHeight - 6) {
                    path.moveTo(x, y);
                    path.addRect(x, y, RectHeight, RectHeight);
                    x += 6;
                }
            }

            painter->setPen(Qt::NoPen);
            QColor dark = GStyleHelper::borderColor();
            dark.setAlphaF(0.4);

            QColor light = GStyleHelper::baseColor();
            light.setAlphaF(0.4);

            painter->fillPath(path, light);
            painter->save();
            painter->translate(1, 1);
            painter->fillPath(path, dark);
            painter->restore();
            painter->translate(3, 3);
            painter->fillPath(path, light);
            painter->translate(1, 1);
            painter->fillPath(path, dark);
            painter->restore();
        }
        break;
    case PE_IndicatorArrowUp:
    case PE_IndicatorArrowDown:
    case PE_IndicatorArrowRight:
    case PE_IndicatorArrowLeft:
        {
            // From windowsstyle but modified to enable AA
            if (option->rect.width() <= 1 || option->rect.height() <= 1)
                break;

            QRect r = option->rect;
            int size = qMin(r.height(), r.width());
            QPixmap pixmap;
            QString pixmapName;
            pixmapName.sprintf("%s-%s-%d-%d-%d-%lld",
                               "$qt_ia", metaObject()->className(),
                               uint(option->state), element,
                               size, option->palette.cacheKey());
            if (!QPixmapCache::find(pixmapName, pixmap)) {
                int border = size/5;
                int sqsize = 2*(size/2);
                QImage image(sqsize, sqsize, QImage::Format_ARGB32);
                image.fill(Qt::transparent);
                QPainter imagePainter(&image);
                imagePainter.setRenderHint(QPainter::Antialiasing, true);
                imagePainter.translate(0.5, 0.5);
                QPolygon a;
                switch (element) {
                    case PE_IndicatorArrowUp:
                        a.setPoints(3, border, sqsize/2,  sqsize/2, border,  sqsize - border, sqsize/2);
                        break;
                    case PE_IndicatorArrowDown:
                        a.setPoints(3, border, sqsize/2,  sqsize/2, sqsize - border,  sqsize - border, sqsize/2);
                        break;
                    case PE_IndicatorArrowRight:
                        a.setPoints(3, sqsize - border, sqsize/2,  sqsize/2, border,  sqsize/2, sqsize - border);
                        break;
                    case PE_IndicatorArrowLeft:
                        a.setPoints(3, border, sqsize/2,  sqsize/2, border,  sqsize/2, sqsize - border);
                        break;
                    default:
                        break;
                }

                int bsx = 0;
                int bsy = 0;

                if (option->state & State_Sunken) {
                    bsx = pixelMetric(PM_ButtonShiftHorizontal);
                    bsy = pixelMetric(PM_ButtonShiftVertical);
                }

                QRect bounds = a.boundingRect();
                int sx = sqsize / 2 - bounds.center().x() - 1;
                int sy = sqsize / 2 - bounds.center().y() - 1;
                imagePainter.translate(sx + bsx, sy + bsy);

                if (!(option->state & State_Enabled)) {
                    QColor foreGround(150, 150, 150, 150);
                    imagePainter.setBrush(option->palette.mid().color());
                    imagePainter.setPen(option->palette.mid().color());
                } else {
                    QColor shadow(0, 0, 0, 100);
                    imagePainter.translate(0, 1);
                    imagePainter.setPen(shadow);
                    imagePainter.setBrush(shadow);
                    QColor foreGround(255, 255, 255, 210);
                    imagePainter.drawPolygon(a);
                    imagePainter.translate(0, -1);
                    imagePainter.setPen(foreGround);
                    imagePainter.setBrush(foreGround);
                }
                imagePainter.drawPolygon(a);
                imagePainter.end();
                pixmap = QPixmap::fromImage(image);
                QPixmapCache::insert(pixmapName, pixmap);
            }
            int xOffset = r.x() + (r.width() - size)/2;
            int yOffset = r.y() + (r.height() - size)/2;
            painter->drawPixmap(xOffset, yOffset, pixmap);
        }
        break;

    default:
        d->style->drawPrimitive(element, option, painter, widget);
        break;
    }
}

void GManhattanStyle::drawControl(ControlElement element, const QStyleOption *option,
                                 QPainter *painter, const QWidget *widget) const
{
    if (!panelWidget(widget))
        return d->style->drawControl(element, option, painter, widget);

    switch (element) {
    case CE_MenuBarItem:
        painter->save();
        if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            QColor highlightOutline = GStyleHelper::borderColor().lighter(120);
            bool act = mbi->state & State_Selected && mbi->state & State_Sunken;
            bool dis = !(mbi->state & State_Enabled);
            GStyleHelper::menuGradient(painter, option->rect, option->rect);
            QStyleOptionMenuItem item = *mbi;
            item.rect = mbi->rect;
            QPalette pal = mbi->palette;
            pal.setBrush(QPalette::ButtonText, dis ? Qt::gray : Qt::black);
            item.palette = pal;
            QCommonStyle::drawControl(element, &item, painter, widget);
            QRect r = option->rect;

            if (act) {
                // Fill|
                QColor baseColor = GStyleHelper::baseColor();
                QLinearGradient grad(option->rect.topLeft(), option->rect.bottomLeft());
                grad.setColorAt(0, baseColor.lighter(120));
                grad.setColorAt(1, baseColor.lighter(130));
                painter->fillRect(option->rect.adjusted(1, 1, -1, 0), grad);

                // Outline
                painter->setPen(QPen(highlightOutline, 0));
                painter->drawLine(QPoint(r.left(), r.top() + 1), QPoint(r.left(), r.bottom()));
                painter->drawLine(QPoint(r.right(), r.top() + 1), QPoint(r.right(), r.bottom()));
                painter->drawLine(QPoint(r.left() + 1, r.top()), QPoint(r.right() - 1, r.top()));
                highlightOutline.setAlpha(60);
                painter->setPen(QPen(highlightOutline, 0));
                painter->drawPoint(r.topLeft());
                painter->drawPoint(r.topRight());

                QPalette pal = mbi->palette;
                uint alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
                if (!styleHint(SH_UnderlineShortcut, mbi, widget))
                    alignment |= Qt::TextHideMnemonic;
                pal.setBrush(QPalette::Text, dis ? Qt::gray : QColor(0, 0, 0, 60));
                drawItemText(painter, item.rect.translated(0, 1), alignment, pal, mbi->state & State_Enabled, mbi->text, QPalette::Text);
                pal.setBrush(QPalette::Text, dis ? Qt::gray : Qt::white);
                drawItemText(painter, item.rect, alignment, pal, mbi->state & State_Enabled, mbi->text, QPalette::Text);
            }
        }
        painter->restore();
        break;

    case CE_ComboBoxLabel:
        if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            if (panelWidget(widget)) {
                QRect editRect = subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField, widget);
                QPalette customPal = cb->palette;

                if (!cb->currentIcon.isNull()) {
                    QIcon::Mode mode = cb->state & State_Enabled ? QIcon::Normal
                                                                 : QIcon::Disabled;
                    QPixmap pixmap = cb->currentIcon.pixmap(cb->iconSize, mode);
                    QRect iconRect(editRect);
                    iconRect.setWidth(cb->iconSize.width() + 4);
                    iconRect = alignedRect(cb->direction,
                                           Qt::AlignLeft | Qt::AlignVCenter,
                                           iconRect.size(), editRect);
                    if (cb->editable)
                        painter->fillRect(iconRect, customPal.brush(QPalette::Base));
                    drawItemPixmap(painter, iconRect, Qt::AlignCenter, pixmap);

                    if (cb->direction == Qt::RightToLeft)
                        editRect.translate(-4 - cb->iconSize.width(), 0);
                    else
                        editRect.translate(cb->iconSize.width() + 4, 0);

                    // Reserve some space for the down-arrow
                    editRect.adjust(0, 0, -13, 0);
                }

                customPal.setBrush(QPalette::All, QPalette::ButtonText, QColor(0, 0, 0, 70));

                QString text = option->fontMetrics.elidedText(cb->currentText, Qt::ElideRight, editRect.width());
                drawItemText(painter, editRect.translated(0, 1),
                             visualAlignment(option->direction, Qt::AlignLeft | Qt::AlignVCenter),
                             customPal, cb->state & State_Enabled, text, QPalette::ButtonText);
                customPal.setBrush(QPalette::All, QPalette::ButtonText, GStyleHelper::panelTextColor());
                drawItemText(painter, editRect,
                             visualAlignment(option->direction, Qt::AlignLeft | Qt::AlignVCenter),
                             customPal, cb->state & State_Enabled, text, QPalette::ButtonText);
            } else {
                d->style->drawControl(element, option, painter, widget);
            }
        }
        break;

    case CE_SizeGrip: {
            painter->save();
            QColor dark = Qt::white;
            dark.setAlphaF(0.1);
            int x, y, w, h;
            option->rect.getRect(&x, &y, &w, &h);
            int sw = qMin(h, w);
            if (h > w)
                painter->translate(0, h - w);
            else
                painter->translate(w - h, 0);
            int sx = x;
            int sy = y;
            int s = 4;
            painter->setPen(dark);
            if (option->direction == Qt::RightToLeft) {
                sx = x + sw;
                for (int i = 0; i < 4; ++i) {
                    painter->drawLine(x, sy, sx, sw);
                    sx -= s;
                    sy += s;
                }
            } else {
                for (int i = 0; i < 4; ++i) {
                    painter->drawLine(sx, sw, sw, sy);
                    sx += s;
                    sy += s;
                }
            }
            painter->restore();
        }
        break;

    case CE_MenuBarEmptyArea: {
            GStyleHelper::menuGradient(painter, option->rect, option->rect);
            painter->save();
            painter->setPen(GStyleHelper::borderColor());
            painter->drawLine(option->rect.bottomLeft(), option->rect.bottomRight());
            painter->restore();
        }
        break;

    case CE_ToolBar:
        {
            QString key;
            key.sprintf("mh_toolbar %d %d %d", option->rect.width(), option->rect.height(), GStyleHelper::baseColor().rgb());;

            QPixmap pixmap;
            QPainter *p = painter;
            QRect rect = option->rect;
            if (GStyleHelper::usePixmapCache() && !QPixmapCache::find(key, pixmap)) {
                pixmap = QPixmap(option->rect.size());
                p = new QPainter(&pixmap);
                rect = QRect(0, 0, option->rect.width(), option->rect.height());
            }

            bool horizontal = option->state & State_Horizontal;
            // Map offset for global window gradient
            QPoint offset = widget->window()->mapToGlobal(option->rect.topLeft()) -
                                                          widget->mapToGlobal(option->rect.topLeft());
            QRect gradientSpan;
            if (widget) {
                gradientSpan = QRect(offset, widget->window()->size());
            }
            if (horizontal)
                GStyleHelper::horizontalGradient(p, gradientSpan, rect);
            else
                GStyleHelper::verticalGradient(p, gradientSpan, rect);

            painter->setPen(GStyleHelper::borderColor());

            if (horizontal) {
                // Note: This is a hack to determine if the
                // toolbar should draw the top or bottom outline
                // (needed for the find toolbar for instance)
                QColor lighter(255, 255, 255, 40);
                if (widget && widget->property("topBorder").toBool()) {
                    p->drawLine(rect.topLeft(), rect.topRight());
                    p->setPen(lighter);
                    p->drawLine(rect.topLeft() + QPoint(0, 1), rect.topRight() + QPoint(0, 1));
                } else {
                    p->drawLine(rect.bottomLeft(), rect.bottomRight());
                    p->setPen(lighter);
                    p->drawLine(rect.topLeft(), rect.topRight());
                }
            } else {
                p->drawLine(rect.topLeft(), rect.bottomLeft());
                p->drawLine(rect.topRight(), rect.bottomRight());
            }

            if (GStyleHelper::usePixmapCache() && !QPixmapCache::find(key, pixmap)) {
                painter->drawPixmap(rect.topLeft(), pixmap);
                p->end();
                delete p;
                QPixmapCache::insert(key, pixmap);
            }
        }
        break;

    default:
        d->style->drawControl(element, option, painter, widget);
        break;
    }
}

void GManhattanStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                        QPainter *painter, const QWidget *widget) const
{
    if (!panelWidget(widget))
         return d->style->drawComplexControl(control, option, painter, widget);

    QRect rect = option->rect;
    switch (control) {
    case CC_ToolButton:
        if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            QString buttonType = widget->property("type").toString();
            QRect button, menuarea;
            button = subControlRect(control, toolbutton, SC_ToolButton, widget);
            menuarea = subControlRect(control, toolbutton, SC_ToolButtonMenu, widget);

            State bflags = toolbutton->state;
            if (bflags & State_AutoRaise) {
                if (!(bflags & State_MouseOver)) {
                    bflags &= ~State_Raised;
                }
            }

            State mflags = bflags;
            if (toolbutton->state & State_Sunken) {
                if (toolbutton->activeSubControls & SC_ToolButton)
                    bflags |= State_Sunken;
                if (toolbutton->activeSubControls & SC_ToolButtonMenu)
                    mflags |= State_Sunken;
            }

            QStyleOption tool(0);
            tool.palette = toolbutton->palette;
            if (toolbutton->subControls & SC_ToolButton) {
                if (buttonType == "dockbutton") {
                    tool.rect = button;
                    tool.state = bflags;
                    drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);
                } else {  // paint status bar button style
                    if (bflags & State_Sunken || bflags & State_On)
                        drawCornerImage(d->buttonImage_pressed, painter, option->rect, 2, 2, 2, 2);
                    else if (bflags & State_Enabled) {
#ifndef Q_WS_MAC
                        if (bflags & State_MouseOver) {
                            drawCornerImage(d->buttonImage, painter, option->rect, 2, 2, 2, 2);
                            QColor shade(255, 255, 255, 50);
                            painter->fillRect(button.adjusted(1, 1, -1, -1), shade);
                        }
#endif
                    }

                }
            }

            if (toolbutton->state & State_HasFocus) {
                QStyleOptionFocusRect fr;
                fr.QStyleOption::operator=(*toolbutton);
                fr.rect.adjust(3, 3, -3, -3);
                if (toolbutton->features & QStyleOptionToolButton::MenuButtonPopup)
                    fr.rect.adjust(0, 0, -pixelMetric(QStyle::PM_MenuButtonIndicator,
                                                      toolbutton, widget), 0);
                QPen oldPen = painter->pen();
                QColor focusColor = GStyleHelper::panelTextColor();
                focusColor.setAlpha(120);
                QPen outline(focusColor, 1);
                outline.setStyle(Qt::DotLine);
                painter->setPen(outline);
                QRect r = option->rect.adjusted(2, 2, -2, -2);
                painter->drawRect(r);
                painter->setPen(oldPen);
            }

            QStyleOptionToolButton label = *toolbutton;
            label.palette = panelPalette(option->palette);
            int fw = pixelMetric(PM_DefaultFrameWidth, option, widget);
            label.rect = button.adjusted(fw, fw, -fw, -fw);
            drawControl(CE_ToolButtonLabel, &label, painter, widget);

            if (toolbutton->subControls & SC_ToolButtonMenu) {
                tool.state = mflags;
                tool.rect = menuarea.adjusted(1, 1, -1, -1);
                if (mflags & (State_Sunken | State_On | State_Raised)) {
                    painter->setPen(Qt::gray);
                    painter->drawLine(tool.rect.topLeft(), tool.rect.bottomLeft());
                    if (mflags & (State_Sunken)) {
                        QColor shade(0, 0, 0, 50);
                        painter->fillRect(tool.rect.adjusted(0, -1, 1, 1), shade);
                    }
#ifndef Q_WS_MAC
                    else if (mflags & (State_MouseOver)) {
                        QColor shade(255, 255, 255, 50);
                        painter->fillRect(tool.rect.adjusted(0, -1, 1, 1), shade);
                    }
#endif
                }
                tool.rect = tool.rect.adjusted(2, 2, -2, -2);
                drawPrimitive(PE_IndicatorArrowDown, &tool, painter, widget);
            } else if (toolbutton->features & QStyleOptionToolButton::HasMenu) {
                int arrowSize = 6;
                QRect ir = toolbutton->rect.adjusted(1, 1, -1, -1);
                QStyleOptionToolButton newBtn = *toolbutton;
                newBtn.palette = panelPalette(option->palette);
                newBtn.rect = QRect(ir.right() - arrowSize - 1,
                                    ir.height() - arrowSize - 2, arrowSize, arrowSize);
                drawPrimitive(PE_IndicatorArrowDown, &newBtn, painter, widget);
            }
        }
        break;

    case CC_ComboBox:
        if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            painter->save();
            bool isEmpty = cb->currentText.isEmpty() && cb->currentIcon.isNull();
            bool reverse = option->direction == Qt::RightToLeft;

            // Draw tool button
            QLinearGradient grad(option->rect.topRight(), option->rect.bottomRight());
            grad.setColorAt(0, QColor(255, 255, 255, 20));
            grad.setColorAt(0.4, QColor(255, 255, 255, 60));
            grad.setColorAt(0.7, QColor(255, 255, 255, 50));
            grad.setColorAt(1, QColor(255, 255, 255, 40));
            painter->setPen(QPen(grad, 0));
            painter->drawLine(rect.topRight(), rect.bottomRight());
            grad.setColorAt(0, QColor(0, 0, 0, 30));
            grad.setColorAt(0.4, QColor(0, 0, 0, 70));
            grad.setColorAt(0.7, QColor(0, 0, 0, 70));
            grad.setColorAt(1, QColor(0, 0, 0, 40));
            painter->setPen(QPen(grad, 0));
            if (!reverse)
                painter->drawLine(rect.topRight() - QPoint(1,0), rect.bottomRight() - QPoint(1,0));
            else
                painter->drawLine(rect.topLeft(), rect.bottomLeft());
            QStyleOption toolbutton = *option;
            if (isEmpty)
                toolbutton.state &= ~(State_Enabled | State_Sunken);
            painter->save();
            painter->setClipRect(toolbutton.rect.adjusted(0, 0, -2, 0));
            drawPrimitive(PE_PanelButtonTool, &toolbutton, painter, widget);
            painter->restore();
            // Draw arrow
            int menuButtonWidth = 12;
            int left = !reverse ? rect.right() - menuButtonWidth : rect.left();
            int right = !reverse ? rect.right() : rect.left() + menuButtonWidth;
            QRect arrowRect((left + right) / 2 + (reverse ? 6 : -6), rect.center().y() - 3, 9, 9);
            if (option->state & State_On)
                arrowRect.translate(d->style->pixelMetric(PM_ButtonShiftHorizontal, option, widget),
                                    d->style->pixelMetric(PM_ButtonShiftVertical, option, widget));

            QStyleOption arrowOpt = *option;
            arrowOpt.rect = arrowRect;
            if (isEmpty)
                arrowOpt.state &= ~(State_Enabled | State_Sunken);

            if (styleHint(SH_ComboBox_Popup, option, widget)) {
                arrowOpt.rect.translate(0, -3);
                drawPrimitive(PE_IndicatorArrowUp, &arrowOpt, painter, widget);
                arrowOpt.rect.translate(0, 6);
                drawPrimitive(PE_IndicatorArrowDown, &arrowOpt, painter, widget);
            } else {
                drawPrimitive(PE_IndicatorArrowDown, &arrowOpt, painter, widget);
            }
            painter->restore();
        }
        break;
    default:
        d->style->drawComplexControl(control, option, painter, widget);
        break;
    }
}

// Mac style reimplements this to control the
// focus widget among other things
bool GManhattanStyle::event(QEvent *e)
{
    Q_ASSERT(d->style);
    return d->style->event(e);
}

}
