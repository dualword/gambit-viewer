// ***************************************************************************
// GambitMain.cpp (c) 2009 Derek Barnett
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
// Entry point for the Gambit executable.  Initializes resources, constructs
// the 'top-level' Gambit object (GambitAPI), and shows the GMainWindow.
// ***************************************************************************

#include <QApplication>
#include "Main/GambitAPI.h"
#include "Main/GMainWindow.h"
using namespace Gambit;

int main(int argc, char* argv[]) {

    // intialize application and resources
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(icons);

    // initialize Gambit Core API
    GambitAPI* api = new GambitAPI;

    // intialize Gambit MainWindow
    GMainWindow mainWindow(api);
    mainWindow.show();

    // start application event loop
    return app.exec();
}
