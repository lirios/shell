// SPDX-FileCopyrightText: 2019-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import Aurora.Compositor
import Aurora.Compositor.WlrLayerShell
import Fluid.Controls as FluidControls

WlrLayerSurfaceItem {
    id: layerSurfaceItem

    property bool createAnimationEnabled: true

    signal createAnimationFinished()
    signal destroyAnimationFinished()

    focusOnClick: layerSurface && layerSurface.keyboardInteractivity === WlrLayerSurfaceV1.ExclusiveKeyboardInteractivity
    opacity: 0.0
    //visible: layerSurface.mapped && layerSurface.configured

    onSurfaceDestroyed: {
        bufferLocked = true;
        if (layerSurface.nameSpace === "dialog")
            shrinkFadeOutAnimation.start();
        else
            destroyAnimationFinished();
    }

    Connections {
        target: shellSurface.surface

        function onHasContentChanged() {
            // Animate create the first time the surface is mapped
            if (shellSurface.surface.hasContent && createAnimationEnabled) {
                if (layerSurface.nameSpace === "dialog")
                    growFadeInAnimation.start();
                else {
                    opacity = 1.0;
                    createAnimationFinished();
                }
            }
        }
    }

    Behavior on x {
        NumberAnimation { duration: 220 }
    }

    Behavior on y {
        NumberAnimation { duration: 220 }
    }

    Behavior on width {
        NumberAnimation { duration: 220 }
    }

    Behavior on height {
        NumberAnimation { duration: 220 }
    }

    SequentialAnimation {
        id: growFadeInAnimation

        alwaysRunToEnd: true

        ParallelAnimation {
            NumberAnimation {
                target: layerSurfaceItem
                property: "scale"
                from: 0.9
                to: 1.0
                easing.type: Easing.OutQuint
                duration: 220
            }
            NumberAnimation {
                target: layerSurfaceItem
                property: "opacity"
                from: 0.0
                to: 1.0
                easing.type: Easing.OutCubic
                duration: 150
            }
        }

        ScriptAction {
            script: {
                // Never show the creation animation again
                createAnimationEnabled = false;

                createAnimationFinished();
            }
        }
    }

    SequentialAnimation {
        id: shrinkFadeOutAnimation

        alwaysRunToEnd: true

        ParallelAnimation {
            NumberAnimation {
                target: layerSurfaceItem
                property: "scale"
                from: 1.0
                to: 0.9
                easing.type: Easing.OutQuint
                duration: 220
            }
            NumberAnimation {
                target: layerSurfaceItem
                property: "opacity"
                from: 1.0
                to: 0.0
                easing.type: Easing.OutCubic
                duration: 150
            }
        }

        ScriptAction {
            script: {
                destroyAnimationFinished();
            }
        }
    }
}
