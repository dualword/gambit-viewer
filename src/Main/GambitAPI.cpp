// ***************************************************************************
// GambitAPI.cpp (c) 2009 Derek Barnett
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
// Describes the top-level API class.  This class links all the sub-modules in
// the Gambit architecture, 'under the hood' in the implementation file.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Main/GambitAPI.h"
#include "DataStructures/GGenomicDataSet.h"
#include "SessionManager/GSessionManager.h"
#include "Viewer/GViewer.h"
using namespace Gambit;
using namespace Gambit::Core;

const QString GambitAPI::GAMBIT_CURRENT_VERSION = "0.4.145(alpha)";
const QString GambitAPI::GAMBIT_MARTHLAB_URL    = "http://bioinformatics.bc.edu/marthlab/Main_Page";
const QString GambitAPI::GAMBIT_PROJECT_URL     = "http://code.google.com/p/gambit-viewer/";

GambitAPI::GambitAPI(QObject* parent)
    : QObject(parent)
{
    CreateModules();
    ConnectModules();
}

GambitAPI::~GambitAPI(void) { }

void GambitAPI::CreateModules(void)
{
    m_sessionManager = new GSessionManager(this);
    m_viewer         = new GViewer((QWidget*)0);
}

void GambitAPI::ConnectModules(void)
{
    // connect 'pass-through' signals
    connect(m_sessionManager, SIGNAL(SessionActivated()),
            this,             SIGNAL(SessionActivated()));

    connect(m_sessionManager, SIGNAL(SessionDeactivated()),
            this,             SIGNAL(SessionDeactivated()));

    // connect session manager <--> viewer
    connect(m_sessionManager, SIGNAL(ReferencesLoaded(GReferenceList)),
            m_viewer,         SLOT(ShowReferences(GReferenceList)));

    connect(m_sessionManager, SIGNAL(SessionDeactivated()),
            m_viewer,         SLOT(Clear()));

    connect(m_viewer,         SIGNAL(ViewerDataRequested(GGenomicDataRegion)),
            m_sessionManager, SLOT(LoadDataForViewer(GGenomicDataRegion)));

    connect(m_sessionManager, SIGNAL(ViewerDataLoaded(GGenomicDataSet)),
            m_viewer,         SLOT(ShowAssembly(GGenomicDataSet)));
}

// Gambit app & developer info
const QString GambitAPI::GambitVersion(void) const    { return GambitAPI::GAMBIT_CURRENT_VERSION; }
const QString GambitAPI::GambitProjectUrl(void) const { return GambitAPI::GAMBIT_PROJECT_URL;     }
const QString GambitAPI::MarthLabUrl(void) const      { return GambitAPI::GAMBIT_MARTHLAB_URL;    }

// GSessionManager API
void GambitAPI::CreateSession(void) { m_sessionManager->Create(); }
void GambitAPI::LoadSession(void)   { m_sessionManager->Load();   }
void GambitAPI::SaveSession(void)   { m_sessionManager->Save();   }

// GFileManager API
void GambitAPI::CloseFiles(void) { m_sessionManager->CloseFiles(); }
void GambitAPI::OpenFiles(void)  { m_sessionManager->OpenFiles();  }

// GDataManager API
void GambitAPI::LoadDataForViewer(const GGenomicDataRegion& region) {
    m_sessionManager->LoadDataForViewer(region);
}

// GViewer API
void GambitAPI::Print(void)     { m_viewer->Print();     }
void GambitAPI::ZoomIn(void)    { m_viewer->ZoomIn();    }
void GambitAPI::ZoomOut(void)   { m_viewer->ZoomOut();   }
void GambitAPI::ZoomReset(void) { m_viewer->ZoomReset(); }

// Return Gambit Viewer as QWidgets
// Allows embedding of Gambit widgets outside API
// Mostly used by GMainWindow for Gambit executable, but could (theoretically) be used by external Qt apps
QWidget* GambitAPI::Viewer(void)  { return m_viewer;  }
