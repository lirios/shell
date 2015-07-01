/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
