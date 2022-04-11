// ***************************************************************************
// GCloseFilesDialog.h (c) 2009 Derek Barnett
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

#ifndef G_CLOSEFILESDIALOG_H
#define G_CLOSEFILESDIALOG_H

#include <QDialog>
#include "DataStructures/GFileInfo.h"
class QListWidgetItem;

namespace Gambit {

class GCloseFilesDialog : public QDialog {

    Q_OBJECT

    public:
        GCloseFilesDialog(const GFileInfoList& currentFiles, QWidget* parent = 0);
        ~GCloseFilesDialog(void);

    public slots:
        GFileInfoList SelectedFiles(void) const;

    private slots:
        void FileItemClicked(QListWidgetItem* item);

    private:
        struct GCloseFilesDialogPrivate;
        GCloseFilesDialogPrivate* d;
};

} // namespace Gambit

#endif // G_CLOSEFILESDIALOG_H
