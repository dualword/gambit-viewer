// ***************************************************************************
// GGff3Reader.cpp (c) 2009 Derek Barnett
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

#include <QtCore>
#include <QProgressDialog>
#include <QtDebug>
#include "./GGff3Reader.h"
#include "DataStructures/GFileInfo.h"
#include "DataStructures/GGene.h"
#include "DataStructures/GGenomicDataSet.h"
#include "DataStructures/GSnp.h"
using namespace Gambit;
using namespace Gambit::FileIO;

struct GGff3Reader::GGff3ReaderPrivate {

    // data members
    QFile File;
    bool  IsReaderOpen;
    GFileInfo::FileType Type;

    // constructor
    GGff3ReaderPrivate(void) : IsReaderOpen(false), Type(GFileInfo::File_Gene) { }
    ~GGff3ReaderPrivate(void) { Close(); }

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

        // GFF2 is 1-based coordinates
        enum Gff3Field { CHROM_NAME = 0
                      , SOURCE
                      , FEATURE_NAME
                      , FEATURE_START
                      , FEATURE_STOP
                      , SCORE
                      , STRAND
                      , FRAME
                      , ATTRIBUTES
                      , COMMENTS
        };
};

GGff3Reader::GGff3Reader(void)
    : GAbstractFileReader()
{
    d = new GGff3ReaderPrivate;
}

GGff3Reader::~GGff3Reader(void) {
    delete d;
    d = 0;
}

bool GGff3Reader::Close(void) {
    return d->Close();
}

bool GGff3Reader::LoadData(GGenomicDataSet& data) {
    if ( !d->IsReaderOpen ) { return false; }
    return d->LoadData(data);
}

bool GGff3Reader::LoadReferences(GReferenceList& references) {
    Q_UNUSED(references);
    return false;
}

bool GGff3Reader::Open(const GFileInfo& fileInfo) {
    return d->Open(fileInfo);
}

// ------------------------------------------
// GFastaReaderPrivate implementation
// ------------------------------------------

bool GGff3Reader::GGff3ReaderPrivate::Close(void) {
    File.close();
    IsReaderOpen = false;
    return true;
}

bool GGff3Reader::GGff3ReaderPrivate::Open(const GFileInfo& fileInfo) {

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

bool GGff3Reader::GGff3ReaderPrivate::LoadData(GGenomicDataSet& data) {

    // skip if reader not open
    if ( !IsReaderOpen ) { return false; }

    switch ( Type ) {

        case ( GFileInfo::File_Gene ) :
            data.Genes = LoadGenes(data.Region);
            break;

        case ( GFileInfo::File_Snp  ) :
            data.Snps = LoadSnps(data.Region);
            break;

        default : qDebug() << "Unknown file type in GGff3Reader..."; return false;
    }

    // return success
    return true;
}

const GGeneList GGff3Reader::GGff3ReaderPrivate::LoadGenes(const GGenomicDataRegion& region) {

    // initialize gene list
    GGeneList genes;

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
        if ( (fields.at(GGff3Reader::GGff3ReaderPrivate::CHROM_NAME)             == region.RefName) &&
             (fields.at(GGff3Reader::GGff3ReaderPrivate::FEATURE_START).toInt() >= region.LeftBound)    &&
             (fields.at(GGff3Reader::GGff3ReaderPrivate::FEATURE_STOP).toInt()  <= region.RightBound)
            )
        {
            // append new gene entry
            genes.append( LoadSingleGene(fields) );
        }
    }

    return genes;
}

const GSnpList GGff3Reader::GGff3ReaderPrivate::LoadSnps(const GGenomicDataRegion& region) {

    // intialize snp list
    GSnpList snps;

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
        if ( (fields.at(GGff3Reader::GGff3ReaderPrivate::CHROM_NAME)             == region.RefName) &&
             (fields.at(GGff3Reader::GGff3ReaderPrivate::FEATURE_START).toInt() >= region.LeftBound)    &&
             (fields.at(GGff3Reader::GGff3ReaderPrivate::FEATURE_STOP).toInt()  <= region.RightBound)
            )
        {
            // append new snp entry
            snps.append( LoadSingleSnp(fields) );
        }
    }

    return snps;
}

GGene GGff3Reader::GGff3ReaderPrivate::LoadSingleGene(const QStringList& fields) {

    // construct new gene
    GGene gGene;
    gGene.Name  = fields.at(GGff3Reader::GGff3ReaderPrivate::FEATURE_NAME).trimmed();
    gGene.Start = fields.at(GGff3Reader::GGff3ReaderPrivate::FEATURE_START).toInt();
    gGene.Stop  = fields.at(GGff3Reader::GGff3ReaderPrivate::FEATURE_STOP).toInt();

    // if strand data available, get it... else set default
    if ( fields.size() > GGff3Reader::GGff3ReaderPrivate::STRAND ) {
        gGene.Strand = fields.at(GGff3Reader::GGff3ReaderPrivate::STRAND).trimmed();
    } else {
        gGene.Strand = "?";
    }

    return gGene;
}

GSnp GGff3Reader::GGff3ReaderPrivate::LoadSingleSnp(const QStringList& fields) {

    // construct new SNP
    GSnp gSnp;
    gSnp.Position = fields.at(GGff3Reader::GGff3ReaderPrivate::FEATURE_START).toInt();

    // if name availale, set snp name
    if (fields.size() > GGff3Reader::GGff3ReaderPrivate::FEATURE_NAME) {
        gSnp.Name = fields.at(GGff3Reader::GGff3ReaderPrivate::FEATURE_NAME);
    }

    // if score data available, set snp score
    if (fields.size() > GGff3Reader::GGff3ReaderPrivate::SCORE ) {
        gSnp.Score = fields.at(GGff3Reader::GGff3ReaderPrivate::SCORE).toDouble();
    }

    return gSnp;
}
