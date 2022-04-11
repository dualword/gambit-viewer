// ***************************************************************************
// GOpenFilesWidget.cpp (c) 2009 Derek Barnett
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
// Describes the main widget used (typically will be embedded in dialog) to
// specify which files to open.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "SessionManager/FileManager/GOpenFilesWidget.h"
using namespace Gambit;

struct GOpenFilesWidget::GOpenFilesWidgetPrivate {

    QTabWidget*   formsTabWidget;
    QTreeWidget*  filesView;
    GFileInfoList files;

    void Init(void);
    void CreateFilesView(void);

    static QStringList InitFileTypes(void);
    static const QStringList FileTypes;
};

QStringList GOpenFilesWidget::GOpenFilesWidgetPrivate::InitFileTypes(void) {
    QStringList types;
    types << "Alignment" << "Gene" << "Reference" << "Snp";
    return types;
}

const QStringList
GOpenFilesWidget::GOpenFilesWidgetPrivate::FileTypes = GOpenFilesWidget::GOpenFilesWidgetPrivate::InitFileTypes();

void GOpenFilesWidget::GOpenFilesWidgetPrivate::Init(void) {

    // set up tab widget
    formsTabWidget = new QTabWidget;

    // set up file view
    filesView = new QTreeWidget;
    filesView->setAllColumnsShowFocus(true);
    filesView->setAnimated(true);
    filesView->setMinimumWidth(300);
    filesView->setHeaderLabel("Summary");
    filesView->setMinimumHeight(200);
    filesView->setRootIsDecorated(true);
    filesView->setSortingEnabled(false);
    filesView->setTextElideMode(Qt::ElideMiddle);
    filesView->setUniformRowHeights(true);
    filesView->setSelectionMode(QAbstractItemView::SingleSelection);
}

void GOpenFilesWidget::GOpenFilesWidgetPrivate::CreateFilesView(void) {

    // get item count
    int count = GOpenFilesWidget::GOpenFilesWidgetPrivate::FileTypes.size();

    // create item
    QList<QTreeWidgetItem*> items;
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(GOpenFilesWidget::GOpenFilesWidgetPrivate::FileTypes.at(i)));
        QFont itemFont = item->font(0);
        itemFont.setWeight(QFont::Bold);
        item->setFont(0, itemFont);
        items.append(item);
    }

    // put items in filesView
    filesView->insertTopLevelItems(0, items);
}

GOpenFilesWidget::GOpenFilesWidget(const GFileFormatList& formats, QWidget* parent)
    : QFrame(parent)
{
    d = new GOpenFilesWidgetPrivate;
    d->Init();

    setMinimumWidth(450);
    connect(d->filesView, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,        SLOT(ShowTabForItem(QTreeWidgetItem*, int)));
    d->CreateFilesView();
    CreateFormsTab(formats);

    QSplitter* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(d->filesView);
    splitter->addWidget(d->formsTabWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(splitter);
    setLayout(layout);
}

GOpenFilesWidget::~GOpenFilesWidget(void) {
    delete d;
    d = 0;
}

void GOpenFilesWidget::CreateFormsTab(const GFileFormatList& formats) {

    GFileFormatList alignmentFormats;
    GFileFormatList geneFormats;
    GFileFormatList referenceFormats;
    GFileFormatList snpFormats;

    // get supported for each file type
    foreach (GFileFormatData formatData, formats) {
        foreach(GFileInfo::FileType type, formatData.Types) {
            switch(type) {
               case(GFileInfo::File_Alignment) : alignmentFormats.append(formatData); break;
               case(GFileInfo::File_Gene)      : geneFormats.append(formatData); break;
               case(GFileInfo::File_Reference) : referenceFormats.append(formatData); break;
               case(GFileInfo::File_Snp)       : snpFormats.append(formatData); break;
               default : break;
            }
        }
    }

    // create forms and add to form tab widget
    GOpenFilesForm* form;
    form = new GOpenFilesForm(GFileInfo::File_Alignment, alignmentFormats);
    connect(form, SIGNAL(DirectoryChanged(QDir)),  this, SLOT(SetDir(QDir)) );
    connect(form, SIGNAL(FileSelected(GFileInfo)), this, SLOT(AddFile(GFileInfo)) );
    d->formsTabWidget->addTab(form, "Alignment");

    form = new GOpenFilesForm(GFileInfo::File_Gene, geneFormats);
    connect(form, SIGNAL(DirectoryChanged(QDir)),  this, SLOT(SetDir(QDir)) );
    connect(form, SIGNAL(FileSelected(GFileInfo)), this, SLOT(AddFile(GFileInfo)) );
    d->formsTabWidget->addTab(form, "Gene");

    form = new GOpenFilesForm(GFileInfo::File_Reference, referenceFormats);
    connect(form, SIGNAL(DirectoryChanged(QDir)),  this, SLOT(SetDir(QDir)) );
    connect(form, SIGNAL(FileSelected(GFileInfo)), this, SLOT(AddFile(GFileInfo)) );
    d->formsTabWidget->addTab(form, "Reference");

    form = new GOpenFilesForm(GFileInfo::File_Snp, snpFormats);
    connect(form, SIGNAL(DirectoryChanged(QDir)),  this, SLOT(SetDir(QDir)) );
    connect(form, SIGNAL(FileSelected(GFileInfo)), this, SLOT(AddFile(GFileInfo)) );
    d->formsTabWidget->addTab(form, "Snp");

    // set initial tab to 'Alignments' tab
    d->formsTabWidget->setCurrentIndex(0);
}

GFileInfoList GOpenFilesWidget::SelectedFiles(void) const {
    return d->files;
}

void GOpenFilesWidget::ShowTabForItem(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    if ( item == NULL ) { return; }
    d->formsTabWidget->setCurrentIndex( d->filesView->indexOfTopLevelItem(item) );
}

void GOpenFilesWidget::AddFile(const GFileInfo& fileInfo) {

    // save file entry
    d->files.append(fileInfo);

    // update view
    QTreeWidgetItem* parentItem = 0;

    int index = 0;
    int fileTypeCount = d->filesView->topLevelItemCount();

    for ( ; index < fileTypeCount; ++index ) {
        parentItem = d->filesView->topLevelItem(index);
        if (parentItem->text(0) == GOpenFilesWidget::GOpenFilesWidgetPrivate::FileTypes.at(fileInfo.Type)) { break; }
    }

    if (index == fileTypeCount) { return; }

    QTreeWidgetItem* newFileItem = new QTreeWidgetItem(parentItem);
    newFileItem->setText(0, fileInfo.Filename);
    newFileItem->setFlags(Qt::ItemIsEnabled);

    d->filesView->expandItem(parentItem);
    d->filesView->resizeColumnToContents(0);

    emit FilesModified(d->files);
}

void GOpenFilesWidget::SetDir(QDir dir) {
    for(int index = 0; index < d->formsTabWidget->count(); ++index) {
        GOpenFilesForm* form = (GOpenFilesForm*)d->formsTabWidget->widget(index);
        form->setDir(dir);
    }
}

struct GOpenFilesForm::GOpenFilesFormPrivate {

    // file type info
    GFileInfo::FileType type;
    GFileFormatList     formats;

    // form components
    QLabel*       titleLabel;
    QLabel*       formatLabel;
    QComboBox*    formatComboBox;
    QLabel*       filenameLabel;
    QLineEdit*    filenameLineEdit;
    QPushButton*  fileBrowseButton;
    QLabel*       indexLabel;
    QRadioButton* indexGenerateRadio;
    QRadioButton* indexUseRadio;
    QButtonGroup* indexButtonGroup;
    QLineEdit*    indexLineEdit;
    QPushButton*  indexBrowseButton;
    QLabel*       readGroupLabel;
    QRadioButton* autoDetectReadGroupRadio;
    QRadioButton* manualDefineReadGroupRadio;
    QButtonGroup* readGroupButtonGroup;
    QPushButton*  applyButton;
    QPushButton*  cancelFormButton;

    // form completion state flags
    bool isUsingIndex;
    bool isFileOk;
    bool isIndexOk;
    bool isAlignmentForm;

    // directory info
    QDir currentDir;

    void Init(GFileInfo::FileType type, const GFileFormatList& formats);
    void CheckForm(void);
    void ClearForm(void);
    void ShowIndexElements(bool);
};

void GOpenFilesForm::GOpenFilesFormPrivate::Init(GFileInfo::FileType someType, const GFileFormatList& someFormats) {

    // initialize internal data
    type                      = someType;
    formats                   = someFormats;
    isUsingIndex              = false;
    isIndexOk                 = false;
    isAlignmentForm           = ( type == GFileInfo::File_Alignment );
    currentDir                = QDir(".");

    // set up title label
    titleLabel = new QLabel;
    switch(type) {
        case(GFileInfo::File_Alignment) : titleLabel->setText("<b>Select alignment file (and index)</b>"); break;
        case(GFileInfo::File_Gene)      : titleLabel->setText("<b>Select gene annotation file</b>"); break;
        case(GFileInfo::File_Reference) : titleLabel->setText("<b>Select reference sequence file (and index)</b>"); break;
        case(GFileInfo::File_Snp)       : titleLabel->setText("<b>Select SNP annotation file</b>"); break;
        default                         : qFatal("GOpenFilesForm::GOpenFilesForm(ctor) => unknown file type");
    }

    // set up format selection
    formatLabel    = new QLabel("Format: ");
    formatComboBox = new QComboBox;

    // set up filename entry components
    filenameLabel    = new QLabel("File:");
    filenameLineEdit = new QLineEdit;
    fileBrowseButton = new QPushButton("Browse...");

    // set up index selection components
    indexLabel         = new QLabel(tr("Index:"));
    indexGenerateRadio = new QRadioButton(tr("Generate new index"));
    indexUseRadio      = new QRadioButton(tr("Use existing index"));
    indexLineEdit      = new QLineEdit("");
    indexBrowseButton  = new QPushButton("Browse...");

    // set up button groups
    indexButtonGroup = new QButtonGroup;
    indexButtonGroup->addButton(indexGenerateRadio);
    indexButtonGroup->addButton(indexUseRadio);
    indexUseRadio->setChecked(true);

    // set up form buttons
    applyButton = new QPushButton(QIcon(":/icons/accept"), QString("Accept"));
    applyButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    applyButton->setEnabled(false);

    cancelFormButton = new QPushButton(QIcon(":/icons/cancel"), QString("Cancel"));
    cancelFormButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void GOpenFilesForm::GOpenFilesFormPrivate::CheckForm(void) {
    bool isFormComplete = (isFileOk && (!isUsingIndex || isIndexOk));
    applyButton->setEnabled(isFormComplete);
}

void GOpenFilesForm::GOpenFilesFormPrivate::ClearForm(void) {
    if (isUsingIndex) { indexLineEdit->clear(); }
    filenameLineEdit->clear();
}

void GOpenFilesForm::GOpenFilesFormPrivate::ShowIndexElements(bool ok) {
    indexLabel->setVisible(ok);
    indexGenerateRadio->setVisible(ok);
    indexUseRadio->setVisible(ok);
    indexLineEdit->setVisible(ok);
    indexBrowseButton->setVisible(ok);
}

GOpenFilesForm::GOpenFilesForm(GFileInfo::FileType type, const GFileFormatList& formats, QWidget* parent)
    : QWidget(parent)
{
    d = new GOpenFilesFormPrivate;
    d->Init(type, formats);

    // set up format selection area
    QHBoxLayout* formatLayout = new QHBoxLayout;
    formatLayout->addWidget(d->formatLabel);
    formatLayout->addWidget(d->formatComboBox);
    formatLayout->addStretch(1);

    // set up filename selection area
    QHBoxLayout* filenameLayout = new QHBoxLayout;
    filenameLayout->addWidget(d->filenameLabel);
    filenameLayout->addWidget(d->filenameLineEdit);
    filenameLayout->addWidget(d->fileBrowseButton);

    // set up main widget layout
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(d->titleLabel, 1, Qt::AlignTop);
    mainLayout->addLayout(formatLayout);
    mainLayout->addLayout(filenameLayout);
    mainLayout->addStretch(2);

    // set up index filename selection area
    QVBoxLayout* indexHeaderLayout = new QVBoxLayout;
    indexHeaderLayout->addWidget(d->indexLabel, 1, Qt::AlignLeft);
    indexHeaderLayout->addWidget(d->indexGenerateRadio, 1, Qt::AlignLeft);
    indexHeaderLayout->addWidget(d->indexUseRadio, 1, Qt::AlignLeft);

    QHBoxLayout* indexFilenameLayout = new QHBoxLayout;
    indexFilenameLayout->addWidget(d->indexLineEdit);
    indexFilenameLayout->addWidget(d->indexBrowseButton);

    // add index elements to main widget layout
    mainLayout->addLayout(indexHeaderLayout);
    mainLayout->addLayout(indexFilenameLayout);
    mainLayout->addStretch(2);

    // set up form button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(4);
    buttonLayout->addWidget(d->applyButton, 1, Qt::AlignCenter);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(d->cancelFormButton, 1,  Qt::AlignCenter);
    buttonLayout->addStretch(4);

    // add form button to main widget layou
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(2);
    setLayout(mainLayout);

    // make connections
    connect(d->formatComboBox,             SIGNAL(currentIndexChanged(int)), this, SLOT(SelectedFormatChanged(int)));
    connect(d->filenameLineEdit,           SIGNAL(textChanged(QString)),     this, SLOT(FilenameLineEditChanged(QString)) );
    connect(d->fileBrowseButton,           SIGNAL(clicked()),                this, SLOT(FileBrowseButtonPressed(void)) );
    connect(d->indexGenerateRadio,         SIGNAL(toggled(bool)),            this, SLOT(GenerateIndexToggled(bool)));
    connect(d->indexUseRadio,              SIGNAL(toggled(bool)),            this, SLOT(UseIndexToggled(bool)));
    connect(d->indexLineEdit,              SIGNAL(textChanged(QString)),     this, SLOT(IndexLineEditChanged(QString)));
    connect(d->indexBrowseButton,          SIGNAL(clicked()),                this, SLOT(IndexBrowseButtonPressed(void)));
    connect(d->applyButton,                SIGNAL(clicked()),                this, SLOT(ApplyButtonPressed()));
    connect(d->cancelFormButton,           SIGNAL(clicked()),                this, SLOT(CancelButtonPressed()));

    // intialize index view state
    d->ShowIndexElements(false);

    // populate format combo box
    QStringList formatNames;
    foreach(GFileFormatData formatData, formats) {
        formatNames.append( formatData.Name );
    }
    d->formatComboBox->addItems(formatNames);
    d->formatComboBox->setCurrentIndex(0);
}

GOpenFilesForm::~GOpenFilesForm(void) {
    delete d;
    d = 0;
}

void GOpenFilesForm::setDir(QDir dir) {
    if (d->currentDir != dir) { d->currentDir = dir; }
}

void GOpenFilesForm::FileBrowseButtonPressed(void) {

    // setup file dialog
    GFileFormatData currentFormat = d->formats.at( d->formatComboBox->currentIndex() );

    // set up file dialog
    QString title  = tr("Open File");
    QString filter = tr("%1 files(").arg(currentFormat.Name);
    foreach (QString aFilter, currentFormat.Extensions) {
        filter.append(aFilter);
        filter.append(" ");
    }
    filter.append(")");

     // get filename from dialog
    QStringList filenames;
    QFileDialog fileDialog(this);
    fileDialog.setWindowTitle(title);
    fileDialog.setDirectory(d->currentDir);
    fileDialog.setNameFilter(filter);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    if (fileDialog.exec()) {
        filenames = fileDialog.selectedFiles();
        emit DirectoryChanged(fileDialog.directory());
        if (filenames.size() == 1) {
            d->filenameLineEdit->setText( filenames.takeFirst() );
        }
    }
}

void GOpenFilesForm::FilenameLineEditChanged(QString text) {

    d->isFileOk = ( !text.isEmpty() );

    // auto-append index filetype for formats that used an index
    if (text.endsWith(".bam")) {
        text += ".bai";
    } else if (text.endsWith(".fasta") || text.endsWith(".fa")) {
        text += ".fai";
    }

    // otherwise, check for completed form and return
    else {
        d->CheckForm();
        return;
    }

    // if auto-named file exists in current directory, set index line edit
    QFileInfo fileInfo(text);
    if ( fileInfo.exists() ) {
        d->indexLineEdit->setText(text);
    }

    // check for complete form
    d->CheckForm();
}

void GOpenFilesForm::GenerateIndexToggled(bool checked) {
    if (checked) {
        d->indexLineEdit->clear();
        d->indexLineEdit->setEnabled(false);
        d->indexBrowseButton->setEnabled(false);
        d->isIndexOk = true;
        d->CheckForm();
    }
}

void GOpenFilesForm::IndexBrowseButtonPressed(void) {

    GFileFormatData currentFormat = d->formats.at( d->formatComboBox->currentIndex() );

    // set up file dialog
    QString title  = tr("Open Index File");
    QString filter = tr("%1 files(").arg(currentFormat.Name);
    foreach (QString aFilter, currentFormat.IndexExtensions) {
        filter.append(aFilter);
        filter.append(" ");
    }
    filter.append(")");

    // get filename from dialog
    QStringList filenames;
    QFileDialog fileDialog(this);
    fileDialog.setWindowTitle(title);
    fileDialog.setDirectory(d->currentDir);
    fileDialog.setNameFilter(filter);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    if (fileDialog.exec()) {
        filenames = fileDialog.selectedFiles();
        emit DirectoryChanged(fileDialog.directory());
        if (filenames.size() == 1) {
            d->indexLineEdit->setText( filenames.takeFirst() );
        }
    }
}

void GOpenFilesForm::IndexLineEditChanged(QString text) {
    d->isIndexOk = ( !text.isEmpty() );
    d->CheckForm();
}

void GOpenFilesForm::UseIndexToggled(bool checked) {
    if (checked) {
        d->indexLineEdit->setEnabled(true);
        d->indexBrowseButton->setEnabled(true);
        d->isIndexOk = false;
        d->CheckForm();
    }
}

void GOpenFilesForm::ApplyButtonPressed(void) {

    // get file info from form
    GFileInfo::FileType type = d->type;
    QString format           = d->formatComboBox->currentText();
    QString filename         = d->filenameLineEdit->text();

    QString indexFilename  = "";
    if ( d->isUsingIndex ) { indexFilename = d->indexLineEdit->text(); }

    // clear form
    d->ClearForm();

    // signal new file info 'packet' to save
    emit FileSelected( GFileInfo(type, format, filename, indexFilename) );
}

void GOpenFilesForm::CancelButtonPressed(void) {
    d->ClearForm();
    emit FormCanceled();
}

void GOpenFilesForm::SelectedFormatChanged(int index) {
    GFileFormatData format = d->formats.at(index);
    d->isUsingIndex = format.UsesIndex;
    d->ShowIndexElements( d->isUsingIndex );
}
