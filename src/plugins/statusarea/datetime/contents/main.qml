// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Liri.Shell 1.0 as Shell

Shell.StatusAreaExtension {
    indicator: Shell.Indicator {
        property var now: new Date()

        text: Qt.formatTime(now)

        Timer {
            interval: 1000
            repeat: true
            running: true
            onTriggered: {
                now = new Date();
            }
        }
    }
}
