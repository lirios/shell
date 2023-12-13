// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import Fluid.Controls as FluidControls

Rectangle {
    width: grid.implicitWidth + 16
    height: grid.implicitHeight + 16
    color: "#b2000000"

    GridLayout {
        id: grid
        columns: 2
        rowSpacing: 8
        columnSpacing: 8
        x: rowSpacing
        y: columnSpacing

        Text {
            text: "Name:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: Screen.name
            font.bold: true
            font.underline: true
            color: "white"
        }

        Text {
            text: "Primary:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: output.primary ? "Yes" : "No"
            font.bold: true
            color: output.primary ? "green" : "red"
        }

        Text {
            text: "Resolution:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: Screen.width + "x" + Screen.height
            color: "white"
        }

        Text {
            text: "Physical Size:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: output.physicalSize.width + "x" + output.physicalSize.height + " mm ("  + (output.physicalSize.width / 10).toFixed(2) + "x" + (output.physicalSize.height / 10).toFixed(2) + " cm)"
            color: "white"
        }

        Text {
            text: "Pixel Density:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: Screen.pixelDensity.toFixed(2) + " dots/mm (" + (Screen.pixelDensity * 25.4).toFixed(2) + " dots/inch)"
            color: "white"
        }

        Text {
            text: "Logical Pixel Density:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: Screen.logicalPixelDensity.toFixed(2) + " dots/mm (" + (Screen.logicalPixelDensity * 25.4).toFixed(2) + " dots/inch)"
            color: "white"
        }

        Text {
            text: "Grid Unit:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: FluidControls.Units.gu(1).toFixed(2)
            color: "white"
        }

        Text {
            text: "Device Pixel Ratio:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: Screen.devicePixelRatio.toFixed(2)
            color: "white"
        }

        Text {
            text: "Output Scale Factor:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: output.scaleFactor.toFixed(2)
            color: "white"
        }

        Text {
            text: "Available Virtual Desktop:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: Screen.desktopAvailableWidth + "x" + Screen.desktopAvailableHeight
            color: "white"
        }

        Text {
            text: "Orientation:"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: orientationToString(Screen.orientation) + " (" + Screen.orientation + ")"
            color: "white"
        }

        Text {
            text: "Primary Orientation"
            font.bold: true
            color: "white"

            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: orientationToString(Screen.primaryOrientation) + " (" + Screen.primaryOrientation + ")"
            color: "white"
        }
    }

    function orientationToString(o) {
        switch (o) {
        case Qt.PrimaryOrientation:
            return "Primary";
        case Qt.PortraitOrientation:
            return "Portrait";
        case Qt.LandscapeOrientation:
            return "Landscape";
        case Qt.InvertedPortraitOrientation:
            return "Inverted Portrait";
        case Qt.InvertedLandscapeOrientation:
            return "Inverted Landscape";
        }
        return "Unknown";
    }
}
