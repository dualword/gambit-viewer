// ***************************************************************************
// GAssemblySettingsManager.cpp (c) 2009 Derek Barnett
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
// Manages settings used by the GAssemblyView.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/AssemblyView/GAssemblySettingsManager.h"
#include "DataStructures/GColorScheme.h"
#include "DataStructures/GGenomicDataSet.h"
#include "Viewer/AssemblyView/GVisibleAlignmentGroup.h"
#include "Viewer/AssemblyView/GAssemblySettingsDialog.h"
using namespace Gambit;
using namespace Gambit::Viewer;

struct GAssemblySettingsManager::GAssemblySettingsManagerPrivate {

    QAction* mergeAction;
    QAction* editAction;
    QAction* showBasesAction;

    bool isGroupsMerged;
    GViewSettingsMap viewModes;
    bool isBasesVisible;

    void Init(void);
    void SetGroupColorScheme(GVisibleAlignmentGroup* visibleGroup);
};

void GAssemblySettingsManager::GAssemblySettingsManagerPrivate::Init(void) {

    AlignmentViewSettings settings;
    QColor color;

    // set up 'Normal' mode defaults
    settings.alignmentFlag = 0;
    color.setNamedColor("#003366");
    color.setAlpha(255);
    settings.currentColorScheme.ForwardBackground = color;
    color.setAlpha(150);
    settings.currentColorScheme.ReverseBackground = color;
    settings.currentColorScheme.NormalText   = Qt::white;
    settings.currentColorScheme.NormalText.setAlpha(150);
    settings.currentColorScheme.MismatchText = Qt::red;
    settings.currentColorScheme.PaddingText  = Qt::yellow;
    settings.defaultColorScheme = settings.currentColorScheme;
    viewModes.insert( GVisibleAlignmentItem::Normal, settings);

    // set up 'Dimmed' mode defaults
    settings.alignmentFlag = 0;
    color.setNamedColor("black");
    settings.currentColorScheme.ForwardBackground = color;
    color.setNamedColor("gainsboro");
    color.setAlpha(100);
    settings.currentColorScheme.ReverseBackground = color;
    settings.currentColorScheme.NormalText   = Qt::white;
    settings.currentColorScheme.MismatchText = Qt::red;
    settings.currentColorScheme.PaddingText  = Qt::yellow;
    settings.defaultColorScheme = settings.currentColorScheme;
    viewModes.insert( GVisibleAlignmentItem::Dimmed, settings );

    // set up 'Highlighted' mode
    settings.alignmentFlag = 0;
    color.setNamedColor("#FFCC33");
    settings.currentColorScheme.ForwardBackground = color;
    color.setAlpha(150);
    settings.currentColorScheme.ReverseBackground = color;
    settings.currentColorScheme.NormalText   = Qt::black;
    settings.currentColorScheme.MismatchText = Qt::red;
    settings.currentColorScheme.PaddingText  = Qt::blue;
    settings.defaultColorScheme = settings.currentColorScheme;
    viewModes.insert( GVisibleAlignmentItem::Highlighted, settings );

    // initialize merge flag
    isGroupsMerged = false;

    // initialize show bases flag
    isBasesVisible = false;

    // set up 'merge groups' action
    mergeAction = new QAction("Merge Groups", (QObject*)0);
    mergeAction->setCheckable(true);

    // set up 'edit view settings' action
    editAction = new QAction("Edit View Settings", (QObject*)0);
    editAction->setCheckable(false);

    // set up 'show bases' action
    showBasesAction = new QAction("Show Bases", (QObject*)0);
    showBasesAction->setCheckable(true);
}

void GAssemblySettingsManager::GAssemblySettingsManagerPrivate::SetGroupColorScheme(GVisibleAlignmentGroup* group) {

    // skip if invalid group
    if ( group == 0 ) { return; }

    // get current settings for the various supported view modes
    // Flag preference: dimmed > highlight > normal
    AlignmentViewSettings& normalSettings    = viewModes[GVisibleAlignmentItem::Normal];
    AlignmentViewSettings& dimSettings       = viewModes[GVisibleAlignmentItem::Dimmed];
    AlignmentViewSettings& highlightSettings = viewModes[GVisibleAlignmentItem::Highlighted];

    // set group header color scheme to current 'normal'
    group->SetColorScheme(normalSettings.currentColorScheme);

    QList<GVisibleAlignmentItem*> items = group->AlignmentItems();
    foreach(GVisibleAlignmentItem* item, items) {
        if ( item == 0 ) { continue; }

        // get current alignment flag for item
        const GAlignment& alignment = item->alignment();
        const GAlignment::GAlignmentOptions flag = alignment.Flags;

        // check flag against dim settings
        if ( (flag & dimSettings.alignmentFlag) != 0 ) {
            item->SetColorScheme(dimSettings.currentColorScheme);
        }

        // check highlight flag
        else if ( (flag & highlightSettings.alignmentFlag) != 0 ) {
            item->SetColorScheme(highlightSettings.currentColorScheme);
        }

        // else normal
        else {
            item->SetColorScheme(normalSettings.currentColorScheme);
        }
    }
}

GAssemblySettingsManager::GAssemblySettingsManager(QObject* parent)
    : QObject(parent)
{
    d = new GAssemblySettingsManagerPrivate;
    d->Init();

    // make connections
    connect(d->mergeAction, SIGNAL(toggled(bool)), this, SLOT(SetMergeGroups(bool)));
    connect(d->editAction,  SIGNAL(triggered()),   this, SLOT(EditSettings()));
    connect(d->showBasesAction, SIGNAL(toggled(bool)), this, SLOT(SetShowBases(bool)));
}

GAssemblySettingsManager::~GAssemblySettingsManager(void) {
    delete d;
    d = 0;
}

QList<QAction*> GAssemblySettingsManager::Actions(void) {
    QList<QAction*> actions;
    actions << d->mergeAction
            << d->editAction
            << d->showBasesAction;
    return actions;
}

bool GAssemblySettingsManager::IsGroupsMerged(void) const {
    return d->isGroupsMerged;
}

bool GAssemblySettingsManager::IsBasesVisible(void) const {
    return d->isBasesVisible;
}

void GAssemblySettingsManager::SetAlignmentFlag(const GVisibleAlignmentItem::AlignmentItemViewMode& mode, const Gambit::GAlignment::GAlignmentOption& option, bool ok) {
    GAlignment::GAlignmentOptions& flag = d->viewModes[mode].alignmentFlag;
    if (ok) flag |= option;  // set flag
    else    flag &= ~option; // clear flag
}

void GAssemblySettingsManager::SetGroupColorScheme(GVisibleAlignmentGroup* visibleGroup) {
    if ( visibleGroup == NULL ) { return; }
    d->SetGroupColorScheme(visibleGroup);
}

const GColorScheme& GAssemblySettingsManager::NormalColorScheme(void) {
    return d->viewModes[GVisibleAlignmentItem::Normal].currentColorScheme;
}

void GAssemblySettingsManager::SetMergeGroups(bool ok) {

    // set flag
    d->isGroupsMerged = ok;

    // toggle action label
    QString label = "Merge Groups";
    if ( ok ) { label = "Split Groups"; }
    d->mergeAction->setText(label);

    // emit change signal
    emit MergeGroupsChanged(ok);
}

void GAssemblySettingsManager::EditSettings(void) {
    GAssemblySettingsDialog dialog(d->viewModes);    
    if ( dialog.exec() == QDialog::Accepted ) {
        d->viewModes = dialog.Settings();
        emit ViewSettingsChanged();
    }
}

void GAssemblySettingsManager::SetShowBases(bool ok) {

    // set flag
    d->isBasesVisible = ok;

    // toggle action label
    QString label = "Show Bases";
    if (ok) { label = "Hide Bases"; }
    d->showBasesAction->setText(label);

    // emit change signal
    emit ShowBasesChanged(ok);
}
