// ***************************************************************************
// GAssemblySettingsDialog.cpp (c) 2009 Derek Barnett
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
// Provides a dialog window for viewing/modifying settings used by
// GAssemblyView.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/AssemblyView/GAssemblySettingsDialog.h"
#include "Utilities/GTrueFalseComboDelegate.h"
using namespace Gambit;
using namespace Gambit::Utilities;
using namespace Gambit::Viewer;

const QStringList OptionNames = AlignmentOptionNames();
const QStringList ModeNames   = ViewModeNames();

// -----------------------------------------------------------------
// GAssemblySettingsForm
// -----------------------------------------------------------------

struct GAssemblySettingsForm::GAssemblySettingsFormPrivate {

    // internal data
    GVisibleAlignmentItem::AlignmentItemViewMode mode;
    AlignmentViewSettings settings;

    // GUI components
    QLabel*      optionTableLabel;
    QTableView*  optionTableView;
    QLabel*      editColorLabel;
    QPushButton* editColorButton;

    // option table view helper classes
    QStandardItemModel*      optionTableModel;
    GTrueFalseComboDelegate* comboDelegate;

    // internal methods
    void Init(void);
    void UpdateAlignmentOptions(GAlignment::GAlignmentOption option, bool set);
};

void GAssemblySettingsForm::GAssemblySettingsFormPrivate::Init(void) {

    // set up table view/model with delegate
    comboDelegate     = new GTrueFalseComboDelegate;
    int optionCount   = OptionNames.size();
    optionTableLabel  = new QLabel("Select alignment options for this view mode: ");
    optionTableModel  = new QStandardItemModel( optionCount, 2 );
    optionTableView   = new QTableView;
    optionTableView->horizontalHeader()->setHidden(true);
    optionTableView->verticalHeader()->setHidden(true);
    optionTableView->setEditTriggers(QAbstractItemView::CurrentChanged |
                                     QAbstractItemView::DoubleClicked  |
                                     QAbstractItemView::SelectedClicked);
    optionTableView->setModel(optionTableModel);
    optionTableView->setItemDelegateForColumn(1, comboDelegate);

    // populate table model with current data
    for(int i = 0; i < optionCount; ++i ) {

        QString label = OptionNames.at(i);
        QModelIndex index;
        QStandardItem* item;

        // insert option label
        index = optionTableModel->index(i, 0, QModelIndex());
        optionTableModel->setData(index, label, Qt::DisplayRole);
        item = optionTableModel->itemFromIndex(index);
        item->setEditable(false);
        item->setSelectable(false);

        // insert true/false entry (selectable through combo box delegate)
        index = optionTableModel->index(i, 1, QModelIndex());

        // set initial true/false entry (based on current alignment flag selection)
        QString boolText;
        switch(i) {
            case(0) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::Paired) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            case(1) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::ProperPair) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            case(2) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::Unmapped) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            case(3) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::MateUnmapped) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            case(4) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::ReverseStrand) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            case(5) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::MateReverseStrand) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            case(6) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::Read1) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            case(7) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::Read2) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            case(8) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::Secondary) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            case(9) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::QcFailed) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            case(10) :
                boolText = QString( ((settings.alignmentFlag & GAlignment::Duplicate) != 0) ? "true" : "false" );
                optionTableModel->setData(index, boolText, Qt::EditRole);
                break;
            default :
                qDebug() << "Unknown alignment option flag"; qApp->exit(1);
        }

        // set item attributes
        item = optionTableModel->itemFromIndex(index);
        item->setSelectable(true);
        item->setEditable(true);
    }

    // set up color editing components
    editColorLabel  = new QLabel("Press button to edit colors for this view mode: ");
    editColorButton = new QPushButton(QIcon(":/icons/colors"), "Edit Colors...");
    editColorButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void GAssemblySettingsForm::GAssemblySettingsFormPrivate::UpdateAlignmentOptions(GAlignment::GAlignmentOption option, bool set) {
    if ( set ) {
        settings.alignmentFlag |= option;  // set flag
    } else {
        settings.alignmentFlag &= ~option; // clear flag
    }
}

GAssemblySettingsForm::GAssemblySettingsForm(const GVisibleAlignmentItem::AlignmentItemViewMode& mode,
                                             AlignmentViewSettings& settings,
                                             QWidget* parent)
    : QWidget(parent)
{
    d = new GAssemblySettingsFormPrivate;
    d->mode = mode;
    d->settings = settings;
    d->Init();

    // make connections
    connect(d->optionTableModel, SIGNAL(itemChanged(QStandardItem*)),
            this,                SLOT(AlignmentOptionsChanged(QStandardItem*)));
    connect(d->editColorButton,  SIGNAL(clicked()),
            this,                SLOT(EditColorScheme()));

    // set up layout
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(d->optionTableLabel);
    layout->addWidget(d->optionTableView);
    layout->addStretch(2);
    layout->addWidget(d->editColorLabel);
    layout->addWidget(d->editColorButton);
    setLayout(layout);
}

GAssemblySettingsForm::~GAssemblySettingsForm(void) {
    delete d;
    d = 0;
}

const GAlignment::GAlignmentOptions& GAssemblySettingsForm::AlignmentOptions(void) {
    return d->settings.alignmentFlag;
}

const GColorScheme& GAssemblySettingsForm::ColorScheme(void) {
    return d->settings.currentColorScheme;
}

void GAssemblySettingsForm::AlignmentOptionsChanged(QStandardItem* item) {

    // get item flag data
    QString text = item->data(Qt::EditRole).toString();
    bool flagSet = (text == "true");

    // update settings
    int row = item->row();
    switch(row) {
        case(0) : d->UpdateAlignmentOptions(GAlignment::Paired, flagSet); break;
        case(1) : d->UpdateAlignmentOptions(GAlignment::ProperPair, flagSet); break;
        case(2) : d->UpdateAlignmentOptions(GAlignment::Unmapped, flagSet); break;
        case(3) : d->UpdateAlignmentOptions(GAlignment::MateUnmapped, flagSet); break;
        case(4) : d->UpdateAlignmentOptions(GAlignment::ReverseStrand, flagSet); break;
        case(5) : d->UpdateAlignmentOptions(GAlignment::MateReverseStrand, flagSet); break;
        case(6) : d->UpdateAlignmentOptions(GAlignment::Read1, flagSet); break;
        case(7) : d->UpdateAlignmentOptions(GAlignment::Read2, flagSet); break;
        case(8) : d->UpdateAlignmentOptions(GAlignment::Secondary, flagSet); break;
        case(9) : d->UpdateAlignmentOptions(GAlignment::QcFailed, flagSet); break;
        case(10): d->UpdateAlignmentOptions(GAlignment::Duplicate, flagSet); break;
        default : qDebug() << "Unknown alignment flag option"; qApp->exit(1);
    }

    // emit changed signal
    emit SettingsChanged(d->mode, d->settings);
}

void GAssemblySettingsForm::EditColorScheme(void) {

    // startup color scheme editor dialog
    GColorSchemeDialog dialog(d->settings);

    // if accepted
    if ( dialog.exec() == QDialog::Accepted ) {

        // update local settings
        d->settings = dialog.Settings();

        // emit changed signal
        emit SettingsChanged(d->mode, d->settings);
    }
}

// -----------------------------------------------------------------
// GAssemblySettingsDialog
// -----------------------------------------------------------------

struct GAssemblySettingsDialog::GAssemblySettingsDialogPrivate {

    GViewSettingsMap  settings;
    QTabWidget*       tabWidget;
    QDialogButtonBox* buttonBox;

    void Init(void);
};

void GAssemblySettingsDialog::GAssemblySettingsDialogPrivate::Init(void) {

    // set up tab widget based on current settings
    tabWidget = new QTabWidget;

    // set up button box
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
}

GAssemblySettingsDialog::GAssemblySettingsDialog(GViewSettingsMap& settings, QWidget* parent)
    : QDialog(parent)
{
    d = new GAssemblySettingsDialogPrivate;
    d->settings = settings;
    d->Init();
    PopulateForms();

    // make connections
    connect(d->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    // set up dialog layout
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(d->tabWidget);
    layout->addWidget(d->buttonBox);
    setLayout(layout);

    // set dialog attributes
    setWindowTitle("Edit View Options");
}

GAssemblySettingsDialog::~GAssemblySettingsDialog(void) {
    delete d;
    d = 0;
}

void GAssemblySettingsDialog::PopulateForms(void) {

    QMapIterator<GVisibleAlignmentItem::AlignmentItemViewMode, AlignmentViewSettings> i(d->settings);
    while (i.hasNext()) {
        i.next();

        // get mode & settings from iterator, skip normal (no option rules for 'normal')
        GVisibleAlignmentItem::AlignmentItemViewMode mode = i.key();
        if ( mode == GVisibleAlignmentItem::Normal ) { continue; }
        AlignmentViewSettings settings = i.value();
        QString label = ModeNames.at((int)mode);

        // create form, add to tab, connect changed signal to update slot
        GAssemblySettingsForm* form = new GAssemblySettingsForm(mode, settings);
        d->tabWidget->addTab(form, label);
        connect(form, SIGNAL(SettingsChanged(GVisibleAlignmentItem::AlignmentItemViewMode,AlignmentViewSettings&)),
                this, SLOT(UpdateSettings(GVisibleAlignmentItem::AlignmentItemViewMode,AlignmentViewSettings&)));
    }
}

const GViewSettingsMap GAssemblySettingsDialog::Settings(void) {
    return d->settings;
}

void GAssemblySettingsDialog::UpdateSettings(const GVisibleAlignmentItem::AlignmentItemViewMode& mode,
                                             AlignmentViewSettings& settings)
{
    AlignmentViewSettings& currentModeSettings = d->settings[mode];
    currentModeSettings = settings;
}

// -----------------------------------------------------------------
// GColorSchemeDialog
// -----------------------------------------------------------------

struct GColorSchemeDialog::GColorSchemeDialogPrivate {

    AlignmentViewSettings settings;
    QLabel*           instructionLabel;
    QTableWidget*     colorTable;
    QPushButton*      restoreDefaultButton;
    QDialogButtonBox* buttonBox;

    void Init(void);
    void PopulateTable(void);
};

void GColorSchemeDialog::GColorSchemeDialogPrivate::Init(void) {

    instructionLabel = new QLabel("Double-click entry to edit color");

    colorTable = new QTableWidget(5, 1);
    colorTable->horizontalHeader()->setHidden(true);
    colorTable->verticalHeader()->setHidden(true);
    PopulateTable();
    colorTable->setColumnWidth(0, 200);

    restoreDefaultButton = new QPushButton("Restore Defaults");
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
}

void GColorSchemeDialog::GColorSchemeDialogPrivate::PopulateTable(void) {

    QTableWidgetItem* item;

    // ForwardBackground
    item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, "Forward Background");
    item->setData(Qt::DecorationRole, settings.currentColorScheme.ForwardBackground);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    colorTable->setItem(0,0,item);

    // ReverseBackground
    item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, "Reverse Background");
    item->setData(Qt::DecorationRole, settings.currentColorScheme.ReverseBackground);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    colorTable->setItem(1,0,item);

    // NormalText
    item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, "Normal Base Color");
    item->setData(Qt::DecorationRole, settings.currentColorScheme.NormalText);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    colorTable->setItem(2,0,item);

    // MismatchText
    item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, "Mismatched Base Color");
    item->setData(Qt::DecorationRole, settings.currentColorScheme.MismatchText);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    colorTable->setItem(3,0,item);

    // PaddingText
    item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, "Padding Color");
    item->setData(Qt::DecorationRole, settings.currentColorScheme.PaddingText);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    colorTable->setItem(4,0,item);
}

GColorSchemeDialog::GColorSchemeDialog(AlignmentViewSettings& settings, QWidget* parent)
    : QDialog(parent)
{
    d = new GColorSchemeDialogPrivate;
    d->settings = settings;
    d->Init();

    // make connections
    connect(d->colorTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(EditColorForItem(QTableWidgetItem*)));
    connect(d->restoreDefaultButton, SIGNAL(clicked()), this, SLOT(RestoreDefault()));
    connect(d->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    // set up dialog layout
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(d->instructionLabel);
    layout->addWidget(d->colorTable);
    layout->addStretch(1);
    layout->addWidget(d->restoreDefaultButton);
    layout->addStretch(1);
    layout->addWidget(d->buttonBox);
    setLayout(layout);

    // set dialog attributes
    setWindowTitle("Edit Color Scheme");
}

GColorSchemeDialog::~GColorSchemeDialog(void) {
    delete d;
    d = 0;
}

const AlignmentViewSettings& GColorSchemeDialog::Settings(void) {
    return d->settings;
}

void GColorSchemeDialog::EditColorForItem(QTableWidgetItem* item) {
    QColor selectedColor = QColorDialog::getColor(QColor(), this, "Select Color", QColorDialog::ShowAlphaChannel);
    if ( selectedColor.isValid() ) {

        // update table view data
        item->setData(Qt::DecorationRole, selectedColor);

        // update settings
        int row = item->row();
        switch(row) {
            case(0) : d->settings.currentColorScheme.ForwardBackground = selectedColor; break;
            case(1) : d->settings.currentColorScheme.ReverseBackground = selectedColor; break;
            case(2) : d->settings.currentColorScheme.NormalText        = selectedColor; break;
            case(3) : d->settings.currentColorScheme.MismatchText      = selectedColor; break;
            case(4) : d->settings.currentColorScheme.PaddingText       = selectedColor; break;
            default : qDebug() << "Unknown color scheme option"; qApp->exit(1);
        }
    }
}

void GColorSchemeDialog::RestoreDefault(void) {

    QTableWidgetItem* item;

    // ForwardBackground
    item = d->colorTable->item(0, 0);
    item->setData(Qt::DecorationRole, d->settings.defaultColorScheme.ForwardBackground);
    d->settings.currentColorScheme.ForwardBackground = d->settings.defaultColorScheme.ForwardBackground;

    // ReverseBackground
    item = d->colorTable->item(1, 0);
    item->setData(Qt::DecorationRole, d->settings.defaultColorScheme.ReverseBackground);
    d->settings.currentColorScheme.ReverseBackground = d->settings.defaultColorScheme.ReverseBackground;

    // NormalText
    item = d->colorTable->item(2, 0);
    item->setData(Qt::DecorationRole, d->settings.defaultColorScheme.NormalText);
    d->settings.currentColorScheme.NormalText = d->settings.defaultColorScheme.NormalText;

    // MismatchText
    item = d->colorTable->item(3, 0);
    item->setData(Qt::DecorationRole, d->settings.defaultColorScheme.MismatchText);
    d->settings.currentColorScheme.MismatchText = d->settings.defaultColorScheme.MismatchText;

    // PaddingText
    item = d->colorTable->item(4, 0);
    item->setData(Qt::DecorationRole, d->settings.defaultColorScheme.PaddingText);
    d->settings.currentColorScheme.PaddingText = d->settings.defaultColorScheme.PaddingText;
}
