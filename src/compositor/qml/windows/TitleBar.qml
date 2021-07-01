// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls

Rectangle {
    property alias iconName: icon.name
    property alias title: titleLabel.text
    property color backgroundColor: defaultBackgroundColor
    property color foregroundColor: defaultForegroundColor
    property bool activated: false
    property bool maximizable: false
    property bool maximized: false
    property alias dragTarget: dragHandler.target

    readonly property color defaultBackgroundColor: Material.color(Material.Blue)
    readonly property color defaultForegroundColor: Material.primaryTextColor

    signal activationRequested()
    signal minimizeClicked()
    signal maximizeClicked()
    signal closeClicked()
    signal windowMenuRequested(point position)

    Material.theme: Material.Dark

    color: chooseColor(backgroundColor, defaultBackgroundColor)

    function chooseColor(color, defaultColor) {
        return color.a > 0 ? color : defaultColor;
    }

    // Mouse click fall through with the new input handlers: if you overlap
    // the title bar of two windows and click on the one stacked above,
    // your click will hit the window below. Until this is sorted out,
    // we use the good old MouseArea
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: {
            if (mouse.button == Qt.LeftButton)
                titleBar.activationRequested();
            else if (mouse.button == Qt.RightButton)
                titleBar.windowMenuRequested(Qt.point(mouse.x, mouse.y));
        }
        onDoubleClicked: {
            titleBar.maximizeClicked();
        }
    }

    DragHandler {
        id: dragHandler

        property var moveBinding: Binding {
            target: dragHandler.target
            property: "moving"
            value: dragHandler.active
        }

        cursorShape: active ? Qt.ClosedHandCursor : Qt.ArrowCursor
        onCentroidChanged: {
            // Activate window when dragging it
            titleBar.activationRequested();
        }
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
                source: "qrc:/images/chrome/window-minimize.svg"
                color: chooseColor(foregroundColor, defaultForegroundColor)
                onClicked: {
                    titleBar.minimizeClicked();
                }
            }

            DecorationButton {
                source: "qrc:/images/chrome/" + (window.maximized ? "window-restore.svg" : "window-maximize.svg")
                color: chooseColor(foregroundColor, defaultForegroundColor)
                visible: maximizable
                onClicked: {
                    titleBar.activationRequested();
                    titleBar.maximizeClicked();
                }
            }

            DecorationButton {
                source: "qrc:/images/chrome/window-close.svg"
                color: chooseColor(foregroundColor, defaultForegroundColor)
                onClicked: {
                    titleBar.closeClicked();
                }
            }
        }
    }
}
