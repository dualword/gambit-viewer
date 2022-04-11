// ***************************************************************************
// GDataManager.cpp (c) 2009 Derek Barnett
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
// Describes the main data processing manager.  Provides basic pre-processing
// of data loaded from files.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "GDataManager.h"
#include "DataStructures/GGenomicDataSet.h"
#include "SessionManager/DataManager/GGenomicDataPadder.h"
#include "SessionManager/DataManager/GMismatchCalculator.h"
using namespace Gambit;
using namespace Gambit::Core;

struct GDataManager::GDataManagerPrivate {

    QStringList readGroups;

    // pre-processing tools
    void ApplyMismatches(GGenomicDataSet& data);
    void ApplyPadding(GGenomicDataSet& data);
};

void GDataManager::GDataManagerPrivate::ApplyMismatches(GGenomicDataSet& data) {
    GMismatchCalculator mismatchCalculator;
    mismatchCalculator.Exec(data);
}

void GDataManager::GDataManagerPrivate::ApplyPadding(GGenomicDataSet& data) {
    GGenomicDataPadder padder;
    padder.Exec(data);
}

// ----------------------------------------------- //
// GDataManager implementation
// ----------------------------------------------- //

GDataManager::GDataManager(QObject* parent)
    : QObject(parent)
{
    d = new GDataManagerPrivate;
}

GDataManager::~GDataManager(void) {
    delete d;
    d = 0;
}

void GDataManager::ProcessData(GGenomicDataSet& data) {
    d->ApplyPadding(data);
    d->ApplyMismatches(data);
}
