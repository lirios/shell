// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.impl 2.15
import Liri.WaylandClient 1.0 as WaylandClient
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects

Window {
    id: window

    default property alias content: content.data
    property alias contentWidth: page.contentWidth
    property alias contentHeight: page.contentHeight
    property alias focus: page.focus
    property alias keyboardInteractivity: layerSurface.keyboardInteractivity
    property alias padding: page.padding
    property alias leftPadding: page.leftPadding
    property alias topPadding: page.topPadding
    property alias rightPadding: page.rightPadding
    property alias bottomPadding: page.bottomPadding
    property alias standardButtons: buttonBox.standardButtons
    property alias footer: page.footer

    Material.elevation: 24

    flags: Qt.Dialog
    color: "transparent"
    visible: false

    width: page.width + (Material.elevation * 8)
    height: page.height + (Material.elevation * 8)

    onVisibleChanged: {
        if (visible)
            liriModal.grab();
        else
            liriModal.close();
    }

    WaylandClient.LiriModal {
        id: liriModal

        onGrabbedChanged: {
            if (!grabbed)
                window.hide();
        }
    }

    WaylandClient.WlrLayerSurfaceV1 {
        id: layerSurface

        layer: WaylandClient.WlrLayerSurfaceV1.OverlayLayer
        keyboardInteractivity: WaylandClient.WlrLayerSurfaceV1.ExclusiveKeyboardInteractivity
        role: "dialog"
    }

    Page {
        id: page

        anchors.centerIn: parent

        Material.theme: window.Material.theme
        Material.primary: window.Material.primary
        Material.accent: window.Material.accent

        implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                                contentWidth + leftPadding + rightPadding,
                                implicitHeaderWidth,
                                implicitFooterWidth)
        implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                                 contentHeight + topPadding + bottomPadding
                                 + (implicitHeaderHeight > 0 ? implicitHeaderHeight + spacing : 0)
                                 + (implicitFooterHeight > 0 ? implicitFooterHeight + spacing : 0))

        padding: 24
        topPadding: 20

        focus: true

        Keys.onEscapePressed: {
            dismiss();
        }

        contentItem: Item {
            id: content
        }

        background: Rectangle {
            anchors.fill: parent
            radius: 2
            color: window.Material.dialogColor

            layer.enabled: window.Material.elevation > 0
            layer.effect: FluidEffects.Elevation {
                elevation: window.Material.elevation
            }
        }

        header: FluidControls.DialogLabel {
            text: window.title
            visible: window.title
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            background: PaddedRectangle {
                radius: 2
                color: window.Material.dialogColor
                bottomPadding: -2
                clip: true
            }
        }

        footer: DialogButtonBox {
            id: buttonBox

            visible: count > 0

            onAccepted: {
                accept();
            }
            onDiscarded: {
                dismiss();
            }
        }
    }

    function standardButton(button) {
        return buttonBox.standardButton(button);
    }

    function accept() {
        hide();
    }

    function dismiss() {
        hide();
    }
}
