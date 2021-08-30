// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

Row {
    height: parent.height

    Repeater {
        model: indicatorsModel

        Loader {
            asynchronous: true
            sourceComponent: model.component
            height: parent.height
        }
    }

    ShutdownIndicator {}
}
