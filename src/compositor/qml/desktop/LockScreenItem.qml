// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Aurora.Compositor 1.0
import Fluid.Controls 1.0 as FluidControls

ShellSurfaceItem {
    id: lockScreenItem

    signal createAnimationFinished()
    signal destroyAnimationFinished()

    opacity: 0.0

    onSurfaceDestroyed: {
        bufferLocked = true;
        lockScreenExitAnimation.start();
    }

    Connections {
        target: shellSurface && shellSurface.surface ? shellSurface.surface : null

        function onHasContentChanged() {
            // Animate create the first time the surface is mapped
            if (shellSurface && shellSurface.surface.hasContent && lockScreenEnterAnimation.enabled)
                lockScreenEnterAnimation.start();
        }
    }

    SequentialAnimation {
        id: lockScreenEnterAnimation

        property bool enabled: true

        alwaysRunToEnd: true

        onFinished: {
            lockScreenEnterAnimation.enabled = false;
            createAnimationFinished();
        }

        PauseAnimation { duration: 80 }

        ParallelAnimation {
            OpacityAnimator {
                target: lockScreenItem
                from: 0.0
                to: 1.0
                easing.type: Easing.Linear
                duration: 153
            }

            YAnimator {
                target: lockScreenItem
                from: -height + (height * 1.05)
                to: 0
                easing.type: Easing.Bezier
                easing.bezierCurve: [0.4,0,0.2,1,1,1]
                duration: 153
            }
        }
    }

    ParallelAnimation {
        id: lockScreenExitAnimation

        alwaysRunToEnd: true

        onFinished: {
            destroyAnimationFinished();
        }

        ScaleAnimator {
            target: lockScreenItem
            from: 1.0
            to: 1.1
            easing.type: Easing.InQuint
            duration: 153
        }

        OpacityAnimator {
            target: lockScreenItem
            from: 1.0
            to: 0.0
            easing.type: Easing.InQuad
            duration: 153
        }
    }
}
