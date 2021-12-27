// SPDX-FileCopyrightText: 2019-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Aurora.Compositor 1.0
import Aurora.Compositor.WlrLayerShell 1.0
import Fluid.Controls 1.0 as FluidControls

WlrLayerSurfaceItem {
    id: layerSurfaceItem

    signal createAnimationFinished()
    signal destroyAnimationFinished()

    opacity: 0.0
    visible: layerSurface.mapped && layerSurface.configured

    onSurfaceDestroyed: {
        bufferLocked = true;
        if (layerSurface.nameSpace === "dialog")
            shrinkFadeOutAnimation.start();
        else if (layerSurface.nameSpace === "lockscreen")
            lockScreenExitAnimation.start();
        else
            destroyAnimationFinished();
    }

    onVisibleChanged: {
        if (visible) {
            if (layerSurface.nameSpace === "dialog")
                growFadeInAnimation.start();
            else if (layerSurface.nameSpace === "lockscreen")
                lockScreenEnterAnimation.start();
            else {
                opacity = 1.0;
                createAnimationFinished();
            }

            if (layerSurface.keyboardInteractivity > WlrLayerSurfaceV1.NoKeyboardInteractivity)
                takeFocus();
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

    SequentialAnimation {
        id: lockScreenEnterAnimation

        alwaysRunToEnd: true

        SequentialAnimation {
            PauseAnimation { duration: 80 }

            ParallelAnimation {
                NumberAnimation {
                    target: layerSurfaceItem
                    property: "opacity"
                    from: 0.0
                    to: 1.0
                    easing.type: Easing.Linear
                    duration: 153
                }
                NumberAnimation {
                    target: layerSurfaceItem
                    property: "y"
                    from: -height + (height * 1.05)
                    to: 0
                    easing.type: Easing.Bezier
                    easing.bezierCurve: [0.4,0,0.2,1,1,1]
                    duration: 153
                }
            }
        }

        ScriptAction {
            script: {
                createAnimationFinished();
            }
        }
    }

    SequentialAnimation {
        id: lockScreenExitAnimation

        alwaysRunToEnd: true

        ParallelAnimation {
            NumberAnimation {
                target: layerSurfaceItem
                property: "scale"
                from: 1.0
                to: 1.1
                easing.type: Easing.InQuint
                duration: FluidControls.Units.shortDuration
            }
            NumberAnimation {
                target: layerSurfaceItem
                property: "opacity"
                from: 1.0
                to: 0.0
                easing.type: Easing.InQuad
                duration: FluidControls.Units.shortDuration
            }
        }

        ScriptAction {
            script: {
                layerSurface.close();
                destroyAnimationFinished();
            }
        }
    }
}
