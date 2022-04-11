// ***************************************************************************
// GFastaReader.cpp (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 17 Dec 2009 (DB)
// ---------------------------------------------------------------------------
// This file is part of Gambit plugin: (lib)gambit_fileformat_fasta.
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
// Implements GAbstractFileReader to handle FASTA format.
// ***************************************************************************

#include <QtCore>
#include <QProgressDialog>
#include <QtDebug>
#include "./GFastaReader.h"
#include "DataStructures/GFileInfo.h"
#include "DataStructures/GGenomicDataSet.h"
using namespace Gambit;
using namespace Gambit::FileIO;

struct GFastaReader::GFastaReaderPrivate {

    // internal data structures
    struct GFastaIndexData {
        QString Name;
        qint32  Length;
        qint64  Offset;
        qint32  LineLength;
        qint32  ByteLength; // LineLength + newline character(s) - varies on OS where file was generated
    };

    // data members
    QFile File;
    QFile IndexFile;
    QList<GFastaIndexData> Index;
    bool IsReaderOpen;

    // constructor
    GFastaReaderPrivate(void) : IsReaderOpen(false) { }
    ~GFastaReaderPrivate(void) { Close(); }

    // 'private' general file handling
    bool Close(void);
    bool Open(const GFileInfo& fileInfo);

    // 'private' index file handling
    void CreateIndex(void);
    void LoadIndex(void);
    void SaveIndex(void);

    // 'private' data load methods
    const QString LoadSequence(GGenomicDataRegion& region);
    const GReferenceList LoadReferences(void);

    // 'private' utility methods
    QPair<qint64,qint64> CalculateOffsets(const quint32& refID,
                                          const qint32&  left,
                                          const qint32&  right);
    const quint32 GetReferenceID(const QString& refName);
    void TrimSequenceWhitespace(QString& sequence);
};


GFastaReader::GFastaReader(void)
    : GAbstractFileReader()
{
    d = new GFastaReaderPrivate;
}

GFastaReader::~GFastaReader(void) {
    delete d;
    d = 0;
}

bool GFastaReader::Close(void) {
    return d->Close();
}

bool GFastaReader::LoadData(GGenomicDataSet& data) {
    if ( !d->IsReaderOpen ) { return false; }
    data.Sequence = d->LoadSequence(data.Region);
    return true;
}

bool GFastaReader::LoadReferences(GReferenceList& references) {
    Q_UNUSED(references);
    return false;
}

bool GFastaReader::Open(const GFileInfo& fileInfo) {
    return d->Open(fileInfo);
}

// ------------------------------------------
// GFastaReaderPrivate implementation
// ------------------------------------------

bool GFastaReader::GFastaReaderPrivate::Close(void) {
    File.close();
    Index.clear();
    IsReaderOpen = false;
    return true;
}

bool GFastaReader::GFastaReaderPrivate::Open(const GFileInfo& fileInfo) {

    // skip if reader already opened
    if ( IsReaderOpen ) { return false; }

    // set filenames
    File.setFileName(fileInfo.Filename);
    IndexFile.setFileName(fileInfo.IndexFilename);

    // open FASTA file
    if ( File.open(QIODevice::ReadOnly) ) {

        // if no index filename provided, we need to generate an index
        if ( fileInfo.IndexFilename.isEmpty() ) {

            // close index if it's open - should not be
            if ( IndexFile.isOpen() ) { IndexFile.close(); }

            // set default index filename for index creation
            QString createIndexFilename = fileInfo.Filename;
            createIndexFilename.append( QString(".fai") );
            IndexFile.setFileName(createIndexFilename);

            // create index data structure in memory
            CreateIndex();

            // save index data to file
            SaveIndex();
        }

        // load index data from index file
        else { LoadIndex(); }

        // set flag
        IsReaderOpen = true;

    } else { IsReaderOpen = false;  /*output error?*/ }

    return true;
}

// NB - Method IS allowed to modify 'right' (reference to GGenomicDataSet::RightBound)
// in cases where desired region is longer than actual data available
const QString
GFastaReader::GFastaReaderPrivate::LoadSequence(GGenomicDataRegion& region)
{
    // intialize sequence string
    QString sequence = "";

    // get reference ID from name
    quint32 refID = GetReferenceID(region.RefName);
    if ( refID >= (quint32)Index.size() ) { qDebug() << "Reference not found."; return sequence; }

    // adjust right bound if greater than ref-seq length
    qint32 refMaxLength = Index.at(refID).Length;
    if (region.RightBound > refMaxLength) { region.RightBound = refMaxLength; }

    // if invalid input device or index, quit
    if ( !File.isOpen() )    { qDebug() << "File not open."; return sequence; }
    if ( Index.size() == 0 ) { qDebug() << "Invalid index";  return sequence; }

    // calculate offset of FASTA entry start
    QPair<qint64,qint64> offsets = CalculateOffsets(refID, region.LeftBound, region.RightBound);
    if ( (offsets.first == -1) ) { qDebug() << "Could not find FASTA entry"; return sequence; }

    // seek to beginning of desired FASTA region
    QTextStream stream(&File);
    if ( !stream.seek(offsets.first) ) { qDebug() << "Could not seek"; return sequence; }

    // read sequence
    sequence = stream.read( offsets.second );

    // remove all internal whitespace and return
    TrimSequenceWhitespace(sequence);
    return sequence.toUpper();
}

// ** need to speed up this naive implementation - WAY too slow **
void GFastaReader::GFastaReaderPrivate::CreateIndex(void) {

    // check valid filestream
    if ( !File.isOpen() ) { return; }

    // construct new FASTA index data structure
    Index.clear();

    // initialize text stream... find first text entry
    QTextStream stream(&File);
    stream.seek(0);
    stream.skipWhiteSpace();

    QProgressDialog progressDialog;
    progressDialog.setMinimumWidth(300);
    progressDialog.setCancelButtonText("&Cancel");
    progressDialog.setRange(0, File.size() );
    progressDialog.setWindowTitle("Loading FASTA index");
    progressDialog.setLabelText("Calculating offsets...");

    // parsing variables
    QString     line;
    QStringList fields;
    qint64      previousLineOffset;

    quint32 lineNumber = 0;

    // while data exists
    while ( !stream.atEnd() ) {

        line = stream.readLine();

        // header found
        if (line.startsWith('>')) {

            GFastaIndexData entry;
            fields = line.split(' ');

            // if space between '>' and next text
            if ( fields.at(0).size() == 1 ) {
                // save the next field as Name
                entry.Name = fields.at(1).trimmed();
            }

            // if no space between '>' and text
            else {
                // remove '>' and save rest of text (until space) as Name
                QString name = fields.at(0);
                entry.Name = name.right( name.length() - 1 ).trimmed();
            }

            // store offset data
            entry.Offset = stream.pos();
            previousLineOffset = entry.Offset;

            // skip whitespace, get next data line (should be actual DNA sequence)
            stream.skipWhiteSpace();
            line = stream.readLine();
            ++lineNumber;

            // store line length ( DNA sequence, physical char's on line (seqLength + 1 ('\n')) )
            entry.LineLength = line.length();

            // check return type used by file, adjust line byte length accordingly ( lineLength +1 or +2 )
            if ( stream.seek( stream.pos() - 2) ) {
                QString returnCheck = stream.read(2);
                if ( returnCheck == QString::fromAscii("\r\n") ) {
                    entry.ByteLength = entry.LineLength + 2;
                } else {
                    entry.ByteLength = entry.LineLength + 1;
                }
            }

            QString sequence;
            while ( !line.isEmpty() ) {

                // if we bump the next header line
                if ( line.startsWith('>') ) {
                    // back the stream up, break out of this loop
                    stream.seek(previousLineOffset);
                    break;
                }

                // save DNA sequence from this line
                sequence.append(line);

                // store offset data, get next line of sequence
                previousLineOffset = stream.pos();
                line = stream.readLine();
                ++lineNumber;

                // update progress bar
                progressDialog.setValue(previousLineOffset);
                qApp->processEvents();
                if (progressDialog.wasCanceled()) { return; }
            }

            // store sequence length
            entry.Length = sequence.length();

            // save index data
            Index.append(entry);
        }
    }
}

void GFastaReader::GFastaReaderPrivate::LoadIndex(void) {

    // if index file not open, open 'ReadOnly'
    if ( !IndexFile.isOpen() ) { IndexFile.open(QIODevice::ReadOnly); }

    // set up data (i.e. binary) stream for file pointer
    QTextStream indexStream(&IndexFile);

    // clear index data
    Index.clear();

    QString     line;
    QStringList fields;
    while ( !indexStream.atEnd() ) {

        line = indexStream.readLine();
        fields = line.split('\t');

        // get index data for reference seq from file
        GFastaIndexData indexEntry;
        indexEntry.Name       = fields.at(0);
        indexEntry.Length     = fields.at(1).toInt();
        indexEntry.Offset     = fields.at(2).toLongLong();
        indexEntry.LineLength = fields.at(3).toInt();
        indexEntry.ByteLength = fields.at(4).toInt();

        // store reference data in total index structure
        Index.append(indexEntry);
    }

    // there should be at least 1
    if ( Index.size() == 0 ) {
        qFatal("GFastaReader::LoadIndex() => no index data loaded");
    }

    // close index file
    IndexFile.close();
}

void GFastaReader::GFastaReaderPrivate::SaveIndex(void) {

    // if index file not open, open 'WriteOnly'
    if ( !IndexFile.isOpen() ) { IndexFile.open(QIODevice::WriteOnly); }

    // set up data (i.e. binary) stream for file pointer
    QTextStream indexStream( &IndexFile );

    // for each reference sequence, write index data to file
    foreach (GFastaIndexData entry, Index) {
        indexStream << entry.Name       << "\t";
        indexStream << entry.Length     << "\t";
        indexStream << entry.Offset     << "\t";
        indexStream << entry.LineLength << "\t";
        indexStream << entry.ByteLength << endl;
    }

    // close index file
    IndexFile.close();
}

QPair<qint64,qint64>
GFastaReader::GFastaReaderPrivate::CalculateOffsets(const quint32& refID,
                                                    const qint32& left,
                                                    const qint32& right)
{
    // get index entry
    GFastaIndexData entry(Index.at(refID));

    // use adjusted coordinates (string indexes are 0-based, Gambit coordinates are 1-based)
    qint32 fastaLeft  = left - 1;
    qint32 fastaRight = right - 1;

    // calculate start offset
    quint32 numLinesToSkip = int(fastaLeft/entry.LineLength);
    quint32 positionOnLine = int(fastaLeft%entry.LineLength);
    qint64  startOffset    = qint64( entry.Offset + (numLinesToSkip*entry.ByteLength) + positionOnLine );

    // calculate end offset
    quint32 lineRemainderWithNewline = entry.ByteLength - positionOnLine;
    quint32 regionRemaining          = ((fastaRight-fastaLeft)+1) - lineRemainderWithNewline;

    quint32 fullLinesToRead = 0;
    for ( ; int(regionRemaining/entry.LineLength) > 0; ++fullLinesToRead ) {
        regionRemaining -= entry.LineLength;
    }
    qint64 bytesToRead = qint64( lineRemainderWithNewline + (fullLinesToRead*entry.ByteLength) + regionRemaining + 1 );

    // return offset pair
    return qMakePair(startOffset, bytesToRead);
}

const quint32 GFastaReader::GFastaReaderPrivate::GetReferenceID(const QString& refName) {
    quint32 id = 0;
    foreach (GFastaIndexData entry, Index) {
        if ( entry.Name == refName ) { break; }
        ++id;
    }
    return id;
}

void GFastaReader::GFastaReaderPrivate::TrimSequenceWhitespace(QString& sequence) {

    // initialize data buffers, indices, and boundaries
    int seqSize = sequence.size();
    if (seqSize == 0) { return; }

    const QChar* from    = (const QChar*) sequence.data();
    const QChar* fromEnd = (const QChar*) from + seqSize;

    QString trimmedSeq;
    trimmedSeq.resize(seqSize);
    QChar* to = (QChar*)trimmedSeq.data();
    int outc = 0;

    // iterate over sequence data
    // relies on conditional to break out at end
    forever {

        // skip any leading whitespace
        while(from!=fromEnd && from->isSpace()) {
            from++;
        }

        // store any non-whitespace chars
        while(from!=fromEnd && !from->isSpace()) {
            to[outc++] = *from++;
        }

        // if end of sequence, quit...
        // otherwise, do nothing - next iteration
        // 'skip any leading whitespace' loop will handle any whitespaces found at current position
        if (from==fromEnd) { break; }
    }

    // trim any trailing whitespace
    if ( (outc > 0) && (to[outc-1].isSpace()) ) { outc--; }
    trimmedSeq.truncate(outc);

    // set FASTA sequence to whitespace-free copy
    sequence = trimmedSeq;
}
