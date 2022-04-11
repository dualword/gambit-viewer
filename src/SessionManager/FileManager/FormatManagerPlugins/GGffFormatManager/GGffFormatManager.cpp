// ***************************************************************************
// GGffFormatManager.cpp (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 17 Dec 2009 (DB)
// ---------------------------------------------------------------------------
// This file is part of Gambit plugin: (lib)gambit_fileformat_gff.
// Plugin license rights are same as main application.
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
// Implements GAbstractFormatManager to handle GFF(2) format.
// ***************************************************************************

#include <QtCore>
#include <QtDebug>
#include "./GGffFormatManager.h"
#include "./GGffReader.h"
using namespace Gambit;
using namespace Gambit::FileIO;

GGffFormatManager::GGffFormatManager(QObject* parent)
    : QObject(parent)
    , GAbstractFormatManager()
{
    m_formatData.Name = "GFF2";
    m_formatData.Extensions << "*.gff";
    m_formatData.Types << GFileInfo::File_Gene << GFileInfo::File_Snp;
    m_formatData.UsesIndex = false;
}

GGffFormatManager::~GGffFormatManager(void) { }

GAbstractFileReader* GGffFormatManager::CreateNewReader(void) {
    return new GGffReader;
}

Q_EXPORT_PLUGIN2(gambit_fileformat_gff, GGffFormatManager)
