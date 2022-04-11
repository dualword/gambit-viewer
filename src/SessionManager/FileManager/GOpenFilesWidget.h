// ***************************************************************************
// GOpenFilesWidget.h (c) 2009 Derek Barnett
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

#ifndef G_OPENFILESWIDGET_H
#define G_OPENFILESWIDGET_H

#include <QDir>
#include <QFrame>
#include <QWidget>
#include "DataStructures/GFileInfo.h"
#include "DataStructures/GFileFormatData.h"
class QTreeWidgetItem;

namespace Gambit {

class GOpenFilesForm : public QWidget {

    Q_OBJECT

    // constructors
    public:
        GOpenFilesForm(GFileInfo::FileType type,
                       const GFileFormatList& formats,
                       QWidget* parent = 0);
        ~GOpenFilesForm(void);

    // GOpenFilesForm interface
    public:
        void setDir(QDir dir);

    // internal slot methods
    private slots:
        void FileBrowseButtonPressed(void);
        void FilenameLineEditChanged(QString text);
        void IndexBrowseButtonPressed(void);
        void IndexLineEditChanged(QString text);
        void GenerateIndexToggled(bool checked);
        void UseIndexToggled(bool checked);
        void ApplyButtonPressed(void);
        void CancelButtonPressed(void);
        void SelectedFormatChanged(int);

    // signals
    signals:
        void DirectoryChanged(QDir dir);
        void FileSelected(const GFileInfo& fileInfo);
        void FormCanceled(void);

    private:
        struct GOpenFilesFormPrivate;
        GOpenFilesFormPrivate* d;
};

class GOpenFilesWidget : public QFrame {

    Q_OBJECT

    public:
        GOpenFilesWidget(const GFileFormatList& formats, QWidget* parent = 0);
        ~GOpenFilesWidget(void);

    public:
        GFileInfoList SelectedFiles(void) const;

    public slots:
        void SetDir(QDir dir);
        void AddFile(const GFileInfo& fileInfo);

    signals:
        void FilesModified(GFileInfoList files);

    private slots:
        void ShowTabForItem(QTreeWidgetItem* item, int column);

    private:
        void CreateFormsTab(const GFileFormatList& formats);

    private:
        struct GOpenFilesWidgetPrivate;
        GOpenFilesWidgetPrivate* d;
};

} // namespace Gambit

#endif // G_OPENFILESWIDGET_H
