// ***************************************************************************
// GMainWindow.cpp (c) 2009 Derek Barnett
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
// Describes the main Gambit application window.
//
// Mostly provides menu options that are forwarded to GambitAPI.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Main/GMainWindow.h"
#include "Main/GambitAPI.h"
#include "Main/GMainNavigationWidget.h"
#include "Main/GHomeWidget.h"
#include "Utilities/GManhattanStyle.h"
using namespace Gambit;

namespace Gambit {
struct GMainWindowActions {

    QAction* OpenFile;
    QAction* CloseFile;
    QAction* SaveSession;
    QAction* LoadSession;
    QAction* Print;
    QAction* Exit;
    QAction* ZoomIn;
    QAction* ZoomOut;
    QAction* ZoomReset;
    QAction* AboutGambit;
    QAction* AboutQt;

    GMainWindowActions(void)
        : OpenFile(NULL)
        , CloseFile(NULL)
        , SaveSession(NULL)
        , LoadSession(NULL)
        , Print(NULL)
        , Exit(NULL)
        , ZoomIn(NULL)
        , ZoomOut(NULL)
        , ZoomReset(NULL)
        , AboutGambit(NULL)
        , AboutQt(NULL)
    { }
};
} // namespace Gambit

struct GMainWindow::GMainWindowPrivate {
    GambitAPI*             api;
    GMainNavigationWidget* navigator;
    GMainWindowActions*    actions;
};

GMainWindow::GMainWindow(GambitAPI* api)
    : QMainWindow()
{
    // initialize internals
    d = new GMainWindowPrivate;
    d->api = api;
    d->navigator = new GMainNavigationWidget;
    d->actions   = new GMainWindowActions;

    // set window style
    QString baseName = qApp->style()->objectName();
#ifdef Q_WS_X11
    if (baseName == QLatin1String("windows")) {
        QByteArray desktopEnvironment = qgetenv("DESKTOP_SESSION");
        if (desktopEnvironment == "kde")
            baseName = QLatin1String("plastique");
        else
            baseName = QLatin1String("cleanlooks");
    }
#endif
    qApp->setStyle(new GManhattanStyle(baseName));

    // set window window attributes
    setWindowTitle(tr("Gambit"));
    setGeometry(50, 50, 1400, 1100);

    // kick off Init() once event loop starts
    // (try to speed up startup times a bit)
    QTimer::singleShot(0, this, SLOT(Init()));
}

GMainWindow::~GMainWindow(void) {
    delete d;
    d = 0;
}

void GMainWindow::Init(void) {
    CreateConnections();
    CreateActions();
    CreateMenus();
    CreateMainDisplay();
}

void GMainWindow::CreateActions(void) {

    // set up 'Open File' action - opens data file(s)
    d->actions->OpenFile = new QAction(QIcon(":/icons/add"), tr("&Open File..."), this);
    d->actions->OpenFile->setToolTip(tr("Open data file(s)"));
    connect(d->actions->OpenFile, SIGNAL(triggered()), d->api, SLOT(OpenFiles()));

    // set up 'Close File' action - closes open file(s)
    d->actions->CloseFile = new QAction(QIcon(":/icons/delete"), tr("&Close File..."), this);
    d->actions->CloseFile->setToolTip(tr("Close open file(s)"));
    connect(d->actions->CloseFile, SIGNAL(triggered()), d->api, SLOT(CloseFiles()));

    // set up 'Save Session' action - saves current session settings
    d->actions->SaveSession = new QAction(QIcon(":/icons/save"), tr("Save Session..."), this);
    d->actions->SaveSession->setToolTip(tr("Save current session settings"));
    connect(d->actions->SaveSession, SIGNAL(triggered()), d->api, SLOT(SaveSession()));

    // set up 'Load Session' action - loads a saved session
    d->actions->LoadSession = new QAction(QIcon(":/icons/load"), tr("Load Session..."), this);
    d->actions->LoadSession->setToolTip(tr("Load a saved session"));
    connect(d->actions->LoadSession, SIGNAL(triggered()), d->api, SLOT(LoadSession()));

    // set up 'Print' action - prints viewer contents
    d->actions->Print = new QAction(QIcon(":/icons/print"), tr("Print..."), this);
    d->actions->Print->setToolTip(tr("Print contents of Viewer"));
    connect(d->actions->Print, SIGNAL(triggered()), d->api, SLOT(Print()));

    // set up 'Exit' action - exits program and closes window
    d->actions->Exit = new QAction(tr("E&xit"), this);
    d->actions->Exit->setToolTip(tr("Exit the application"));
    connect(d->actions->Exit, SIGNAL(triggered()), this, SLOT(close()));

    // set up 'Zoom In' action - zooms in sequence view 10% - **** probably will move to GViewerToolbar *** makes more sense there, than persistent menu option
    d->actions->ZoomIn = new QAction(tr("Zoom &In"), this);
    d->actions->ZoomIn->setShortcut(QKeySequence::ZoomIn);
    d->actions->ZoomIn->setToolTip(tr("Zoom in sequence view"));
    connect(d->actions->ZoomIn, SIGNAL(triggered()), d->api, SLOT(ZoomIn()));

    // set up 'Zoom Out' action - zooms out sequence view 10% - **** probably will move to GViewerToolbar *** makes more sense there, than persistent menu option
    d->actions->ZoomOut = new QAction(tr("Zoom &Out"), this);
    d->actions->ZoomOut->setShortcut(QKeySequence::ZoomOut);
    d->actions->ZoomOut->setToolTip(tr("Zoom out sequence view"));
    connect(d->actions->ZoomOut, SIGNAL(triggered()), d->api, SLOT(ZoomOut()));

    // set up 'Zoom Reset' action - resets zoom level and re-centers sequence view - **** probably will move to GViewerToolbar *** makes more sense there, than persistent menu option
    d->actions->ZoomReset = new QAction(tr("&Reset"), this);
    d->actions->ZoomReset->setShortcut(QKeySequence::Refresh);
    d->actions->ZoomReset->setToolTip(tr("Reset sequence view"));
    connect(d->actions->ZoomReset, SIGNAL(triggered()), d->api, SLOT(ZoomReset()));

    // set up 'About Gambit' action - shows message box with Gambit info
    d->actions->AboutGambit = new QAction(tr("About &Gambit"), this);
    d->actions->AboutGambit->setToolTip(tr("About Gambit"));
    connect(d->actions->AboutGambit, SIGNAL(triggered()), this, SLOT(ShowAboutGambitDialog()));

    // set up 'About Qt' action - shows message box with Qt info
    d->actions->AboutQt = new QAction(tr("About &Qt"), this);
    d->actions->AboutQt->setToolTip(tr("About Qt"));
    connect(d->actions->AboutQt, SIGNAL(triggered()), this, SLOT(ShowAboutQtDialog()));
}

void GMainWindow::CreateConnections(void) {
    connect(d->api, SIGNAL(SessionActivated()),   this, SLOT(ShowViewerTab()));
    connect(d->api, SIGNAL(SessionDeactivated()), this, SLOT(ShowHomeTab()));
}

void GMainWindow::CreateMainDisplay(void) {

    // set up 'home' tab widget
    GHomeWidget* home = new GHomeWidget(this);
    connect(home, SIGNAL(RequestCreateSession(void)), d->api, SLOT(CreateSession(void)));
    connect(home, SIGNAL(RequestLoadSession(void)),   d->api, SLOT(LoadSession(void)));

    // set up 'viewer' tab widget
    QWidget* viewer = d->api->Viewer();

    // set up 'tools' tab widget
    QLabel* toolIconLabel    = new QLabel("");
    toolIconLabel->setPixmap( QPixmap(":/icons/info") );
    QLabel* toolMessageLabel  = new QLabel("<b>Toolkit currently unavailable.</b>");
    QLabel* toolMessage2Label = new QLabel("<b>This panel will later provide the analysis toolkit plugin \'dashboard\'</b>");

    QHBoxLayout* toolIconLayout = new QHBoxLayout;
    toolIconLayout->addStretch(2);
    toolIconLayout->addWidget(toolIconLabel);
    toolIconLayout->addWidget(toolMessageLabel);
    toolIconLayout->addStretch(2);

    QVBoxLayout* toolLayout = new QVBoxLayout;
    toolLayout->setMargin(0);
    toolLayout->addWidget( new QToolBar);
    toolLayout->addStretch(3);
    toolLayout->addLayout(toolIconLayout);
    toolLayout->addWidget(toolMessage2Label, 1, Qt::AlignCenter);
    toolLayout->addStretch(3);
    QFrame* tool = new QFrame;
    tool->setLayout(toolLayout);

    // set up 'web' tab widget
    QLabel* webIconLabel    = new QLabel("");
    webIconLabel->setPixmap( QPixmap(":/icons/info") );
    QLabel* webMessageLabel  = new QLabel("<b>Web currently unavailable.</b>");
    QLabel* webMessage2Label = new QLabel("<b>This panel will later provide an interface to external web resources.</b>");

    QHBoxLayout* webIconLayout = new QHBoxLayout;
    webIconLayout->addStretch(2);
    webIconLayout->addWidget(webIconLabel);
    webIconLayout->addWidget(webMessageLabel);
    webIconLayout->addStretch(2);

    QVBoxLayout* webLayout = new QVBoxLayout;
    webLayout->setMargin(0);
    webLayout->addWidget( new QToolBar);
    webLayout->addStretch(3);
    webLayout->addLayout(webIconLayout);
    webLayout->addWidget(webMessage2Label, 1, Qt::AlignCenter);
    webLayout->addStretch(3);
    QFrame* web = new QFrame;
    web->setLayout(webLayout);

    // set up 'help' tab widget
    QLabel* helpIconLabel    = new QLabel("");
    helpIconLabel->setPixmap( QPixmap(":/icons/info") );
    QLabel* helpMessageLabel  = new QLabel("<b>Help mode currently unavailable.</b>");
    QLabel* helpMessage2Label = new QLabel("<b>This panel will later provide extensive documentation for users and developers.</b>");
    QLabel* helpMessage3Label = new QLabel("<b>For now, please look to the PDF documentation or search posted issues on the Gambit project homepage.</b>");
    QLabel* helpMessage4Label = new QLabel(d->api->GambitProjectUrl());

    QHBoxLayout* helpIconLayout = new QHBoxLayout;
    helpIconLayout->addStretch(2);
    helpIconLayout->addWidget(helpIconLabel);
    helpIconLayout->addWidget(helpMessageLabel);
    helpIconLayout->addStretch(2);

    QVBoxLayout* helpLayout = new QVBoxLayout;
    helpLayout->setMargin(0);
    helpLayout->addWidget( new QToolBar );
    helpLayout->addStretch(3);
    helpLayout->addLayout(helpIconLayout);
    helpLayout->addWidget(helpMessage2Label, 1, Qt::AlignCenter);
    helpLayout->addWidget(helpMessage3Label, 1, Qt::AlignCenter);
    helpLayout->addWidget(helpMessage4Label, 1, Qt::AlignCenter);
    helpLayout->addStretch(3);
    QFrame* help = new QFrame;
    help->setLayout(helpLayout);

    // set up navigation widget tabs, set start tab
    d->navigator->insertTab(GMainNavigationWidget::Home,   home,   QIcon(":/icons/home"), "Home");
    d->navigator->insertTab(GMainNavigationWidget::Viewer, viewer, QIcon(":/icons/view"), "Viewer");
    d->navigator->insertTab(GMainNavigationWidget::Tools,  tool,   QIcon(":/icons/tool"), "Tools");
    d->navigator->insertTab(GMainNavigationWidget::Web,    web,    QIcon(":/icons/web"),  "Web");
    d->navigator->insertTab(GMainNavigationWidget::Help,   help,   QIcon(":/icons/help"), "Help");
    ShowHomeTab();

    // set up window layout
    QVBoxLayout* windowLayout = new QVBoxLayout;
    windowLayout->setContentsMargins(0,0,0,0);
    windowLayout->addWidget(d->navigator);

    // set up layout on main window frame
    QFrame* windowFrame = new QFrame;
    windowFrame->setLayout(windowLayout);
    setCentralWidget(windowFrame);
}

void GMainWindow::CreateMenus(void) {

    // set up 'File' menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(d->actions->OpenFile);
    fileMenu->addAction(d->actions->CloseFile);
    fileMenu->addSeparator();
    fileMenu->addAction(d->actions->SaveSession);
    fileMenu->addAction(d->actions->LoadSession);
    fileMenu->addSeparator();
//    fileMenu->addAction(d->actions->Print);               // printing not fully tested, omit for now
//    fileMenu->addSeparator();
    fileMenu->addAction(d->actions->Exit);

    // set up 'Edit' menu
//    QMenu* editMenu  = menuBar()->addMenu(tr("&Edit"));
//    editMenu->addAction(d->actions->EditReadGroups);

    // set up 'View' menu - **** probably will move to GViewerToolbar *** makes more sense there, than persistent menu options
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(d->actions->ZoomIn);
    viewMenu->addAction(d->actions->ZoomOut);
    viewMenu->addAction(d->actions->ZoomReset);

    // set up 'Tools' menu (skip for now)
//    QMenu* toolsMenu = menuBar()->addMenu(tr("&Tools"));

    // set up 'Help' menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(d->actions->AboutGambit);
    helpMenu->addAction(d->actions->AboutQt);
}

void GMainWindow::ShowAboutGambitDialog(void) {

// MacOS-specific message box handling
// Avoids creating duplicate 'about' dialogs
#ifdef Q_WS_MAC
    static QPointer<QMessageBox> oldMsgBox;
    if (oldMsgBox) {
        oldMsgBox->show();
        oldMsgBox->raise();
        oldMsgBox->activateWindow();
        return;
    }
#endif

    QString versionNumber   = d->api->GambitVersion();
    QString labHomepage     = d->api->MarthLabUrl();
    QString projectHomepage = d->api->GambitProjectUrl();

    QString aboutGambitText = tr(
        "<h3>Version %1</h3>"
        "<p>(c) 2009 Derek Barnett<br>"
        "Marth Lab, Department of Biology, Boston College</p>"
        "<p><b>Lab Website:</b> %2 </p>"
        "<p><b>Project Website:</b> %3 </p>"
        ).arg(versionNumber).arg(labHomepage).arg(projectHomepage);

    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowTitle(tr("About Gambit"));
    msgBox->setText(aboutGambitText);

    QPixmap pm(":/icons/logo");
    if (!pm.isNull()) {
        msgBox->setIconPixmap(pm.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

// the rest of MacOS-specific message box handling
#ifdef Q_WS_MAC
    oldMsgBox = msgBox;
    msgBox->show();
#else
    msgBox->exec();
#endif

}

// show pre-packaged AboutQt() dialog
void GMainWindow::ShowAboutQtDialog(void) {
    QString title = "About Qt";
    QMessageBox::aboutQt(this);
}

// convenience methods for tab selection
void GMainWindow::ShowHomeTab(void)    { d->navigator->setCurrentIndex(GMainNavigationWidget::Home);   }
void GMainWindow::ShowViewerTab(void)  { d->navigator->setCurrentIndex(GMainNavigationWidget::Viewer); }
void GMainWindow::ShowToolkitTab(void) { d->navigator->setCurrentIndex(GMainNavigationWidget::Tools);  }
void GMainWindow::ShowWebTab(void)     { d->navigator->setCurrentIndex(GMainNavigationWidget::Web);    }
void GMainWindow::ShowHelpTab(void)    { d->navigator->setCurrentIndex(GMainNavigationWidget::Help);   }
