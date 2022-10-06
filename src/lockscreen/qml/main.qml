// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQml 2.15
import QtQuick 2.15

Item {
    /*
     * UI
     */

    Instantiator {
        model: Qt.application.screens

        LockScreenWindow {
            screen: modelData
        }
    }
}
