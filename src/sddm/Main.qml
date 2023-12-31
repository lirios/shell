/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2014-2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Fluid.Controls
import Fluid.Effects
import Liri.Shell

Rectangle {
    id: root
    width: 1024
    height: 768

    // Main color, remember to apply the same color to the Plymouth theme
    color: Material.color(Material.BlueGrey, Material.Shade800)

    Material.theme: Material.Dark
    Material.primary: Material.color(Material.Blue, Material.Shade500)
    Material.accent: Material.color(Material.Blue, Material.Shade500)

    // Make sure the pointer is an arrow
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: Qt.ArrowCursor
    }

    Component {
        id: bootSplashComponent

        Item {
            Image {
                // Plymouth theme spinner takes 16px, align vertically with it
                x: (root.width * 0.5) - (sourceSize.width * 0.5)
                y: (root.height * 0.5) - (sourceSize.height * 0.5) - 16
                source: Qt.resolvedUrl("images/logo.png")
            }
        }
    }

    Component {
        id: primaryComponent

        Item {
            Background {
                id: background
                anchors.fill: parent
                blur: true
                blurRadius: 16
                visible: !vignette.visible
            }

            Vignette {
                id: vignette
                anchors.fill: parent
                source: background
                radius: 4
                brightness: 0.4
                visible: background.mode === "wallpaper" && background.imageLoaded
            }

            Greeter {
                id: greeter

                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height

                primary: true
                onUserSelected: {
                    background.mode = user.backgroundMode
                    background.pictureUrl = user.pictureUrl
                    background.primaryColor = user.primaryColor
                    background.secondaryColor = user.secondaryColor
                    background.fillMode = user.fillMode
                }
                onLoginRequested: sddm.login(userName, password, sessionIndex)
                // XXX: This is not done because by the time loginSucceeded() is
                // emitted, SDDM already started the user session and closed the greeter
                onLoginSucceeded: stackView.replace(desktopStillComponent)
                onLoginFailed: errorBar.open(message)
            }

            InputPanel {
                id: inputPanel

                anchors.left: parent.left
                anchors.right: parent.right
            }

            SnackBar {
                id: errorBar
            }
        }
    }

    Component {
        id: desktopStillComponent

        DesktopStill {}
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: bootSplashComponent
        replaceEnter: Transition {
            YAnimator {
                from: height
                to: 0
                duration: Units.mediumDuration
            }
        }
        replaceExit: Transition {
            YAnimator {
                from: 0
                to: -height
                duration: Units.mediumDuration
            }
        }
    }

    Timer {
        interval: 1000
        running: true
        // TODO: SDDM doesn't handle primary screen very well in clone mode,
        // let's wait for the Wayland port which won't have clone mode at all
        //onTriggered: stackView.replace(primaryScreen ? primaryComponent : secondaryComponent)
        onTriggered: stackView.replace(primaryComponent)
    }
}
