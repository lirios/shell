/****************************************************************************
**
** Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

Item {
    id: container

    property variant child: null
    property variant chrome: null
    property bool unresponsive: false
    property bool animationsEnabled: true

    Behavior on x {
        enabled: container.animationsEnabled
        NumberAnimation { easing.type: Easing.InCubic; duration: 200 }
    }

    Behavior on y {
        enabled: container.animationsEnabled
        NumberAnimation { easing.type: Easing.InQuad; duration: 200 }
    }

    Behavior on width {
        enabled: container.animationsEnabled
        NumberAnimation { easing.type: Easing.InCubic; duration: 200 }
    }

    Behavior on height {
        enabled: container.animationsEnabled
        NumberAnimation { easing.type: Easing.InCubic; duration: 200 }
    }

    Behavior on scale {
        enabled: container.animationsEnabled
        NumberAnimation { easing.type: Easing.InQuad; duration: 200 }
    }

    Behavior on opacity {
        enabled: true
        NumberAnimation { easing.type: Easing.Linear; duration: 250 }
    }

    // Decrease opacity for unfocused windows
    ContrastEffect {
        id: effect
        anchors.fill: child
        source: child
        opacity: 1.0
        z: 1

        Behavior on blend {
            enabled: true
            NumberAnimation { easing.type: Easing.Linear; duration: 250 }
        }
    }

    // Unresponsive effect
    Colorize {
        id: unresponsiveEffect
        anchors.fill: effect
        source: effect
        hue: 0.0
        saturation: 0.5
        lightness: -0.2
        opacity: 0.0
        z: 1

        Behavior on opacity {
            NumberAnimation { easing.type: Easing.Linear; duration: 250 }
        }
    }

    states: [
        State {
            name: "selected"
            when: child && child.focus && !unresponsive
            PropertyChanges { target: effect; blend: 0.0 }
        },
        State {
            name: "unselected"
            when: child && !child.focus && !unresponsive
            PropertyChanges { target: effect; blend: 1.0 }
        },
        State {
            name: "unresponsive"
            when: child && unresponsive
            PropertyChanges { target: unresponsiveEffect; opacity: 1.0 }
        }
    ]
}
