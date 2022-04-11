// ***************************************************************************
// GAbstractFormatManager.cpp (c) 2009 Derek Barnett
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
// Describes main file format manager interface.  Specific file format
// managers must implement this interface.
//
// * Note - This is the entry point for file format plugins.  Each plugin must
// inherit from this interface, implement the CreateNewReader() method, which
// returns an file reader that implements the GAbstractFileReader interface.
// GAbstractFormatManager provides reasonable defaults for the remaining
// methods that should rarely need overriding.
//
// Please see the file format plugins supplied 'out-of-the-box' for examples
// of properly implementing this interface.
// ***************************************************************************

#include <QtCore>
#include <QtDebug>
#include "SessionManager/FileManager/GAbstractFormatManager.h"
#include "SessionManager/FileManager/GAbstractFileReader.h"
using namespace Gambit;
using namespace Gambit::FileIO;

void GAbstractFormatManager::CloseFiles(const GFileInfoList& files) {
    foreach (GFileInfo fileInfo, files) {
        GAbstractFileReader* reader = m_readerMap.take(fileInfo);
        if ( reader ) {
            reader->Close();
            delete reader;
            reader = 0;
        }
    }
}

void GAbstractFormatManager::OpenFiles(const GFileInfoList& files) {
    foreach (GFileInfo fileInfo, files) {
        if (fileInfo.Format == m_formatData.Name) {
            GAbstractFileReader* reader = CreateNewReader();
            if ( reader ) {
                reader->Open(fileInfo);
                m_readerMap.insert(fileInfo, reader);
            }
        }
    }
}

void GAbstractFormatManager::LoadData(GGenomicDataSet& data) {
    GReaderMap::const_iterator mapIter = m_readerMap.constBegin();
    GReaderMap::const_iterator mapEnd  = m_readerMap.constEnd();
    for ( ; mapIter != mapEnd; ++mapIter ) {
        GAbstractFileReader* reader = mapIter.value();
        reader->LoadData(data);
    }
}

bool GAbstractFormatManager::LoadReferences(GReferenceList& references) {
    GReaderMap::const_iterator mapIter = m_readerMap.constBegin();
    GReaderMap::const_iterator mapEnd  = m_readerMap.constEnd();
    for ( ; mapIter != mapEnd; ++mapIter ) {
        GAbstractFileReader* reader = mapIter.value();
        if ( reader->LoadReferences(references) ) {
            return true;
        }
    }
    return false;
}

const GFileFormatData& GAbstractFormatManager::FormatData(void) {
    return m_formatData;
}

const GFileInfoList GAbstractFormatManager::CurrentFiles(void) {
    return m_readerMap.keys();
}
