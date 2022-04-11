// ***************************************************************************
// GSessionManager.cpp (c) 2009 Derek Barnett
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
// Describes the main Gambit session manager.  This is a manager of
// sub-managers(currently the File- and Data-Managers).  Handles requests
// (input) and signals(output) as well as serialization of Gambit session
// state.
// Sessions are saved to a ".gss" (Gambit Session Settings) file.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "SessionManager/GSessionManager.h"
#include "SessionManager/DataManager/GDataManager.h"
#include "SessionManager/FileManager/GFileManager.h"
#include "SessionManager/FileManager/GFilenameDialog.h"
using namespace Gambit;
using namespace Gambit::Core;
using namespace Gambit::FileIO;

class GSessionManager::GSessionManagerPrivate {

    // constructor/destructor
    public:
        GSessionManagerPrivate(QObject* parent = 0);
        ~GSessionManagerPrivate(void);

    // implementation of parent interface
    public:
        void Create(void);
        void Load(void);
        void Save(bool force = false);
        void OpenFiles(const GFileInfoList& files = GFileInfoList());
        void CloseFiles(const GFileInfoList& files = GFileInfoList());
        GGenomicDataSet LoadData(const GGenomicDataRegion& region);

    // internally used methods
    private:
        void          ClearCurrent(void);
        bool          ConfirmSaveFromUser(void);
        const QString GetLoadFilenameFromUser(void);
        const QString GetSaveFilenameFromUser(void);

    // data available to session manager
    public:
        GDataManager* dataManager;
        GFileManager* fileManager;

    // true internally used data members
    private:
        QString  filename;
        bool     isSessionActive;
        QObject* parent;

    // version data
    private:
        static quint32 MAGIC_NUMBER;
        static quint32 FIRST_VERSION;
        static quint32 CURRENT_VERSION;
};

// define static version data
quint32 GSessionManager::GSessionManagerPrivate::MAGIC_NUMBER    = 0x10311301;
quint32 GSessionManager::GSessionManagerPrivate::FIRST_VERSION   = 100;
quint32 GSessionManager::GSessionManagerPrivate::CURRENT_VERSION = 100;

GSessionManager::GSessionManagerPrivate::GSessionManagerPrivate(QObject* parentObj)
    : dataManager( new GDataManager )
    , fileManager( new GFileManager )
    , filename("")
    , isSessionActive(false)
    , parent(parentObj)
{ }

GSessionManager::GSessionManagerPrivate::~GSessionManagerPrivate(void) {

    // destroy data manager
    if ( dataManager ) {
        delete dataManager;
        dataManager = 0;
    }

    // destroy file manager
    if ( fileManager ) {
        delete fileManager;
        fileManager = 0;
    }
}

void GSessionManager::GSessionManagerPrivate::ClearCurrent(void) {

    // if current session modified, save before clearing
    if ( fileManager->IsModified() ) {
        if ( ConfirmSaveFromUser() ) { Save(); }
    }

    // clear data
    filename = "";
    fileManager->CloseAll();

    GSessionManager* manager = (GSessionManager*)parent;
    emit manager->SessionDeactivated();
}

void GSessionManager::GSessionManagerPrivate::Create(void) {

    // clear data if session is active (saves first if modified)
    if ( isSessionActive ) { qDebug() << "session is activated... why?"; ClearCurrent(); }

    // get new session data from user
    fileManager->OpenFiles();
    filename = GetSaveFilenameFromUser();

    // save if filename provided
    if ( !filename.isEmpty() ) {

        // save
        Save(true);

        // set flag
        isSessionActive = true;

        // emit signal
        GSessionManager* manager = (GSessionManager*)parent;
        emit manager->SessionActivated();

    }
}

void GSessionManager::GSessionManagerPrivate::Load(void) {

    // clear data if session is active (saves first if modified)
    if ( isSessionActive ) { ClearCurrent(); }

    // get filename
    filename = GetLoadFilenameFromUser();
    if ( filename.isEmpty() ) { return; }

    // open file
    QFile loadFile(filename);
    if ( !loadFile.open(QIODevice::ReadOnly) ) { return; }

    // initialize data stream
    QDataStream loadStream(&loadFile);

    // read and validate 'magic number'
    quint32 magic;
    loadStream >> magic;
    bool isMagicOk = (magic == GSessionManager::GSessionManagerPrivate::MAGIC_NUMBER);

    if ( !isMagicOk ) {
        QString title = "Load Session Error";
        QString text  = "Could not load session: improper file format";
        QMessageBox::warning(0, title, text);
        return;
    }

    // check version of settings file
    quint32 version;
    loadStream >> version;
    bool isVersionOk = ( (version >= GSessionManager::GSessionManagerPrivate::FIRST_VERSION) &&
                         (version <= GSessionManager::GSessionManagerPrivate::CURRENT_VERSION) ) ;
    if ( !isVersionOk ) {
        QString title = "Load Session Error";
        QString text  = "Could not load session: unsupported version of session format";
        QMessageBox::warning(0, title, text);
        return;
    }

    // set version of data stream depending on GSS version (will expand later as 'legacy support' is required)
    if ( version <= GSessionManager::GSessionManagerPrivate::CURRENT_VERSION ) {
        loadStream.setVersion(QDataStream::Qt_4_5);
    }

    // load session data into sub-managers
    fileManager->Load(loadStream, version);

    // clean up and set flag
    loadFile.close();
    isSessionActive = true;

    // emit signal
    GSessionManager* manager = (GSessionManager*)parent;
    emit manager->SessionActivated();
}

void GSessionManager::GSessionManagerPrivate::Save(bool force) {

    // if not a forced save (these occur immediately after create())
    if ( !force ) {

        // do not save if no active session is loaded
        if ( !isSessionActive ) { return; }

        // if no sub-managers are modified, skip
        if ( !fileManager->IsModified() ) { return; }
    }

    // if current session filename is not set
    if ( filename.isEmpty() ) {

        // if user wants to save current session
        if ( ConfirmSaveFromUser() ) {

            // get save filename from user
            QString name = GetSaveFilenameFromUser();
            if ( name.isEmpty() ) { return; }
            filename = name;
        }

        // if user doesnt want to save
        else { return; }
    }

    // open file
    QFile saveFile( filename );
    if ( !saveFile.open(QIODevice::WriteOnly) ) { return; }

    // initialize data stream
    QDataStream saveStream(&saveFile);
    saveStream.setVersion(QDataStream::Qt_4_5);

    // save header - 'magic number' and settings version
    saveStream << (quint32)GSessionManager::GSessionManagerPrivate::MAGIC_NUMBER;
    saveStream << (quint32)GSessionManager::GSessionManagerPrivate::CURRENT_VERSION;

    // save session data from sub-managers
    fileManager->Save(saveStream);

    // clean up
    saveFile.close();
}

void GSessionManager::GSessionManagerPrivate::OpenFiles(const GFileInfoList& files) {
    fileManager->OpenFiles(files);
}

void GSessionManager::GSessionManagerPrivate::CloseFiles(const GFileInfoList& files) {
    fileManager->CloseFiles(files);
}

GGenomicDataSet GSessionManager::GSessionManagerPrivate::LoadData(const GGenomicDataRegion& region)
{
    GGenomicDataSet data = fileManager->LoadData(region);
    dataManager->ProcessData(data);
    return data;
}

bool GSessionManager::GSessionManagerPrivate::ConfirmSaveFromUser(void) {

    // set up message box
    QMessageBox msgBox;
    msgBox.setText("Current session has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
    msgBox.setDefaultButton(QMessageBox::Save);

    // if user clicks save, return true
    return ( msgBox.exec() == QMessageBox::Save );
}

const QString GSessionManager::GSessionManagerPrivate::GetLoadFilenameFromUser(void) {

    // get load filename from dialog, return if accepted
    GFilenameDialog dialog(GFilenameDialog::LOAD, (QWidget*)0);
    if ( dialog.exec() == QDialog::Accepted) {
        return dialog.Filename();
    }

    // default (empty string)
    return QString();
}

const QString GSessionManager::GSessionManagerPrivate::GetSaveFilenameFromUser(void) {

    // get save filename from dialog, return if accepted
    GFilenameDialog dialog(GFilenameDialog::SAVE, (QWidget*)0);
    if ( dialog.exec() == QDialog::Accepted) {
        return dialog.Filename();
    }

    // default (empty string)
    return QString();
}

// ----------------------------------------------------------------------------------

GSessionManager::GSessionManager(QObject* parent)
    : QObject(parent)
{
    d = new GSessionManagerPrivate(this);

    // connect 'pass-through' signals
    connect(d->fileManager, SIGNAL(FilesClosed(GFileInfoList)),       this, SIGNAL(FilesClosed(GFileInfoList)));
    connect(d->fileManager, SIGNAL(FilesOpened(GFileInfoList)),       this, SIGNAL(FilesOpened(GFileInfoList)));
    connect(d->fileManager, SIGNAL(ReferencesLoaded(GReferenceList)), this, SIGNAL(ReferencesLoaded(GReferenceList)));
}

GSessionManager::~GSessionManager(void) {
    delete d;
    d = 0;
}

void GSessionManager::Create(void) {
    d->Create();
}

void GSessionManager::Load(void) {
    d->Load();
}

void GSessionManager::Save(void) {
    d->Save();
}

void GSessionManager::OpenFiles(const GFileInfoList& files) {
    d->OpenFiles(files);
}

void GSessionManager::CloseFiles(const GFileInfoList& files) {
    d->CloseFiles(files);
}

void GSessionManager::LoadDataForViewer(const GGenomicDataRegion& region)
{
    GGenomicDataSet data = d->LoadData(region);
    emit ViewerDataLoaded(data);
}
