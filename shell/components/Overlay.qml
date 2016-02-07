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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes

Components.Showable {
    //! How long the overlay will stay on screen
    property alias timeout: timer.interval

    //! Name of the icon to show (see XDG icon naming specification)
    property alias iconName: icon.iconName

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
        duration: Themes.Units.mediumDuration
        from: 0.0
        to: 1.0
    }
    hideAnimation: OpacityAnimator {
        target: rect
        easing.type: Easing.InSine
        duration: Themes.Units.mediumDuration
        from: 1.0
        to: 0.0
    }
    width: height
    height: Themes.Units.gu(15)
    onIconNameChanged: if (timer.running) timer.restart()
    onValueChanged: if (timer.running) timer.restart()
    onShowProgressChanged: if (timer.running) timer.restart()

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
        border.color: Themes.Theme.palette.rgba(Themes.Theme.palette.panel.backgroundColor, 0.5)
        border.width: Themes.Units.dp(1)
        color: Themes.Theme.palette.rgba(Themes.Theme.palette.panel.backgroundColor, 0.85)
        radius: Themes.Units.dp(6)
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
            anchors.margins: Themes.Units.largeSpacing
            spacing: Themes.Units.smallSpacing

            Components.Icon {
                id: icon
                width: Themes.Units.iconSizes.enormous
                height: width
                color: Themes.Theme.palette.panel.textColor

                Layout.alignment: Qt.AlignHCenter
            }

            ProgressBar {
                id: progress
                minimumValue: 0
                maximumValue: 100
                value: visible ? root.value : 0
                visible: false

                Layout.fillWidth: true
            }

            Text {
                renderType: Text.NativeRendering
                text: progress.visible ? "" : (root.value ? root.value : "")
                visible: !progress.visible

                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
