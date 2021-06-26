// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15

BorderlessWindow {
    property bool configured: false
    property bool registered: false

    onConfiguredChanged: {
        if (configured && !registered) {
            refCount--;
            registered = true;
        }
    }

    Component.onCompleted: {
        refCount++;
    }
}
