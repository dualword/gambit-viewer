// ***************************************************************************
// GVisibleAlignmentGroup.cpp (c) 2009 Derek Barnett
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

#include <QtGui>
#include <QtDebug>
#include "Viewer/AssemblyView/GVisibleAlignmentGroup.h"
#include "Viewer/AssemblyView/GVisibleAlignmentGroupHeader.h"
#include "Viewer/AssemblyView/GVisibleAlignmentItem.h"
#include "DataStructures/GColorScheme.h"
using namespace Gambit;
using namespace Gambit::Viewer;

struct GVisibleAlignmentGroup::GVisibleAlignmentGroupPrivate {

    QString      readGroup;
    GColorScheme colorScheme;
    QFont        font;
    int          fontHeight;
    int          fontWidth;
    qint32       leftBound;

    GVisibleAlignmentGroupHeader* headerWidget;
    QGraphicsProxyWidget*         headerProxy;
    QList<GVisibleAlignmentItem*> alignmentItems;

    bool isModified;
    bool isBasesVisible;

    const qreal ALIGNMENT_HEIGHT;
    const qreal SPACER;
    const qreal VIEW_MARGIN;

    // constructor
    GVisibleAlignmentGroupPrivate(const QString&      some_readGroup,
                                  const GColorScheme& some_colorScheme,
                                  const QFont&        some_font,
                                  const int&          some_fontHeight,
                                  const int&          some_fontWidth,
                                  const qint32&       some_leftBound)
        : readGroup(some_readGroup)
        , colorScheme(some_colorScheme)
        , font(some_font)
        , fontHeight(some_fontHeight)
        , fontWidth(some_fontWidth)
        , leftBound(some_leftBound)
        , isModified(false)
        , isBasesVisible(false)
        , ALIGNMENT_HEIGHT(15)
        , SPACER(2)
        , VIEW_MARGIN(5)
    { }

    // internal methods

};

GVisibleAlignmentGroup::GVisibleAlignmentGroup(const QString& readGroup,
                                               const GColorScheme& colorScheme,
                                               const QFont&   font,
                                               const int&     fontHeight,
                                               const int&     fontWidth,
                                               const qint32&  leftBound,
                                               QGraphicsItem* parent)
    : QGraphicsItemGroup(parent)
{   
    d = new GVisibleAlignmentGroupPrivate(readGroup, colorScheme, font, fontHeight, fontWidth, leftBound);

    // group header widget and alignment items handle their own mouse events
    setZValue(-10);
    setHandlesChildEvents(false);
    setVisible(false);
}

GVisibleAlignmentGroup::~GVisibleAlignmentGroup(void) {
    Clear();
    delete d;
    d = 0;
}

QRectF GVisibleAlignmentGroup::boundingRect(void) const {

    // initialize bottom right coordinates with header sizes
    qreal bottomRight_X = 0;
    qreal bottomRight_Y = 0;

    if ( d->headerProxy ) {
        bottomRight_X = d->headerProxy->size().width();
        bottomRight_Y = d->headerProxy->size().height();
    }

    foreach (GVisibleAlignmentItem* gvaItem, d->alignmentItems) {

        // skip invalid or non-visible items
        if ( (gvaItem == 0) || !gvaItem->isVisible() ) { continue; }

        // retrieve basic item coordinate values
        QRectF itemBoundingRect = gvaItem->boundingRect();
        QPointF itemPos = gvaItem->pos();

        // if item exceeds (so-far) group max width, update max width
        qreal item_X = itemPos.x() + itemBoundingRect.width();
        if ( item_X > bottomRight_X) { bottomRight_X = item_X; }

        // if item exceeds (so-far) group max height, update max height
        qreal item_Y = itemPos.y() + itemBoundingRect.height();
        if ( item_Y > bottomRight_Y) { bottomRight_Y = item_Y; }
    }

    return QRectF(0, 0, bottomRight_X, bottomRight_Y);
}

QString GVisibleAlignmentGroup::Label(void) {
    return d->readGroup;
}

void GVisibleAlignmentGroup::SetReadGroup(QString& readGroup) {
    if ( d->headerProxy && d->headerWidget ) {
        d->headerWidget->SetReadGroup(readGroup);
    }
}

void GVisibleAlignmentGroup::MoveHeaderToPosition(int value, qreal horizontalScaleFactor) {
    if ( d->headerProxy == 0 ) { return; }
    prepareGeometryChange();
    d->headerProxy->setPos(value/horizontalScaleFactor, 0);
}

void GVisibleAlignmentGroup::AddAlignment(GAlignment& alignment) {
    GVisibleAlignmentItem* item = new GVisibleAlignmentItem(alignment, d->isBasesVisible, d->font, d->fontHeight, d->fontWidth);
    item->SetColorScheme(d->colorScheme);
    AddAlignmentItem(item);
}

void GVisibleAlignmentGroup::AddAlignments(GAlignmentList alignments) {
    foreach (GAlignment alignment, alignments) {
        AddAlignment(alignment);
    }
}

void GVisibleAlignmentGroup::AddAlignmentItem(GVisibleAlignmentItem* item) {
    if ( item == 0 ) { return; }
    addToGroup(item);
    d->alignmentItems.append(item);
    d->isModified = true;
}



void GVisibleAlignmentGroup::Clear(void) {

    // remove all alignments from group, delete
    foreach (GVisibleAlignmentItem* item, d->alignmentItems) {
        if ( item == 0 ) { continue; }
        removeFromGroup(item);
        delete item;
        item = 0;
    }

    // clear item list
    d->alignmentItems.clear();

    // delete header proxy widget (and embedded widget)
    if ( d->headerProxy ) {
        delete d->headerProxy;
        d->headerProxy = 0;
    }
}

void GVisibleAlignmentGroup::DrawAlignments(void) {

    // skip if nothing new to draw
    if ( !d->isModified ) { return; }

    // store 'next-available' X coordinate for each row
    QList<qreal> rows;
    rows.append(0);

    // prepare QGraphicsItem for boundingRect() change
    prepareGeometryChange();

    // iterate over all alignment items in group
    foreach(GVisibleAlignmentItem* gvaItem, d->alignmentItems) {

        // skip invalid item
        if ( gvaItem == 0 ) { continue; }

        // skip invalid data for GVAItem
        GAlignment gAlignment = gvaItem->alignment();

        // calculate starting X coordinate for alignment
        qreal alignmentStartX = ( (gAlignment.Position + gAlignment.PadsBefore - d->leftBound) * d->fontWidth) + d->VIEW_MARGIN;

        // calculate row to place alignment
        int useRow = 0;
        for ( ; useRow < rows.size(); ++useRow) {
            if ( alignmentStartX >= rows.at(useRow) ) { break; }
        }

        // if row is beyond currently 'known' rows, create new row
        if ( useRow == rows.size() ) { rows.append(alignmentStartX); }

        // set alignment item position
        QPointF position(alignmentStartX, (useRow*(d->ALIGNMENT_HEIGHT+d->SPACER) + d->headerWidget->size().height()));
        gvaItem->setPos( position );

        // save new end position for row used
        rows[useRow] = alignmentStartX + gvaItem->boundingRect().width() + d->SPACER*d->fontWidth;
    }

    // set state flag
    d->isModified = false;
}

void GVisibleAlignmentGroup::DrawHeader(void) {

    // create group header widget
    d->headerWidget = new GVisibleAlignmentGroupHeader(d->readGroup, d->colorScheme);

    // make header connections
    connect(d->headerWidget, SIGNAL(CollapseRequested()), this, SLOT(Collapse()));
    connect(d->headerWidget, SIGNAL(ExpandRequested()),   this, SLOT(Expand()));

    // embed header widget into QGraphicsProxyWidget
    // allows easier manipulation on the QGraphicsScene
    d->headerProxy = new QGraphicsProxyWidget;
    d->headerProxy->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    d->headerProxy->setWidget(d->headerWidget);
    addToGroup(d->headerProxy);

    // set initial position
    QGraphicsScene* scene = d->headerProxy->scene();
    if ( scene == 0 ) { return; }
    QGraphicsView* view = scene->views().first();
    if ( view == 0 ) { return; }
    QPointF initialPoint = view->mapToScene(0,0);
    d->headerProxy->setPos(initialPoint);
}

void GVisibleAlignmentGroup::Draw(void) {
    DrawHeader();
    DrawAlignments();
}

void GVisibleAlignmentGroup::Expand(void) {

    // alert the QGraphicsItemGroup that its boudingRect will change soon (assists QGraphicsScene in indexing)
    prepareGeometryChange();

    // show all alignment items
    foreach (GVisibleAlignmentItem* item, d->alignmentItems) {
        if ( item == 0 ) { continue; }
        item->show();
    }

    // signal change
    emit GroupExpanded();
}

void GVisibleAlignmentGroup::Collapse(void) {

    // alert the QGraphicsItemGroup that its boundingRect will change soon (assists QGraphicsScene in indexing)
    prepareGeometryChange();

    // hide all alignment items
    foreach (GVisibleAlignmentItem* item, d->alignmentItems) {
        if ( item == 0 ) { continue; }
        item->hide();
    }

    // signal change
    emit GroupCollapsed();
}

void GVisibleAlignmentGroup::SetLeftBound(qint32 left) {
    d->leftBound = left;
}

GAlleleList GVisibleAlignmentGroup::AllelesOverlappingPosition(qint32 position) {

    GAlleleList alleles;

    // iterate over alignment items in group
    foreach(GVisibleAlignmentItem* item, d->alignmentItems) {

        // skip if invalid item
        if ( item == 0 ) { continue; }

        // get alignment data from item
        GAlignment alignment = item->alignment();

        // calculate allele, store if valid
        GAllele allele;
        if ( AlleleAt(allele, alignment, position) ) {
            alleles.append(allele);
        }
    }

    return alleles;
}

GAlignmentList GVisibleAlignmentGroup::Alignments(void) {
    GAlignmentList alignments;
    foreach (GVisibleAlignmentItem* item, d->alignmentItems) {
        if ( item == 0 ) { continue; }
        alignments.append( item->alignment() );
    }
    return alignments;
}

const QList<GVisibleAlignmentItem*>& GVisibleAlignmentGroup::AlignmentItems(void) {
    return d->alignmentItems;
}

void GVisibleAlignmentGroup::SetAlignmentItems(const QList<GVisibleAlignmentItem*>& items) {
    d->alignmentItems = items;
    update();
}

void GVisibleAlignmentGroup::SetColorScheme(const GColorScheme& colorScheme) {
    if ( d->headerProxy  == 0 ) { return; }
    if ( d->headerWidget == 0 ) { return; }
    d->colorScheme = colorScheme;
    update();
}

void GVisibleAlignmentGroup::SetBasesVisible(bool ok) {
    d->isBasesVisible = ok;
    foreach (GVisibleAlignmentItem* item, d->alignmentItems) {
        if ( item == NULL ) { continue; }
        item->SetBasesVisible(ok);
    }
}
