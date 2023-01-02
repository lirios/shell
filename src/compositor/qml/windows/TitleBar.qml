// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls
import Liri.Shell.Compositor 1.0 as LS

LS.AbstractTitleBar {
    property alias iconName: icon.name
    property alias title: titleLabel.text
    property color backgroundColor: defaultBackgroundColor
    property color foregroundColor: defaultForegroundColor
    property bool activated: false
    property bool maximizable: false
    property bool maximized: false

    readonly property color defaultBackgroundColor: Material.color(Material.Blue)
    readonly property color defaultForegroundColor: Material.primaryTextColor

    Material.theme: Material.Dark

    color: chooseColor(backgroundColor, defaultBackgroundColor)

    function chooseColor(color, defaultColor) {
        return color.a > 0 ? color : defaultColor;
    }

    Item {
        anchors.fill: parent
        opacity: activated ? 1 : 0.5

        FluidControls.Icon {
            id: icon

            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.verticalCenter: parent.verticalCenter

            width: 24
            height: width
            visible: name != "" && status == Image.Ready
        }

        Label {
            id: titleLabel

            anchors.left: icon.visible ? icon.right : parent.left
            anchors.right: windowControls.left
            anchors.verticalCenter: parent.verticalCenter

            font.bold: true
            font.pixelSize: 16

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.NoWrap
            color: chooseColor(foregroundColor, defaultForegroundColor)
        }

        Row {
            id: windowControls

            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.verticalCenter: parent.verticalCenter

            spacing: 12

            DecorationButton {
                icon.source: "qrc:/images/chrome/window-minimize.svg"
                icon.color: chooseColor(foregroundColor, defaultForegroundColor)
                onClicked: {
                    titleBar.minimizeClicked();
                }
            }

            DecorationButton {
                icon.source: "qrc:/images/chrome/" + (window.maximized ? "window-restore.svg" : "window-maximize.svg")
                icon.color: chooseColor(foregroundColor, defaultForegroundColor)
                visible: maximizable
                onClicked: {
                    titleBar.activationRequested();
                    titleBar.maximizeClicked();
                }
            }

            DecorationButton {
                icon.source: "qrc:/images/chrome/window-close.svg"
                icon.color: chooseColor(foregroundColor, defaultForegroundColor)
                onClicked: {
                    titleBar.closeClicked();
                }
            }
        }
    }
}
