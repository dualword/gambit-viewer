// ***************************************************************************
// GSnpInfoDialog.cpp (c) 2009 Derek Barnett
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
// Provides a dialog window displaying coverage and quality summary for
// alleles overlapping a clicked GVisibleSnpItem.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/GSnpInfoDialog.h"
#include "DataStructures/GSnp.h"
using namespace Gambit;

struct GSnpInfoDialog::GSnpInfoDialogPrivate {

    // data members
    GSnp snp;
    GAlleleList alleles;

    // data members
    QLabel* positionLabel;
    QLabel* scoreLabel;
    QLabel* coverageLabel;
    QPushButton* closeButton;

    QGraphicsView*      alleleGraphView;
    QGraphicsScene*     alleleGraphScene;
    QTableView*         alleleDataView;
    QStandardItemModel* alleleDataModel;
    QMap<QChar, QList<quint32> > qualityMap;

    // constructor & destructor
    GSnpInfoDialogPrivate(void) { }
    ~GSnpInfoDialogPrivate(void) {
        alleles.clear();
        qualityMap.clear();
    }

    // internal methods
    void DrawAlleleGraph(void);
    void Init(void);
    void LoadData(void);
    void PopulateModelForAllele(QChar allele, QList<quint32> qualities, int column);
};

void GSnpInfoDialog::GSnpInfoDialogPrivate::Init(void) {

    // set up info components
    positionLabel = new QLabel(QString("Position: %1").arg(QString::number(snp.Position)));
    scoreLabel    = new QLabel(QString("Score: %1").arg(snp.Score));
    coverageLabel = new QLabel(QString("Coverage: %1").arg(alleles.size()));

    // set up data view
    alleleDataView = new QTableView;
    alleleDataView->setFixedSize(320, 90);
    alleleDataView->setSelectionMode(QAbstractItemView::NoSelection);

    // set up data model
    alleleDataModel = new QStandardItemModel(2, 6);
    alleleDataModel->setVerticalHeaderLabels( QStringList() << "Count" << "Avg Qual" );
    alleleDataView->setModel(alleleDataModel);

    // horizontal header (column labels)
    QHeaderView* header = alleleDataView->horizontalHeader();
    header->setStretchLastSection(false);
    header->setResizeMode(QHeaderView::ResizeToContents);
    header->setClickable(false);
    header->setMovable(false);

    // vertical header (row labels)
    header = alleleDataView->verticalHeader();
    header->setStretchLastSection(false);
    header->setResizeMode(QHeaderView::ResizeToContents);
    header->setClickable(false);
    header->setMovable(false);

    // set up SNP allele graph view attributes
    alleleGraphView = new QGraphicsView;
    alleleGraphView->setFixedSize(320, 180);
    alleleGraphView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    QPalette p = alleleGraphView->palette();
    p.setBrush(QPalette::Base, QBrush(QColor(Qt::black)));
    alleleGraphView->setPalette(p);

    // set up SNP allele graph scene
    alleleGraphScene = new QGraphicsScene;
    alleleGraphView->setScene(alleleGraphScene);

    // set up close button
    closeButton = new QPushButton("Close");
}

GSnpInfoDialog::GSnpInfoDialog(const GSnp& snp, GAlleleList alleles, QWidget* parent)
    : QDialog(parent)

{
    d = new GSnpInfoDialogPrivate;
    d->snp = snp;
    d->alleles = alleles;
    d->Init();

    // set up SNP info display area
    QVBoxLayout* snpInfoLayout = new QVBoxLayout;
    snpInfoLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    snpInfoLayout->addWidget(d->positionLabel);
    snpInfoLayout->addWidget(d->scoreLabel);
    snpInfoLayout->addWidget(d->coverageLabel);

    // set up dialog layout
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(snpInfoLayout, 0);
    layout->addWidget(d->alleleGraphView, 0, Qt::AlignCenter);
    layout->addWidget(d->alleleDataView,  0, Qt::AlignTop | Qt::AlignHCenter);
    layout->addStretch(2);
    layout->addWidget(d->closeButton, 0, Qt::AlignBottom | Qt::AlignRight);
    connect(d->closeButton, SIGNAL(clicked()), this, SLOT(accept()));

    // populate data model
    d->LoadData();

    // set dialog attributes
    setWindowTitle(tr("Variant Info"));
    setLayout(layout);
    setModal(false);
}

GSnpInfoDialog::~GSnpInfoDialog(void) {
    delete d;
    d = 0;
}

void GSnpInfoDialog::GSnpInfoDialogPrivate::LoadData(void) {

    // clear any prior data
    alleleDataModel->removeColumns(0, alleleDataModel->columnCount());
    qualityMap.clear();

    // intialize quality map
    qualityMap.insert('A', QList<quint32>() );
    qualityMap.insert('C', QList<quint32>() );
    qualityMap.insert('G', QList<quint32>() );
    qualityMap.insert('T', QList<quint32>() );
    qualityMap.insert('N', QList<quint32>() );
    qualityMap.insert('*', QList<quint32>() );
    qualityMap.insert('-', QList<quint32>() );

    // iterate over alleles
    for (int index = 0; index < alleles.size(); ++index) {
        GAllele& gAllele = alleles[index];
        qualityMap[gAllele.Base].append(gAllele.Quality);
    }

    // set up allele data
    int currentColumn = 0;
    QMap<QChar, QList<quint32> >::const_iterator mapIter = qualityMap.constBegin();
    QMap<QChar, QList<quint32> >::const_iterator mapEnd  = qualityMap.constEnd();
    for ( ; mapIter != mapEnd; ++mapIter ) {
        if ( mapIter.value().size() > 0 ) {
            PopulateModelForAllele(mapIter.key(), mapIter.value(), currentColumn);
            ++currentColumn;
        }
    }

    // set row labels
    alleleDataModel->setVerticalHeaderItem(0, new QStandardItem(QString("Count")));
    alleleDataModel->setVerticalHeaderItem(1, new QStandardItem(QString("Avg Qual")));

    // draw allele graph
    DrawAlleleGraph();
}

void GSnpInfoDialog::GSnpInfoDialogPrivate::PopulateModelForAllele(QChar allele, QList<quint32> qualities, int currentColumn) {

    // sum up all qualities for this allele
    quint32 sum = 0;
    for( int i = 0; i < qualities.size(); ++i ) {
        sum += qualities.at(i);
    }

    // insert column for allele
    alleleDataModel->insertColumn(currentColumn, QModelIndex());
    alleleDataModel->setHorizontalHeaderItem(currentColumn, new QStandardItem(QString(allele)));

    // format and set data for coverage count
    alleleDataModel->setData( alleleDataModel->index(0, currentColumn, QModelIndex()), QBrush(QColor(Qt::black)), Qt::BackgroundRole);
    alleleDataModel->setData( alleleDataModel->index(0, currentColumn, QModelIndex()), QBrush(QColor(Qt::white)), Qt::ForegroundRole);
    alleleDataModel->setData( alleleDataModel->index(0, currentColumn, QModelIndex()), Qt::AlignHCenter,          Qt::TextAlignmentRole);
    alleleDataModel->setData( alleleDataModel->index(0, currentColumn, QModelIndex()), qualities.size(),          Qt::DisplayRole);
    alleleDataModel->item(0,currentColumn)->setFlags(Qt::NoItemFlags);

    // format and show data for average quality value
    alleleDataModel->setData( alleleDataModel->index(1, currentColumn, QModelIndex()), QBrush(QColor(Qt::black)),     Qt::BackgroundRole);
    alleleDataModel->setData( alleleDataModel->index(1, currentColumn, QModelIndex()), QBrush(QColor(Qt::white)),     Qt::ForegroundRole);
    alleleDataModel->setData( alleleDataModel->index(1, currentColumn, QModelIndex()), Qt::AlignHCenter,              Qt::TextAlignmentRole);
    alleleDataModel->setData( alleleDataModel->index(1, currentColumn, QModelIndex()), ((float)sum/qualities.size()), Qt::DisplayRole);
    alleleDataModel->item(1,currentColumn)->setFlags(Qt::NoItemFlags);
}

void GSnpInfoDialog::GSnpInfoDialogPrivate::DrawAlleleGraph(void) {

    // make sure data is available
    if ( qualityMap.isEmpty() ) { return; }

    // calculate max coverage
    int maxCoverage = 0;
    QMap<QChar, QList<quint32> >::const_iterator mapIter = qualityMap.constBegin();
    QMap<QChar, QList<quint32> >::const_iterator mapEnd  = qualityMap.constEnd();
    for ( ; mapIter != mapEnd; ++mapIter ) {
        if ( mapIter.value().size() > maxCoverage ) {
            maxCoverage = mapIter.value().size();
        }
    }

    // draw graph items
    int allelesDrawn    = 0;
    const int tableOffset     = alleleDataView->verticalHeader()->width();
    const qreal graphBaseLine = 180;
    const qreal graphBarWidth = 20;

    // iterate over alleles
    for ( mapIter = qualityMap.constBegin(); mapIter != mapEnd; ++mapIter ) {

        // get total coverage for allele
        int alleleCoverage = mapIter.value().size();

        // skip if no data available for current allele
        if ( alleleCoverage == 0 ) { continue; }

        // calculate size of graph bar

        qreal graphBarHeight = ( ((qreal)alleleCoverage/(maxCoverage + 2)) * graphBaseLine );

        // determine x coordinate of graph bar
        qreal dataColumnStart = /*graphViewStart*/ tableOffset + alleleDataView->columnViewportPosition(allelesDrawn);
        qreal dataColumnWidth = alleleDataView->columnWidth(allelesDrawn);
        qreal graphBarPos     = (dataColumnStart + (dataColumnWidth)/2) - (graphBarWidth/2);

        // draw allele graph bar
        QGraphicsRectItem* alleleItem = new QGraphicsRectItem(QRectF(QPointF(0,0), QSizeF(graphBarWidth, graphBarHeight)));

        // set graph bar color
        char base = mapIter.key().toAscii();
        switch( base ) {
            case('a') :
            case('A') : alleleItem->setBrush(QColor("green"));
                        break;
            case('c') :
            case('C') : alleleItem->setBrush(QColor("blue"));
                        break;
            case('g') :
            case('G') : alleleItem->setBrush(QColor("yellow"));
                        break;
            case('t') :
            case('T') : alleleItem->setBrush(QColor("red"));
                        break;
            case('N') : alleleItem->setBrush(QColor("lightgray"));
                        break;
            case('*') : alleleItem->setBrush(QColor("beige"));
                        break;
            case('-') : alleleItem->setBrush(QColor("cadetblue"));
                        break;
            default   : ; // error message?
        }

        // set graph bar position
        alleleGraphScene->addItem(alleleItem);

        QPoint viewPos(graphBarPos, (graphBaseLine - graphBarHeight)-10 );
        alleleItem->setPos( alleleGraphView->mapToScene(viewPos) );
        qDebug() << "Item pos set to point: " << alleleGraphView->mapToScene(viewPos);

        // increment counter
        ++allelesDrawn;
    }

    QRectF rect(0, 0, 300, 160);
    alleleGraphScene->setSceneRect(rect);
    alleleGraphView->ensureVisible(rect);
    alleleGraphView->update();

}
