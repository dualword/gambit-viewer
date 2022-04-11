// ***************************************************************************
// GSliderView.cpp (c) 2009 Derek Barnett
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
// Provides reference 'slider'. Displays full view of current reference.
// Draws a rectangle around currently displayed region.  Catches user
// interaction to provide for jumping to new position in reference.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/GSliderView.h"
#include "DataStructures/GGenomicDataRegion.h"
#include "DataStructures/GReference.h"
using namespace Gambit;
using namespace Gambit::Viewer;

namespace Gambit {
namespace Viewer {

class CoordinatesItem : public QGraphicsRectItem {

    public:
        CoordinatesItem(QGraphicsItem* parent = 0)
            : QGraphicsRectItem(parent)
            , m_leftBound(0)
            , m_rightBound(0)
            , m_majorInterval(5)
            , m_minorInterval(1)
            , m_basesPerScenePos(1.0)
        { }

        void CalculateIntervals(void) {

            // reset intervals
            m_majorInterval = 5;
            m_minorInterval = 1;

            // set up possible intervals for this range
            QList<qint32> intervals;
            for (qint32 interval = 1; interval <= m_rightBound; interval *= 10) {
                intervals << interval << (interval*5);
            }

            // determine "best-fit" intervals for this range
            // where major interval is lowest possible value to provide <= 10 'chunks' across the given range
            // minor interval is the interval preceding it in the list (if major == 50 then minor = 10, if major == 100 then minor = 50, etc)
            qint32 coordinateRange = m_rightBound - m_leftBound;
            for (int index = 1; index < intervals.size(); ++index) {
                qint32 interval = intervals.at(index);
                if ( coordinateRange/interval <= 10 ) {
                    m_majorInterval = interval;
                    m_minorInterval = intervals.at(index - 1);
                    break;
                }
            }
        }

        void SetScaleFactor(const qreal& factor) {
            m_basesPerScenePos = factor;
        }

        void SetRange(const qint32& left, const qint32& right) {
            m_leftBound = left;
            m_rightBound = right;
            CalculateIntervals();
        }

        QRectF boundingRect(void) const {
            return QGraphicsRectItem::boundingRect();
        }

        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

            Q_UNUSED(option);
            Q_UNUSED(widget);

            // set painter attributes
            painter->setBrush(QBrush(Qt::white));
            painter->setPen(QPen(Qt::white));

            // draw labels and store tick marks (will draw all in one go later)
            QVector<QLineF> tickMarks;
            for (qint32 genomicPosition = m_minorInterval; genomicPosition <= m_rightBound; genomicPosition += m_minorInterval) {

                // convert genomic position to screen position
                qreal screenPos = genomicPosition / m_basesPerScenePos;

                // if major interval position
                if ( (genomicPosition%m_majorInterval) == 0 ) {

                    // draw text label
                    QPointF textPosition(screenPos, -10);
                    QString text( QString::number(genomicPosition) );
                    painter->drawText(textPosition, text);

                    // store tick mark
                    tickMarks << QLineF(screenPos, 0, screenPos, 20);

                }

                // otherwise, store tick mark for minor interval position
                else {
                    tickMarks << QLineF(screenPos, 12, screenPos, 20);
                }

            }

            // draw tick marks
            painter->drawLines(tickMarks);
        }

    private:
        qint32 m_leftBound;
        qint32 m_rightBound;
        qint32 m_majorInterval;
        qint32 m_minorInterval;
        qreal  m_basesPerScenePos;
};

class ReferenceItem : public QGraphicsRectItem {

    public:
        ReferenceItem(QGraphicsItem* parent = 0)
            : QGraphicsRectItem(parent)
        {
            QColor backgroundColor = QColor("steelblue");
            m_gradient.setSpread(QGradient::ReflectSpread);
            m_gradient.setColorAt(0.0, backgroundColor.lighter() );
            m_gradient.setColorAt(0.4, backgroundColor.lighter() );
            m_gradient.setColorAt(0.6, backgroundColor);
            m_gradient.setColorAt(1.0, QColor(Qt::black).lighter(150) );
        }

        QRectF boundingRect(void) const {
            return QGraphicsRectItem::boundingRect();
        }

        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

            Q_UNUSED(option);
            Q_UNUSED(widget);

            if ( scene() == 0 ) { return; }

            // set up rectangle background brush & pen
            QRectF rect(0,0,scene()->sceneRect().width(), 20);
            m_gradient.setStart(rect.topLeft());
            m_gradient.setFinalStop(rect.bottomLeft());
            painter->setBrush( QBrush(m_gradient) );
            painter->setPen(Qt::black);

            // draw rectangle
            painter->drawRect(rect);
        }

        private:
            QLinearGradient m_gradient;
};

class SliderItem : public QGraphicsItem {

    public:
        SliderItem(QGraphicsItem* parent = 0)
            : QGraphicsItem(parent)
            , m_brush(Qt::yellow)
        {
            QColor color = m_brush.color();
            color.setAlpha(100);
            m_brush.setColor(color);
            setZValue(5);
        }

        QRectF boundingRect(void) const {
            return m_rect;
        }

        void SetRect(const QRectF& rect) {
            m_rect = rect;
        }

        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

            Q_UNUSED(option);
            Q_UNUSED(widget);

            if ( scene() == 0 ) { return; }

            // set up rectangle background brush
            painter->setBrush(m_brush);
            painter->setPen(Qt::black);

            // draw rectangle
            painter->drawRect(m_rect);
        }
    private:
        QBrush m_brush;
        QRectF m_rect;
};

} // namespace Viewer
} // namespace Gambit

struct GSliderView::GSliderViewPrivate {

    QGraphicsScene*  scene;
    CoordinatesItem* coordinatesItem;
    ReferenceItem*   referenceItem;
    SliderItem*      sliderItem;

    GReference reference;
    qint32     sliderLeftBound;
    qint32     sliderRightBound;
    qreal      basesPerScenePos;

    enum MouseState { Pressed, Released };
    MouseState mouseState;

    void Init(void);
    void MoveSlider(const QPoint& mousePos);
    void UpdateScaleFactor(void);
    void UpdateSlider(void);
};

void GSliderView::GSliderViewPrivate::Init(void) {

    // initialize scene
    scene = new QGraphicsScene;

    // create default items
    coordinatesItem = new CoordinatesItem;
    referenceItem   = new ReferenceItem;
    sliderItem      = new SliderItem;

    // hide items intially
    referenceItem->setVisible(false);
    sliderItem->setVisible(false);

    // set item positions
    coordinatesItem->setPos(0, -20);
    referenceItem->setPos(0, 0);
    sliderItem->setPos(0,-5);

    // add items to scene
    scene->addItem(coordinatesItem);
    scene->addItem(referenceItem);
    scene->addItem(sliderItem);

    // initialize coordinates, scale factor, and mouse state
    sliderLeftBound  = 0;
    sliderRightBound = 0;
    basesPerScenePos = 1.0;
    mouseState       = GSliderView::GSliderViewPrivate::Released;
}

void GSliderView::GSliderViewPrivate::MoveSlider(const QPoint& mousePos) {

    const qint32 sliderRange = sliderRightBound - sliderLeftBound;
    const qint32 mouseClickCoordinate = (qint32)(mousePos.x() * basesPerScenePos);

    // if range overlaps zero
    if ( mouseClickCoordinate < (sliderRange/2) ) {
        sliderLeftBound  = 0;
        sliderRightBound = sliderRange;
    }

    // if range extends beyond reference
    else if ( (mouseClickCoordinate + (sliderRange/2)) > reference.Length  ) {
        sliderLeftBound  = reference.Length - sliderRange;
        sliderRightBound = reference.Length;
    }

    // otherwise, 'normal' case
    else {
        sliderLeftBound  = mouseClickCoordinate - (sliderRange/2);
        sliderRightBound = mouseClickCoordinate + (sliderRange/2);
    }

    // update components
    UpdateSlider();
    referenceItem->update();
}

void GSliderView::GSliderViewPrivate::UpdateSlider(void) {
    qint32 width = (sliderRightBound - sliderLeftBound + 1) / basesPerScenePos;
    QRectF sliderRect = QRect(0,0, width, 30);
    sliderItem->SetRect(sliderRect);
    sliderItem->setPos(sliderLeftBound/basesPerScenePos, -5);
    sliderItem->update();
}

void GSliderView::GSliderViewPrivate::UpdateScaleFactor(void) {
    if ( reference.Length == 0 ) {
        basesPerScenePos = 1.0;
    } else {
        basesPerScenePos = (reference.Length / scene->width() );
    }

    coordinatesItem->SetScaleFactor(basesPerScenePos);
    coordinatesItem->update();
}

GSliderView::GSliderView(QWidget* parent)
    : QGraphicsView(parent)
{
    d = new GSliderViewPrivate;
    d->Init();
    setScene(d->scene);

    QPalette p = palette();
    p.setBrush(QPalette::Base, QBrush(QColor(Qt::black)));
    setPalette(p);

    setAlignment(Qt::AlignLeft);
    setEnabled(true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

GSliderView::~GSliderView(void) {
    delete d;
    d = 0;
}

void GSliderView::resizeEvent(QResizeEvent* event) {

    // expand QGraphicsScene to fill view
    int width = event->size().width();
    QRectF sceneRect = d->scene->sceneRect();
    sceneRect.setWidth(width);
    d->scene->setSceneRect(sceneRect);

    // update scene scaling factor
    d->UpdateScaleFactor();

    // update items
    d->coordinatesItem->update();
    d->UpdateSlider();
    d->referenceItem->update();
    update();

    // pass resize event along
    QGraphicsView::resizeEvent(event);
}

void GSliderView::mousePressEvent(QMouseEvent* event) {
    if ( event->button() != Qt::LeftButton ) { event->ignore(); }
    d->mouseState = GSliderView::GSliderViewPrivate::Pressed;
    d->MoveSlider(event->pos());
    update();
    GGenomicDataRegion region(d->reference.Name, d->sliderLeftBound, d->sliderRightBound);
    emit RegionRequested(region);
}

void GSliderView::mouseMoveEvent(QMouseEvent* event) {
    if ( event->button() != Qt::LeftButton ) { event->ignore(); }
    if ( d->mouseState == GSliderView::GSliderViewPrivate::Pressed ) {
        d->MoveSlider(event->pos());
        update();
    }
}

void GSliderView::mouseReleaseEvent(QMouseEvent* event) {
    if ( event->button() != Qt::LeftButton ) { event->ignore(); }
    d->mouseState = GSliderView::GSliderViewPrivate::Released;
    d->MoveSlider(event->pos());
    update();
    GGenomicDataRegion region(d->reference.Name, d->sliderLeftBound, d->sliderRightBound);
    emit RegionRequested(region);
}

void GSliderView::SetSelectedRegion(const GGenomicDataRegion& region) {
    d->sliderLeftBound  = region.LeftBound;
    d->sliderRightBound = region.RightBound;
    d->UpdateSlider();
    d->sliderItem->setVisible(true);
    update();
}

void GSliderView::SetReference(const GReference& reference) {
    d->reference = reference;
    d->referenceItem->setVisible(true);
    d->referenceItem->update();
    d->coordinatesItem->SetRange(1, reference.Length);
    d->UpdateScaleFactor();
    update();
}
