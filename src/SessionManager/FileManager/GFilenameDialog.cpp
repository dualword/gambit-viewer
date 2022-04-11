// ***************************************************************************
// GFilenameDialog.cpp (c) 2009 Derek Barnett
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
// Describes a dialog used to prompt the user for a 'load' or 'save' filename.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "SessionManager/FileManager/GFilenameDialog.h"
using namespace Gambit;
using namespace Gambit::Core;

struct GFilenameDialog::GFilenameDialogPrivate {

    QLabel*      titleLabel;
    QLineEdit*   filenameLineEdit;
    QPushButton* browseButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
    GFilenameDialog::PromptType type;

    void Init(void);
    void CheckFilename(QString& filename);
};

void GFilenameDialog::GFilenameDialogPrivate::Init(void) {

    QString title;
    if ( type == GFilenameDialog::LOAD )    { title = "Load session from filename:"; }
    else if (type == GFilenameDialog::SAVE) { title = "Save session to filename:"; }
    else { qFatal("Unknown prompt type"); }

    titleLabel       = new QLabel(title);
    filenameLineEdit = new QLineEdit("");
    browseButton     = new QPushButton("Browse...");
    okButton         =  new QPushButton("Ok");
    cancelButton     = new QPushButton("Cancel");
}

void GFilenameDialog::GFilenameDialogPrivate::CheckFilename(QString& filename) {
    if ( filename.endsWith(".gss") ) { return; }
    else { filename.append(".gss"); }
    okButton->setDefault(true);
}

GFilenameDialog::GFilenameDialog(PromptType type, QWidget* parent)
    : QDialog(parent)
{
    d = new GFilenameDialogPrivate;
    d->type = type;
    d->Init();

    QHBoxLayout* titleLayout = new QHBoxLayout;
    titleLayout->addWidget(d->titleLabel);

    QHBoxLayout* filenameLayout = new QHBoxLayout;
    filenameLayout->addWidget(d->filenameLineEdit);
    filenameLayout->addWidget(d->browseButton);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(d->okButton);
    buttonLayout->addWidget(d->cancelButton);
    buttonLayout->addStretch(1);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(titleLayout);
    layout->addLayout(filenameLayout);
    layout->addLayout(buttonLayout);
    setLayout(layout);

    connect(d->browseButton,     SIGNAL(clicked()),            this, SLOT(BrowseButtonClicked()));
    connect(d->okButton,         SIGNAL(clicked()),            this, SLOT(accept()));
    connect(d->cancelButton,     SIGNAL(clicked()),            this, SLOT(reject()));
}

GFilenameDialog::~GFilenameDialog(void) {
    delete d;
    d = 0;
}

const QString GFilenameDialog::Filename(void) {
    return d->filenameLineEdit->text();
}

void GFilenameDialog::BrowseButtonClicked(void) {

     QString filename = "";

    if (d->type == GFilenameDialog::LOAD) {
        QString title(tr("Load session data"));
        QString dir(".");
        QString filter("Gambit session (*.gss)");
        filename = QFileDialog::getOpenFileName((QWidget*)0, title, dir, filter);
        if ( filename.isEmpty() ) { return; }
    }

    else if ( d->type == GFilenameDialog::SAVE) {
        QString title(tr("Save session data"));
        QString dir(".");
        QString filter("Gambit session (*.gss)");
        filename = QFileDialog::getSaveFileName((QWidget*)0, title, dir, filter);
        if ( filename.isEmpty() ) { return; }
    }

    else { qFatal("Unknown prompt type"); }

    d->CheckFilename(filename);
    d->filenameLineEdit->setText(filename);
    d->filenameLineEdit->update();
}
