// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQml.Models 2.2
import Fluid.Controls 1.0
import Liri.Shell 1.0
import Liri.private.shell 1.0 as P
import "../indicators"
import "../launcher"

Row {
    id: indicatorsView

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
