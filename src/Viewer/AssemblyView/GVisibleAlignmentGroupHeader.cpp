// ***************************************************************************
// GVisibleAlignmentGroupHeader.cpp (c) 2009 Derek Barnett
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
// Describes the header widget that accompanies a GVisibleAlignmentGroup.
// Header displays the group name, current color scheme for "normal" items,
// and provides collapse/expand functionality to the group.
// ***************************************************************************

#include <QtGui>
#include <QtDebug>
#include "Viewer/AssemblyView/GVisibleAlignmentGroupHeader.h"
#include "DataStructures/GColorScheme.h"
using namespace Gambit;
using namespace Gambit::Viewer;

namespace Gambit {
namespace Viewer {

class GVisibleAlignmentGroupSeparator : public QFrame {
    public:
        GVisibleAlignmentGroupSeparator(QWidget* parent = 0)
            : QFrame(parent)
        {
            setFrameStyle( QFrame::HLine | QFrame::Plain );
            setLineWidth(2);
            setMidLineWidth(1);
            setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

            QPalette p;
            p.setColor(QPalette::WindowText, Qt::white);
            setPalette(p);
        }
};

class GVisibleAlignmentGroupButton : public QPushButton {

    public:
        GVisibleAlignmentGroupButton(QWidget* parent = 0)
            : QPushButton(parent)
        {
            setAttribute(Qt::WA_TranslucentBackground);
            setAttribute(Qt::WA_NoSystemBackground);
        }

        void paintEvent(QPaintEvent* e) {

            Q_UNUSED(e);

            // center icon in buton region, save offset
            m_offset.setX((size().width()  - iconSize().width())/2);
            m_offset.setY((size().height() - iconSize().height())/2);

            // draw button icon
            QPainter painter(this);
            if ( isEnabled() ) {
                painter.drawPixmap(m_offset,icon().pixmap(iconSize(),QIcon::Normal));
            } else {
                painter.drawPixmap(m_offset,icon().pixmap(iconSize(),QIcon::Disabled));
            }
        }

        bool hitButton(const QPoint& pos) const {
            QImage image = icon().pixmap(iconSize(), QIcon::Normal).alphaChannel().toImage();
            QPoint iconPos = pos - m_offset;
            return (image.valid(iconPos));
        }

    private:
        QPoint m_offset;
};

} // namespace Viewer
} // namespace Gambit

struct GVisibleAlignmentGroupHeader::GVisibleAlignmentGroupHeaderPrivate {

    QString      readGroup;
    GColorScheme colorScheme;
    QLabel*      groupLabel;
    QLabel*      colorSwatch_Forward;
    QLabel*      colorSwatch_Reverse;
    QPushButton* expandButton;
    QPushButton* collapseButton;

    void Init(void);
    void UpdateSwatches(void);
};

void GVisibleAlignmentGroupHeader::GVisibleAlignmentGroupHeaderPrivate::Init(void) {

    // set read group to "Unknown" if empty string passed
    if ( readGroup.isEmpty() ) { readGroup = "Unknown"; }

    // set up header label
    QPalette labelPalette;
    labelPalette.setColor(QPalette::WindowText, Qt::white);

    QString labelText = QString("Read Group: %1").arg(readGroup);
    groupLabel = new QLabel(labelText);
    groupLabel->setAlignment(Qt::AlignCenter);
    groupLabel->setPalette(labelPalette);
    groupLabel->setAttribute(Qt::WA_TranslucentBackground);

    colorSwatch_Forward = new QLabel("+");
    colorSwatch_Forward->setAlignment(Qt::AlignCenter);
    colorSwatch_Forward->setFixedSize(20, 20);
    colorSwatch_Forward->setAutoFillBackground(true);

    colorSwatch_Reverse = new QLabel("-");
    colorSwatch_Reverse->setAlignment(Qt::AlignCenter);
    colorSwatch_Reverse->setFixedSize(20, 20);
    colorSwatch_Reverse->setAutoFillBackground(true);
    UpdateSwatches();

    // set up expand and collapse buttons
    QPixmap arrowPixmap(":/icons/play");
    QTransform transform;
    transform.rotate(90);

    expandButton = new Gambit::Viewer::GVisibleAlignmentGroupButton;
    expandButton->setIcon(arrowPixmap.transformed(transform));
    expandButton->setToolTip("Expand group");

    transform.rotate(180);
    collapseButton = new Gambit::Viewer::GVisibleAlignmentGroupButton;
    collapseButton->setIcon(arrowPixmap.transformed(transform));
    collapseButton->setToolTip("Collapse group");
}

void GVisibleAlignmentGroupHeader::GVisibleAlignmentGroupHeaderPrivate::UpdateSwatches(void) {

    // set up 'color swatches' to indicate orientation coloring
    QPalette forwardPalette;
    forwardPalette.setColor(QPalette::Window,     colorScheme.ForwardBackground);
    forwardPalette.setColor(QPalette::WindowText, colorScheme.NormalText);
    colorSwatch_Forward->setPalette(forwardPalette);

    QPalette reversePalette;
    reversePalette.setColor(QPalette::Window,     colorScheme.ReverseBackground);
    reversePalette.setColor(QPalette::WindowText, colorScheme.NormalText);
    colorSwatch_Reverse->setPalette(reversePalette);
}

GVisibleAlignmentGroupHeader::GVisibleAlignmentGroupHeader(const QString& readGroup,
                                                           const GColorScheme& colorScheme,
                                                           QWidget* parent)
    : QWidget(parent)
{
    d = new GVisibleAlignmentGroupHeaderPrivate;
    d->readGroup   = readGroup;
    d->colorScheme = colorScheme;
    d->Init();

    // set up layout
    QHBoxLayout* midLayout = new QHBoxLayout;
    midLayout->setAlignment(Qt::AlignVCenter);
    midLayout->setSpacing(2);
    midLayout->addWidget(d->groupLabel);
    midLayout->addWidget(d->colorSwatch_Forward);
    midLayout->addWidget(d->colorSwatch_Reverse);
    midLayout->addWidget(d->expandButton);
    midLayout->addWidget(d->collapseButton);
    setLayout(midLayout);

    // initialize state
    d->collapseButton->show();
    d->expandButton->hide();

    // set up attributes
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    // set up connections
    connect(d->expandButton,   SIGNAL(clicked()), this, SLOT(ExpandButtonClicked()));
    connect(d->collapseButton, SIGNAL(clicked()), this, SLOT(CollapseButtonClicked()));
}

GVisibleAlignmentGroupHeader::~GVisibleAlignmentGroupHeader(void) {
    delete d;
    d = 0;
}

void GVisibleAlignmentGroupHeader::CollapseButtonClicked(void) {
    emit CollapseRequested();
    d->collapseButton->hide();
    d->expandButton->show();
}

void GVisibleAlignmentGroupHeader::ExpandButtonClicked(void) {
    emit ExpandRequested();
    d->expandButton->hide();
    d->collapseButton->show();
}

void GVisibleAlignmentGroupHeader::SetReadGroup(const QString& readGroup) {

    // store read group text
    d->readGroup = readGroup;

    // update group label
    QString labelText = QString("Read Group: %1").arg(readGroup);
    d->groupLabel->setText(labelText);
}

void GVisibleAlignmentGroupHeader::SetColorScheme(const GColorScheme& colorScheme) {
    d->colorScheme = colorScheme;
    d->UpdateSwatches();
}
