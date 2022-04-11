// ***************************************************************************
// GAssemblyView.cpp (c) 2009 Derek Barnett
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
// Describes the main assembly view widget.
// Handles drawing of coordinates, annotations, and alignments, as well as
// responding to various user-directed events and settings modifications.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/AssemblyView/GAssemblyView.h"
#include "DataStructures/GGenomicDataSet.h"
#include "Utilities/flickcharm.h"
#include "Viewer/GSnpInfoDialog.h"
#include "Viewer/AssemblyView/GAssemblySettingsManager.h"
#include "Viewer/AssemblyView/GVisibleAlignmentGroup.h"
#include "Viewer/AssemblyView/GVisibleCursorItem.h"
#include "Viewer/AssemblyView/GVisibleGeneItem.h"
#include "Viewer/AssemblyView/GVisibleSnpItem.h"
using namespace Gambit;
using namespace Gambit::Viewer;

struct GAssemblyView::GAssemblyViewPrivate {

    // store back-pointer to 'parent' public view
    GAssemblyView* view;

    // interface container for all graphics
    QGraphicsScene* scene;

    // internal settings manager
    // handles alignment flag filtering & color scheme selection
    GAssemblySettingsManager* settingsManager;

    // handles 'kinetic scrolling' effect
    FlickCharm flicker;

    // font/text data for all sequence(reference & alignments)
    // also used to size annotations correctly
    QFont font;
    int   fontHeight;
    int   fontWidth;
    int   fontSpacing;

    // range data
    qint32 leftBound;
    qint32 rightBound;

    // display data
    qint32 centerOnPosition;
    bool   isCenterOnSet;
    int    nextAvailableTrackStart;
    qint32 viewMargin;

    // vertical cursor
    GVisibleCursorItem* cursorItem;

    // item to act as parent for all coordinate/annotation track data
    QGraphicsItemGroup*   trackGroup;
    QList<QGraphicsItem*> trackItems;

    // alignment groups
    QMap<QString, GVisibleAlignmentGroup*> groupMap;

    // intializer method
    void Init(void);

    // interface methods
    void ClearCurrentData(void);
    void ShowBases(bool ok);
    void ShowData(const GGenomicDataSet& data);
    void MergeReadGroups(bool ok);
    GAlleleList AllelesOverlappingPosition(qint32 position);
    void AdjustGroupLayout(void);

    private:
        // set up font
        void CreateFont(void);

        // alignment group drawing
        void ShowAlignments(const GAlignmentList& alignments);
        void CreateNewAlignmentGroups(const GAlignmentList& alignments);
        void DrawAlignmentGroups(void);

        // coordinate & annotation track drawing
        void ShowCoordinates(const GPaddingMap& padding);
        void ShowReferenceSequence(const QString& sequence);
        void ShowGenes(const GGeneList& genes);
        void ShowSnps(const GSnpList& snps);
        void UpdateTrackBackground(void);
};

void GAssemblyView::GAssemblyViewPrivate::Init(void) {

    // initialize scene
    scene = new QGraphicsScene;
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // initialize settings manager
    settingsManager = new GAssemblySettingsManager;

    // initialize font data
    fontSpacing = 3;
    CreateFont(); // sets font height/width

    // default data ranges
    leftBound  = 1;
    rightBound = 1;

    // default display data values
    centerOnPosition        = 0;
    isCenterOnSet           = false;
    nextAvailableTrackStart = 0;
    viewMargin              = 5;

    // set up vertical cursor
    cursorItem = new GVisibleCursorItem(fontWidth);
    cursorItem->setPos(0,0);
    scene->addItem(cursorItem);

    // set (NULL) track group
    trackGroup = 0;
}

GAssemblyView::GAssemblyView(QWidget* parent)
    : QGraphicsView(parent)
{
    d = new GAssemblyViewPrivate;
    d->view = this;
    d->Init();
    setScene(d->scene);

    // active FlickCharm for 'kinetic-scrolling'
    d->flicker.activateOn(this);
    connect(&d->flicker, SIGNAL(MouseMoved(QPoint)), this, SLOT(CatchMouseMove(QPoint)));  // *** implement this *** //

    // catch scroll changes
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(CatchHorizontalScrollbarChange(int)));
    connect(verticalScrollBar(),   SIGNAL(valueChanged(int)), this, SLOT(CatchVerticalScrollbarChange(int)));

    // catch signals from settings manager
    connect(d->settingsManager, SIGNAL(MergeGroupsChanged(bool)), this, SLOT(MergeReadGroups(bool)));
    connect(d->settingsManager, SIGNAL(ViewSettingsChanged()),    this, SLOT(ViewSettingsChanged()));
    connect(d->settingsManager, SIGNAL(ShowBasesChanged(bool)),   this, SLOT(ShowBases(bool)));

    // set up view background
    QPalette p = palette();
    p.setBrush(QPalette::Base, QBrush(QColor(Qt::black)));
    setPalette(p);

    // set up attributes
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

GAssemblyView::~GAssemblyView(void) {
    delete d;
    d = 0;
}

QList<QAction*> GAssemblyView::SettingsActions(void) {
    return d->settingsManager->Actions();
}

void GAssemblyView::ClearCurrentData(void)                { d->ClearCurrentData(); }
void GAssemblyView::ShowData(const GGenomicDataSet& data) { d->ShowData(data);     }

void GAssemblyView::SetCenterOn(qint32 position) {
    d->centerOnPosition = position;
    d->isCenterOnSet = true;
    ZoomReset();
}

void GAssemblyView::ZoomIn(void)    {
    scale(1.1, 1.1);
}

void GAssemblyView::ZoomOut(void)   {
    scale(0.9, 0.9);
}

void GAssemblyView::ZoomReset(void) {
    QMatrix m = matrix();
    scale(1.0/m.m11(), 1.0/m.m22());
    if (d->isCenterOnSet) {
        centerOn( qreal((d->centerOnPosition - d->leftBound)*d->fontWidth), 0 );
    }
}

void GAssemblyView::AdjustGroupLayout(void) { d->AdjustGroupLayout(); }

void GAssemblyView::CatchHorizontalScrollbarChange(int value) {
    emit HorizontalScrollChanged( value, matrix().m11() );
}

void GAssemblyView::CatchVerticalScrollbarChange(int value) {

    if ( d->trackGroup ) {
        QPointF trackPos = d->trackGroup->pos();
        qreal yValue = value/matrix().m22();
        d->trackGroup->setPos(trackPos.x(), yValue);
        d->trackGroup->update();
    }
}

void GAssemblyView::MergeReadGroups(bool ok) { d->MergeReadGroups(ok); }
void GAssemblyView::ShowBases(bool ok)       { d->ShowBases(ok); }

// do nothing here for now
void GAssemblyView::CatchAlignmentClick(const GAlignment& alignment) { emit AlignmentClicked(alignment); }
void GAssemblyView::CatchGeneClick(const GGene& gene)                { emit GeneClicked(gene); }

void GAssemblyView::CatchSnpClick(const GSnp& snp) {

    // show SnpInfoDialog
    ShowSnpInfoDialog(snp);

    // pass along signal
    emit SnpClicked(snp);
}

void GAssemblyView::ShowSnpInfoDialog(const GSnp& snp) {

    // retrieve alleles overlapping snp position, skip if none found
    GAlleleList alleles = d->AllelesOverlappingPosition( snp.Position );
    if ( alleles.isEmpty() ) { return; }

    // show dialog
    GSnpInfoDialog dialog(snp, alleles);
    dialog.exec();
}

void GAssemblyView::CatchMouseMove(const QPoint& mousePos) {
    MoveCursor(mousePos);
}

void GAssemblyView::MoveCursor(const QPoint& mousePos) {
    QPointF scenePos = mapToScene(mousePos);
    qreal xPos = scenePos.x();
    d->cursorItem->setPos(xPos, 0);
    update();
}

// --------------------------------------------------------------------------------- //
// GAssemblyViewPrivate implementation
// --------------------------------------------------------------------------------- //

void GAssemblyView::GAssemblyViewPrivate::CreateFont(void) {

// I hate to pepper platform-dependent ifdefs but fonts are definitely not consistent across OS's
//
// A predictable monospace, fixed-pitch font is absolutely crucial to accurate assembly view.
// OS is **guided** toward a desirable font-style, but actual font selection is not guaranteed.
// Therefore, font metrics have to be detected at runtime.

#if defined(Q_OS_LINUX)
    font.setFamily("DejaVu Sans Mono");
#elif defined(Q_OS_WIN32)
    font.setFamily("Lucida Console");
#elif defined(Q_OS_MAC)
    // Menlo font introduced in OSX10.6, so if not found try "old" Monaco font
    font.setFamily("Menlo");
    if (!font.exactMatch()) { font.setFamily("Monaco"); }
#endif

    // set font attributes
    font.setPointSize(8);
    font.setFixedPitch(true);
    font.setKerning(false);
    font.setStyleHint(QFont::TypeWriter);
    font.setStyleStrategy(QFont::PreferBitmap);
    font.setLetterSpacing(QFont::AbsoluteSpacing, fontSpacing);

    // store font measurements, depending on runtime font selection
    QFontMetrics fontMetrics(font);
    fontHeight = fontMetrics.height();
    fontWidth  = fontMetrics.width('G') + fontSpacing;
}

void GAssemblyView::GAssemblyViewPrivate::ClearCurrentData(void) {

    // clear out alignment groups & group map
    QList<GVisibleAlignmentGroup*> items = groupMap.values();
    while (!items.isEmpty()) {
        GVisibleAlignmentGroup* visibleGroup = items.takeFirst();
        if ( visibleGroup == NULL ) { continue; }
        visibleGroup->Clear();
        visibleGroup->deleteLater();
    }
    groupMap.clear();

    // clear out coordinate/annotation track items
    if ( trackGroup ) {
        scene->destroyItemGroup(trackGroup);
        trackGroup = 0;

        while (!trackItems.isEmpty() ) {
            QGraphicsItem* item = trackItems.takeFirst();
            if ( item == NULL ) { continue; }
            scene->removeItem(item);
            delete item;
            item = 0;
        }
    }

    // turn off scroll bars
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void GAssemblyView::GAssemblyViewPrivate::ShowData(const GGenomicDataSet& data) {

    // clear out current data
    ClearCurrentData();

    // reset track boundary
    nextAvailableTrackStart = 0;

    // set boundary positions
    leftBound  = data.Region.LeftBound;
    rightBound = data.Region.RightBound;

    // draw coordinate & annotation components
    ShowCoordinates(data.Padding);
    ShowReferenceSequence(data.Sequence);
    ShowGenes(data.Genes);
    ShowSnps(data.Snps);
    scene->update();

    // draw alignment groups
    ShowAlignments(data.Alignments);
    scene->update();

    // update scene attributes, cursor items, and track background
    if (isCenterOnSet) { view->SetCenterOn(centerOnPosition); }
    cursorItem->update( cursorItem->boundingRect() );
    scene->setSceneRect( scene->itemsBoundingRect() );
    UpdateTrackBackground();
    scene->update();

    // clear center on flag
    centerOnPosition = 0;
    isCenterOnSet = false;

    // turn on scroll bars
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void GAssemblyView::GAssemblyViewPrivate::ShowCoordinates(const GPaddingMap& padding) {

    // positional values
    qint32 coordinate_X   = 0;
    qint32 windowPosition = 0;
    const qint32 interval = 10;

    // drawing size values
    const qint32 label_Ypos      = nextAvailableTrackStart + 10;
    const qint32 tickMark_height = 10;
    const qint32 tickMark_Ypos   = label_Ypos + 20;

    // intialize label text stream (used for converting number to characters)
    QString label;
    QTextStream str(&label);

    // iterate over data range
    for (qint32 i = leftBound; i <= rightBound; ++i) {

        // if genomic index has padding
        if (padding.contains(i)) {
            // increase window position marker by number of pads
            windowPosition += padding.value(i);
        }

        // if position is at tick mark interval
        if ( (i % interval) == 0 ) {

            // x coordinate is data 'window position' * font width
            coordinate_X = (windowPosition * fontWidth);

            // label attributes
            label.clear();
            str << dec << i;

            QGraphicsTextItem* tickMarkLabel = new QGraphicsTextItem(label);
            tickMarkLabel->setFont(font);
            tickMarkLabel->setDefaultTextColor(QColor(Qt::white));
            tickMarkLabel->setPos( QPointF((coordinate_X + viewMargin/2), label_Ypos) );
            tickMarkLabel->setZValue(5);

            // add label to scene
            scene->addItem(tickMarkLabel);
            trackItems.append(tickMarkLabel);

            // tick mark attributes
            QPointF tickMark_Top( viewMargin, 0 );
            QPointF tickMark_Bottom( viewMargin, tickMark_height);
            QGraphicsLineItem* tickMarkLine = new QGraphicsLineItem( QLineF(tickMark_Top, tickMark_Bottom));
            tickMarkLine->setPos( QPointF((coordinate_X + (fontWidth/2) ), tickMark_Ypos) );
            tickMarkLine->setPen(QPen(QColor(Qt::white)));
            tickMarkLine->setZValue(5);

            // add tick mark to scene
            scene->addItem(tickMarkLine);
            trackItems.append(tickMarkLine);
        }
        // increment window position marker
        ++windowPosition;
    }

    // save next track position
    nextAvailableTrackStart = tickMark_Ypos + 10;
}

void GAssemblyView::GAssemblyViewPrivate::ShowReferenceSequence(const QString& sequence) {

    // skip if nothing to draw
    if ( sequence.isEmpty() ) { return; }

    // create new text item
    QGraphicsTextItem* item = new QGraphicsTextItem(sequence);
    item->setFont(font);
    item->setPos( viewMargin/2, nextAvailableTrackStart );
    item->setDefaultTextColor( QColor(Qt::white) );
    item->setZValue(5);

    // save sequence as track item
    scene->addItem(item);
    trackItems.append(item);

    // save next track position
    nextAvailableTrackStart += (int)item->boundingRect().height() + 10;
}

void GAssemblyView::GAssemblyViewPrivate::ShowGenes(const GGeneList& genes) {

    // skip if nothing to draw
    if ( genes.isEmpty() ) { return; }

    // layout parameters - this will all surely change or move elsewhere
    const qint32 topRow_Y   = nextAvailableTrackStart;
    const qint32 geneHeight = 15;
    const qint32 geneSpacer = 2;

    // store 'next-available' position for each row
    QList<qreal> rows;
    rows.append(0);

    // iterate over genes
    for ( int index = 0; index < genes.size(); ++index ) {
        const GGene& gGene = genes[index];

        // create visible item
        GVisibleGeneItem* item = new GVisibleGeneItem(gGene, fontHeight, fontWidth);
        if (item == NULL) { continue; }
        item->setZValue(5);
        connect( item, SIGNAL(clicked(GGene)), view, SLOT(CatchGeneClick(GGene)));

        // calculate starting X coordinate for gene
        qreal geneStartX = ( (gGene.Start + gGene.StartOffset - leftBound)*fontWidth ) + viewMargin;

        // calculate row to place alignment
        int useRow = 0;
        for ( ; useRow < rows.size(); ++useRow) {
            if ( geneStartX >= rows.at(useRow) ) { break; }
        }

        // if row is beyond currently 'known' rows, create new row
        if ( useRow == rows.size() ) { rows.append(geneStartX); }

        // set gene item position
        item->setPos(geneStartX, (useRow*(geneHeight+geneSpacer) + topRow_Y));

        // save new end position for row used
        rows[useRow] = geneStartX + item->boundingRect().width() + geneSpacer*fontWidth;

        // add gene item to scene
        scene->addItem(item);
        trackItems.append(item);
    }

    // save next track position
    nextAvailableTrackStart += (rows.size() * (geneSpacer*fontHeight)) + 10;
}

void GAssemblyView::GAssemblyViewPrivate::ShowSnps(const GSnpList& snps) {

    // skip if nothing to draw
    if ( snps.isEmpty() ) { return; }

    // layout parameters - this will all surely change or move elsewhere
    const qint32 topRow_Y  = nextAvailableTrackStart;
    const qint32 snpHeight = 15;
    const qint32 snpSpacer = 2;

    // store 'next-available' position for each row
    QList<qreal> rows;
    rows.append(0);

    // iterate over snps
    for ( int index = 0; index < snps.size(); ++index ) {
        const GSnp& gSnp = snps[index];

        // create visible item
        GVisibleSnpItem* item = new GVisibleSnpItem(gSnp, fontHeight, fontWidth);
        if (item == NULL) { continue; }
        item->setZValue(5);
        connect(item, SIGNAL(clicked(GSnp)), view, SLOT(CatchSnpClick(GSnp)));

        // calculate starting X coordinate for snp
        qreal snpStartX = ( (gSnp.Position + gSnp.PaddingOffset - leftBound)*fontWidth ) + viewMargin;

        // calculate row to place snp
        int useRow = 0;
        for ( ; useRow < rows.size(); ++useRow) {
            if ( snpStartX >= rows.at(useRow) ) { break; }
        }

        // if row is beyond currently 'known' rows, create new row
        if ( useRow == rows.size() ) { rows.append(snpStartX); }

        // set gene item position
        item->setPos(snpStartX, (useRow*(snpHeight+snpSpacer) + topRow_Y));

        // save new end position for row used
        rows[useRow] = snpStartX + item->boundingRect().width() + snpSpacer*fontWidth;

        // add snp item to scene
        scene->addItem(item);
        trackItems.append(item);
    }

    // save next track position
    nextAvailableTrackStart += (rows.size() * (snpSpacer*fontHeight)) + 10;
}

void GAssemblyView::GAssemblyViewPrivate::UpdateTrackBackground(void) {

    scene->update();

    QPointF topLeft(0,0);
    QPointF bottomRight( scene->width(), nextAvailableTrackStart);
    QRectF  rectangle(topLeft, bottomRight);
    QGraphicsRectItem* item = scene->addRect(rectangle, QPen(Qt::black), QBrush(Qt::black));
    item->setPos(0,0);
    item->setZValue(3);
    trackItems.append(item);

    trackGroup = scene->createItemGroup(trackItems);
    trackGroup->setHandlesChildEvents(false);
}

void GAssemblyView::GAssemblyViewPrivate::ShowAlignments(const GAlignmentList& alignments) {

    // clear out alignment groups & group map
    QList<GVisibleAlignmentGroup*> items = groupMap.values();
    while (!items.isEmpty()) {
        GVisibleAlignmentGroup* visibleGroup = items.takeFirst();
        if ( visibleGroup == NULL ) { continue; }
        visibleGroup->Clear();
        delete visibleGroup;
        visibleGroup = 0;
    }
    groupMap.clear();

    // create & draw new groups
    CreateNewAlignmentGroups(alignments);
    DrawAlignmentGroups();
    AdjustGroupLayout();
}

void GAssemblyView::GAssemblyViewPrivate::CreateNewAlignmentGroups(const GAlignmentList& alignments) {

    // get current 'normal' color scheme for creating groups
    const GColorScheme& colorScheme = settingsManager->NormalColorScheme();

    // if 'merge groups' active, create single alignment group
    if ( settingsManager->IsGroupsMerged() ) {

        // create group
        QString groupLabel = "Merged";
        GVisibleAlignmentGroup* visibleGroup = new GVisibleAlignmentGroup(groupLabel, colorScheme, font, fontHeight, fontWidth, leftBound);
        connect(visibleGroup, SIGNAL(GroupExpanded()),  view, SLOT(AdjustGroupLayout()));
        connect(visibleGroup, SIGNAL(GroupCollapsed()), view, SLOT(AdjustGroupLayout()));
        connect(view, SIGNAL(HorizontalScrollChanged(int,qreal)), visibleGroup, SLOT(MoveHeaderToPosition(int, qreal)));

        // set bases-visible flag
        visibleGroup->SetBasesVisible( settingsManager->IsBasesVisible() );

        // add alignments to group
        visibleGroup->AddAlignments(alignments);

        // add group to scene
        scene->addItem(visibleGroup);

        // store group data
        groupMap.insert(groupLabel, visibleGroup);
    }

    // else split by read group text
    else {

        foreach(GAlignment alignment, alignments) {

            // see if visible group already exists for this read group
            QString label = alignment.ReadGroup;
            GVisibleAlignmentGroup* visibleGroup = 0;
            if ( groupMap.contains(label) ) {
                visibleGroup = groupMap.value(label);
            }

            // else create new group
            else {
                visibleGroup = new GVisibleAlignmentGroup(label, colorScheme, font, fontHeight, fontWidth, leftBound);
                connect(visibleGroup, SIGNAL(GroupExpanded()),  view, SLOT(AdjustGroupLayout()));
                connect(visibleGroup, SIGNAL(GroupCollapsed()), view, SLOT(AdjustGroupLayout()));
                connect(view, SIGNAL(HorizontalScrollChanged(int,qreal)), visibleGroup, SLOT(MoveHeaderToPosition(int, qreal)));

                // set bases-visible flag
                visibleGroup->SetBasesVisible( settingsManager->IsBasesVisible() );

                scene->addItem(visibleGroup);
                groupMap.insert(label, visibleGroup);
            }

            // store alignment in visible group
            visibleGroup->AddAlignment(alignment);
        }
    }
}

void GAssemblyView::GAssemblyViewPrivate::DrawAlignmentGroups(void) {

     // skip if no groups to draw
    if ( groupMap.isEmpty() ) { return; }

    // iterate over groups, skip invalid group entries
    foreach (GVisibleAlignmentGroup* group, groupMap) {

        // skip if invalid group
        if ( group == NULL ) { continue; }

        // set group color scheme
        settingsManager->SetGroupColorScheme(group);

        // draw, but set invisible until group layouts are adjusted
        group->SetLeftBound(leftBound);
        group->Draw();
        group->setVisible(false);
    }

    // turn on scoll bars
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void GAssemblyView::GAssemblyViewPrivate::AdjustGroupLayout(void) {

    // skip if no groups to draw
    if ( groupMap.isEmpty() ) { return; }

    // initialize coordinates
    const qreal xPos = 0;
          qreal yPos = (qreal)nextAvailableTrackStart;

    // iterate over groups, skip invalid group entries
    foreach (GVisibleAlignmentGroup* group, groupMap) {
        if ( group == NULL ) { continue; }

        // show group, set to proper position on scene
        group->setVisible(true);
        group->setPos(xPos, yPos);

        // update coordinates for next group
        yPos += group->boundingRect().height();
    }

    // update scene & cursorItem
    scene->update( scene->sceneRect() );
    cursorItem->SetHeight( scene->sceneRect().height() );
}

GAlleleList GAssemblyView::GAssemblyViewPrivate::AllelesOverlappingPosition(qint32 position) {

    // initialize allele list
    GAlleleList alleles;

    // iterate over alignment groups
    foreach(GVisibleAlignmentGroup* group, groupMap) {

        // store list of alleles from each group that overlap desired position
        alleles.append( group->AllelesOverlappingPosition(position) );
    }

    // return list of alleles
    return alleles;
}

void GAssemblyView::GAssemblyViewPrivate::MergeReadGroups(bool ok) {

    Q_UNUSED(ok);

    // get current alignments
    GAlignmentList alignments;
    foreach (GVisibleAlignmentGroup* visibleGroup, groupMap) {
        if ( visibleGroup == NULL ) { continue; }
        alignments.append( visibleGroup->Alignments() );
    }

    // sort alignments (for proper collapsed view)
    qSort(alignments.begin(), alignments.end(), GAlignment::SortByPosition());

    // re-draw alignment groups
    ShowAlignments(alignments);
}

void GAssemblyView::GAssemblyViewPrivate::ShowBases(bool ok) {

    foreach( GVisibleAlignmentGroup* group, groupMap ) {
        if ( group == NULL ) { continue; }
        group->SetBasesVisible(ok);
        group->update( group->boundingRect() );
    }
    view->update();
}

void GAssemblyView::ViewSettingsChanged(void) {
    foreach (GVisibleAlignmentGroup* group, d->groupMap) {
        if ( group == NULL ) { continue; }
        d->settingsManager->SetGroupColorScheme(group);
        group->update( group->boundingRect() );
    }
    update();
}
