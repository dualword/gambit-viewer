// ***************************************************************************
// GBamReader.cpp (c) 2009 Derek Barnett
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
// Implements GAbstractFileReader to handle BAM format.
// ***************************************************************************

#include <QtCore>
#include <QtConcurrentMap>
#include <QtDebug>
#include "./GBamReader.h"
#include "DataStructures/GAlignment.h"
#include "DataStructures/GColorScheme.h"
#include "DataStructures/GFileInfo.h"
#include "DataStructures/GGenomicDataSet.h"
using namespace Gambit;
using namespace Gambit::FileIO;

#include "BamReader.h"
using namespace BamTools;

struct GBamReader::GBamReaderPrivate {

    // 'private' data
    BamReader Reader;
    bool      IsReaderOpen;

    // constructor
    GBamReaderPrivate(void) : IsReaderOpen(false) { }
    ~GBamReaderPrivate(void) { Close(); }

    // 'private' general file handling
    bool Close(void);
    bool Open(const GFileInfo& fileInfo);

    // 'private' data load methods
    const GAlignmentList LoadAlignments(const GGenomicDataRegion& region);

    bool LoadReferences(GReferenceList& references);
};

GBamReader::GBamReader(void)
    : GAbstractFileReader()
{
    d = new GBamReaderPrivate;
}

GBamReader::~GBamReader(void) { 
    delete d;
    d = 0;
}

bool GBamReader::Open(const GFileInfo& fileInfo) {
    return d->Open(fileInfo);
}

bool GBamReader::Close(void) {
    return d->Close();
}

bool GBamReader::LoadData(GGenomicDataSet& data) {
    if ( !d->IsReaderOpen ) { return false; }
    data.Alignments += d->LoadAlignments(data.Region);
    return true;
}

bool GBamReader::LoadReferences(GReferenceList& references) {
    return d->LoadReferences(references);
}

bool GBamReader::GBamReaderPrivate::Close(void) {

    // skip if not open
    if ( !IsReaderOpen ) { return false; }

    // close reader
    Reader.Close();
    IsReaderOpen = false;

    return true;
}

bool GBamReader::GBamReaderPrivate::Open(const GFileInfo& fileInfo) {

    // skip if already open
    if ( IsReaderOpen ) { return false; }

    // open reader, create index if necessary
    if ( fileInfo.IndexFilename.isEmpty() ) {
        Reader.Open(fileInfo.Filename.toStdString());
        Reader.CreateIndex();
    } else {
        Reader.Open(fileInfo.Filename.toStdString(), fileInfo.IndexFilename.toStdString());
    }

    // set flag return success (fail?)
    IsReaderOpen = true;
    return true;
}

const GAlignmentList
GBamReader::GBamReaderPrivate::LoadAlignments(const GGenomicDataRegion& region) {

    const qint32 refID = Reader.GetReferenceID( region.RefName.toStdString() );

    // try to jump to specified region
    if ( !Reader.Jump(refID, region.LeftBound) ) { return GAlignmentList(); }

    // create container of BamAlignments
    QList<BamAlignment> bAlignments;

    // populate BamAlignment container
    BamAlignment bAlignment;
    while ( Reader.GetNextAlignment(bAlignment) ) {

        // increment position by 1 (BAM is 0-based, Gambit is 1-based)
        ++bAlignment.Position;

        if ((bAlignment.Position >= region.LeftBound) && (bAlignment.Position <= region.RightBound)) {
            bAlignments.append(bAlignment);
        }

        // alignment positions are starting beyond rightbound, just stop checking
        if ( bAlignment.Position >= region.RightBound) { break; }
    }

    // convert BamAlignments to GAlignments
    // Qt 4.5 introduced a simple interface for multi-threading this type of operation
    // otherwise, just iterate 'normally'

    GAlignmentList alignments;

#if QT_VERSION >= 0x040500
    alignments = QtConcurrent::blockingMapped(bAlignments, Gambit::FileIO::ConvertAlignment);
#else
    foreach (bAlignment, bAlignments) {
        GAlignment gAlignment = ConvertAlignment(bAlignment);
        alignments.append(gAlignment);
    }
#endif

    // remove empty alignments (no sequence present... happens in odd cases)
    QMutableListIterator<GAlignment> alignIter(alignments);
    while (alignIter.hasNext()) {
        GAlignment gAlignment = alignIter.next();
        if (gAlignment.Bases == "") {
            alignIter.remove();
        }
    }

    // sort alignments by position before return
    qSort(alignments.begin(), alignments.end(), GAlignment::SortByPosition());
    return alignments;
}

bool GBamReader::GBamReaderPrivate::LoadReferences(GReferenceList& references) {

    references.clear();

    // get reference data
    RefVector refData = Reader.GetReferenceData();

    // iterate over references, get names of references that have alignments
    RefVector::const_iterator refIter = refData.begin();
    RefVector::const_iterator refEnd  = refData.end();

    quint32 id = 0;
    for ( ; refIter != refEnd; ++refIter) {

        QString name  = QString( (*refIter).RefName.c_str() );
        qint32 length = qint32(  (*refIter).RefLength );

        GReference aReference(id, name, length);
        references.append(aReference);
        ++id;
    }

    return true;    // error case anywhere?
}

GAlignment Gambit::FileIO::ConvertAlignment(const BamAlignment& bAlignment) {

    // build new GAlignment from BamAlignment data
    GAlignment gAlignment;
    gAlignment.Name       = QString( bAlignment.Name.c_str() );
    gAlignment.Bases      = QString( bAlignment.AlignedBases.c_str() );
    gAlignment.Length     = gAlignment.Bases.length();
    gAlignment.Qualities  = QString( bAlignment.Qualities.c_str() );
    gAlignment.RefId      = qint32( bAlignment.RefID );
    gAlignment.Position   = qint32( bAlignment.Position );     // position already converted to 1-based above
    gAlignment.MapQuality = quint32( bAlignment.MapQuality );
    gAlignment.IsReverseComplement = bAlignment.IsReverseStrand();

    // store alignment flag
    gAlignment.Flags = QFlag(bAlignment.AlignmentFlag);

    // initialize PaddedBases with (regular) Bases
    gAlignment.PaddedBases = gAlignment.Bases;

    // will track aligned genomic position, not position on read
    quint32 genomicPosition = gAlignment.Position;

    // iterate over Bam CIGAR operations
    QVector<CigarOp> cigarOps = QVector<CigarOp>::fromStdVector( bAlignment.CigarData );

    CigarOp op;
    foreach (op, cigarOps) {

        // if insertion found
        if (op.Type == 'I') {
            // store insertion position/length
            gAlignment.Insertions.insert(genomicPosition, qint32(op.Length));
        } else {
            // update genomic position
            genomicPosition += op.Length;
        }
    }

    // store read group string
    std::string readGroupText;
    if ( bAlignment.GetReadGroup(readGroupText) ) {
        gAlignment.ReadGroup = QString( readGroupText.c_str() );
    }

    return gAlignment;
}
