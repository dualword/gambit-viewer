// ***************************************************************************
// GTrueFalseComboDelegate.h (c) 2009 Derek Barnett
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
// Provides a true/false drop down option for QAbstractItemView-derived
// classes.
// ***************************************************************************

#ifndef G_TRUEFALSECOMBODELEGATE_H
#define G_TRUEFALSECOMBODELEGATE_H

#include <QItemDelegate>

namespace Gambit {
namespace Utilities {

class GTrueFalseComboDelegate : public QItemDelegate {

     Q_OBJECT

     public:
         GTrueFalseComboDelegate(QObject* parent = 0);

         QWidget* createEditor(QWidget* parent,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const;
         void setEditorData(QWidget* editor,
                            const QModelIndex& index) const;
         void setModelData(QWidget* editor,
                           QAbstractItemModel* model,
                           const QModelIndex& index) const;

    private slots:
         void emitCommitData(void);
 };

} // namespace Utilities
} // namespace Gambit

#endif // G_TRUEFALSECOMBODELEGATE_H
