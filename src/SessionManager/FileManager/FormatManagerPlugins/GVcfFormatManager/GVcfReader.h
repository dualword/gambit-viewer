// ***************************************************************************
// GVcfReader.h (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 17 Dec 2009 (DB)
// ---------------------------------------------------------------------------
// This file is part of Gambit plugin: (lib)gambit_fileformat_vcf.
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
// Implements GAbstractFileReader to handle VCF format.
// ***************************************************************************

#ifndef G_VCFREADER_H
#define G_VCFREADER_H

#include "SessionManager/FileManager/GAbstractFileReader.h"
#include "DataStructures/GReference.h"
class QString;

namespace Gambit {
namespace FileIO {

class GVcfReader : public GAbstractFileReader {

    public:
        GVcfReader(void);
        ~GVcfReader(void);

    public:
        bool Close(void);
        bool LoadData(GGenomicDataSet& data);
        bool Open(const GFileInfo& fileInfo);
        bool LoadReferences(GReferenceList& references);

    private:
        struct GVcfReaderPrivate;
        GVcfReaderPrivate* d;
};

} // namespace FileIO
} // namespace Gambit

#endif // G_VCFREADER_H
