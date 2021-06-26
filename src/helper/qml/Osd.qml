// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Liri.WaylandClient 1.0 as WaylandClient

WaylandClient.LiriOsd {
    property Component osdComponent: Component {
        OsdWindow {}
    }
    property OsdWindow osdWindow: null

    onTextRequested: {
        if (osdWindow) {
            osdWindow.iconName = iconName;
            osdWindow.text = text;
            osdWindow.progressVisible = false;
        } else {
            osdWindow = osdComponent.createObject(this, {iconName: iconName, text: text, progressVisible: false});
        }
    }
    onProgressRequested: {
        if (osdWindow) {
            osdWindow.iconName = iconName;
            osdWindow.text = "";
            osdWindow.value = value;
            osdWindow.progressVisible = true;
        } else {
            osdWindow = osdComponent.createObject(this, {iconName: iconName, value: value, progressVisible: true});
        }
    }
}
