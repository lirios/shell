// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import Qt5Compat.GraphicalEffects
import Fluid.Core
import Fluid.Controls as FluidControls

Loader {
    id: background

    property real pictureWidth: width
    property real pictureHeight: height
    property string mode
    property url pictureUrl
    property string primaryColor
    property string secondaryColor
    property string fillMode
    property bool blur: false
    property real blurRadius: 32
    readonly property bool loaded: __private.loaded
    readonly property bool imageLoaded: __private.imageLoaded

    states: [
        State {
            name: "solid"
            when: background.mode === "solid"
            PropertyChanges { background.sourceComponent: solid }
        },
        State {
            name: "gradient"
            when: background.mode === "hgradient" || background.mode === "vgradient"
            PropertyChanges { background.sourceComponent: gradient }
        },
        State {
            name: "wallpaper"
            when: background.mode === "wallpaper"
            PropertyChanges { background.sourceComponent: wallpaper }
        }
    ]

    QtObject {
        id: __private

        property bool loaded: false
        property bool imageLoaded: false
    }

    Component {
        id: solid

        Rectangle {
            objectName: "solid"
            color: background.primaryColor

            Component.onCompleted: {
                __private.loaded = true;
            }

            Behavior on color {
                ColorAnimation {
                    easing.type: Easing.OutQuad
                    duration: FluidControls.Units.mediumDuration
                }
            }
        }
    }

    Component {
        id: gradient

        Rectangle {
            readonly property bool vertical: background.mode === "vgradient"

            objectName: "gradient"
            rotation: vertical ? 270 : 0
            scale: vertical ? 2 : 1
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: background.primaryColor

                    Behavior on color {
                        ColorAnimation {
                            easing.type: Easing.OutQuad
                            duration: FluidControls.Units.mediumDuration
                        }
                    }
                }
                GradientStop {
                    position: 1
                    color: background.secondaryColor

                    Behavior on color {
                        ColorAnimation {
                            easing.type: Easing.OutQuad
                            duration: FluidControls.Units.mediumDuration
                        }
                    }
                }
            }

            Component.onCompleted: {
                __private.loaded = true;
            }
        }
    }

    Component {
        id: wallpaper

        Item {
            objectName: "wallpaper"

            Rectangle {
                anchors.fill: parent
                color: background.primaryColor
                visible: picture.status !== Image.Loading

                Behavior on color {
                    ColorAnimation {
                        easing.type: Easing.OutQuad
                        duration: FluidControls.Units.mediumDuration
                    }
                }
            }

            FluidControls.SmoothFadeImage {
                readonly property real aspectRatio: width / height

                id: picture
                anchors.fill: parent
                source: background.pictureUrl
                // Source size cannot be set to item width and height, otherwise it
                // will interfere with animations that affect size therefore we
                // we use fixed values (pictureWidth and pictureHeight)
                sourceSize.width: background.pictureWidth
                sourceSize.height: background.pictureHeight
                fillMode: background.convertFillMode(background.fillMode)
                onStatusChanged: {
                    __private.loaded = picture.status === Image.Ready;
                    __private.imageLoaded = __private.loaded;
                }
                visible: !blur.visible
            }

            FastBlur {
                id: blur
                anchors.fill: parent
                source: picture
                radius: background.blurRadius
                visible: background.blur
            }
        }
    }

    function convertFillMode(fillMode) {
        switch (fillMode) {
        case "preserve-aspect-fit":
            return Image.PreserveAspectFit;
        case "preserve-aspect-crop":
            return Image.PreserveAspectCrop;
        case "tile":
            return Image.Tile;
        case "tile-vertically":
            return Image.TileVertically;
        case "tile-horizontally":
            return Image.TileHorizontally;
        case "pad":
            return Image.Pad;
        default:
            return Image.Stretch;
        }
    }
}
