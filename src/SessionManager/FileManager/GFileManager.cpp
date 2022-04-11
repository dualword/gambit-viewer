// ***************************************************************************
// GFileManager.cpp (c) 2009 Derek Barnett
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
// Describes the main file manager.
// This manager is currently responsible for all major file-related operations:
//   - Opening/closing data files
//   - Retrieving data from a specified region (from all open files)
//   - Serialization (saves list of currently open files)
//   - Detecting/loading file format plugins
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "SessionManager/FileManager/GFileManager.h"
#include "DataStructures/GGenomicDataSet.h"
#include "DataStructures/GFileFormatData.h"
#include "SessionManager/FileManager/GAbstractFileReader.h"
#include "SessionManager/FileManager/GAbstractFormatManager.h"
#include "SessionManager/FileManager/GCloseFilesDialog.h"
#include "SessionManager/FileManager/GOpenFilesWidget.h"
using namespace Gambit;
using namespace Gambit::FileIO;

GFileManager::GFileManager(QObject* parent)
    : QObject(parent)
    , m_isModified(false)
{
    LoadPlugins();
}

GFileManager::~GFileManager(void) {
    qDeleteAll(m_managers);
}

void GFileManager::LoadPlugins(void) {

    // load static plugins
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        SavePlugin(plugin);
    }

    // retrieve plugin directory for dynamic plugins
    // (application directory structure varies per OS)
     QDir pluginsDir = QDir(qApp->applicationDirPath());

 #if defined(Q_OS_WIN) // get out of executable folder back to parent application directory
     if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
         pluginsDir.cdUp();
 #elif defined(Q_OS_MAC) // get out of executable folder back to application bundle 'Contents' directory
     if (pluginsDir.dirName() == "MacOS") {
         pluginsDir.cdUp();
     }
 #endif

     pluginsDir.cd("plugins");

     // load dynamic plugins
     foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
         QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
         QObject *plugin = loader.instance();
         if (plugin) {
             SavePlugin(plugin);
         }
     }
}

void GFileManager::SavePlugin(QObject* plugin) {

    // check if plugin is actually a FormatManager
    GAbstractFormatManager* manager = qobject_cast<GAbstractFormatManager*>(plugin);

    // check if plugin already known to FileManager
    // (will allow safer 'refresh' when adding plugins during operation)
    if ( manager && !m_managers.contains(manager) ) {
        m_managers.append(manager);
    }
}

void GFileManager::CloseAll(void) {
    CloseFiles( GetOpenFiles() );
}

void GFileManager::CloseFiles(GFileInfoList files) {

    // if 'generic' close files command passed, get files from user
    if ( files.isEmpty() ) { files = ShowCloseFilesDialog(); }

    // if file list *still* empty (user canceled close files form) - do nothing
    if ( files.isEmpty() ) { return; }

    foreach (GAbstractFormatManager* manager, m_managers) {
        if ( manager ) { manager->CloseFiles(files); }
    }

    m_isModified = true;
    emit FilesClosed(files);
}

void GFileManager::OpenFiles(GFileInfoList files) {

    // if 'generic' open files command passed, get files from user
    if ( files.isEmpty() ) { files = ShowOpenFilesDialog(); }

    // if file list *still* empty (user canceled open files form) - do nothing
    if ( files.isEmpty() ) { return; }

    bool referencesLoaded = false;
    foreach (GAbstractFormatManager* manager, m_managers) {
        if ( manager ) {
            manager->OpenFiles(files);
            if ( !referencesLoaded ) {
                GReferenceList references;
                if ( manager->LoadReferences(references) ) {
                    emit ReferencesLoaded(references);
                    referencesLoaded = true;
                }
            }
        }
    }
    m_isModified = true;
    emit FilesOpened(files);
}

const GFileInfoList GFileManager::ShowCloseFilesDialog(void) {
    GCloseFilesDialog dialog( GetOpenFiles() );
    if ( dialog.exec() != QDialog::Accepted ) {
        return GFileInfoList();
    } else {
        return dialog.SelectedFiles();
    }
}

const GFileInfoList GFileManager::ShowOpenFilesDialog(void) {

    // farm this out to separate dialog class later
    QDialog* dialog = new QDialog;

    QPushButton* okButton = new QPushButton("Ok", dialog);
    connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));

    QPushButton* cancelButton = new QPushButton("Cancel", dialog);
    connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(2);
    buttonLayout->addWidget(okButton, 1, Qt::AlignCenter);
    buttonLayout->addStretch(2);
    buttonLayout->addWidget(cancelButton, 1, Qt::AlignCenter);
    buttonLayout->addStretch(2);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    GOpenFilesWidget* widget = new GOpenFilesWidget( GetSupportedFormats(), dialog);
    layout->addWidget(widget, 1, Qt::AlignCenter);
    layout->addLayout(buttonLayout);
    dialog->setLayout(layout);

    if ( dialog->exec() == QDialog::Accepted ) {
        return widget->SelectedFiles();
    } else {
        return GFileInfoList();
    }
}

GFileInfoList GFileManager::GetOpenFiles(void) {
    GFileInfoList files;
    foreach(GAbstractFormatManager* manager, m_managers) {
        if ( manager ) { files.append( manager->CurrentFiles() ); }
    }
    return files;
}

GFileFormatList GFileManager::GetSupportedFormats(void) {
    GFileFormatList formats;
    foreach (GAbstractFormatManager* manager, m_managers) {
        if ( manager ) { formats.append( manager->FormatData() ); }
    }
    return formats;
}

GGenomicDataSet GFileManager::LoadData(const GGenomicDataRegion& region)
{
    GGenomicDataSet data(region);
    foreach (GAbstractFormatManager* manager, m_managers) {
        if ( manager ) { manager->LoadData(data); }
    }

    // ensure right bound matches actual length of reference sequence available
    // (requested range might initially extend beyond reference length)
    data.Region.RightBound = (data.Region.LeftBound + data.Sequence.length() - 1);
    return data;
}


void GFileManager::Load(QDataStream& in, int version) {
    if ( version >= 100 ) {
        GFileInfoList files;
        in >> files;
        OpenFiles(files);
    } else { qDebug() << "Unknown version: " << version; }
}

void GFileManager::Save(QDataStream& out) {
    GFileInfoList files = GetOpenFiles();
    out << files;
    m_isModified = false;
}
