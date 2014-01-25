/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import Hawaii.Shell.Settings 1.0

QtObject {
    /*! Internal name */
    property string shell: "org.hawaii.shells.tablet"

    /*! Path of this shell handler (set by the shell manager) */
    //property string path: "package://shell/org.hawaii.shells.tablet"
    property string path: ""

    /*! Returns \a true if current hardware setup demands this handler */
    property bool willing: settings.handler === shell

    /*! Handler priority */
    property int priority: 1

    /*! Set to \c true when this handler is loaded */
    property bool loaded: false

    default property var children

    ShellSettings {
        id: settings
    }
}
