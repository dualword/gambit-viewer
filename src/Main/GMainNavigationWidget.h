// ***************************************************************************
// GMainNavigationWidget.h (c) 2009 Derek Barnett
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

#ifndef G_MAINNAVIGATIONWIDGET_H
#define G_MAINNAVIGATIONWIDGET_H

#include <QTabBar>
#include <QTimeLine>
class QStatusBar;

namespace Gambit {

class GMainNavigationBar : public QTabBar {

    Q_OBJECT

    public:
        GMainNavigationBar(QWidget* parent = 0);
        ~GMainNavigationBar(void);

    public:
        QSize tabSizeHint(int index) const;
        void paintEvent(QPaintEvent* event);
        void paintTab(QPainter* painter, int tabIndex) const;
        void mouseMoveEvent(QMouseEvent* e);
        void enterEvent(QEvent* e);
        void leaveEvent(QEvent* e);
        void tabInserted(int index);
        void tabRemoved(int index);

    public slots:
        void updateHover(void);

    private:
        static const int m_rounding;
        static const int m_textPadding;
        QTimeLine m_hoverControl;
        QRect m_hoverRect;
};

class GMainNavigationWidget : public QWidget {

    Q_OBJECT

    public:
        enum Mode { Home = 0, Viewer, Tools, Web, Help };

    public:
        GMainNavigationWidget(QWidget* parent = 0);
        ~GMainNavigationWidget(void);

    public:
        void insertTab(int index, QWidget* tab, const QIcon& icon, const QString& label);
        void removeTab(int index);
        void setBackgroundBrush(const QBrush& brush);
        void addCornerWidget(QWidget* widget);
        void insertCornerWidget(int pos, QWidget* widget);
        int cornerWidgetCount(void) const;
        void setTabToolTip(int index, const QString& toolTip);
        void paintEvent(QPaintEvent* event);
        int currentIndex(void) const;
        QStatusBar* statusBar(void) const;

    signals:
        void currentAboutToShow(int index);
        void currentChanged(int index);

    public slots:
        void setCurrentIndex(int index);

    private slots:
        void showWidget(int index);

    private:
        struct GMainNavigationWidgetPrivate;
        GMainNavigationWidgetPrivate* d;
};

} // namespace Gambit

#endif // G_MAINNAVIGATIONWIDGET_H
