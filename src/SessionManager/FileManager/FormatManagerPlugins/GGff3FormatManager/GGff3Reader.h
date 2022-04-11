// ***************************************************************************
// GGff3Reader.h (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 17 Dec 2009 (DB)
// ---------------------------------------------------------------------------
// This file is part of Gambit plugin: (lib)gambit_fileformat_gff3.
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
// Implements GAbstractFileReader to handle GFF3 format.
// ***************************************************************************

#ifndef G_GFF3READER_H
#define G_GFF3READER_H

#include "SessionManager/FileManager/GAbstractFileReader.h"
#include "DataStructures/GReference.h"
class QString;

namespace Gambit {
namespace FileIO {

class GGff3Reader : public GAbstractFileReader {

    public:
        GGff3Reader(void);
        ~GGff3Reader(void);

    public:
        bool Close(void);
        bool LoadData(GGenomicDataSet& data);
        bool Open(const GFileInfo& fileInfo);
        bool LoadReferences(GReferenceList& references);

    private:
        struct GGff3ReaderPrivate;
        GGff3ReaderPrivate* d;
};

} // namespace FileIO
} // namespace Gambit

#endif // G_GFF3READER_H
