// ***************************************************************************
// GGenomicDataPadder.cpp (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
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
// Describes the pre-processing tool that calculates and applies alignment
// padding to a dataset.
// ***************************************************************************

#include <QtCore>
#include <QtDebug>
#include <QProgressDialog>
#include <QtConcurrentMap>
#include "SessionManager/DataManager/GGenomicDataPadder.h"
#include "DataStructures/GGenomicDataSet.h"
using namespace Gambit;

void GGenomicDataPadder::Exec(GGenomicDataSet& data) {
    CalculatePadding(data);
    MergePadding(data);
}

void GGenomicDataPadder::CalculatePadding(GGenomicDataSet& data) {

    // get alignments, skip if empty
    GAlignmentList& alignments = data.Alignments;
    if (alignments.isEmpty()) { return; }

    // initialize progress dialog
    QProgressDialog progressDialog;
    progressDialog.setMinimumWidth(300);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, alignments.size());
    progressDialog.setWindowTitle(tr("Padding Alignments"));
    progressDialog.setLabelText(tr("Calculating padding..."));
    int currentAlignmentIndex = 0;

    // clear existing padding data
    GPaddingMap& padding = data.Padding;
    padding.clear();

    // iterate over all alignments
    for ( int index = 0; index < data.Alignments.size(); ++index ) {
        GAlignment& gAlignment = data.Alignments[index];

        // report progress to progress dialog
        progressDialog.setValue(currentAlignmentIndex);
        qApp->processEvents();
        if (progressDialog.wasCanceled()) { return; }

        // increment counter for progress dialog
        ++currentAlignmentIndex;

        // iterate over all insertions on alignment
        QMap<qint32, qint32>::const_iterator insIter = gAlignment.Insertions.constBegin();
        QMap<qint32, qint32>::const_iterator insEnd  = gAlignment.Insertions.constEnd();
        for ( ; insIter != insEnd; ++insIter ) {

            // get insertion data
            qint32 genomicPosition      = insIter.key();
            qint32 insertionsAtPosition = insIter.value();

            // if insertion position does not exist in the map yet OR
            // if numInsertions at current position greater than existing value in map
            if ( !padding.contains(genomicPosition) || ( insertionsAtPosition > padding.value(genomicPosition) ) ) {
                // store current insertion length for each insertion position
                padding.insert(genomicPosition, insertionsAtPosition);
            }
        }
    }
}

void GGenomicDataPadder::MergePadding(GGenomicDataSet& data) {
    ApplyPaddingToAlignments(data);
    ApplyPaddingToGenes(data);
    ApplyPaddingToSnps(data);
    ApplyPaddingToSequence(data);
}

void GGenomicDataPadder::ApplyPaddingToAlignments(GGenomicDataSet& data) {

    // get alignments, skip if empty
    GAlignmentList& alignments = data.Alignments;
    if (alignments.isEmpty()) { return; }

    // get padding data, skip if empty
    GPaddingMap& padding = data.Padding;
    if (padding.isEmpty() ) { return; }

    // initialize progress dialog
    QProgressDialog progressDialog;
    progressDialog.setMinimumWidth(300);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, alignments.size());
    progressDialog.setWindowTitle(tr("Padding Alignments"));
    progressDialog.setLabelText(tr("Applying padding..."));
    int currentAlignmentIndex = 0;

    // iterate over all alignments
    for ( int index = 0; index < data.Alignments.size(); ++index ) {
        GAlignment& gAlignment = data.Alignments[index];

        // report progress to progress dialog
        progressDialog.setValue(currentAlignmentIndex);
        qApp->processEvents();
        if (progressDialog.wasCanceled()) { return; }

        // update counter for progress dialog
        ++currentAlignmentIndex;

        // alignment coordinates/counters
        qint32 alignmentLeft  = gAlignment.Position;
        qint32 alignmentRight = alignmentLeft + gAlignment.Bases.length() - 1;
        qint32 positionInAlignment = 0;
        qint32 padsBeforeCurrentPosition = 0;
        qint32 padsBeforeAlignmentStart  = 0;

        // iterate over padding locations
        QMap<qint32, qint32>::const_iterator padIter = padding.constBegin();
        QMap<qint32, qint32>::const_iterator padEnd  = padding.constEnd();
        for ( ; padIter != padEnd; ++padIter ) {

            // get padding data
            qint32 genomicPosition   = padIter.key();
            qint32 maxPadsAtPosition = padIter.value();

            // if before alignment begin, store number of pads (used later in GAssemblyView to adjust drawing coordinates )
            if ( genomicPosition < alignmentLeft ) {
                padsBeforeAlignmentStart += maxPadsAtPosition;
            }

            else if ( positionInAlignment >= gAlignment.PaddedBases.length() ) {
                ; // skip this position
            }

            else {

                // if within alignment range
                if ( genomicPosition <= alignmentRight) {

                    qint32 padsToInsert = maxPadsAtPosition;

                    // if alignment already contains an insertion at this position
                    if ( gAlignment.Insertions.contains(genomicPosition) ) {

                        // subtract insertions in this read from max from all reads
                        padsToInsert -= gAlignment.Insertions.value(genomicPosition);
                    }

                    // adjust genomic position to position in alignment sequence, shift by pads inserted before this round
                    positionInAlignment = (genomicPosition - alignmentLeft) + padsBeforeCurrentPosition;

                    // insert padding at position
                    gAlignment.PaddedBases = gAlignment.PaddedBases.insert(positionInAlignment, QString(padsToInsert, '*') );

                    // update padding counter for this read
                    padsBeforeCurrentPosition += maxPadsAtPosition;
                }

                // beyond alignment end, stop processing
                else { break; }
            }
        }

        // save number of pads before alignment (GAssemblyView will use this to adjust drawing coordinates)
        gAlignment.PadsBefore = padsBeforeAlignmentStart;
    }
}

void GGenomicDataPadder::ApplyPaddingToGenes(GGenomicDataSet& data) {

    // get genes, skip if empty
    GGeneList& genes = data.Genes;
    if (genes.isEmpty()) { return; }

    // get padding data, skip if empty
    GPaddingMap& padding = data.Padding;
    if (padding.isEmpty() ) { return; }

    // initialize progress dialog
    QProgressDialog progressDialog;
    progressDialog.setMinimumWidth(300);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, genes.size());
    progressDialog.setWindowTitle(tr("Padding Genes"));
    progressDialog.setLabelText(tr("Applying padding..."));
    int currentGeneIndex = 0;

    // iterate over all genes
    for ( int index = 0; index < data.Genes.size(); ++index ) {
        GGene& gGene = data.Genes[index];

        // report progress to progress dialog
        progressDialog.setValue(currentGeneIndex);
        qApp->processEvents();
        if (progressDialog.wasCanceled()) { return; }

        // update counter for progress dialog
        ++currentGeneIndex;

        // gene coordinates/counters
        qint32 geneStart = gGene.Start;
        qint32 geneStop  = gGene.Stop;

        // iterate over padding locations
        QMap<qint32, qint32>::const_iterator padIter = padding.constBegin();
        QMap<qint32, qint32>::const_iterator padEnd  = padding.constEnd();
        for ( ; padIter != padEnd; ++padIter ) {

            // get padding data
            qint32 genomicPosition = padIter.key();
            qint32 padsAtPosition  = padIter.value();

            // if before gene begin, store number of pads (used later by GAssemblyView to adjust drawing coordinates )
            if ( genomicPosition < geneStart ) {
                gGene.StartOffset += padsAtPosition;
            }

            else {

                // if within range
                if ( genomicPosition <= geneStop) {

                    // update padding counter for this gene
                    gGene.StopOffset += padsAtPosition;
                }

                // beyond gene end, stop processing
                else { break; }
            }
        }
    }
}

void GGenomicDataPadder::ApplyPaddingToSequence(GGenomicDataSet& data) {

    // get sequence, skip if empty
    QString& sequence = data.Sequence;
    if (sequence.isEmpty()) { return; }

    // get padding data, skip if empty
    GPaddingMap& padding = data.Padding;
    if (padding.isEmpty() ) { return; }

    // initialize progress dialog
    QProgressDialog progressDialog;
    progressDialog.setMinimumWidth(300);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, padding.size() );
    progressDialog.setWindowTitle(tr("Padding Sequence"));
    progressDialog.setLabelText(tr("Applying padding..."));
    int currentPadIndex = 0;

    // keep track of pads inserted before current position
    qint32 padsBeforeCurrentPosition = 0;

    // iterate over padding locations
    QMap<qint32, qint32>::const_iterator padIter = padding.constBegin();
    QMap<qint32, qint32>::const_iterator padEnd  = padding.constEnd();
    for ( ; padIter != padEnd; ++padIter ) {

        // report progress to progress dialog
        progressDialog.setValue(currentPadIndex);
        qApp->processEvents();
        if (progressDialog.wasCanceled()) { return; }

        // adjust genomic position for selected range, then shift to account for previously inserted padding char's
        int positionInSequence = ( padIter.key() - data.Region.LeftBound ) + padsBeforeCurrentPosition;
        int numPads = padIter.value();

        // check validity of sequence index after adjustment    0 <= index < string.length()
        if (positionInSequence < 0) {
            qFatal("GGDPadder::ApplyPaddingToSequence() => string index less than zero");
        }

        // stop padding if beyond sequence length
        if ( positionInSequence >= sequence.length() ) { return; }

        // insert padding at position
        sequence.insert( positionInSequence, QString(numPads, '*') );

        // update padding counter
        padsBeforeCurrentPosition += numPads;

        // update progress counter
        ++currentPadIndex;
    }
}

void GGenomicDataPadder::ApplyPaddingToSnps(GGenomicDataSet& data) {

    // get snp data, skip if empty
    GSnpList& snps = data.Snps;
    if (snps.isEmpty()) { return; }

    // get padding data, skip if empty
    GPaddingMap& padding = data.Padding;
    if (padding.isEmpty() ) { return; }

    // initialize progress dialog
    QProgressDialog progressDialog;
    progressDialog.setMinimumWidth(300);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, snps.size());
    progressDialog.setWindowTitle(tr("Padding SNPs"));
    progressDialog.setLabelText(tr("Applying padding..."));
    int currentSnpIndex = 0;

    // iterate over all SNPs
    for ( int index = 0; index < data.Snps.size(); ++index ) {
        GSnp& gSnp = data.Snps[index];

        // report progress to progress dialog
        progressDialog.setValue(currentSnpIndex);
        qApp->processEvents();
        if (progressDialog.wasCanceled()) { return; }

        // update counter for progress dialog
        ++currentSnpIndex;

        // SNP position
        qint32 snpPosition = gSnp.Position;

        // iterate over padding locations
        QMap<qint32, qint32>::const_iterator padIter = padding.constBegin();
        QMap<qint32, qint32>::const_iterator padEnd  = padding.constEnd();
        for ( ; padIter != padEnd; ++padIter ) {

            // get padding data
            qint32 genomicPosition = padIter.key();
            qint32 padsAtPosition  = padIter.value();

            // if before SNP position, store number of pads (used later by GAssemblyView to adjust drawing coordinates )
            if ( genomicPosition <= snpPosition ) {
                gSnp.PaddingOffset += padsAtPosition;
            } else { break; }
        }
    }
}
