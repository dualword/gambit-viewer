// ***************************************************************************
// GMainNavigationWidget.cpp (c) 2009 Derek Barnett
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
// Describes the main left-hand naviagation tab bar.
//
// (Code borrows heavily from Qt Creator v1.1 source (fancytabwidget.h/.cpp),
// distributed under LGPL v2.1.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Main/GMainNavigationWidget.h"
#include "Utilities/GStyleHelper.h"
using namespace Gambit;

const int GMainNavigationBar::m_rounding    = 22;
const int GMainNavigationBar::m_textPadding = 4;

struct GMainNavigationWidget::GMainNavigationWidgetPrivate {
    GMainNavigationBar* tabBar;
    QWidget*            cornerWidgetContainer;
    QStackedLayout*     modesStack;
    QWidget*            selectionWidget;
    QStatusBar*         statusBar;
};

GMainNavigationBar::GMainNavigationBar(QWidget* parent)
    : QTabBar(parent)
{
    setStyle(new QWindowsStyle);
    setDrawBase(false);
    setElideMode(Qt::ElideNone);
    setMinimumWidth(qMax(2 * m_rounding, 40));
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::NoFocus);
    m_hoverControl.setFrameRange(0, 20);
    m_hoverControl.setDuration(130);
    m_hoverControl.setCurveShape(QTimeLine::EaseInCurve);
    connect(&m_hoverControl, SIGNAL(frameChanged(int)), this, SLOT(updateHover()));
    setMouseTracking(true); // Needed for hover events
    setExpanding(false);
}

GMainNavigationBar::~GMainNavigationBar(void) {
    delete style();
}

QSize GMainNavigationBar::tabSizeHint(int index) const {
    QFont boldFont(font());
    boldFont.setPointSizeF(GStyleHelper::sidebarFontSize());
    boldFont.setBold(true);
    QFontMetrics fm(boldFont);
    int spacing = 6;
    int width = 60 + spacing + 2;
    return QSize(width, tabIcon(index).actualSize(QSize(64, 64)).height() + spacing + fm.height());
}

void GMainNavigationBar::paintEvent(QPaintEvent* event) {

    Q_UNUSED(event);

    QPainter p(this);
    for (int i = 0; i < count(); ++i){
        if (i != currentIndex()) {
            paintTab(&p, i);
        }
    }

    // paint active tab last, since it overlaps the neighbors
    paintTab(&p, currentIndex());
}

// Handle hover events for mouse fade ins
void GMainNavigationBar::mouseMoveEvent(QMouseEvent* e) {

    if (!m_hoverRect.contains(e->pos())) {
        for (int i = 0; i < count(); ++i) {
            QRect area = tabRect(i);
            if (area.contains(e->pos())) {
                QRect oldHoverRect = m_hoverRect;
                m_hoverRect = area;
                update(oldHoverRect);
                m_hoverControl.stop();
                m_hoverControl.start();
                break;
            }
        }
    }
}

void GMainNavigationBar::updateHover(void) {
    update(m_hoverRect);
}

// Resets hover animation on mouse enter
void GMainNavigationBar::enterEvent(QEvent* e) {
    Q_UNUSED(e);
    m_hoverRect = QRect();
}

// Resets hover animation on mouse enter
void GMainNavigationBar::leaveEvent(QEvent *e) {
    Q_UNUSED(e);
    m_hoverControl.stop();
    m_hoverControl.start();
}

void GMainNavigationBar::paintTab(QPainter* painter, int tabIndex) const {

    QStyleOptionTabV2 tab;
    initStyleOption(&tab, tabIndex);
    QRect rect = tab.rect;
    painter->save();

    bool selected = tab.state & QStyle::State_Selected;
    bool hover    = tab.state & QStyle::State_MouseOver;

#ifdef Q_WS_MAC
    hover = false; // Dont hover on Mac
#endif

    QColor background = QColor(0, 0, 0, 10);
    QColor hoverColor;

    if (hover) {
        hoverColor = QColor(255, 255, 255, m_hoverControl.currentFrame());
    }

    QColor light = QColor(255, 255, 255, 40);
    QColor dark = QColor(0, 0, 0, 60);

    if (selected) {
        QLinearGradient selectedGradient(rect.topLeft(), QPoint(rect.center().x(), rect.bottom()));
        selectedGradient.setColorAt(0, Qt::white);
        selectedGradient.setColorAt(0.3, Qt::white);
        selectedGradient.setColorAt(0.7, QColor(230, 230, 230));

        painter->fillRect(rect, selectedGradient);
        painter->setPen(QColor(200, 200, 200));
        painter->drawLine(rect.topLeft(), rect.topRight());
        painter->setPen(QColor(150, 160, 200));
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    } else {
        painter->fillRect(tab.rect, background);
        if (hover) {
            painter->fillRect(tab.rect, hoverColor);
        }
        painter->setPen(QPen(light, 0));
        painter->drawLine(rect.topLeft(), rect.topRight());
        painter->setPen(QPen(dark, 0));
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    }

    QString tabText(tab.text);
    QRect tabTextRect(tab.rect);
    QRect tabIconRect(tab.rect);
    QFont boldFont(painter->font());
    boldFont.setPointSizeF(GStyleHelper::sidebarFontSize());
    boldFont.setBold(true);
    painter->setFont(boldFont);
    painter->setPen(selected ? GStyleHelper::panelTextColor() : QColor(30, 30, 30, 80));
    int textFlags = Qt::AlignCenter | Qt::AlignBottom | Qt::ElideRight | Qt::TextWordWrap;
    painter->drawText(tabTextRect, textFlags, tabText);
    painter->setPen(selected ? QColor(60, 60, 60) : GStyleHelper::panelTextColor());
    int textHeight = painter->fontMetrics().boundingRect(QRect(0, 0, width(), height()), Qt::TextWordWrap, tabText).height();
    tabIconRect.adjust(0, 4, 0, -textHeight);
    style()->drawItemPixmap(painter, tabIconRect, Qt::AlignCenter | Qt::AlignVCenter,
                            tab.icon.pixmap(QSize(64, 64)));
    painter->translate(0, -1);
    painter->drawText(tabTextRect, textFlags, tabText);
    painter->restore();
}

void GMainNavigationBar::tabInserted(int index) {
    Q_UNUSED(index);
}

void GMainNavigationBar::tabRemoved(int index) {
    Q_UNUSED(index);
}

//////
// FancyColorButton
//////

class FancyColorButton : public QWidget {
    public:
        FancyColorButton(QWidget* parent)
          : m_parent(parent)
        {
            setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        }

        void mousePressEvent(QMouseEvent* ev) {
            if (ev->modifiers() & Qt::ShiftModifier)
                GStyleHelper::setBaseColor(QColorDialog::getColor(GStyleHelper::baseColor(), m_parent));
        }

    private:
        QWidget *m_parent;
};

//////
// GMainNavigationWidget
//////

GMainNavigationWidget::GMainNavigationWidget(QWidget* parent)
    : QWidget(parent)
{
    d = new GMainNavigationWidgetPrivate;

    d->tabBar = new GMainNavigationBar(this);
    d->tabBar->setShape(QTabBar::RoundedEast);
    d->tabBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    d->tabBar->setUsesScrollButtons(false);

    d->selectionWidget = new QWidget(this);
    QVBoxLayout* selectionLayout = new QVBoxLayout;
    selectionLayout->setSpacing(0);
    selectionLayout->setMargin(0);

    QToolBar* bar = new QToolBar;
    bar->addWidget(new FancyColorButton(this));
    bar->setFixedHeight(GStyleHelper::navigationWidgetHeight());
    selectionLayout->addWidget(bar);

    selectionLayout->addWidget(d->tabBar, 1);
    d->selectionWidget->setLayout(selectionLayout);
    d->selectionWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);

    d->cornerWidgetContainer = new QWidget(this);
    d->cornerWidgetContainer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    d->cornerWidgetContainer->setAutoFillBackground(false);

    QVBoxLayout* cornerWidgetLayout = new QVBoxLayout;
    cornerWidgetLayout->setSpacing(0);
    cornerWidgetLayout->setMargin(0);
    cornerWidgetLayout->addStretch();
    d->cornerWidgetContainer->setLayout(cornerWidgetLayout);

    selectionLayout->addWidget(d->cornerWidgetContainer, 0);

    d->modesStack = new QStackedLayout;
    d->statusBar = new QStatusBar;
    d->statusBar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

    QVBoxLayout* vlayout = new QVBoxLayout;
    vlayout->setMargin(0);
    vlayout->setSpacing(0);
    vlayout->addLayout(d->modesStack);
    vlayout->addWidget(d->statusBar);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(1);
    layout->addWidget(d->selectionWidget);
    layout->addLayout(vlayout);
    setLayout(layout);

    connect(d->tabBar, SIGNAL(currentChanged(int)), this, SLOT(showWidget(int)));
}

GMainNavigationWidget::~GMainNavigationWidget(void) {
    if ( d ) {
        delete d;
        d = NULL;
    }
}

void GMainNavigationWidget::insertTab(int index, QWidget* tab, const QIcon& icon, const QString& label) {
    d->modesStack->insertWidget(index, tab);
    d->tabBar->insertTab(index, icon, label);
}

void GMainNavigationWidget::removeTab(int index) {
    d->modesStack->removeWidget(d->modesStack->widget(index));
    d->tabBar->removeTab(index);
}

void GMainNavigationWidget::setBackgroundBrush(const QBrush& brush) {

    QPalette pal = d->tabBar->palette();
    pal.setBrush(QPalette::Mid, brush);
    d->tabBar->setPalette(pal);

    pal = d->cornerWidgetContainer->palette();
    pal.setBrush(QPalette::Mid, brush);
    d->cornerWidgetContainer->setPalette(pal);
}

void GMainNavigationWidget::paintEvent(QPaintEvent* event) {

    Q_UNUSED(event);

    QPainter p(this);
    QRect rect = d->selectionWidget->rect().adjusted(0, 0, 1, 0);
    rect = style()->visualRect(layoutDirection(), geometry(), rect);
    GStyleHelper::verticalGradient(&p, rect, rect);
    p.setPen(GStyleHelper::borderColor());
    p.drawLine(rect.topRight(), rect.bottomRight());
}

void GMainNavigationWidget::insertCornerWidget(int pos, QWidget* widget) {
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(d->cornerWidgetContainer->layout());
    layout->insertWidget(pos, widget);
}

int GMainNavigationWidget::cornerWidgetCount(void) const {
    return d->cornerWidgetContainer->layout()->count();
}

void GMainNavigationWidget::addCornerWidget(QWidget* widget) {
    d->cornerWidgetContainer->layout()->addWidget(widget);
}

int GMainNavigationWidget::currentIndex(void) const {
    return d->tabBar->currentIndex();
}

QStatusBar* GMainNavigationWidget::statusBar(void) const {
    return d->statusBar;
}

void GMainNavigationWidget::setCurrentIndex(int index) {
    d->tabBar->setCurrentIndex(index);
}

void GMainNavigationWidget::showWidget(int index) {
    emit currentAboutToShow(index);
    d->modesStack->setCurrentIndex(index);
    emit currentChanged(index);
}

void GMainNavigationWidget::setTabToolTip(int index, const QString& toolTip) {
    d->tabBar->setTabToolTip(index, toolTip);
}
