// ***************************************************************************
// GFilenameDialog.h (c) 2009 Derek Barnett
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

#ifndef G_FILENAMEDIALOG_H
#define G_FILENAMEDIALOG_H

#include <QDialog>

namespace Gambit {
namespace Core   {

class GFilenameDialog : public QDialog {

    Q_OBJECT

    public:
        enum PromptType { LOAD = 0, SAVE };

    public:
        GFilenameDialog(PromptType type, QWidget* parent = 0);
        ~GFilenameDialog(void);
        const QString Filename(void);

    public slots:
        void BrowseButtonClicked(void);

    private:
        struct GFilenameDialogPrivate;
        GFilenameDialogPrivate* d;
};

} // namespace Core
} // namespace Gambit

#endif // G_FILENAMEDIALOG_H
