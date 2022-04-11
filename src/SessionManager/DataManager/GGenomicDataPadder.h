// ***************************************************************************
// GGenomicDataPadder.h (c) 2009 Derek Barnett
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

#ifndef G_GENOMICDATAPADDER_H
#define G_GENOMICDATAPADDER_H

#include <QObject>

namespace Gambit {

class GGenomicDataSet;

class GGenomicDataPadder : public QObject {

    Q_OBJECT

    // constructor
    public:
        GGenomicDataPadder(QObject* parent = 0) : QObject(parent) { }

    // tool interface
    public slots:
        void Exec(GGenomicDataSet& data);

    // internal methods
    private:
        void CalculatePadding(GGenomicDataSet& data);
        void MergePadding(GGenomicDataSet& data);
        void ApplyPaddingToAlignments(GGenomicDataSet& data);
        void ApplyPaddingToGenes(GGenomicDataSet& data);
        void ApplyPaddingToSequence(GGenomicDataSet& data);
        void ApplyPaddingToSnps(GGenomicDataSet& data);
};

} // namespace Gambit

#endif // G_GENOMICDATAPADDER_H
