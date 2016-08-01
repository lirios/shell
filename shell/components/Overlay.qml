/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Hawaii.Themes 1.0 as Themes
import Fluid.Controls 1.0

Showable {
    //! How long the overlay will stay on screen
    property alias timeout: timer.interval

    //! Name of the icon to show (see XDG icon naming specification)
    property alias iconName: icon.name

    //! Overlay value, either text or number
    property var value

    //! Set to true if the value is a number used for progress,
    // otherwise the value will be assumed to be a string to
    // be used for text
    property alias showProgress: progress.visible

    id: root
    showAnimation: OpacityAnimator {
        target: rect
        easing.type: Easing.InSine
        duration: Units.mediumDuration
        from: 0.0
        to: 1.0
    }
    hideAnimation: OpacityAnimator {
        target: rect
        easing.type: Easing.InSine
        duration: Units.mediumDuration
        from: 1.0
        to: 0.0
    }
    width: height
    height: 256
    onIconNameChanged: if (timer.running) timer.restart()
    onValueChanged: if (timer.running) timer.restart()
    onShowProgressChanged: if (timer.running) timer.restart()

    Material.theme: Material.Dark

    // Hide automatically after the timeout
    Timer {
        id: timer
        running: root.visible
        interval: 2500
        onTriggered: hide()
    }

    Rectangle {
        id: rect
        anchors.fill: parent
        border.color: Themes.Theme.palette.rgba(Material.drawerBackgroundColor, 0.5)
        border.width: 1
        color: Themes.Theme.palette.rgba(Material.drawerBackgroundColor, 0.85)
        radius: 6
        antialiasing: true
        opacity: 0.0
        onOpacityChanged: {
            if (opacity == 0.0) {
                // Reset values to prevent fading from old values
                root.timeout = 2500;
                root.iconName = "";
                root.value = 0;
                root.showProgress = false;
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Units.largeSpacing
            spacing: Units.smallSpacing

            Icon {
                id: icon
                width: Units.iconSizes.enormous
                height: width
                color: Material.primaryTextColor

                Layout.alignment: Qt.AlignHCenter
            }

            ProgressBar {
                id: progress
                from: 0
                to: 100
                value: visible ? root.value : 0
                visible: false

                Layout.fillWidth: true
            }

            Label {
                text: progress.visible ? "" : (root.value ? root.value : "")
                visible: !progress.visible

                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
