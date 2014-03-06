/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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

import QtQuick 2.0

Item {
    property url sourceUrl

    id: background
    onSourceUrlChanged: {
        animation.running = false;

        if (__priv.currentLoader == loader1) {
            __priv.currentLoader = loader2;
            __priv.nextLoader = loader1;
        } else {
            __priv.currentLoader = loader1;
            __priv.nextLoader = loader2;
        }

        __priv.currentLoader.source = sourceUrl;
        __priv.currentLoader.opacity = 0;
        __priv.currentLoader.z = 1;
        __priv.nextLoader.z = 0;

        animation.running = true;
    }

    QtObject {
        id: __priv

        property Loader currentLoader: loader1
        property Loader nextLoader: loader2
    }

    SequentialAnimation {
        id: animation
        running: false

        NumberAnimation {
            target: __priv.currentLoader
            properties: "opacity"
            from: 0.0
            to: 1.0
            duration: 400
            easing.type: Easing.OutQuad
        }

        ScriptAction {
            script: {
                __priv.nextLoader.opacity = 0;
                //__priv.nextLoader.source = "";
            }
        }
    }

    Loader {
        id: loader1
        anchors.fill: parent
        z: 1
    }

    Loader {
        id: loader2
        anchors.fill: parent
        z: 0
    }
}
