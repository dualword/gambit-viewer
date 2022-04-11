// ***************************************************************************
// GReferenceView.h (c) 2009 Derek Barnett
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
// Visually displays all references loaded from files.
// ***************************************************************************

#ifndef G_REFERENCEVIEW_H
#define G_REFERENCEVIEW_H

#include <QGraphicsView>
#include "DataStructures/GReference.h"

namespace Gambit {
namespace Viewer {

class GReferenceView : public QGraphicsView {

    Q_OBJECT

    // constructors
    public:
        GReferenceView(QWidget* parent = 0);
        ~GReferenceView(void);

    // interface
    public slots:
        void Clear(void);
        void SetReferences(const GReferenceList& references);

    // signals
    signals:
        void ReferenceClicked(const GReference& ref);

    // data members
    private:
        struct GReferenceViewPrivate;
        GReferenceViewPrivate* d;
};

} // namespace Viewer
} // namespace Gambit

#endif // G_REFERENCEVIEW_H
