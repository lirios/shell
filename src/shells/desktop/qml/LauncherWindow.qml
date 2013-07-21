/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import QtQuick.Window 2.0

Window {
    property alias size: launcher.size
    property alias alignment: launcher.alignment

    color: "transparent"
    x: {
        switch (launcher.alignment) {
        case LauncherAlignment.Left:
        case LauncherAlignment.Bottom:
            return 0;
        default:
            return Screen.width - launcher.size;
        }
    }
    y: {
        switch (launcher.alignment) {
        case LauncherAlignment.Left:
        case LauncherAlignment.Right:
            return 24;
        default:
            return Screen.height - launcher.size;
        }
    }
    width: {
        switch (launcher.alignment) {
        case LauncherAlignment.Left:
        case LauncherAlignment.Right:
            return launcher.size;
        default:
            return Screen.width;
        }
    }
    height: {
        switch (launcher.alignment) {
        case LauncherAlignment.Left:
        case LauncherAlignment.Left:
            return Screen.height - 24;
        default:
            return launcher.size;
        }
    }

    Launcher {
        id: launcher
        anchors.fill: parent
    }
}
