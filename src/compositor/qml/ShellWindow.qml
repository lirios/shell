/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import QtGraphicalEffects 1.0
import WaylandCompositor 1.0
import GreenIsland 1.0
import Hawaii.Shell 0.1

WaylandSurfaceItem {
    id: surfaceItem
    onSurfaceChanged: {
        // Render the surface as soon as it is available
        renderer.source = surfaceItem;

        // Handle surface properties
        if (surfaceItem.surface) {
            surfaceProperties.target = surfaceItem.surface.windowProperties;
            surfaceItem.role = surfaceItem.surface.windowProperties.role;
            surfaceItem.restack();
        } else {
            surfaceProperties.target = null;
        }
    }

    property bool animationsEnabled: true
    property var role: null

    // Render item taking care of y inverted surfaces
    SurfaceRenderer {
        id: renderer
        anchors.fill: surfaceItem
    }

    // React to window properties changes
    Connections {
        id: surfaceProperties
        onRoleChanged: {
            surfaceItem.role = surfaceItem.surface.windowProperties.role;
            surfaceItem.restack();
        }
    }

    // Popup transform for map animation
    Translate {
        id: popupMapTransform
        y: 50
    }

    // Popup map animation
    ParallelAnimation {
        id: popupMapAnimation

        NumberAnimation {
            target: surfaceItem
            property: "opacity"
            easing.type: Easing.Linear
            to: 1.0
            duration: 250
        }

        SequentialAnimation {
            ScriptAction {
                script: surfaceItem.transform = popupMapTransform
            }

            NumberAnimation {
                target: popupMapTransform
                property: "y"
                easing.type: Easing.OutExpo
                to: 0
                duration: 250
            }

            ScriptAction {
                script: surfaceItem.transform = []
            }
        }
    }

    // Popup transform for unmap animation
    Translate {
        id: popupUnmapTransform
        y: 0
    }

    // Popup unmap animation
    ParallelAnimation {
        id: popupUnmapAnimation

        SequentialAnimation {
            ScriptAction {
                script: surfaceItem.transform = popupUnmapTransform
            }

            NumberAnimation {
                target: popupUnmapTransform
                property: "y"
                easing.type: Easing.OutExpo
                to: -50
                duration: 250
            }

            ScriptAction {
                script: surfaceItem.transform = []
            }
        }

        NumberAnimation {
            target: surfaceItem
            property: "opacity"
            easing.type: Easing.Linear
            to: 0.0
            duration: 250
        }
    }

    // Dialog transform for map animation
    Scale {
        id: dialogMapTransform
        origin.x: surfaceItem.width / 2
        origin.y: surfaceItem.height / 2
        xScale: 0.01
        yScale: 0.01
    }

    // Dialog map animation
    ParallelAnimation {
        id: dialogMapAnimation

        NumberAnimation {
            target: surfaceItem
            property: "opacity"
            easing.type: Easing.Linear
            to: 1.0
            duration: 250
        }

        SequentialAnimation {
            ScriptAction {
                script: surfaceItem.transform = dialogMapTransform
            }

            ParallelAnimation {
                NumberAnimation {
                    target: dialogMapTransform
                    property: "xScale"
                    easing.type: Easing.OutExpo
                    to: 1.0
                    duration: 350
                }

                NumberAnimation {
                    target: dialogMapTransform
                    property: "yScale"
                    easing.type: Easing.OutExpo
                    to: 1.0
                    duration: 350
                }
            }

            ScriptAction {
                script: surfaceItem.transform = []
            }
        }
    }

    // Dialog transform for unmap animation
    Scale {
        id: dialogUnmapTransform
        origin.x: surfaceItem.width / 2
        origin.y: surfaceItem.height / 2
        xScale: 1.0
        yScale: 1.0
    }

    // Dialog unmap animation
    ParallelAnimation {
        id: dialogUnmapAnimation

        SequentialAnimation {
            ScriptAction {
                script: surfaceItem.transform = dialogUnmapTransform
            }

            ParallelAnimation {
                NumberAnimation {
                    target: dialogUnmapTransform
                    property: "xScale"
                    easing.type: Easing.OutExpo
                    to: 0.01
                    duration: 350
                }

                NumberAnimation {
                    target: dialogUnmapTransform
                    property: "yScale"
                    easing.type: Easing.OutExpo
                    to: 0.01
                    duration: 350
                }
            }

            ScriptAction {
                script: surfaceItem.transform = []
            }
        }

        NumberAnimation {
            target: surfaceItem
            property: "opacity"
            easing.type: Easing.Linear
            to: 0.0
            duration: 250
        }
    }

    function restack() {
        if (surfaceItem.role == null)
            return;

        switch (surfaceItem.role) {
        case Compositor.BackgroundWindowRole:
            surfaceItem.z = 0;
            break;
        case Compositor.PanelWindowRole:
        case Compositor.LauncherWindowRole:
        case Compositor.SpecialWindowRole:
        case Compositor.PopupWindowRole:
            surfaceItem.z = 2;
            break;
        case Compositor.OverlayWindowRole:
            surfaceItem.z = 3;
            break;
        case Compositor.DialogWindowRole:
            surfaceItem.z = 999;
            break;
        default:
            break;
        }
    }

    function runMapAnimation () {
        if (surfaceItem.animationsEnabled) {
            switch (surfaceItem.role) {
            case Compositor.PopupWindowRole:
                popupMapAnimation.start();
                break;
            case Compositor.DialogWindowRole:
                dialogMapAnimation.start();
                break;
            default:
                break;
            }
        }
    }

    function runDestroyAnimation () {
        if (surfaceItem.animationsEnabled) {
            switch (surfaceItem.role) {
            case Compositor.PopupWindowRole:
                popupUnmapAnimation.start();
                break;
            case Compositor.DialogWindowRole:
                dialogUnmapAnimation.start();
                break;
            default:
                break;
            }
        }
    }
}
