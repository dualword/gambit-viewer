// ***************************************************************************
// GHomeWidget.cpp (c) 2009 Derek Barnett
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
// Describes the main home screen.  Displays basic welcome message(s) and
// provides create/load session options.
// ***************************************************************************

#include <QtGui>
#include "Main/GHomeWidget.h"
using namespace Gambit;

GHomeWidget::GHomeWidget(QWidget* parent)
    : QWidget(parent)
{
    QPixmap logo(":/icons/logo");
    QLabel* logoLabel = new QLabel(this);
    logoLabel->setPixmap(logo.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QFrame* logoFrame = new QFrame;
    logoFrame->setAutoFillBackground(true);
    logoFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    QPixmap background(":icons/background");
    QBrush brush(QPixmap(background.scaled(600, 300, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
    QPalette p;
    p.setBrush(QPalette::Window, brush);
    logoFrame->setPalette(p);

    QHBoxLayout* logoLayout = new QHBoxLayout;
    logoLayout->addWidget(logoLabel, 4, Qt::AlignLeft);
    logoFrame->setLayout(logoLayout);

    QString descriptionText = tr(
        "<h3>Welcome</h3>"
        "<p>Gambit is a cross-platform, low-memory viewer and analysis workbench for genomic data.</p>"
        "<p>It provides integration of sequence alignments, annotation data, and built-in analysis tools.<br></p>"
        );
    QLabel* descriptionLabel = new QLabel(descriptionText);
    descriptionLabel->setWordWrap(true);

    QString instructionText = tr(
            "<p>Gambit uses \'sessions\' for conveniently reloading commonly accessed data.</p>"
            "<p>Please select one of the following options:<br></p>"
            );
    QLabel* instructionLabel = new QLabel(instructionText);
    instructionLabel->setWordWrap(true);

    QPushButton* createButton = new QPushButton(QIcon(":icons/new"),  "New session...");
    QPushButton* openButton   = new QPushButton(QIcon(":icons/open"), "Open existing session...");
    connect(createButton, SIGNAL(clicked()), this, SIGNAL(RequestCreateSession()));
    connect(openButton,   SIGNAL(clicked()), this, SIGNAL(RequestLoadSession()));

    QVBoxLayout* selectionLayout = new QVBoxLayout;
    selectionLayout->addWidget(instructionLabel);
    selectionLayout->addWidget(createButton);
    selectionLayout->addWidget(openButton);

    QFrame* selectionWidget = new QFrame;
    selectionWidget->setFrameStyle(QFrame::StyledPanel);
    selectionWidget->setLayout(selectionLayout);

    QHBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(descriptionLabel, 7, Qt::AlignTop | Qt::AlignLeft);
    bottomLayout->addWidget(selectionWidget,  3, Qt::AlignTop | Qt::AlignRight);

    QFrame* bottomFrame = new QFrame;
    bottomFrame->setLayout(bottomLayout);

    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->setFrameStyle(QFrame::StyledPanel);
    splitter->setHandleWidth(0);
    splitter->addWidget(logoFrame);
    splitter->addWidget(bottomFrame);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget( new QToolBar );
    layout->addStretch(3);
    layout->addWidget(splitter, 6, Qt::AlignCenter);
    layout->addStretch(9);
    setLayout(layout);
}
