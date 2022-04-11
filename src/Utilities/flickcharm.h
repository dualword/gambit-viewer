// ***************************************************************************
// flickcharm.h (c) 2009 Derek Barnett
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
// Provides flick-scrolling for QAbstractScrollArea-derived classes.
// Modified from Graphics Dojo project on Trolltech Labs (flickcharm.h/.cpp),
// distributed under GPL v2.0.
// Modified to fit Gambit style conventions, and inclusion in to Gambit
// namespaces. Also modified to enable mouse-tracking when not scrolling.
// ***************************************************************************

#ifndef FLICKCHARM_H
#define FLICKCHARM_H

#include <QObject>
#include <QPoint>
class QWidget;

namespace Gambit {

class FlickCharm: public QObject {

    Q_OBJECT

    // constructors & destructors
    public:
        FlickCharm(QObject* parent = 0);
        ~FlickCharm();

    signals:
        void MouseMoved(QPoint mousePos);

    // FlickCharm interface
    public:
        void activateOn(QWidget*);
        void deactivateFrom(QWidget*);
        bool eventFilter(QObject*, QEvent*);
    protected:
        void timerEvent(QTimerEvent*);

    // 'hidden' data implementation
    private:
        struct FlickCharmPrivate;
        FlickCharmPrivate* d;
};

} // namespace Gambit

#endif // FLICKCHARM_H
