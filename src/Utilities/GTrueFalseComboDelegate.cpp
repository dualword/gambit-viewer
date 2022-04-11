// ***************************************************************************
// GTrueFalseComboDelegate.cpp (c) 2009 Derek Barnett
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

#include <QtGui>
#include <QtDebug>
#include "Utilities/GTrueFalseComboDelegate.h"
using namespace Gambit;
using namespace Gambit::Utilities;

GTrueFalseComboDelegate::GTrueFalseComboDelegate(QObject* parent)
    : QItemDelegate(parent)
{ }

QWidget* GTrueFalseComboDelegate::createEditor(QWidget* parent,
                                               const QStyleOptionViewItem& option,
                                               const QModelIndex& index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QComboBox* comboBox = new QComboBox(parent);
    comboBox->addItem(tr("true"));
    comboBox->addItem(tr("false"));
    connect(comboBox, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
    return comboBox;
}

void GTrueFalseComboDelegate::setEditorData(QWidget* editor,
                                            const QModelIndex& index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) return;
    int pos = comboBox->findText(index.model()->data(index).toString(), Qt::MatchExactly);
    comboBox->setCurrentIndex(pos);
}

void GTrueFalseComboDelegate::setModelData(QWidget* editor,
                                           QAbstractItemModel* model,
                                           const QModelIndex& index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) return;
    model->setData(index, comboBox->currentText());
}

void GTrueFalseComboDelegate::emitCommitData(void) {
    emit commitData(qobject_cast<QWidget*>(sender()));
}
