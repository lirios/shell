/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

Item {
    property var window: parent

    id: root
    anchors.fill: parent

    MouseArea {
        anchors.fill: parent
        enabled: !window.focus
        preventStealing: true
        acceptedButtons: Qt.AllButtons
        onClicked: {
            // Change stacking order
            var container = window.parent;
            var i, windowsList = container.parent.children;
            //for (i = windowsList.length - 1; i >= 0; i--) {
            for (i = 0; i < windowsList.length; i++) {
                // Process only application windows
                if (windowsList[i].objectName !== "clientWindow")
                    continue;

                // Asign a z-index
                if (windowsList[i] !== container)
                    windowsList[i].z = i;
            }
            container.z = windowsList.length;
            container.child.takeFocus();

            // Pass the ball to the surface item input handler
            // FIXME: Doesn't work that way
            mouse.accepted = false;
        }
    }
}
