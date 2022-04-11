// ***************************************************************************
// GBamFormatManager.cpp (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 17 Dec 2009 (DB)
// ---------------------------------------------------------------------------
// This file is part of Gambit plugin: (lib)gambit_fileformat_bam.
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
// Implements GAbstractFormatManager to handle BAM format.
// ***************************************************************************

#include <QtCore>
#include <QtDebug>
#include "./GBamFormatManager.h"
#include "./GBamReader.h"
#include "DataStructures/GFileInfo.h"
using namespace Gambit;
using namespace Gambit::FileIO;

GBamFormatManager::GBamFormatManager(QObject* parent)
    : QObject(parent)
    , GAbstractFormatManager()
{
    m_formatData.Name = "BAM";
    m_formatData.Extensions << "*.bam";
    m_formatData.IndexExtensions << "*.bai";
    m_formatData.Types << GFileInfo::File_Alignment;
    m_formatData.UsesIndex = true;
}

GBamFormatManager::~GBamFormatManager(void) { }

GAbstractFileReader* GBamFormatManager::CreateNewReader(void) {
    return new GBamReader;
}

Q_EXPORT_PLUGIN2(gambit_fileformat_bam, GBamFormatManager)
