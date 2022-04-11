// ***************************************************************************
// GViewer.cpp (c) 2009 Derek Barnett
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
// Provides main viewer area - currently is composite of GAssemblyView,
// GAssemblyToolbar, GReferenceView, and GSliderView.
//
// Main interface between view components and GambitAPI.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/GViewer.h"
#include "DataStructures/GGenomicDataSet.h"
#include "Utilities/GStyleHelper.h"
#include "Viewer/AssemblyView/GAssemblyToolbar.h"
#include "Viewer/AssemblyView/GAssemblyView.h"
#include "Viewer/GReferenceView.h"
#include "Viewer/GSliderView.h"
using namespace Gambit;
using namespace Gambit::Core;
using namespace Gambit::Viewer;

namespace Gambit {
namespace Viewer {

class GViewerDivider : public QFrame {

    public:
        GViewerDivider(QWidget* parent = 0)
            : QFrame(parent)
        { }

        void paintEvent(QPaintEvent *event) {
            QPainter painter(this);
            QRect rect = event->rect().adjusted(0, 0, 1, 0);
            rect = style()->visualRect(layoutDirection(), geometry(), rect);
            GStyleHelper::verticalGradient(&painter, rect, rect);
        }
};

} // namespace Viewer
} // namespace Gambit

struct GViewer::GViewerPrivate {

    GAssemblyToolbar*  assemblyToolbar;
    GAssemblyView*     assemblyView;
    GReferenceView*    referenceView;
    GSliderView*       sliderView;
    GReferenceList     references;

    void Init(GViewer* parent);
};

void GViewer::GViewerPrivate::Init(GViewer* parent) {

    // create view components
    assemblyView    = new GAssemblyView(parent);
    referenceView   = new GReferenceView(parent);
    sliderView      = new GSliderView(parent);

    // create view toolbar(s)
    QList<QAction*> actions = assemblyView->SettingsActions();
    assemblyToolbar = new GAssemblyToolbar(actions, parent);

    // set size policies for components
    assemblyView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    sliderView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
}

GViewer::GViewer(QWidget* parent)
    : QFrame(parent)
{
    // initialize components
    d = new GViewerPrivate;
    d->Init(this);

    // make connections

    // convert jump request from toolbar to LoadData signal
    connect(d->assemblyToolbar, SIGNAL(JumpRequested(GGenomicDataRegion)),
            this, SLOT(CatchToolbarJumpRequest(GGenomicDataRegion)));

    // handle center-on request from toolbar
    connect(d->assemblyToolbar, SIGNAL(ViewCenterOnRequested(qint32)),
            d->assemblyView,    SLOT(SetCenterOn(qint32)));

    // handle reference clicks
    connect(d->referenceView, SIGNAL(ReferenceClicked(GReference)),
            this, SLOT(ReferenceDefaultDataRequested(GReference)));
    connect(d->referenceView, SIGNAL(ReferenceClicked(GReference)),
            d->sliderView,    SLOT(SetReference(GReference)));

    // handle slider move events (request new data)
    connect(d->sliderView, SIGNAL(RegionRequested(GGenomicDataRegion)),
            this,          SLOT(CatchSliderClick(GGenomicDataRegion)));

    // set up layout
    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(d->assemblyView);
    rightLayout->addWidget(d->sliderView);

    GViewerDivider* divider = new GViewerDivider;
    divider->setFixedWidth(10);
    QPalette p = divider->palette();
    QBrush brush = p.brush(QPalette::Base);
    QColor color = brush.color();
    color.setAlpha(30);
    brush.setColor(color);
    p.setBrush(QPalette::Base, brush);
    divider->setPalette(p);

    QHBoxLayout* viewLayout = new QHBoxLayout;
    viewLayout->setMargin(0);
    viewLayout->setSpacing(0);
    viewLayout->addWidget(d->referenceView);
    viewLayout->addWidget(divider);
    viewLayout->addLayout(rightLayout);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(d->assemblyToolbar, 0);
    layout->addLayout(viewLayout, 9);
    setLayout(layout);
}

GViewer::~GViewer(void) {
    delete d;
    d = 0;
}

void GViewer::ShowReferences(GReferenceList references) {
    d->references.clear();
    d->references = references;
    d->assemblyToolbar->SetReferenceNames( ReferenceNames(references) );
    d->referenceView->SetReferences(references);
}

void GViewer::Clear(void) {
    d->referenceView->Clear();
    d->assemblyView->ClearCurrentData();
}

void GViewer::Update(void) {
    d->assemblyView->update();
}

// forward zoom instructions on to GAssemblyView
void GViewer::ZoomIn(void)    { d->assemblyView->ZoomIn();    }
void GViewer::ZoomOut(void)   { d->assemblyView->ZoomOut();   }
void GViewer::ZoomReset(void) { d->assemblyView->ZoomReset(); }

void GViewer::SetCenterOn(qint32 position) {
    d->assemblyView->SetCenterOn(position);
    d->assemblyToolbar->SetPosition(position);
}

void GViewer::ShowAssembly(GGenomicDataSet data) {
    d->assemblyView->ShowData(data);
    d->assemblyToolbar->SetSelectedReference(data.Region.RefName);
    d->sliderView->SetSelectedRegion(data.Region);
}

void GViewer::Print(void) {

    QPrinter printer(QPrinter::ScreenResolution);
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() != QDialog::Accepted) { return; }

    QPainter painter(&printer);
    d->assemblyView->render(&painter);
}

void GViewer::ReferenceDefaultDataRequested(const GReference& reference) {
    d->assemblyToolbar->SetPosition(GAssemblyToolbar::DEFAULT_POSITION);
    d->assemblyToolbar->SetRange(GAssemblyToolbar::DEFAULT_RANGE);

    GGenomicDataRegion region(reference.Name, GAssemblyToolbar::DEFAULT_POSITION, GAssemblyToolbar::DEFAULT_RANGE);
    emit ViewerDataRequested(region);
    d->sliderView->SetSelectedRegion(region);
}

void GViewer::CatchSliderClick(const GGenomicDataRegion& region) {

    // update toolbar to reflect slider click
    qint32 range = (region.RightBound - region.LeftBound);
    qint32 position = region.LeftBound + (qint32)range/2;
    SetCenterOn(position);
    d->assemblyToolbar->SetSelectedReference(region.RefName);

    // emit signal
    emit ViewerDataRequested(region);
}

void GViewer::CatchToolbarJumpRequest(const GGenomicDataRegion& region) {

    // look up reference item
    bool found = false;
    GReference ref;
    foreach (ref, d->references) {
        if ( QString::compare(ref.Name, region.RefName) == 0 ) {
            found = true;
            break;
        }
    }

    // if found, set slider and emit request signal
    if ( found ) {
        d->sliderView->SetReference(ref);
        d->sliderView->SetSelectedRegion(region);
        emit ViewerDataRequested(region);
    }
}
