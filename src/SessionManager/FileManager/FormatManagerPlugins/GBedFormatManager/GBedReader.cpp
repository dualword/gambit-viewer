// ***************************************************************************
// GBedReader.cpp (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 17 Dec 2009 (DB)
// ---------------------------------------------------------------------------
// This file is part of Gambit plugin: (lib)gambit_fileformat_bed.
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
// Implements GAbstractFileReader to handle BED format.
// ***************************************************************************

#include <QtCore>
#include <QProgressDialog>
#include <QtDebug>
#include "./GBedReader.h"
#include "DataStructures/GFileInfo.h"
#include "DataStructures/GGene.h"
#include "DataStructures/GGenomicDataSet.h"
#include "DataStructures/GSnp.h"
using namespace Gambit;
using namespace Gambit::FileIO;

struct GBedReader::GBedReaderPrivate {

    // data members
    QFile File;
    bool  IsReaderOpen;
    GFileInfo::FileType Type;

    // constructor
    GBedReaderPrivate(void) : IsReaderOpen(false), Type(GFileInfo::File_Gene) { }
    ~GBedReaderPrivate(void) { Close(); }

    // general file handling
    bool Close(void);
    bool Open(const GFileInfo& fileInfo);
    bool LoadData(GGenomicDataSet& data);

    // used internally for specific data access
    private:
        const GGeneList LoadGenes(const GGenomicDataRegion& region);
        const GSnpList  LoadSnps(const GGenomicDataRegion& region);
        GGene LoadSingleGene(const QStringList& fields);
        GSnp  LoadSingleSnp(const QStringList& fields);

        // note - BED uses 0-based coordinates
        enum BedField { CHROM_NAME = 0
                      , CHROM_START
                      , CHROM_END
                      , NAME
                      , SCORE
                      , STRAND
                      , THICK_START
                      , THICK_END
                      , RGB_VALUE
                      , BLOCK_COUNT
                      , BLOCK_SIZES
                      , BLOCK_STARTS
        };
};

GBedReader::GBedReader(void)
    : GAbstractFileReader()
{
    d = new GBedReaderPrivate;
}

GBedReader::~GBedReader(void) {
    delete d;
    d = 0;
}

bool GBedReader::Close(void) {
    return d->Close();
}

bool GBedReader::LoadData(GGenomicDataSet& data) {
    if ( !d->IsReaderOpen ) { return false; }
    return d->LoadData(data);
}

bool GBedReader::LoadReferences(GReferenceList& references) {
    Q_UNUSED(references);
    return false;
}

bool GBedReader::Open(const GFileInfo& fileInfo) {
    return d->Open(fileInfo);
}

// ------------------------------------------
// GFastaReaderPrivate implementation
// ------------------------------------------

bool GBedReader::GBedReaderPrivate::Close(void) {
    File.close();
    IsReaderOpen = false;
    return true;
}

bool GBedReader::GBedReaderPrivate::Open(const GFileInfo& fileInfo) {

    // skip if reader already opened
    if ( IsReaderOpen ) { return false; }

    // set filename & type
    File.setFileName(fileInfo.Filename);
    Type = fileInfo.Type;

    // open BED file
    if ( File.open(QIODevice::ReadOnly)) { IsReaderOpen = true; }

    // return success/fail
    return IsReaderOpen;
}

bool GBedReader::GBedReaderPrivate::LoadData(GGenomicDataSet& data) {

    // skip if reader not open
    if ( !IsReaderOpen ) { return false; }

    switch ( Type ) {
        case (GFileInfo::File_Gene) :
            data.Genes = LoadGenes(data.Region);
            break;

        case (GFileInfo::File_Snp ) :
            data.Snps = LoadSnps(data.Region);
            break;

        default : qDebug() << "Unknown file type in GBedReader..."; return false;
    }

    // return success
    return true;
}

const GGeneList GBedReader::GBedReaderPrivate::LoadGenes(const GGenomicDataRegion& region) {

    // initialize gene list
    GGeneList genes;

    // use adjusted coordinates for range checking (BED is 0-based (half-open), Gambit is 1-based)
    qint32 bedLeft  = region.LeftBound - 1;
    qint32 bedRight = region.RightBound;

    // initialize string parsing variables
    QString line = "";
    QStringList fields;

    // rewind file pointer
    File.seek(0);

    // while data exists
    while ( !File.atEnd() ) {

        // get line data
        line   = File.readLine();
        fields = line.split('\t');

        // if entry in desired region
        if ( (fields.at(GBedReader::GBedReaderPrivate::CHROM_NAME)          == region.RefName) &&
             (fields.at(GBedReader::GBedReaderPrivate::CHROM_START).toInt() >= bedLeft) &&
             (fields.at(GBedReader::GBedReaderPrivate::CHROM_END).toInt()   <= bedRight)
            )
        {
            // append new gene entry
            genes.append( LoadSingleGene(fields) );
        }
    }

    return genes;
}

const GSnpList GBedReader::GBedReaderPrivate::LoadSnps(const GGenomicDataRegion& region) {

    // intialize snp list
    GSnpList snps;

    // use adjusted leftBound coordinate for range checking (BED is half-open, 0-based... Gambit is 1-based)
    qint32 bedLeft  = region.LeftBound - 1;
    qint32 bedRight = region.RightBound;

    // initialize string parsing variables
    QString line = "";
    QStringList fields;

    // rewind
    File.seek(0);

    // while data exists
    while ( !File.atEnd() ) {

        // get line data
        line   = File.readLine();
        fields = line.split('\t');

        // if entry in desired region
        if ( (fields.at(GBedReader::GBedReaderPrivate::CHROM_NAME)          == region.RefName) &&
             (fields.at(GBedReader::GBedReaderPrivate::CHROM_START).toInt() >= bedLeft)    &&
             (fields.at(GBedReader::GBedReaderPrivate::CHROM_END).toInt()   <= bedRight)
            )
        {
            // append new snp entry
            snps.append( LoadSingleSnp(fields) );
        }
    }

    return snps;
}

GGene GBedReader::GBedReaderPrivate::LoadSingleGene(const QStringList& fields) {

    // construct new gene
    GGene gGene;
    gGene.Name  = fields.at(GBedReader::GBedReaderPrivate::NAME).trimmed();
    gGene.Start = fields.at(GBedReader::GBedReaderPrivate::CHROM_START).toInt() + 1; // adjust to 1-based coordinates
    gGene.Stop  = fields.at(GBedReader::GBedReaderPrivate::CHROM_END).toInt();

    // if strand data available, get it... else set default
    if ( fields.size() > GBedReader::GBedReaderPrivate::STRAND ) {
        gGene.Strand = fields.at(GBedReader::GBedReaderPrivate::STRAND).trimmed();
    } else {
        gGene.Strand = "?";
    }

    // if exon data available, get it
    if (fields.size() > GBedReader::GBedReaderPrivate::BLOCK_COUNT) {

        // iterate over exons
        int numExons = fields.at(GBedReader::GBedReaderPrivate::BLOCK_COUNT).toInt();
        QStringList exonLengths = fields.at(GBedReader::GBedReaderPrivate::BLOCK_SIZES).split(',');
        QStringList exonStarts  = fields.at(GBedReader::GBedReaderPrivate::BLOCK_STARTS).split(',');
        for(int i = 0; i < numExons; ++i) {

            // construct new exon
            GExon gExon;
            gExon.Start = gGene.Start + exonStarts.at(i).toInt();     // exon start is relative to gene start
            gExon.Stop  = gExon.Start + exonLengths.at(i).toInt();    // exon stop is exon start plus exon length

            // store exon in gene
            gGene.Exons.append(gExon);
        }
    }

    return gGene;
}

GSnp GBedReader::GBedReaderPrivate::LoadSingleSnp(const QStringList& fields) {

    // construct new SNP
    GSnp gSnp;
    gSnp.Position = fields.at(GBedReader::GBedReaderPrivate::CHROM_START).toInt() + 1; // adjust to 1-based coordinates

    // if name availale, set snp name
    if (fields.size() > GBedReader::GBedReaderPrivate::NAME) {
        gSnp.Name = fields.at(GBedReader::GBedReaderPrivate::NAME);
    }

    // if score data available, set snp score
    if (fields.size() > GBedReader::GBedReaderPrivate::SCORE) {
        gSnp.Score = fields.at(GBedReader::GBedReaderPrivate::SCORE).toDouble();
    }

    return gSnp;
}
