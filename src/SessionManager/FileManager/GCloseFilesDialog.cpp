// ***************************************************************************
// GCloseFilesDialog.cpp (c) 2009 Derek Barnett
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
// Describes a dialog used to show files that are currently open, allows user
// to select which files to close.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "SessionManager/FileManager/GCloseFilesDialog.h"
using namespace Gambit;

struct GCloseFilesDialog::GCloseFilesDialogPrivate {

    QLabel*       instructionLabel;
    QListWidget*  filesView;
    QPushButton*  okButton;
    QPushButton*  cancelButton;
    GFileInfoList currentFiles;

    GCloseFilesDialogPrivate(const GFileInfoList& openFiles)
        : currentFiles(openFiles)
    {
        Init();
    }

    void Init(void);
    void PopulateFilesView(void);
};

void GCloseFilesDialog::GCloseFilesDialogPrivate::Init(void) {

    // initialize components
    instructionLabel = new QLabel("Please select files to close, then click Ok");
    filesView        = new QListWidget;
    okButton         = new QPushButton(QIcon(":/icons/accept"), QString("Ok"));
    cancelButton     = new QPushButton(QIcon(":/icons/cancel"), QString("Cancel"));

    // set up file view attributes
    filesView->setResizeMode(QListView::Adjust);
    filesView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    filesView->setTextElideMode(Qt::ElideMiddle);

    PopulateFilesView();
}

void GCloseFilesDialog::GCloseFilesDialogPrivate::PopulateFilesView(void) {
    foreach (GFileInfo fileInfo, currentFiles) {
        QListWidgetItem* item = new QListWidgetItem(fileInfo.Filename);
        item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable );
        item->setCheckState(Qt::Unchecked);
        filesView->addItem(item);
    }
}

GCloseFilesDialog::GCloseFilesDialog(const GFileInfoList& openFiles, QWidget* parent)
    : QDialog(parent)
{
    d = new GCloseFilesDialogPrivate(openFiles);

    // set up dialog layout
    QHBoxLayout* instructionLayout = new QHBoxLayout;
    instructionLayout->addWidget(d->instructionLabel);

    QHBoxLayout* fileViewLayout = new QHBoxLayout;
    fileViewLayout->addWidget(d->filesView);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(3);
    buttonLayout->addWidget(d->okButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(d->cancelButton);
    buttonLayout->addStretch(3);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(instructionLayout,0);
    layout->addLayout(fileViewLayout,3);
    layout->addLayout(buttonLayout,0);
    setLayout(layout);

    // make connections
    connect(d->okButton,     SIGNAL(clicked()), this, SLOT(accept()));
    connect(d->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(d->filesView,    SIGNAL(itemClicked(QListWidgetItem*)),
            this,            SLOT(FileItemClicked(QListWidgetItem*)));

    // set up dialog attributes
    setWindowTitle("Select Files to Close");
    setMinimumHeight(400);
    setMinimumWidth(300);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
}

GCloseFilesDialog::~GCloseFilesDialog(void) {
    delete d;
    d = 0;
}

GFileInfoList GCloseFilesDialog::SelectedFiles(void) const {
    GFileInfoList filesToClose;
    int itemCount = d->filesView->count();
    for(int index = 0; index < itemCount; ++index) {
        QListWidgetItem* item = d->filesView->item(index);
        if ( item->checkState() == Qt::Checked ) {
            filesToClose.append( d->currentFiles.at(index) );
        }
    }
    return filesToClose;
}

void GCloseFilesDialog::FileItemClicked(QListWidgetItem* item) {

    // skip invalid list item
    if ( item == NULL ) { return; }

    // flip check state
    if ( item->checkState() == Qt::Checked ) {
        item->setCheckState(Qt::Unchecked);
    } else {
        item->setCheckState(Qt::Checked);
    }
}
