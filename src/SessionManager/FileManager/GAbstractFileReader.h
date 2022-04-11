// ***************************************************************************
// GbstractFileReader.h (c) 2009 Derek Barnett
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
// Describes main file reader interface.  Specific file format readers must
// implement this interface for data retrieval.
// ***************************************************************************

#ifndef G_ABSTRACTFILEREADER_H
#define G_ABSTRACTFILEREADER_H

#include "DataStructures/GReference.h"
class QString;

namespace Gambit {

class GFileInfo;
class GGenomicDataSet;

namespace FileIO {

class GAbstractFileReader {

    public:
        GAbstractFileReader(void) { }
        virtual ~GAbstractFileReader(void) { }

    public:
        virtual bool Close(void) =0;
        virtual bool Open(const GFileInfo& fileInfo) =0;
        virtual bool LoadData(GGenomicDataSet& data) =0;
        virtual bool LoadReferences(GReferenceList& references) =0;
};

} // namespace FileIO
} // namespace Gambit

#endif // G_ABSTRACTFILEREADER_H
