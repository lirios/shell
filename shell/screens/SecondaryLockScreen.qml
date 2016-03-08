/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

import QtQuick 2.4
import QtGraphicalEffects 1.0
import Hawaii.Components 1.0 as Components
import Fluid.Ui 1.0 as FluidUi
import Fluid.Effects 1.0 as FluidEffects

Components.Showable {
    id: root
    showAnimation: OpacityAnimator {
        target: root
        easing.type: Easing.InSine
        duration: FluidUi.Units.longDuration
        from: 0.0
        to: 1.0
    }
    hideAnimation: OpacityAnimator {
        target: root
        easing.type: Easing.OutSine
        duration: FluidUi.Units.longDuration
        from: 1.0
        to: 0.0
    }
    opacity: 0.0
    visible: true

    Image {
        id: picture
        anchors.fill: parent
        source: hawaiiCompositor.settings.lockScreen.pictureUrl
        sourceSize.width: width * 0.75
        sourceSize.height: height * 0.75
        fillMode: hawaiiCompositor.settings.convertFillMode(hawaiiCompositor.settings.lockScreen.fillMode)
        cache: false
        visible: false
    }

    FastBlur {
        id: blur
        anchors.fill: picture
        source: picture
        radius: 32
    }

    FluidEffects.Vignette {
        anchors.fill: parent
        source: blur
        radius: 8
        brightness: 0.2
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
    }
}
