// ***************************************************************************
// GMainWindow.h (c) 2009 Derek Barnett
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

#ifndef G_MAINWINDOW_H
#define G_MAINWINDOW_H

#include <QMainWindow>

namespace Gambit {

class GambitAPI;

class GMainWindow : public QMainWindow {

    Q_OBJECT

    // constructors
    public:
        GMainWindow(GambitAPI* api = 0);
        ~GMainWindow(void);

    // initial factory methods
    private:
        void CreateActions(void);
        void CreateConnections(void);
        void CreateMenus(void);
        void CreateMainDisplay(void);
        void CreateToolBar(void);

    // 'internal' view instructions
    private slots:
        void Init(void);
        void ShowAboutGambitDialog(void);
        void ShowAboutQtDialog(void);
        void ShowHelpTab(void);
        void ShowHomeTab(void);
        void ShowToolkitTab(void);
        void ShowViewerTab(void);
        void ShowWebTab(void);

    // data members
    private:
        struct GMainWindowPrivate;
        GMainWindowPrivate* d;

}; // class GMainWindow

}  // namespace Gambit

#endif // G_MAINWINDOW_H
