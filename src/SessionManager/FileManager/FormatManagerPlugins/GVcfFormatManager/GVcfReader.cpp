// ***************************************************************************
// GVcfReader.cpp (c) 2009 Derek Barnett
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

#include <QtCore>
#include <QProgressDialog>
#include <QtDebug>
#include "./GVcfReader.h"
#include "DataStructures/GFileInfo.h"
#include "DataStructures/GGene.h"
#include "DataStructures/GGenomicDataSet.h"
#include "DataStructures/GSnp.h"
using namespace Gambit;
using namespace Gambit::FileIO;

struct GVcfReader::GVcfReaderPrivate {

    // data members
    QFile File;
    bool  IsReaderOpen;
    GFileInfo::FileType Type;

    // constructor
    GVcfReaderPrivate(void) : IsReaderOpen(false), Type(GFileInfo::File_Gene) { }
    ~GVcfReaderPrivate(void) { Close(); }

    // general file handling
    bool Close(void);
    bool Open(const GFileInfo& fileInfo);
    bool LoadData(GGenomicDataSet& data);

    // used internally for specific data access
    private:
        const GSnpList LoadSnps(const GGenomicDataRegion& region);
        GSnpList LoadSnpLine(const QStringList& fields);

        // VCF is 1-based coordinates
        enum VcfField { CHROM_NAME = 0
                      , POSITION
                      , FEATURE_ID
                      , REF_ALLELE
                      , ALT_ALLELES
                      , QUAL
                      , FILTER
                      , ADDL_INFO
        };
};

// ------------------------------------------
// GVcfReader implementation
// ------------------------------------------

GVcfReader::GVcfReader(void)
    : GAbstractFileReader()
{
    d = new GVcfReaderPrivate;
}

GVcfReader::~GVcfReader(void) {
    delete d;
    d = 0;
}

bool GVcfReader::Close(void) {
    return d->Close();
}

bool GVcfReader::LoadData(GGenomicDataSet& data) {
    if ( !d->IsReaderOpen ) { return false; }
    return d->LoadData(data);
}

bool GVcfReader::LoadReferences(GReferenceList& references) {
    Q_UNUSED(references);
    return false;
}

bool GVcfReader::Open(const GFileInfo& fileInfo) {
    return d->Open(fileInfo);
}

// ------------------------------------------
// GVcfReaderPrivate implementation
// ------------------------------------------

bool GVcfReader::GVcfReaderPrivate::Close(void) {
    File.close();
    IsReaderOpen = false;
    return true;
}

bool GVcfReader::GVcfReaderPrivate::Open(const GFileInfo& fileInfo) {

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

bool GVcfReader::GVcfReaderPrivate::LoadData(GGenomicDataSet& data) {

    // skip if reader not open
    if ( !IsReaderOpen ) { return false; }

    // load data
    data.Snps = LoadSnps(data.Region);

    // return success
    return true;
}

const GSnpList GVcfReader::GVcfReaderPrivate::LoadSnps(const GGenomicDataRegion& region) {

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

        // skip header comments
        if ( line.startsWith("#")) { continue; }

        // retrieve tab-delimited fields
        fields = line.split('\t');

        // if entry in desired region
        if ( (fields.at(GVcfReader::GVcfReaderPrivate::CHROM_NAME)       == region.RefName) &&
             (fields.at(GVcfReader::GVcfReaderPrivate::POSITION).toInt() >= region.LeftBound)
            )
        {
            // append new snp entry
            snps.append( LoadSnpLine(fields) );
        }
    }

    return snps;
}

GSnpList GVcfReader::GVcfReaderPrivate::LoadSnpLine(const QStringList& fields) {

    GSnpList snps;
    QString altAlleles = fields.at(GVcfReader::GVcfReaderPrivate::ALT_ALLELES);
    QStringList alleleList = altAlleles.split(',');

    foreach (QString alleleEntry, alleleList) {
        GSnp gSnp;
        gSnp.Position = fields.at(GVcfReader::GVcfReaderPrivate::POSITION).toInt();
        gSnp.Name     = fields.at(GVcfReader::GVcfReaderPrivate::FEATURE_ID);
        gSnp.Score    = fields.at(GVcfReader::GVcfReaderPrivate::QUAL).toDouble();
        snps.append(gSnp);
    }

    return snps;

}
