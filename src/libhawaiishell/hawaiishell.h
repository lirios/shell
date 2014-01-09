/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef HAWAIISHELL_H
#define HAWAIISHELL_H

#include <QtCore/QObject>

#include <HawaiiShell/Export>

namespace Hawaii {

namespace Shell {

class HAWAIISHELL_EXPORT Types : public QObject
{
    Q_OBJECT
public:
    ~Types();

    /*!
        Describes the form factor of an element.
    */
    enum FormFactor {
        /*! Element is on a plane. */
        Plane = 0,
        /*! Horizontal element. */
        Horizontal,
        /*! Vertical element. */
        Vertical
    };
    Q_ENUMS(FormFactor)

    /*!
        Describes where on the screen an element is positioned.
    */
    enum Location {
        /*! Free floating. */
        Floating = 0,
        /*! On the desktop layer, extending across the full screen
         *  from edge to edge.
         */
        Desktop,
        /*! Full screen. */
        FullScreen,
        /*! Along the top of the screen. */
        TopEdge,
        /*! Along the bottom of the screen. */
        BottomEdge,
        /*! Along the left side of the screen. */
        LeftEdge,
        /*! Along the right side of the screen. */
        RightEdge
    };
    Q_ENUMS(Location)

private:
    Types(QObject *parent = 0);
};

} // Shell namespace

} // Hawaii namespace

#endif // HAWAIISHELL_H
