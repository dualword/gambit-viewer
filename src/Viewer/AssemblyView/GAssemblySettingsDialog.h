// ***************************************************************************
// GAssemblySettingsDialog.h (c) 2009 Derek Barnett
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

#ifndef G_ASSEMBLYSETTINGSDIALOG_H
#define G_ASSEMBLYSETTINGSDIALOG_H

#include <QDialog>
#include <QMap>
#include "DataStructures/GAlignment.h"
#include "DataStructures/GColorScheme.h"
#include "Viewer/AssemblyView/GVisibleAlignmentItem.h"
class QStandardItem;
class QTableWidgetItem;

namespace Gambit {
namespace Viewer {

struct AlignmentViewSettings {
    GAlignment::GAlignmentOptions alignmentFlag;    // flag to hold 'rules' for this mode
    GColorScheme currentColorScheme;    // current color display scheme for this mode
    GColorScheme defaultColorScheme;    // default display scheme (saved for later restoring, if desired)
};

typedef QMap<GVisibleAlignmentItem::AlignmentItemViewMode, AlignmentViewSettings> GViewSettingsMap;

class GAssemblySettingsForm : public QWidget {

    Q_OBJECT

    public:
        GAssemblySettingsForm(const GVisibleAlignmentItem::AlignmentItemViewMode& mode,
                              AlignmentViewSettings& settings,
                              QWidget* parent = 0);
        ~GAssemblySettingsForm(void);
    public:
        const GAlignment::GAlignmentOptions& AlignmentOptions(void);
        const GColorScheme& ColorScheme(void);

    signals:
        void SettingsChanged(const GVisibleAlignmentItem::AlignmentItemViewMode& mode,
                             AlignmentViewSettings& settings);

    private slots:
        void AlignmentOptionsChanged(QStandardItem* item);
        void EditColorScheme(void);

    private:
        struct GAssemblySettingsFormPrivate;
        GAssemblySettingsFormPrivate* d;
};

class GAssemblySettingsDialog : public QDialog {

    Q_OBJECT

    public:
        GAssemblySettingsDialog(GViewSettingsMap& settings, QWidget* parent = 0);
        ~GAssemblySettingsDialog(void);

    public:
        const GViewSettingsMap Settings(void);

    private slots:
        void UpdateSettings(const GVisibleAlignmentItem::AlignmentItemViewMode& mode,
                            AlignmentViewSettings& settings);

    private:
        void PopulateForms(void);

    private:
        struct GAssemblySettingsDialogPrivate;
        GAssemblySettingsDialogPrivate* d;
};

class GColorSchemeDialog : public QDialog {

    Q_OBJECT

    public:
        GColorSchemeDialog(AlignmentViewSettings& settings, QWidget* parent = 0);
        ~GColorSchemeDialog(void);

    public:
        const AlignmentViewSettings& Settings(void);

    private slots:
        void EditColorForItem(QTableWidgetItem* item);
        void RestoreDefault(void);

    private:
        struct GColorSchemeDialogPrivate;
        GColorSchemeDialogPrivate* d;

};

} // namespace Viewer
} // namespace Gambit

#endif // G_ASSEMBLYSETTINGSDIALOG_H
