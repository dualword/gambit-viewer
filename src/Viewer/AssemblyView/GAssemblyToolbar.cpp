// ***************************************************************************
// GAssemblyToolbar.cpp (c) 2009 Derek Barnett
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
// Provides the toolbar that sits above the GAssemblyView.  Provides access
// to common view actions.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/AssemblyView/GAssemblyToolbar.h"
using namespace Gambit;
using namespace Gambit::Viewer;

// initialize options (and option labels) for alignment filters
const QStringList OptionNames = AlignmentOptionNames();
const QList<GAlignment::GAlignmentOption> Options = AlignmentOptions();

namespace Gambit {
namespace Viewer {

class GToolbarSeparator : public QFrame {

    public:
        GToolbarSeparator(QWidget* parent = 0)
            : QFrame(parent)
        {
            setFrameStyle( QFrame::VLine | QFrame::Plain );
            setLineWidth(2);
            setMidLineWidth(1);
        }
};

} // namespace Viewer
} // namespace Gambit

const qint32 GAssemblyToolbar::DEFAULT_POSITION = 1;
const qint32 GAssemblyToolbar::DEFAULT_RANGE    = 500;

struct GAssemblyToolbar::GAssemblyToolbarPrivate {

    GAssemblyToolbar* parentToolbar;

    QLabel*    referenceLabel;
    QComboBox* referenceComboBox;
    QLabel*    positionLabel;
    QLineEdit* positionLineEdit;
    QLabel*    rangeLabel;
    QComboBox* rangeComboBox;
    QAction*   jumpAction;

    void Init(GAssemblyToolbar* parent);
    void GetBoundaryCoordinates(qint32& leftBound, qint32& rightBound);
};

void GAssemblyToolbar::GAssemblyToolbarPrivate::Init(GAssemblyToolbar* parent) {

    // save back-pointer to parent class
    parentToolbar = parent;

    // set up reference selector
    referenceLabel    = new QLabel("Reference: ");
    referenceComboBox = new QComboBox;
    referenceComboBox->setEditable(false);
    referenceComboBox->setMinimumContentsLength(8);
    referenceComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    // set up position input field
    positionLabel    = new QLabel("Position: ");
    positionLineEdit = new QLineEdit;
    positionLineEdit->setEnabled(true);
    positionLineEdit->setMaximumWidth(200);
    positionLineEdit->setMaxLength(12);

    // adjust line edit background
    QPalette p;
    p.setBrush(QPalette::Window, QBrush(Qt::white));
    positionLineEdit->setPalette(p);
    positionLineEdit->setAutoFillBackground(true);

    // set up range selector
    rangeLabel    = new QLabel("Range: ");
    rangeComboBox = new QComboBox;
    rangeComboBox->setEditable(false);
    rangeComboBox->addItems( QStringList() << "100" << "500" << "1000" << "5000" << "10000" );
    rangeComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    // set up 'jump' action
    jumpAction = new QAction("Jump", parentToolbar);
}

void GAssemblyToolbar::GAssemblyToolbarPrivate::GetBoundaryCoordinates(qint32& leftBound, qint32& rightBound ) {

    // get user selections
    qint32 position = positionLineEdit->text().toInt();
    qint32 range    = rangeComboBox->currentText().toInt();

    // default values   (1..range)
    leftBound  = GAssemblyToolbar::DEFAULT_POSITION;
    rightBound = range;

    // no position (0) allowed, auto-convert to (1)
    // update line edit box
    if ( position == 0 ) {
        position = 1;
        positionLineEdit->setText( QString::number(1) );
    }

    // if position with (1..range+1) keep coordinates as default
    // otherwise, calculate left/right-bound coordinates
    if ( position > ((range/2)+1) ) {
        leftBound  = position - (range/2);
        rightBound = position + (range/2);
    }
}


GAssemblyToolbar::GAssemblyToolbar(const QList<QAction*>& actions, QWidget* parent)
    : QToolBar(parent)
{
    d = new GAssemblyToolbarPrivate;
    d->Init(this);

    // set initial state
    SetPosition(GAssemblyToolbar::DEFAULT_POSITION);
    SetRange(GAssemblyToolbar::DEFAULT_RANGE);

    // make connections
    connect(d->positionLineEdit, SIGNAL(returnPressed()),      this, SLOT(JumpTriggered()));
    connect(d->jumpAction,       SIGNAL(triggered()),          this, SLOT(JumpTriggered()));

    // add components to toolbar
    addWidget(d->referenceLabel);
    addWidget(d->referenceComboBox);
    addWidget(d->positionLabel);
    addWidget(d->positionLineEdit);
    addWidget(d->rangeLabel);
    addWidget(d->rangeComboBox);
    addAction(d->jumpAction);
    addWidget( new GToolbarSeparator );

    // add actions to toolbar
    foreach (QAction* action, actions) { addAction(action); }

    // set attributes
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    setFloatable(false);
    setMovable(false);
}

GAssemblyToolbar::~GAssemblyToolbar(void) {
    delete d;
    d = 0;
}

void GAssemblyToolbar::JumpTriggered(void) {

    // calculate left/right-bound coordinates
    // (allowed to modify user-selected position, if position == 0)
    qint32 leftBound;
    qint32 rightBound;
    d->GetBoundaryCoordinates(leftBound, rightBound);

    // emit center-on request for selected position (could be modified, see above)
    qint32 position = d->positionLineEdit->text().toInt();
    emit ViewCenterOnRequested(position);

    // get reference name and signal data jump request
    QString referenceName = d->referenceComboBox->currentText();
    GGenomicDataRegion region(referenceName, leftBound, rightBound);
    emit JumpRequested(region);
}

void GAssemblyToolbar::SetPosition(qint32 position) {
    d->positionLineEdit->setText( QString::number(position) );
}

void GAssemblyToolbar::SetRange(qint32 value) {
    int index = d->rangeComboBox->findText(QString::number(value));
    if ( index > 0 ) { d->rangeComboBox->setCurrentIndex(index); }
    else { d->rangeComboBox->setCurrentIndex(0); }
}

void GAssemblyToolbar::SetReferenceNames(QStringList names) {
    d->referenceComboBox->clear();
    d->referenceComboBox->addItems(names);
}

void GAssemblyToolbar::SetSelectedReference(const QString& name) {
    int index = d->referenceComboBox->findText(name);
    if ( index == -1 ) { return; }
    d->referenceComboBox->setCurrentIndex(index);
}
