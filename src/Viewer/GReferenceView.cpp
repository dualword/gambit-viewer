// ***************************************************************************
// GReferenceView.cpp (c) 2009 Derek Barnett
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
// Visually displays all references loaded from files.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/GReferenceView.h"
#include "Viewer/GVisibleReferenceItem.h"
using namespace Gambit;
using namespace Gambit::Viewer;

struct GReferenceView::GReferenceViewPrivate {

    GReferenceView* view;
    QGraphicsScene* scene;

    void Clear(void);
    void Draw(const GReferenceList& references);
    void Init(GReferenceView* parentView);
};

void GReferenceView::GReferenceViewPrivate::Clear(void) {
    QList<QGraphicsItem*> items = scene->items();
    while(!items.isEmpty()) {
        QGraphicsItem* item = items.takeFirst();
        if ( item ) {
            scene->removeItem(item);
            delete item;
            item = 0;
        }
    }
}

void GReferenceView::GReferenceViewPrivate::Draw(const GReferenceList& references) {

    // remove any previous graphics
    Clear();

    // skip if no references to draw
    if ( references.isEmpty() ) { return; }

    // general drawing area attributes
    qreal xMargin    = 10;
    qreal yMargin    = 10;
    qreal itemHeight = 10;
    qreal spacer     = 25;

    // individual item positions
    qreal xPos = xMargin;
    qreal yPos = yMargin;

    // iterate over references
    foreach (GReference ref, references) {

        // draw reference rectangle
        qreal visibleRefLength = 150 - (3*xMargin);

        GVisibleReferenceItem* refItem = new GVisibleReferenceItem(ref, visibleRefLength);
        refItem->setPos(xPos, yPos);

        // connect to item's clicked signal
        connect(refItem, SIGNAL(clicked(const GReference&)), view, SIGNAL(ReferenceClicked(const GReference&)));

        // add items to scene
        scene->addItem(refItem);

        // update yPos for next reference item
        yPos += (itemHeight + spacer);
    }

    // update scene
    scene->update();
}

void GReferenceView::GReferenceViewPrivate::Init(GReferenceView* parentView) {
    view  = parentView;
    scene = new QGraphicsScene;
}

GReferenceView::GReferenceView(QWidget* parent)
    : QGraphicsView(parent)
{
    d = new GReferenceViewPrivate;
    d->Init(this);

    // set up scene
    setAlignment(Qt::AlignTop | Qt::AlignLeft );
    setFixedWidth(150);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setScene(d->scene);

    // set background color
    QPalette p = palette();
    p.setBrush(QPalette::Base, QBrush(QColor(Qt::black)));
    setPalette(p);
}

GReferenceView::~GReferenceView(void) {
    delete d;
    d = 0;
}

void GReferenceView::Clear(void) {
    d->Clear();
}

void GReferenceView::SetReferences(const GReferenceList& references) {
    d->Draw(references);
}
