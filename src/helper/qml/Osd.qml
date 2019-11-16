/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
        } else {
            osdWindow = osdComponent.createObject(this, {iconName: iconName, text: text});
        }
    }
    onProgressRequested: {
        if (osdWindow) {
            osdWindow.iconName = iconName;
            osdWindow.text = "";
            osdWindow.value = value;
        } else {
            osdWindow = osdComponent.createObject(this, {iconName: iconName, value: value});
        }
    }
}